#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use Getopt::Std;
use Apsr;
use Dada::server_tcs_interface qw(%cfg);


Dada::preventDuplicateDaemon(basename($0));

#
# Global Variable Declarations
#
%cfg = Apsr::getConfig();

#
# Initialize module variables
#
$Dada::server_tcs_interface::dl = 1;
$Dada::server_tcs_interface::daemon_name = Dada::daemonBaseName($0);
$Dada::server_tcs_interface::tcs_cfg_file = $cfg{"CONFIG_DIR"}."/apsr_tcs.cfg";
$Dada::server_tcs_interface::tcs_spec_file = $cfg{"CONFIG_DIR"}."/apsr_tcs.spec";


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::server_tcs_interface->main();

exit($result);

