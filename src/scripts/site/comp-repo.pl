#perl -w

use strict;
use FindBin;
use Cwd;
use Getopt::Long;
use File::Basename;

$0 = fileparse($0);

use Safe;
my $SAFE = new Safe;
${$SAFE->varglob('ENV')}{COMPUTERNAME} = $ENV{COMPUTERNAME};
my $M = $SAFE->rdo(File::Spec->catfile($FindBin::Bin, "comp-repo.conf"));

my $OPT = {};
my @DIR_STACK = ();

my $UNK = 'unk';
my $DIF = 'diff';
my $OLD = 'old';
my $NEW = 'new';

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

sub my_system
{
    my $cmd = shift || die;
    my $err = system($cmd, @_) / 256;
    if ($err) {
	die "Command \"$cmd\" failed with exit code: $err\n";
    }
}

sub runbanner
{
    my $cmd = shift || die;
    print "\nRunning $cmd in directory ",my_cwd(),"\n\n";

}

sub do_for_eval
{
    my $eval = shift || die;
    my $cmd = shift || die;
    my_system("for /f \"delims=\" \%a in (\'$eval\') do \@$cmd");
}

sub get_cvs
{
    my $dir = shift || die;
    my $src = shift || die;
    runbanner("CVS to get $dir");
    die if (!$src->{cvsroot});
    die if (!$src->{module});
    my_system("cvs -d $src->{cvsroot} co -P -d $dir $src->{module}");

    pushd($dir);
    runbanner("keyword cleanup");
    do_for_eval('dir /ad /s /b CVS?', 'rd /s/q %a');
    popd();
}

sub get_zip
{
    my $dir = shift || die;
    my $src = shift || die;
    die if (!$src->{dir});
    my $top = my_cwd();
    pushd($src->{dir});
    runbanner("zip to get $dir");
    my_system("zip -rX $top/$dir.zip * -x CVS/* -x */CVS/* -x .#* -x */.#* -x *~");
    popd();

    my_mkdir($dir);
    pushd($dir);
    runbanner('unzip to get $dir');
    my_system("unzip $top/$dir.zip");
    popd();
}

sub get
{
    my $dir = shift || die;
    my $info = shift || die;
    if ($info->{cvsroot}) {
	get_cvs($dir, $info);
    } elsif ($info->{dir}) {
	get_zip($dir, $info);
    } else {
	die;
    }
    keyword_cleanup($dir);
}

sub make_diff
{
    my $D = do_diff();
    #my_system("start windiff $OLD $NEW");

    runbanner('diff zipfile creation');
    zip_diff($NEW, $D->{$DIF});
    zip_diff($NEW, $D->{$NEW});

    my_mkdir($DIF);
    pushd($DIF);
    my_system("unzip ../$DIF.zip");
    popd();
}

sub zip_diff
{
    my $dir = shift || die;
    my $list = shift;
    return if !$list;
    my @files = quotify(prefix("$dir/", @$list));
    my_system("zip -X $DIF.zip ".join(' ', @files));
}

sub prefix
{
    my $prefix = shift || die;
    my @list = @_;
    my @files = ();
    map { push(@files, $prefix.$_); } @list;
    return @files;
}

sub quotify
{
    my @list = @_;
    my @files = ();
    map { push(@files, '"'.$_.'"'); } @list;
    return @files;
}

sub keyword_cleanup
{
    my $dir = shift || die;
    pushd($dir);
    runbanner('CVS keyword cleanup');
    my @files = `cmd /c dir /a-d /s /b`;
    map {
	my $file = $_;
	chomp($file);
	system('perl', ('-pi.bak', '-e', 's/\$((Author)|(Date)|(Header)):.*\$/\$$1\$/', "\"$file\""));
    } @files;
    do_for_eval('dir /a-d /s /b *.bak', 'del "%a"');
    popd();
}

sub do_diff
{
    my $D; # = { $OLD => [], $NEW => [], $DIF => [], };
    my @lines = `diff -rq $OLD $NEW`;
    foreach my $line (@lines) {
	chomp ($line);
	if ($line =~ /^Only in (($OLD)|($NEW))\/?(.*): (.*)$/) {
	    push(@{$D->{$1}}, ($4 ? ($4.'/'):'').$5);
	} elsif ($line =~ /^Files $OLD\/(.*) and $NEW\/(.*) differ$/) {
	    push(@{$D->{$DIF}}, $1);
	} else { 
	    push(@{$D->{$UNK}}, $_);
	}
    }
    map {
	print "$_:\n";
	map {
	    print "\t$_\n";
	} @{$D->{$_}};
    } keys %$D;
    return $D;
}

sub usage
{
    my $code = shift || 0;
    print <<USAGE;
Usage: $0 [options] module
    -h, -H, --help   help
  Valid Modules:
USAGE
    foreach my $m (sort keys %$M) {
	print "    $m\n";
    }
    exit($code);
}

sub main
{
    Getopt::Long::Configure('bundling');
    GetOptions($OPT,
	       'help|h|?',
	       );

    my $module = shift @ARGV;
    usage() if $OPT->{help};
    usage(1) if (!$module ||
		 !$M->{$module} ||
		 !$M->{$module}->{$OLD} ||
		 !$M->{$module}->{$NEW});

    if (-d 'temp') {
	die "temp already exists\n";
    }
    my_mkdir('temp');
    pushd('temp');
    get($OLD, $M->{$module}->{$OLD});
    get($NEW, $M->{$module}->{$NEW});
    make_diff();
    popd();
}

main();
