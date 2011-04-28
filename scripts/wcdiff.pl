#!perl -w

use strict;
use FindBin;

use File::Path;
use File::Spec;
use File::Copy;
use POSIX;
use Cwd;
use File::Find;
use File::Basename;
$0 = fileparse($0);

use sigtrap 'handler', \&my_handler;

sub my_handler
{
    my $sig = shift;
#    rmtree($tmp);
    print "CLEANUP Aborted: $sig\n";
    exit(2);
}

sub checksigs
{
    use Config;
    defined $Config{sig_name} || die "No sigs?";
    my $i = 0;
    my %signo;
    my @signame;
    foreach my $name (split(' ', $Config{sig_name})) {
	$signo{$name} = $i;
	$signame[$i] = $name;
#	print "signal $i is $name\n";
#	$SIG{$name} = \&cl;
	$i++;
    }
}

sub mktmpdir
{
    my $prefix = $ENV{TEMP}."/tmpdir";
    my $i = 0;
    my $tmp;
    do {
	$tmp = sprintf("%s_%04d", $prefix, $i++);
    } until (mkdir($tmp, 0700));
    return $tmp;
}

checksigs();

my $tmp;

sub cl
{
    my $sig = shift;
    rmtree($tmp);
    print "CLEANUP Aborted: $sig\n";
    exit(2);
}

use Getopt::Long;
my $OPT = {};
Getopt::Long::Configure('bundling');
GetOptions($OPT, 
	   "help|H|?", 
	   "leave|l",
	   ) || die "Ugh\n";

my $cur = cwd() || die;
$ENV{TEMP} || die;
$tmp = mktmpdir();
#$SIG{INT} = &cl;
#$SIG{QUIT} = &cl;
print "Using: $tmp\n";
chdir($tmp) || die;
mkdir('CVS', 0700) || die;
chdir('CVS');
copy("$cur/CVS/Root", ".") || die;
copy("$cur/CVS/Repository", ".") || die;
copy("$cur/CVS/Entries", ".") || die;
chdir('..') || die;
!(system("cvs up -dP") % 256) || die;
find(\&f, $tmp);
chdir($cur);
!(system("start /wait windiff $cur $tmp") % 256) || die;
rmtree($tmp);

sub f
{
    if (($_ eq 'CVS') && (-d $File::Find::name))
    {
	rmtree($File::Find::name);
    }
}

