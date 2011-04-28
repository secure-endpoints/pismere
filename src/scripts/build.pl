#!perl -w

use strict;
use FindBin;
use File::Spec;
use File::Basename;
use lib "$FindBin::Bin/build/lib";
use Logger;
use Getopt::Long;
use Cwd;

my $BAIL;

$0 = fileparse($0);

my $OPT = { logfile => 'build.pl.log' };
my $DIR_TOP = File::Spec->catfile($FindBin::Bin, '..');


my $DIR_ATHENA = File::Spec->catfile($DIR_TOP, 'athena');
my $DIR_ATHENA_INC = File::Spec->catfile($DIR_TOP, 'athena', 'include');
my $DIR_SCRIPTS = File::Spec->catfile($DIR_TOP, 'scripts');
my $DIR_INC = File::Spec->catfile($DIR_SCRIPTS, 'build', 'inc');
my $DIR_DOC = File::Spec->catfile($DIR_SCRIPTS, 'build', 'doc');
my $DIR_SITE = File::Spec->catfile($DIR_SCRIPTS, 'build', 'site');

my $DIR_BUILD = $DIR_ATHENA;

my $MAKE='NMAKE';

sub makescript
{
    #
    # NOTE: We must invoke perl directly instead of using file associations
    # so as to prevent the shell from doing weird things when we CTRL-C.
    #
    return $^X.' '.File::Spec->catfile($DIR_SCRIPTS, @_);
}

sub pismere_dirify
{
    my $dir = shift;
    my $i = index($dir, $DIR_TOP);
    if ($i == 0) {
	substr($dir, $i, length($DIR_TOP), '$(PISMERE)');
    }
    return $dir;
}

sub main
{
    Getopt::Long::Configure('no_bundling', 'no_auto_abbrev',
			    'no_getopt_compat', 'require_order',
			    'ignore_case', 'pass_through',
			    'prefix_pattern=(--|-|\+|\/)',
			   );
    GetOptions($OPT,
	       'help|h|?',
	       'docs',
	       'top',
	       'logfile|l:s',
	       'nolog|n',
	       'softdirs',
	       'norecurse',
	       'oldstyle|o',
	       'tools',
	       'toolsonly',
	       );

    if ($OPT->{help} || !$OPT->{logfile}) {
	usage();
	exit(0);
    }

    if ($OPT->{docs}) {
	my $norm = File::Spec->catfile($DIR_DOC, 'Makefile.html');
	my $site = File::Spec->catfile($DIR_SITE, 'Makefile.html');
	if (-e $site) {
	    system("start $site");
	} elsif (-e $norm) {
	    system("start $norm");
	} else {
	    die "Could not find documentation\n";
	}
	exit(0);
    }

    $ENV{BUILD_MAKEFILE_INC} = File::Spec->catfile($DIR_INC, 'Makefile.inc');
    $ENV{BINPLACE_CMD} = makescript('binplace.pl');
    $ENV{WHICH_CMD} = makescript('which.pl');
    
    $ENV{BUILD_INCLUDE} = join(';',
			       pismere_dirify($DIR_ATHENA_INC),
			       (-d $DIR_SITE) ? pismere_dirify($DIR_SITE) :(),
			       pismere_dirify($DIR_INC));
    $ENV{PISMERE} = $DIR_TOP;
    $ENV{USERNAME} = $ENV{USERNAME}?$ENV{USERNAME}:'*Unknown*';
    {
	my ($name,$aliases,$addrtype,$length,@addrs) = gethostbyname('');
#	$ENV{HOSTNAME} = ($addrs[0])?gethostbyaddr($addrs[0], $addrtype):
#	  '*Unknown*';
        $ENV{HOSTNAME} = ($name) ? ($name) : '*Unknown*';

    }

    use Time::gmtime;
    $ENV{DATE} = gmctime()." GMT";

    {
	my $LOCAL_OPTS = 'build_local_opts.pm';
	my $LOCAL_OPTS_PATHNAME =  File::Spec->catfile($DIR_SITE, $LOCAL_OPTS);
	if (-e $LOCAL_OPTS_PATHNAME) {
	    print "Using $LOCAL_OPTS\n";
	    do $LOCAL_OPTS_PATHNAME;
	}
    }

    if ($OPT->{top} && !chdir($DIR_BUILD)) {
	die "Could not chdir to $DIR_BUILD\n";
    }

    if ($OPT->{toolsonly}) {
	print_tools();
	exit(0);
    }

    my $l;
    if (!$OPT->{nolog}) {
	$l = new Logger $OPT->{logfile};
	$l->start;
    }

    print_tools() if $OPT->{tools};

    unshift(@ARGV, '/nologo');
    print "$0 invoking $MAKE with the following options:\n";
    map { print "\t$_\n" } @ARGV;
    print "\n";
    my $err = 0;
    if ($OPT->{oldstyle}) {
	$ENV{BUILD_USING_OLDSTYLE_BUILD} = 1;
	$err = system($MAKE, @ARGV) / 256;
    } else {
	$err = do_dir(\&build_dir, '.');
    }
    if ($BAIL) {
	print $BAIL;
    }
    if (!$OPT->{nolog}) {
	$l->stop;
    }
    return $err;
}

sub read_file
{
    my $file = shift;
    my $fh = new IO::File;
    $fh->open("<$file");
    my @lines = <$fh>;
    $fh->close();
    @lines;
}

sub parse_dirs_lines
{
    my @lines = @_;
    my $info = {};
    while (my $line = shift @lines) {
	chomp($line);

	$line =~ s/^#.*//;     # remove any line starting with #
	$line =~ s/[^\^]#.*//; # remove anything after an unescaped #

	if (($line =~ /^(.*)\\\s*$/) && !($line =~ /^(.*)\^\\\s*$/)) {
	    my $n = shift @lines;
	    $line = $1 . $n;
	    unshift(@lines, $line);
	    next;
	}

	$line =~ s/\^#/#/g;    # replace all ^# with #

	if ($line =~ /^\s*$/) {
	    next;
	} elsif ($line =~ /^\s*([^=\s]+)\s*=(.*)$/) {
	    $info->{$1} = $2;
	} else {
	    print "ERROR: $line\n";
	    return 0;
	}
    }
    foreach my $key (keys %$info) {
	if (($key eq 'DIRS') || ($key eq 'PREDIRS')) {
	    $info->{$key} =  [ split(' ', $info->{$key}) ];
	} elsif ($key eq 'OPTDIRS') {
	    my @list = split(' ', $info->{$key});
	    $info->{$key} = {};
	    map { $info->{$key}->{$_} = 1; } @list;
	} else {
	    print "ERROR: Invalid key found: $key = $info->{$key}\n";
	    return 0;
	}
    }
    if ($info->{DIRS} || $info->{PREDIRS}) {
	return $info;
    }
    print "ERROR: Missing DIRS/PREDIRS directives in Makefile.dir\n";
    return 0;
}

sub do_dirs
{
    my $routine = shift;
    my $basedir = shift;
    my $dirs = shift;
    my $key = shift;

    if ($dirs->{$key}) {
	foreach my $dir (@{$dirs->{$key}}) {
	    my $short_dir = $dir;
	    $dir = File::Spec->catfile($basedir, $short_dir);
	    if (-d $dir) {
		my $err = 0;
		if (-e File::Spec->catfile($dir, 'Makefile.dir') ||
		    -e File::Spec->catfile($dir, 'Makefile.src') ||
		    -e File::Spec->catfile($dir, 'Makefile')) {
		    $err = do_dir($routine, $dir);
		} elsif ($OPT->{softdirs}) {
		    print "SKIPPING DIR MISSING Makefiles (softdirs enabled): $dir\n";
		} else {
		    print "ERROR: Cannot find Makefile.dir, Makefile.src, or Makefile in $dir!\n";
		    $err = 2;
		}
		return $err if $err;
	    } elsif ($OPT->{softdirs}) {
		print "SKIPPING MISSING DIR (softdirs enabled): $dir\n";
	    } elsif ($dirs->{OPTDIRS}->{$short_dir}) {
		print "SKIPPING MISSING OPTIONAL DIR: $dir\n";
	    } else {
		print "ERROR -- COULD NOT FIND DIR: $dir\n";
		return 2; # XXX
	    }
	}
    }
    return 0;
}

sub build_dir
{
    my $dir = shift;
    my $curdir = cwd() || die "Cannot get cwd\n";
    print "\nBuilding $dir\n";
    chdir($dir) || die "Could not chdir to $dir\n";
    my $err = 0;
    if (-e 'Makefile.src') {
	$err = system($MAKE, ('-f', $ENV{BUILD_MAKEFILE_INC}, @ARGV)) / 256;
    } elsif (-e 'Makefile') {
	$err = system($MAKE, @ARGV) / 256;
    } else {
	print "ERROR: Cannot find Makefile.src or Makefile\n";
	$err = 2;
    }
    chdir($curdir) || die "Could not chdir to $curdir\n";
    return $err;
}

sub do_dir
{
    my $routine = shift;
    my $dir = shift;
    my $dirs = {};
    my $Makefile_dir = File::Spec->catfile($dir, 'Makefile.dir');
    if (!$OPT->{norecurse} && -e $Makefile_dir) {
	$dirs = parse_dirs_lines(read_file($Makefile_dir));
	return 0 if !$dirs;
    }
    print "Entering $dir\n";
    if (!(-e File::Spec->catfile($dir, 'Makefile.dir') ||
	  -e File::Spec->catfile($dir, 'Makefile.src') ||
	  -e File::Spec->catfile($dir, 'Makefile'))) {
	print "ERROR: Cannot find Makefile.dir, Makefile.src, or Makefile in $dir!\n";
	return 2;
    }
    my $err = 0;
    $err = do_dirs($routine, $dir, $dirs, 'PREDIRS');
    return $err if $err;
    if (-e File::Spec->catfile($dir, 'Makefile.src') ||
	-e File::Spec->catfile($dir, 'Makefile')) {
	$err = &$routine($dir);
	return $err if $err;
    }
    $err = do_dirs($routine, $dir, $dirs, 'DIRS');
    return $err if $err;
    print "Exiting $dir\n\n";
    return 0;
}

sub print_tools
{
    sub get_info
    {
	my $cmd = shift || die;
	my $which = makescript('which.pl');
	my $full = `$which $cmd`;
	return 0 if ($? / 256);
	chomp($full);
	$full = "\"".$full."\"";
	my $ver = `filever $full`;
	chomp($ver);
	return { cmd => $cmd, full => $full, ver => $ver };
    }
    sub get_info_alt
    {
	my $cmd = shift || die;
	my $which = makescript('which.pl');
	my $full = `$which $cmd`;
	return 0 if ($? / 256);
	chomp($full);
	$full = "\"".$full."\"";
	my $ver = `$full --version`;
	chomp($ver);
	return { cmd => $cmd, full => $full, ver => $ver };
    }
    my $V =
      {
       COMPILER => get_info('cl.exe'),
       LINKER => get_info('link.exe'),
       LIB => get_info('lib.exe'),
       NMAKE => get_info( 'nmake.exe'),
       PERL => get_info('perl.exe'),
       sed => get_info_alt('sed.exe'),
       sed => get_info('sed.exe'),
       awk => get_info_alt('gawk.exe'),
       awk => get_info('gawk.exe'),
       MessageCompiler => get_info('mc.exe'),
       ResourceCompiler => get_info('rc.exe'),
      };
    my $PATH = $ENV{PATH} || '';
    my $INCLUDE = $ENV{INCLUDE} || '';
    my $LIB = $ENV{LIB} || '';

    print <<DATA;

PATH=$PATH

INCLUDE=$INCLUDE
LIB=$LIB

DATA

    foreach my $k ('COMPILER', 'LINKER', 'LIB', 'NMAKE', 'PERL', 'sed', 'awk', 'MessageCompiler', 'ResourceCompiler') {
	if ($V->{$k}) {
	    print "$k: $V->{$k}->{full}\n$V->{$k}->{ver}\n";
	} else {
	    print "$k: NOT FOUND\n";
	}
    }

    print "\n";
}

sub usage
{
    print <<USAGE;
Usage: $0 [options] $MAKE-options

  Options:
    --help, -h, -?      usage information (what you now see)
    --docs              build documentation
    --top               build from top-level dir (default is current dir)
    --logfile filename  log to specified filename (default: build.pl.log)
       or -l filename
    --nolog, -n         do not log
    --softdirs          do not bail on missing directories
    --norecurse         do not recurse
    --oldstyle, -o      use old style
    --tools             show tools
    --toolsonly         show tools and exit
  Other:
    $MAKE-options       any options you want to pass to $MAKE, which can be:
                        (note: /nologo is always used)

USAGE
    system("$MAKE /?");
}

sub handler {
    my $sig = shift;
    my $bailmsg = "Bailing out due to SIG$sig!\n";
    my $warnmsg = <<EOH;
*********************************
* FUTURE BUILDS MAY FAIL UNLESS *
* BUILD DIRECTORIES ARE CLEANED *
*********************************
EOH
    $BAIL = $bailmsg.$warnmsg;
}

$SIG{'INT'} = \&handler;
$SIG{'QUIT'} = \&handler;

exit(main());
