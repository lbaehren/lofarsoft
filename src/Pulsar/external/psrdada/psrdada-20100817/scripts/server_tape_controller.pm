package Dada::server_tape_controller;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use threads;
use threads::shared;
use IO::Socket;
use IO::Select;
use Net::hostent;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $daemon_name $pid $pid_report_port $db_dir $dest_script $dest_host $dest_user %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our $pid;
our $pid_report_port;
our $db_dir;
our $dest_script;
our $dest_host;
our $dest_user;
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
$daemon_name = 0;
$pid = "";
$pid_report_port = 0;
$db_dir = "";
$dest_script = "";
$dest_host = "";
$dest_user = "";
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

  my $pid_file    = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file   = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $log_file    = $cfg{"SERVER_LOG_DIR"}."/".$daemon_name.".log";

  my $control_thread = 0;
  my $pid_report_thread = 0;
  my $result = "";
  my $response = "";
  my $cmd = "";
  my $user = "";
  my $host = "";
  my $dir = "";

  # Every 60 seconds, check for the existence of the script on the remote machine
  my $counter_freq = 60;
  my $counter = 0;
  my $premature_exit = 0;

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

  Dada::logMsg(0, $dl ,"STARTING SCRIPT");

  Dada::logMsg(1, $dl, "Clearing status warn/error files");
  unlink($warn); 
  unlink($error); 

  # Start the tape script (background daemon)
  ($user, $host, $dir) = split(/:/, $db_dir);

  # Set if a daemon is running there already
  Dada::logMsg(2, $dl, "main: checkRemoteScript(".$user.", ".$host.")");
  ($result, $response) = checkRemoteScript($user, $host);
  Dada::logMsg(2, $dl, "main: checkRemoteScript() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($error, "could not contact ".$dest_script.", quitting");
    $quit_daemon = 1;
    unlink($pid_file);
    return 1;
  }

  if (($result eq "ok") && ($response eq "process existed")) {
    Dada::logMsgWarn($error, "tape archiver script already running, quitting");
    unlink($pid_file);
    return 1;
  }

  # start the control thread
  Dada::logMsg(2, $dl ,"starting controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # Start the PID reporting thread
  $pid_report_thread = threads->new(\&pidReportThread, $pid);

  # Start the remote tape script
  Dada::logMsg(1, $dl, "Starting ".$dest_script);

  ($result, $response) = sshViaProxy($user, $host, $dest_script." ".$pid);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, $user."@".$host.":".$dest_script." returned ".$response);
    $premature_exit = 1;
  }

  Dada::logMsg(2, $dl, "main: entering poll loop for quit file/SIGINT");


  # Poll for the existence of the control file
  while ((!$quit_daemon) && (!$premature_exit)) {

    Dada::logMsg(3, $dl, "main: Sleeping for quit_daemon");
    sleep(1);

    if ($counter == $counter_freq) {
      $counter = 0;

      Dada::logMsg(2, $dl, "main: checkRemoteScript(".$user.", ".$host.")");
      ($result, $response) = checkRemoteScript($user, $host);
      Dada::logMsg(2, $dl, "main: checkRemoteScript() ".$result." ".$response);

      # Occaisionally we loose the ssh connection to swin, this is not
      # a premature exit
      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "Could not ssh to ".$user."@".$host.", sleeping 60 seconds");
        my $sleep_count = 60;
        while ((!$quit_daemon) && ($sleep_count > 0)) {
          sleep(1);
          $sleep_count--;
        } 

      } elsif ($response eq "no process existed") {

        Dada::logMsgWarn($error, "remote script exited unexpectedly");
        $premature_exit = 1;

      } else {
        # everything ok

      }

    } else {
      $counter++;
    }
  }

  # If we are exiting and it was not a premature exit
  if (!$premature_exit) {

    # Now stop the tape script
    Dada::logMsg(1, $dl, "main: stopping ".$dest_script);

    $cmd = "touch \$DADA_ROOT/control/".Dada::daemonBaseName($dest_script).".quit";
    Dada::logMsg(2, $dl, "main: sshViaProxy(".$user.", ".$host.",". $cmd.")");
    ($result, $response) = sshViaProxy($user, $host, $cmd);
    Dada::logMsg(2, $dl, "main: sshViaProxy() ".$result." ".$response);

    sleep(5);

    # We need to allow lots of time for the script to stop. This could be as long
    # as it takes to write ~17 GB to tape (17 * 1024)/60 == 300 seconds

    my $remote_daemon_exited = 0;
    my $n_tries = 60;

    # Since the daemon can take quite some time to exit, we need to be leniant here
    while ((!$remote_daemon_exited) && ($n_tries > 0)) {

      ($result, $response) = checkRemoteScript($user, $host);

      if ($result ne "ok") {
        Dada::logMsgWarn($error, "could not contact ".$user."@".$host.":".$dest_script.", quitting");
        $remote_daemon_exited = 1;
      } else {

        if ($response eq "process existed") {
          Dada::logMsg(1, $dl, "main: waiting for ".$user."@".$host.":".$dest_script." to quit");
          sleep(5);
          $n_tries--;
          } else {
          $remote_daemon_exited = 1;
        }
      }
    }

    if ($n_tries == 0) {
      Dada::logMsgWarn($error, $user."@".$host.":".$dest_script." did not exit after 300 seconds, quitting");
    }

    # Now stop the tape script
    Dada::logMsg(1, $dl, "main: unlinking quit file on remote machine");
    $cmd = "unlink \$DADA_ROOT/control/".Dada::daemonBaseName($dest_script).".quit";
    Dada::logMsg(2, $dl, "main: sshViaProxy(".$user.", ".$host.",". $cmd.")");
    ($result, $response) = sshViaProxy($user, $host, $cmd);
    Dada::logMsg(2, $dl, "main: sshViaProxy() ".$result." ".$response);

  }

  $quit_daemon = 1;

  # Rejoin our daemon control thread
  $control_thread->join();

  $pid_report_thread->join();

  Dada::logMsg(0, $dl ,"STOPPING SCRIPT");

  return 0;

}

#
# Checks to see if the tape archiving script is running on the remote host
#
sub checkRemoteScript($$) {

  my ($user, $host) = @_;

  my $cmd = "ps aux | grep perl | grep -v grep | grep -c ".$dest_script;
  my $result = "";
  my $response = "";

  Dada::logMsg(2, $dl, "checkRemoteScript: ".$cmd);
  ($result, $response) = sshViaProxy($user, $host, $cmd);
  Dada::logMsg(2, $dl, "checkRemoteScript: ".$result." ".$response);

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
  Dada::logMsg(2, $dl, "sshViaProxy(".$user.", ".$host.", ".$remote_cmd.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  if ($dest_host ne "") {
    $cmd = "ssh -x -l ".$user." ".$host." 'ssh -x -l ".$dest_user." ".$dest_host." \"".$remote_cmd."\"'";
  } else {
    $cmd = "ssh -x -l ".$user." ".$host." '".$remote_cmd."'";
  }

  Dada::logMsg(2, $dl, "sshViaProxy: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "sshViaProxy: ".$result." ".$response);

  return ($result, $response);

}


#
# Reports the PID the script was launched with on a socket
#
sub pidReportThread($) {

  (my $daemon_pid) = @_;

  Dada::logMsg(1, $dl, "pidReportThread: starting");

  my $sock = 0;
  my $host = $cfg{"SERVER_HOST"};
  my $port = $pid_report_port;
  my $handle = 0;
  my $string = "";
  my $rh = 0;

  $sock = new IO::Socket::INET (
    LocalHost => $host,
    LocalPort => $port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );

  if (!$sock) {
    Dada::logMsgWarn($warn, "Could not create PID reporting socket [".$host.":".$port."]: ".$!);

  } else {

    my $read_set = new IO::Select();
    $read_set->add($sock);

    while (!$quit_daemon) {

      # Get all the readable handles from the server
      my ($rh_set) = IO::Select->select($read_set, undef, undef, 1);

      foreach $rh (@$rh_set) {

        if ($rh == $sock) {

          $handle = $rh->accept();
          $handle->autoflush();
          Dada::logMsg(2, $dl, "pidReportThread: Accepting connection");

          # Add this read handle to the set
          $read_set->add($handle);
          $handle = 0;

        } else {

          $string = Dada::getLine($rh);

          if (! defined $string) {
            Dada::logMsg(2, $dl, "pidReportThread: Closing a connection");
            $read_set->remove($rh);
            close($rh);

          } else {

            Dada::logMsg(2, $dl, "pidReportThread: <- ".$string);

            if ($string eq "get_pid") {
              print $rh $daemon_pid."\r\n";
              Dada::logMsg(2, $dl, "pidReportThread: -> ".$daemon_pid);
            } else {
              Dada::logMsgWarn($warn, "pidReportThread: received unexpected string: ".$string);
            }
          }
        }
      }
    }
  }

  Dada::logMsg(1, $dl, "pidReportThread: exiting");

  return 0;
}


sub controlThread($$) {

  Dada::logMsg(1, $dl ,"controlThread: starting");

  my ($quit_file, $pid_file) = @_;

  Dada::logMsg(2, $dl ,"controlThread(".$quit_file.", ".$pid_file.")");

  # Poll for the existence of the control file
  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  # ensure the global is set
  $quit_daemon = 1;

  if ( -f $pid_file) {
    Dada::logMsg(2, $dl ,"controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    Dada::logMsgWarn($warn, "controlThread: PID file did not exist on script exit");
  }

  Dada::logMsg(1, $dl, "controlThread: exiting");

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
