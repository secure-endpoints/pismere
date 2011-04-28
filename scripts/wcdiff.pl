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
use DirHandle;
$0 = fileparse($0);

my @cvs_revisions = ();  # holds the revisions to fetch from cvs
my @cvs_dates = ();      # holds the dates to fetch from cvs
my $files = "";          # string containing the files the user asked for
my $tmp1;                # name of temp dir where set1 will be checked out to
my $tmp2;                # name of temp dir where set2 will be checked out to
my $global_u = "";       # global nolock option
my $verbosity = "-Q";    # verbosity - super quiet by default
my $not_recursive = "";  # don't do a recursive check
my $dp_option = "-dP";   # use cvs up -dP
my $cur;                 # base directory
my $relative_dir = "";   # relative dir from $cur or $tmp2

# Fairly self-evident - figures out where the temp directory for
# this user is and creates a unique subdirectory
sub mktmpdir
{
    my $program = basename($0, '.pl');
    my $prefix = $ENV{TEMP}."\\tmpdir_$program";
    my $i = 0;
    my $tmp;
    do {
        $tmp = sprintf("%s_%04d", $prefix, $i++);
    } until (mkpath($tmp, !$verbosity));
    return $tmp;
}

# parse command line options
# call put_files_in_tmp_dirs subroutine
# call windiff on new folders
# delete the temporary directories
sub main
{
    use Getopt::Long;

    my $OPT = {};
    Getopt::Long::Configure('no_ignore_case','bundling',
               'no_auto_abbrev', 'no_getopt_compat', 'require_order');
    GetOptions($OPT,
               "help|H|?",
               "global_u|u",
               "not_recursive|l",
               "verbose|V",
               "r=s" => \@cvs_revisions,
               "D=s" => \@cvs_dates,
	       ) || usage();

    if (($OPT->{help}) || ($#cvs_revisions + $#cvs_dates > 0)) {
	    usage();
    }

    # flatten files requested into a string
    foreach my $file (@ARGV) { $files = $files." ".$file; }

    # files specified => don't recursively diff
    if ($OPT->{not_recursive} || $files) {
        $not_recursive = "-l";
        $dp_option = "";
    }

    if ($OPT->{global_u}) {
        $global_u = "-u";
    }
    if ($OPT->{verbose}) {
        $verbosity = "";
    }

    $cur = cwd() || do_error_exit("Could not cwd");
    $ENV{TEMP} || do_error_exit("No environmental variable named TEMP");
    $tmp1 = mktmpdir();
    $tmp2 = mktmpdir();

    put_files_in_tmp_dirs();

    !(system("start /wait windiff $tmp1 $tmp2") / 256) || do_error_exit("Could not run windiff");

    if (-d $tmp1) { rmtree($tmp1, !$verbosity); }
    if (-d $tmp2) { rmtree($tmp2, !$verbosity); }
}

sub put_files_in_tmp_dirs
{
    my $stuff_to_get = "";

    print "Using: $tmp1\n";
    chdir($tmp1) || do_error_exit("Could not chdir to $tmp1");
    mkpath('CVS', !$verbosity) || do_error_exit("Could not mkpath CVS");
    chdir('CVS');
    copy("$cur\\CVS\\Root", ".") || do_error_exit("Could not copy CVS\\Root");
    copy("$cur\\CVS\\Repository", ".") || do_error_exit("Could not copy CVS\\Repository");
    copy("$cur\\CVS\\Entries", ".") || do_error_exit("Could not copy CVS\\Entries");
    chdir('..') || do_error_exit("Could not chdir");

    # Gets the first revision or date if the user specified one
    # Gets the most recent stuff in the repository otherwise
    if($cvs_revisions[0]) {
        $stuff_to_get = "-r ".$cvs_revisions[0];
    }elsif($cvs_dates[0]){
        $stuff_to_get = "-D ".$cvs_dates[0];
    }

    print "Executing: cvs $verbosity $global_u up $dp_option $not_recursive $stuff_to_get $files\n";
    !(system("cvs $verbosity $global_u up $dp_option $not_recursive $stuff_to_get $files") / 256)
           || do_error_exit("Could not run cvs up");
    find(\&f, $tmp1);
    chdir($cur);

    print "Using: $tmp2\n";

    # The user wants to compare the current dir and the repository
    if($#cvs_revisions + $#cvs_dates != 0) {
        print "Copying files from: $cur\n";
        chdir($cur);
        copy_dir("");
        chdir($cur);
        return;
    }

    # The user wanted to compare two revisions OR
    # The user wanted to compare two dates     OR
    # The user wanted to compare a date with a revision
    chdir($tmp2) || do_error_exit("Could not chdir to $tmp2");
    mkpath('CVS', !$verbosity) || do_error_exit("Could not mkpath CVS");
    chdir('CVS');
    copy("$cur\\CVS\\Root", ".") || do_error_exit("Could not copy CVS\\Root");
    copy("$cur\\CVS\\Repository", ".") || do_error_exit("Could not copy CVS\\Repository");
    copy("$cur\\CVS\\Entries", ".") || do_error_exit("Could not copy CVS\\Entries");
    chdir('..') || do_error_exit("Could not chdir");
    if($cvs_revisions[1]) {
        $stuff_to_get = "-r ".$cvs_revisions[1];
    }elsif($cvs_dates[1]){
        $stuff_to_get = "-D ".$cvs_dates[1];
    }

    print "Executing: cvs $verbosity $global_u up $dp_option $not_recursive $stuff_to_get $files\n";
    !(system("cvs $verbosity $global_u up $dp_option $not_recursive $stuff_to_get $files") / 256)
           || do_error_exit("Could not run cvs up");

    find(\&f, $tmp2);
    chdir($cur);
}

# This function copies all of the files in our current directory
# that we aren't told to ignore in a .cvsignore file.
# Then, unless specified otherwise at the command line, it recursively
# acts on all of the subdirectories that we aren't ignoring.
sub copy_dir
{
    my $relative_dir = shift;
    my $complete_dir = $cur.$relative_dir;

    # if certain files are specified, don't copy any others
    if ($files) {
        my @temp = split(/ /,$files);
        foreach my $temp2 (@temp) {
            print "copying $relative_dir\\$temp2\n" if !$verbosity;
            copy("$temp2", "$tmp2\\$relative_dir\\$temp2");
        }
        return;
    }

    # This will store any regexp filters for our files
    # By default, we will be ignoring certain files
    my @ignore_regexps = ('^.+\.obj$',  # ignore *.obj
                          '^.+\.exe$',  # ignore *.exe
                          '^cvs$'       # ignore CVS subdir
                         );

    chdir($complete_dir);
    print "Checking: $relative_dir\n" if !$verbosity;
    mkpath($tmp2.$relative_dir, !$verbosity);

    # if there is a .cvsignore file, open it up and add
    # some regexps to ignore the correct type of files
    if(open(CVSIGNORE, "<.cvsignore")) {
        print "Found $relative_dir\\.cvsignore - parsing\n" if !$verbosity;
        my @lines = <CVSIGNORE>;     # read it into an array
        close(CVSIGNORE);            # close the file
        foreach my $line (@lines) {  # assign @lines to $line, one at a time
            chomp($line);
            next if ($line =~ m/^\s*$/);  # ignore blank lines in .cvsignore
            if ($line =~ m/\./) {         # restrict a file type
                print "restricting file type: $line\n" if !$verbosity;
                $line =~ s/\./\\\./;      # replace . with \.
                $line =~ s/\*/.+/;        # replace * with .+
            } else {                      # restrict a subdir
                print "restricting subdir: $line\n" if !$verbosity;
            }
            push(@ignore_regexps, "^$line\$");
        }
    }

    # copy every file that we aren't told to ignore
    my $d = new DirHandle $complete_dir;
    my @files = $d->read;
    $d->close;
	my $bad_file;
    while ($files[0]) {
	    $bad_file = 0;
        my $f = shift @files;
        next if (($f eq '.') || ($f eq '..'));   # skip dirs
        next if(-d $complete_dir."\\".$f);       # skip dirs

		foreach my $regexp (@ignore_regexps) {   # skip any ignores
		    if ($f =~ m/$regexp/i) {
			    print "Ignoring file: $relative_dir$f\n" if !$verbosity;
			    $bad_file = 1;
			}
		}
        next if($bad_file);

        print "copying $relative_dir\\$f\n" if !$verbosity;
        copy("$f", "$tmp2\\$relative_dir\\$f");    # copy otherwise
    }

    return if $not_recursive;

    # recursively copy_dir any subdirs
    $d = new DirHandle $complete_dir;
    @files = $d->read;
    $d->close;
	my $bad_dir;
    while ($files[0]) {
	    $bad_dir = 0;
        my $f = shift @files;
        next if (($f eq '.') || ($f eq '..'));
        next if !(-d $complete_dir."\\".$f);    # its a file

        foreach my $regexp (@ignore_regexps) {   # skip any ignores
            if ($f =~ m/$regexp/i) {
                print "Ignoring directory: $relative_dir\\$f\n" if !$verbosity;
                $bad_dir = 1;
			}
		}
        next if($bad_dir);

        copy_dir($relative_dir."\\".$f);
    }
}

sub f
{
    if (($_ eq 'CVS') && (-d $File::Find::name))
    {
        rmtree($File::Find::name);
    }
}

# Print an error message and exit the program.
sub do_error_exit
{
    my $error_msg = shift;
    $error_msg = "$0: ".$error_msg;
    print "\n$error_msg\n";
    chdir($cur);  # It's possible that we exited while in a temp dir
    if (-d $tmp1) { rmtree($tmp1, !$verbosity); }
    if (-d $tmp2) { rmtree($tmp2, !$verbosity); }

    exit(1);
}

sub usage
{
    print <<USAGE;
Usage: $0 [-ulV] [[-r rev1 | -D date1] [-r rev2 | -D date2]] [files...]

Options:
    -?, -H, --help        Help
    -u, --global_u        Use cvs -u (nolock) option
    -l, --not_recursive   Local directory only, not recursive
    -V, --verbose         Verbose output
    -D  date1             Diff revision for date1 against working file
    -D  date2             Diff rev1/date1 against date2
    -r  rev1              Diff revision for rev1 against working file
    -r  rev2              Diff rev1/date1 against rev2.
USAGE
    exit(0);
}

main();
