package Logger;

use strict;
use IO::File;
use FindBin;

sub new {
    my $class = shift;
    my $file = shift;
    my $append = shift;
    $file || die "Usage: \$foo = new Logger filename [append]\n";
    my $self = {};
    bless  $self, $class;
    $self->{FILE} = $file;
    $self->{APPEND} = $append?'-a':'';
    return $self;
}

sub start {
    my $self = shift;

    return 1 if $self->{PIPE};

    STDOUT->flush;
    STDERR->flush;

    my $fh_out = new IO::File;
    my $fh_err = new IO::File;
    my $fh_pipe = new IO::File;

    $self->{OUT} = $fh_out;
    $self->{ERR} = $fh_err;
    $self->{PIPE} = $fh_pipe;

    $fh_out->open(">&STDOUT") || die;
    $fh_err->open(">&STDERR") || die;
    $fh_pipe->open("|$^X $FindBin::Bin/tee.pl $self->{APPEND} $self->{FILE}") || die;

    STDOUT->fdopen(fileno $fh_pipe, "w") || die;
    STDERR->fdopen(fileno $fh_pipe, "w") || die;

    STDOUT->autoflush(1);
    STDERR->autoflush(1);

    $SIG{__DIE__} = sub {
	print STDERR $_[0]; 
	$self->stop;
	die "\n";
    };


    return 1;
}

sub stop {
    my $self = shift;

    return 0 if !$self->{PIPE};

    STDOUT->close;
    STDERR->close;
    $self->{PIPE}->close;
    STDOUT->fdopen(fileno $self->{OUT}, "w");
    STDERR->fdopen(fileno $self->{ERR}, "w");
    delete $self->{OUT};
    delete $self->{ERR};
    delete $self->{PIPE};
    return 1;
}

sub DESTROY {
}

1;
