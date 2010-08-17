#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Apsr;
use Dada::server_transfer_manager qw(%cfg);

#
# Global Variable Declarations
#
%cfg = Apsr::getConfig();

sub usage() {
  print STDERR "Usage: ".$0." PID [dir]\n";
  print STDERR "  PID     Project ID to process\n";
  print STDERR "  dir     Overide default destination dir with specified\n";
}

#
# Initialize module variables
#
$Dada::server_transfer_manager::dl = 1;
$Dada::server_transfer_manager::daemon_name = Dada::daemonBaseName($0);
$Dada::server_transfer_manager::rate = 100;
$Dada::server_transfer_manager::server_logger = "server_apsr_logger.pl";
$Dada::server_transfer_manager::dest_id = 0;

# Autoflush STDOUT
$| = 1;

my $pid_arg = "";

if (!(($#ARGV == 0) || ($#ARGV == 1))) {
  usage();
  print STDERR "Only ".($#ARGV+1)." argument provided\n";
  exit(1);
}

# Get the PID from the command line args
$pid_arg = $ARGV[0];
if (!defined($cfg{$pid_arg."_DEST"})) {
  print STDERR "Specified PID [".$pid_arg."] did not have matching '".$pid_arg."_DEST' in the APSR config file\n";
  exit(1);
}

# Determine the dest_id based on the PID
if (!defined($cfg{$pid_arg."_DEST_ID"})) {
  print STDERR "Specified PID [".$pid_arg."] did not have matching '".$pid_arg."_DEST_ID' in the APSR config file\n";
  exit(1);
}

$Dada::server_transfer_manager::dest_id = $cfg{$pid_arg."_DEST_ID"};
$Dada::server_transfer_manager::pid = $pid_arg;
$Dada::server_transfer_manager::dest = $cfg{$pid_arg."_DEST"};

# Optional override of the destination dir
if ($#ARGV == 3) {
  $Dada::server_transfer_manager::optdir = $ARGV[2];
}

my $result = 0;
$result = Dada::server_transfer_manager->main();

exit($result);

