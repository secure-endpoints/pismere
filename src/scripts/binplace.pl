#!perl -w

use strict;
use FindBin;
use File::Spec;
use File::Copy;
use File::Path;
use File::Basename;
use File::stat;

$0 = fileparse($0);

use Getopt::Long;

my $OPT = {};
my $DEBUG_DIR;
my $CPU;
my $TOPDIR = File::Spec->catfile($FindBin::Bin, '..');
my $DIRBASE = File::Spec->catfile($TOPDIR, 'target');
my $MAP;
my $EXTRA;
my @FILES;

sub set_cpu
{
    my @VALID_CPUS = ( 'i386', 'alpha' );
    $CPU = $OPT->{cpu} || lc($ENV{CPU});
    if (!$CPU) {
	$CPU = lc($ENV{PROCESSOR_ARCHITECTURE});
	$CPU = 'i386' if ($CPU eq 'x86');
	$CPU = 'i386' if !$CPU;
    }
    foreach my $c (@VALID_CPUS) {
	return if ($CPU eq $c);
    }
    die "Cannot figure out the CPU\n";
}

sub set_debug_dir
{
    if ($OPT->{debug} && $OPT->{nodebug}) {
	die "Cannot mix debug and nodebug options\n";
    }
    if (!$OPT->{debug} && !$OPT->{nodebug}) {
	die "Must specify one of debug or nodebug\n";
    }
    $DEBUG_DIR = $OPT->{nodebug}?'rel':'dbg';
}

sub set_map
{
    my $DIR =
      {
       BIN => File::Spec->catfile($DIRBASE, 'bin', $CPU, $DEBUG_DIR),
       LIB => File::Spec->catfile($DIRBASE, 'lib', $CPU, $DEBUG_DIR),
       INC => File::Spec->catfile($DIRBASE, 'inc'),
      };
    $MAP =
      {
       exe => $DIR->{BIN},
       dll => $DIR->{BIN},
       cpl => $DIR->{BIN},
       scr => $DIR->{BIN},
       hlp => $DIR->{BIN},
       cnt => $DIR->{BIN},
       lib => $DIR->{LIB},
       h   => $DIR->{INC},
       sys => $DIR->{BIN},
       inf => $DIR->{BIN},
       msc => $DIR->{BIN},
       htm => $DIR->{BIN},
       html => $DIR->{BIN},
       conf => $DIR->{BIN},
       cmd => $DIR->{BIN},
       pl => $DIR->{BIN},
      };
    $EXTRA =
      {
       exe => ['pdb', 'bsc'],
       scr => ['pdb', 'bsc'],
       dll => ['pdb', 'bsc', 'lib'],
       cpl => ['pdb', 'bsc', 'lib'],
       lib => ['pdb', 'bsc'],
       sys => ['pdb', 'bsc'],
      }
}

sub get_extras
{
    my $file = shift;
    foreach my $ext (keys %$EXTRA) {
	if ($file =~ /^(.+\.)$ext$/i) {
	    my @extras;
	    map { push(@extras, $1.$_); } @{$EXTRA->{$ext}};
	    return @extras;
	}
    }
    return ();
}

sub get_by_ext
{
    my $file = shift;
    foreach my $ext (keys %$MAP) {
	if ($file =~ /^.+\.$ext$/i) {
	    return $MAP->{$ext};
	}
    }
    return 0;
}

sub init
{
    set_cpu();
    set_debug_dir();
    set_map();
}

sub main
{
    Getopt::Long::Configure('bundling');
    if (!GetOptions($OPT,
		    "help|H|h|?",
		    "debug",
		    "nodebug",
		    "cpu=s",
		    "exclude|x=s@",
		    "optional",
		   )) {
	usage();
	die "\n";
    }
    my $file = shift @ARGV;
    if (!$file) {
	usage();
	die "\n";
    }
    unshift @ARGV, $file;
    if ($OPT->{help}) {
	usage();
	return;
    }
    init();
    @FILES = @ARGV;
    while ($file = shift @ARGV) {
	place($file);
    }
}

sub excluded
{
    my $file = shift;
    my ($name, $path) = fileparse($file);
    foreach my $x (@{$OPT->{exclude}}) {
	my ($xname, $xpath) = fileparse($x);
	return 1 if !$xpath && (lc($name) eq lc($x));
	return 1 if lc($file) eq lc($x);
    }
    return 0;
}

sub in_list
{
    my $element = shift;
    my $list = shift;
    foreach my $e (@$list) {
	return 1 if $e eq $element;
    }
    return 0;
}

sub place
{
    my $file = shift;
    my $dir_extras = shift;

    if (!-e $file) {
	if ($OPT->{optional}) {
	    print "WARNING: Cannot find $file\n" if in_list($file, \@FILES);
	} else {
	    die "ERROR: Cannot find $file\n" if in_list($file, \@FILES);
	}
	return;
    }
    return if (!-e $file);
    return if excluded($file);

    my $dir = get_by_ext($file) || $dir_extras;
    if (!$dir) {
	die "ERROR: No target dir for $file\n";
	return;
    }
    if (! -d $dir) {
	mkpath([ $dir ]);
    }
    if (! -d $dir) {
	die "ERROR: Could not create directory $dir\n";
	return;
    }
    return if !mycopy($file, $dir);
    if (!$dir_extras) {
	my @extras = get_extras($file);
#	print "EXTRAS: ", join(', ', @extras), "\n";
	foreach my $extra (@extras) {
	    place($extra, $dir);
	}
    }
}

sub mycopy
{
    my $file = shift;
    my $dir = shift;

    my ($name, $path) = fileparse($file);
    my $outfile = File::Spec->catfile($dir, $name);

    my $pdir = $dir;
    my $i = length($TOPDIR);
    if (lc(substr($pdir, 0, $i)) eq lc($TOPDIR)) {
	my $c = substr($pdir, $i, 1);
	if ($c eq '/' || $c eq '\\') {
	    $pdir = substr($pdir, $i + 1);
	}
    }

    my $stin = -e $file ? stat($file) : 0;
    my $stout = -e $outfile ? stat($outfile) : 0;
    if ($stin && $stout) {
	if ($stout->mtime > $stin->mtime) {
	    print "WARNING: $name in $pdir is newer...SKIPPED\n";
	    return 1;
	}
	if ($stout->mtime == $stin->mtime) {
	    print "NOCHANGE: $name in $pdir is up-to-date...SKIPPED\n";
	    return 1;
	}
    }
    undef $stout;

    my $rc = copy($file, $outfile);
    if ($rc) {
	utime($stin->atime, $stin->mtime, $outfile);
	print "COPIED: $name to $pdir\n";
    } else {
	die "ERROR: Could not copy $name to $pdir\n";
    }
    return $rc;
}

sub usage
{
    print <<USAGE;
Usage: $0 [options] file(s)

  Options:
    -h, -H, --help     help
    --debug            files are debug files
    --nodebug          files are release files
    --excludep=file    exclude pathname
    --exclude=file     exclude basename (w/extension)
    --cpu=cputype      override CPU setting
    --optional         do not bomb out if file does not exist
USAGE
}

main();
