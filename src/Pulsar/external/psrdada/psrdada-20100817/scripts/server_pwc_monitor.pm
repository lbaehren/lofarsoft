package Dada::server_pwc_monitor;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use IO::Socket;
use IO::Select;
use Net::hostent;
use threads;
use threads::shared;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $log_host $log_port $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $log_host;
our $log_port;
our $daemon_name;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $warn;
our $error;

#
# initialize package globals
#
$dl = 1; 
$log_host = "";
$log_port = 0;
$daemon_name = 0;
%cfg = ();

#
# initialize other variables
#
$warn = ""; 
$error = ""; 
$quit_daemon = 0;

###############################################################################
#
# package functions
# 

sub main() {

  $warn  = $cfg{"STATUS_DIR"}."/".$daemon_name.".warn";
  $error = $cfg{"STATUS_DIR"}."/".$daemon_name.".error";

  my $pid_file  = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $log_file  = $cfg{"SERVER_LOG_DIR"}."/".$daemon_name.".log";
  my $control_thread = 0;
  my $read_set = 0;
  my $rh = 0;
  my $readable_handles = ();
  my $handle = 0;
  my $hostname = "";
  my $hostinfo = 0;
  my $host = "";
  my $line = "";
  my $result = "";
  my $response = "";
  my $timeout = 0;

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;
  
  # become a daemon
  Dada::daemonize($log_file, $pid_file);

  Dada::logMsg(0, $dl, "STARTING SCRIPT");

  # start the control thread
  Dada::logMsg(2, $dl, "starting controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  Dada::logMsg(1, $dl, "Connecting to dada_pwc_command ".$log_host.":".$log_port);

  while (!$quit_daemon) {

    # If we have lost the connection, try to reconnect to the PWCC (nexus)
    while (!$handle && !$quit_daemon) {

      Dada::logMsg(2, $dl, "Connecting to ".$log_host.":".$log_port);

      $handle = Dada::connectToMachine($log_host, $log_port);

      if (!$handle)  {
        sleep(1);
        Dada::logMsg(2, $dl, "Failed to connect to dada_pwc_command ".$log_host.":".$log_port);
      } else {
        Dada::logMsg(1, $dl, "Connected to dada_pwc_command ".$log_host.":".$log_port);

      }
    }

    # create a read set for selecting on the socket
    if ($handle) {
      $read_set = new IO::Select($handle);
    }

    while ($handle && !$quit_daemon) {

      $timeout = 1;
      ($readable_handles) = IO::Select->select($read_set, undef, undef, $timeout);

      $line = "";
      foreach $rh (@$readable_handles) {
        $line = Dada::getLine($rh);
      }

      # If we have lost the connection
      if (! defined $line) {

        Dada::logMsg(1, $dl, "Lost connection from dada_pwc_command ".$log_host.":".$log_port);
        $handle->close();
        $handle = 0;

      } elsif ($line eq "") {
        # nothing was read from the socket 

      } elsif ($line eq "null") {

        Dada::logMsg(3, $dl, "Received null line");

      } else {

        Dada::logMsg(2, $dl, "Received: ".$line);
        logMessage($line);

      }
    }
  
    if (!$quit_daemon) {
      sleep(1);
    }
  }

  # Rejoin our daemon control thread
  $control_thread->join();

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");

  return 0;

}


# 
# logs a message to the status dir if warn/error, NOT the scripts' log
#
sub logMessage($) {

  my ($line) = @_;

  my $host = "";
  my $rest = "";
  my $time = "";
  my $msg = "";
  my $status_file = "";
  my $status_dir =  $cfg{"STATUS_DIR"};
  my $result = "";
  my $response = "";
  my $cmd = "";

  # determine the source machine
  ($host, $rest) =  split(/: \[/, $line, 2);
  $time = substr($rest,0,19);
  $msg  = substr($rest,21);

  # If contains a warning message
  if ($msg =~ /WARN: /) {
    $status_file = $status_dir."/".$host.".pwc.warn" ;

  # If contains an error message
  } elsif ($msg =~ /ERR:/) {
    $status_file = $status_dir."/".$host.".pwc.error" ;

  # If we are starting a new obs, delete the error and warn files 
  } elsif ($msg =~ /STATE = prepared/) {
    $cmd = "rm -f ".$status_dir."/*.warn ".$status_dir."/*.error";
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "failed to delted warn and error status files: ".$response);
    }

  } else {
    # We ignore the message
  }

  if ($status_file ne "") {
    if (-f $status_file) {
      open(FH,">>".$status_file);
    } else {
      open(FH,">".$status_file);
    }
    if ($? != 0) {
      Dada::logMsg(0, $dl, "Could not open status_file: ".$status_file);
    }

    print FH $msg."\n";
    close FH;
    Dada::logMsg(1,  $dl, "Logged: ".$host.", ".$time.", ".$msg);
  }
}

sub controlThread($$) {

  Dada::logMsg(1, $dl, "controlThread: starting");

  my ($quit_file, $pid_file) = @_;

  Dada::logMsg(2, $dl, "controlThread(".$quit_file.", ".$pid_file.")");

  # Poll for the existence of the control file
  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  # ensure the global is set
  $quit_daemon = 1;

  if ( -f $pid_file) {
    Dada::logMsg(2, $dl, "controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    Dada::logMsgWarn($warn, "controlThread: PID file did not exist on script exit");
  }

  return 0;
}
  


#
# Handle a SIGINT or SIGTERM
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(3);
  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}

# 
# Handle a SIGPIPE
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";

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

  # this script can *only* be run on the configured server
  if (index($cfg{"SERVER_ALIASES"}, Dada::getHostMachineName()) < 0 ) {
    return ("fail", "Error: script must be run on ".$cfg{"SERVER_HOST"}.
                    ", not ".Dada::getHostMachineName());
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
