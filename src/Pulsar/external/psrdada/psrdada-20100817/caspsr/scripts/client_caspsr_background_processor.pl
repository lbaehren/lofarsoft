#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Caspsr;
use Dada::client_background_processor qw(%cfg);


#
# Global Variables
# 
%cfg = Caspsr->getConfig();

#
# Initialize module variables
#
$Dada::client_background_processor::dl = 2;
$Dada::client_background_processor::daemon_name = Dada->daemonBaseName($0);
$Dada::client_background_processor::user = "caspsr";

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_background_processor->main();

exit($result);

