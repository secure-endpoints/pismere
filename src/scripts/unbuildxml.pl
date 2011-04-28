#This is the new version of MakeXML.


#steps:

#run msi2xml -c on the msi.  
#find the files table
#read in the first filename  (In order to find the right file, you're gonna need to read in the first few lines and #make sure that you're getting the filename and not the modified MSI name.)
#search the tree for that file's location


#read through all possibilities for that file, and compare the MD5 in the xml to the md5 of that file.  if it matches, #use that one.  If none of them match, use the one you cached as the first one as the file, and replace the md5 in the #xml to match.
#(Assign MD5 to a value --> scan for files --> --> store name/location of first file --> get md5 of first file + #compare --> repeat till a match, or till end of file list --> use first stored file as file --> get the md5 of that #file again --> replace the md5 in the code to match the new md5)


#change the href in the XML file to the new one
#go through and check the MD5 of all the files in question and change them if need be.
#delete cabs table (if it exists)
#delete cab's folder created during msi2xml
#delete the original XML
#rename the new xml to the old one
#done.

#!perl -w

use strict;
use FindBin;
use Cwd;

unlink<NewXML.xml>;

my $DIR_PISMERE    = File::Spec->catfile($FindBin::Bin);
$DIR_PISMERE =~ tr/\//\\/;           # Change any '/' characters to '\'
$DIR_PISMERE =~ s/[\\][^\\]*$//;     # Back us up a dir

opendir (HOME, ".") or die "can't read current dir\n";


my @HOME_FILES = readdir (HOME);

my $potential_msi;
my $FILENAME_NO_EXTENSION = "";
my $extension;
my $FILENAME;
my $DEBUG;
my $RELEASE;
foreach $potential_msi (@HOME_FILES)
{
	$extension = lc($potential_msi);
	if (substr($extension,-3) eq "msi")
	{
		$FILENAME_NO_EXTENSION = substr($potential_msi,0,-4);	
	#	system "msi2xml -c Cab $potential_msi";
		$FILENAME = $potential_msi;
		last;
	}
}

if ($FILENAME_NO_EXTENSION eq "")
{
	die "no msi file found\n";
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
$DEBUG =~ s/\\/\\\\/g;
$RELEASE =~ s/\\/\\\\/g;

#find the files table

open (XML, "$FILENAME_NO_EXTENSION.xml") or die "can't find $FILENAME\n";
open (NEWXML, ">>newxml.xml");

my $current_dir = cwd();
$current_dir =~ tr/\//\\/;         

my $COPY = 1;
my $GO = 0;
my $XML_counter = 0;
my $SEARCH = 0;
my $HREF_LINE = "";
my $The_Tagged_Path = "";
my $XML_Filenamenumber = -1;
my @lines = 0;
my $original_line;
while ($original_line = <XML>) 	#this loop is going to run until the bottom of the script at the ned of this big while loop, have a line that reads like print OUT $output.  Output will just be the line if it's supposed to be, or it will be
{
	$XML_counter++;
	my $line = $original_line;
	my $XML_junk;
	my $XML_ColumnName;
	my $XML_FileName;
	
	$line = lc($line);
	chomp($line);

	#remove the initial tabs from the line
	$line =~ s/^\s*(.*?)//;

	if ($line eq '<table name="Cabs">')
	{
		$COPY = 0;
	}

	if ($line eq '</table>')
	{
		if ($COPY == 0)
		{
			$COPY = 1;
		}
		if ($GO == 1)
		{
			$GO = 0;
		}
	}
	
	if ($line eq '<table name="file">')
	{
		#read in the first filename
		$XML_counter = 1;
		$GO = 1;
	}
	
	if ($GO and $line eq "<row>")
	{
		$XML_counter = 1;
		@lines = 0;
		$COPY = 0;
	}
	
	if ($COPY)
	{
		print NEWXML $original_line;
	}
	
	if ($GO ne 1)
	{
		next;
	}
		
	($XML_junk, $XML_ColumnName) = split(">",$line);
	if ($GO and $XML_ColumnName eq "filename</col")
	{
		$XML_Filenamenumber = $XML_counter;
		print "XML_FILENUMBER $XML_Filenamenumber\n";
		next;
	}
	
	if ($COPY)
	{
		next;
	}
	
	$lines[$XML_counter-1] = $original_line;
		
	if ($GO and substr($line, 0, 8) eq "<td href") 
	{
		$HREF_LINE = $original_line;
		next;
	}
	if ($GO and $XML_counter eq $XML_Filenamenumber)
	{		
		$XML_FileName = $XML_ColumnName;
		$XML_FileName = substr($XML_FileName,0,-4);
		$SEARCH = 1;
	}
		
	
	
	#search the tree for that file's location

	if ($SEARCH)
	{
		# change to the source dir
		chdir $SRC;
		my $HREFFILE;
		my $CHKSUM;
		my $XML_CHKSUM;
		my $FULL_FILENAME;
		($HREFFILE, $CHKSUM) = split('md5="', $HREF_LINE);
		# find the checksum of the href file
		$XML_CHKSUM = substr($CHKSUM,0,32);
		
		# get the filename
		if ($XML_FileName =~ /.*\|(.*)/i)
		{
			$XML_FileName = $1;
		}
		
		# search in the source tree for the file
		my @Tagged_Path = `"dir "$XML_FileName" /s"`;

		#This reads through the results of the search.  When it finds a line that reads "Directory of ........" it stops, and reads in that path and aborts the loop.
		my $found = 0;
		my $Tagged_Path;
		foreach $Tagged_Path (@Tagged_Path)
		{
			chomp $Tagged_Path;
			if (substr($Tagged_Path,0,14) eq "File Not Found")
			{
				last;
			}

			if (substr($Tagged_Path,0,10) eq " Directory")
			{
				my $The_Tagged_Path = substr($Tagged_Path,14);		
				#check the md5
				$FULL_FILENAME = $The_Tagged_Path . "\\" . $XML_FileName;
			
				my $FILE_CHKSUM = `md5sum $FULL_FILENAME`;
				if (substr($FILE_CHKSUM, 0, 1) eq "\\")
				{
					$FILE_CHKSUM = substr($FILE_CHKSUM,1,32);
				}
				else
				{
					$FILE_CHKSUM = substr($FILE_CHKSUM,0,32);
				}
				
				# checksum match: file found
				if ($FILE_CHKSUM eq $XML_CHKSUM)
				{
					$found = 1;
					print("found: $FULL_FILENAME\n");
					last;
					
				}
	
			}
		}
		if (!$found)
		{
			print "****file $XML_FileName is not found in the source****\n";
			for (my $i = 0; $i<@lines; $i++)
			{
				print NEWXML $lines[$i];
			}
			
			@lines = 0;
			$COPY = 1;
		}
		else
		{
			my $OLD_LINE = $HREF_LINE;
			# trim white spaces
			$FULL_FILENAME =~ s/^\s*(.*?)\s*$/$1/;
			$FULL_FILENAME = lc($FULL_FILENAME);
			$FULL_FILENAME =~ s/$DEBUG/\$(TARGET)/g;
			$FULL_FILENAME =~ s/$RELEASE/\$(TARGET)/g;
			
			my $i = index($FULL_FILENAME, $SRC);
			if ($i == 0) {
				substr($FULL_FILENAME, 0, length($SRC), '$(SRC)');
			}
			
			$HREF_LINE =~ s/(.*?href=")(.*?)(".*)/$1$FULL_FILENAME$3/;
			for (my $i = 0; $i<@lines; $i++)
			{
				if ($lines[$i] eq $OLD_LINE)
				{
					$lines[$i] = $HREF_LINE;
				}	
				print NEWXML $lines[$i];
			}
			
			@lines = 0;
			$COPY = 1;
		}
		# Reset the $SEARCH variable
		$SEARCH = 0;
	} # end of search
	
}  # end of processing XML file

close(XML);
close(NEWXML);
#delete cab's folder created during msi2xml

if (chdir("$current_dir\\Cab"))
{
	unlink <*>;
	chdir("..");
	rmdir("Cab");
}


