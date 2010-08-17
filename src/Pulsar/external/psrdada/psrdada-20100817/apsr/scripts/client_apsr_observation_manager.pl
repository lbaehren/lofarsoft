#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Apsr;
use Dada::client_observation_manager qw(%cfg);

#
# Global Variables
# 
%cfg = Apsr::getConfig();

#
# Initialize module variables
#
$Dada::client_observation_manager::dl = 1;
$Dada::client_observation_manager::user = "apsr";
$Dada::client_observation_manager::daemon_name = Dada::daemonBaseName($0);
$Dada::client_observation_manager::dada_header_cmd = "dada_header -k ".lc($cfg{"RECEIVING_DATA_BLOCK"});
$Dada::client_observation_manager::gain_controller = "client_apsr_gain_controller.pl";
$Dada::client_observation_manager::client_logger = "client_apsr_sys_logger.pl";


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_observation_manager->main();

exit($result);

