#!perl -w

#
# fixmak15.pl
#
# This script takes a MSVC 1.5 Makefile in stdin and processes it
# so that it builds its targets into different debug and release
# directories (WinDebug and WinRelease).  It also does its best to
# remove any hardcoded path dependencies that will cause the makefile.
# In the process, it tosses out all dependencies for .c and .rc files
# because MSVC 1.5 is dumb and keeps hardcoded paths in there.
# 
# Sample usage: perl fixmak15.pl < foo.mak > newfoo.mak
#
# This script is indempotent -- i.e., you can run it over its own output.
#

use strict;
use Cwd;

# These need to be less than or equal 8 characters in length
my $DEBUG_INTDIR = 'WinDebug';
my $DEBUG_OUTDIR = 'WinDebug';
my $RELEASE_INTDIR = 'WinRel';
my $RELEASE_OUTDIR = 'WinRel';

my $have_dirs = 0;
my $in_dep = 0;

# This is so that we can eventually do our own dependency scanning:
my @INCLUDES = ('.', cwd, split(/;/, $ENV{INCLUDE}));

while (<>) {
    # Fix up stuff with intermediate directories
    s/(^|[^\w\\])(\w+\.OBJ)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;
    s/(^|[^\w\\])(\w+\.SBR)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;
    s/(^|[^\w\\])(\w+\.RES)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;
    s/(^|[^\w\\])(\w+\.BND)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;
    s/(^|[^\w\\])(\w+\.PDB)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;
    s/(^|[^\w\\])(\w+\.PCH)\b/$1.'$(INTDIR)\\'.lc($2)/ieg;

    # collect include dir info
    if (/^CFLAGS/) {
        my $line = $_;
        while ($line =~ /\/I\s*\"([^\"]+)\"/) {
            push(@INCLUDES, $1);
            $line = $';
        }
    }

    # blow away deps
    if ((!$in_dep && /^\s*\w+_(RC)?DEP\s*=\s*(.+)\b\s*(\\)?\s*$/) ||
        ($in_dep && /^\s*()(.+)\b\s*(\\)?\s*$/)) {
        $in_dep = $3;
        $_ = '';
    } elsif ($in_dep) {
        $in_dep = 0;
    }

    # if we ever want to lowercase filenames for the heck of it...
#    s/\b(\w+\.C)\b/lc($1)/ieg;
#    s/\b(\w+\.CPP)\b/lc($1)/ieg;
#    s/\b(\w+\.CXX)\b/lc($1)/ieg;
#    s/\b(\w+\.CC)\b/lc($1)/ieg;
#    s/\b(\w+\.H)\b/lc($1)/ieg;
#    s/\b(\w+\.HPP)\b/lc($1)/ieg;
#    s/\b(\w+\.HXX)\b/lc($1)/ieg;
#    s/\b(\w+\.DEF)\b/lc($1)/ieg;
#    s/\b(\w+\.RC)\b/lc($1)/ieg;

    # Fix up stuff with output directories
    s/(^|[^\w\\])(\$\(PROJ\)\.)(\w+)/$1.'$(OUTDIR)\\'.$2.lc($3)/ieg;

    # lowercase project name
    goto end if s/^(PROJ\s*=\s*)(\w+)$/$1.lc($2)/e;

    # put all output into the right output directory
    goto end if s/(PCHFLAG\)) (\/c)/$1 \/Fo\$\@ \/FR\$\(\@R\)\.sbr $2/;
    goto end if s/(\$\(RCDEFINES\)) (-r)/$1 -fo \$\@ $2/;

    # if we have not put in the intermediate/output directory stuff in,
    # put it in
    $have_dirs = 1 if /^INTDIR/;
    if (!$have_dirs && /^PROGTYPE\s*=\s*\S+/) {
        $_ = '!if "$(DEBUG)" == "1"
INTDIR='.$DEBUG_INTDIR.'
OUTDIR='.$DEBUG_OUTDIR.'
!else
INTDIR='.$RELEASE_INTDIR.'
OUTDIR='.$RELEASE_OUTDIR.'
!endif

!if [if not exist $(INTDIR) md $(INTDIR)]
!endif
!if [if not exist $(OUTDIR) md $(OUTDIR)]
!endif

'.$_;
        goto end;
    }

  end:
    print $_;
}

#print join('|', @INCLUDES);
