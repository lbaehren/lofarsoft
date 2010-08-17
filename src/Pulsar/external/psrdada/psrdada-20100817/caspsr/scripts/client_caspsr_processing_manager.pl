#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Caspsr;
use Dada::client_processing_manager qw(%cfg);


#
# Global Variables
# 
%cfg = Caspsr->getConfig();

#
# Initialize module variables
#
$Dada::client_processing_manager::user = "caspsr";
$Dada::client_processing_manager::dl = 2;
$Dada::client_processing_manager::daemon_name = Dada->daemonBaseName($0);
$Dada::client_processing_manager::dada_header_cmd = "dada_header -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});
$Dada::client_processing_manager::client_logger = "client_caspsr_src_logger.pl";

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_processing_manager->main();

exit($result);

