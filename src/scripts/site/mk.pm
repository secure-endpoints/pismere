package mk;

use strict;
use File::Basename;

BEGIN {
    use Exporter   ();
    use vars       qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);

    $VERSION     = 1.00;

    @ISA         = qw(Exporter);
    @EXPORT      = qw(&mkdate &rtouch &mktmpdir &fixtime &pushd &popd
		      &my_system &my_system_error &my_cwd &my_pause
		      &my_move &my_copy &my_isabsolute &my_getdrive);
    %EXPORT_TAGS = ();
    @EXPORT_OK   = qw($M);
}

use vars @EXPORT_OK;

use File::Copy;
use File::Spec;
use Cwd;

sub mkdate
{
    my $tm = shift;
#    return sprintf("%04d-%02d-%02d-%02d%02d",
#		   1900+$tm->year, $tm->mon+1, $tm->mday,
#		   $tm->hour, $tm->min);
    return sprintf("%04d-%02d-%02d",
		   1900+$tm->year, $tm->mon+1, $tm->mday);
}

sub mktmpdir
{
    my $program = basename($0, '.pl');
    my $prefix = $ENV{TEMP}."\\tmpdir_$program";
    my $i = 0;
    my $tmp;
    do {
	$tmp = sprintf("%s_%04d", $prefix, $i++);
    } until (mkdir($tmp, 0700));
    return $tmp;
}

sub rtouch
{
    my $dir = shift;
    my $time = shift;

    utime $time, $time, $dir;

    use DirHandle;
    my $d = new DirHandle $dir;
    return if !defined $d;
    my @files = $d->read;
    $d->close;
    foreach my $f (@files) {
	next if (($f eq '.') || ($f eq '..'));
	$f = File::Spec->catfile($dir, $f);
	if (-d $f) {
	    rtouch($f, $time);
	} else {
	    utime $time, $time, $f;
	}
    }
}

sub fixtime
{
    my $ts = shift;
    if ($ts =~ /^\s*(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)\s*$/) {
	my $year = $1;
	my $mon = $2 - 1;
	my $mday = $3;
	my $hours = $4;
	my $min = $5;
	my $sec = $6;
	use Time::Local;
	use Time::localtime;
	my $time = timelocal($sec,$min,$hours,$mday,$mon,$year);
	if (!$time || !ctime($time)) {
	    die "Time value out of range\n";
	}
	return $time;
    } else {
	return 0;
    }
}

sub sitescriptexec
{
    my $name = shift;
    my $args = shift;
    $^X.' '.File::Spec->catfile('pismere', 'scripts', 'site', $name).
	($args?(' '.$args):'');
}

sub my_chdir
{
    my $dir = shift || die;
    return chdir($dir) || die "Could not change directory to $dir\n";
}

sub my_mkdir
{
    my $dir = shift || die;
    return mkdir($dir, 0700) || die "Could not create directory $dir\n";
}

sub my_cwd
{
    return cwd() || die "Could not get current directory\n";
}

my @DIR_STACK = ();

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

my $SYSTEM_ERROR;

sub my_system
{
    my $cmd = shift || die;
    my $err;
    print "CMD: $cmd\n";
    $err = system($cmd, @_) / 256;
    if ($err) {
	warn "Command \"$cmd\" failed with exit code: $err\n";
	$SYSTEM_ERROR = $err;
    }
    return !$err;
}

sub my_system_error
{
    return $SYSTEM_ERROR;
}

sub my_pause
{
    print "--- Press ENTER to continue ---\n";
    <STDIN>;
}

sub my_move
{
    my $from = shift || die;
    my $to = shift || die;
    my $verbose = shift;
    print "MOVE: $from ==> $to..." if $verbose;
    move($from, $to) || die "\nCould not move: $from ==> $to\n";
    print "DONE\n" if $verbose;
    return 1;
}

sub my_copy
{
    my $from = shift || die;
    my $to = shift || die;
    my $verbose = shift;
    print "COPY: $from ==> $to..." if $verbose;
    copy($from, $to) || die "\nCould not copy: $from ==> $to\n";
    print "DONE\n" if $verbose;
    return 1;
}

sub my_isabsolute
{
    my $path = shift || die;

    my $first = substr($path, 0, 1);

    return ($first eq '/' || $first eq "\\" || !!my_getdrive($path));
}

sub my_getdrive
{
    my $path = shift || die;

    my $first = uc(substr($path, 0, 1));
    my $second = substr($path, 1, 1);

    return (($first =~ /[A-Z]/) && ($second eq ':')) ? $first : 0;
}

END {
    ### XXX ###
    #print "TRAP\n";
}

1;
