#!perl -w

use strict;
#use sigtrap qw(handler my_cleanup normal-signals);
use FindBin;
use File::Spec;
use File::Path;
use File::Basename;
use Time::localtime;
use lib $FindBin::Bin;
use mk;

$0 = fileparse($0);

use Getopt::Long;

my $OPT = {};
my $DEBUG_DIR;
my $CPU;
my $DIR_TOP = File::Spec->catfile($FindBin::Bin, '..', '..');
my $DIR_TARGET = File::Spec->catfile($DIR_TOP, 'target');

my $TMPDIR;

sub set_cpu
{
    my @VALID_CPUS = ( 'i386', 'alpha', 'alpha64', 'amd64', 'ia64');
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
    my $debug = shift;
    die if not defined $debug;
    $DEBUG_DIR = $debug?'dbg':'rel';
}

sub init
{
    my $type = shift || die usage();
    my $debug;

    if ($type eq 'dbg') {
	$debug = 1;
    } elsif ($type eq 'rel') {
	$debug = 0;
    } else {
	die "Invalid type: \"$type\".\nType must be \"rel\" or \"dbg\".\n";
    }
    set_cpu();
    set_debug_dir($debug);
    return $debug;
}

sub main
{
   Getopt::Long::Configure('bundling');
   if (!GetOptions($OPT,
		   "help|H|h|?",
		   "nopause|n",
		   "cpu=s",
		   "gtag=s%",
		   "official",
		  )) {
       die usage();
   }

   if ($OPT->{help}) {
       print usage();
       return;
   }

   my $op = shift @ARGV || die usage();
   my $next = shift @ARGV || die usage();

   if ($op eq 'help') {
       print usage();
       print "\n";
       my $mod = $next;
       my $mk_help = File::Spec->catfile($FindBin::Bin, "mk/$mod/mk.help");
       if (!$mod || ! -e $mk_help) {
	   die "No help info for $mod found!\n";
       } else {
	   do $mk_help || die;
	   print mk_help_do($mod);
       }
   } elsif (($op eq 'bin') || ($op eq 'sdk')) {
       my $mod = shift @ARGV || die usage();
       my $debug = init($next);
       # mk bin mod [zip-tag]
       mk_bin_sdk($op, $mod, $debug, @ARGV);
   } elsif ($op eq 'src') {
       my $mod = $next;
       # mk src mod tag [args]
       my $tag = shift @ARGV || die usage();
       my $zdir = shift @ARGV;
       mk_src($mod, $tag, $zdir, @ARGV);
   } elsif ($op eq 'build') {
       my $mod = shift @ARGV || die usage();
       my $debug = init($next);
       mk_build($mod, $debug, @ARGV);
   } else {
       die usage();
   }
}

sub mk_src
{
    my $mod = shift || die;
    my $tag = shift || die;
    my $zdir = shift;
    my @rest = @_;

    if (!$zdir) {
	my $date = mkdate(localtime);
	$zdir = "$mod-$date";
    }

    print "MODULE:  $mod\n";
    print "TAG:     $tag\n";
    print "ZIPBASE: $zdir\n";

    my_pause() if !$OPT->{nopause};

    my $tmp = mktmpdir() || die;
    $TMPDIR = $tmp;

    my $origdir = my_cwd();
    pushd($tmp);

    my $mk_src = File::Spec->catfile($FindBin::Bin, "mk/$mod/mk.src");
    do $mk_src || die;
    my $site = mk_src_do($OPT, $origdir, $tag, $zdir, @rest) || die;
    push(@$site, "$zdir/*/site");

    sub _j
    {
	my $prefix = shift || die;
	my $postfix = shift || die;
	my $list = shift || die;
	return @$list ? ($prefix.join("$postfix $prefix", @$list).$postfix)
	  : '';
    }

    sub _i
    {
	my $inc = shift;
	my $list = shift || die;
	my $postfix = shift || '/*';
	my $prefix = $inc ? ' -i ' : ' -x ';
	return _j($prefix, $postfix, $list);
    }

    sub _z
    {
	my $base = shift || die;
	my $zdir = shift;
	return "zip -rX $zdir-$base.zip $zdir";
    }

    my $cvs = [ '*/CVS' ];
    my_pause() if !$OPT->{nopause};
    # COMMON
    my_system(_z("src", $zdir).         _i(0, $site)._i(0, $cvs)) || die;
    my_system(_z("src-cvs", $zdir).     _i(0, $site)._i(1, $cvs)) || die;
    my_system(_z("src-site", $zdir).    _i(1, $site)._i(0, $cvs)) || die;
    my_system(_z("src-site-cvs", $zdir)._i(1, $site, '/CVS/*')._i(1, $site, '*/CVS/*')) || die;
    popd();

    my_move("$tmp/$zdir-src.zip", '.');
    my_move("$tmp/$zdir-src-cvs.zip", '.');
    my_move("$tmp/$zdir-src-site.zip", '.');
    my_move("$tmp/$zdir-src-site-cvs.zip", '.');

    my_pause() if !$OPT->{nopause};

  cleanup:
    print "Cleaning up...\n";
    rmtree($tmp) || die;
    print "Cleaned up\n";
}

sub mk_pdb
{
    my $tmp = shift || die;
    my $zdir = shift || die;
    my $ddir = shift || die;
    my $zip = shift || die;
    my $T = shift || die;
    my @keys = @_;

    my $pdb_exists;
    print "Copying PDB files...\n";
    foreach my $key (sort @keys) {
	my $dest = "$tmp/$zdir/$key";
	foreach my $file (@{$T->{$key}}) {
	    my ($base, $dir, $ext) = fileparse($file, '\.[^\.]*');
	    my $pdb = "$dir/$base.pdb";
	    if (-e $pdb) {
		$pdb_exists++;
		my_copy($pdb, "$dest");
	    }
	}
    }
    if ($pdb_exists) {
	print "Copied PDB files\n";
	pushd($tmp);
	my_system("zip -rX $zdir-symbols.zip $zdir -i *.pdb");
	popd();
	my_move("$tmp/$zdir-symbols.zip", $ddir);
    } else {
	print "NO PDB FILES FOUND\n!";
    }
}

sub mk_bin_sdk
{
    my $op = shift || die;
    my $mod = shift || die;
    my $debug = shift;
    my $zdir = shift;

    die if not defined $debug;

    die "invalid operation: $op\n" if  !(($op eq 'bin') || ($op eq 'sdk'));

    my $ddir = ".";

    if (!$zdir) {
	my $date = mkdate(localtime);
	$zdir = "$mod-$date";
    }

    use Safe;
    my $SAFE = new Safe;
    ${$SAFE->varglob('ENV')}{COMPUTERNAME} = $ENV{COMPUTERNAME};
    ${$SAFE->varglob('dbg')} = $DEBUG_DIR;
    ${$SAFE->varglob('cpu')} = $CPU;
    ${$SAFE->varglob('lib')} = "$DIR_TOP/target/lib/$CPU/$DEBUG_DIR";
    ${$SAFE->varglob('top')} = $DIR_TOP;
    ${$SAFE->varglob('bin')} = "$DIR_TOP/target/bin/$CPU/$DEBUG_DIR";
    my $M = $SAFE->rdo(File::Spec->catfile($FindBin::Bin, "mk/$mod/mk.conf"));
    die "Could not read configuration\n" if !$M;

    my $T = $M->{$mod}->{$op};

    my $tmp = mktmpdir() || die;
    $TMPDIR = $tmp;

    print "Copying files...\n";
    my @keys = keys %$T;
    foreach my $key (sort @keys) {
	my $dest = "$tmp/$zdir/$key";
	mkpath("$dest") || die;
	foreach my $file (@{$T->{$key}}) {
	    use File::DosGlob 'glob';
	    my @files = glob($file);
	    my $copied;
	    foreach my $f (@files) {
		next if -d $f;
		my_copy($f, "$dest");
		$copied++;
	    }
	    die "Could not expand: $file\n" if !$copied;
	}
    }
    print "Copied files\n";

    my $zip;
    my $sym_zip;

    if ($op eq 'bin') {
	$zip = "$zdir-bin-$DEBUG_DIR.zip";
	$sym_zip = "$zdir-bin-symbols.zip";
    } elsif ($op eq 'sdk') {
	$zip = "$zdir-sdk-$DEBUG_DIR.zip";
	$sym_zip = "$zdir-sdk-symbols.zip";
    } else {
	die "invalid operation: $op\n";
    }

    pushd($tmp);
    my_system("zip -rX $zip $zdir");
    popd();
    my_move("$tmp/$zip", $ddir);

    mk_pdb($tmp, $zdir, $ddir, $sym_zip, $T, @keys) if $debug;

    my_pause() if !$OPT->{nopause};

    print "Cleaning up...\n";
    rmtree($tmp) || die;
    print "Cleaned up\n";
}

sub mk_build
{
    my $mod = shift || die;
    my $debug = shift;

    die if not defined $debug;

    my $mk_build = File::Spec->catfile($FindBin::Bin, "mk/$mod/mk.build");
    do $mk_build || die;
    my $dir = mk_build_do($OPT, $DIR_TOP) || die;
    pushd($dir);
    my @args = ("-w", "$FindBin::Bin/../build.pl", "-l",
		'mkbuild.'.($debug?'dbg':'rel').'.log', @ARGV);
    push(@args, "NODEBUG=1") if !$debug;
    my_system($^X, @args);
    popd();
}

sub usage
{
    my $usage = <<USAGE;
Usage: $0 [options] operation operation-args

  Options:
    -h, -H, --help     help
    -n, --nopause      do not pause

    --cpu=cputype      override CPU setting (build/bin/sdk)
    --gtag gmodule=tag override graft tag for grafted "gmodule" (src)
    --official         do an "official" build (build)

  Operations:
    help  module
    src   module tag [zip-tag] [module-specific-args]
    build type module
    bin   type module [zip-tag]
    sdk   type module [zip-tag]

  types (build/bin/sdk):
    rel                release version (not debug)
    dbg                debug version

  modules:
USAGE
    use DirHandle;
    my $d = new DirHandle File::Spec->catfile($FindBin::Bin, 'mk');
    if (defined $d) {
        while (defined($_ = $d->read)) {
	    $usage .= <<DATA if not (($_ eq '.') || ($_ eq '..') || ($_ eq 'CVS'));
    $_
DATA
	}
        undef $d;
    }
    return $usage;
}

main();
