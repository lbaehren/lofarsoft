package Dada::client_auxiliary_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use threads;
use threads::shared;
use IO::Socket;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $daemon_name $user $dada_header_cmd %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl : shared;
our $daemon_name : shared;
our $user : shared;
our $dada_header_cmd : shared;
our %cfg : shared;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $log_host;
our $log_port;
our $log_sock;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
$user = "";
$dada_header_cmd = 0;
%cfg = ();

#
# initialize other variables
#
$quit_daemon = 0;
$log_host = 0;
$log_port = 0;
$log_sock = 0;


###############################################################################
#
# package functions
# 

sub main() {

  my $log_file      = $cfg{"CLIENT_LOG_DIR"}."/".$daemon_name.".log";;
  my $pid_file      = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file     = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".quit";

  my $control_thread = 0;
  my $prev_header = "";
  my $result = "";
  my $response = "";
  my $cmd = "";

  $log_host = $cfg{"SERVER_HOST"};
  $log_port = $cfg{"SERVER_SYS_LOG_PORT"};

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  # install signal handlers
  $SIG{INT}  = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;

  # become a daemon
  Dada::daemonize($log_file, $pid_file);

  # Open a connection to the server_sys_monitor.pl script
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  if (!$log_sock) {
    print STDERR "Could not open log port: ".$log_host.":".$log_port."\n";
  }

  logMsg(0, "INFO", "STARTING SCRIPT");

  # Start the daemon control thread
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # Main Loop
  while (!($quit_daemon)) {

    # Run the processing thread once
    $prev_header = processingThread($prev_header);
    sleep(1);

  }

  $control_thread->join();

  logMsg(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);

  return 0;
}


sub processingThread($) {

  (my $prev_header) = @_;

  my $bindir         = Dada::getCurrentBinaryVersion();
  my $processing_dir = $cfg{"CLIENT_RESULTS_DIR"};
  my $raw_data_dir   = $cfg{"CLIENT_RECORDING_DIR"};

  my $result = "";
  my $response = "";
  my %h = ();
  my $raw_header = "";
  my $cmd = "";
  my $proc_cmd = "";
  my $node = "";

  # Get the projects/groups that the user is a member of
  my $unix_groups = `groups $user`;
  chomp $unix_groups;

  # Get the next filled header on the data block. Note that this may very
  # well hang for a long time - until the next header is written...
  $cmd = $bindir."/".$dada_header_cmd;
  logMsg(2, "INFO", "processingThread: ".$cmd);
  $raw_header = `$cmd 2>&1`;

  # since the only way to currently stop this daemon is to send a kill
  # signal to dada_header_cmd, we should check the return value
  if ($? == 0) {

    $proc_cmd = "";

    ($result, $response) = Dada::processHeader($raw_header, $cfg{"CONFIG_DIR"});

    if ($result ne "ok") {
      logMsg(0, "ERROR", $response);
      logMsg(0, "ERROR", "DADA header malformed, jettesioning xfer");  
      $proc_cmd = $bindir."/dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    } elsif ($raw_header eq $prev_header) {
      logMsg(0, "ERROR", "DADA header repeated, likely cause failed PROC_CMD, jettesioning xfer");
      $proc_cmd = $bindir."/dada_dbnull -s -k ".lc($cfg{"PROCESSING_DATA_BLOCK"});

    } else {
      logMsg(2, "INFO", "processingThread: DADA header looks correct");
      $proc_cmd = $response;
      if ($proc_cmd =~ m/dspsr/) {
        $proc_cmd .= " ".$cfg{"PROCESSING_DB_KEY"};
      }

      # Check if any auxiliary nodes are available for processing 
      logMsg(2, "INFO", "processingThread: asking for aux nodes");
      $node = auxNodesAvailable();
      logMsg(2, "INFO", "processingThread: received ".$node);

      # We got a free node, run dbnic on the xfer
      if ($node =~ /apsr(\d\d):(\d+)/) {

        logMsg(2, "INFO", "processingThread: transferring data to ".$node);
        $proc_cmd = $bindir."/dada_dbnic -k ".$cfg{"AUXILIARY_DATA_BLOCK"}." -s -N ".$node;

      # If no nodes are available, run dbdisk on the xfer
      } else {
        # Do a check if the main command was dada_dbdisk, just dbnull it
        if ($proc_cmd =~ m/dada_dbdisk/) {
          $proc_cmd =  $bindir."/dada_dbnull -s -k ".$cfg{"AUXILIARY_DATA_BLOCK"};
        } else {
          $proc_cmd = $bindir."/dada_dbdisk -k ".$cfg{"AUXILIARY_DATA_BLOCK"}." -s -D ".$raw_data_dir;
        }
      }
    }

    # Create an obs.start file in the processing dir:
    logMsg(0, "INFO", "START ".$proc_cmd);

    $response = `$proc_cmd`;

    if ($? != 0) {
      logMsg(0, "ERROR", "Aux cmd ".$proc_cmd." failed: ".$response);
    }

    logMsg(0, "INFO", "END ".$proc_cmd);

    return ($raw_header);
  
  } else {

    if (!$quit_daemon) {
      logMsg(2, "WARN", "processingThread: dada_header_cmd failed - probably no data block");
      sleep 1;
    }
    return ("");
  
  }
}


sub auxNodesAvailable() {
  
  my $host = $cfg{"SERVER_HOST"};
  my $port = $cfg{"SERVER_AUX_CLIENT_PORT"};
  my $localhost = Dada::getHostMachineName();
  
  logMsg(2, "INFO", "auxNodesAvailable: conecting to ".$host.":".$port);
  my $handle = Dada::connectToMachine($host, $port);
  
  if (!$handle) {

    logMsg(0, "WARN", "Could not connect to assisant manager ".
                  $host.":".$port);
    return "none";
  
  } else {

    logMsg(2, "INFO", "auxNodesAvailable: connected to ".$host.":".$port);

    print $handle $localhost.":help\r\n";

    logMsg(2, "INFO", "auxNodesAvailable: sent ".$localhost.":help");
    my $string = Dada::getLineSelect($handle,2);
    logMsg(2, "INFO", "auxNodesAvailable: received ".$string);

    close($handle);
    logMsg(2, "INFO", "auxNodesAvailable: closed handle");
    return $string;
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
  $cmd = "ps aux | grep -v grep | grep ".$user." | grep '".$dada_header_cmd.
          "' | awk '{print \$2}'";
  logMsg(2, "INFO", "controlThread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  $response =~ s/\n/ /;
  logMsg(2, "INFO", "controlThread: ".$result." ".$response);
  if ($result eq "ok") {
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
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "aux mngr", $msg);
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

  if ( ($daemon_name eq "") || ($dada_header_cmd eq "") || ($user eq "")) {
    return ("fail", "Error: a package variable missing [daemon_name dada_header_cmd user]");
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
