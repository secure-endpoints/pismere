#Chris Cary
#10:29 AM 8/21/2003

#this script is designed to be called by buildinst.pl when it encounters an XML file.  In that regard, this script effectively does for XML files what buildinst.pl currently does for wise.
#!perl -w

use strict;
use FindBin;
use Getopt::Long;
use File::Basename;

$0 = fileparse($0);
sub usage
{
    my $message = shift;
    print "$message\n\n" if $message;

    print <<USAGE;
Usage: $0 [options] XMLFILE TARGET

  Options:
    --help, -h, -?     Usage Information (what you now see)

  XMLFILE              name of the XMLFILE without the extension
  TARGET               optional, release or debug. default to release
USAGE
}

unlink<NewXML.xml>;

my $DIR_PISMERE    = File::Spec->catfile($FindBin::Bin);
$DIR_PISMERE =~ tr/\//\\/;           # Change any '/' characters to '\'
$DIR_PISMERE =~ s/[\\][^\\]*$//;     # Back us up a dir

my $OPT={};
Getopt::Long::Configure('no_bundling', 'no_auto_abbrev',
			    'no_getopt_compat', 'require_order',
			    'ignore_case', 'pass_through',
			    'prefix_pattern=(--|-|\+|\/)',
			   );
    GetOptions($OPT,
	       'help|h|?'
	       );

    if ($OPT->{help}) {
	usage();
	exit(0);
    }


my $FILENAME_NO_EXTENSION = shift;
my $target = shift;
my $DEBUG;
my $RELEASE;
my $SUBST;

if (!$target) {
	$target = "release";   # default to release target
}


#find the files table
if (!$FILENAME_NO_EXTENSION || $FILENAME_NO_EXTENSION eq "")
{
	usage("no xml file found");
	exit(1);
}

my $SRC = $DIR_PISMERE;
if (open(CFG, "$FILENAME_NO_EXTENSION.cfg"))
{
	while (<CFG>)
	{
		chomp;
		s/^\s*(.*?)\s*$/$1/;
		if (/^SRC=(.*)/i)
		{
			$SRC = $DIR_PISMERE."\\".$1;
		}
		elsif (/^DEBUG=(.*)/i)
		{
			$DEBUG=$1;
		}
		elsif (/^RELEASE=(.*)/i)
		{
			$RELEASE=$1;
		}
	}
}
		
$SRC = lc($SRC);
$target = lc($target);

if ($target eq "debug")
{
	$SUBST = $DEBUG;
}
else
{
	$SUBST = $RELEASE;
}

open(XML, "$FILENAME_NO_EXTENSION.xml") or die "can't find $FILENAME_NO_EXTENSION.xml\n";
open (NEWXML, ">>newxml.xml");

my $GO = 0;
my $HREF_LINE = "";
my $The_Tagged_Path = "";
my $original_line;

while ($original_line = <XML>) 	#this loop is going to run until the bottom of the script at the ned of this big while loop, have a line that reads like print OUT $output.  Output will just be the line if it's supposed to be, or it will be
{
	my $line = $original_line;

	$line = lc($line);
	chomp($line);

	#remove the initial tabs from the line
	$line =~ s/^\s*(.*?)//;

	if ($line eq '<table name="file">')
	{
		#read in the first filename
		$GO = 1;
	}
	
	if ($line eq '</table>')
	{
		if ($GO == 1)
		{
			$GO = 0;
		}
	}
	
	if ($GO ne 1)
	{
		print NEWXML $original_line;
		next;
	}
	

	if ($GO and substr($line, 0, 8) eq "<td href") 
	{
		$HREF_LINE = $original_line;
		if ($HREF_LINE =~ /.* href="(.*?)"/i)
		{
			my $FILE_NAME = $1;
			my $i = index($FILE_NAME, '$(SRC)');
			if ($i >= 0)
			{
				substr($FILE_NAME, $i, length('$(SRC)'), $SRC);
			}
			
			$FILE_NAME =~ s/\$\(TARGET\)/$SUBST/g;
			
			$HREF_LINE =~ s/(.*?href=")(.*?)(".*)/$1$FILE_NAME$3/;
			
			my $XML_CHKSUM = `md5sum $FILE_NAME`;
			
			if ($XML_CHKSUM eq "")
			{
				print "****can't get the checksum for file: $FILE_NAME****\n";
				print NEWXML $HREF_LINE;
			}
			else
			{
				if (substr($XML_CHKSUM, 0, 1) eq "\\")
				{
					$XML_CHKSUM = substr($XML_CHKSUM,1,32);
				}
				else
				{
					$XML_CHKSUM = substr($XML_CHKSUM,0,32);
				}
				
				$HREF_LINE =~ s/(.*?md5=")(.*?)(".*)/$1$XML_CHKSUM$3/;
				print NEWXML $HREF_LINE;
			}
		}
		else
		{
			close(XML);
			close(NEWXML);
			die "The HREF line $HREF_LINE is not formatted correctly.";
		}
	} #end href line
	else
	{
		print NEWXML $original_line;
	}
	
} # end of processing the XML file

close(XML);
close(NEWXML);
system "XML2MSI NewXML.xml";

unlink<$FILENAME_NO_EXTENSION.msi>;

system "rename NewXML.MSI $FILENAME_NO_EXTENSION.msi";

unlink<NewXML.xml>;






