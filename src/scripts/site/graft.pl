#!perl -w

use strict;
use FindBin;
use File::Spec;
use File::Basename;
use File::Copy;
use IO::File;
use File::Path;
use File::Find;
use File::stat;
use Cwd;

$0 = fileparse($0);

use Getopt::Long;

use Safe;
my $SAFE = new Safe;
${$SAFE->varglob('ENV')}{COMPUTERNAME} = $ENV{COMPUTERNAME};
my $M = $SAFE->rdo(File::Spec->catfile($FindBin::Bin, "graft.conf"));

my $DONE = {};
my $TOP = File::Spec->catfile($FindBin::Bin, '..', '..');
my $FIND_OK = 0;
my $NO_RMTREE_CLEANUP = 0;


sub svn_try
{
    my $k = shift;
    my $rev = shift;
    my $svnroot = $M->{$k}->{svn};

    return 0 if !$svnroot; 

    my $cmd = "svn checkout $svnroot/$rev $k";
    print "Trying: $cmd\n";
    my $error = 0;
    if (system($cmd) / 256) {
	$error = 1;
    }
	
    if ($error && !$NO_RMTREE_CLEANUP) {
	print "Error detected!  Removing $k\n";
	rmtree($k);
	#die "Could not successfully get $k\n";
    }
    return !$error;

}


sub cvs_try
{
    my $k = shift;
    my $rev = shift;
    my $type = shift;
    my $u = shift;

    my $cvsroot = $M->{$k}->{$type};
    my $u_gopt = $u ? ($M->{$k}->{u} ? '-u ' : '') : '';
    my $n_lopt = $M->{$k}->{n} ? '-n ' : '';

# Asanka: used for password file change. (Some Windows cow prevents
#         password specification on -d.
	my $old_Env;

    $rev = $rev?"-r $rev":'';
    

    return 0 if !$cvsroot || ($u && !$u_gopt);

	if ($M->{$k}->{passfile}) {
		$old_Env = $ENV{'CVS_PASSFILE'};
		$ENV{'CVS_PASSFILE'} = $TOP.'/'.$M->{$k}->{passfile};
	}

    my $cmd = "cvs ".$u_gopt."-d $cvsroot co ".$n_lopt."-P $rev $k";
    print "Trying: $cmd\n";
    my $error = 0;
    if (system($cmd) / 256) {
	$error = 1;
    }
    if ($u_gopt && !cvs_linkage($k, 0)) {
	$error = 1;
    }

	if ($M->{$k}->{passfile} && $old_Env) {
		$ENV{'CVS_PASSFILE'} = $old_Env;
	}

    if ($error && !$NO_RMTREE_CLEANUP) {
	print "Error detected!  Removing $k\n";
	rmtree($k);
	#die "Could not successfully get $k\n";
    }
    return !$error;
}

sub graft
{
    my $k = shift;
    my $rev = shift;
    my $noorig = shift;
    my $nodefault = shift;

    my $vstudiover = 6;
    if (defined($ENV{"MSVCDIR"}) && $ENV{"MSVCDIR"} =~ m/visual studio .net/i){
		$vstudiover = 7;
	}

    return if $DONE->{$k};

    if ($vstudiover == 6 && $M->{$k}->{"skipifvs6"})
    {
		return;
		}
	

    my $dir = File::Spec->catfile($TOP, $M->{$k}->{in});
    my $requires = $M->{$k}->{requires};

    if ($requires) {
	foreach my $r (@$requires) {
	    graft($r, $rev, $noorig, $nodefault);
	}
    }

    chdir($dir) || die "$0: Could not chdir to $dir for $k\n";
	if (-e $k) {
	print "$k already exists...";
	if (-d $k) {
	    print "skipping\n";
	    $DONE->{$k} = 1;
	    return $DONE->{$k};
	}
	print "and it is not a directory!\n";
	die "$0: Found $k as file when expecting a directory\n";
    }
    $rev = $rev?$rev:0;

    if (!$rev && !$nodefault && defined($ENV{"KRB5BRANCH"}) && $ENV{"KRB5BRANCH"} =~ m/TAGGED/i)
    {
		if ($vstudiover == 7){
			if ($M->{$k}->{"default-if-tagged"}) {
				$rev = $M->{$k}->{"default-if-tagged"};
			}
		}
		else
		{
			if ($M->{$k}->{"vc6default-if-tagged"}) {
				$rev = $M->{$k}->{"vc6-default-if-tagged"};
			}
		}
	
		
				
	print "Environment variable KRB5BRANCH=\"".$ENV{"KRB5BRANCH"}."\":\n"; 
	print "  Using the \"default-if-tagged\" revision, \"$rev\".\n";
    }

    if (!$rev && !$nodefault){
		if ($vstudiover == 7){
			if ($M->{$k}->{default}) {
				$rev = $M->{$k}->{default};
			}
		}
		else {
			if ($M->{$k}->{vc6default}) {
				$rev = $M->{$k}->{vc6default};
			}
		}
	}
	
    if ($M->{$k}->{svn})
    {	
	$DONE->{$k} =
		svn_try($k, $rev);
    }	
    else{
    	if (!$noorig) {
		$DONE->{$k} =
	  	cvs_try($k, $rev, 'original', 0) ||
	    	cvs_try($k, $rev, 'original', 1);
    	}
    	$DONE->{$k} = $DONE->{$k} ||
      		cvs_try($k, $rev, 'cvs', 0) ||
		cvs_try($k, $rev, 'cvs', 1);
   }
    if ($DONE->{$k}) {
	$DONE->{$k} = update($k, 0);
	rmtree($k) if !$DONE->{$k} && !$NO_RMTREE_CLEANUP;
    }

    die "Could not successfully get $k\n" if (!$DONE->{$k});

   
	if ($M->{$k}->{"update-installer"}) {
		$dir = File::Spec->catfile($TOP, $M->{$k}->{"update-installer"});
		chdir($dir) || die "$0: Could not chdir to $dir for installer $M->{$k}->{installer}\n";
		print "cvs update -d -P -r $rev $M->{$k}->{installer} \n";
		if (system("cvs update -d -P -r $rev $M->{$k}->{installer}")/256)
		{
			die "could not update $M->{$k}->{installer}";
		}


	}
    return $DONE->{$k};
}

sub get_modules_from_args
{
    my $OPT = shift;
    my @args = @_;
    my @m_all = $OPT->{all} ? sort keys %$M : @args;
    my @m = ();
    foreach my $m (@m_all) {
	push(@m, $m) if !($OPT->{all} && $M->{$m}->{hide});
    }
    return @m;
}

sub update
{
    my $k = shift;
    my $force = shift;
    my $n = shift;

    print "Updating $k...";
    my $dir = File::Spec->catfile($FindBin::Bin, '..', '..', 
				  $M->{$k}->{in});
    my $from_dir = File::Spec->catfile($FindBin::Bin, 'graft', $k);
    my $to_dir = File::Spec->catfile($dir, $k);
    if (! -d $to_dir) {
	print "$k does not exist\n";
	return 0;
    }
    print "\n";
    if (-d $from_dir) {
	find(gen_find_func($from_dir, $to_dir, $force, $n), $from_dir);
	if (!$FIND_OK) {
	    return 0;
	}
    }
    return 1;
}

sub svn_try_update
{
    my $k = shift;
    
    my $cmd =  "svn update ";
    print "Trying: $cmd\n";
    my $error = system($cmd) / 256;
    
    return !$error;
}


sub cvs_try_update
{
    my $k = shift;
    my $u = shift;
    my $n = shift;
    my $q = shift;

    my $u_gopt = $u ? ($M->{$k}->{u} ? '-u ' : '') : '';
    my $n_gopt = $n ? '-n ' : '';
    my $q_gopt = $q ? '-q ' : '';

    my $d_lopt = $n ? '' : 'd';

    return 0 if ($u && !$u_gopt);

    my $moved;
    my $_CVS = '_CVS';
    my $CVS = 'CVS';

    if (!(-d $CVS) && (-d $_CVS)) {
	move($_CVS, $CVS) || die "$0: Could not rename $_CVS to $CVS\n";
	$moved = 1;
    }
    if (!(-d $CVS)) {
	die "Could not find CVS state\n";
    }
	my $old_Env;

   if ($M->{$k}->{passfile}) {
		$old_Env = $ENV{'CVS_PASSFILE'};
		$ENV{'CVS_PASSFILE'} = $TOP.'/'.$M->{$k}->{passfile};
	}

    my $cmd = "cvs $u_gopt$n_gopt$q_gopt up -".$d_lopt."P";
    print "Trying: $cmd\n";
    my $error = system($cmd) / 256;

	if ($M->{$k}->{passfile} && $old_Env) {
		$ENV{'CVS_PASSFILE'} = $old_Env;
	}

    if ($moved) {
	move($CVS, $_CVS) || die "$0: Could not rename $CVS to $_CVS\n";
    }
    return !$error;
}

sub cvs_update
{
    my $k = shift;
    my $n = shift;
    my $q = shift;
    my $cwd = cwd();

    print "CVS Updating $k...\n";
    my $dir = File::Spec->catfile($FindBin::Bin, '..', '..', 
				  $M->{$k}->{in}, $k);
    print "In dir: $dir\n";
    chdir($dir) || die;
    if ($M->{$k}->{svn})
    {
	svn_try_update($k);
    }
    else
    {
    	cvs_try_update($k, 0, $n , $q) || cvs_try_update($k, 1, $n, $q);
    }
	
    chdir($cwd) || die;
}

sub min {
    my $a = shift;
    my $b = shift;
    return ($a < $b)?$a:$b;
}

sub partial
{
    my $base = shift;
    my $full = shift;
    return substr($full, min(length($base)+1, length($full))) || '.';
}

sub gen_file_list_func
{
    my $basedir = shift;
    my $ref = shift;
    print "Indexing $basedir...\n";
    return sub {
	my $name = $_;
	if (($name eq 'CVS') || ($name =~ /~$/)) {
	    $File::Find::prune = 1;
	    return;
	}
	my $dir = $File::Find::dir;
	my $full = $File::Find::name;
	my $part = partial($basedir, $full);
	if (-f $full) {
	    $ref->{$part} = 1;
	}
    };
}

sub gen_find_func
{
    my $from_dir = shift;
    my $to_dir = shift;
    my $force = shift;
    my $n = shift;
    $FIND_OK = 1;
    return sub {
	my $name = $_;
	if (($name eq 'CVS') || ($name =~ /~$/)) {
	    $File::Find::prune = 1;
	    return;
	}
	my $dir = $File::Find::dir;
	my $full = $File::Find::name;
	my $part = partial($from_dir, $full);
	if (-f $full) {
	    my $to_file = File::Spec->catfile($to_dir, $part);
	    my $ok = 1;
	    my $st_from = stat($full);
	    my $st_to = stat($to_file);
	    my $skip_copy = 0;
	    if ($st_from && $st_to) {
		if ($st_to->mtime > $st_from->mtime) {
		    print "$part has been modified in working directory\n";
		    $skip_copy = 1;
		} elsif ($st_to->mtime == $st_from->mtime) {
		    $skip_copy = 1;
		}
	    }
	    if (!$n && !$skip_copy) {
		print "Copy $part...";
		if (!$force && -f $to_file) {
		    $ok = 0;
		    print "ERROR: FILE ALREADY EXISTS!";
		} else {
		    $ok = copy($full, $to_file);
		    utime($st_from->atime, $st_from->mtime, $to_file) if $ok;
		    print $ok?"OK":"ERROR";
		}
		print "\n";
	    } elsif (!$skip_copy) {
		print "$part needs an update\n";
	    }
	    $FIND_OK = 0 if !$ok;
	}
    };
}

sub uniq
{
    my %h;
    my @ret;
    foreach $_ (@_) {
	if (!$h{$_}) {
	    $h{$_} = 1;
	    push(@ret, $_);
	}
    }
    return @ret;
}

sub check_args
{
    my $OPT = shift;
    my $check_all = shift;
    my $check_multi = shift;
    my $check_single = shift;
    my $m_extra = shift;
    my $m = shift;

    return 0 if $OPT->{all} && $m;
    my $found = 0;
    foreach my $e (uniq(@$check_all, @$check_multi, @$check_single)) {
	if ($OPT->{$e}) {
	    $found = 1;
	    last;
	}
    }
    return 0 if !($found || $OPT->{all} || $m);
    $found = $OPT->{all};
    foreach my $e (uniq(@$check_multi, @$check_single)) {
	if ($OPT->{$e}) {
	    return 0 if $found;
	    $found = 1;
	}
	return 0 if ($OPT->{$e} && !$m);
    }
    return 0 if !($found || $m);
    foreach my $e (@$check_single) {
	return 0 if $OPT->{$e} && @$m_extra;
    }
    return 1;
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

sub do_import
{
    my $m = shift;
    my $tag = shift;
    my $skip0 = shift;
    my $skip1 = shift;
    my $skip2 = shift;
    my $skip_on;
    my $cwd = cwd() || die;
    my $ok;
    my $partial;
    my $death;

    my $rev = $tag || 0;
    $tag = $tag || 'current';

    my $temp1;
    my $temp2;
    my $cvs1;
    my $cvs2;

    if ($skip1 && $skip2) {
	die "Invalid dirs $skip1 $skip2\n" if !(-d $skip1 && -d $skip2);
	$cvs1 = $temp1 = $skip1;
	$cvs2 = $temp2 = $skip2;
	$skip_on = 1;
	goto skip;
    } else {
	$temp1 = mktmpdir();
	$temp2 = mktmpdir();
    }

    $cvs1 = File::Spec->catfile($temp1, $m) || die;
    $cvs2 = File::Spec->catfile($temp2, $m) || die;

    chdir($temp1) ||
      ($death = "Cannot chdir to $temp1" && goto cleanup);
    cvs_try($m, $rev, 'original', 0) || 
      cvs_try($m, $rev, 'original', 1) ||
	($death = "Cannot cvs" && goto cleanup);
    chdir($temp2) ||
      ($death = "Cannot chdir to $temp2" && goto cleanup);
    cvs_try($m, 0, 'cvs', 0) || 
      cvs_try($m, 0, 'cvs', 1) ||
	($death = "Cannot cvs" && goto cleanup);
  skip:
    my $l1 = {};
    my $l2 = {};
    my $lr = {};
    my $remove;
    find(gen_file_list_func($cvs1, $l1), $cvs1);
    find(gen_file_list_func($cvs2, $l2), $cvs2);
    map { $remove = $lr->{$_} = 1 if !$l1->{$_}; } sort keys %$l2;
    print "Removal list is ".($remove?"as follows:":"empty")."\n";
    map { print "\t$_\n"; } sort keys %$lr;
    my $cvsroot = $M->{$m}->{cvs};
    my $wrappers = $M->{$m}->{wrappers};
    chdir($cvs1) ||
      ($death = "Cannot chdir to $cvs1" && goto cleanup);
    # NOTE: We cannot use -d because there is some cvs Windows cow with it.
    my $cmd = "cvs -d $cvsroot import -m \"Import of $m $tag\" $wrappers $m $m $tag";
    print "Import dir: ",cwd(),"\n";
    print "Import command: $cmd\n";
    $partial = 1;
    if (!$skip_on && !$skip0) {
	if (system($cmd) / 256) {
	    print "Error on cvs import step of import\n";
	    goto cleanup;
	}
    }
    if ($remove) {
	print "Removing old files...\n";
	chdir($cvs2) ||
	  ($death = "Cannot chdir to $cvs2" && goto cleanup);
	my @cmds;
	map { push(@cmds, "cvs remove -f $_"); } sort keys %$lr;
	push(@cmds, "cvs commit -m \"Remove for $m $tag\"");
	print "In dir: ",cwd(),"\n";
	foreach my $cmd (@cmds) {
	    print "Executing $cmd\n";
	    if (!$skip_on && !$skip0) {
		if (system($cmd) / 256) {
		    print "Error on cvs remove/commit step of import\n";
		    goto cleanup;
		}
	    }
	}
    }
    $partial = 0;
    $ok = 1;
  cleanup:
    if (!$ok) {
	print "An error occurred while trying to import $m\n";
    }
    if ($partial) {
	print "Things were partially imported!!!\n";
	print "*"x3," This is ", "*REALLY*"x3, "bad! ", "*"x3;
	print "Let people know ASAP!\n";
    }
    chdir($cwd);
    if (!$skip_on && !$skip0) {
	print "Cleaning up temporary files...\n";
	rmtree($temp1);
	rmtree($temp2);
    }
    if ($death) {
	die $death,"\n";
    }
    # co original -> temp1
    # co cvs -> temp2
    # import temp1 -> cvs
    # generate file list for both
    # remove files in temp2 but not in temp1 from cvs
}

sub main
{
    my $OPT = {};
    Getopt::Long::Configure('bundling');
    if (!GetOptions($OPT, 
		    'help|H|h|?', 
		    'tag|r:s',
		    'noorig',
		    'all|a',
		    'graft',
		    'link',
		    'unlink',
		    'import',
		    'update',
		    'remove',
		    'nowrite|n',
		    'quiet|q',
		    'nodefault|N',
		    'skipcvs|s',
		    'skip0',
		    'skip1:s',
		    'skip2:s',
		    'no_rmtree_cleanup',
		   )) {
	usage();
	die "\n";
    }

    if ($OPT->{no_rmtree_cleanup}) {
	$NO_RMTREE_CLEANUP = 1;
    }

    my $m = shift @ARGV;
    my @m_extra = @ARGV;
    unshift(@ARGV, $m) if $m;

    my $ok = 1;
    foreach my $e (@ARGV) {
	if (!$M->{$e}) {
	    $ok = 0;
	    last;
	}
    }

    if ($OPT->{help} || !$ok ||
	!check_args($OPT, 
		    ['graft', 'update', 'link', 'unlink', 'remove'],
		    [], 
		    ['import'],
		    \@m_extra, $m))
    {
	usage();
	return;
    }

    my @m = get_modules_from_args($OPT, @ARGV);

    if ($OPT->{import}) {
	if (!$M->{$m}->{original}) {
	    print "$m is not importable\n";
	} else {
	    do_import($m, $OPT->{tag} || 0, $OPT->{skip0} || 0,
		      $OPT->{skip1} || 0, $OPT->{skip2} || 0);
	}
    } elsif ($OPT->{update}) {
	foreach $m (@m) {
	    my $dir = File::Spec->catfile($FindBin::Bin, '..', '..', 
					  $M->{$m}->{in}, $m);
	    if (-d $dir) {
		cvs_update($m, $OPT->{nowrite} || 0, $OPT->{quiet} || 0)
		    if !$OPT->{skipcvs};
		update($m, 1, $OPT->{nowrite} || 0);
	    } else {
		print "WARNING: $m has not been grafted -- skipping update\n";
	    }
	}
    } elsif ($OPT->{link}) {
	print "Not implemented\n";
    } elsif ($OPT->{unlink}) {
	print "Not implemented\n";
    } elsif ($OPT->{remove}) {
	foreach $m (@m) {
	    my $dir = File::Spec->catfile($TOP, $M->{$m}->{in}, $m);
	    if (-d $dir) {
		print "Removing $m...";
		print rmtree($m)?"OK":"ERROR";
	    } else {
		print "$m is already gone";
	    }
	    print "\n";
	}
    } else {
	# graft...
	foreach $m (@m) {
	    graft($m, $OPT->{tag} || 0, $OPT->{noorig}, $OPT->{nodefault});
	}
    }
}

sub usage
{
    print <<USAGE;
Usage: $0 [options] [command] [modules]

  Regular Options:
    -h, -H, --help   help
    -r tag           tag to get
    --noorig         do not graft from original location
    --all, -a        do all modules (don't specify modules in command line)
    --skipcvs, -s    skips cvs portion of update (applies only to --update)
    --nowrite, -n    like -n in cvs (applies only to --update)
    --quiet, -q      like -q in cvs (applies only to --update)
    --nodefault, -N  do not get implicit default tag (applies to --graft)
    --no_rmtree_cleanup  Do not rmtree to cleanup (temp fix for tomt's cvs bug)

  Commands that take one or more modules (or --all):
    --graft          graft modules (default)
    --update         update modules
    --link           link an unlinked module (not implemented)
    --unlink         unlink a linked module (not implemented)
    --remove         remove module (USE CAUTIOUSLY!)

  Commands that take exactly one module:
    --import         import a module (USE CAUTIOSLY!)

  Valid Modules:
USAGE
    foreach my $m (sort keys %$M) {
	print "    $m", $M->{$m}->{hide}?" (hidden)":"",
	  $M->{$m}->{default}?" (default tag: $M->{$m}->{default})":"","\n";
    }
}

sub cvs_linkage
{
    my $k = shift || die;
    my $link = shift;

    my $from = $link?'_CVS':'CVS';
    my $to   = $link?'CVS':'_CVS';

    my $dir = File::Spec->catfile($FindBin::Bin, '..', '..', 
				  $M->{$k}->{in});

    chdir($dir) || die "$0: Could not chdir to $dir for $k\n";
    do_fix_cvs_Entries($k) || die "$0: Could not fix CVS/Entries\n";
    $dir = File::Spec->catfile($dir, $k);
    chdir($dir) || die "$0: Could not chdir to $dir for $k\n";
    move($from, $to) || die "$0: Could not rename CVS to _CVS\n";
}

sub do_fix_cvs_Entries
{
    my $MODULE = shift || die;

    sub perror { 
	my $msg = shift;
	print "fix_cvs_Entries: $msg\n";
	return 0;
    };

    my $name = 'CVS/Entries';
    my $name_tmp = $name.'.tmp';
    my $name_bak = $name.'.bak';

    my $fi = new IO::File "<$name";
    my $fo = new IO::File ">$name_tmp";

    return perror("Could not open $name for input") if (!defined $fi);
    return perror("Could not open $name_tmp for output") if (!defined $fo);

    while (<$fi>) {
	print $fo $_ if (!/^D\/$MODULE\/\//);
    }

    undef $fi;
    undef $fo;

    return perror("Could not copy $name to $name_bak")
      if !copy($name, $name_bak);
    return perror("Could not move $name_tmp to $name")
      if !rename($name_tmp, $name);
    return 1;
}

main();
