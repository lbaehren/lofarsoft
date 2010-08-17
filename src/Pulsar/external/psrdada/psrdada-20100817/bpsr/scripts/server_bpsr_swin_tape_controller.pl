#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Bpsr;
use Dada::server_tape_controller qw(%cfg);

#
# Global Variable Declarations
#
%cfg = Bpsr::getConfig();

sub usage() {
  print STDERR "Usage: ".$0." PID\n";
  print STDERR "  PID   Project ID to process\n";
}

#
# Initialize module variables
#
$Dada::server_tape_controller::dl = 1;
$Dada::server_tape_controller::daemon_name = Dada::daemonBaseName($0);
$Dada::server_tape_controller::pid_report_port = $cfg{"SERVER_SWINTAPE_PID_PORT"};
$Dada::server_tape_controller::db_dir = $cfg{"SWIN_DB_DIR"};
$Dada::server_tape_controller::dest_script = "server_bpsr_swin_tape_archiver.pl";
$Dada::server_tape_controller::dest_host = "shrek201";
$Dada::server_tape_controller::dest_user = "pulsar";

# Autoflush STDOUT
$| = 1;

if ($#ARGV != 0) {
  usage();
  exit(1);
}

$Dada::server_tape_controller::pid = $ARGV[0];

my $result = 0;

$result = Dada::server_tape_controller->main();

exit($result);

