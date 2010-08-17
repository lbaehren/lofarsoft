#!/usr/bin/env perl

#
# Author:   Andrew Jameson
# Created:  ? Dec, 2007
# Modified: 4 Jan, 2008
#
# This daemons runs continuously and reports datablock and udp packet
# statistics to ganglia
#

use lib $ENV{"DADA_ROOT"}."/bin";

#
# Include Modules
#

use IO::Select;     # Allow select command
use threads;        # For posix style threads
use threads::shared;
use strict;         # strict mode (like -Wall)
use File::Basename;
use Apsr;           # APSR/DADA Module for configuration options

#
# Constants
#
use constant DEBUG_LEVEL      => 0;
use constant SLEEP_TIME       => 1;
use constant PIDFILE          => "apsr_monitor.pid";
use constant LOGFILE          => "apsr_monitor.log";


#
# Global Variables
#
our $log_socket = 0;
our $quit_daemon : shared  = 0;
our %cfg : shared = Apsr::getApsrConfig();      # Apsr.cfg in a hash


#
# Local Variables
#
my $logfile = $cfg{"CLIENT_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"CLIENT_CONTROL_DIR"}."/".PIDFILE;
my $bindir = Dada::getCurrentBinaryVersion();
my $cmd = $bindir."/dada_dbmetric -k eada";
my $daemon_quit_file = Dada::getDaemonControlFile($cfg{"CLIENT_CONTROL_DIR"});


#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

# Turn the script into a daemon
Dada::daemonize($logfile, $pidfile);

# Autoflush output
$| = 1;

# Open a logging connection to the Nexus
$log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SYS_LOG_PORT"});
if (!$log_socket) {
  print "Could not open a connection to the nexus SYS log: $log_socket\n";
}

logMessage(0, "INFO", "STARTING SCRIPT");


# Datablock monitoring thread
my $db_thread = threads->new(\&db_thread, $cmd);
logMessage(2, "INFO", "Datablock monitoring thread started");

# UDP packet monitoring thread 
my $udp_thread = threads->new(\&udp_thread);
logMessage(2, "INFO", "udp stats thread started");

# Let the threads do their thing, while we wait for a quit signal
while (!-f ($daemon_quit_file)) {
  sleep (1);
} 

$quit_daemon = 1;

logMessage(1, "INFO", "Waiting for threads to exit");
# The threads are waiting for the same signal, 
my $db_result = $db_thread->join;
logMessage(1, "INFO", "db thread joined");
my $udp_result = $udp_thread->join;
logMessage(1, "INFO", "udp thread joined");

unlink($pidfile);
logMessage(1, "INFO", "pid file unlinked");

logMessage(0, "INFO", "STOPPING SCRIPT");

close($log_socket);

exit(0);


#
# Thread to listen to apsr_udpdb for stats on packet loss
#
sub udp_thread() {

  # Connect to apsr_udpdb running on this machine.
  my $localhost = Dada::getHostMachineName();
  my $port = $cfg{"CLIENT_UDPDB_STATS_PORT"};
  my $threadQuit = "false";

  my $last_report = time;
  my $gmetric_cmd = "";

  while ($threadQuit eq "false") {

    logMessage(2, "INFO", "udpthread: Trying to connect to udpdb stats ".$localhost.":".$port);
    my $handle = Dada::connectToMachine($localhost,$port); # udpdb stats port

    if (!$handle) {

      # This occurs if apsr_udpdb is not currently running
      logMessage(2, "INFO", "udp_thread: Could not connect to udpdb stats ".$localhost.":".$port);

    } else {

      logMessage(2, "INFO", "udp_thread: Connected to udpdb stats ".$localhost.":".$port);

      # now we wait on data. we select on the socket with a 1 second sleep
      # since apsr_udpdb will not provide packet loss information if it
      # is not receiving data.

      # Add the active connection to apsr_udpdb
      my $read_set = new IO::Select();
      $read_set->add($handle);

      my $packets_lost = 0;
      my $buf = "";
      my $rh;

      while (!($quit_daemon)) {

        logMessage(2, "INFO", "udp_thread: checking for thread quit");

        # Select/sleep for 1 second waiting for data from apsr_udpdb
        my ($readable_handles) = IO::Select->select($read_set, undef, undef, 1);

        # nb there should only be 1 handle here...
        foreach $rh (@$readable_handles) {

          if ($rh == $handle) {

            # we have recieved some data on our socket
            $buf = $rh->getline();

            if (! defined $buf) {
              logMessage(1, "INFO", "apsr_udpdb has closed the connection");
              $read_set->remove($rh);
              close($rh);
            } else {
              chomp($buf);
              $packets_lost += int($buf);
              logMessage(2, "INFO", "pkts lost = ".$packets_lost);

            }

          } else {

            # we have a new connection on this socket - this should happent
            logMessage(0, "ERROR", "udp_thread received a new connection unexpectedly");

          }
        } 

        # Report the udp packets lost every 5 seconds
        if (time > ($last_report+5)) {

          $last_report = time;
          $gmetric_cmd = "gmetric --type int16 --name udp_packets_lost --value ".$packets_lost."";
         `$gmetric_cmd`;
          logMessage(1, "INFO", "gmetric \"udp_packets_lost\" == \"".$packets_lost."\"");

        }

      }
    
      # If we have for some reason lost connection with apsr_udpdb, close and 
      # we will try to reconnect
      logMessage(1, "INFO", "udp_thread: asked to quit");
      $handle->close();

    }

    # Check to see if we have been asked to quit...
    if ($quit_daemon) {
      $threadQuit = "true";
    } else {
      sleep(SLEEP_TIME);
    }

  }
  logMessage(1, "INFO", "udp thread returning ok");
  
  return "ok";
}


#
# Thread to poll the data block for information on usage
#
sub db_thread($) {

  (my $cmd) = @_;

  my $threadQuit = "false";
  my $gmetric_cmd;

  my $last_report = time;

  while ($threadQuit eq "false") {

    # If 5 seconds since our last report of data
    if (time > ($last_report+5)) {

      $last_report = time;

      my $response = `$cmd 2>&1`;
      my $rval = $?;

      if ($rval != 0) {

        # Check if the datablock simply was not created 
        if ($response =~ /ipcsync_get: ipc_alloc error/) {
          logMessage(2, "WARN", "Data block did not exist");
        } else {
        
          chomp($response);
          logMessage(0, "WARN", "Command $cmd failed \"".$response."\"");
        }

        if (!$quit_daemon) {
          sleep(3);
        }

      } else {

        my @values = split(",",$response);

        if (DEBUG_LEVEL >= 3) {
          print "Header Block:\n";
          print "  Total sub blocks: ".@values[5]."\n";
          print "  Full blocks:      ".@values[6]."\n";
          print "  Cleared blocks:   ".@values[7]."\n";
          print "  sync->w_buf:      ".@values[8]."\n";
          print "  sync->r_buf:      ".@values[9]."\n";
          print "Data Block:\n";
          print "  Total sub blocks: ".@values[0]."\n";
          print "  Full blocks:      ".@values[1]."\n";
          print "  Cleared blocks:   ".@values[2]."\n";
          print "  sync->w_buf:      ".@values[3]."\n";
          print "  sync->r_buf:      ".@values[4]."\n";
        }

        my $percent_filled = (@values[1] / @values[0]);
        my $percent_cleared = (@values[2] / @values[0]);
        my $percent_free = ((@values[0]-@values[1]) / @values[0]);

        $gmetric_cmd = "gmetric --type float --name db_\%_filled --value ".$percent_filled ." --unit %";
        `$gmetric_cmd`;
        $gmetric_cmd = "gmetric --type float --name db_\%_cleared --value ".$percent_cleared." --unit %";
        `$gmetric_cmd`;
        $gmetric_cmd = "gmetric --type float --name db_\%_free --value ".$percent_free." --unit %";
        `$gmetric_cmd`;

        logMessage(1, "INFO", "gmetric \"db_\%_filled\"  \"".$percent_filled."\"");
        logMessage(1, "INFO", "gmetric \"db_\%_cleared\" \"".$percent_cleared."\"");
        logMessage(1, "INFO", "gmetric \"db_\%_free\"    \"".$percent_free."\"");
      }

    }

    logMessage(2, "INFO", "db_thread: checking for threat quit");
    if ($quit_daemon) {
      logMessage(1, "INFO", "db_thread: asked to quit");
      $threadQuit = "true";
    } else {
      sleep(SLEEP_TIME);
    }

  }

  logMessage(1, "INFO", "db_thread: returning ok");
  return "ok";

}


#
# Logs a message to the Nexus
#
sub logMessage($$$) {
  (my $level, my $type, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    my $time = Dada::getCurrentDadaTime();
    if (!($log_socket)) {
      $log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SYS_LOG_PORT"});
    }
    if ($log_socket) {
      Dada::nexusLogMessage($log_socket, $time, "sys", $type, "monitor", $message);
    }
    print "[".$time."] ".$message."\n";
  }
}

#
# Handles INT and TERM signals
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";

  $quit_daemon = 1;
  sleep(3);

  if ($log_socket) {
    close($log_socket);
  }

  print STDERR basename($0)." : Exiting\n";
  exit 1;
                                                                                                        
}

#
# Handles a broken pipe signal
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $log_socket = 0;
  $log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SYS_LOG_PORT"});

}
