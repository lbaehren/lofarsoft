#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_transfer_manager.pl
#
# Transfers obsevation/beam to the swinburne and parkes holding areas for 
# subsequent tape archival
#

use lib $ENV{"DADA_ROOT"}."/bin";

#
# Required Modules
#
use strict;
use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use File::Basename;
use threads;
use threads::shared;
use Bpsr;

sub usage() {
  print "Usage: ".basename($0)." PID\n";
  print "   PID   Project ID. The bpsr user must be a member of this unix group\n";
}

#
# Sanity check to prevent multiple copies of this daemon running
#
Dada::preventDuplicateDaemon(basename($0));


#
# Constants
#
use constant DL            => 1;
use constant PIDFILE       => "bpsr_transfer_manager.pid";
use constant LOGFILE       => "bpsr_transfer_manager.log";
use constant QUITFILE      => "bpsr_transfer_manager.quit";
use constant DATA_RATE     => 600;  # Mbits / sec
use constant TCP_WINDOW    => 1700;
use constant VSIB_MIN_PORT => 23301;
use constant VSIB_MAX_PORT => 23326;
use constant SSH_OPTS      => "-x -o BatchMode=yes";

#
# Global Variables
#
our %cfg   = Bpsr::getBpsrConfig();   # Bpsr.cfg
our $error = $cfg{"STATUS_DIR"}."/bpsr_transfer_manager.error";
our $warn  = $cfg{"STATUS_DIR"}."/bpsr_transfer_manager.warn";
our $quit_daemon : shared  = 0;
our $pid : shared = "";

# For interrupting and killing in the signal handlers
our $s_user    : shared = "";      # swinburne user
our $s_host    : shared = "";      # swinburne host
our $s_dir     : shared = "";      # swinburne dir
our $p_user    : shared = "";      # parkes user
our $p_host    : shared = "";      # parkes host
our $p_dir     : shared = "";      # parkes dir
our $curr_obs  : shared = "none";  # current utc
our $curr_beam : shared = "none";  # current beam
our $send_host : shared = "";      # current sending host
our $send_user : shared = "bpsr";  # current sending user


# Autoflush output
$| = 1;

# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;


if ($#ARGV != 0) {
  usage();
  exit(1);
}

$pid  = $ARGV[0];

#
# Local Varaibles
#
my $logfile = $cfg{"SERVER_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
my $daemon_control_thread = 0;
my $pid_report_thread = 0;
my $obs = "";
my $beam = "";
my $result;
my $response;
my $i=0;

#
# Holding areas for later tape archival
#
my @s_disks = ();
my @p_disks = ();

for ($i=0; $i<$cfg{"NUM_SWIN_DIRS"}; $i++) {
  push (@s_disks, $cfg{"SWIN_DIR_".$i});
}

for ($i=0; $i<$cfg{"NUM_PARKES_DIRS"}; $i++) {
  push (@p_disks, $cfg{"PARKES_DIR_".$i});
}

#
# Main
#

Dada::daemonize($logfile, $pidfile);
Dada::logMsg(0, DL, "STARTING SCRIPT");

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

# Start the PID reporting thread
$pid_report_thread = threads->new(\&pidReportThread, $pid);

setStatus("Starting script");

my $s_i = 0;
my $p_i = 0;

# my $s_start_disk = 0;
# my $p_start_disk = 0;

# Ensure that destination directories exist for this project
($result, $response) = checkDestinations(\@s_disks);
if ($result ne "ok") {
  $quit_daemon = 1;
  sleep(3);
  exit(1);
}

# Ensure that destination directories exist for this project
($result, $response) = checkDestinations(\@p_disks);
if ($result ne "ok") {
  $quit_daemon = 1;
  sleep(3);
  exit(1);
}


chdir $cfg{"SERVER_ARCHIVE_NFS_MNT"};

# Ensure no vsib processes running on sending machines, srv's or remote's
interrupt_all(\@s_disks, \@p_disks);

#
# Main loop
#

# We cycle through ports to prevent "Address already in use" problems
my $vsib_port = VSIB_MIN_PORT;

my $is_fold = 0;
my $cmd = "";
my $path = "";
my $xfer_failure = 0;
my $dest_string = "";
my $files = "";
my $counter = 0;
my $go_swin = 0;
my $go_parkes = 0;
my $is_fold = 0;

# On startup do a check for fully transferred or fully archived obvserations
Dada::logMsg(1, DL, "Checking for fully transferred observations");
($result, $response) = checkFullyTransferred();
Dada::logMsg(2, DL, "main: checkFullyTransferred(): ".$result.":".$response);

Dada::logMsg(1, DL, "Checking for fully archived observations");
($result, $response) = checkFullyArchived();
Dada::logMsg(2, DL, "main: checkFullyArchived(): ".$result.":".$response);

Dada::logMsg(1, DL, "Checking for fully deleted observations");
($result, $response) = checkFullyDeleted();
Dada::logMsg(2, DL, "main: checkFullyDeleted(): ".$result.":".$response);

Dada::logMsg(1, DL, "Starting Main [".$pid."]");

while (!$quit_daemon) {

  # See if we can write to either place...
  # This is important otherwise we can get stuck in a loop waiting for one place
  ($s_user, $s_host, $s_dir) = findHoldingArea(\@s_disks, $s_i);
  ($p_user, $p_host, $p_dir) = findHoldingArea(\@p_disks, $p_i);

  Dada::logMsg(2, DL, "Swin holding area: ".$s_host.":".$s_dir);
  Dada::logMsg(2, DL, "Parkes holding area: ".$p_host.":".$p_dir);

  # Find the optimal beam to send to swin/parkes
  Dada::logMsg(2, DL, "main: getBeamToSend(". $s_host.", ".$p_host.")");
  ($obs, $beam, $go_swin, $go_parkes, $is_fold) = getBeamToSend($s_host, $p_host);
  Dada::logMsg(2, DL, "main: getBeamToSend(): ".$obs.", ".$beam.", ".$go_swin.", ".$go_parkes.", ".$is_fold);

  if ($is_fold) {
    Dada::logMsg(2, DL, "main: ".$obs." is a folded observation");
    $s_dir .= "/".$pid."/pulsars";
    $p_dir .= "/".$pid."/pulsars";
  } else {
    $s_dir .= "/".$pid."/staging_area";
    $p_dir .= "/".$pid."/staging_area";
  }

  # If we have something to send

  if ($quit_daemon) {

    # Quit from the loop

  } elsif (($obs ne "none") && ($obs =~ /^2/)) {

    $xfer_failure = 0;
    $dest_string = "";
    $files = "";

    # Make adjustments to the destinations based on getBeamToSend
    if ($go_swin) {
      if ($beam eq "13") {
        $s_i = ($s_i + 1) % ($#s_disks+1);
      }
      $dest_string .= $s_host." ";
    } else {
      $s_user = "none";
      $s_host = "none";
      $s_dir = "none";
    }
    if ($go_parkes) {
      if ($beam eq "13") {
        $p_i = ($p_i + 1) % ($#s_disks+1);
      }
      $dest_string .= $p_host." ";
    } else {
      $p_user = "none";
      $p_host = "none";
      $p_dir  = "none";
    }

    Dada::logMsg(2, DL, "main: swin: ".$s_user."@".$s_host.":".$s_dir);
    Dada::logMsg(2, DL, "main: parkes ".$p_user."@".$p_host.":".$p_dir);
    Dada::logMsg(2, DL, "main: dest_string ".$dest_string);
  
    setStatus($obs."/".$beam." &rarr; ".$dest_string);

    # If at least one destination is available
    if (($p_host ne "none") || ($s_host ne "none")) {

      Dada::logMsg(2, DL, "main: getBeamFiles(".$obs.", ".$beam.")");
      $files = getBeamFiles($obs, $beam);
      Dada::logMsg(2, DL, "main: getBeamFiles: ".$files);

      $path = $obs."/".$beam;

      # Transfer the files to 
      ($vsib_port, $xfer_failure) = transferBeam($obs, $beam, $vsib_port, $files, $p_user, $p_host, $p_dir, $s_user, $s_host, $s_dir);

      # If we have been asked to quit
      if ($quit_daemon) {
        Dada::logMsg(2, DL, "main: asked to quit");

      } else {

        # Test to see if this beam was sent correctly
        if (!$xfer_failure) {

          if ($p_host ne "none") {
            ($result, $response) = checkTransferredFiles($p_user, $p_host, $p_dir, $files);
            if ($result eq "ok") {
              Dada::logMsg(0, DL, "Successfully transferred to parkes: ".$path);
              recordTransferResult("sent.to.parkes", $path);
              markRemoteCompleted("xfer.complete", $p_user, $p_host, $p_dir, $path);
            } else {
              $xfer_failure = 1;
              Dada::logMsgWarn($warn, "Failed to transfer ".$path." to parkes: ".$response);


    my $cmd = "mv ".$path."/obs.start ".$path."/obs.bad";
    Dada::logMsg(1, DL, $cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(1, DL, $result." ".$response);

            }
          }
          if ($s_host ne "none") {
            ($result, $response) = checkTransferredFiles($s_user, $s_host, $s_dir, $files);
            if ($result eq "ok") {
              Dada::logMsg(0, DL, "Successfully transferred to swin: ".$path);
              recordTransferResult("sent.to.swin", $path);
              markRemoteCompleted("xfer.complete", $s_user, $s_host, $s_dir, $path);
            } else {
              $xfer_failure = 1;
              Dada::logMsgWarn($warn, "Failed to transfer ".$path." to swin: ".$response);
            }
          }
        }

        # If we have a failed transfer, mark the /nfs/archives as failed
        if ($xfer_failure) {
          setStatus("WARN: ".$path." xfer failed");
          if ($p_host ne "none") {
            recordTransferResult("error.to.parkes", $path);
          }
          if ($s_host ne "none") {
            recordTransferResult("error.to.swin", $path);
          }
        } else {
          setStatus($path." xfer success");
        }
        $curr_obs = "none";
        $curr_beam = "none";
      }

      # Check if all beams have been trasnferred successfully, if so, mark 
      # the observation as sent.to.dest
      Dada::logMsg(2, DL, "main: checkAllBeams(".$obs.")");
      ($result, $response) = checkAllBeams($obs);
      Dada::logMsg(2, DL, "main: checkAllBeams: ".$result." ".$response);

      if ($result ne "ok") { 
        Dada::logMsgWarn($warn, "main: checkAllBeams failed: ".$response);

      } else {
        if ($response ne "all beams sent") {
          Dada::logMsg(2, DL, "Obs ".$obs." not fully transferred: ".$response);

        } else {
          Dada::logMsg(0, DL, "Successfully transferred to swin & parkes: ".$obs);
          recordTransferResult("obs.transferred", $obs);
          unlink ($obs."/obs.finished");
          setStatus($obs." xfer success");

          # Dada::logMsg(2, DL, "Checking for fully transferred observations");
          # ($result, $response) = checkFullyTransferred();
          # Dada::logMsg(2, DL, "Checking for fully archived observations");
          # ($result, $response) = checkFullyArchived();
          # Dada::logMsg(2, DL, "Checking for fully deleted observations");
          # ($result, $response) = checkFullyDeleted();
        }
      }

    } else {
      Dada::logMsg(0, DL, "Obs ".$obs." is ready to send, but swin & parkes unavailable to receive");
      setStatus("Warn: no dest available");
    }

  } else {

    Dada::logMsg(2, DL, "Checking for fully transferred observations");
    ($result, $response) = checkFullyTransferred();
    Dada::logMsg(2, DL, "Checking for fully archived observations");
    ($result, $response) = checkFullyArchived();
    Dada::logMsg(2, DL, "Checking for fully deleted observations");
    ($result, $response) = checkFullyDeleted();

    setStatus("Waiting for obs");
    Dada::logMsg(2, DL, "Sleeping 60 seconds");

    $counter = 12;
    while ((!$quit_daemon) && ($counter > 0)) {
      sleep(5);
      $counter--;
    }
  }

}

# rejoin threads
$daemon_control_thread->join();

# rejoin threads
$pid_report_thread->join();

setStatus("Script stopped");
Dada::logMsg(0, DL, "STOPPING SCRIPT");

exit 0;



###############################################################################
#
# Functions
#


#
# Ensure the files sent matches the local
#
sub checkTransferredFiles($$$$) {

  my ($user, $host, $dir, $files) = @_;

  Dada::logMsg(2, DL, "checkRemoteArchive(".$user.", ".$host.", ".$dir.", ".$files.")");

  my $cmd = "ls -l ".$files." | awk '{print \$5\" \"\$9}' | sort";

  Dada::logMsg(2, DL, "checkRemoteArchive: ".$cmd);
  my $local_list = `$cmd`;
  if ($? != 0) {
    Dada::logMsgWarn($warn, "checkRemoteArchive: local find failed ".$local_list);
  }

  $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; ls -l ".$files."\" | awk '{print \$5\" \"\$9}' | sort";
  
  Dada::logMsg(2, DL, "checkRemoteArchive: ".$cmd);
  my $remote_list = `$cmd`;

  if ($? != 0) {
    Dada::logMsgWarn($warn, "checkRemoteArchive: remote find failed ".$remote_list);
  }

  # Regardless of transfer success, remove the WRITIING flag
  $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; rm -f ../../../WRITING\"";
  Dada::logMsg(2, DL, "checkRemoteArchive: ".$cmd);
  system($cmd);
  if ($? != 0) {
    Dada::logMsgWarn($warn, "checkRemoteArchive: could not remove remote WRITING file");
  }

  if (($local_list eq $remote_list) && ($local_list ne "")) {

    return ("ok","");
  } else {

    Dada::logMsgWarn($warn, "ARCHIVE MISMATCH: local: ".$local_list);
    Dada::logMsgWarn($warn, "ARCHIVE MISMATCH: remote: ".$remote_list);

    return ("fail", "archive mismatch");
  }

}


sub recordTransferResult($$) {

  my ($file, $dir) = @_;

  my $cmd = "sudo -u bpsr touch ".$dir."/".$file;
  Dada::logMsg(2, DL, "recordTransferResult: ".$cmd);
  my ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "recordTransferResult: ".$result." ".$response);

  return ($result, $response);
}

sub markRemoteCompleted($$$$$) {

  my ($file, $user, $host, $dir, $obs) = @_;
  Dada::logMsg(2, DL, "markRemoteCompleted(".$file.", ".$user.", ".$host.", ".$dir.", ".$obs.")");

  my $cmd = "cd ".$dir."; touch ".$obs."/".$file;
  my $ssh_cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"".$cmd."\"";

  Dada::logMsg(2, DL, "markRemoteCompleted: ".$cmd);
  my ($result, $response) = Dada::mySystem($ssh_cmd);
  Dada::logMsg(2, DL, "markRemoteCompleted: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "could not mark ".$host.":".$dir."/".$obs." as completed");
  }

  return ($result, $response);
}


sub findHoldingArea(\@$) {

  (my $disks_ref, my $startdisk) = @_;

  my @disks = @$disks_ref;
  my @disk_components = ();

  my $c=0;
  my $i=0;
  my $disk = "";
  my $user = "";
  my $host = "";
  my $path = "";
  my $cmd = "";
  my $result = "";

  # If the array as 0 size, return none
  if ($#disks == -1) {
    return ("none", "none", "none");
  }

  for ($c=0; $c<=$#disks; $c++) {
    $i = ($c + $startdisk)%($#disks+1);

    $disk = $disks[$i];
    Dada::logMsg(2, DL, "Evaluating ".$path);

    $user = "";
    $host = "";
    $path = "";

    @disk_components = split(":",$disk,3);

    if ($#disk_components == 2) {

      $user = $disk_components[0];
      $host = $disk_components[1];
      $path = $disk_components[2];

      # check for disk space on this disk
      $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"ls ".$path."\" 2>&1";
      Dada::logMsg(2, DL, $cmd);
      $result = `$cmd`;

      if ($? != 0) {
        chomp $result;
        Dada::logMsgWarn($warn, "ssh cmd '".$cmd."' failed: ".$result);
        $result = "";
      } else {

        # check if this is being used for reading
        $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"ls ".$path."/../READING\" 2>&1";
        $result = `$cmd`;
        chomp $result;
        if ($result =~ m/No such file or directory/) {

          # There is no READING file.
          $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"df ".$path." -P\" | tail -n 1";
          Dada::logMsg(2, DL, $cmd);
          $result = `$cmd`;
          if ($? != 0) {
            Dada::logMsgWarn($warn, "df command ".$cmd." failed: ".$result);
            $result = "";
          } 
        } else {
          # we are writing to the disk
          Dada::logMsg(2, DL, "Skipping disk $host:$path as the disk is being used for reading");
          $result="";
        }
      }
    } else {

      Dada::logMsgWarn($warn, "disk line syntax error ".$disk);
      $result = "";

    }

    if ($result ne "") {

      chomp($result);

      Dada::logMsg(2, DL, "df_result  = $result");

      if ($result =~ m/No such file or directory/) {

        Dada::logMsgWarn($error, $user." ".$host." ".$path." was not a valid directory");
        $result = "";

      } else {

        my ($location, $total, $used, $avail, $junk) = split(" ",$result);
        Dada::logMsg(2, DL, "used = $used, avail = $avail, total = $total");

        my $stop_percent = 0.05;
        if ($host =~ m/apsr/) {
          $stop_percent = 0.50;
        }

        if (($avail / $total) < $stop_percent) {

          Dada::logMsgWarn($warn, $host.":".$path." is over ".((1.00-$stop_percent)*100)." full");

        } else {

          # Need more than 10 Gig
          if ($avail < 10000) {

            Dada::logMsgWarn($warn, $host.":".$path." has less than 10 GB left");

          } else {
            Dada::logMsg(2, DL, "Holding area: ".$user." ".$host." ".$path);
            return ($user,$host,$path);
          }
        }
      }
    }
  }

  return ("none", "none", "none");

}


#
# Find a beam to send. Looks for observations that have an obs.finished in them
# 
sub getBeamToSend($$) {

  my ($swin, $parkes) = @_;

  Dada::logMsg(2, DL, "getBeamToSend(".$swin.", ".$parkes.")");

  my $obs_to_send= "";
  my $beam_to_send = "";
  my $cmd = "";
  my $source = "";
  my $obs_pid = "";
  my $result = "";
  my $response = "";
  my $archives_dir = $cfg{"SERVER_ARCHIVE_NFS_MNT"};

  my @obs_finished = ();
  my %list = ();
  my $find_result = "";
  my $beam = "";
  my @beams = ();
  my $i = 0;
  my $j = 0;

  # find all observations that have been finished
  $cmd = "find ".$archives_dir." -maxdepth 2 -name \"obs.finished\"".
         " -printf \"%h %T@\\n\" | sort | awk -F/ '{print \$NF}'";
  Dada::logMsg(2, DL, "getBeamToSend: ".$cmd);
  $find_result = `$cmd`;
  chomp $find_result;
  @obs_finished = split(/\n/, $find_result);

  if ($#obs_finished == -1) {
    return ("none", "none", 0, 0, 0);
  }

  my $optimal_obs = "none";
  my $optimal_beam = "none";
  my $acceptable_obs = "none";
  my $acceptable_beam = "none";
  my $suboptimal_obs = "none";
  my $suboptimal_beam = "none";
  
  my $time = 0;
  my $obs = "";

  # control flags
  my $have_obs = 0;
  my $survey_obs = 0;
  my $twobit_file = 0;
  my $swin_file = 0;
  my $parkes_file = 0;
  my $psrxml_file = 0;
  my $archive_file = 0;
  my $sent_beams = 0;

  # desried destinations
  my $want_swin = 0;
  my $want_parkes = 0;
  my $is_fold = 0;

  # Ensure NFS mounts exist for subsequent parsing
  $cmd = "ls /nfs/apsr??/ >& /dev/null";
  Dada::logMsg(2, DL, "getBeamToSend: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "getBeamToSend: ".$result.":".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "Not all NFS directories were mounted");
  }

  Dada::logMsg(2, DL, "Found (".$#obs_finished.") observations with obs.finished");

  # Go through the list of finished observations, looking for something to send
  for ($i=0; (($i<=$#obs_finished) && ($optimal_obs eq "none") && (!$have_obs)); $i++) {

    Dada::logMsg(3, DL, "getBeamToSend: checking ".$obs_finished[$i]);
    ($obs, $time) = split(/ /,$obs_finished[$i]);
    Dada::logMsg(3, DL, "getBeamToSend: time: ".$time.", obs:".$obs);

    # skip if no obs.info exists
    if (!( -f $obs."/obs.info")) {
      Dada::logMsg(0, DL, "Required file missing: ".$obs."/obs.info");
      next;
    }

    # get the PID
    $cmd = "grep ^PID ".$obs."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);
    $obs_pid = `$cmd`;
    chomp $obs_pid;
    Dada::logMsg(3, DL, "getBeamToSend: PID = ".$obs_pid);
    if ($obs_pid ne $pid) {
      Dada::logMsg(2, DL, "getBeamToSend: skipping ".$obs." as its PID [".$obs_pid."] was not ".$pid);
      next;
    }

    $cmd = "grep SOURCE ".$obs."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);
    $source = `$cmd`;
    chomp $source;
    Dada::logMsg(3, DL, "getBeamToSend: SOURCE = ".$source);

    if ($source =~ m/^G/) {
      $survey_obs = 1;
      $is_fold = 0;
    } else {
      $survey_obs = 0;
      $is_fold = 1;
    }

    # determine the required destinations based on PID
    ($want_swin, $want_parkes) = Bpsr::getObsDestinations($obs_pid, $cfg{$obs_pid."_DEST"});

    Dada::logMsg(2, DL, "getBeamToSend: ".$obs." [".$source."] want[".$want_swin.",".$want_parkes."] fold[".$is_fold."]");

    # Get the sorted list of beam nfs links for this obs
    # $cmd = "find  ".$obs." -maxdepth 1 -type l -printf '\%f\n' | sort";
    # This command will timeout on missing NFS links (6 s), but wont print ones that are missing
    $cmd = "find -L ".$obs." -mindepth 1 -maxdepth 1 -type d -printf \"%f\\n\" | sort";
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);

    $find_result = `$cmd`;
    chomp $find_result;
    @beams = split(/\n/, $find_result);
    Dada::logMsg(3, DL, "getBeamToSend: found ".($#beams+1)." beams in obs ".$obs);

    $sent_beams = 0;

    # See if we can find a beam that matches
    for ($j=0; (($j<=$#beams) && ($optimal_obs eq "none") && (!$have_obs)); $j++) {

      $beam = $beams[$j];
      Dada::logMsg(3, DL, "getBeamToSend: checking ".$obs."/".$beam);
      
      # If the remote NFS mount exists
      if (-f $obs."/".$beam."/obs.start") {

        # see what flags we have
        $twobit_file = (-f $obs."/".$beam."/".$obs.".fil") ? 1 : 0;
        $swin_file = (-f $obs."/".$beam."/sent.to.swin") ? 1 : 0;
        $parkes_file = (-f $obs."/".$beam."/sent.to.parkes") ? 1 : 0;
        $psrxml_file = (-f $obs."/".$beam."/".$obs.".psrxml") ? 1 : 0;
        $archive_file = (-f $obs."/".$beam."/integrated.ar") ? 1 : 0;

        if (!(($want_swin && !$swin_file) || ($want_parkes && !$parkes_file))) {
          $sent_beams += 1;  
        }

        Dada::logMsg(3, DL, "getBeamToSend: ".$obs."/".$beam." fil[".$twobit_file."] swin[".$want_swin.",".$swin_file."] parkes[".$want_parkes.",".$parkes_file."]");

        if ($twobit_file) {

          # If both need to be transferred, done!          
          if (($want_swin) && ($want_parkes) && ($swin ne "none") && (!$swin_file) && ($parkes ne "none") && (!$parkes_file)) {
            $optimal_obs = $obs;
            $optimal_beam = $beam;
            $have_obs = 1;
            Dada::logMsg(2, DL, "getBeamToSend: found optimal beam ".$obs."/".$beam);
          }

          # If only one, then done acceptable
          if ( ($acceptable_obs eq "none") && ((($swin ne "none")   && (!$swin_file)   && ($want_swin)) || 
                                               (($parkes ne "none") && (!$parkes_file) && ($want_parkes)))) {
            $acceptable_obs = $obs;
            $acceptable_beam = $beam;
            $have_obs = 1;
            Dada::logMsg(2, DL, "getBeamToSend: found acceptable beam ".$obs."/".$beam);
          }

        }

        # If only one, then sub optimal
        if ( ($suboptimal_obs eq "none") && ((($swin ne "none") && (!$swin_file) && ($want_swin)) ||
                                             (($parkes ne "none") && (!$parkes_file) && ($want_parkes)))) {
          $suboptimal_obs = $obs;
          $suboptimal_beam = $beam;
          $have_obs = 1;
          Dada::logMsg(2, DL, "getBeamToSend: found sub optimal beam ".$obs."/".$beam);
        }

      } else {
        # Dada::logMsgWarn($warn, $obs."/".$beam."/obs.start did not exist, or dir was not mounted");
        Dada::logMsg(3, DL, $obs."/".$beam."/obs.start did not exist, or dir was not mounted");
      }
    
    }
    if ($sent_beams == ($#beams+1)) {
      Dada::logMsg(2, DL, "getBeamToSend: ".$obs.": ALL BEAMS SENT");

      # Check if all beams have been trasnferred successfully, if so, mark 
      # the observation as sent.to.dest
      Dada::logMsg(2, DL, "getBeamToSend: checkAllBeams(".$obs.")");
      ($result, $response) = checkAllBeams($obs);
      Dada::logMsg(2, DL, "getBeamToSend: checkAllBeams: ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "main: checkAllBeams failed: ".$response);

      } else {
        if ($response ne "all beams sent") {
          Dada::logMsg(1, DL, "Obs ".$obs." not fully transferred: ".$response);

        } else {
          Dada::logMsg(0, DL, "Successfully transferred to swin & parkes: ".$obs);
          recordTransferResult("obs.transferred", $obs);
          unlink ($obs."/obs.finished");
        }
      }
    }

  }

  Dada::logMsg(2, DL, "have_obs: ".$have_obs);

  if ($have_obs) {
    if ($optimal_obs ne "none") {
      $obs_to_send = $optimal_obs;
      $beam_to_send = $optimal_beam;
    } elsif ($acceptable_obs ne "none") {
      $obs_to_send = $acceptable_obs;
      $beam_to_send = $acceptable_beam;
    } elsif ($suboptimal_obs ne "none") {
      $obs_to_send = $suboptimal_obs;
      $beam_to_send = $suboptimal_beam;
    } else {
      Dada::logMsgWarn($error, "getBeamToSend: something went wrong in logic");
    }

    Dada::logMsg(2, DL, "getBeamToSend: to_send obs=".$obs_to_send.", beam=".$beam_to_send);

    # get the PID
    $cmd = "grep ^PID ".$obs_to_send."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);
    $obs_pid = `$cmd`;
    chomp $obs_pid;
    Dada::logMsg(2, DL, "getBeamToSend: PID = ".$obs_pid);
    
    $cmd = "grep SOURCE ".$obs_to_send."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);
    $source = `$cmd`;
    chomp $source;
    Dada::logMsg(2, DL, "getBeamToSend: SOURCE = ".$source);
    
    if ($source =~ m/^G/) {
      $survey_obs = 1;
      $is_fold = 0;
    } else {
      $survey_obs = 0;
      $is_fold = 1;
    }
    
    # determine the required destinations based on PID
    Dada::logMsg(3, DL, "getBeamToSend: getObsDestinations(".$obs_pid.", ".$cfg{$obs_pid."_DEST"}.")");
    ($want_swin, $want_parkes) = Bpsr::getObsDestinations($obs_pid, $cfg{$obs_pid."_DEST"});
    Dada::logMsg(2, DL, "getBeamToSend: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);

    # Flags for chosen archive
    $twobit_file = 0;
    $archive_file = 0;
    $swin_file = 0;
    $parkes_file = 0;
    $go_swin = 0;
    $go_parkes = 0;

    Dada::logMsg(3, DL, "getBeamToSend: testing ".$obs_to_send."/".$beam_to_send);
    $cmd = "ls ".$obs_to_send."/".$beam_to_send;
    Dada::logMsg(3, DL, "getBeamToSend: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "getBeamToSend: ".$result.":".$response);

    if ( -f $obs_to_send."/".$beam_to_send."/".$obs_to_send.".fil" ) { 
      $twobit_file = 1; 
    }
    if ( -f $obs_to_send."/".$beam_to_send."/integrated.ar" ) { 
      $archive_file = 1;
    }
    if ( -f $obs_to_send."/".$beam_to_send."/sent.to.swin" ) { 
      $swin_file = 1; 
    }
    if ( -f $obs_to_send."/".$beam_to_send."/sent.to.parkes") { 
      $parkes_file = 1; 
    }
    Dada::logMsg(2, DL, "getBeamToSend: [".$twobit_file.", ".$swin_file.", ".$parkes_file.", ".$archive_file."]");

    if (($want_swin) && (!$swin_file)) {
      $go_swin = 1;
    }
    if (($want_parkes) && (!$parkes_file)) {
      $go_parkes = 1;
    }
    Dada::logMsg(2, DL, "getBeamToSend: returning (".$obs_to_send.", ".$beam_to_send.", ".$go_swin.
                        ", ".$go_parkes.", ".$is_fold.")"); 
  
    return ($obs_to_send, $beam_to_send, $go_swin, $go_parkes, $is_fold);
  } else {
    return ("none", "none", 0, 0, 0);
  }
    
}


#
# Create the receiving directory on the remote machine
# and then run vsib_recv
#
sub run_vsib_recv($$$$$) {

  my ($user, $host, $dir, $obs_dir, $port) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";

  # create the writing file 
  #   [disk]/bpsr/[staging_area|pulsars]/<PID>/../../../WRITING
  #   [disk]/WRITING
  $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; touch ../../../WRITING\"";
  Dada::logMsg(2, DL, "run_vsib_recv [".$host."]: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "run_vsib_recv [".$host."]: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "run_vsib_recv [".$host."]: could not touch remote WRITING file");
  }

  # sleeping to avoid immediately consecutive ssh commands
  sleep(1);

  # create the output directory
  $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"mkdir -p ".$dir."/".$obs_dir."\"";
  Dada::logMsg(2, DL, $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "run_vsib_recv [".$host."]: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($error, "run_vsib_recv[".$host."]: could not create output dir \"".
                            $dir."/".$obs_dir."\": ".$response);
    return ("fail");
  }

  # sleeping to avoid immedialte consecutive ssh commands
  sleep(1);

  # run vsib_recv
  $cmd = "vsib_recv -q -1 -w ".TCP_WINDOW." -p ".$port." >> transfer.log";
  my $ssh_cmd =  "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; ".$cmd."\" 2>&1 | server_bpsr_server_logger.pl -n vsib_recv_".$host;

  Dada::logMsg(2, DL, "run_vsib_recv [".$host."]: ".$ssh_cmd);
  system($ssh_cmd);

  if ($? != 0) {
    Dada::logMsgWarn($warn, "vsib_recv returned a non zero exit value");
    return ("fail");
  } else {
    Dada::logMsg(2, DL, "vsib_recv succeeded");
    return ("ok");
  }

}

#
# Runs vsib_proxy on the localhost, it will connect to s_host upon receiving 
# an incoming connection
#
sub run_vsib_proxy($$) {

  my ($host, $port) = @_;

  Dada::logMsg(2, DL, "run_vsib_proxy()");

  my $vsib_args = "-1 -q -w ".TCP_WINDOW." -p ".$port." -H ".$host;
  my $cmd = "vsib_proxy ".$vsib_args." 2>&1 | server_bpsr_server_logger.pl -n vsib_proxy";
  Dada::logMsg(2, DL, $cmd);

  # Run the command on the localhost
  system($cmd);

  # Check return value for "success"
  if ($? != 0) {

    Dada::logMsgWarn($error, "run_vsib_proxy: command failed");
    return ("fail");

  # On sucesss, return ok
  } else {

    Dada::logMsg(2, DL, "run_vsib_proxy: succeeded");
    return ("ok");

  }

}


sub interrupt_all(\@\@) {

  my ($s_ref, $p_ref) = @_;

  Dada::logMsg(2, DL, "interrupt_all()");

  my @s = @$s_ref;
  my @p = @$p_ref;

  my $cmd = "";
  my $i=0;

  # kill all potential senders
  for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
    Dada::logMsg(2, DL, "interrupt_all: interrupt_vsib_send(bpsr, ".$cfg{"PWC_".$i}.")");
    interrupt_vsib_send("bpsr", $cfg{"PWC_".$i});
  }

  interrupt_vsib_proxy();

  my $disk = "";
  my $user = "";
  my $host = "";
  my $path = "";
  my @disk_components = ();

  # for each swinburne disk
  for ($i=0; $i<=$#s; $i++) {

    $disk = $s[$i];
    $user = "";
    $host = "";
    $path = "";
    @disk_components = split(":",$disk,3);

    Dada::logMsg(2, DL, "interrupt_all: swin ".$disk);

    if ($#disk_components == 2) {
      $user = $disk_components[0];
      $host = $disk_components[1];
      $path = $disk_components[2]."/".$pid."/staging_area";
      interrupt_vsib_recv($user, $host, $path, "none", "none");
    }
  }

  # for each parkes disk
  for ($i=0; $i<=$#p; $i++) {

    $disk = $p[$i];
    $user = "";
    $host = "";
    $path = "";
    @disk_components = split(":",$disk,3);

    Dada::logMsg(2, DL, "interrupt_all: parkes ".$disk);

    if ($#disk_components == 2) {
      $user = $disk_components[0];
      $host = $disk_components[1];
      $path = $disk_components[2]."/".$pid."/staging_area";
      interrupt_vsib_recv($user, $host, $path, "none", "none");
    }
  }
}

#
# Terminate the vsib_proxy running on localhost
#
sub interrupt_vsib_proxy() {

  Dada::logMsg(2, DL, "interrupt_vsib_proxy()");
  
  my $cmd = "killall vsib_proxy";
  Dada::logMsg(1, DL, "interrupt_vsib_proxy: ".$cmd);
  my $result = `$cmd 2>&1`;
}

#
# ssh to host specified and kill all vsib_send's
#
sub interrupt_vsib_send($$) {

  my ($user, $host) = @_;

  Dada::logMsg(2, DL, "interrupt_vsib_send(".$user.", ".$host.")");
 
  my $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"killall vsib_send\"";
  Dada::logMsg(1, DL, "interrupt_vsib_send: ".$cmd);
  my $result = `$cmd 2>&1`;

}

#
# ssh to host specified and kill all vsib_recv's, cleanup the obs
#
sub interrupt_vsib_recv($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  my $cmd = "";
  Dada::logMsg(2, DL, "interrupt_vsib_recv(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  if ($host ne "none") {

    # kill vsib_recv
    $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"killall vsib_recv\"";
    Dada::logMsg(1, DL, "interrupt_vsib_recv: ".$cmd);
    my $result = `$cmd 2>&1`;

    # delete the WRITING flag
    $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; rm -f ../../../WRITING\"";
    Dada::logMsg(1, DL, "interrupt_vsib_recv: ".$cmd);
    system($cmd);

    # optionally delete the transfer specified, this is a beam
    if (($obs ne "none") && ($obs ne "")) {
      $cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"cd ".$dir."; rm -rf ".$obs."/".$beam."\"";
      Dada::logMsg(1, DL, "interrupt_vsib_recv: ".$cmd);
      system($cmd);
    }
  }
}

#
# ssh send_host and run vsib_send
#
sub run_vsib_send($$$$$) {

  my ($send_host, $sw_host, $pa_host, $port, $files) = @_;

  if (($pa_host eq "none") && ($sw_host eq "none")) {
    return ("fail");
  }

  # Interim hack for apsr buffering area
  if ($pa_host =~ m/apsr1\d/) {
    $pa_host .= "-udp";
  }

  my $vsi_hosts = "'".$pa_host." ".$sw_host."'";

  if ($pa_host eq "none") {
    $vsi_hosts = $sw_host;
  } elsif ($sw_host eq "none") {
    $vsi_hosts = $pa_host;
  } else {
    # normal
  }

  my $cmd = "vsib_send -q -w ".TCP_WINDOW." -p ".$port." -H ".$vsi_hosts." -z ".DATA_RATE." ".$files;

  my $ssh_cmd = "ssh ".SSH_OPTS." -l bpsr ".$send_host." \"cd ".$cfg{"CLIENT_ARCHIVE_DIR"}."; ".$cmd."\" 2>&1 | server_bpsr_server_logger.pl -n vsib_send";

  Dada::logMsg(2, DL, "run_vsib_send: ".$ssh_cmd);
  system($ssh_cmd);

  if ($? != 0) {
    Dada::logMsgWarn($warn, "run_vsib_send: vsib_send returned a non-zero exit value");
    return("fail");
  } else {
    return("ok");
  }

}

#
# Returns the files to be sent for this beam
#
sub getBeamFiles($$) {

  my ($obs, $beam) = @_;

  Dada::logMsg(2, DL, "getBeamFiles(".$obs.", ".$beam.")");

 # my $archives_dir = $cfg{"SERVER_ARCHIVE_NFS_MNT"};
  my $dir = $obs."/".$beam;
  my @files = ();
  my $file_list = "";
  my $cmd = "";
  my $list = "";

  # aux.tar, integrated.ar, $obs.psrxml, $obs.fil, obs.txt and obs.start
  #$cmd = "ls -1 ".$dir."/*ar ".$dir."/".$obs.".* ".$dir."/obs.*";
  $cmd = "find -L ".$dir." -maxdepth 1 -name '*ar' -o -name '".$obs.".*' -o -name  'obs.*'";
  Dada::logMsg(2, DL, "getBeamFiles: ".$cmd);
  $list = `$cmd`;
  chomp $list;
  @files = split(/\n/,$list);

  my $i=0;
  for ($i=0; $i<=$#files; $i++) {
    $file_list .= $files[$i]." ";
  }
  
  Dada::logMsg(2, DL, "getBeamFiles: ".$file_list);

  return $file_list;

}



#
# Polls for the "quitdaemons" file in the control dir
#
sub daemonControlThread() {

  Dada::logMsg(2, DL, "daemon_control: thread starting");

  my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
  my $daemon_quit_file = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;

  if (-f $daemon_quit_file) {
    print STDERR "daemon_control: quit file existed on startup, exiting\n";
    exit(1);
  }

  # poll for the existence of the control file
  while ((!-f $daemon_quit_file) && (!$quit_daemon)) {
    Dada::logMsg(3, DL, "daemon_control: Polling for ".$daemon_quit_file);
    sleep(1);
  }

  # signal threads to exit
  $quit_daemon = 1;

  # try to cleanly stop things
  if (!(($curr_obs eq "none") || ($curr_obs eq ""))) {
    Dada::logMsg(1, DL, "daemon_control: interrupting obs ".$curr_obs);
    interrupt_vsib_send($send_user, $send_host);
    interrupt_vsib_proxy();
    interrupt_vsib_recv($p_user, $p_host, $p_dir, $curr_obs, $curr_beam);
    interrupt_vsib_recv($s_user, $s_host, $s_dir, $curr_obs, $curr_beam);

    sleep(2);

    my $tags_to_clean = "";
    if ($s_host ne "none") {
      $tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/".$curr_beam."/sent.to.swin ";
      $tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/".$curr_beam."/error.to.swin ";
      #$tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/sent.to.swin ";
      #$tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/error.to.swin ";
    }
    if ($p_host ne "none") {
      $tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/".$curr_beam."/sent.to.parkes ";
      $tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/".$curr_beam."/error.to.parkes ";
      #$tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/sent.to.parkes ";
      #$tags_to_clean .= $cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$curr_obs."/error.to.parkes ";
    } 

    # Clean up the sent.to.[swin|parkes] in the current archives dir
    my $cmd = "sudo -u bpsr rm -f ".$tags_to_clean;
    Dada::logMsg(1, DL, "daemon_control: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(1, DL, "daemon_control: ".$result." ".$response);

  } else {
    Dada::logMsg(1, DL, "daemon_control: not interrupting due to no current observation");
  }

  Dada::logMsg(2, DL, "daemon_control: Unlinking PID file ".$pidfile);
  unlink($pidfile);

  Dada::logMsg(2, DL, "daemon_control: exiting");

}

#
# Reports the PID the script was launched with on a socket
#
sub pidReportThread($) {

  (my $daemon_pid) = @_;

  Dada::logMsg(1, DL, "pidReportThread: thread starting");

  my $sock = 0;
  my $host = $cfg{"SERVER_HOST"};
  my $port = $cfg{"SERVER_XFER_PID_PORT"};
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
          Dada::logMsg(2, DL, "pidReportThread: Accepting connection");

          # Add this read handle to the set
          $read_set->add($handle);
          $handle = 0;

        } else {

          $string = Dada::getLine($rh);

          if (! defined $string) {
            Dada::logMsg(2, DL, "pidReportThread: Closing a connection");
            $read_set->remove($rh);
            close($rh);
  
          } else {

            Dada::logMsg(2, DL, "pidReportThread: <- ".$string);

            if ($string eq "get_pid") {
              print $rh $daemon_pid."\r\n";
              Dada::logMsg(2, DL, "pidReportThread: -> ".$daemon_pid);
            } else {
              Dada::logMsgWarn($warn, "pidReportThread: received unexpected string: ".$string);
            }
          }
        }
      }
    }
  }

  Dada::logMsg(1, DL, "pidReportThread: thread exiting");

  return 0;
}
#
# Handle INT AND TERM signals
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(3);
  print STDERR basename($0)." : Exiting: ".Dada::getCurrentDadaTime(0)."\n";

}

sub killAndCleanExisting($$$) {

  my ($user, $host, $dir) = @_;

  my $cmd =  "ssh ".SSH_OPTS." -l ".$user." ".$host." \"server_bpsr_transfer_killer.sh ".$dir."\"";

  Dada::logMsg(2, DL, $cmd);
  my ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, DL, "server_bpsr_transfer_killer.sh: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, DL, "server_bpsr_transfer_killer returned a non zero exit value");
  }

  return ($result, $response);

}

sub killLocal(){
  my $cmd = "server_bpsr_transfer_kill_server.sh";
  Dada::logMsg(2, DL, $cmd);
  my ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, DL, "server_bpsr_transfer_kill_server.sh: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "server_bpsr_transfer_kill_server returned a non zero exit value");
  }

  return ($result, $response);
}

sub killDisks(\@) {

  (my $disks_ref) = @_;

  my @disks = @$disks_ref;
  my @disk_components = ();

  my $i=0;
  my $disk = "";
  my $user = "";
  my $host = "";
  my $path = "";
  my $cmd = "";
  my $result = "";

  for ($i=0; $i<=$#disks; $i++) {

    my $disk = $disks[$i];
    Dada::logMsg(2, DL, "Evaluating ".$path);

    $user = "";
    $host = "";
    $path = "";

    @disk_components = split(":",$disk,3);

    if ($#disk_components == 2) {

      $user = $disk_components[0];
      $host = $disk_components[1];
      $path = $disk_components[2];
      ($result,$response) = killAndCleanExisting($user,$host,$path);

    }
  }
  return;
}


#
# This checks if the data is a real survey pointing or a pulsar
#
sub checkIfFold($){

  my $obs = shift;
  my $is_fold = 1;

  # Mike, really, what were you thinking???
  # my $cmd="sed -e 's:^.*<source_name_centre_beam> *\(.\)[^ ]* *</source_name_centre_beam>.*\$:\1:p' -e '/.*/d' $obs/01/*.psrxml";

  # this is muuuuuuuuuuuuch easier to understand 
  my $cmd = "grep source_name_centre_beam ".$obs."/01/*.psrxml| awk -F\\> '{print \$2}' | awk -F\\< '{printf(\"%.1s\",\$1)}'";
  Dada::logMsg(2, DL, "checkIfFold: ".$cmd);
  my ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "checkIfFold: ".$result." ".$response);
 
  if ($result eq "ok") {

    if ($response eq "G") {
      $is_fold = 0;
    } else {
      $is_fold = 1;
    }
  } else {
    $is_fold = 1;
  }

  Dada::logMsg(2, DL, "checkIfFold(): For $obs is_fold=".$is_fold);

  return $is_fold;
}

#
# Write the current status into the /nfs/control/bpsr/xfer.state file
#
sub setStatus($) {

  (my $message) = @_;

  Dada::logMsg(2, DL, "setStatus(".$message.")");

  my $dir = "/nfs/control/bpsr/";
  my $file = "xfer.state";
  my $cmd = "rm -f ".$dir."/".$file;
  my $result = "";
  my $response = "";


  Dada::logMsg(2, DL, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "setStatus: ".$result." ".$response);

  $cmd = "echo '".$message."' > ".$dir."/".$file;
  Dada::logMsg(2, DL, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "setStatus: ".$result." ".$response);

  return ("ok", "");
    
}

#
# Transfer the specified obs/beam on port to hosts
#
sub transferBeam($$$$$$$$$$) {

  my ($obs, $beam, $port, $files, $p_user, $p_host, $p_dir, $s_user, $s_host, $s_dir) = @_;

  my $result = "";
  my $response = "";
  my $cmd = "";

  # Thread ID's
  my $send_thread = 0;
  my $proxy_thread = 0;
  my $s_recv_thread = 0;
  my $p_recv_thread = 0;

  my $xfer_failure = 0;
  my $path = $obs."/".$beam;
  my $proxy_host = Dada::getHostMachineName();

  Dada::logMsg(1, DL, "Transferring ".$path." to (".$p_host." ".$s_host.")");

  # copy the obs.txt file into the beam directory (if it exists)
  if (-f $obs."/obs.txt") {
    my $cmd = "cp ".$obs."/obs.txt ".$path."/obs.txt";
    Dada::logMsg(2, DL, $cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, DL, $result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "could not add obs.txt to the transfer");
    } else {
      $files .= " ".$path."/obs.txt";
    }
  }

  # Set the current obs and beam global variables
  $curr_obs = $obs;
  $curr_beam = $beam;

  # Find the host on which the beam directory is located
  $cmd = "find ".$path." -maxdepth 1 -printf \"\%l\" | awk -F/ '{print \$3}'";
  Dada::logMsg(2, DL, $cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, $result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($error, "could not determine host on which ".$path." resides");
    return ("fail", "could not determine host on which ".$path." resides");
  }

  # host for vsib_send
  $send_host = $response;

  # start the receivers
  if ($p_host ne "none") {
    Dada::logMsg(2, DL, "run_vsib_recv(".$p_user.", ".$p_host.", ".$p_dir." ".$path.", ".$port.")");
    $p_recv_thread = threads->new(\&run_vsib_recv, $p_user, $p_host, $p_dir, $path, $port);
  }

  if ($s_host ne "none") {

    Dada::logMsg(2, DL, "run_vsib_recv(".$s_user.", ".$s_host.", ".$s_dir.", ".$path.", ".$port.")");
    $s_recv_thread = threads->new(\&run_vsib_recv, $s_user, $s_host, $s_dir, $path, $port);

    # The proxy is only run on srv0 for swin transfers
    $proxy_thread = threads->new(\&run_vsib_proxy, $s_host, $port);

  }

  # sleep a small amount to allow startup of the receivers
  sleep(4);

  # For swinburne transfers, connect the vsib_send to srv0's vsib_proxy
  my $s_proxy_host = "none";
  if ($s_host ne "none") {
    $s_proxy_host = "srv0"
  }

  # Wait for the recv threads to be "ready"
  my $receivers_ready = 0;
  my $recv_wait = 10;
  my $rval = 0;
  while ((!$receivers_ready) && ($recv_wait > 0) && (!$quit_daemon)) {

    $receivers_ready = 1;

    if ($p_recv_thread) {
      $cmd = "netstat -tln | grep ".$port;
      Dada::logMsg(3, DL, "main: ".$cmd);
      ($result, $rval, $response) = Dada::remoteSshCommand($p_user, $p_host, $cmd);
      Dada::logMsg(3, DL, "main: $result, $rval, $response");
      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "run_vsib_recv [".$p_host."]: ssh cmd failed: ".$response);
        $receivers_ready = 0;
      } else {
        if ($rval != 0) {
          $receivers_ready = 0;
        }
      }
    }

    if ($s_recv_thread) {
      $cmd = "netstat -tln | grep ".$port;
      Dada::logMsg(3, DL, "main: ".$cmd);
      ($result, $rval, $response) = Dada::remoteSshCommand($s_user, $s_host, $cmd);
      Dada::logMsg(3, DL, "main: $result, $rval, $response");
      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "run_vsib_recv [".$s_host."]: ssh cmd failed: ".$response);
        $receivers_ready = 0;
      } else {
        if ($rval != 0) {
          $receivers_ready = 0;
        }
      }
    }

    if (!$receivers_ready) {
      Dada::logMsg(1, DL, "Waiting 2 seconds for receivers [".$p_host.", ".$s_host."] to be ready");
      sleep(2);
      $recv_wait--;
    }
  }

  if ($quit_daemon) {
    Dada::logMsgWarn($warn, "daemon asked to quit whilst waiting for vsib_recv");
    $xfer_failure = 1;

  } elsif (!$receivers_ready) {
    Dada::logMsgWarn($error, "vsib_recv was not ready after 20 seconds of waiting");
    $xfer_failure = 1;

  } else {

  }

  if ($xfer_failure) {
    # Ensure the receiving thread is not hung waiting for an xfer
    if ($p_recv_thread) {
      interrupt_vsib_recv($p_user, $p_host, $p_dir, $curr_obs, $curr_beam);
    }
    if ($s_recv_thread) {
      interrupt_vsib_proxy();
      interrupt_vsib_recv($s_user, $s_host, $s_dir, $curr_obs, $curr_beam);
    }

  } else {

    sleep(5);

    # start the sender
    Dada::logMsg(2, DL, "run_vsib_send(".$send_host.", ".$s_proxy_host.", ".$p_host.", ".$port.")");
    $send_thread = threads->new(\&run_vsib_send, $send_host, $s_proxy_host, $p_host, $port, $files);

    # Transfer runs now
    Dada::logMsg(1, DL, "Transfer now running: ".$path);

    # join the sending thread
    Dada::logMsg(2, DL, "joining send_thread");
    $result = $send_thread->join();
    Dada::logMsg(2, DL, "send_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "send_thread failed for ".$path);
    }
  }

  # join the parkes thread, if it exists
  if ($p_recv_thread) {
    Dada::logMsg(2, DL, "joining p_recv_thread");
    $result = $p_recv_thread->join();
    Dada::logMsg(2, DL, "p_recv_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "p_recv_thread failed for ".$path);
    }
  }

  # join the swin thread, if it exists
  if ($s_recv_thread) {

    Dada::logMsg(2, DL, "joining proxy_thread");
    $result = $proxy_thread->join();
    Dada::logMsg(2, DL, "proxy_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "proxy_thread failed for ".$path);
    }

    Dada::logMsg(2, DL, "joining s_recv_thread");
    $result = $s_recv_thread->join();
    Dada::logMsg(2, DL, "s_recv_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "s_recv_thread failed for ".$path);
    }
  }
  if ($xfer_failure) {
    Dada::logMsgWarn($warn, "Transfer error for beam: ".$path);
  } else {
    Dada::logMsg(1, DL, "Transfer complete: ".$path);
  }

  # Increment the vsib port to prevent SO_REUSEADDR based issues
  $port++;
  if ($port == VSIB_MAX_PORT) {
    $port = VSIB_MIN_PORT;
  }

  return ($port, $xfer_failure);

}


#
# checks all the beam directories in obs to see if they have
# been sent
#
sub checkAllBeams($) {

  my ($obs) = @_;

  my $cmd = "";
  my $find_result = "";
  my $beam = "";
  my @beams = ();
  my $all_sent = 1;
  my $result = "";
  my $response = "";
  my $nbeams = 0;
  my $nbeams_mounted = 0;
  my $obs_pid = "";

  # Determine the number of NFS links in the archives dir
  $cmd = "find ".$obs." -mindepth 1 -maxdepth 1 -type l | wc -l";
  Dada::logMsg(2, DL, "checkAllBeams: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "checkAllBeams: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkAllBeams: find command failed: ".$response);
    return ("fail", "find command failed");
  }
  $nbeams = $response;
  Dada::logMsg(2, DL, "checkAllBeams: Total number of beams ".$nbeams);

  # Now find the number of mounted NFS links
  $cmd = "find -L ".$obs." -mindepth 1 -maxdepth 1 -type d -printf '\%f\\n' | sort";
  Dada::logMsg(2, DL, "checkAllBeams: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkAllBeams: find command failed: ".$response);
     return ("fail", "find command failed");
  }
  @beams = split(/\n/, $response);
  $nbeams_mounted = $#beams + 1;
  Dada::logMsg(2, DL, "checkAllBeams: Total number of mounted beams: ".$nbeams_mounted);

  # If a machine is not online, they cannot all be verified
  if ($nbeams != $nbeams_mounted) {
    return ("ok", "all beams not mounted");

  } else {
    $all_sent = 1;

    # skip if no obs.info exists
    if (!( -f $obs."/obs.info")) {
      Dada::logMsgWarn($warn, "checkAllBeams: Required file missing ".$obs."/obs.info");
      return ("fail", $obs."/obs.info did not exist");
    }

    # get the PID
    $cmd = "grep ^PID ".$obs."/obs.info | awk '{print \$2}'";
    Dada::logMsg(2, DL, "checkAllBeams: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, DL, "checkAllBeams: ".$result." ".$response);
    if ($result ne "ok") {
      return ("fail", "could not determine PID");
    }
    $obs_pid = $response;

    Dada::logMsg(3, DL, "checkAllBeams: getObsDestinations(".$obs_pid.", ".$cfg{$obs_pid."_DEST"}.")");
    my ($want_swin, $want_parkes) = Bpsr::getObsDestinations($obs_pid, $cfg{$obs_pid."_DEST"});
    Dada::logMsg(2, DL, "checkAllBeams: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);
 
    for ($i=0; (($i<=$#beams) && ($all_sent)); $i++) {
      $beam = $beams[$i];

      if ($want_swin && (! -f $obs."/".$beam."/sent.to.swin")) {
        $all_sent = 0;
        Dada::logMsg(2, DL, "checkAllBeams: ".$obs."/".$beam."/sent.to.swin did not exist");
      }

      if ($want_parkes && (! -f $obs."/".$beam."/sent.to.parkes")) {
        Dada::logMsg(2, DL, "checkAllBeams: ".$obs."/".$beam."/sent.to.parkes did not exist");
        $all_sent = 0;
      }
    }

    if ($all_sent) {
      return ("ok", "all beams sent");
    } else {
      return ("ok", "all beams not sent");
    }
  }

}


#
# Looks for observations that have been marked obs.deleted and moves them
# to /nfs/old_archives/bpsr and /nfs/old_results/bpsr if they are deleted
# and > 1 month old
#
sub checkFullyDeleted() {

  my $cmd = "";
  my $result = "";
  my $response = "";

  Dada::logMsg(2, DL, "checkFullyDeleted()");

  # Find all observations marked as obs.deleted and > 14*24 hours since being modified 
  $cmd = "find ".$cfg{"SERVER_ARCHIVE_DIR"}." -mindepth 2 -maxdepth 2 -name 'obs.deleted' -mtime +14 -printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";
  Dada::logMsg(2, DL, "checkFullyDeleted: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, DL, "checkFullyDeleted: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkFullyDeleted: find command failed: ".$response);
    return ("fail", "find command failed: ".$response);
  }

  chomp $response;
  my @observations = split(/\n/,$response);
  my $s = "";
  my $n_beams = 0;
  my $n_deleted = 0;
  my $o = "";
  my @remote_hosts = ();
  my $host = "";
  my $user = "";
  my $rval = "";
  my $j = 0;
  my $i = 0;

  for ($i=0; (($i<=$#observations) && (!$quit_daemon)); $i++) {
    $o = $observations[$i];

    $result = "ok";
    $response = "";

    # get a list of the host machines that have an archive dir on them
    $cmd = "find ".$cfg{"SERVER_ARCHIVE_DIR"}."/".$o." -mindepth 1 -maxdepth 1 -type l -printf '\%l\n' | awk -F/ '{print \$3}' | sort";
    Dada::logMsg(2, DL, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyDeleted: ".$result." ".$response);
    if ($result ne "ok") {
      return ("fail", "failed to find remote hosts for ".$o);
    }
    @remote_hosts = split(/\n/, $response);

    # copy the archives dir, converting NFS links to remote nodes to directories
    $cmd = "cp -rL ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$o." /nfs/old_archives/bpsr/".$o;
    Dada::logMsg(2, DL, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyDeleted: ".$result." ".$response);
    if ($result ne "ok") {
      return ("fail", "failed to copy ".$o." to old_archives");
    }

    # delete the client archive dirs fir each remote host
    $cmd = "rm -rf ".$cfg{"CLIENT_ARCHIVE_DIR"}.$o;
    $user = "bpsr";

    for ($j=0; $j<=$#remote_hosts; $j++) {

      $host = $remote_hosts[$j];

      ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
      Dada::logMsg(2, DL, "checkFullyDeleted: [".$host."] ".$result." ".$response);
      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "checkFullyDeleted: ssh failed: ".$response);
        return ("fail", "ssh ".$user."@".$host." failed: ".$response);
      } else {
        if ($rval != 0) {
          Dada::logMsgWarn($warn, "checkFullyDeleted: remote dir did not exist: ".$response);
          return ("fail", $cmd." failed: : ".$response);
        }
      }

    }

    $result = "ok";
    $response = "";

    # move the results dir
    $cmd = "mv ".$cfg{"SERVER_RESULTS_NFS_MNT"}."/".$o." /nfs/old_results/bpsr/".$o;
    Dada::logMsg(2, DL, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyDeleted: ".$result." ".$response);

    if ($result ne "ok") {
      return ("fail", "failed to move ".$o." to old_results");
    }

    Dada::logMsg(1, DL, $o.": deleted -> old");

  }
  return ("ok", "");
}

#
# Looks for fully archived observations, and marks obs.deleted if they
#
sub checkFullyArchived() {

  my $cmd = "";
  my $result = "";
  my $response = "";

  Dada::logMsg(2, DL, "checkFullyArchived()");

  # Find all observations marked as obs.transferred to 
  $cmd = "find ".$cfg{"SERVER_ARCHIVE_DIR"}." -mindepth 2 -maxdepth 2 -name 'obs.archived' -printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";
  Dada::logMsg(2, DL, "checkFullyArchived: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, DL, "checkFullyArchived: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkFullyArchived: find command failed: ".$response);
    return ("fail", "find command failed: ".$response);
  }

  chomp $response;
  my @observations = split(/\n/,$response);
  my $s = "";
  my $n_beams = 0;
  my $n_deleted = 0;
  my $o = "";

  for ($i=0; (($i<=$#observations) && (!$quit_daemon)); $i++) {
    $o = $observations[$i];

    if (-f $o."/obs.deleted") {
      Dada::logMsg(2, DL, "checkFullyArchived: removing old ".$o."/obs.archived");
      unlink $o."/obs.archived";
      next;
    }

    # find out how many beam directories we have
    $cmd = "find  ".$o." -mindepth 1 -maxdepth 1 -type l | wc -l";
    Dada::logMsg(3, DL, "checkFullyArchived: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyArchived: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyArchived: could not determine number of beam directories: ".$response);
      next;
    }
    chomp $response;
    $n_beams = $response;

    # find out how many beam.deleted files we have
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 2 -name 'beam.deleted' | wc -l";
    Dada::logMsg(3, DL, "checkFullyArchived: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyArchived: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyArchived: could not count beam.deleted files: ".$response);
      next;
    }
    chomp $response;
    $n_deleted = $response;

    if ($n_deleted == $n_beams) {
      $cmd = "touch ".$o."/obs.deleted";
      Dada::logMsg(1, DL, $o.": archived -> deleted");
      Dada::logMsg(2, DL, "checkFullyArchived: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      Dada::logMsg(3, DL, "checkFullyArchived: ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "checkFullyArchived: could not touch ".$o."/obs.deleted: ".$response);
      }

      if (-f $o."/obs.archived") {
        Dada::logMsg(2, DL, "checkFullyArchived: removing ".$o."/obs.archived");
        unlink $o."/obs.archived";
      }

    }
  }
  return ("ok", "");
}


#
# Looks for fully transferred observations to see if they have been archived yet
#
sub checkFullyTransferred() {

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $i = 0;
  my $n_beams = 0;
  my $n_swin = 0;
  my $n_parkes = 0;
  my $o = "";
  my $obs_pid = "";

  Dada::logMsg(2, DL, "checkFullyTransferred()");
 
  # Find all observations marked as obs.transferred to 
  $cmd = "find ".$cfg{"SERVER_ARCHIVE_DIR"}."  -mindepth 2 -maxdepth 2 -name 'obs.transferred' -printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";
  Dada::logMsg(2, DL, "checkFullyTransferred: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, DL, "checkFullyTransferred: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkFullyTransferred: find command failed: ".$response);
    return ("fail", "find command failed: ".$response);
  }

  chomp $response;
  my @observations = split(/\n/,$response);

  for ($i=0; (($i<=$#observations) && (!$quit_daemon)); $i++) {

    $o = $observations[$i];
  
    # skip if no obs.info exists
    if (!( -f $o."/obs.info")) {
      Dada::logMsgWarn($warn, "checkFullyTransferred: Required file missing ".$o."/obs.info");
      next;
    }

    # get the PID 
    $cmd = "grep ^PID ".$o."/obs.info | awk '{print \$2}'";
    Dada::logMsg(2, DL, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, DL, "checkFullyTransferred: ".$result." ".$response);
    if ($result ne "ok") {
      return ("fail", "could not determine PID");
    }
    $obs_pid = $response;
  
    Dada::logMsg(3, DL, "checkFullyTransferred: getObsDestinations(".$obs_pid.", ".$cfg{$obs_pid."_DEST"}.")");
    my ($want_swin, $want_parkes) = Bpsr::getObsDestinations($obs_pid, $cfg{$obs_pid."_DEST"});
    Dada::logMsg(2, DL, "checkFullyTransferred: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);

    # find out how many beam directories we have
    $cmd = "find ".$o." -mindepth 1 -maxdepth 1 -type l | wc -l";
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyTransferred: could not determine number of beam directories: ".$response);
      next;
    }
    chomp $response;
    $n_beams = $response;

    $n_swin = 0;
    $n_parkes = 0;

    # num on.tape.swin
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 2 -name 'on.tape.swin' | wc -l";
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$result." ".$response);
    
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyTransferred: could not count on.tape.swin files: ".$response);
      next;
    }
    chomp $response;
    $n_swin = $response;

    # num on.tape.parkes
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 2 -name 'on.tape.parkes' | wc -l";
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, DL, "checkFullyTransferred: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyTransferred: could not count on.tape.parkes files: ".$response);
      next;
    }
    chomp $response;
    $n_parkes = $response;

    # now the conditions
    if ( ($want_swin && ($n_swin < $n_beams)) || 
         ($want_parkes && ($n_parkes < $n_beams)) ){

      Dada::logMsg(2, DL, "checkFullyTransferred: ".$o." [".$obs_pid."] swin[".$n_swin."/".$n_beams."] ".
                          "parkes[".$n_parkes."/".$n_beams."]");

    } else {

      if (-f $o."/obs.archived") {
        # do nothing

      } else {
        $cmd = "touch ".$o."/obs.archived";
        Dada::logMsg(1, DL, $o.": transferred -> archived");
        Dada::logMsg(2, DL, "checkFullyTransferred: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        Dada::logMsg(2, DL, "checkFullyTransferred: ".$result." ".$response);

      }

      if (-f $o."/obs.transferred") {
        $cmd = "rm -f ".$o."/obs.transferred";
        Dada::logMsg(2, DL, "checkFullyTransferred: ".$o." has been fully archived, deleting obs.transferred");
        Dada::logMsg(2, DL, "checkFullyTransferred: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        Dada::logMsg(2, DL, "checkFullyTransferred: ".$result." ".$response);
      }

    }
  }
 
  return ("ok", ""); 

}

sub checkDestinations(\@) {

  my ($ref) = @_;

  Dada::logMsg(2, DL, "checkDestinations()");

  my @dests = @$ref;
  my $i = 0;
  my $user = "";
  my $host = "";
  my $path = "";
  my $cmd = "";
  my $result = "";
  my $rval = 0;
  my $response = "";

  # for each destinatinos
  for ($i=0; $i<=$#dests; $i++) {

    ($user, $host, $path) = split(":",$dests[$i],3);
    Dada::logMsg(2, DL, "checkDestinations: ".$user."@".$host.":".$path);

    $cmd = "ls ".$path;
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkDestinations: ssh failed: ".$response);
      return ("fail", "ssh ".$user."@".$host." failed: ".$response);

    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "checkDestinations: remote dir did not exist: ".$response);
        return ("fail", "remote dir did not exist: ".$response);
      }
    }

    $cmd = "ls ".$path."/".$pid."/staging_area > /dev/null";
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkDestinations: ssh failed: ".$response);
      return ("fail", "ssh ".$user."@".$host." failed: ".$response);

    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "checkDestinations: remote dir did not exist: ".$response);
        $cmd = "mkdir -p ".$path."/".$pid."/staging_area";
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$cmd);
        ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$result." ".$response);
      }
    }

    $cmd = "ls ".$path."/".$pid."/on_tape > /dev/null";
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkDestinations: ssh failed: ".$response);
      return ("fail", "ssh ".$user."@".$host." failed: ".$response);
    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "checkDestinations: remote dir did not exist: ".$response);
        $cmd = "mkdir -p ".$path."/".$pid."/on_tape";
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$cmd);
        ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$result." ".$response);
      }
    }

    $cmd = "ls ".$path."/".$pid."/pulsars > /dev/null";
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
    Dada::logMsg(2, DL, "checkDestinations: [".$host."] ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkDestinations: ssh failed: ".$response);
      return ("fail", "ssh ".$user."@".$host." failed: ".$response);
    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "checkDestinations: remote dir did not exist: ".$response);
        $cmd = "mkdir -p ".$path."/".$pid."/pulsars";
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$cmd);
        ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd);
        Dada::logMsg(1, DL, "checkDestinations: [".$host."] ".$result." ".$response);
      }
    }

  }

  return ("ok", "");
}


