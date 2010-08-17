package Dada::client_disk_cleaner;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use threads;
use threads::shared;
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
  @EXPORT_OK   = qw($dl $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl : shared;
our $daemon_name : shared;
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

  $log_host         = $cfg{"SERVER_HOST"};
  $log_port         = $cfg{"SERVER_SYS_LOG_PORT"};

  my $control_thread = 0;
  my $result = "";
  my $response = "";
  my $cmd = "";
  my $obs = "";
  my $band = "";

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

    logMsg(2, "INFO", "main: findCompletedBand(".$cfg{"CLIENT_ARCHIVE_DIR"}.")");
    ($result, $response, $obs, $band) = findCompletedBand($cfg{"CLIENT_ARCHIVE_DIR"});
    logMsg(2, "INFO", "main: findCompletedBand() ".$result." ".$response." ".$obs." ".$band);

    if (($result eq "ok") && ($obs ne "none")) {

      logMsg(2, "INFO", "main: checkBandArchives(".$cfg{"CLIENT_ARCHIVE_DIR"}.", ".$obs.", ".$band.")");
      ($result, $response) = checkBandArchives($cfg{"CLIENT_ARCHIVE_DIR"}, $obs, $band);
      logMsg(2, "INFO", "main: checkBandArchives() ".$result." ".$response);

      if ($result eq "ok") {

        logMsg(2, "INFO", "main: deleteCompletedBand(".$cfg{"CLIENT_ARCHIVE_DIR"}.", ".$obs.", ".$band.")");
        ($result, $response) = deleteCompletedBand($cfg{"CLIENT_ARCHIVE_DIR"}, $obs, $band);
        logMsg(2, "INFO", "main: deleteCompletedBand() ".$result." ".$response);

        if ($result ne "ok") {
          logMsg(0, "ERROR", "Failed to delete ".$obs."/".$band.": ".$response);
          $quit_daemon = 1;

        } else {
          logMsg(1, "INFO", $obs."/".$band.": transferred -> deleted");
        }
      } else {
        logMsg(0, "ERROR", "Checking of archives before deletion failed");
        $quit_daemon = 1;
      }
    } else {
      logMsg(2, "INFO", "Found no bands to delete ".$obs."/".$band);
    }

    my $counter = 24;
    logMsg(2, "INFO", "Sleeping ".($counter*5)." seconds");
    while ((!$quit_daemon) && ($counter > 0) && ($obs eq "none")) {
      sleep(5);
      $counter--;
    }

  }

  $control_thread->join();

  logMsg(0, "INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);

  return 0;
}


#
# Find an obs/band that has the required sent.to.* flags set
#
sub findCompletedBand($) {

  (my $archives_dir) = @_;
  
  logMsg(2, "INFO", "findCompletedBand(".$archives_dir.")");
  
  my $found_obs = 0;
  my $result = "";
  my $response = "";
  my $obs = "none";
  my $band = "none";
  my $cmd = "";
  my $i = 0;
  my $k = "";   # obs
  my $source = "";  # source
  my $pid = "";       # first letter of source
  my $file = "";
  
  my %deleted        = ();    # obs that have a band.deleted
  my %sent_to_swin   = ();    # obs that have been sent.to.swin
  my %sent_to_parkes = ();    # obs that have been sent.to.parkes
  my %sent_to        = ();    # obs that have been sent.to.*
  my @pids = ();
  my %pid_dests = ();
  
  my @array = ();
  
  $cmd = "find ".$archives_dir." -maxdepth 3 -name band.deleted".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBand: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBand: ".$result." ".$response);
  if ($result eq "ok") {
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      $deleted{$array[$i]} = 1;
    }
  } 
  
  $cmd = "find ".$archives_dir." -maxdepth 3 -name sent.to.swin".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBand: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBand: ".$result." ".$response);
  if ($result eq "ok") {
    @array = ();
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      if (!defined($deleted{$array[$i]})) {
        $sent_to_swin{$array[$i]} = 1;
        $sent_to{$array[$i]} = 1;
      }
    }
  }

  @array = ();
  $cmd = "find ".$archives_dir." -maxdepth 3 -name sent.to.parkes".
         " -printf '\%h\\n' | awk -F/ '{print \$(NF-1)\"\/\"\$NF}' | sort";
  logMsg(3, "INFO", "findCompletedBand: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "findCompletedBand: ".$result." ".$response);
  if ($result eq "ok") {
    @array = ();
    @array= split(/\n/, $response);
    for ($i=0; $i<=$#array; $i++) {
      if (!defined($deleted{$array[$i]})) {
        $sent_to_parkes{$array[$i]} = 1;
        if (!defined($sent_to{$array[$i]})) {
          $sent_to{$array[$i]} = 1;
        }
      }
    }
  }

  @array = ();
  @pids = Dada::getProjectGroups("apsr");

  for ($i=0; $i<=$#pids; $i++) {
    if (!(exists($cfg{$pids[$i]."_DEST"}))) {
      logMsg(0, "WARN", "findCompletedBand: missing config param: ".$pids[$i]."_DEST");
    } else {
      $pid_dests{$pids[$i]} = $cfg{$pids[$i]."_DEST"};
      logMsg(2, "INFO", "findCompletedBand: pid_dests{".$pids[$i]."} = ".$pid_dests{$pids[$i]});
    }
  }

  my @keys = sort keys %sent_to;
  my $want_swin = 0;
  my $want_parkes = 0;
  my $on_swin = 0;
  my $on_parkes = 0;
  my $space = 0;

  logMsg(2, "INFO", "findCompletedBand: ".($#keys+1)." observations to consider");

  for ($i=0; ((!$quit_daemon) && (!$found_obs) && ($i<=$#keys)); $i++) {

    $k = $keys[$i];

    # check the type of the observation
    $file = $archives_dir."/".$k."/obs.start";
    logMsg(3, "INFO", "findCompletedBand: testing for existence: ". $file);
    if (-f $file) {

      $cmd = "grep ^SOURCE ".$file." | awk '{print \$2}'";
      logMsg(3, "INFO", "findCompletedBand: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(3, "INFO", "findCompletedBand: ".$result." ".$response);

      if ($result ne "ok") {
        logMsg(0, "WARN", "findCompletedBand could not extract SOURCE from obs.start");
        next;
      }

      $source = $response;
      chomp $source;
      logMsg(3, "INFO", "findCompletedBand: SOURCE=".$source);

      $cmd = "grep ^PID ".$file." | awk '{print \$2}'";
      logMsg(3, "INFO", "findCompletedBand: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(3, "INFO", "findCompletedBand: ".$result." ".$response);

      if ($result ne "ok") {
        logMsg(0, "WARN", "findCompletedBand: could not extract PID from obs.start");
        next;
      }
      $pid = $response;

      # determine the required destinations based on PID
      logMsg(3, "INFO", "findCompletedBand: getObsDestinations(".$pid.", ".$pid_dests{$pid}.")");
      ($want_swin, $want_parkes) = Dada::getObsDestinations($pid, $pid_dests{$pid});
      logMsg(3, "INFO", "findCompletedBand: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);

      $found_obs = 1;

      $on_swin = (defined($sent_to_swin{$k})) ? 1 : 0;
      $on_parkes = (defined($sent_to_parkes{$k})) ? 1 : 0;

      logMsg(2, "INFO", "findCompletedBand: ".$k." SOURCE=".$source.", PID=".$pid.
                 ", SWIN=".$on_swin."/".$want_swin.", PARKES=".$on_parkes."/".$want_parkes);

      if ($want_swin && (!$on_swin)) {
        $found_obs = 0;
        logMsg(2, "INFO", "findCompletedBand: ".$k." [".$source."] sent.to.swin missing");
      }

      if ($want_parkes && (!$on_parkes)) {
        $found_obs = 0;
        logMsg(2, "INFO", "findCompletedBand: ".$k." [".$source."] sent.to.parkes missing");
      }

      if ($found_obs) {

        $cmd = "du -sh ".$archives_dir."/".$k." | awk '{print \$1}'";
        logMsg(2, "INFO", "findCompletedBand: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "findCompletedBand: ".$result." ".$response);
        $space = $response;

        logMsg(2, "INFO", "findCompletedBand: found ".$k." PID=".$pid.", SIZE=".$space);
        ($obs, $band) = split(/\//, $k);

      }

    } else {
      if (-f $archives_dir."/".$k."/obs.deleted") {
        $cmd = "touch ".$archives_dir."/".$k."/band.deleted";
        logMsg(2, "INFO", "findCompletedBand: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMsg(2, "INFO", "findCompletedBand: ".$result." ".$response);

      } else {
        logMsg(1, "INFO", "findCompletedBand: file did not exist :".$file);
      }
    }
  }

  $result = "ok";
  $response = "";

  logMsg(2, "INFO", "findCompletedBand ".$result.", ".$response.", ".$obs.", ".$band);

  return ($result, $response, $obs, $band);
}

#
# Check whether any the band.?res files are fully up to date
#
sub checkBandArchives($$$) {

  my ($dir, $obs, $band) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $file = "";
  my $proc_file = "";
  my $path = $dir."/".$obs."/".$band;
  my @sources = ();
  my @multi_sources = ();
  my $n_archives = 0;
  my $n_subints = 0;
  my $i = 0;

  # check for single pulse
  $file = $path."/obs.start";
  $cmd = "grep ^PROC_FILE ".$file." | awk '{print \$2}'";
  logMsg(3, "INFO", "checkBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "checkBandArchives: ".$result." ".$response);

  if ($result ne "ok") {
    logMsg(0, "WARN", "checkBandArchives: could not extract PROC_FILE from obs.start: ".$response);
    return ("fail", "Could not read PROC_FILE from obs.start");
  }
  $proc_file = $response;
  chomp $proc_file;
  logMsg(3, "INFO", "checkBandArchives: PROC_FILE=".$proc_file);
  if ($proc_file =~ m/single/) {
    return ("ok", "single pulses");
  }

  # get any subdirs [i.e. multifold] that may exist
  $cmd = "find ".$path." -mindepth 1 -type d -printf '%f\n'";
  logMsg(3, "INFO", "checkBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "checkBandArchives: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "WARN", "checkBandArchives: could not check if multifold: ".$response);
    return ("fail", "could not check if multifold");
  }
  
  if ($response eq "") {
    push @sources, $obs."/".$band;
  } else {
    @multi_sources = split(/\n/, $response);
    for ($i=0; $i<=$#multi_sources; $i++) {
      push @sources, $obs."/".$band."/".$multi_sources[$i];
    }
  }
 
  for ($i=0; $i<=$#sources; $i++) {

    $path = $dir."/".$sources[$i];

    # count the archives
    $cmd = "find ".$path." -name '*.ar' | wc -l";
    logMsg(3, "INFO", "checkBandArchives: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(3, "INFO", "checkBandArchives: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "checkBandArchives: could not count archives for ".$path.": ".$response);
      return ("fail", "could not count archives for ".$path);
    }
    $n_archives = $response;

    # get the subints in the tres archive 
    if ( -f $path."/band.tres" ) {
      $cmd = "psredit -c nsubint -Q ".$path."/band.tres | awk '{print \$2}'";
      logMsg(3, "INFO", "checkBandArchives: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(3, "INFO", "checkBandArchives: ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", "checkBandArchives: could not count subints in band.tres for ".$path.": ".$response);
        return ("fail", "could not count subints in band.tres for ".$path);
      }
      $n_subints = $response;
    } else {
      $n_subints = 0;
    }

    # If we need to regenerate this archive
    logMsg(2, "INFO", "checkBandArchives: n_subints=".$n_subints.", n_archives=".$n_archives);
    if ($n_subints != $n_archives) {
      logMsg(0, "WARN", "Regenerating band.?res for ".$path." [".$n_subints." != ".$n_archives."]");
      ($result, $response) = regenBandArchives($path);
      logMsg(2, "INFO", "regenBandArchives() ".$result." ".$response);
      if ($result ne "ok") {
        return ("fail", "Could not regenerate band archives for ".$path);
      }
    }
  }
  return ("ok", "");
}


#
# Regenerates the band.?res archives for the specified path
#
sub regenBandArchives($) 
{
  my ($dir) = @_;
  logMsg(2, "INFO", "regenBandArchives(".$dir.")");

  my $result = "";
  my $response = "";
  my $cmd = "";
  my $fres_archive = $dir."/band.fres";
  my $tres_archive = $dir."/band.tres";

  $cmd = "find -L ".$dir." -name '2*.ar'";
  logMsg(2, "INFO", "regenBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "regenBandArchives: ".$result." ".$response);

  if ($result ne "ok") {
    logMsg(0, "ERROR", "regenBandArchives: ".$cmd." failed: ".$response);
    return ("fail", "find command failed");
  }
  if ($response eq "") {
    logMsg(0, "ERROR", "regenBandArchives: ".$cmd." returned 0 archives");
    return ("fail", "find command returned no archives to combine");
  }
  
  if (-f $dir."/archives.list" ) {
    unlink $dir."/archives.list";
  }
  if (-f $fres_archive) {
    unlink($fres_archive);
  }
  if (-f $tres_archive) {
    unlink($tres_archive);
  }
  
  # generate a META file list of the filenames to be processed
  $cmd = "find ".$dir." -name '2*.ar' | sort > ".$dir."/archives.list";
  logMsg(2, "INFO", "regenBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "regenBandArchives: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "ERROR", "regenBandArchives: ".$cmd." failed: ".$response);
    return ("fail", "failed to generate archives list");
  }
  
  # Add the archive to the T scrunched total
  $cmd = "psradd -T -f ".$fres_archive." -M ".$dir."/archives.list";
  logMsg(2, "INFO", "regenBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "regenBandArchives: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "ERROR", $cmd." failed: ".$response);
    return ("fail", "psradd for fres failed");
  }
  
  # Add the archive to the F scrunched total
  $cmd = "psradd -jF -f ".$tres_archive." -M ".$dir."/archives.list";
  logMsg(2, "INFO", "regenBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "regenBandArchives: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "ERROR", $cmd." failed: ".$response);
    return ("fail", "psradd for fres failed");
  }
  
  if (-f $dir."/archives.list" ) {
    unlink $dir."/archives.list";
  }

  return ("ok","");
}



#
# Delete the specified obs/band 
#
sub deleteCompletedBand($$$) {

  my ($dir, $obs, $band) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $path = $dir."/".$obs."/".$band;

  logMsg(2, "INFO", "Deleting archived files in ".$path);

  if (-f $path."/slow_rm.ls") {
    $cmd = "rm -f ".$path."/slow_rm.ls";
    logMsg(2, "INFO", "deleteCompletedBand: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "deleteCompletedBand: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "ERROR", "deleteCompletedBand: ".$cmd." failed: ".$response);
      return ("fail", "failed to delete ".$path."/slow_rm.ls");
    }
  }

  $cmd = "find ".$path." -name '*.ar' -o -name '*.tar' > ".$path."/slow_rm.ls";
  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", $result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "ERROR", "deleteCompletedBand: find command failed: ".$response);
    return ("fail", "find command failed");
  }

  chomp $response;
  my $files = $response;

  $files =~ s/\n/ /g;
  $cmd = "slow_rm -r 256 -M ".$path."/slow_rm.ls";

  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "deleteCompletedBand: ".$result." ".$response);
  if ($result ne "ok") {
    logMsg(0, "ERROR", "deleteCompletedBand: ".$cmd." failed: ".$response);
    return ("fail", "failed to delete files for ".$obs."/".$band);
  }

  $cmd = "touch ".$path."/band.deleted";
  logMsg(2, "INFO", "deleteCompletedBand: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "deleteCompletedBand: ".$result." ".$response);

  # If the old style UTC_START/band/obs.deleted existed, change it to a band.deleted
  if (-f $path."/obs.deleted") {
    $cmd = "rm -f ".$path."/obs.deleted";
    logMsg(2, "INFO", "deleteCompletedBand: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "deleteCompletedBand: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "ERROR", "deleteCompletedBand: ".$cmd." failed: ".$response);
      return ("fail", "failed to delete ".$path."/obs.deleted");
    }
  }

  if (-f $path."/slow_rm.ls") {
     $cmd = "rm -f ".$path."/slow_rm.ls";
    logMsg(2, "INFO", "deleteCompletedBand: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "deleteCompletedBand: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "ERROR", "deleteCompletedBand: ".$cmd." failed: ".$response);
      return ("fail", "failed to delete ".$path."/slow_rm.ls");
    }
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
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "cleaner", $msg);
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

  if ($daemon_name eq "") {
    return ("fail", "Error: a package variable missing [daemon_name]");
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
