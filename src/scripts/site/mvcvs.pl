#!perl -w

use strict;
use File::Basename;
use Getopt::Long;
use IO::File;

$0 = fileparse($0);

sub tmp
{
    my $file = shift;
    return "$file.tmp";
}

sub bak
{
    my $file = shift;
    return "$file.bak";
}

sub uniq
{
    my %h;
    my @ret;
    foreach $_ (@_) {
	if (!$h{$_}) {
	    $h{$_} = 1;
	    push(@ret, $_);
	}
    }
    return @ret;
}

sub common
{
    my $a = shift;
    my $b = shift;

    my $k = length($a);
    my $l = length($b);
    while ($l) {
	my $s = substr($b, 0, $l);
	my $i = rindex($a, $s);
	return $s if (($i >= 0) && ($i + $l) == $k);
	$l--;
    }
    return 0;
}

sub find_tag
{
    my $root = shift;
    return $1 if $root =~ /:gserver:[^:]+:(.+)/i;
    return $1 if $root =~ /:kserver:[^:]+:(.+)/i;
    return $1 if $root =~ /:pserver:[^:]+:(.+)/i;
    return $1 if $root =~ /:local:(.+)/i;
    return 0;
}

sub open_or_die
{
    my $file = shift;
    my $output = shift;
    return (new IO::File $output?">$file":"<$file") || 
      die "Could not open $file for ".$output?"output":"input"."\n";
}

sub main
{
    Getopt::Long::Configure('bundling', 'no_auto_abbrev', 
			    'no_getopt_compat', 'require_order',
			    'ignore_case', 'pass_through',
			    'prefix_pattern=(--|-|\+|\/)',
			   );
    my $OPT = {};
    GetOptions($OPT,
	       'help|h|?',
	       'local|l',
	       'quiet|q',
	       'verbose|v',
	       'nochange|n',
	       'backup|b',
	       );

    my $root = shift @ARGV;
    if ($OPT->{help} || !$root) {
	usage();
	exit(0) if $OPT->{help};
	exit(1);
    }
    my $tag = find_tag($root) || die "Cannot understand CVS root: $root\n";
    print "Repository portion of CVS root is: $tag\n";

    my @dirs;
    if ($#ARGV < 0) {
	push(@dirs, '.');
    } else {
	@dirs = @ARGV;
    }
    @dirs = uniq(@dirs);
    my $STATE = {};
    while (my $dir = shift @dirs) {
	process($root, $tag, $dir, $STATE, $OPT);
    }
    exit(0);
}

sub process()
{
    my $root = shift;
    my $tag = shift;
    my $dir = shift;
    my $STATE = shift;
    my $OPT = shift;

    print "Processing: $dir\n" unless $OPT->{quiet};
  
    my $fdir = $dir.'/CVS';
    my $fent = $fdir.'/Entries';
    my $froot = $fdir.'/Root';
    my $frepo = $fdir.'/Repository';

    my $h;
    my $r;

    $h = open_or_die($froot);
    $r = <$h> || die "No data in $froot\n";
    undef $h;
    chomp($r);
    if (!$STATE->{old_root}) {
	$STATE->{old_root} = $r;
	print "Root: $STATE->{old_root} -> $root\n" if $OPT->{verbose};
    }
    ($r eq $STATE->{old_root}) || 
      die "Inconsitent data in $froot: got $r when expecting $STATE->{old_root}\n";

    $h = open_or_die($frepo);
    $r = <$h> || die "No data in $frepo\n";
    undef $h;
    chomp($r);

    if (!$STATE->{skip_repo_init})
    {
	$STATE->{skip_repo_init} = 1;
	$STATE->{skip_repo} = (($STATE->{old_root} =~ /^:local:/) && 
			       !common($STATE->{old_root}, $r));
    }

    if (!($STATE->{skip_repo})) {

	if (!$STATE->{old_tag}) {
	    $STATE->{old_tag} = common($STATE->{old_root}, $r) || 
	      die "Mismatch between $STATE->{old_root} and $r\n";
	}

	my $or = $r;
	!index($r, $STATE->{old_tag}) ||
	  die "Could not find $STATE->{old_tag} at start of $r\n";
	substr($r, 0, length($STATE->{old_tag}), $tag);

	print "Repo: $or -> $r\n" if $OPT->{verbose};
    }

    sub update_file
    {
	my $file = shift;
	my $data = shift;
	my $backup = shift;
	my $error = "Could not backup $file -- may be inconsistent --- get help!!!\n";
	my $h = open_or_die(tmp($file), 1);
	print $h $data;
	undef $h;
	if ($backup) {
	    rename($file, bak($file)) || die $error;
	}
	rename(tmp($file), $file) || die $error;
    }

    if (!$OPT->{nochange}) {
	update_file($froot, "$root\n", $OPT->{backup});
	update_file($frepo, "$r\n", $OPT->{backup}) if !$STATE->{skip_repo};
    }

    if (!$OPT->{local}) {
	$h = (new IO::File "<$fent" ) || die "Could not open $fent\n";
	while (<$h>) {
	    if (/^D\/([^\/]+)\//) {
		process($root, $tag, $dir.'/'.$1, $STATE, $OPT);
	    }
	}
	undef $h;
    }
}

sub usage
{
    print <<USAGE;
Usage: $0 [options] newroot [dir]
    command         find file executed by this command by looking at PATH
    -l, --local     local, don't recurse
    -q, --quiet     quiet
    -v, --verbose   verbose
    -n, --nochange  don't change anything on the disk
    -b, --backup    create backup files
    -?, -H, --help  help
USAGE
}

main();
