#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Caspsr;
use Dada::server_gain_manager qw(%cfg);

#
# Global Variable Declarations
#
%cfg = Caspsr->getConfig();

#
# Initialize module variables
#
$Dada::server_gain_manager::dl = 2;
$Dada::server_gain_manager::hw_gain_tag = "CASPSRGAIN";
$Dada::server_gain_manager::hw_host = $cfg{"HWGAIN_HOST"};
$Dada::server_gain_manager::hw_port = $cfg{"HWGAIN_PORT"};
$Dada::server_gain_manager::daemon_name = Dada->daemonBaseName($0);

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::server_gain_manager->main();

exit($result);

