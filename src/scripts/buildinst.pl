#!perl -w

use strict;
use FindBin;
use File::Path;
use File::Spec;
use File::Copy;
use File::Basename;
use lib "$FindBin::Bin/build/lib";
use Logger;
use Getopt::Long;
use Cwd;
use Win32::OLE;
use File::stat;

$0 = fileparse($0);


my $BUILDXML = 0; #If set to 0, buildinst will not support XML installers.  To enable, change value to 1.

if (defined($ENV{"BUILDINSTALLER"}) && lc($ENV{"BUILDINSTALLER"}) eq "xml")
{
	$BUILDXML = 1;
}


my $PROGID = 'WfWI.Document';
my $MAKE='wfwi';
my $WRAP='guiwrap';
my $WRAP_ARGS="--matchaserror --maxcount 3 --maxrun 3600 \"Wise for Windows Installer\" #32770 --exec $MAKE";

my $makefile_dir  = 'Makefile.dir';
my $makefile_inst = 'Makefile.inst';
my $helpfile      = 'Makefile-installers.html';
my $target;
my $verbose;
my $logger;

my $OPT = { logfile => "$0.log" };

my $DIR_CURRENT    = File::Spec->catfile($FindBin::Bin);
my $DIR_FILES      = File::Spec->catfile($DIR_CURRENT, 'files');

# We know the pismere directory is the parent directory of this script, so...
my $DIR_PISMERE    = File::Spec->catfile($FindBin::Bin);
$DIR_PISMERE =~ tr/\//\\/;           # Change any '/' characters to '\'
$DIR_PISMERE =~ s/[\\][^\\]*$//;     # Back us up a dir

my $DIR_INSTALLERS = File::Spec->catfile($DIR_PISMERE, 'installers');
my $DIR_MODULES    = File::Spec->catfile($DIR_INSTALLERS, 'module');
my $DIR_SCRIPTS    = File::Spec->catfile($DIR_PISMERE, 'scripts');
my $DIR_DOC        = File::Spec->catfile($DIR_SCRIPTS, 'build', 'doc');

my $DIR_TOP = $DIR_INSTALLERS;

# This variable gets fixed up later on...
my $DIR_TOP_PISMERE = File::Spec->catfile($FindBin::Bin, '..');

my @VALID_CPUS = ( 'i386', 'alpha' );
my $CPU;
my $TARGET_DIR;
my $DIR_PISMERE_MSI_TARGET;
my $Flags;
my $silent = '';

#########################################################################
#
# Global program configuration structures and functions for initializing them
#
#########################################################################

my $Targets =
{
    # To add a target, be aware that the program uses the following format
    #
    # target =>
    # {
    #     directory => 'text',              - source and output directories for this target
    #     requires_make => 1,               - this target requires the make program
    #     description => "text",            - printed by the usage screen
    #     properties => "prop1=val1 ..."),  - ;-delimited properties to set by default for this target
    # }

    release =>
    {
        directory => 'rel',
        requires_make => 1,
        description => "Build installer using release binaries (default)",
        properties => "",
    },
    debug =>
    {
        directory => 'dbg',
        requires_make => 1,
        description => "Build installer using debug binaries",
        properties => "DEBUGBUILD=1",
    },
    clean =>
    {
        no_check_ole => 1,
        description => "Deletes copied files, module, and installers",
    },
    getfiles =>
    {
        no_check_ole => 1,
        directory => 'rel',
        description => "Get release files only, do not build module or installer",
    },
    module =>
    {
        directory => 'rel',
        requires_make => 1,
        description => "Get files and build release module only, do not build installer",
    },
};

sub set_target_dir
{
    $CPU = $OPT->{cpu} || lc($ENV{CPU});
    if (!$CPU) {
        $CPU = lc($ENV{PROCESSOR_ARCHITECTURE});
        $CPU = 'i386' if ($CPU eq 'x86');
        $CPU = 'i386' if !$CPU;
    }
    my $valid = 0;
    foreach my $c (@VALID_CPUS) {
        $valid = 1 if ($CPU eq $c);
    }
    die "Unrecognized CPU type specified\n" if !$valid;

    if ($Targets->{$target}->{directory}) {
        $TARGET_DIR = $Targets->{$target}->{directory};
    } else {
        $TARGET_DIR = '';
    }

    $Flags->{PATH_BIN}->{default} = File::Spec->catfile($DIR_PISMERE, 'target', 'bin', $CPU, $TARGET_DIR);
    $Flags->{PATH_LIB}->{default} = File::Spec->catfile($DIR_PISMERE, 'target', 'lib', $CPU, $TARGET_DIR);
    $Flags->{PATH_INC}->{default} = File::Spec->catfile($DIR_PISMERE, 'target', 'inc');
    $Flags->{PATH_EXTRA}->{default} = '';

    # This gets set for place
    $DIR_PISMERE_MSI_TARGET = File::Spec->catfile($DIR_PISMERE, 'target', 'msi', $CPU, $TARGET_DIR);

    my $TEMP = cwd() || die;
    chdir($DIR_TOP_PISMERE) || die;
    $DIR_TOP_PISMERE = cwd() || die;
    chdir($TEMP) || die;
    $DIR_TOP_PISMERE =~ s/\//\\/g;
}

sub set_target_properties
{
    if ($Targets->{$target}->{properties}) {
        my @temp = split(';', $Targets->{$target}->{properties});
        foreach my $t (@temp) {
            $t =~ m/(.*)=(.*)/;
            $Flags->{$1}->{override} = $2;
        }
    }
}

sub set_flags
{
$Flags =
{
    # To add an option, be aware that the program uses the following format
    #
    # OPTION =>
    # {
    #     required => 1,          - necessary for a makefile to be valid
    #     makefile_only => 1,     - can't be command line overriden
    #     path = 'path',          - use this field of Flags to get path information
    #     description => "text",  - printed by the usage screen, MUST BE SPECIFIED!
    # }
    #
    # Internally, the program will use:
    #     override => ?,          - value to use instead of default or makefile specified
    #     value => ?,             - value specified in the current makefile
    #     default => ?,           - default value

    NAME =>
    {
        required => 1,
        makefile_only => 1,
        description => "Base name of target (don't specify extension)",
    },
    TYPE =>
    {
        wsm => "msm",  # How to convert the file extension from input to output
        wsi => "msi",  # How to convert the file extension from input to output
        required => 1,
        makefile_only => 1,
        description => "wsi or wsm",
    },
    OUTPUT =>
    {
        description => "Name of output file (don't specify extension)",
    },
    NOPLACE =>
    {
        description => "Don't place the output files",
    },
    PATH_BIN =>
    {
        description => "Source directory for binary files",
    },
    PATH_LIB =>
    {
        description => "Source directory for lib files",
    },
    PATH_INC =>
    {
        description => "Source directory for include files",
    },
    PATH_EXTRA =>
    {
        description => "Source directory for extra files",
    },
    FILES_BIN =>
    {
        makefile_only => 1,
        description => "Files to copy from PATH_BIN into files\\bin",
    },
    FILES_LIB =>
    {
        makefile_only => 1,
        description => "Files to copy from PATH_LIB into files\\lib",
    },
    FILES_INC =>
    {
        makefile_only => 1,
        description => "Files to copy from PATH_INC into files\\inc",
    },
    FILES_EXTRA =>
    {
        makefile_only => 1,
        description => "Other files to copy into files\\extra (give path)",
    },
};
}

sub properties_to_cl
{
    my $type = shift;
    my $cl = "";
    my $val = "";
    foreach my $f (sort keys %$Flags) {
        # If this is an expected property, don't put in compile command line
        next if ( $Flags->{$f}->{description} || $f =~ /FILES_\w+|PATH_\w+/);
        $cl = $cl."/p $f=";
        if (exists $Flags->{$f}->{override}) {
            $val = $Flags->{$f}->{override};
        } else {
            $val = $Flags->{$f}->{value};
        }
        $val = "\"$val\"" if ($val =~ m/.+\w.+/);
        $cl = $cl."$val ";
    }

    if (!$OPT->{nocomment} && ($type eq 'wsi')) {
	$cl = $cl."/p ARPCOMMENTS=\"".timestamp()."\" ";
    }

    return $cl;
}

sub timestamp
{
    use Win32;
    use Time::gmtime;
    use Sys::Hostname;
    my $USERNAME = $ENV{USERNAME} || Win32::LoginName();
    my $DATE = gmctime()." GMT";
    my $HOSTNAME = hostname();
    return "Build of $DATE by $USERNAME on $HOSTNAME";
}

#########################################################################
#
# Functions for parsing command line input
#
#########################################################################

# Gets the target from the command line, checking to make sure
# that it is a valid target, and that only one target is defined.
sub get_target
{
    my $multiple_targets = 0;

    foreach my $arg (@ARGV) {
        if ($arg =~ /^[^=]*$/) { # it is a target
            if ($Targets->{$arg}) {
                if ($multiple_targets) {
                    usage("\nCan't define multiple targets");
                }
                $target = $arg;
                $multiple_targets = 1;   # get target if specified
            } else {
                usage("\n$arg is not a valid target");
            }
        }
    }

    if (!$target) {
        $target = "release";   # default to release target
    }

    print "Target: $target\n" if $verbose;
}

# Gets overrides from the command line, checking to make sure that
# the option is recognized and overrideable
sub get_overrides
{
    if (!@ARGV) { return; }

    # Get information from command line
    foreach my $arg (@ARGV) {
        if ($arg =~ /(.*)=(.*)/) { # it is an override
            if ($Flags->{$1}) {
                if ($Flags->{$1}->{makefile_only}) {
                    usage("\nThe $1 option can not be overriden on the command line.");
                }
                usage("\nThe $1 flag is not repeatable") if $Flags->{$1}->{override};
            }
            $Flags->{$1}->{override} = $2;
        }
    }
}

sub print_overrides
{
    print "\n";
    my $found_an_override = 0;
    foreach my $f (sort keys %$Flags) {
        $found_an_override = 1 if exists $Flags->{$f}->{override};
    }
    print "Overriding $makefile_inst settings with the following options:\n" if $found_an_override;
    foreach my $f (sort keys %$Flags) {
        print exists $Flags->{$f}->{override} ? "\t$f = $Flags->{$f}->{override}\n" : "";
    }
    print "\n" if $found_an_override;
}

#########################################################################
#
# Functions for reading and parsing Makefiles
#
#########################################################################

sub read_file
{
    my $file = shift;
    my $fh = new IO::File;
    $fh->open("<$file");
    my @lines = <$fh>;
    $fh->close();
    @lines;
}

# This function parses Makefile.inst files
sub parse_inst_lines
{
    my @lines = @_;

    # Before we begin, lets clean up our hash and start fresh
    foreach my $f (keys %$Flags) {
        delete $Flags->{$f}->{value};
        if (!$Flags->{$f}->{description} && !(exists $Flags->{$f}->{override})) {
            delete $Flags->{$f}
        }
    }

    while (my $line = shift @lines) {
        chomp($line);

        $line =~ s/^#.*//;     # remove any line starting with #
        $line =~ s/[^\^]#.*//; # remove anything after an unescaped #

        # Allow the '\' character to signify that a line is continued
        if (($line =~ /^(.*)\\\s*$/) && !($line =~ /^(.*)\^\\\s*$/)) {
            my $n = shift @lines;
            $line = $1 . $n;
            unshift(@lines, $line);
            next;
        }

        $line =~ s/\^#/#/g;    # replace all ^# with #

        if ($line =~ /^\s*$/) {        # Ignore any blank lines
            next;
        } elsif ($line =~ /^\s*([^=\s]+)\s*=(.*)$/) {
            my $opt = $1;
            if ($Flags->{$opt}) {
                die "The $opt flag is not repeatable" if $Flags->{$opt}->{value};
            }
            $Flags->{$opt}->{value} = $2;
        } else {
            usage("\nBad $makefile_inst: $line is unrecognized");
        }
    }

    # Check to make sure all required entries have been filled
    foreach my $f (keys %$Flags) {
        if ($Flags->{$f}->{required} && !$Flags->{$f}->{value}) {
            die "Missing required $f in $makefile_inst";
        }
    }

    return 0;
}

# This function parses Makefile.dir files
sub parse_dirs_lines
{
    my @lines = @_;
    my $info = {};
    while (my $line = shift @lines) {
        chomp($line);

        $line =~ s/^#.*//;     # remove any line starting with #
        $line =~ s/[^\^]#.*//; # remove anything after an unescaped #

        # Allow the '\' character to signify that a line is continued
        if (($line =~ /^(.*)\\\s*$/) && !($line =~ /^(.*)\^\\\s*$/)) {
            my $n = shift @lines;
            $line = $1 . $n;
            unshift(@lines, $line);
            next;
        }

        $line =~ s/\^#/#/g;    # replace all ^# with #

        if ($line =~ /^\s*$/) {        # Ignore any blank lines
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
    print "ERROR: Missing DIRS/PREDIRS directives in $makefile_dir\n";
    return 0;
}

#########################################################################
#
# Functions for recursively traveling the directories
#
#########################################################################

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
                if (-e File::Spec->catfile($dir, $makefile_dir) ||
                    -e File::Spec->catfile($dir, $makefile_inst)) {
                    $err = do_dir($routine, $dir);
                } elsif ($OPT->{softdirs}) {
                    print "SKIPPING DIR MISSING Makefiles (softdirs enabled): $dir\n";
                } else {
                    print "ERROR: Cannot find $makefile_dir or $makefile_inst in $dir!\n";
                    $err = 2;
                }
                return $err if $err;
            } elsif ($OPT->{softdirs}) {
                print "SKIPPING MISSING DIR (softdirs enabled): $dir\n";
            } elsif ($dirs->{OPTDIRS}->{$short_dir}) {
                print "SKIPPING MISSING OPTIONAL DIR: $dir\n";
            } else {
                print "ERROR -- COULD NOT FIND DIR: $dir\n";
                return 2;
            }
        }
    }
    return 0;
}

sub do_dir
{
    my $routine = shift;
    my $dir = shift;
    my $dirs = {};
    my $dir_Makefile = File::Spec->catfile($dir, $makefile_dir);
    if (-e $dir_Makefile) {
        $dirs = parse_dirs_lines(read_file($dir_Makefile));
        return 0 if !$dirs;
    }
    print "Entering $dir\n" if $verbose;
    if (!(-e File::Spec->catfile($dir, $makefile_dir) ||
          -e File::Spec->catfile($dir, $makefile_inst)))
    {
        print "ERROR: Cannot find $makefile_dir or $makefile_inst in $dir!\n";
        return 2;
    }
    my $err = 0;
    $err = do_dirs($routine, $dir, $dirs, 'PREDIRS');
    return $err if $err;
    if (-e File::Spec->catfile($dir, $makefile_inst)) {
        $err = &$routine($dir);
        return $err if $err;
    }
    $err = do_dirs($routine, $dir, $dirs, 'DIRS');
    return $err if $err;
    print "Exiting $dir\n" if $verbose;
    return 0;
}

#########################################################################
#
# Functions for cleaning directories
#
#########################################################################

sub my_rmdir
{
    my $dir = shift;
    $dir =~ s/(\\\.)+//;  # remove any extra path information

    if (-d $dir) {
      print " DELETING: $dir\n";
      rmtree($dir, $verbose);
    }
}

sub my_rm
{
    my $dir = shift;
    my $file = shift;

    $dir =~ s/(\\\.)+//;  # remove any extra path information
    $file =~ s/.*[\\]+//; # remove any extra path information

    # Delete the file if it exists
    if (-e $dir.'\\'.$file) {
      print " DELETING: $dir\\$file\n";
      unlink("$dir\\$file");
    }
}

sub delete_filelist
{
    my $dir = shift;
    my $filelist = shift;

    if ($filelist)
    {
        my @temp = split(' ', $filelist);
        map { my_rm($dir, $_); } @temp;
	rmdir($dir);
    }
}

# This function deletes the target directories, the module
# for the directory, and any files that may have been grabbed
sub clean
{
    my $prevdir = shift;
    my $dir = shift;
    $dir = File::Spec->catfile($prevdir, $dir);

    print "Cleaning $dir\n" if $verbose;

    # Remove any target directories
    my_rmdir($dir.'\\obj');

    # Remove the module if it exists
    my_rm($DIR_MODULES, $Flags->{"NAME"}->{value}.'.'.'msm');

    # Remove Files
    foreach my $f (keys %$Flags) {
        if ($f =~ /FILES_/ && $Flags->{$f}->{value}) {  # get FILES_* only
            my $temp = get_getfiles_output_dir($f);
            delete_filelist($dir.'\\'.$temp, $Flags->{$f}->{value});
        }
    }
}

#########################################################################
#
# Functions for getting files
#
#########################################################################

# Given a type FILES_XYZ, gets the path for that type from PATH_XYZ
# The path for FILES_XYZ is taken first from the command line override,
# then from the Makefile, and defaulting to a default.
sub get_path_from_type
{
    my $type = shift;
    return '' if ($type eq ''); # return nothing if given nothing

    my $path = $type;
    $path =~ s/^FILES_/PATH_/;  # substitute PATH_ for FILES_

    # This is a sanity check - did the programmer upgrade the app properly?
    die "No path variable defined for this type!!!!!!!" if !$Flags->{$path};

    my $output = $Flags->{$path}->{override} || # Use command line specified path
                 $Flags->{$path}->{value}    || # Else use makefile specified path
                 $Flags->{$path}->{default};    # Else fallback to default

    return $output;
}

sub mycopy
{
    my $complete = shift; # file
    my $to = shift; # file or dir

    my $placing = shift;

    my ($name, $path) = fileparse($complete);

    my $outfile;

    if (-d $to) {
        $outfile = $to.'\\'.$name;
    } else {
        $outfile = $to;
    }

    my ($to_name, $to_path) = fileparse($to);

    sub get_relative
    {
        my $path = shift;
        my $top = shift;

        my $pdir = $path;
        my $i = length($top);
        if (lc(substr($pdir, 0, $i)) eq lc($top)) {
            my $c = substr($pdir, $i, 1);
            if ($c eq '/' || $c eq '\\') {
                $pdir = substr($pdir, $i + 1);
            }
        }
        $pdir;
    }

    my $stin = stat($complete);
    my $stout = stat($outfile);
    my $skip;
    if ($stin && $stout) {
        if ($stout->mtime == $stin->mtime && $stout->size == $stin->size) {
            $skip = 1;
        }
    }
    undef $stout;

    my $from_rel = get_relative($complete, $DIR_TOP_PISMERE);
    my $to_rel = get_relative($to, $DIR_TOP_PISMERE);

    print $placing ? " PLACING $name in" : " COPYING $from_rel to",
          " ", $placing ? $to_rel : $to;

    if ($skip) {
        print " ...up-to-date...SKIPPED\n";
        return 1;
    } else {
        my $ok = copy($complete, $to);
        if ($ok) {
            utime($stin->atime, $stin->mtime, $outfile);
            print "\n";
            #print "COPIED: $name to $pdir\n";
        } else {
            die " ...ERROR: $!\n";
        }
        return $ok;
    }
}

# my_copy()
#
# This function copies a file from a dir to a dir.
#
# If $from is blank, $file should have a complete path.  The function
# knows filename1:filename means rename filename1 to filename2 during
# copy.  The function recognizes $(PISMERE) and will substitute the
# correct path for it.  The function recognizes $(CPU) and will
# substitute the correct CPU for it The function recognizes $(T) and
# will substitute the correct target for it.
sub my_copy
{
    my $from = shift; # directory
    my $file = shift;
    my $to = shift;   # directory
    my $placing = shift;

    my $orig_to = $to;

    if (! -d $to) {
        mkpath($to, $verbose) || die "$!";
    }

    # If we want to rename the file upon copying, do it
    if ($file =~ s/(.*):(.*)/$1/) {
        $to = $to.'\\'.$2
    }

    # if we were passed in a blank from,
    my $complete;
    if ($from) {
        $complete = "$from\\$file"
    } else {
        $complete = $file;
        $file =~ s/.*[\\]+//;
    }

    # Replace any variables
    $complete =~ s/\$\(PISMERE\)/$DIR_PISMERE/;
    $complete =~ s/\$\(CPU\)/$CPU/;
    $complete =~ s/\$\(T\)/$TARGET_DIR/;

    my $op;
    my $print_to;

    mycopy($complete, $to, $placing, $file) || die "$!";
}

sub get_files
{
    my $prevdir = shift;
    my $dir = shift;
    $dir = File::Spec->catfile($prevdir, $dir);

    # Get all FILES_XXX
    foreach my $f (keys %$Flags) {
        if ($f =~ /FILES_/ && $Flags->{$f}->{value}) {  # get FILES_* only
            my @temp = split(' ', $Flags->{$f}->{value});
            map { my_copy(get_path_from_type($f),
                          $_,
                          get_getfiles_output_dir($f)); } @temp;
        }
    }
}

# Takes a flag FILES_XXX, returns files\\xxx
sub get_getfiles_output_dir
{
    my $type = shift;
    $type =~ s/^FILES_//;
    $type =~ tr/[A-Z]/[a-z]/;
    return ".\\files\\$type";
}

#########################################################################
#
# Functions for building files
#
#########################################################################

sub get_output_filename
{
    return $Flags->{"OUTPUT"}->{override} ||
           $Flags->{"OUTPUT"}->{value}    ||
           $Flags->{"NAME"}->{value};
}

# Takes a file type, returns the directory the output file should go in
sub get_target_dir
{
    my $type = shift;
    return "$DIR_MODULES\\" if ($type eq 'wsm');
    return ".\\obj\\$CPU\\$TARGET_DIR\\";
}






sub buildinst
{
    my $prevdir = shift;
    my $dir = shift;
    my $err = 0;

    # build each configuration in Makefile.inst
    my @temp = split(' ', $Flags->{"TYPE"}->{value});
    foreach my $t (@temp) {


	my $target_dir = get_target_dir($t);

	if ($t eq "xml" and $BUILDXML)
	{
		
		my $NAME = get_output_filename();
		system "perl $DIR_SCRIPTS\\BuildXML.pl $NAME $target";
		#Copy the file from the current dir to the target dir, then delete the original MSI

		if (! -d $target_dir) {
			    mkpath($target_dir, $verbose) || die "$!";
		}


		system "copy $NAME.msi $target_dir";
		system "copy $NAME.msi $DIR_PISMERE_MSI_TARGET";
		system "del $NAME.msi";
		next;
		

	}
	if ($t eq "xml" and !$BUILDXML)
	{
		next;
	}

	if ($t eq "transform")
	{
		#skip if we are doing the vstuio 6 build
		if (defined($ENV{"VSTUDIOVER"}) && ($ENV{"VSTUDIOVER"} =~ m/VSTUDIO6/i))
		{
			next;
		}
		my $NAME = get_output_filename();
		my $SRCDIR = "\\\\afs\\all\\athena.mit.edu\\user\\d\\o\\dongq\\Public\\$NAME";
		my $err;
		my $command;
		my $filename;
		if ($target eq "debug")
		{
			$filename = "$NAME-DEBUG.msi";
		}
		else 
		{
			$filename = "$NAME.msi";
		}
		$command = "copy $SRCDIR\\$filename .";
		print "$command \n";
		$err = system($command) / 256;
		if ($err)
		{ 
			print "error copying $SRCDIR\\$filename. return code:$err\n";
		}
		else
		{
			$command = "msitran.exe -a $NAME.mst $filename";
			print "$command \n";
			$err = system($command)/ 256;
			if ($err) {
				print "error applying transform to $filename. return code:$err\n";
			}
			else{
				if (! -d $target_dir) {
			    		mkpath($target_dir, $verbose) || die "$!";
				}
	
				$command = "copy $filename $target_dir ";
				print "\n$command \n";
				$err = system($command) / 256;
				if (!$err)
				{
					$command = "copy $filename $DIR_PISMERE_MSI_TARGET"; 
					print "$command \n";
					$err = system($command) / 256;
				}
				if ($err)
				{
					print "error copying $filename to the target directory. return code:$err\n";
				}
				system "del $filename";
			}
		}
		next;		
	}

	next if ($target eq 'module' && $t ne 'wsm'); # skip installers if target is module
	my $output_file = get_output_filename().'.'.$Flags->{"TYPE"}->{$t};
	my $output_file_path = $target_dir.$output_file;
	my $del;
	$del = (-e $output_file_path) ?
	  unlink $output_file_path : 1;
	if (!$del) {
	    print "ERROR: Could not delete pre-existing ".
	      "$output_file_path\n";
	    last;
	}

	if (! -d $target_dir) {
	    mkpath($target_dir, $verbose) || die "$!";
	}

	if (!$OPT->{oldstyle}) {
	    my $oWsh = 0;
	    my $ok;

	    sub SetMediaOption
	    {
		my $oWsh = shift || die;
		my $opt = shift;
		my $value = shift;

		my $ok = $oWsh->SetMediaOption($opt, $value);
		if (!$ok) {
		    my $errdesc = $oWsh->GetLastError();
		    print "SetMediaOption $opt Error: $errdesc\n";
		}
	    }

	    print "Instantiating $PROGID...\n";
	    $oWsh = Win32::OLE->GetActiveObject($PROGID);
	    if (!$oWsh) {
		$oWsh = Win32::OLE->new($PROGID) or
		  die "Could not load WfWI object, path problem?\n";
	    }

	    if ($OPT->{silent}) {
		$oWsh->SetSilent();
	    }

	    print " prevdir: $prevdir\n";
	    print " dir: $dir\n";

	    SetMediaOption($oWsh, "PathType", "0");
	    SetMediaOption($oWsh, "File Location", "1");
	    SetMediaOption($oWsh, "EXE Build", "2");
	    SetMediaOption($oWsh, "Filename Type", "2");

	    $oWsh->EnableRelease("", 1);

	    my $input_file_name;
	    $input_file_name = File::Spec->catfile($prevdir, $dir, $Flags->{"NAME"}->{value});
	    $input_file_name = $input_file_name.".".$t;
	    print "Opening: $input_file_name\n";
	    $ok = $oWsh->Open($input_file_name);
	    if (!$ok) {
		my $errdesc = $oWsh->GetLastError();
		print "WfWI Document Open Error: $errdesc\n";
	    }

	    sub SetProperty
	    {
		my $oWsh = shift || die;
		my $f = shift;
		my $val = shift;
		print " Setting property: $f to $val\n";
		my $ok = $oWsh->Invoke('SetProperty', $f, $val);
		if (!$ok) {
		    my $errdesc = $oWsh->GetLastError();
		    print "SetProperty $f Error: $errdesc\n";
		}
	    }

	    my $val = "";
	    foreach my $f (sort keys %$Flags) {
		# If this is an expected property,
		# don't put in compile command line
		next if ($Flags->{$f}->{description} ||
			 $f =~ /FILES_\w+|PATH_\w+/);

		if (exists $Flags->{$f}->{override}) {
		    $val = $Flags->{$f}->{override};
		} else {
		    $val = $Flags->{$f}->{value};
		}
		SetProperty($oWsh, $f, $val);
	    }

	    # If we are building a wsi file, append a timestamp to the
	    # Add / Remove Panel comments field
	    if (!$OPT->{nocomment} && ($t eq 'wsi')) {
		my $current_comments = $oWsh->GetProperty('ARPCOMMENTS');
		$current_comments .= " " if $current_comments;
		SetProperty($oWsh, 'ARPCOMMENTS', $current_comments.timestamp());
	    }

	    my $full_output_file_path = ($t eq 'wsm') ? $output_file_path : File::Spec->catfile($prevdir, $dir, $output_file_path);

	    print " input name: $input_file_name\n";
	    print " output path: $output_file_path\n";
	    print " output file: $output_file\n";
	    print " target dir: $target_dir\n";
	    print " full output: $full_output_file_path\n";
	    $ok = $oWsh->Compile($full_output_file_path);
	    if (!$ok) {
		my $errdesc = $oWsh->GetLastError();
		print "WFWI Document Compile Error: $errdesc\n";
	    }
	    # $err = $oWsh->DESTROY();
	    $err = 0;
	} else {
	    my $command =
	      "$MAKE ".		# wise executable
		$Flags->{"NAME"}->{value}.".$t ". # input file
		  $silent.	# Silent flag?
		    "/c ".	# compile and exit
		      properties_to_cl($t). # properties
			"/o ".get_target_dir($t). # output directory
			  $output_file;

	    print " COMPILE $command\n";
	    $err = system($command) / 256;
	}

	if ($err) {
	    print "RETURNED ERROR CODE: $err\n";
	    $del = (-e $output_file_path) ?
	      unlink $output_file_path : 1;
	    if (!$del) {
		print "ERROR: Could not delete pre-existing ".
		  "$output_file_path\n";
		last;
	    }
	    last;
	}

	next if ($t ne 'wsi');	# only try to place msi files
	my $noplace = $Flags->{"NOPLACE"}->{override} ||
	  $Flags->{"NOPLACE"}->{value};
	next if $noplace;
	$dir = File::Spec->catfile($prevdir, $dir, 'obj', $CPU, $TARGET_DIR);
	my_copy($dir, $output_file, $DIR_PISMERE_MSI_TARGET, 1) || die "$!";
    }
    return $err;
}

sub build_dir
{
    my $dir = shift;
    my $prevdir = cwd() || die "Cannot get cwd\n";
    chdir($dir) || die "Could not chdir to $dir\n";
    my $err = 0;

    if (! -e $makefile_inst) {
        print "ERROR: Cannot find $makefile_inst in $dir!\n";
        $err = 2;
	goto cleanup;
    }

    print "Parsing $makefile_inst in $dir\n";
    parse_inst_lines(read_file($makefile_inst));

    if ($target eq 'clean') {
	clean($prevdir, $dir);
    } else {
	get_files($prevdir, $dir);
	if ($target ne 'getfiles') { # don't build if we are just getting files
	    $err = buildinst($prevdir, $dir);
	}
    }

  cleanup:
    chdir($prevdir) || die "Could not chdir to $prevdir\n";
    return $err;
}

#########################################################################
#
# Main and Usage
#
#########################################################################

sub makescript
{
    return $^X.' '.File::Spec->catfile($DIR_SCRIPTS, @_);
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
               'silent|s',
               'softdirs',
               'cpu=s',
               'verbose',
               'forcewrap',
               'nowrap',
               'oldstyle|o',
               'tools',
               'toolsonly',
               'nocomment',
              );

    if ($OPT->{docs}) {
        system("start ".File::Spec->catfile($DIR_DOC, $helpfile));
        exit(0);
    }

    if ($OPT->{verbose}) {
        $verbose = "verbose";
    }

    if ($OPT->{silent}) {
        $silent = "/s ";
    }

    if ($OPT->{top} && !chdir($DIR_TOP)) {
        die "Could not chdir to $DIR_TOP\n";
    }

    # Parse the command line options
    set_flags();
    get_overrides();
    get_target();
    set_target_dir();
    set_target_properties();

    # OK to call usage after this point in execution
    if ($OPT->{help} || !$OPT->{logfile}) {
        usage();
    }

    # Check wrap settings
    if ($OPT->{forcewrap} && $OPT->{nowrap}) {
        usage();
    }

    # Use old style if we specify any wrapping options
    if ($OPT->{forcewrap} || $OPT->{nowrap}) {
        $OPT->{oldstyle} = 1;
    }

    # Make sure we have $MAKE and/or $WRAP if needed
    {
        my $WHICH_CMD = makescript('which.pl');
        my $path_for_make = `$WHICH_CMD $MAKE`;
        chomp($path_for_make) if $path_for_make;

        if (!-e $path_for_make && $Targets->{$target}->{requires_make}) {
            die "$MAKE is not in your path.\n";
        }

        my $BIN_DIR = File::Spec->catfile($DIR_PISMERE, 'target', 'bin', $CPU,
                                          $TARGET_DIR);

        my $path_for_wrap = `$WHICH_CMD --path "$BIN_DIR;$ENV{PATH}" $WRAP`;
        chomp($path_for_wrap) if $path_for_wrap;

        if (!$OPT->{nowrap} && -e $path_for_wrap) {
            $MAKE = "$path_for_wrap $WRAP_ARGS";
        } elsif ($OPT->{forcewrap}) {
            die "$WRAP is not in you path.\n";
        }
    }

    # Check for automation stuff
    if (!$OPT->{oldstyle} && !$Targets->{$target}->{no_check_ole}) {
        if (!check_ole($PROGID)) {
            die "$PROGID is not installed\n";
        }
    }


    if ($OPT->{toolsonly}) {
        print_tools();
        exit(0);
    }

    # Start logging at this point
    if (!$OPT->{nolog}) {
        $logger = new Logger $OPT->{logfile};
        $logger->start;
    }

    print_tools() if $OPT->{tools};

    print_overrides();

    my $err = 0;
    $err = do_dir(\&build_dir, '.');

    # If we were logging, stop the logger
    if (!$OPT->{nolog}) {
        $logger->stop;
    }

    return $err;
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
        my $ver = `filever $full`;
        chomp($ver);
        return { cmd => $cmd, full => $full, ver => $ver };
    }
    my $V =
    {
       WfWI => get_info('wfwi.exe'),
    };
    my $PATH = $ENV{PATH} || '';

    print <<DATA;

PATH=$PATH

DATA

    foreach my $k ('WfWI') {
        if ($V->{$k}) {
            print "$k: $V->{$k}->{full}\n$V->{$k}->{ver}\n";
        } else {
            print "$k: NOT FOUND\n";
        }
    }

    print "\n";

    my $check = check_ole($PROGID);
    if ($check) {
        print "$PROGID is being served by $check->{exe}\n";
    } else {
        print "$PROGID is not installed\n";
    }
}

sub check_ole
{
    use Win32::TieRegistry;
    my $check_progid = shift || die;

    # We check for the keys one step at a time to make sure we don't
    # accidentally try to create them.

    my $CLASSES;
    my $PROGID;
    my $CLSID;
    my $CHECK;

    $CLASSES = $Registry->{Classes};
    return 0 if !$CLASSES;
    $PROGID = $CLASSES->{$check_progid};
    return 0 if !$PROGID;
    $CLSID = $PROGID->{CLSID};
    return 0 if !$CLSID;

    my $desc = $PROGID->{''};
    my $clsid = $CLSID->{''};
    return 0 if !$clsid;

    $CLSID = $CLASSES->{CLSID};
    return 0 if !$CLSID;
    $CLSID = $CLSID->{$clsid};
    return 0 if !$CLSID;

    $CHECK = $CLSID->{'LocalServer32'};
    return 0 if !$CHECK;
    my $exe = $CHECK->{''};

    $CHECK = $CLSID->{'ProgID'};
    return 0 if !$CHECK;
    my $progid = $CHECK->{''};

    $CHECK = $CLSID->{'InprocHandler32'};
    return 0 if !$CHECK;
    my $dll = $CHECK->{''};

    return 0 if !$desc;
    return 0 if !$exe;
    return 0 if !$progid;
    return 0 if !$dll;

    return { progid => $progid, clsid => $clsid, desc => $desc,
         exe => $exe, dll => $dll };
}

sub usage
{
    my $message = shift;
    print "$message\n\n" if $message;

    print <<USAGE;
Usage: $0 [options] [overrides] [target]

  Options:
    --help, -h, -?     Usage Information (what you now see)
    --docs             Build Documentation
    --top              Build from top-level dir (default is current dir)
    --logfile filename Log to specified filename (default: $0.log)
       or -l filename
    --nolog, -n        Do not log
    --silent, -s       Build silently (no message boxes)
    --softdirs         Do not bail on missing directories
    --cpu=cputype      Override CPU setting
    --verbose          Verbose output
    --forcewrap        Requires wrapper (cannot use --nowrap) (oldstyle)
    --nowrap           No wrapper (cannot use --forcewrap) (oldstyle)
    --oldstyle, -o     Use no-automation style (default is to use automation)
    --tools            Show tools
    --toolsonly        Show tools and exit
    --nocomment        Do not timestamp the installer's comment field
USAGE

    print "\n  Valid Makefile Overrides:\n";
    foreach my $f (sort keys %$Flags) {
        if ( !$Flags->{$f}->{makefile_only} ) { # only print valid command line overrides
            my $string = "   $f";
            if ($Flags->{$f}->{description} ) { # only print description if there is one
                $string = $string."\t$Flags->{$f}->{description}";
            }
            print $string, "\n";
        }
    }
    print "   Any other override is assumed to be a property, and is passed through to the compiler.\n";

    print "\n  Valid Targets:       (Only one may be specified)\n";
    foreach my $t (sort keys %$Targets) {
        print "   $t", $Targets->{$t}->{description}?"\t$Targets->{$t}->{description}":"","\n";
    }

    exit(1);
}

exit(main());
