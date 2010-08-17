package Caspsr;

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);
use Dada;

require Exporter;
require AutoLoader;

@ISA = qw(Exporter AutoLoader);

@EXPORT_OK = qw(
  &getConfig
);

$VERSION = '1.00';

my $DADA_ROOT = $ENV{'DADA_ROOT'};

sub getConfig() {
  my $config_file = $DADA_ROOT."/share/caspsr.cfg";
  my %config = Dada->readCFGFileIntoHash($config_file, 0);
  return %config;
}

__END__
