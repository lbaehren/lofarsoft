#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Apsr;
use Dada::client_background_processor qw(%cfg);


#
# Global Variables
# 
%cfg = Apsr::getConfig();

#
# Initialize module variables
#
$Dada::client_background_processor::dl = 1;
$Dada::client_background_processor::daemon_name = Dada::daemonBaseName($0);
$Dada::client_background_processor::user = "apsr";

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_background_processor->main();

exit($result);

