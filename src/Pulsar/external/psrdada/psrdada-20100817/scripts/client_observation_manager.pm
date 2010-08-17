package Dada::client_observation_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use Dada;
use strict;
use warnings;
use File::Basename;
use threads;         # standard perl threads
use threads::shared; # standard perl threads
use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;


BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

  # your exported package globals go here,
  # as well as any optionally exported functions
  @EXPORT_OK   = qw($dl $user $daemon_name $dada_header_cmd $gain_controller $client_logger %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $user : shared;
our $dl : shared;
our $daemon_name : shared;
our $dada_header_cmd;
our $gain_controller;
our $client_logger;
our %cfg : shared;

#
# non-exported package globals go here
#
our $currently_processing : shared;
our $quit_daemon : shared;
our $log_host;
our $log_port;
our $log_sock;

#
# initialize package globals
#
$user = "";
$dl = 1; 
$daemon_name = 0;
$dada_header_cmd = 0;
$gain_controller = 0;
$client_logger = 0;
%cfg = ();

#
# initialize other variables
#
$quit_daemon = 0;
$currently_processing = "none";
$log_host = 0;
$log_port = 0;
$log_sock = 0;


###############################################################################
#
# package functions
# 

sub main() {

  my $log_file       = $cfg{"CLIENT_LOG_DIR"}."/".$daemon_name.".log";;
  my $pid_file       = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file      = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".quit";

  $log_host          = $cfg{"SERVER_HOST"};
  $log_port          = $cfg{"SERVER_SYS_LOG_PORT"};

  my $control_thread = 0;
  my $bg_processing_thread = 0;
  my $calibrator_thread = 0;
  # my $first_calibrator_thread = 0;

  my $utc_start = "";
  my $obs_offset = "";
  my $band = "";
  my $source = "";
  my $calibrate = 0;
  my $prev_utc_start = "";
  my $prev_obs_offset = "";
  # my $prev_band = "";
  # my $prev_source=  "";
  # my $prev_calibrate = 0; 
  my $obs_end = 0;

  my $result = "";
  my $response = "";

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  # install signal handles
  $SIG{INT}  = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;

  # become a daemon
  Dada::daemonize($log_file, $pid_file);

  # Auto flush output
  $| = 1;

  # Open a connection to the server_sys_monitor.pl script
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  if (!$log_sock) {
    print STDERR "Could not open log port: ".$log_host.":".$log_port."\n";
  }

  logMsg(0, "INFO", "STARTING SCRIPT");

  # Start the daemon control thread
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # This thread responds to requests for background processing
  $bg_processing_thread = threads->new(\&bgProcessingThread);

  # Main Loop
  while( (!($quit_daemon) ) ) {

    # Process one observation/xfer
    ($utc_start, $band, $source, $obs_offset, $calibrate, $obs_end) = processingThread($prev_utc_start, $prev_obs_offset);

    logMsg(0, "INFO", "processingThread() ($utc_start, $band, $source, $obs_offset, $calibrate, $obs_end)");

    if ($quit_daemon) {
      # we have been asked to quit, no need to complain about anything

    } elsif (($utc_start eq "invalid") || ($band eq "invalid")) {

      logMsg(0, "ERROR", "processingThread return an invalid obs/header");
      sleep(1);

    } else {

      if ($obs_end) {

        if ($utc_start eq $prev_utc_start) {

          logMsg(0, "ERROR", "main: obs_end and UTC_START repeated");

        } else {

          # touch the band.finished file in the 
          touchBandFinished($utc_start, $band);

          # Join a previous calibrator thread if it existed
          if ($calibrator_thread) {
            $calibrator_thread->join();
            $calibrator_thread = 0;
          }

          if ($calibrate) {
            logMsg(2, "INFO", "main: calibratorThread(".$utc_start.", ".$band.", ".$source.")");
            $calibrator_thread = threads->new(\&calibratorThread, $utc_start, $band, $source);
          }
        }
      }
    }
 
    $prev_utc_start = $utc_start;
    $prev_obs_offset = $obs_offset;
    # $prev_band = $band;
    # $prev_source = $source;
    # $prev_calibrate = $calibrate;

  }

  logMsg(2, "INFO", "main: joining threads");
  $control_thread->join();
  logMsg(2, "INFO", "main: control_thread joined");
  $bg_processing_thread->join();
  logMsg(2, "INFO", "main: bg_processing_thread joined");
  if ($calibrator_thread) {
    $calibrator_thread->join();
    logMsg(2, "INFO", "main: calibrator_thread joined");
  }

  logMsg(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);

  return 0;
}

#
# Processes a single observation
# Creates the required directories for the output data on the server, and generates the obs.start file
#

sub processingThread($$) {

  (my $prev_utc_start, my $prev_obs_offset) = @_;

  my $bindir = Dada::getCurrentBinaryVersion();

  my $remote_dirs_thread = 0;
  my $utc_start = "invalid";
  my $band = "invalid";
  my $obs_offset = "invalid";
  my $proc_cmd = "";
  my $proj_id = "invalid";
  my $source = "invalid";
  my $proc_file = "";
  my $end_of_obs = 1;
  my $obs_xfer = 0;

  my %h = ();
  my $raw_header = "";
  my $cmd = "";

  my $digimon_thread = 0;
  my $digimon_cmd = $bindir."/digimon";
  my $digimon_nchan = 1;
  my $digimon_rval = 0;

  my $calibrator = 0;
  my $header_valid = 1;

  # Get the next filled header on the data block. Note that this may very
  # well hang for a long time - until the next header is written...
  $cmd = $bindir."/".$dada_header_cmd;
  logMsg(2, "INFO", "Running ".$cmd);
  $raw_header = `$cmd 2>&1`;
  logMsg(2, "INFO", $cmd." returned");

  # since the only way to currently stop this daemon is to send a kill
  # signal to dada_header_cmd, we should check the return value
  if ($? == 0) {

    %h = Dada::headerToHash($raw_header);

    $proc_file  = $h{"PROC_FILE"};
    my $obs_start_file = "";

    if (defined($h{"UTC_START"}) && length($h{"UTC_START"}) > 5) {
      $utc_start  = $h{"UTC_START"};
    } else {
      logMsg(0, "ERROR", "UTC_START [".$utc_start."] was malformed or non existent");
      $header_valid = 0;
    }

    if (defined($h{"FREQ"}) && length($h{"FREQ"}) > 0) {
      $band = $h{"FREQ"};
    } else {
      logMsg(0, "ERROR", "Error: FREQ [".$band."] was malformed or non existent");
      $header_valid = 0;
    }

    if (defined($h{"OBS_OFFSET"}) && length($h{"OBS_OFFSET"}) > 0) {
      $obs_offset = $h{"OBS_OFFSET"};
    } else {
      logMsg(0, "ERROR", "Error: OBS_OFFSET [".$h{"OBS_OFFSET"}."] was malformed or non existent");
      $header_valid = 0;
    }
    if (defined($h{"PID"}) && length($h{"PID"}) > 3) {
      $proj_id = $h{"PID"};
    } else {
      logMsg(0, "ERROR", "Error: PID [".$proj_id."] was malformed or non existent");
      $header_valid = 0;
    }

    if (defined($h{"SOURCE"}) && length($h{"SOURCE"}) > 3) {
      $source = $h{"SOURCE"};
    } else {
      logMsg(0, "ERROR", "Error: SOURCE [".$source."] was malformed or non existent");
      $header_valid = 0;
    }

    if (defined($h{"OBS_XFER"})) {
      $obs_xfer = $h{"OBS_XFER"};
      if ($obs_xfer eq "-1") {
        $end_of_obs = 1;
      } else {
        $end_of_obs = 0;
      }
      
    } else {
      $end_of_obs = 1;
      $obs_xfer = 0;
    }

    logMsg(1, "INFO", "new header: UTC_START=".$utc_start.", FREQ=".$band.", OBS_OFFSET=".$obs_offset.", OBS_XFER=".$obs_xfer." END_OF_OBS=".$end_of_obs);

    # TESTING FOR CASPSR
    if (($obs_xfer eq "-1") && ($proc_file  =~ m/dbdisk/)) {
      logMsg(1, "INFO", "Ignoring final transfer that had dbdisk set as procfile");
      $header_valid = 0;
    }

    if (!$header_valid) {

      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"RECEIVING_DATA_BLOCK"});
      logMsg(0, "ERROR", "header invalid: jettesioning observation with dada_dbnull");
      $currently_processing = "invalid";

    } else {

      # If the UTC START is the same
      if ($utc_start eq $prev_utc_start) {

        if ((!$end_of_obs) && ($obs_offset eq $prev_obs_offset)) {
          logMsg(0, "ERROR", "The UTC_START and OBS_OFFSET has been repeated");
        }  else {
          logMsg(0, "INFO", "New xfer for ".$utc_start."/".$band." OBS_OFFSET=".$obs_offset." END_OF_OBS=".$end_of_obs);
        }

      # this is a new observation
      } else {

        logMsg(0, "INFO", "New observation: ".$utc_start."/".$band);

        $obs_start_file = createLocalDirectories($utc_start, $band, $proj_id, $raw_header);

        $remote_dirs_thread = threads->new(\&remoteDirsThread, $utc_start, $band, $proj_id, $obs_start_file);

      }

      # Determine if this observation is a calibrator
      if ($h{"MODE"} =~ m/CAL/) {
        $calibrator = 1;
      }

      # So that the background manager knows we are processing
      $currently_processing = $utc_start;

      $proc_cmd = "dada_dbNdb -k ".lc($cfg{"PROCESSING_DATA_BLOCK"})." -k ".lc($cfg{"AUXILIARY_DATA_BLOCK"});

      # if we have been configured with a gain controller 
      if ($gain_controller ne "") {
        logMsg(1, "INFO", "Starting level setting thread");
        $digimon_thread  = threads->new(\&digimonThread, $digimon_cmd, $digimon_nchan);
      } else {
        $digimon_thread = 0;
      }
    }

    # run the processing command itself
    logMsg(1, "INFO", "START ".$proc_cmd);
    $cmd = $bindir."/".$proc_cmd;
    $cmd .= " 2>&1 | ".$cfg{"SCRIPTS_DIR"}."/".$client_logger;
    logMsg(2, "INFO", "cmd = $cmd");
    my $return_value = system($cmd);
   
    if ($return_value != 0) {
      logMsg(0, "ERROR", $proc_cmd." failed: ".$?." ".$return_value);
    }

    # So that the background manager knows we have stopped processing
    $currently_processing = "none";

    logMsg(1, "INFO", "END ".$proc_cmd);

    if ($header_valid) {

      if ($utc_start ne $prev_utc_start) {

        logMsg(2, "INFO", "joining remoteDirsThread");
        $remote_dirs_thread->join();
        logMsg(2, "INFO", "remoteDirsThread joined");

      }

      # if configured with a gain controller 
      if ($gain_controller ne "") {

        logMsg(2, "INFO", "Stopping gain controller");

        # a rather inelegant way to kill digimon
        $cmd = "killall digimon";
        system($cmd);

        logMsg(2, "INFO", "Waiting for level setting thread to join");
        $digimon_rval = $digimon_thread->join();
        logMsg(2, "INFO", "Waiting for level setting has joined: ".$digimon_rval);
        $digimon_thread = 0;
      }
    }

    return ($utc_start, $band, $source, $obs_offset, $calibrator, $end_of_obs);

  } else {
    if (!$quit_daemon) {
      logMsg(2, "WARN", "dada_header_cmd failed - probably no data block");
      sleep 1;
    }
    return ($utc_start, $band, $source, $obs_offset, $calibrator, $end_of_obs);
  }
}


sub controlThread($$) {

  my ($quit_file, $pid_file) = @_;
  logMsg(2, "INFO", "controlThread: starting");

  my $cmd = "";
  my $result = "";
  my $response = "";

  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  $quit_daemon = 1;

  # Kill the dada_header command
  $cmd = "ps aux | grep -v grep | grep ".$user." | grep '".$dada_header_cmd."' | awk '{print \$2}'";
  logMsg(2, "INFO", " controlThread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  $response =~ s/\n/ /;
  logMsg(2, "INFO", "controlThread: ".$result." ".$response);
  if (($result eq "ok") & ($response ne "")) {
    $cmd = "kill -KILL ".$response;
    logMsg(1, "INFO", "controlThread: Killing dada_header: ".$cmd);
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

  return 0;
}

sub bgProcessingThread() {

  logMsg(2, "INFO", "bgProcessingThread: starting");

  my $localhost = Dada::getHostMachineName();

  my $server = new IO::Socket::INET (
    LocalHost => $localhost,
    LocalPort => $cfg{"CLIENT_BG_PROC_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1);
                                                                                                                                                                                           
  my $read_set = new IO::Select();  # create handle set for reading
  $read_set->add($server);   # add the main socket to the set
  my $rh;

  while (!$quit_daemon) {

    # Get all the readable handles from the server
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);
    foreach $rh (@$readable_handles) {

      logMsg(3, "INFO", "checking a read handle");

      if ($rh == $server) {

        my $handle = $rh->accept();
        $handle->autoflush(1);
        my $hostinfo = gethostbyaddr($handle->peeraddr);
        my $hostname = $hostinfo->name;

        logMsg(2, "INFO", "Accepting connection from $hostname");

        # Add this read handle to the set
        $read_set->add($handle);

      } else {

        my $string = Dada::getLine($rh);

        if (! defined $string) {
          $read_set->remove($rh);
          close($rh);
        } else {
          logMsg(2, "INFO", "received string \"".$string."\"");
          print $rh $currently_processing."\r\n";
          logMsg(2, "INFO", "sent string \"".$currently_processing."\"");
        }
      }
    }
  }

  logMsg(2, "INFO", "bgProcessingThread: exiting");
}


# 
# this thread waits for the archives from the calibrator to have been 
# transferred to the ARCHIVES_DIR, then copies the Tscrunched psradded
# archives to the appropriate calibrator database dir
#
sub calibratorThread($$$) {

  my ($utc_start, $band, $source) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";

  my $archives_dir = $cfg{"CLIENT_ARCHIVE_DIR"};
  my $tres_archive = $archives_dir."/".$utc_start."/".$band."/band.tres";
  my $fres_archive = $archives_dir."/".$utc_start."/".$band."/band.fres";

  # The tres archive *should* have been produced by the archive_manager
  if (! -f $tres_archive) {
    return ("fail", "tres archive did not exist: ".$tres_archive);
  }

  my $max_wait = 50;  # seconds
  my $done = 0;
  my $n_archives = 0;
  my $n_added = 0;

  logMsg(1, "INFO", "calibratorThread: Waiting 10 seconds for dspsr to finish");

  while ((!$quit_daemon) && ($max_wait > 0) && (!$done)) {
 
    sleep(10); 

    $n_archives = 0;
    $n_added = 0;

    # find out how many archives exist the tres archive
    $cmd = "psredit -q -c nsubint ".$tres_archive." | awk -F= '{print \$2}'";
    logMsg(2, "INFO", "calibratorThread: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "calibratorThread: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "calibratorThread: ".$cmd." failed: ".$response);
      $done = 2;
    } else {
      $n_added = $response;
    }

    # find out how many archive exist on disk
    $cmd = "find ".$archives_dir."/".$utc_start."/".$band." -name '*.ar' | wc -l";
    logMsg(2, "INFO", "calibratorThread: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "calibratorThread: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "calibratorThread: ".$cmd." failed: ".$response);
      $done = 2;
    } else {
      $n_archives = $response;
    }

    # If we have a match
    if (($n_added == $n_archives) && ($n_added != 0)) {
      $done = 1;
    }
  }

  # if we exited the while loop because we had an tres archive, add it to the DB
  if ($done == 1) {

    my $dir = $cfg{"CLIENT_CALIBRATOR_DIR"}."/".$source."/".$utc_start;

    $cmd = "mkdir -p ".$dir;
    logMsg(2, "INFO", "calibratorThread: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "calibratorThread: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "calibratorThread: ".$cmd." failed: ".$response);
    }

    $cmd = "cp ".$fres_archive." ".$dir."/".$band.".ar";
    logMsg(2, "INFO", "calibratorThread: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "calibratorThread: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "calibratorThread: ".$cmd." failed: ".$response);
    }

    $cmd = "pac -w -p ".$cfg{"CLIENT_CALIBRATOR_DIR"}." -u ar -u fits";
    logMsg(2, "INFO", "calibratorThread: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "calibratorThread: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "calibratorThread: ".$cmd." failed: ".$response);
    }
  }

  logMsg(1, "INFO", "calibratorThread: exiting");

  return 0;
}


#
# Thread to create the remote directories and copy the obs_start file
# 
sub remoteDirsThread($$$$) {

  my ($utc_start, $band, $proj_id, $obs_start_file) = @_;

  logMsg(2, "INFO", "remoteDirsThread(".$utc_start.", ".$band.", ".$proj_id.", ".$obs_start_file.")");

  createRemoteDirectories($utc_start, $band, $proj_id);

  copyObsStart($utc_start, $band, $obs_start_file);

}



#
# Create the remote directories required for this observation
#
sub createRemoteDirectories($$$) {

  my ($utc_start, $band, $proj_id) = @_;

  logMsg(2, "INFO", "createRemoteDirectories(".$utc_start.", ".$band.", ".$proj_id.")");

  my $remote_archive_dir = $cfg{"SERVER_ARCHIVE_NFS_MNT"};
  my $remote_results_dir = $cfg{"SERVER_RESULTS_NFS_MNT"};
  my $localhost = Dada::getHostMachineName();
  my $cmd = "";
  my $dir = "";
  my $user_groups = "";

  # Ensure each directory is automounted
  if (!( -d  $remote_archive_dir)) {
    `ls $remote_archive_dir >& /dev/null`;
  }
  if (!( -d  $remote_results_dir)) {
    `ls $remote_results_dir >& /dev/null`;
  }

  # If the remote archives dir did not yet exist for some strange reason
  if (! -d $remote_archive_dir."/".$utc_start ) {
    $cmd = "mkdir -p ".$remote_archive_dir."/".$utc_start;
    logMsg(2, "INFO", $cmd);
    system($cmd);
  }

  # Create the nfs soft link to the local archives directory 
  $cmd = "ln -s /nfs/".$localhost."/".$user."/archives/".$utc_start."/".$band.
          " ".$remote_archive_dir."/".$utc_start."/".$band;
  logMsg(2, "INFO", $cmd);
  system($cmd);

  $cmd = "mkdir -p ".$remote_results_dir."/".$utc_start."/".$band;
  logMsg(2, "INFO", $cmd);
  system($cmd);

  # Check whether the user is a member of the specified group
  $user_groups = `groups $user`;
  chomp $user_groups;

  if ($user_groups =~ m/$proj_id/) {
    logMsg(2, "INFO", "Chmodding to ".$proj_id);
  } else {
    logMsg(0, "WARN", "Project ".$proj_id." was not an ".$user." group. Using '".$user."' instead'");
    $proj_id = $user;
  }

  # Adjust permission on remote archive directory
  $dir = $remote_archive_dir."/".$utc_start;
  $cmd = "chgrp -R ".$proj_id." ".$dir;
  logMsg(2, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chgrp remote archive dir \"".
               $dir."\" to \"".$proj_id."\"");
  }

  $cmd = "chmod -R g+s ".$dir;
  logMsg(2, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chmod remote archive dir \"".
               $dir."\" to \"".$proj_id."\"");
  }

  # Adjust permission on remote results directory
  $dir = $remote_results_dir."/".$utc_start;
  $cmd = "chgrp -R ".$proj_id." ".$dir;
  logMsg(2, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chgrp remote results dir \"".
               $dir."/".$utc_start."\" to \"".$proj_id."\"");
  }

  $cmd = "chmod -R g+s ".$dir;
  logMsg(2, "INFO", $cmd);
  `$cmd`;
  if ($? != 0) {
    logMsg(0, "WARN", "Failed to chmod remote results dir \"".
               $dir."\" to \"".$proj_id."\"");
  }

}

  
#
# Create the local directories required for this observation
#
sub createLocalDirectories($$$$) {

  (my $utc_start, my $centre_freq, my $proj_id, my $raw_header) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $file = "";

  my $results_dir = $cfg{"CLIENT_RESULTS_DIR"}."/".$utc_start."/".$centre_freq;
  my $archive_dir = $cfg{"CLIENT_ARCHIVE_DIR"}."/".$utc_start."/".$centre_freq;

  # Create the results dir
  logMsg(2, "INFO", "Creating local results dir: ".$results_dir);
  $cmd = "mkdir -p ".$results_dir;
  logMsg(2, "INFO", "createLocalDirectories: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "createLocalDirectories: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0,"ERROR", "Could not create local results dir: ".$response);
  }

  # Create the archive dir
  logMsg(2, "INFO", "Creating local results dir: ".$archive_dir);
  $cmd = "mkdir -p ".$archive_dir;
  logMsg(2, "INFO", "createLocalDirectories: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "createLocalDirectories: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0,"ERROR", "Could not create local archive dir: ".$response);
  }

  # create an obs.start file in the archive dir
  $file = $archive_dir."/obs.start";
  open(FH,">".$file.".tmp");
  print FH $raw_header;
  close FH;
  rename($file.".tmp",$file);

  # set the GID on the UTC_START dirs
  $results_dir = $cfg{"CLIENT_RESULTS_DIR"}."/".$utc_start;
  $archive_dir = $cfg{"CLIENT_ARCHIVE_DIR"}."/".$utc_start;

  # Set GID on these dirs
  $cmd = "chgrp -R $proj_id $results_dir $archive_dir";
  logMsg(2, "INFO", "createLocalDirectories: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "createLocalDirectories: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "WARN", "chgrp to ".$proj_id." failed on $results_dir $archive_dir");
  }

  # Set group sticky bit on local archive dir
  $cmd = "chmod -R g+s $results_dir $archive_dir";
  logMsg(2, "INFO", "createLocalDirectories: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "createLocalDirectories: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "WARN", "chmod g+s failed on $results_dir $archive_dir");
  }

  return $file;

}



#
# Copies the UTC_START file via NFS to the server's results directory
# utcstart file resides in clients archive dir
#
sub copyObsStart($$$) {

  my ($utc_start, $centre_freq, $obs_start) = @_;

  my $dir = "";
  my $cmd = ""; 
  my $result = "";
  my $response = "";

  # Ensure each directory is automounted
  $cmd = "ls ".$cfg{"SERVER_RESULTS_NFS_MNT"}." >& /dev/null";
  ($result, $response) = Dada::mySystem($cmd);

  # Create the full nfs destinations
  $dir = $cfg{"SERVER_RESULTS_NFS_MNT"}."/".$utc_start."/".$centre_freq;

  $cmd = "cp ".$obs_start." ".$dir."/";
  logMsg(2, "INFO", "NFS copy \"".$cmd."\"");
  ($result, $response) = Dada::mySystem($cmd,0);

  if ($result ne "ok") {
    logMsg(0, "ERROR", "NFS copy failed: ".$obs_start." to ".$dir.", response: ".$response);
    logMsg(0, "ERROR", "NFS cmd: ".$cmd);
    if (-f $obs_start) {
      logMsg(0, "ERROR", "File existed locally");
    } else {
      logMsg(0, "ERROR", "File did not exist locally");
    }
  }

  logMsg(2, "INFO", "Server directories perpared");

}


#
# Run the level setting cmd with the specified number of channels
#
sub digimonThread($$) {

  my ($cmd, $nchan) = @_;

  logMsg(2, "INFO", "digimonThread: starting");

  # Add the data block key
  $cmd .= " ".$cfg{"VIEWING_DB_KEY"};
  $cmd .= " | ".$cfg{"SCRIPTS_DIR"}."/".$gain_controller." ".$nchan;
  logMsg(2, "INFO", "Running digimon: $cmd");

  my $returnVal = system($cmd);

  if ($returnVal != 0) {
    logMsg(0, "WARN", "digimonThread: digimon failed: ".$cmd." ".$returnVal);
  }

  logMsg(2, "INFO", "digimonThread: exiting");

  return $returnVal;
}


#
# If there are no rawdata files, touch band.finished
#
sub touchBandFinished($$) {

  my ($utc_start, $centre_freq) = @_;
  logMsg(2, "INFO", "touchBandFinished(".$utc_start.", ".$centre_freq.")");

  my $dir = "";
  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "find ".$cfg{"CLIENT_RECORDING_DIR"}." -maxdepth 1 -name '".$utc_start."*.dada' | wc -l";
  logMsg(2, "INFO", "touchBandFinished: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "touchBandFinished: ".$result." ".$response);

  if (($result eq "ok") && ($response == "0")) {

    # Ensure the results directory is mounted
    $cmd = "ls ".$cfg{"SERVER_RESULTS_NFS_MNT"}." >& /dev/null";
    ($result, $response) = Dada::mySystem($cmd);
  
    # Create the full nfs destinations
    $dir = $cfg{"SERVER_RESULTS_NFS_MNT"}."/".$utc_start."/".$centre_freq;
  
    $cmd = "touch ".$dir."/band.finished";
    logMsg(2, "INFO", "touchBandFinished: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd ,0);
    logMsg(2, "INFO", "touchBandFinished: ".$result." ".$response);

  }
  
}


#
# logs a message to the nexus logger and prints to stdout
#
sub logMsg($$$) {

  my ($level, $type, $msg) = @_;
  if ($level <= $dl) {
    my $time = Dada::getCurrentDadaTime();
    if (! $log_sock ) {
      print "opening nexus log: ".$log_host.":".$log_port."\n";
      $log_sock = Dada::nexusLogOpen($log_host, $log_port);
    }
    if ($log_sock) {
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "obs mngr", $msg);
    }
    print "[".$time."] ".$msg."\n";
  }
}


#
# Handle a SIGINT or SIGTERM
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  
  # Tell threads to try and quit
  $quit_daemon = 1;
  sleep(3);
  
  if ($log_sock) {
    close($log_sock);
  } 
  
  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}

#
# Handle a SIGPIPE
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  $log_sock = 0;
  if ($log_host && $log_port) {
    $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  }

}

#
# Test to ensure all module variables are set before main
#
sub good($) {

  my ($quit_file) = @_;

  # check the quit file does not exist on startup
  if (-f $quit_file) {
    return ("fail", "Error: quit file ".$quit_file." existed at startup");
  }

  # the calling script must have set this
  if (! defined($cfg{"INSTRUMENT"})) {
    return ("fail", "Error: package global hash cfg was uninitialized");
  }

  if ( ($daemon_name eq "") || ($user eq "") || ($dada_header_cmd eq "") || 
       ($client_logger eq "")) {
    return ("fail", "Error: a package variable missing [daemon_name, user, dada_header_cmd, client_logger]");
  }

  # Ensure more than one copy of this daemon is not running
  my ($result, $response) = Dada::checkScriptIsUnique(basename($0));
  if ($result ne "ok") {
    return ($result, $response);
  }

  return ("ok", "");

}


END { }

1;  # return value from file
