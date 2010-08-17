#!/usr/bin/env perl

#
# Author:   Andrew Jameson
# Created:  24 Jan 2008
# Modified: 24 Jan 2008
# 
# This daemons runs continuously and launches dspsr each time
# a full header block is written
#
# 1.  Runs dada_header which will print out the next free header block
# 2.  Greps for the PROCESSING_CMD 
# 3.  Runs the PROCESSING_CMD in a system call, waiting for it to terminate
# 4.  Rinse, Lather, Repeat.

use lib $ENV{"DADA_ROOT"}."/bin";


#
# Include Modules
#
use strict;           # strict mode (like -Wall)
use threads;          # standard perl threads
use threads::shared;  # Allow shared variables for threads
use File::Basename;
use IO::Socket;
use Apsr;             # APSR/DADA Module for configuration options

#
# Constants
#
use constant DEBUG_LEVEL        => 1;
use constant DADA_HEADER_BINARY => "dada_header -k fada";
use constant PIDFILE            => "apsr_aux_manager.pid";
use constant LOGFILE            => "apsr_aux_manager.log";


#
# Global Variable Declarations
#
our $log_socket;
our %cfg : shared = Apsr::getApsrConfig();      # Apsr.cfg in a hash
our $quit_daemon : shared = 0;


#
# Local Variable Declarations
#
my $logfile = $cfg{"CLIENT_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"CLIENT_CONTROL_DIR"}."/".PIDFILE;
my $prev_header = "";
my $daemon_quit_file = Dada::getDaemonControlFile($cfg{"CLIENT_CONTROL_DIR"});
my $daemon_control_thread = "";
my $proc_thread = "";


#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

# Turn the script into a daemon
Dada::daemonize($logfile, $pidfile);

# Open a connection to the nexus logging facility
$log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SYS_LOG_PORT"});
if (!$log_socket) {
  print "Could not open a connection to the nexus SYS log: $log_socket\n";
}

logMessage(0, "INFO", "STARTING SCRIPT");

if (!(-f $daemon_quit_file)) {

  # This thread will monitor for our daemon quit file
  $daemon_control_thread = threads->new(\&daemon_control_thread, "dada_header");

  # Main Loop
  while(!$quit_daemon) {

    # Run the processing thread once
    $prev_header = processing_thread($prev_header);
    sleep(1);

  }

  logMessage(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_socket);
  $daemon_control_thread->join();

  exit(0);

} else {

  logMessage(0,"INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_socket);
  exit(1);

}


sub processing_thread($) {

  (my $prev_header) = @_;

  my $bindir = Dada::getCurrentBinaryVersion();
  my $dada_header_cmd = $bindir."/".DADA_HEADER_BINARY;
  my $processing_dir = $cfg{"CLIENT_ARCHIVE_DIR"};
  my $raw_data_dir = $cfg{"CLIENT_RECORDING_DIR"};

  my %h = ();
  my @lines = ();
  my $line = "";
  my $key = "";
  my $val = "";
  my $raw_header = "";
  my $cmd = "";

  # Get the projects/groups that apsr is a member of
  my $unix_groups = `groups`;
  chomp $unix_groups;

  # Get the next filled header on the data block. Note that this may very
  # well hang for a long time - until the next header is written...
  logMessage(2, "INFO", "Running cmd \"".$dada_header_cmd."\"");
  my $raw_header = `$dada_header_cmd 2>&1`;

  # since the only way to currently stop this daemon is to send a kill
  # signal to dada_header_cmd, we should check the return value
  if ($? == 0) {

    my $proc_cmd = "";

    @lines = split(/\n/,$raw_header);
    foreach $line (@lines) {
      ($key,$val) = split(/ +/,$line,2);
      if ((length($key) > 1) && (length($val) > 1)) {
        # Remove trailing whitespace
        $val =~ s/\s*$//g;
        $h{$key} = $val;
      }
    }

    my $header_ok = 1;
    if (length($h{"UTC_START"}) < 5) {
      logMessage(0, "ERROR", "UTC_START was malformed or non existent");
      $header_ok = 0;
    }
    if (length($h{"OBS_OFFSET"}) < 1) {
      logMessage(0, "ERROR", "Error: OBS_OFFSET was malformed or non existent");
      $header_ok = 0;
    }
    if ($unix_groups =~ m/$h{"PID"}/) {
      logMessage(0, "ERROR", "Error: APSR user was not a member of ".$h{"PID"}." group");
      $header_ok = 0;
    }

    if ($raw_header eq $prev_header) {
                                                                                                                 
      logMessage(0, "ERROR", "DADA header repeated, likely cause failed PROC_CMD, jettesioning xfer");
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});
                                                                                                                 
    } elsif (! $header_ok) {
                                                                                                                 
      logMessage(0, "ERROR", "DADA header malformed, jettesioning xfer");
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});
                                                                                                                 
    } else {

      logMessage(2, "INFO", "asking for aux nodes");
      # Check if any auxiliary nodes are available for processing 
      my $node = auxiliaryNodesAvailable();
      logMessage(2, "INFO", "response was ".$node);

      # We got a free node, run dbnic on the xfer
      if ($node =~ /apsr(\d\d):(\d+)/) {

        logMessage(2, "INFO", "Sending data to ".$node);
        $proc_cmd = $bindir."/dada_dbnic -k fada -s -N ".$node;

      # If no nodes are available, run dbdisk on the xfer
      } else {

        $proc_cmd = $bindir."/dada_dbdisk -k fada -s -D ".$raw_data_dir;

      }

      # Create an obs.start file in the processing dir:
      logMessage(0, "INFO", "START $proc_cmd");

      my $response = `$proc_cmd`;
    
      if ($? != 0) {
        logMessage(0, "ERROR", "Aux cmd \"".$proc_cmd."\" failed \"".$response."\"");
      }

      logMessage(0, "INFO", "END ".$proc_cmd);

    }

    return ($raw_header);

  } else {

    logMessage(2, "WARN", "dada_header_cmd failed - probably no data block");
    sleep 1;
    return ("");

  }
}


sub auxiliaryNodesAvailable() {

  my $host = $cfg{"SERVER_HOST"};
  my $port = $cfg{"SERVER_AUX_CLIENT_PORT"};

  my $localhost = Dada::getHostMachineName();

  logMessage(2, "INFO", "Trying to connect to ". $cfg{"SERVER_HOST"}.":".$cfg{"SERVER_AUX_CLIENT_PORT"});
  my $handle = Dada::connectToMachine($host, $port);

  if (!$handle) {

    logMessage(0, "WARN", "Could not connect to assisant manager ". $cfg{"SERVER_HOST"}.":".$cfg{"SERVER_AUX_CLIENT_PORT"});
    return "none";

  # We have connected to the
  } else {

    logMessage(2, "INFO", "Connected to ". $cfg{"SERVER_HOST"}.":".$cfg{"SERVER_AUX_CLIENT_PORT"});

    print $handle $localhost.":help\r\n";

    logMessage(2, "INFO", "Sent \"".$localhost.":help\"");
    my $string = Dada::getLineSelect($handle,2);
    logMessage(2, "INFO", "Received \"".$string."\"");

    close($handle);
    logMessage(2, "INFO", "Closed handle");
    return $string;
  }
}


sub daemon_control_thread($) {

  (my $cmd_to_kill) = @_;

  logMessage(2, "INFO", "control_thread: kill command ".$cmd_to_kill);

  my $pidfile = $cfg{"CLIENT_CONTROL_DIR"}."/".PIDFILE;
  my $daemon_quit_file = Dada::getDaemonControlFile($cfg{"CLIENT_CONTROL_DIR"});

  while ((!(-f $daemon_quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }
  # If the daemon quit file exists, we MUST kill the dada_header process if 
  # it exists. If the processor is still processing, then we would wait for
  # it to finish.
  $quit_daemon = 1;

  logMessage(2, "INFO", "control_thread: unlinking PID file");
  unlink($pidfile);

  # It is quit possible that the processing command will be running, and 
  # in this case, the processing thread itself will exit.
  my $cmd = "killall -KILL ".$cmd_to_kill;

  logMessage(2, "INFO", "control_thread: running $cmd");
  `$cmd`;

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
      Dada::nexusLogMessage($log_socket, $time, "sys", $type, "aux mngr", $message);
    }
    print "[".$time."] ".$message."\n";
  }
}

sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  print basename($0)." : Received SIG".$sigName."\n";

  # Tell threads to try and quit
  $quit_daemon = 1;
  sleep(3);

  if ($log_socket) {
    close($log_socket);
  }

  print STDERR basename($0)." : Exiting\n";
  exit 1;

}

sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  print basename($0)." : Received SIG".$sigName."\n";
  $log_socket = 0;
  $log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SYS_LOG_PORT"});


}


