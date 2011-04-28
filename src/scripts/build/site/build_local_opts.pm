package build_local_opts;

use strict;

print "<<" . $ENV{HOSTNAME} . ">>\n";

if ($ENV{HOSTNAME} && ($ENV{HOSTNAME} =~ /\.mit\.edu$/i) &&
    !(grep(/^MIT_ONLY=/, @ARGV))) {
    push(@ARGV, 'MIT_ONLY=1');
}

if (grep(/^MIT_ONLY=0*$/, @ARGV))
{
    @ARGV = grep (!/^MIT_ONLY=/, @ARGV);
}

$ENV{OPTDIRS_MESSAGE} = "(You may need to run graft.pl)";

1;
