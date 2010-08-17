#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Caspsr;
use Dada::client_logger qw(%cfg);


#
# Global Variable Declarations
#
%cfg = Caspsr->getConfig();


#
# Initialize module variables
#
$Dada::client_logger::dl = 2;
$Dada::client_logger::log_host = $cfg{"SERVER_HOST"};
$Dada::client_logger::log_port = $cfg{"SERVER_SYS_LOG_PORT"};
$Dada::client_logger::log_sock = 0;
$Dada::client_logger::daemon_name = Dada->daemonBaseName($0);
$Dada::client_logger::tag = "src";
$Dada::client_logger::daemon = "proc";


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_logger->main();

exit($result);
