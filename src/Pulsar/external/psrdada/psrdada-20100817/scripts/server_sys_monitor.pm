package Dada::server_sys_monitor;

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
our $log_sock;
our $warn;
our $error;

#
# initialize package globals
#
$dl = 1; 
$log_host = "";
$log_port = 0;
$log_sock = 0;
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
  my $handle = 0;
  my $hostname = "";
  my $hostinfo = 0;
  my $host = "";
  my $domain = "";
  my $string = "";
  my $result = "";
  my $response = "";

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

  # create a read set for handle connections and data 
  $read_set = new IO::Select();
  $read_set->add($log_sock);

  Dada::logMsg(2, $dl, "Waiting for connection on ".$log_host.":".$log_port);

  while (!$quit_daemon) {

    # Get all the readable handles from the log_sock 
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);

    foreach $rh (@$readable_handles) {

      # if it is the main socket then we have an incoming connection and
      # we should accept() it and then add the new socket to the $Read_Handles_Object
      if ($rh == $log_sock) {

        $handle = $rh->accept();
        $handle->autoflush();
        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;

        Dada::logMsg(2, $dl, "Accepting connection from ". $hostname);

        # Add this read handle to the set
        $read_set->add($handle);
        $handle = 0;

      } else {

        $hostinfo = gethostbyaddr($rh->peeraddr);
        $hostname = $hostinfo->name;
        ($host, $domain) = split(/\./,$hostname,2);
        $string = Dada::getLine($rh);

        if (! defined $string) {
          $read_set->remove($rh);
          close($rh);
        } else {
          Dada::logMsg(2, $dl, "Received String \"".$string."\"");
          logMessage($host, $string);
        }
      }
    }
  }

  # Rejoin our daemon control thread
  $control_thread->join();

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");

  close($log_sock);

  return 0;

}


#
# logs a message to the desginated log file, NOT the scripts' log
#
sub logMessage($$) {

  (my $machine, my $string) = @_;

  my $statusfile_dir = $cfg{"STATUS_DIR"};
  my $logfile_dir    = $cfg{"SERVER_LOG_DIR"};
  my $status_file = "";
  my $host_log_file = "";
  my $combined_log_file = "";
  my $time = "";
  my $tag = "";
  my $lvl = "";
  my $src = "";
  my $msg = "";

  # determine the source machine
  ($time, $tag, $lvl, $src, $msg) = split(/\|/,$string,5);
  my @array = split(/\|/,$string,5);

  $host_log_file = $logfile_dir."/".$machine.".".$tag.".log";
  $combined_log_file = $logfile_dir."/nexus.".$tag.".log";

  if ($lvl eq "WARN") {
    $status_file = $statusfile_dir."/".$machine.".".$tag.".warn";
  } 
  if ($lvl eq "ERROR") {
    $status_file = $statusfile_dir."/".$machine.".".$tag.".error";
  }

  # Log the message to the hosts' log file
  if ($host_log_file ne "") {
    if (-f $host_log_file) {
      open(FH,">>".$host_log_file);
    } else {
      open(FH,">".$host_log_file);
    }

    if ($lvl eq "INFO") {
      print FH "[".$time."] ".$src.": ".$msg."\n";
    } else {
      print FH "[".$time."] ".$src.": ".$lvl.": ".$msg."\n";
    }
    close FH;
  }

  # Log the message to the combined log file
  if (-f $combined_log_file) {
    open(FH,">>".$combined_log_file);
  } else {
    open(FH,">".$combined_log_file);
  }


  if (($lvl eq "WARN") || ($lvl eq "ERROR")) {
    print FH $machine." [".$time."] ".$lvl." ".$src.": ".$msg."\n";  
  } else  {
    print FH $machine." [".$time."] ".$src.": ".$msg."\n";
  }
  
  close FH;

  # If the file is a warning or error, we create a warn/error file too
  if ($status_file ne "") {
    if (-f $status_file) {
      open(FH,">>".$status_file);
    } else {
      open(FH,">".$status_file);
    }
    print FH $src.": ".$msg."\n";
    close FH;
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

  $log_sock = new IO::Socket::INET (
    LocalHost => $log_host,
    LocalPort => $log_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$log_sock) {
    return ("fail", "Could not create listening socket: ".$log_host.":".$log_port);
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
