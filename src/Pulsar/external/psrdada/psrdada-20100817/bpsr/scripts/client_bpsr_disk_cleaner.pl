#!/usr/bin/env perl

#
# Author:   Andrew Jameson
# Created:  28 Jan 2009
# 
# This daemons deletes observations slowly from the local disk that 
# have been written to tape in all required locations
#


use lib $ENV{"DADA_ROOT"}."/bin";

#
# Include Modules
#
use Bpsr;            # DADA Module for configuration options
use strict;          # strict mode (like -Wall)
use threads;         # standard perl threads
use threads::shared; # standard perl threads
use Net::hostent;


#
# Constants
#
use constant DAEMON_NAME        => "bpsr_disk_cleaner";

#
# Global Variable Declarations
#
our $dl : shared = 1;
our $quit_daemon : shared = 0;
our %cfg : shared = Bpsr::getBpsrConfig();	# dada.cfg in a hash
our $log_host = ""; 
our $log_port = 0; 
our $log_sock = 0;

#
# Local Variable Declarations
#
my $log_file = "";
my $pid_file = "";
my $quit_file = "";
my $control_thread = 0;
my $result = "";
my $response = "";
my $obs = "";
my $beam = "";

# install signal handlers
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

$log_host = $cfg{"SERVER_HOST"};
$log_port = $cfg{"SERVER_SYS_LOG_PORT"};

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

    ($result, $response, $obs, $beam) = findCompletedBeam($cfg{"CLIENT_ARCHIVE_DIR"});

    if (($result eq "ok") && ($obs ne "none")) {
      logMsg(1, "INFO", "Deleting ".$obs."/".$beam);
      ($result, $response) = deleteCompletedBeam($cfg{"CLIENT_ARCHIVE_DIR"}, $obs, $beam);
    } else {
      logMsg(2, "INFO", "Found no beams to delete ".$obs."/".$beam);
    }

    my $counter = 24;
    logMsg(2, "INFO", "Sleeping ".($counter*5)." seconds");
    while ((!$quit_daemon) && ($counter > 0) && ($obs eq "none")) {
      sleep(5);
      $counter--;
    }

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
# Find an obs/beam that has the required on.tape.* flags set
#
sub findCompletedBeam($) {

  (my $archives_dir) = @_;

  logMsg(2, "INFO", "findCompletedBeam(".$archives_dir.")");

  my $found_obs = 0;
  my $result = "";
  my $response = ""; 
  my $obs = "none";
  my $beam = "none";
  my $cmd = "";
  my $i = 0;
  my $k = "";   # obs
  my $source = "";  # source
  my $pid = "";       # first letter of source
  my $file = "";

  my %deleted        = ();    # obs that have a beam.deleted
  my %on_tape_swin   = ();    # obs that have been sent.to.swin
  my %on_tape_parkes = ();    # obs that have been sent.to.parkes
  my %on_tape        = ();    # obs that have been sent.to.*
  my @pids = ();
  my %pid_dests = ();

  my @array = ();

  $cmd = "find ".$archives_dir." -maxdepth 3 -name beam.deleted".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBeam: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBeam: ".$result." ".$response);
  if ($result eq "ok") {
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      $deleted{$array[$i]} = 1;
    }
  }

  $cmd = "find ".$archives_dir." -maxdepth 3 -name on.tape.swin".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBeam: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBeam: ".$result." ".$response);
  if ($result eq "ok") {
    @array = ();
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      if (!defined($deleted{$array[$i]})) {
        $on_tape_swin{$array[$i]} = 1;
        $on_tape{$array[$i]} = 1;
      }
    }
  }


  @array = ();
  $cmd = "find ".$archives_dir." -maxdepth 3 -name on.tape.parkes".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBeam: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBeam: ".$result." ".$response);
  if ($result eq "ok") {
    @array = ();
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      if (!defined($deleted{$array[$i]})) {
        $on_tape_parkes{$array[$i]} = 1;
        if (!defined($on_tape{$array[$i]})) {
          $on_tape{$array[$i]} = 1;
        }
      }
    }
  }
  @array = ();

  @pids = Dada::getProjectGroups("bpsr");

  for ($i=0; $i<=$#pids; $i++) {
    $pid_dests{$pids[$i]} = $cfg{$pids[$i]."_DEST"};
    logMsg(2, "INFO", "findCompletedBeam: pid_dests{".$pids[$i]."} = ".$pid_dests{$pids[$i]});
  }

  my @keys = sort keys %on_tape;
  my $k = "";
  my $want_swin = 0;
  my $want_parkes = 0;
  my $on_swin = 0;
  my $on_parkes = 0;
  my $space = 0;

  logMsg(2, "INFO", "findCompletedBeam: ".($#keys+1)." observations to consider");

  for ($i=0; ((!$quit_daemon) && (!$found_obs) && ($i<=$#keys)); $i++) {

    $k = $keys[$i];

    # check the type of the observation
    $file = $archives_dir."/".$k."/obs.start";
    logMsg(3, "INFO", "findCompletedBeam: testing for existence: ". $file);
    if (-f $file) {

      $cmd = "grep ^SOURCE ".$file." | awk '{print \$2}'";
      logMsg(3, "INFO", "findCompletedBeam: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(3, "INFO", "findCompletedBeam: ".$result." ".$response);

      if ($result ne "ok") {
        logMsg(0, "WARN", "findCompletedBeam could not extract SOURCE from obs.start");
        next;
      } 

      $source = $response;
      chomp $source;

      logMsg(3, "INFO", "findCompletedBeam: SOURCE=".$source);

      $cmd = "grep ^PID ".$file." | awk '{print \$2}'";
      logMsg(3, "INFO", "findCompletedBeam: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(3, "INFO", "findCompletedBeam: ".$result." ".$response);
  
      if ($result ne "ok") {
        logMsg(0, "WARN", "findCompletedBeam: could not extract PID from obs.start");
        next;
      }
      $pid = $response;

      # determine the required destinations based on PID
      logMsg(3, "INFO", "findCompletedBeam: getObsDestinations(".$pid.", ".$pid_dests{$pid}.")");
      ($want_swin, $want_parkes) = Bpsr::getObsDestinations($pid, $pid_dests{$pid});
      logMsg(3, "INFO", "findCompletedBeam: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);

      $found_obs = 1;

      $on_swin = (defined($on_tape_swin{$k})) ? 1 : 0;
      $on_parkes = (defined($on_tape_parkes{$k})) ? 1 : 0;

      logMsg(2, "INFO", "findCompletedBeam: ".$k." SOURCE=".$source.", PID=".$pid.
                 ", SWIN=".$on_swin."/".$want_swin.", PARKES=".$on_parkes."/".$want_parkes);

      if ($want_swin && (!$on_swin)) {
        $found_obs = 0;
        logMsg(2, "INFO", "findCompletedBeam: ".$k." [".$source."] on.tape.swin missing");
      }

      if ($want_parkes && (!$on_parkes)) {
        $found_obs = 0;
        logMsg(2, "INFO", "findCompletedBeam: ".$k." [".$source."] on.tape.parkes missing");
      }

      if ($found_obs) {

        $cmd = "du -sh ".$archives_dir."/".$k." | awk '{print \$1}'";
        logMsg(2, "INFO", "findCompletedBeam: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "findCompletedBeam: ".$result." ".$response);
        $space = $response;
        
        logMsg(2, "INFO", "findCompletedBeam: found ".$k." PID=".$pid.", SIZE=".$space);
        ($obs, $beam) = split(/\//, $k);

      }
    
    } else {
      if (-f $archives_dir."/".$k."/obs.deleted") {
        $cmd = "touch ".$archives_dir."/".$k."/beam.deleted";
        logMsg(2, "INFO", "findCompletedBeam: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "findCompletedBeam: ".$result." ".$response);

      } else {
        logMsg(1, "INFO", "findCompletedBeam: file did not exist :".$file);
      }
    }
  } 

  $result = "ok";
  $response = "";

  logMsg(2, "INFO", "findCompletedBeam ".$result.", ".$response.", ".$obs.", ".$beam);

  return ($result, $response, $obs, $beam);
}


#
# Delete the specified obs/beam 
#
sub deleteCompletedBeam($$) {

  my ($dir, $obs, $beam) = @_;

  my $result = "";
  my $response = "";
  my $path = $dir."/".$obs."/".$beam;

  logMsg(2, "INFO", "Deleting archived files in ".$path);

  if (-f $path."/slow_rm.ls") {
    unlink $path."/slow_rm.ls";
  }

  my $cmd = "find ".$path." -name '*.fil' -o -name 'aux.tar' -o -name '*.ar' -o -name '*.png' -o -name '*.bp*' -o -name '*.ts?' > ".$path."/slow_rm.ls";

  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", $result." ".$response);

  if ($result ne "ok") {
    logMsg(0, "ERROR", "deleteCompletedBeam: find command failed: ".$response);
    return ("fail", "find command failed");
  }

  chomp $response;
  my $files = $response;

  $files =~ s/\n/ /g;
  $cmd = "slow_rm -r 256 -M ".$path."/slow_rm.ls";

  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  if ($result ne "ok") {
    logMsg(1, "WARN", $result." ".$response);
  }

  if (-d $path."/aux") {
    rmdir $path."/aux";
  }

  $cmd = "touch ".$path."/beam.deleted";
  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", $result." ".$response);

  # If the old style UTC_START/BEAM/obs.deleted existed, change it to a beam.deleted
  if (-f $path."/obs.deleted") {
    unlink $path."/obs.deleted";
  }

  if (-f $path."/slow_rm.ls") {
    unlink $path."/slow_rm.ls";
  }

  $result = "ok";
  $response = "";

  return ($result, $response);
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
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "cleaner", $msg);
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

