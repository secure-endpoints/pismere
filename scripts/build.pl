#!perl -w

use strict;
use FindBin;
use File::Spec;
use File::Basename;
use lib $FindBin::Bin;
use Logger;
use Getopt::Long;

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
    my $prefix = ($ENV{OS} && ($ENV{OS} eq 'Windows_NT'))?'':'perl ';
    return $prefix.File::Spec->catfile($DIR_SCRIPTS, @_);
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
	       );

    if ($OPT->{help} || !$OPT->{logfile}) {
	usage();
	exit(0);
    }

    if ($OPT->{docs}) {
	system("start ".File::Spec->catfile($DIR_DOC, 'Makefile.html'));
	exit(0);
    }

    $ENV{BINPLACE_CMD} = makescript('binplace.pl');
    $ENV{WHICH_CMD} = makescript('which.pl');
    $ENV{INCLUDE} = $DIR_ATHENA_INC.';'.$ENV{INCLUDE};
    $ENV{INCLUDE} = $DIR_INC.';'.$ENV{INCLUDE};
    if (-d $DIR_SITE) {
	$ENV{INCLUDE} = $DIR_SITE.';'.$ENV{INCLUDE};
    }
    $ENV{PISMERE} = $DIR_TOP;
    $ENV{USERNAME} = $ENV{USERNAME}?$ENV{USERNAME}:'*Unknown*';
    {
	my ($name,$aliases,$addrtype,$length,@addrs) = gethostbyname('');
	$ENV{HOSTNAME} = ($addrs[0])?gethostbyaddr($addrs[0], $addrtype):
	  '*Unknown*';
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
    my $l;
    if (!$OPT->{nolog}) {
	$l = new Logger $OPT->{logfile};
	$l->start;
    }
    unshift(@ARGV, '/nologo');
    print "$0 invoking $MAKE with the following options:\n";
    map { print "\t$_\n" } @ARGV;
    print "\n";
    system($MAKE, @ARGV);
    if (!$OPT->{nolog}) {
	$l->stop;
    }
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
  Other:
    $MAKE-options       any options you want to pass to $MAKE, which can be:
                        (note: /nologo is always used)

USAGE
    system("$MAKE /?");
}

main();
