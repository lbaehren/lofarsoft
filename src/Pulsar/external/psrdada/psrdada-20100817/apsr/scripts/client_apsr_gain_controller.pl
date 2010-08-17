#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use File::Basename;
use Apsr;
use Dada::client_gain_controller qw(%cfg);

sub usage() {
  print "Usage: ".basename($0)." chan\n";
  print "   chan   Total number of channels on this node\n";
  print "\n";
}

# Get command line
if ($#ARGV!=0) {
  usage();
  exit 1;
}

(my $chan) = @ARGV;
  

#
# Global Variables
# 
%cfg = Apsr::getConfig();

#
# Initialize module variables
#
$Dada::client_gain_controller::dl = 1;
$Dada::client_gain_controller::nchan = $chan;

if ($cfg{"USE_DFB_SIMULATOR"} == 1) {
  $Dada::client_gain_controller::gain_min = 0;
  $Dada::client_gain_controller::gain_default = 33;
  $Dada::client_gain_controller::gain_max = 100;
} else {
  $Dada::client_gain_controller::gain_min = 0;
  $Dada::client_gain_controller::gain_default = 2000;
  $Dada::client_gain_controller::gain_max = 65535;
}
$Dada::client_gain_controller::daemon_name = Dada::daemonBaseName($0);


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_gain_controller->main();

exit($result);



