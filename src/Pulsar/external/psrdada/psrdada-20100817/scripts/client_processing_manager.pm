package Dada::client_processing_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use Dada;
use strict;
use warnings;
use threads;         # standard perl threads
use threads::shared; # standard perl threads
use IO::Socket;      # Standard perl socket library
use IO::Select;      # Allows select polling on a socket
use Net::hostent;
use File::Basename;


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
  @EXPORT_OK   = qw($user $dl $daemon_name $dada_header_cmd client_logger %cfg);

}

our @EXPORT_OK;

#
# Constants
#
use constant NUM_CPUS        => 8;

#
# exported package globals
#
our $user;
our $dl : shared;
our $daemon_name : shared;
our $dada_header_cmd;
our $client_logger;
our %cfg : shared;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $log_host;
our $log_port;
our $log_sock;
our $dspsr_start_time : shared;   # Flag to indcate if dspsr is running
our $dada_header_pid : shared;    # PID of the dada_header command for killing
our $processor_pid : shared;      # PID of the processor (dspsr) for killing


#
# initialize package globals
#
$user = "";
$dl = 1; 
$daemon_name = 0;
$dada_header_cmd = 0;
$client_logger = 0;
%cfg = ();

#
# initialize other variables
#
$quit_daemon = 0;
$log_host = 0;
$log_port = 0;
$log_sock = 0;
$dspsr_start_time = 0;
$dada_header_pid = 0;
$processor_pid = 0;


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
  my $prev_header = "";
  my $quit = 0;
  my $load_control_thread = 0;
  my $result = "";
  my $response = "";

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR "ERROR failed to start: ".$response."\n";
    return 1;
  }

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;

  # become a daemon
  Dada::daemonize($log_file, $pid_file);
  
  # open a connection to the nexus logging port
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  if (!$log_sock) {
    print STDERR "Could open log port: ".$log_host.":".$log_port."\n";
  }

  logMsg(0,"INFO", "STARTING SCRIPT");

  # start the control thread
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # start the load monitoring thread
  $load_control_thread  = threads->new(\&loadControlThread);

  # Main Loop
  while(!$quit_daemon) {

    # Run the processing thread once
    ($quit_daemon, $prev_header) = processing_thread($prev_header);

  }

  logMsg(2, "INFO", "main: joining threads");
  $load_control_thread->join();
  logMsg(2, "INFO", "main: loadControlThread joined");
  $control_thread->join();
  logMsg(2, "INFO", "main: controlThread joined");

  logMsg(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);

  return 0;
}

sub processing_thread($) {

  (my $prev_header) = @_;

  my $bindir = Dada::getCurrentBinaryVersion();
  my $proc_log = $cfg{"CLIENT_LOG_DIR"}."/dspsr.log";
  my $processing_dir = $cfg{"CLIENT_RESULTS_DIR"};
  my $raw_header = "";
  my $cmd = "";
  my $result = "";
  my $response = "";
  my %h = ();

  # Get the next filled header on the data block. Note that this may very
  # well hang for a long time - until the next header is written...
  $cmd =  $bindir."/".$dada_header_cmd;
  logMsg(2, "INFO", "Running ".$cmd);
  $raw_header = `$cmd 2>&1`;
  logMsg(2, "INFO", $cmd." returned");

  # since the only way to currently stop this daemon is to send a kill
  # signal to dada_header_cmd, we should check the return value
  if ($? == 0) {

    my $proc_cmd = "";

    ($result, $response) = Dada::processHeader($raw_header, $cfg{"CONFIG_DIR"}); 

    if ($result ne "ok") {
      logMsg(0, "ERROR", $response);
      logMsg(0, "ERROR", "DADA header malformed, jettesioning xfer");  
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    } elsif ($raw_header eq $prev_header) {
      logMsg(0, "ERROR", "DADA header repeated, likely cause failed PROC_CMD, jettesioning xfer");
      $proc_cmd = "dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    } else {

      logMsg(2, "INFO", "DADA header looks correct");
      $proc_cmd = $response;
      if ($proc_cmd =~ m/dspsr/) {
        $proc_cmd .= " ".$cfg{"PROCESSING_DB_KEY"};
      }

      %h = Dada::headerToHash($raw_header);

      $processing_dir .= "/".$h{"UTC_START"}."/".$h{"FREQ"};

      # This should not be requried as the observation manager should be creating
      # this directory for us
      if (! -d ($processing_dir)) {
        logMsg(0, "WARN", "The archive directory was not created by the ".
                  "observation manager: \"".$processing_dir."\"");
        system("mkdir -p ".$processing_dir);
        system("chmod g+s ".$processing_dir);

        # Check whether the user is a member of the specified group
        my $proj_id = $h{"PID"};
        my $user_groups = `groups`;
        chomp $user_groups;

        if ($user_groups =~ m/$proj_id/) {
          logMsg(2, "INFO", "chgrp to ".$proj_id);
        } else {
          logMsg(0, "WARN", "Project ".$proj_id." was not an ".$user." group. Using '".$user."' instead'");
          $proj_id = $user;
        }
         system("chgrp -R ".$proj_id." ".$processing_dir);
      }
    }

    # Create an obs.start file in the processing dir:
    logMsg(1, "INFO", "START ".$proc_cmd);

    chdir $processing_dir;
    $dspsr_start_time = time;
    logMsg(2, "INFO", "Setting dspsr_start_time = ".$dspsr_start_time);

    $cmd = $bindir."/".$proc_cmd;
    $cmd .= " 2>&1 | ".$cfg{"SCRIPTS_DIR"}."/".$client_logger;

    logMsg(1, "INFO", "Cmd: ".$cmd);

    my $returnVal = system($cmd);

    if ($returnVal != 0) {
      logMsg(0, "WARN", "Processing command failed: ".$?." ".$returnVal);
    }

    logMsg(1, "INFO", "END ".$proc_cmd);

    $dspsr_start_time = 0;
    logMsg(2, "INFO", "Setting dspsr_start_time = ".$dspsr_start_time);

    return (0, $raw_header);

  } else {

    if (!$quit_daemon) {
      logMsg(0, "WARN", "dada_header_cmd failed");
      sleep 1;
    }
    return (1, "");

  }
}

sub controlThread($$) {

  my ($quit_file, $pid_file) = @_;

  logMsg(2, "INFO", "controlThread: starting (".$quit_file.", ".$pid_file.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  # poll for the existence of the quit_file or the global quit variable
  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  $quit_daemon = 1;

  # Kill the dada_header command
  $cmd = "ps aux | grep -v grep | grep ".$user." | grep '".$dada_header_cmd."' | awk '{print \$2}'";
  logMsg(2, "INFO", " controlThread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", " controlThread: ".$result." ".$response);
  $response =~ s/\n/ /;
  if (($result eq "ok") && ($response ne "")) {
    $response =~ s/\n/ /;
    $cmd = "kill -KILL ".$response;
    logMsg(1, "INFO", "controlThread: Killing dada_header: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "controlThread: ".$result." ".$response);
  }

  # Kill all running dspsr commands
  $cmd = "ps aux | grep -v grep | grep ".$user." | grep dspsr | awk '{print \$2}'";
  logMsg(2, "INFO", "controlThread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  $response =~ s/\n/ /;
  logMsg(2, "INFO", "controlThread: ".$result." ".$response);
  if (($result eq "ok") && ($response ne "")) {
    $cmd = "killall -KILL ".$response;
    logMsg(1, "INFO", "Killing all dspsr ".$cmd);
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
# If dspsr has been running for 1 minute, return the number of free processors
# based on the 1 minute load. 
#
sub loadControlThread($) {

  logMsg(2, "INFO", "loadControlThread: starting");

  my $localhost = 0;
  my $server = 0;
  my $rh = 0;
  my $result = "";
  my $response = "";
  my $handle = 0;
  my $hostinfo = 0;
  my $hostname = "";
  my $taking_data = "some";
  my $num_cores_available = 0;
  my $load = 0;
 
  $localhost = Dada::getHostMachineName();

  # create a listening server socket
  $server = new IO::Socket::INET (
    LocalHost => $localhost, 
    LocalPort => $cfg{"CLIENT_PROC_LOAD_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1);

  my $read_set = new IO::Select();  # create handle set for reading
  $read_set->add($server);   # add the main socket to the set

  while (!$quit_daemon) {

    # Get all the readable handles from the server
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);                                                                                 
    foreach $rh (@$readable_handles) {

      logMsg(3, "INFO", "loadControlThread: checking read handle");

      if ($rh == $server) {
                                                                                
        $handle = $rh->accept();
        $handle->autoflush(1);
        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;

        logMsg(2, "INFO", "loadControlThread: accepting connection from". $hostname);

        # Add this read handle to the set
        $read_set->add($handle);
        $handle = 0;

      } else {

        my $string = Dada::getLine($rh);

        if (! defined $string) {
          logMsg(3, "INFO", "loadControlThread: removing read handle");
          $read_set->remove($rh);
          logMsg(3, "INFO", "loadControlThread: closing socket");
          close($rh);

        } else {

          logMsg(2, "INFO", "loadControlThread: received ".$string);

          $num_cores_available = 0;

          # find out if we are still taking data
          $handle = Dada::connectToMachine($localhost, $cfg{"CLIENT_BG_PROC_PORT"});

          # Assume we are taking data
          $taking_data = "some";

          if ($handle) {

            logMsg(2, "INFO", "loadControlThread: obs mngr connection established");

            print $handle "is data currently being received?\r\n";
            $taking_data = Dada::getLine($handle);
            logMsg(2, "INFO", "loadControlThread: obs mngr replied ".$taking_data);
            $handle->close();
            $handle = 0;

          } else {
            # we assume that we are not taking data
            logMsg(0, "WARN", "Could not connect to obs mngr ".$localhost.":".
                              $cfg{"CLIENT_BG_PROC_PORT"});
            $taking_data = "none";
          }

          # if we are taking data, be sensitive to the machines current load
          if ($taking_data ne "none") {

            logMsg(2, "INFO", "loadControlThread: receiving data");

            # if dspsr has been running for more than 120 seconds
            if (($dspsr_start_time+120) < time) {

              logMsg(2, "INFO", "loadControlThread: dspsr running > 120 seconds");

              $load = int(Dada::getLoad("one"));

              # always keep 1 core free
              $num_cores_available = ($cfg{"CLIENT_NUM_CORES"} - (1+$load));

            # We have not been running for a minute, 0 cores available
            } else {
              logMsg(2, "INFO", "loadControlThread: dspsr running < 120 seconds");
              $num_cores_available = 0;

            }

          # we are not currently taking data, but check if dspsr is running
          } else {

            logMsg(2, "INFO", "loadControlThread: not receving data");

            if ($dspsr_start_time) {

              logMsg(2, "INFO", "loadControlThread: dspsr is running");

              $load = int(Dada::getLoad("one"));

              # always keep 1 core free
              $num_cores_available = ($cfg{"CLIENT_NUM_CORES"}- (1+$load));

            } else {
              logMsg(2, "INFO", "loadControlThread: dspsr not running");
              $num_cores_available = $cfg{"CLIENT_NUM_CORES"};
            }
          }

          logMsg(2, "INFO", "loadControlThread: replying ".$num_cores_available.
                            " cores available");

          print $rh $num_cores_available."\r\n";

        }
      }
    }
  }

  logMsg(2, "INFO", "loadControlThread: exiting");
}

#
# logs a message to the nexus logger and prints to stdout
#
sub logMsg($$$) {

  my ($level, $type, $msg) = @_;
  if ($level <= $dl) {
    my $time = Dada::getCurrentDadaTime();
    if (! $log_sock ) {
      $log_sock = Dada::nexusLogOpen($log_host, $log_port);
    }
    if ($log_sock) {
      Dada::nexusLogMessage($log_sock, $time, "src", $type, "proc mngr", $msg);
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

  # check required gloabl parameters
  if ( ($user eq "") || ($dada_header_cmd eq "") || ($client_logger eq "")) {
    return ("fail", "Error: a package variable missing [user, dada_header_cmd, client_logger]");
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
