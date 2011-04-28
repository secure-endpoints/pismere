#!perl -w

use strict;
use Config;
use FindBin;
use File::Basename;
use File::Spec;
use lib $FindBin::Bin;
use Getopt::Long;
use Time::localtime;
use File::Path;
use DirHandle;
use Win32;
use Sys::Hostname;

$0 = fileparse($0);
my $DIR_SITE_SCRIPTS = File::Spec->catfile($FindBin::Bin);


my $todays_dir;
my $complete_dir;

# Global option variables
my $OPT = {};

my $DIR_SUFFIX = "";
my $tagged_build = 0;
$tagged_build = 1 if (defined($ENV{"KRB5BRANCH"}) && ($ENV{"KRB5BRANCH"} =~ m/TAGGED/i));
$DIR_SUFFIX = "-tagged" if ($tagged_build);
$DIR_SUFFIX = "-head" if (defined($ENV{"KRB5BRANCH"}) && ($ENV{"KRB5BRANCH"} =~ m/HEAD/i));

$DIR_SUFFIX = ($DIR_SUFFIX."-vs7") if (defined($ENV{"VSTUDIOVER"}) && ($ENV{"VSTUDIOVER"} =~ m/VSTUDIO7/i));

$DIR_SUFFIX = ($DIR_SUFFIX."xpsp2") if (defined($ENV{"SDKVER"}) && ($ENV{"SDKVER"} =~ m/MSSDK\.XPSP2/i));

my $USERNAME = $ENV{USERNAME} || Win32::LoginName();
my $DOMAIN = 'mit.edu';
my $EMAIL = $USERNAME.'@'.$DOMAIN;
my $MAIL_SERVER = 'outgoing.mit.edu';
my $SUMMARY_FILE = 'summary.txt';
my $LINKD_NAME = "build-current$DIR_SUFFIX";

my $basedir;
my $sharedir;
my $builds_to_keep = 0;
my $summary = $SUMMARY_FILE;
my $name_for_linkd = $LINKD_NAME;
my $from = $EMAIL;
my $who_to_mail = $EMAIL;
my $mail_server = $MAIL_SERVER;

my $cvsroot;
my $cvs_arguments;
my @cvs_commands;

my @build_names;
my %build_commands;

my $replace_cvs_name;
my $replace_build_name;
my $replace_wash_name;
my $replace_tag_name;

my @tagged_commands;
sub main
{
    # First take care of command line options
    Getopt::Long::Configure('bundling');
    usage() if (!GetOptions($OPT,
			    'help|H|?',
			    'nokrb|K',
			    'nomail|N',
			    'replacecvs|C=s' => \$replace_cvs_name,
			    'replacebuild|B=s' => \$replace_build_name,
			    'replacewash|W=s' => \$replace_wash_name,
			    'replacetag|T=s' => \$replace_tag_name,
			    'nodelete',
			    'nocvs',
			    ) || @ARGV);
    usage() if $OPT->{help};



    # Next, lets parse our config files.
    parse_config() || do_error_exit("Bad configuration files");
    print_params();

    if (defined($ENV{"REMOTEWASH"}) && ($ENV{"REMOTEWASH"}) eq 'yes')
    {
        my $tm = localtime;
        my $s_date = sprintf("on %02d-%02d-%04d at %02d:%02d",
                                  $tm->mon+1, $tm->mday, 1900+$tm->year, $tm->hour, $tm->min);
	  
        
        my $remote_config = "mail-recipient:";
	$remote_config .=$ENV{"USERNAME"};
	$remote_config .= "\nbuild-tool:";
	$remote_config .= $ENV{"VSTUDIOVER"};
	$remote_config .= "\nMicrosoft SDK version:";
	$remote_config .= $ENV{"SDKVER"};
	$remote_config .= "\nbuild-type:";
        $remote_config .= $ENV{"KRB5BRANCH"};
	$remote_config .= "\ninstaller:";
      	$remote_config .= $ENV{"BUILDINSTALLER"};
      	$remote_config .= "\nupdatewash:";
      	$remote_config .= $ENV{"UPDATEWASH"}; 
      	$remote_config .= "\nsend-log-to:";
	$remote_config .= $ENV{"LOGLOCATION"};       
		
	my $remote_config_file = $DIR_SITE_SCRIPTS."\\remote_config_file"; 
	unlink $remote_config_file;
	open(TEMPFILE, "+>$remote_config_file") || do_error_exit("Could not open $remote_config_file for output");


	print TEMPFILE $remote_config;
	close TEMPFILE;
	my $commandline = "blat $remote_config_file -s \"wash$DIR_SUFFIX started remotely $s_date by ";
	$commandline .= $ENV{"AUTHENTICATEDUSER"} ;
	$commandline .= "\" ";
                
        $commandline .= "-t \"$who_to_mail\" ";
        $commandline .= "-f $from ";
        $commandline .= "-server $mail_server ";
        print "\n$commandline\n";
        if(system($commandline)/256) {
            print "$0: Execution of $commandline failed\n";
        }
	unlink $remote_config_file;
        
	  
    }
	
	
    # The only reason that we get tickets before getting
    # today's dir is that there may be a long deletion
    # process and we want the user's time commitment to be
    # minimal
    get_kerb_tickets() if !$OPT->{nokrb};
    go_to_todays_dir();
    do_cvs() if !$OPT->{nocvs};
    do_tag() if $tagged_build;
    destroy_kerb_tickets() if !$OPT->{nokrb};

    do_builds();

    # if too many directories, delete the oldest
    delete_dirs_as_necessary();

    linkd_to_today();

    exit(0);
}

#########################################################################
#
# Functions for parsing options and config files
#
#########################################################################

sub print_params
{
    print "\$ENV\{KRB5BRANCH\}:\"".$ENV{"KRB5BRANCH"}."\": Appending \"$DIR_SUFFIX\" to directory names...\n" if ($DIR_SUFFIX);
    print "BASEDIR:         $basedir\n";
    print "SHAREDIR:        $sharedir\n";
    print "BUILDS_TO_KEEP:  $builds_to_keep\n";
    print "WHO_TO_MAIL:     $who_to_mail\n";
    print "FROM:            $from\n";
    print "MAIL_SERVER:     $mail_server\n";
    print "NAME_OF_SUMMARY: $summary\n";
    print "LINKD_NAME:      $name_for_linkd\n";
    print "CVSROOT:         $cvsroot\n";
    print "CVS_ARGUMENTS:   $cvs_arguments\n";
    print "CVS_COMMANDS:\n";
    print "\t", join("\n\t", @cvs_commands), "\n";
    print "BUILD_COMMANDS: (TBD)\n";
#    print "\t", join("\n\t", %build_commands), "\n";
    print "TAGGED_COMMANDS:\n";
    print "\t", join("\n\t", @tagged_commands), "\n";

}

# returns 1 if OK, exits if NOT OK
sub parse_config
{
    my $wash_name = $DIR_SITE_SCRIPTS."\\wash.conf";
    foreach my $line (read_config_file($wash_name))
    {
	# Don't bother parsing a section if it's destined to be replaced
        if ($line =~ m/^CVS\s?:\s?(.*)/) {
            parse_cvs_option($1) if !$replace_cvs_name;
        } elsif ($line =~ m/^BUILD\s?:\s?(.*)/) {
            parse_build_option($1) if !$replace_build_name;
        } elsif ($line =~ m/^WASH\s?:\s?(.*)/) {
            parse_wash_option($1) if !$replace_wash_name;
        } elsif ($line =~ m/^TAG\s?:\s?(.*)/) {
            parse_tag_option($1) if ($tagged_build && !$replace_tag_name);
        } else {
            do_error_exit("Unknown line in $wash_name:\n $line");
        }
    }

    # Parse overriding CVS options
    # NOTE: These blocks do not return errors on malformed lines
    if ($replace_cvs_name) {
	foreach my $line (read_config_file($replace_cvs_name)) {
	    if ($line =~ m/^CVS\s?:\s?(.*)/) {
		parse_cvs_option($1);
	    }
	}
    }

    # Parse overriding build options
    if ($replace_build_name) {
	foreach my $line (read_config_file($replace_build_name)) {
	    if ($line =~ m/^BUILD\s?:\s?(.*)/) {
		parse_build_option($1);
	    }
	}
    }

    # Parse overriding wash options
    if ($replace_wash_name) {
	foreach my $line (read_config_file($replace_wash_name)) {
	    if ($line =~ m/^WASH\s?:\s?(.*)/) {
		parse_wash_option($1);
	    }
	}
    }

    # Parse overriding tag options
    if (($tagged_build) && ($replace_tag_name)) {
        foreach my $line (read_config_file($replace_tag_name)) {
	    if ($line =~ m/^TAG\s?:\s?(.*)/) {
		parse_tag_option($1);
	    }
        }
    }

    # Make sure that the config file isn't missing any mandatory options
    do_error_exit("Missing CVSROOT in $wash_name") if !$cvsroot;
    do_error_exit("Missing CVS_ARGUMENTS in $wash_name") if !$cvs_arguments;
    do_error_exit("Missing BASEDIR in $wash_name") if !$basedir;
    do_error_exit("Missing SHAREDIR in $wash_name") if !$sharedir;

    return 1;
}

sub read_config_file
{
    my $filename = shift;

    open(CONFIG_FILE, "<$filename") || do_error_exit("Could not open $filename for input");
    my @lines = <CONFIG_FILE>;
    close(CONFIG_FILE);

    # Filter out blank and comment lines and chomp entries before returning
    foreach (@lines) { chomp($_); }
    return grep { $_ !~ m/^\#/ && $_ !~ m/^\s*$/ } @lines;
}

sub parse_cvs_option
{
    my $option = shift;

    if ($option =~ m/^CVSROOT\s?=\s?(.*)/i) { # case insensitive match
        $cvsroot = $1;
    } elsif ($option =~ m/^ARGUMENTS\s?=\s?(.*)/i) {
        $cvs_arguments = $1;
    } elsif ($option =~ m/^EXTRA_COMMAND\s?=\s?(.*)/i) {
        push(@cvs_commands, $option);
    } else {
        do_error_exit("Unrecognized cvs option:\n $option");
    }
}

sub parse_build_option
{
    my $option = shift;
    $option =~ m/(\w*)\s?=\s?(.*)/; # split around the equals sign
    do_error_exit("Unrecognized build option:\n $option") if !($1 && $2);
    push(@build_names, $1);
    $build_commands{$1} = $2;
}

sub parse_wash_option
{
    my $option = shift;

    if ($option =~ m/^BASEDIR\s?=\s?(.*)/i) { # case insensitive match
        $basedir = $1;
    } elsif ($option =~ m/^BUILDS_TO_KEEP\s?=\s?(.*)/i) {
        $builds_to_keep = $1;
    } elsif ($option =~ m/^WHO_TO_MAIL\s?=\s?(.*)/i) {
        $who_to_mail = $1;
    } elsif ($option =~ m/^FROM\s?=\s?(.*)/i) {
        $from = $1;
    } elsif ($option =~ m/^SHAREDIR\s?=\s?(.*)/i) {
        $sharedir = $1;
        my $host = hostname();
        $sharedir =~ s/\*/$host/;
    } elsif ($option =~ m/^MAIL_SERVER\s?=\s?(.*)/i) {
        $mail_server = $1;
    } elsif ($option =~ m/^NAME_OF_SUMMARY\s?=\s?(.*)/i) {
        $summary = $1;
    } elsif ($option =~ m/^LINKD_NAME\s?=\s?(.*)/i) {
        $name_for_linkd = $1;
    } else {
        do_error_exit("Unrecognized wash option:\n $option");
    }
}

sub parse_tag_option
{
    my $option = shift;
    if ($option =~ m/(.+?):(.*)/)
    {
      push(@tagged_commands,($2." ".$1));
    }
    else
    {
      push(@tagged_commands,$option);
    }
}

#########################################################################
#
# Functions for kerberos tickets
#
#########################################################################

sub get_kerb_tickets
{
    # Get new kerberos tickets
    destroy_kerb_tickets();
    print "Please enter kerberos principal for cvs: " unless $USERNAME;
    my $principal = $USERNAME || <>;
    if (system("kinit $principal")/256) {
        do_error_exit("Error on kinit");
    }
}

sub destroy_kerb_tickets
{
    if (system("kdestroy")/256) {
        print "$0: Error on kdestroy\n";
        exit(1);
    }
}

#########################################################################
#
# Functions for checking out the source tree
#
#########################################################################

# Run the cvs command
# run the additional commands
sub do_cvs
{
    print "Doing CVS...\n";
    if (system("cvs -d$cvsroot $cvs_arguments")/256) {
        do_error_exit("Error on cvs - $!");
    }

   foreach my $cvs_cmd (@cvs_commands) {
        # split entry into command and dir to run in
        $cvs_cmd =~ m/(.*):(.*)/;
        chdir($basedir.$1);
        if (system("$2")/256) {
           do_error_exit("Error on $2 - $!");
        }
    }
}

# Run the cvs tagged command

sub do_tag
{
	if (!defined($ENV{"KRB5BRANCH"}) ||($ENV{"KRB5BRANCH"} !~ m/TAGGED/i))
	{
		return;
	}

    print "Doing CVS tagging...\n";

	foreach my $comm (@tagged_commands)
	{
		print "cvs -d$cvsroot update -d -P $comm\n";
		if (system("cvs -d$cvsroot update -d -P $comm")/256)
		{
			do_error_exit("Error on cvs - $!");
		}
     }
}

#########################################################################
#
# Functions for manipulating the directories
#
#########################################################################

sub e_date
{
    my $tm = shift;
    my $s_date = ref($tm)?sprintf("%02d-%02d-%04d",
                                  $tm->mon+1, $tm->mday, 1900+$tm->year):
                                  $tm;
    do_error_exit("Internal error: e_date($tm)!") if
                  !($s_date =~ /^\d{2}-\d{2}-\d{4}$/);
    return $s_date;
}

sub first_newer_than_second
{
    my $first = shift;
    my $second = shift;

    $first =~ m/^build-(\d{2})-(\d{2})-(\d{4})/i;
    my $first_month = $1;
    my $first_day = $2;
    my $first_year = $3;
    $second =~ m/^build-(\d{2})-(\d{2})-(\d{4})/i;
    my $second_month = $1;
    my $second_day = $2;
    my $second_year = $3;

    return 1 if($first_year > $second_year);
    return 0 if ($first_year < $second_year);

    return 1 if($first_month > $second_month);
    return 0 if ($first_month < $second_month);

    return 1 if($first_day > $second_day);
    return 0 if ($first_day < $second_day);

    return 0;
}

sub delete_dirs_as_necessary()
{
    my $d = new DirHandle $basedir;
    return if !$builds_to_keep;
    my @files = $d->read;
    $d->close;
    my $count=0;
    while ($files[0]) { # get a count of files
        my $f = shift @files;
        next if (($f eq '.') || ($f eq '..') || ($f !~ m/^build-\d\d-\d\d-\d\d\d\d$DIR_SUFFIX$/i));
        my $path = $basedir.'\\'.$f;
        $count++ if (-d $path);
    }

    while($count > $builds_to_keep) {
        $d = new DirHandle $basedir;
        @files = $d->read;
        $d->close;
        my $oldest_dir = $files[0];
        while (($oldest_dir eq '.') || ($oldest_dir eq '..') || ($oldest_dir !~ m/^build-\d\d-\d\d-\d\d\d\d$DIR_SUFFIX$/i)) {
            $oldest_dir = shift @files;
        }

        # delete oldest directory
        while ($files[0]) {
            my $f = shift @files;
            my $path = $basedir.'\\'.$f;
            next if (($f eq '.') || ($f eq '..') || ($f !~ m/^build-\d\d-\d\d-\d\d\d\d$DIR_SUFFIX$/i));
            next if (! -d $path);
            $oldest_dir = $f if first_newer_than_second($oldest_dir, $f);
        }
        rmtree($basedir."\\".$oldest_dir);
        $count--;
    }
}

sub go_to_todays_dir
{
    # go to basedir, create it if it doesn't exist.
    if (!chdir($basedir)) {
        print "basedir $basedir does not exist.\n";
        print "Attempting to create $basedir.\n";
        mkpath($basedir) || do_error_exit("Error on mkdir - $!");
        chdir($basedir) || do_error_exit("Could not chdir to $basedir\n");
    }
    # get today's date, use it to create a directory.
    # (if the directory already exists, blow it away)
    my $tm = localtime;
    my $s_date = e_date($tm);
    $complete_dir = $basedir."\\build-".$s_date.$DIR_SUFFIX;
    $todays_dir = "build-".$s_date.$DIR_SUFFIX;
    my $skip_mkdir = 0;
    if(chdir($complete_dir)) {
	if ($OPT->{nodelete}) {
	    # The directory already exists, so we don't need to create it.
	    $skip_mkdir = 1;
	} else {
	    chdir($basedir); # destroy!
	    print "Deleting existing directory, please be patient.\n";
	    rmtree($complete_dir);
	    print "Completed deletion, continuing with wash.\n";
	}
    }
    if (!$skip_mkdir) {
	if (system("mkdir $complete_dir")/256) {
	    do_error_exit("Error on mkdir - $!");
	}
    }
    chdir($complete_dir);
}

# use linkd to map a standard junction point to today
sub linkd_to_today()
{
    chdir($basedir) || do_error_exit("Error on chdir - $!");
    if (system("linkd $name_for_linkd $complete_dir")/256) {
        do_error_exit("Could not $name_for_linkd to $complete_dir");
    }
}

#########################################################################
#
# Functions for building the source
#
#########################################################################

# build the tree in each mode
sub do_builds
{
    my $tm = localtime;
    my $today = e_date($tm);
    my $all_succeeded = 1;
    my $log_pass = 0;

    # get a text file ready to mail
    open(TEMP_FILE, "+>$complete_dir\\$summary") || do_error_exit("Could not open $complete_dir\\$summary for output");

    foreach my $build_tag (@build_names) {
        my $build_cmd = $build_commands{$build_tag};
        $build_cmd =~ m/(.*):(.*):(.*):(.*)/; # log file = $1, output dir = $2
        chdir($complete_dir);
        chdir($3);

        my $tm_start = localtime;
        my $err = system("$complete_dir\\$4")/256;
        my $tm_stop = localtime;

        $all_succeeded = 0 if $err;

        print TEMP_FILE "$build_tag: ";
        print TEMP_FILE "FAILED\n" if $err;
        print TEMP_FILE "SUCCEEDED\n" if !$err;
        print TEMP_FILE "build cmd: $4\n";

	# If necessary, look deeper for errors by scanning the output log
	# (keep in mind that grep returns 0 if no matches are found)
	if (!$err) { $log_pass = system("grep -i fatal $sharedir\\$todays_dir\\$1"); }

        my $start_date = sprintf("%s %02d:%02d:%02d",
                         $today, $tm_start->hour, $tm_start->min, $tm_start->sec);
        my $stop_date = sprintf("%s %02d:%02d:%02d",
                        $today, $tm_stop->hour, $tm_stop->min, $tm_stop->sec);
        print TEMP_FILE "build started at: $start_date\n";
        print TEMP_FILE "build stopped at: $stop_date\n";
        print TEMP_FILE "Log available at: $sharedir\\$todays_dir\\$1 \n";
	if ($err) {
		my $n = 20;
		print TEMP_FILE "Last $n lines:\n";
		my $lines = `tail -$n $sharedir\\$todays_dir\\$1`;
		$lines =~ s/^/  /mg; # indent 2 spaces
		$lines =~ s|\[../../.. ..:..:..\] ||g; # remove annoying timestamps
		print TEMP_FILE $lines;
	}
        print TEMP_FILE "Binaries available at: $sharedir\\$todays_dir\\$2 \n\n";
	  
	if (defined($ENV{"REMOTEWASH"}) && ($ENV{"REMOTEWASH"}) eq 'yes')
        {
		if (defined($ENV{"LOGLOCATION"}) && $ENV{"LOGLOCATION"} ne ' ')
		{
			my $log_loc = $ENV{"LOGLOCATION"};
			if (system("copy $sharedir\\$todays_dir\\$1 $log_loc\\$build_tag.log")/256) 
			{
        			do_error_exit("can not copy $sharedir\\$todays_dir\\$1 to $log_loc");
        		}
		}
	}
    }
    close(TEMP_FILE);            # Close the file

    if (!$OPT->{nomail}) {
	my $host = hostname();
        my $commandline =  "blat $complete_dir\\$summary -q ";
        if($all_succeeded) {
            $commandline .= "-s \"wash$DIR_SUFFIX SUCCEEDED on $today on $host\" ";
        } elsif (!$log_pass) {
	    $commandline .= "-s \"wash$DIR_SUFFIX FAILED (read log) on $today on $host\" ";
	} else {
            $commandline .= "-s \"wash$DIR_SUFFIX FAILED on $today on $host\" ";
        }
        $commandline .= "-t \"$who_to_mail\" ";
        $commandline .= "-f $from ";
        $commandline .= "-server $mail_server ";
        if(system($commandline)/256) {
            print "$0: Execution of $commandline failed\n";
        }
    }
}

#########################################################################
#
# Misc. Functions
#
#########################################################################

sub do_error_exit
{
    my $error_msg = shift;
    $error_msg = "$0: ".$error_msg;
    print "$error_msg";

    # send error mail here
    if (!$OPT->{nomail}) {
        my $commandline =  "echo $error_msg | blat - -q ";
        $commandline .= "-s \"WASH ERROR\" ";
        $commandline .= "-t \"$who_to_mail\" ";
        $commandline .= "-f $from ";
        $commandline .= "-server $mail_server ";
        if(system($commandline)/256) {
            print "$0: Execution of $commandline failed\n";
        }
    }

    destroy_kerb_tickets() if !$OPT->{nokrb};
    exit(1);
}

sub usage
{
    print <<USAGE;
Usage: $0 [options]

Options:
    -?, -H, --help             Usage Information (what you now see)
    -K, --nokrb                Assume user has tickets, don't call kinit
    -N, --nomail               Don't send mail upon completion
    -C, --replacecvs <file>    Override CVS options
    -B, --replacebuild <file>  Override build options
    -W, --replacewash <file>   Override wash options
    -T, --replacetag <file>    Override tag options
  For testing:
    --nodelete                 Do not delete directory if it already exists
    --nocvs                    Do not do CVS step
USAGE
    exit(1);
}

main();
