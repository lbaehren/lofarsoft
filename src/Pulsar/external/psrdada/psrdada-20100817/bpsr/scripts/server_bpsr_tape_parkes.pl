#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_parkes_tape.pl
#
# This script launches the parkes tape script remotely and logs messages from
# the tape script in files on the server
#

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Bpsr;           # Bpsr Module for configuration options
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;

sub usage() {
  print "Usage: ".basename($0)." PID\n";
}

#
# Sanity check to prevent multiple copies of this daemon running
#
Dada::preventDuplicateDaemon(basename($0));


#
# Constants
#
use constant DL        => 1;
use constant LOCATION  => "parkes";
use constant PIDFILE   => "bpsr_tape_".LOCATION.".pid";
use constant QUITFILE  => "bpsr_tape_".LOCATION.".quit";
use constant LOGFILE   => "bpsr_tape_".LOCATION.".log";
use constant REMOTE_USER => "pulsar";
use constant REMOTE_HOST => "shrek202";

#
# Global Variables
#
our %cfg = Bpsr::getBpsrConfig();      # Bpsr.cfg
our $quit_daemon : shared  = 0;
our $error = $cfg{"STATUS_DIR"}."/bpsr_tape_".LOCATION.".error";
our $warn  = $cfg{"STATUS_DIR"}."/bpsr_tape_".LOCATION.".warn";


# Autoflush output
$| = 1;


# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;

if ($#ARGV != 0) {
  usage();
  exit(1);
}
my $pid = @ARGV[0];


#
# Local Varaibles
#
my $logfile = $cfg{"SERVER_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
my $quitfile = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;
my $db_dir_string = uc(LOCATION)."_DB_DIR";

my $result = "";
my $response = "";
my $cmd = "";
my $user = "";
my $host = "";
my $dir = "";

# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}

# Redirect standard output and error
Dada::daemonize($logfile, $pidfile);

Dada::logMsg(0, DL, "STARTING SCRIPT");

# Start the tape script (background daemon)
($user, $host, $dir) = split(/:/, $cfg{$db_dir_string});

# Set if a daemon is running there already
Dada::logMsg(2, DL, "main: checkRemoteScript(".$user.", ".$host.", ".LOCATION.")");
($result, $response) = checkRemoteScript($user, $host, LOCATION);
Dada::logMsg(2, DL, "main: checkRemoteScript() ".$result." ".$response);

if ($result ne "ok") {
  Dada::logMsgWarn($error, "could not contact tape archiver at ".LOCATION.", quitting");
  unlink($pidfile);
  exit(1);
}

if (($result eq "ok") && ($response eq "process existed")) {
  Dada::logMsgWarn($error, "tape archiver script already running, quitting");
  unlink($pidfile);
  exit(1);
}

# Start the remote tape script

Dada::logMsg(1, DL, "Starting ".LOCATION." tape archiver");

($result, $response) = sshViaProxy($user, $host, "server_bpsr_tape_archiver.pl ".LOCATION." ".$pid);

if ($result ne "ok") {
  Dada::logMsgWarn($warn, "server_bpsr_tape_archiver.pl on ".LOCATION." return ".$response);
}

Dada::logMsg(2, DL, "Entering poll loop for quit file/SIGINT");

# Every 60 seconds, check for the existence of the script on the remote machine
my $counter_freq = 30;
my $counter = 0;
my $premature_exit = 0;

# Poll for the existence of the control file
while ((!$quit_daemon) && (! -f $quitfile) && (!$premature_exit)) {
  Dada::logMsg(3, DL, "sleeping for quitfile: ".$quitfile);
  sleep(2);

  if ($counter == $counter_freq) {
    $counter = 0;
    ($result, $response) = checkRemoteScript($user, $host, LOCATION);
    if (!(($result eq "ok") && ($response eq "process existed"))) {
      Dada::logMsgWarn($error, "remote script exited unexpectedly");
      $premature_exit = 1;
    } 
  
  } else {
    $counter++;
  } 

}

if (!$premature_exit) {

  # Now stop the tape script
  Dada::logMsg(1, DL, "Stopping ".LOCATION." tape archiver");

  ($result, $response) = sshViaProxy($user, $host, "touch \$DADA_ROOT/control/bpsr_tape_archiver.quit");

  sleep(5);

  # We need to allow lots of time for the script to stop. This could be as long
  # as it takes to write ~17 GB to tape (17 * 1024)/60 == 300 seconds

  my $remote_daemon_exited = 0;
  my $n_tries = 60;

  # Since the daemon can take quite some time to exit, we need to be leniant here
  while ((!$remote_daemon_exited) && ($n_tries > 0)) {

    ($result, $response) = checkRemoteScript($user, $host, LOCATION);

    if ($result ne "ok") {
      Dada::logMsgWarn($error, "could not contact tape archiver at ".LOCATION.", quitting");
      $remote_daemon_exited = 1;
    } else {

      if ($response eq "process existed") {
        Dada::logMsg(1, DL, "Waiting for tape archiver at ".LOCATION." to quit");
        sleep(5);
        $n_tries--;
      } else {
        $remote_daemon_exited = 1;
      }
    }
  }

  if ($n_tries == 0) {
    Dada::logMsgWarn($error, "remote tape script did not exit after 300 seconds, quitting");
  }

  # Now stop the tape script
  Dada::logMsg(2, DL, "Unlinking quitdaemon file on remote machine");
  ($result, $response) = sshViaProxy($user, $host, "unlink \$DADA_ROOT/control/bpsr_tape_archiver.quit");
}

Dada::logMsg(2, DL, "Unlinking PID file: ".$pidfile);
unlink($pidfile);

Dada::logMsg(0, DL, "STOPPING SCRIPT");

exit(0);


###############################################################################
#
# Functions
#

#
# Handle INT AND TERM signals
#
sub sigHandle($) {
                                                                                
  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(3);
  print STDERR basename($0)." : Exiting...\n";
}

#
# Checks to see if the tape archiving script is running on the remote host
#
sub checkRemoteScript($$$) {

  my ($user, $host, $type) = @_;

  my $cmd = "ps aux | grep perl | grep -v grep | grep -c server_bpsr_tape_archiver.pl";
  my $result = "";
  my $response = "";

  Dada::logMsg(2, DL, "checkRemoteScript: ".$cmd);
  ($result, $response) = sshViaProxy($user, $host, $cmd);
  Dada::logMsg(2, DL, "checkRemoteScript: ".$result." ".$response);

  if ($result ne "ok") {
    # the grep command failed (no process) and returned a line count of 0
    if ($response eq "0") {
      return ("ok", "no process existed");
    # something else in the command failed (e.g. ssh problem)
    } else {
      return ("fail", "ssh command failed: ".$response);
    }

  } else {
    return ("ok", "process existed");
  }
}

sub sshViaProxy($$$) {

  my ($user, $host, $remote_cmd) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";

  if (LOCATION eq "swin") {
    $cmd = "ssh -x -l ".$user." ".$host." 'ssh -x -l ".REMOTE_USER." ".REMOTE_HOST." \"".$remote_cmd."\"'";
  } else {
    $cmd = "ssh -x -l ".$user." ".$host." '".$remote_cmd."'";
  }

  Dada::logMsg(2, DL, "sshViaProxy: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "sshViaProxy: ".$result." ".$response);

  return ($result, $response);

}

