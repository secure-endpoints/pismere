#!perl -w

# XXX - Fix INCLUDES search so that we use implicit . only for "foo.h"
# and not for <foo.h>

# XXX - convert all \ into / ... (DONE?)

# XXX - process .rc file directives: CURSOR, ICON, BITMAP, RCDATA

# Use CL /E to process #include directives...as though building
# Also use CL /E to handle .rc files

use strict;
use FindBin;
use File::Spec;
use File::Basename;
use Getopt::Long;
use Cwd;
use IO::File;

$0 = fileparse($0);

my $OPT = { outfile => 'Makefile.dep' };

my @DIR_STACK = ();

sub my_chdir
{
    my $dir = shift || die;
    return chdir($dir) || die "Could not change directory to $dir\n";
}

sub my_cwd
{
    return cwd() || die "Could not get current directory\n";
}

sub pushd
{
    my $dir = shift || die;
    push(@DIR_STACK, my_cwd());
    return my_chdir($dir);
}

sub popd
{
    my_chdir(pop(@DIR_STACK));
}

sub read_source_file
{
    my $info = shift || die;
    my $ginfo = shift || die;
    my $file = shift || die;
    my $source = shift || die;
    my $orig = shift || die;

    return if $info->{done}->{$file};

    my $dir = $info->{dir} || die;

    my $fh = new IO::File;
    $fh->open("<$file") || die "Cannot open file $file\n";
    $info->{done}->{$file} = 1;
    while (!$fh->eof()) {
	my $line = $fh->getline();
	if ($line =~ /^\s*\#\s*include\s*[\"\<]([^\"\>]+)[\"\>]/) {
	    my $inc = $1;
	    $inc =~ s/\//\\/;
	    my $found = find_include($info, $ginfo, $inc, $source);
	    if ($found) {
		if (!$info->{dep_h}->{lc($orig)}->{lc($inc)}) {
		    $info->{dep_h}->{lc($orig)}->{lc($inc)} = $found;
		    push(@{$info->{dep_l}->{lc($orig)}}, lc($inc));;
		}
	    }
	}
    }
    $fh->close();
}

sub find_include
{
    my $info = shift || die;
    my $ginfo = shift || die;
    my $inc = shift || die;
    my $source = shift || die;
    my $dir = $info->{dir} || die;
    my $retval = 0;
    my $prefound = $info->{cache}->{$inc};
    my $found = ($prefound ||
		 find_include_internal($info, $ginfo, $inc, $source,
				       @{$info->{inc}}) ||
		 find_include_internal($info, $ginfo, $inc, $source,
				       @{$ginfo->{inc}->{build}}) ||
		 find_include_internal($info, $ginfo, $inc, 0,
				       @{$ginfo->{inc}->{env}}));
    if ($found && $source && !$info->{hide}->{$inc}) {
	$retval = $found;
    }
    if (!$prefound && $found && !$info->{hide}->{$inc}) {
	print "FOUND: $inc at $found\n" if $OPT->{verbose};
    } elsif (!$found && !$info->{warned}->{$inc}) {
	$info->{warned}->{$inc} = 1;
	print "WARNING: Could not find $inc\n" unless $OPT->{quiet};
    }
    return $retval;
}

sub find_include_internal
{
    my $info = shift || die;
    my $ginfo = shift || die;
    my $inc = shift || die;
    my $source = shift;
    my @dirs = @_;

    my $found = $info->{cache}->{$inc};
    if (!$found) {
	foreach my $dir (@dirs) {
	    my $file = "$dir\\$inc";
	    if (-f xlat($ginfo->{pismere}, $file)) {
		$found = $info->{cache}->{$inc} = $file;
		$info->{hide}->{$inc} = 1 if !$source;
		last;
	    }
	}
    }
    if ($found && !$info->{hide}->{$inc}) {
	read_source_file($info, $ginfo, xlat($ginfo->{pismere},
					     $found), $source, $inc);
    }
    return $found;
}

sub xlat
{
    my $pismere = shift;
    my $dir = shift || die;
    my $PISMERE = '$(PISMERE)';
    my $i = index($dir, $PISMERE);
    if ($i == 0) {
	substr($dir, $i, length($PISMERE), $pismere);
    }
    return $dir;
}

sub get_build_info
{
    my $info = shift || die;
    my $ginfo = shift || die;

    my $dir = $info->{dir} || die;

    if (!(-e File::Spec->catfile($dir, 'Makefile.src'))) {
	die "Could not find Makefile.src in $dir\n";
    }

    my $BUILD = File::Spec->catfile($FindBin::Bin, 'build.pl');
    pushd($dir);
    my @lines = `$^X $BUILD -n --norecurse /P /N`;
    popd();
    my $vars = parse_lines(@lines);

    add_from_vars($info->{inc},
		  $vars, 'INCLUDES', '\s*;\s*');
    if (!@{$ginfo->{inc}->{env}}) {
	add_from_vars($ginfo->{inc}->{env},
		      $vars, 'ENV_INCLUDE', '\s*;\s*');
    }
    if (!@{$ginfo->{inc}->{build}}) {
	add_from_vars($ginfo->{inc}->{build},
		      $vars, 'BUILD_INCLUDE', '\s*;\s*');
	fix_binc($ginfo->{inc}->{build}, $ginfo->{inc}->{env});
    }
    $ginfo->{pismere} = ($ginfo->{pismere} ||
			 add_from_vars(0, $vars, 'PISMERE'));
    add_from_vars($info->{objs}, $vars, 'OBJS', '\s+');
    add_from_vars($info->{objs}, $vars, 'RES', '\s+');
}

sub parse_lines
{
    my @lines = @_;
    my $res = {};
    map {
	chomp($_);
	if (/^\s*(.*\S)\s*=\s*(.+\S)\s*$/) {
	    $res->{$1} = $2;
	}
    } @lines;
    $res;
}

sub add_from_vars
{
    my $list = shift;
    my $vars = shift || die;
    my $var = shift || die;
    my $split = shift;
    if ($vars->{$var}) {
	my $varcopy = $vars->{$var};
	$varcopy =~ s/\//\\/;
	if ($list) {
	    die if !$split;
	    push(@$list, split(/$split/, $varcopy));
	    @$list = grep(!/^\s*$/, @$list);
	    return $list;
	} else {
	    return $varcopy;
	}
    }
    return 0;
}

sub read_file
{
    my $file = shift || die;
    my $fh = new IO::File;
    $fh->open("<$file") || die "Could not open file $file\n";
    my @lines = <$fh>;
    map { chomp($_); } @lines;
    $fh->close();
    @lines;
}

sub get_excludes
{
    my $WHICH = File::Spec->catfile($FindBin::Bin, 'which.pl');
    my @lists = (`$^X $WHICH sysincl.dat`, `$^X $WHICH msvcincl.dat`);
    my @files = ();
    map { chomp($_); push(@files, read_file($_)) if (-e $_); } @lists;
    @files;
}

sub fix_binc
{
    my $binc = shift || die;
    my $envinc = shift || die;

    my @binc = @$binc;
    my @envinc = @$envinc;

    my $b = pop(@binc);
    my $env = pop(@envinc);
    while ($env && $b && ($env eq $b)) {
	$b = pop(@binc);
	$env = pop(@envinc);
    }
    if ($b) {
	push(@binc, $b);
    }
    @$binc = @binc;
}

sub usage
{
    my $code = shift || 0;
    print <<USAGE;
Usage: $0 [options] [DIR]
    -h, --help         help
    -v, --verbose      verbose output
    -q, --quiet        quiet output
    -f, --force        replace output file if it exists
    -s, --site         include site-specific (*/site/*) dependencies
    -o FILE,           specify an output file (default is Makefile.dep)
     --outfile FILE    (If FILE lacks dir components, FILE is put in DIR.
                        Otherwise, FILE is relative to current directory.)

    DIR                directory to process (default is .)
USAGE
    exit($code);
}

sub main
{
    Getopt::Long::Configure('bundling');
    GetOptions($OPT,
	       'help|h|?',
	       'verbose|v',
	       'quiet|q',
	       'force|f',
	       'outfile|o:s',
	       'site|s',
	       ) || usage(1);

    usage() if ($OPT->{help});

    my $dir = shift @ARGV || '.';

    if (!($OPT->{outfile} eq '-') && !($OPT->{outfile} =~ /[\\\/]/)) {
	$OPT->{outfile} = "$dir\\$OPT->{outfile}";
    }

    check_file();

    my $ginfo =
      {
       inc =>
       {
	build => [],
	env => [ split(/;/, $ENV{INCLUDE}) ],
       },
      };

    my $info =
      {
       dir => $dir,
       inc => ['.'],
       objs => [],
       dep => {},
       cache => {},
      };

    get_build_info($info, $ginfo);

    if ($OPT->{verbose}) {
	print "INCLUDES is:\n";
	map {print "\t[$_]\n"; } @{$info->{inc}};
	print "BUILD INCLUDE is:\n";
	map {print "\t[$_]\n"; } @{$ginfo->{inc}->{build}};
	print "ENVIRONMENT INCLUDE is:\n";
	map {print "\t[$_]\n"; } @{$ginfo->{inc}->{env}};
	print "OBJS is:\n";
	map {print "\t[$_]\n"; } @{$info->{objs}};
    }

    if (!@{$info->{objs}}) {
	print "No source files to process\n";
	exit(0);
    }

    pushd($dir);
    map {
	my $match;
	my $file = (search_source(\$match, '.', $_, 'obj',
				  'cp', 'cxx', 'cpp', 'c') ||
		    search_source(\$match, '.', $_, 'res',
				  'rc'));
	if (!$file && !$match) {
	    die "ERROR: Unexpected file specification $_\n";
	}
	if ($file) {
	    push(@{$info->{sources}}, $file);
	    read_source_file($info, $ginfo, $file, $file, $file);
	}
    } @{$info->{objs}};
    popd();

    process_deps($info);
    print_file($info);
}

sub check_file
{
    my $file = $OPT->{outfile};
    die "$file already exists\n" if (-e $file && !$OPT->{force} &&
				     !($file eq '-'));
    $file;
}

sub process_deps_internal
{
    my $info = shift || die;
    my $source = shift || die;
    my $done = shift || die;
    my $file = shift || die;
    my $inc = shift || die;

    if (!$done->{$_}) {
	$done->{$_} = 1;
	my $found = $info->{dep_h}->{lc($file)}->{lc($inc)};
	if ($found) {
	    push(@{$info->{dep}->{$source}}, $found);
	    process_deps_recurse($info, $source, $done, $inc);
	}
    }
}

sub process_deps_recurse
{
    my $info = shift || die;
    my $source = shift || die;
    my $done = shift || die;
    my $inc = shift || die;

    #print "rec_it: $source, $inc\n";
    map {
	process_deps_internal($info, $source, $done, $inc, $_);
    } @{$info->{dep_l}->{lc($inc)}};
}

sub process_deps
{
    my $info = shift || die;

    map {
	my $source = $_;
	#print "SOURCE: $source\n";
	my $done;
	$done->{$source} = 1;
	map {
	    #print "GOT: $_\n";
	    process_deps_internal($info, $source, $done, $source, $_);
	} @{$info->{dep_l}->{lc($source)}};
    } @{$info->{sources}};
}

sub print_file
{
    my $info = shift || die;
    check_file();
    my $file = $OPT->{outfile};
    my $fh = new IO::File;
    if ($file eq '-') {
	$fh->open(">&STDOUT") || die "Cannot open STDOUT\n";
    } else {
	print "Using file: $file\n";
	$fh->open(">$file") || die "Cannot open $file\n";
    }

    map {
	# XXX - a hack:
	my %printed;
	print $fh "$_: \\\n";
	map {
	    if ($OPT->{site} || !/[\\\/]site[\\\/]/) {
		print $fh "\t\"$_\"\\\n" if !$printed{lc($_)};
		$printed{lc($_)} = 1;
	    }
	} sort insensitive @{$info->{dep}->{$_}};
	print $fh "\n";
    } sort insensitive keys %{$info->{dep}};
    $fh->close();
}

sub insensitive
{
    lc($a) cmp lc($b) || $a cmp $b;
}

sub search_source
{
    my $match = shift || die;
    my $dir = shift || die;
    my $obj = shift || die;
    my $obj_ext = shift || die;
    my @src_exts = @_;

    my $file;
    $$match = 0;
    if ($obj =~ /^\$\(O\)[\\\/](.+)\.$obj_ext/i) {
	$$match = 1;
	$file = find_right_ext($dir, $1, @src_exts);
	if (!$file) {
	    print "WARNING: Could not find source file for $obj\n";
	}
    }
    return $file;
}

sub find_right_ext
{
    my $dir = shift || die;
    my $base = shift || die;
    my @exts = @_;
    foreach my $ext (@exts) {
	my $file = "$base.$ext";
	return $file if (-e File::Spec->catfile($dir, $file));
    }
    return 0;
}

main();
