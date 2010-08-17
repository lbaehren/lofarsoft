#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Bpsr;
use Dada::server_pwc_monitor qw(%cfg);

#
# Global Variable Declarations
#
%cfg = Bpsr::getConfig();

#
# Initialize module variables
#
$Dada::server_pwc_monitor::dl = 1;
$Dada::server_pwc_monitor::daemon_name = Dada::daemonBaseName($0);
$Dada::server_pwc_monitor::log_host = $cfg{"SERVER_HOST"};
$Dada::server_pwc_monitor::log_port = $cfg{"PWCC_LOGPORT"};

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::server_pwc_monitor->main();

exit($result);

