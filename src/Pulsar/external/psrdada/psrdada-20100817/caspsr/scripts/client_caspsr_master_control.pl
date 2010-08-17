#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Caspsr;
use Dada::client_master_control qw(%cfg);


#
# Global Variables
# 
%cfg = Caspsr->getConfig();

#
# Initialize module variables
#
$Dada::client_master_control::dl = 1;
$Dada::client_master_control::daemon_name = Dada->daemonBaseName($0);
$Dada::client_master_control::pwc_add = " -H /home/dada/dada_headers/caspsr_test.txt";

# Determine the additional parameters for the PWC_BINARY
my $i = 0;
my $myhostname = Dada->getHostMachineName();
my $add_string = "";

for ($i=0; $i< $cfg{"NUM_PWC"}; $i++) {
  if ($myhostname eq $cfg{"PWC_".$i}) {
    $add_string = $i." ".$cfg{"NUM_PWC"}." ".$cfg{"PKTS_PER_XFER"}." 0";
  } 
}
if ($add_string ne "") {
  $Dada::client_master_control::pwc_add .= " ".$add_string;
} else {
  exit(-1);
}

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_master_control->main();

exit($result);

