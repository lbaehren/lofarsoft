#!/usr/bin/env perl

#
# Author:   Andrew Jameson
# Created:  6 Dec, 2007
# 


use lib $ENV{"DADA_ROOT"}."/bin";

#
# Include Modules
#
use Bpsr;            # DADA Module for configuration options
use strict;          # strict mode (like -Wall)
use threads;         # standard perl threads
use threads::shared; # standard perl threads
use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;


#
# Constants
#
use constant DADA_HEADER_BINARY => "dada_header -k deda";
use constant DAEMON_NAME        => "bpsr_observation_manager";
use constant LOCAL_PSRCAT_DB    => "/tmp/htru_interim.db";
use constant REMOTE_PSRCAT_DB   => "/psr1/cvshome/pulsar/soft_atnf/search/hitrun/database/htru_interim.db";

#
# Global Variable Declarations
#
our $dl : shared;
our $quit_daemon : shared;
our %cfg : shared;
our $log_host;
our $log_port;
our $log_sock;


#
# Initialize globals
#
$dl = 1;
$quit_daemon = 0;
%cfg = Bpsr::getBpsrConfig();
$log_host = $cfg{"SERVER_HOST"};
$log_port = $cfg{"SERVER_SYS_LOG_PORT"};
$log_sock = 0;

#
# Local Variable Declarations
#
my $log_file = "";
my $pid_file = "";
my $quit_file = "";
my $control_thread = 0;
my $folding_thread = 0;
my $prev_header = "";
my $quit = 0;

#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

$log_file = $cfg{"CLIENT_LOG_DIR"}."/".DAEMON_NAME.".log";
$pid_file = $cfg{"CLIENT_CONTROL_DIR"}."/".DAEMON_NAME.".pid";
$quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".DAEMON_NAME.".quit";

# become a daemon
Dada::daemonize($log_file, $pid_file);

# Auto flush output
$| = 1;

# Open a connection to the server_sys_monitor.pl script
$log_sock = Dada::nexusLogOpen($log_host, $log_port);
if (!$log_sock) {
  print STDERR "Could open log port: ".$log_host.":".$log_port."\n";
}

logMsg(1,"INFO", "STARTING SCRIPT");

if (! -f $quit_file ) {

  # This thread will monitor for our daemon quit file
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # Main Loop
  while (!$quit_daemon) {

    # Run the processing one observation
    ($quit, $prev_header) = process_one_obs($prev_header);

    if ($quit) {
      $quit_daemon = 1;
    }

    # join the folding thread as it should have exited by now
    if ($folding_thread) {
      $folding_thread->join();
      $folding_thread = 0;
    }

    # try to fold the previous observation
    $folding_thread = threads->new(\&fold_one_obs, $prev_header);

  }

  logMsg(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);
  $control_thread->join();
  exit(0);

} else {

  logMsg(0,"INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);
  exit(1);

}


#
# Processes a single observation
#
# Creates the required directories for the output data on the server, and generates the obs.start file
#
sub process_one_obs($) {

  (my $prev_header) = @_;

  my $bindir = Dada::getCurrentBinaryVersion();
  my $dada_header_cmd = $bindir."/".DADA_HEADER_BINARY;
  my $processing_dir = $cfg{"CLIENT_ARCHIVE_DIR"};
  my $utc_start = "";
  my $acc_len = "";
  my $obs_offset = "";
  my $proc_cmd = "";
  my $proc_cmd_file = "";
  my $proj_id = "";
  my $beam = "";
  my $remote_dirs_thread = 0;

  my %h = ();
  my @lines = ();
  my $line = "";
  my $key = "";
  my $val = "";
  my $raw_header = "";
  my $time_str;
  my $cmd = "";

  chdir $processing_dir;

  # Get the next filled header on the data block. Note that this may very
  # well hang for a long time - until the next header is written...
  logMsg(2, "INFO", "Running cmd \"".$dada_header_cmd."\"");
  my $raw_header = `$dada_header_cmd 2>&1`;

  if ($remote_dirs_thread) {
    logMsg(2, "INFO", "joining remoteDirsThread");
    $remote_dirs_thread->join();
    logMsg(2, "INFO", "remoteDirsThread joined");
  }

  # since the only way to currently stop this daemon is to send a kill
  # signal to dada_header_cmd, we should check the return value
  if ($? == 0) {

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
      logMsg(0, "ERROR", "UTC_START was malformed or non existent");
      $header_ok = 0;
    }
    if (length($h{"OBS_OFFSET"}) < 1) {
      logMsg(0, "ERROR", "Error: OBS_OFFSET was malformed or non existent");
      $header_ok = 0;
    }
    if (length($h{"PROC_FILE"}) < 1) {
      logMsg(0, "ERROR", "PROC_FILE was malformed or non existent");
      $header_ok = 0;
    }

    # command line that will be run
    my $proc_cmd = "";

    # Test for a repeated header
    if ($raw_header eq $prev_header) {
      logMsg(0, "ERROR", "DADA header repeated, likely cause failed PROC_CMD, jettesioning xfer");
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    # Or if malformed
    } elsif (! $header_ok) {
      logMsg(0, "ERROR", "DADA header malformed, jettesioning xfer");
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    } else {

      my $obs_start_file = createLocalDirectories($h{"UTC_START"}, $h{"BEAM"}, $h{"PID"}, $raw_header);

      # launch thread to create remote NFS dirs, as this can take a long time due to NFS load
      $remote_dirs_thread = threads->new(\&remoteDirsThread, $h{"UTC_START"}, $h{"BEAM"}, $h{"PID"}, $obs_start_file);

      $time_str = Dada::getCurrentDadaTime();

      $processing_dir .= "/".$h{"UTC_START"}."/".$h{"BEAM"};

      # Add the dada header file to the proc_cmd
      my $proc_cmd_file = $cfg{"CONFIG_DIR"}."/".$h{"PROC_FILE"};

      logMsg(2, "INFO", "Full path to PROC_FILE: ".$proc_cmd_file);

      my %proc_cmd_hash = Dada::readCFGFile($proc_cmd_file);
      $proc_cmd = $proc_cmd_hash{"PROC_CMD"};

      logMsg(2, "INFO", "Initial PROC_CMD: ".$proc_cmd);

      # Normal processing via the_decimator
      if ($proc_cmd =~ m/the_decimator/) {
        $proc_cmd .= " -o ".$h{"UTC_START"};
      }

      $proc_cmd .= " ".$cfg{"PROCESSING_DB_KEY"};

      logMsg(2, "INFO", "Final PROC_CMD: ".$proc_cmd);
    }

    logMsg(1, "INFO", "START ".$proc_cmd);
    logMsg(2, "INFO", "Changing dir to $processing_dir");
    chdir $processing_dir;

    $cmd = $bindir."/".$proc_cmd;
    $cmd .= " 2>&1 | ".$cfg{"SCRIPTS_DIR"}."/client_bpsr_src_logger.pl";

    logMsg(2, "INFO", "cmd = $cmd");

    my $return_value = system($cmd);
   
    if ($return_value != 0) {
      logMsg(0, "ERROR", $proc_cmd." failed: ".$?." ".$return_value);
    }

    $time_str = Dada::getCurrentDadaTime();

    chdir "../../";
    logMsg(1, "INFO", "END ".$proc_cmd);;

    return (0, $raw_header);

  } else {

    logMsg(2, "WARN", "dada_header_cmd failed - probably no data block");
    sleep 1;
    return (0, "");

  }
}


#
# Thread to create remote NFS links on the server
#
sub remoteDirsThread($$$$) {

  my ($utc_start, $beam, $proj_id, $obs_start_file) = @_;

  logMsg(2, "INFO", "remoteDirsThread(".$utc_start.", ".$beam.", ".$proj_id.", ".$obs_start_file.")");

  createRemoteDirectories($utc_start, $beam, $proj_id);

  copyUTC_STARTfile($utc_start, $beam, $obs_start_file);

}



#
# check the header supplied 
# 
sub fold_one_obs($) {

  my ($raw_header) = @_;

  my %h = ();
  my $line = "";
  my @lines = ();
  my $key = "";
  my $val = "";
  my $proc_cmd = "";
  my $cmd = "";
  my $result = "";
  my $response = "";
  my $fil_file = "";
  my $par_file = "";
  my $ar_file = "";

  @lines = split(/\n/,$raw_header);
  foreach $line (@lines) {
    ($key,$val) = split(/ +/,$line,2);
    if ((length($key) > 1) && (length($val) > 1)) {
      # Remove trailing whitespace
      $val =~ s/\s*$//g;
      $h{$key} = $val;
    }
  }

  # Add the dada header file to the proc_cmd
  my $proc_cmd_file = $cfg{"CONFIG_DIR"}."/".$h{"PROC_FILE"};
  my %proc_cmd_hash = Dada::readCFGFile($proc_cmd_file);
  $proc_cmd = $proc_cmd_hash{"PROC_CMD"};

  logMsg(2, "INFO", "fold_one_obs: Initial PROC_CMD: ".$proc_cmd);

  # If we 2bit file and source starts with J and Beam01
  if ( ( $proc_cmd =~ m/the_decimator/) && ($h{"SOURCE"} =~ m/^J/) && 
       ( $h{"BEAM"} eq "01" ) ) {

    logMsg(1, "INFO", "fold_one_obs: UTC_START=".$h{"UTC_START"}.", SOURCE=".$h{"SOURCE"});

    $fil_file = $cfg{"CLIENT_ARCHIVE_DIR"}."/".$h{"UTC_START"}."/".$h{"BEAM"}."/".$h{"UTC_START"}.".fil";

    if (-f $fil_file) {

      # we will try to process this sucker
      $cmd = "scp -o BatchMode=yes dada\@jura:".REMOTE_PSRCAT_DB." ".LOCAL_PSRCAT_DB;
      logMsg(2, "INFO", "fold_one_obs: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

      # we managed to copy the database
      if ($result eq "ok") {

        $par_file = "/tmp/".$h{"SOURCE"}.".par";
        logMsg(1, "INFO", "fold_one_obs: setting par_file to: ".$par_file);

        if (-f $par_file ) {
          unlink ($par_file);
        }

        # create a temporary par file 
        $cmd = "psrcat -merge ".LOCAL_PSRCAT_DB." -e ".$h{"SOURCE"}." > ".$par_file;
        logMsg(1, "INFO", "fold_one_obs: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

        # since psrcat does not produce set a return value on success/failure i
        # to find a psr, check the parfile for "not in catalogue"
        $cmd = "grep 'not in catalogue' ".$par_file;
        logMsg(2, "INFO", "fold_one_obs: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

        # if we dont find this string, then we should have a valid ephemeris
        if ($result ne "ok") {

          $ar_file = "/tmp/".$h{"UTC_START"}.".ar";

          # run dspsr with the temp par file (CHECK WORKING DIR)
          $cmd = "cd /tmp; dspsr -Q -U 1 -E ".$par_file." -L 10 ".$fil_file." -e aa";
          # " -O ".$ar_file;
          logMsg(1, "INFO", "fold_one_obs: ".$cmd);
          ($result, $response) = Dada::mySystem($cmd);
          logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

          if ($result eq "ok") {

            # PSRADD 
            $cmd = "psradd -o ".$ar_file." /tmp/*.aa";
            logMsg(1, "INFO", "fold_one_obs: ".$cmd);
            ($result, $response) = Dada::mySystem($cmd);
            logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

            if ($result eq "ok") {

              # ensure this directory is automounted 
              $cmd = "ls /nfs/archives/ >& /dev/null";
              logMsg(2, "INFO", "fold_one_obs: ".$cmd);
              ($result, $response) = Dada::mySystem($cmd);
              logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);
  
              # copy the archive
              $cmd = "cp ".$ar_file." /nfs/archives/bpsr_pulsars/";
              logMsg(1, "INFO", "fold_one_obs: ".$cmd);
              ($result, $response) = Dada::mySystem($cmd);
              logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);

              if ($result eq "ok") {
                logMsg(1, "INFO", "fold_one_obs: SUCCESS!!");
              } else {
                logMsg(0, "WARN", "fold_one_obs: failed to copy dspsrd file: ".$response);
              }
            } else {
              logMsg(0, "WARN", "fold_one_obs: psradd failed: ".$response);
            }

            $cmd = "rm -f /tmp/*.aa";
            logMsg(2, "INFO", "fold_one_obs: ".$cmd);
            ($result, $response) = Dada::mySystem($cmd);
            logMsg(2, "INFO", "fold_one_obs: ".$result." ".$response);


          } else {
            logMsg(0, "WARN", "fold_one_obs: dspsr failed: ".$response);
          }
        } else {
          logMsg(0, "WARN", "fold_one_obs: source ".$h{"SOURCE"}." was not in the catalogue");
        }
      } else {
        logMsg(0, "WARN", "fold_one_obs: scp of updated DB failed: ".$response);
      }
    } else {
      logMsg(0, "WARN", "fold_one_obs: fil file [".$fil_file."] did not exist");
    }
  }
}


sub controlThread($$) {

  my ($quit_file, $pid_file) = @_;

  logMsg(2, "INFO", "controlThread : starting");

  my $cmd = "";
  my $result = "";
  my $response = "";

  while ((!$quit_daemon) && (!(-f $quit_file))) {
    sleep(1);
  }

  $quit_daemon = 1;

  # Kill the dada_header command
  $cmd = "ps aux | grep -v grep | grep bpsr | grep '".DADA_HEADER_BINARY."' | awk '{print \$2}'";

  logMsg(2, "INFO", " controlThread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  $response =~ s/\n/ /;
  logMsg(2, "INFO", "controlThread: ".$result." ".$response);

  if ($result eq "ok") {
    $cmd = "kill -KILL ".$response;
    logMsg(1, "INFO", "controlThread: Killing ".DADA_HEADER_BINARY.": ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "controlThread: ".$result." ".$response);
  }

  if ( -f $pid_file) {
    logMsg(2, "INFO", "controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    logMsg(1, "WARN", "controlThread: PID file did not exist on script exit");
  }

  logMsg(2, "INFO", "controlThread: exiting");

}




#
# Logs a message to the nexus logger and print to STDOUT with timestamp
#
sub logMsg($$$) {

  my ($level, $type, $msg) = @_;

  if ($level <= $dl) {

    my $time = Dada::getCurrentDadaTime();
    if (!($log_sock)) {
      $log_sock = Dada::nexusLogOpen($log_host, $log_port);
    }
    if ($log_sock) {
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "obs mngr", $msg);
    }
    print "[".$time."] ".$msg."\n";
  }
}



sub sigHandle($) {

  my $sigName = shift;
  print STDERR DAEMON_NAME." : Received SIG".$sigName."\n";

  # if we CTRL+C twice, just hard exit
  if ($quit_daemon) {
    print STDERR DAEMON_NAME." : Recevied 2 signals, Exiting\n";
    exit 1;

  # Tell threads to try and quit
  } else {

    $quit_daemon = 1;
    if ($log_sock) {
      close($log_sock);
    }
  }
}

sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR DAEMON_NAME." : Received SIG".$sigName."\n";
  $log_sock = 0;
  if ($log_host && $log_port) {
    $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  }

}



#
# Create the remote directories required for this observation
#

sub createRemoteDirectories($$$) {

  my ($utc_start, $beam, $proj_id) = @_;
  
  my $localhost = Dada::getHostMachineName();
  my $remote_archive_dir = $cfg{"SERVER_ARCHIVE_NFS_MNT"};
  my $remote_results_dir = $cfg{"SERVER_RESULTS_NFS_MNT"};
  my $cmd = "";
  my $dir = "";

  # Wait for remote results directory to be created...

  # Ensure each directory is automounted
  if (!( -d  $remote_archive_dir)) {
    `ls $remote_archive_dir >& /dev/null`;
  }
  if (!( -d  $remote_results_dir)) {
    `ls $remote_results_dir >& /dev/null`;
  }

  # Create the nfs soft link to the local archives directory 
  #chdir $remote_archive_dir."/".$utc_start;
  #$cmd = "ln -s /nfs/".$localhost."/bpsr/archives/".$utc_start."/".$beam." .";
  $cmd = "ln -s /nfs/".$localhost."/bpsr/archives/".$utc_start."/".$beam." ".$remote_archive_dir."/".$utc_start."/".$beam;
  logMsg(2, "INFO", $cmd);
  system($cmd);
  
  # Create the remote nfs directory
  $cmd = "mkdir -p ".$remote_results_dir."/".$utc_start."/".$beam;
  logMsg(2, "INFO", $cmd);
  system($cmd);

  # Adjust permission on remote results directory
  $dir = $remote_results_dir."/".$utc_start."/".$beam;
  $cmd = "chgrp -R ".$proj_id." ".$dir;
  logMsg(2, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chgrp remote results dir \"".
               $dir."\" to \"".$proj_id."\"");
  }

  $cmd = "chmod g+s ".$dir;
  logMsg(1, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chmod remote results dir \"".
               $dir."\" to g+s");
  }

}

  
#
# Create the local directories required for this observation
#
sub createLocalDirectories($$$$) {

  (my $utc_start, my $beam, my $proj_id, my $raw_header) = @_;

  my $local_archive_dir = $cfg{"CLIENT_ARCHIVE_DIR"};
  my $dir = "";

  # Create local archive directory
  $dir = $local_archive_dir."/".$utc_start."/".$beam;
  logMsg(2, "INFO", "Creating local output dir \"".$dir."\"");
  `mkdir -p $dir`;
  if ($? != 0) {
    logMsg(0,"ERROR", "Could not create local archive dir \"".
               $dir."\"");
  }

  $dir .= "/aux";
  `mkdir -p $dir`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to create local aux dir \"".$dir."\"");
  }

  # Set GID on local archive dir
  $dir = $local_archive_dir."/".$utc_start;
  `chgrp -R $proj_id $dir`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chgrp local archive dir \"".
               $dir."\" to group \"".$proj_id."\"");
  }
 
  # Set group sticky bit on local archive dir
  `chmod g+s $dir $dir/$beam`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to set sticky bit on local archive dir \"".
               $dir."\"");
  }

  
  # Create an obs.start file in the processing dir:
  logMsg(2, "INFO", "Creating obs.start");
  $dir = $local_archive_dir."/".$utc_start."/".$beam;
  my $obsstart_file = $dir."/obs.start";
  open(FH,">".$obsstart_file.".tmp");
  print FH $raw_header;
  close FH;
  rename($obsstart_file.".tmp",$obsstart_file);

  return $obsstart_file;

}


#
# Copies the UTC_START file via NFS to the server's results and archive directories:
#

sub copyUTC_STARTfile($$$) {

  my ($utc_start, $beam, $obs_start_file) = @_;

  # nfs mounts
  my $results_dir = $cfg{"SERVER_RESULTS_NFS_MNT"};
  my $cmd = ""; 
  my $result = "";
  my $response = "";

  # Ensure each directory is automounted
  if (!( -d  $results_dir)) {
    `ls $results_dir >& /dev/null`;
  }

  # Create the full nfs destinations
  $results_dir .= "/".$utc_start."/".$beam;

  $cmd = "cp ".$obs_start_file." ".$results_dir;
  logMsg(2, "INFO", "NFS copy \"".$cmd."\"");
  ($result, $response) = Dada::mySystem($cmd,0);
  if ($result ne "ok") {
                                                                                                                                                                     
    logMsg(0, "ERROR", "Failed to nfs copy \"".$obs_start_file."\" to \"".$results_dir."\": response: \"".$response."\"");
    logMsg(0, "ERROR", "Command was: \"".$cmd."\"");
    if (-f $obs_start_file) {
      logMsg(0, "ERROR", "File existed locally");
    } else {
      logMsg(0, "ERROR", "File did not exist locally");
    }
  }

  logMsg(2, "INFO", "Server directories perpared");

}

