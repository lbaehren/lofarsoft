##############################################################################
#  
#     Copyright (C) 2009 by Andrew Jameson
#     Licensed under the Academic Free License version 2.1
# 
###############################################################################
# 
# Generalized transfer manager for APSR and CAPSR instruments
#
 
package Dada::server_transfer_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use threads;
use threads::shared;
use File::Basename;
use Time::Local;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $daemon_name $server_logger $pid $dest $dest_id $optdir $rate %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our %cfg;
our $server_logger;
our $pid;
our $dest;
our $dest_id;
our $optdir;
our $rate;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $curr_obs : shared;
our $curr_band : shared;
our $instrument;
our $warn;
our $error;

our $send_user : shared;
our $send_host : shared;
our $dest_user : shared;
our $dest_host : shared;
our $dest_dir  : shared;


#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
%cfg = ();
$server_logger = "";
$pid = "";
$dest = "";
$dest_id = 0;
$optdir = "";
$rate = 0;

#
# initialize other variables
#
$warn = ""; 
$error = ""; 
$quit_daemon = 0;
$curr_obs = "none";
$curr_band = "none";
$instrument = "";
$send_user = "none";
$send_host = "none";
$dest_user = "";
$dest_host = "";
$dest_dir = "";

#
# Constants
#
use constant TCP_WINDOW    => 1700;   # About right for Parkes-Swin transfers
use constant VSIB_MIN_PORT => 25301;
use constant VSIB_MAX_PORT => 25326;
use constant SSH_OPTS      => "-x -o BatchMode=yes";


###############################################################################
#
# package functions
# 

sub main() {

  $warn  = $cfg{"STATUS_DIR"}."/".$daemon_name.".warn";
  $error = $cfg{"STATUS_DIR"}."/".$daemon_name.".error";
  $instrument = $cfg{"INSTRUMENT"};

  my $pid_file    = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file   = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $log_file    = $cfg{"SERVER_LOG_DIR"}."/".$daemon_name.".log";

  my $result = "";
  my $response = "";
  my $control_thread = 0;
  my $obs = "";
  my $band = "";
  my $i = 0;
  my $vsib_port = 0;
  my $cmd = "";
  my $xfer_failure = 0;
  my $files = "";
  my $counter = 0;

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }
  # user/host/dir should now be set and tested

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;


  # become a daemon
  Dada::daemonize($log_file, $pid_file);
  
  Dada::logMsg(0, $dl ,"STARTING SCRIPT");

  Dada::logMsg(2, $dl, "Clearing status warn/error files");
  unlink($warn);
  unlink($error);

  # start the control thread
  Dada::logMsg(2, $dl, "main: controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  setStatus("Starting script");

  # We cycle through ports to prevent "Address already in use" problems
  $vsib_port = VSIB_MIN_PORT;

  Dada::logMsg(1, $dl, "Starting Transfer Manager for ".$pid);

  chdir $cfg{"SERVER_ARCHIVE_NFS_MNT"};

  # On startup do a check for fully transferred or fully archived obvserations
  Dada::logMsg(1, $dl, "Checking for fully transferred observations");
  ($result, $response) = checkFullyTransferred();
  Dada::logMsg(2, $dl, "main: checkFullyTransferred(): ".$result.":".$response);

  Dada::logMsg(1, $dl, "Checking for fully deleted observations");
  ($result, $response) = checkFullyDeleted();
  Dada::logMsg(2, $dl, "main: checkFullyDeleted(): ".$result.":".$response);

  while (!$quit_daemon) {

    # Fine an observation to send based on the CWD
    Dada::logMsg(2, $dl, "main: getBandToSend()");
    ($obs, $band) = getBandToSend($cfg{"SERVER_ARCHIVE_NFS_MNT"});
    Dada::logMsg(2, $dl, "main: getBandToSend(): ".$obs."/".$band);

    if ($obs eq "none") {
      Dada::logMsg(2, $dl, "main: no observations to send. sleep 60");

      # On startup do a check for fully transferred or fully archived obvserations
      Dada::logMsg(2, $dl, "Checking for fully transferred observations");
      ($result, $response) = checkFullyTransferred();
      Dada::logMsg(2, $dl, "main: checkFullyTransferred(): ".$result.":".$response);

      Dada::logMsg(2, $dl, "Checking for fully deleted observations");
      ($result, $response) = checkFullyDeleted();
      Dada::logMsg(2, $dl, "main: checkFullyDeleted(): ".$result.":".$response)

    } else {

      # Find a place to write to
      ($result, $response) = checkDestination($dest_user, $dest_host, $dest_dir);

      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "destination not available: ".$response);
        setStatus("WARN: ".$response);
        $obs = "none";
        $band = "none";

      } else {

        Dada::logMsg(2, $dl, "Destination: ".$dest_user."@".$dest_host.":".$dest_dir);

        # If we have something to send
        if ($quit_daemon) {
          # Quit from the loop

        } elsif (($obs ne "none") && ($obs =~ /^2/)) {

          $xfer_failure = 0;
          $files = "";

          setStatus($obs."/".$band." &rarr; ".$dest_host);

          $curr_obs  = $obs;
          $curr_band = $band;

          ($vsib_port, $xfer_failure) = transferBand($obs, $band, $vsib_port);

          # If we have been asked to quit
          if ($quit_daemon) {

            Dada::logMsg(2, $dl, "main: asked to quit");

          } else {

            # Test to see if this band was sent correctly
            if (!$xfer_failure) {

              ($result, $response) = checkTransfer($dest_user, $dest_host, $dest_dir, $obs, $band);

              if ($result eq "ok") {

                if ($pid ne "P427") {

                  # change the remote directory structure to SOURCE / UTC_START
                  Dada::logMsg(2, $dl, "main: changeSourceDirStructure(".$dest_user.", ".$dest_host.", ".$dest_dir.", ".$obs.", ".$band.")");
                  ($result, $response) = changeSourceDirStructure($dest_user, $dest_host, $dest_dir, $obs, $band);
                  Dada::logMsg(2, $dl, "main: changeSourceDirStructure: ".$result." ".$response);

                } else {

                  # touch a remote xfer.complete for P427 
                  Dada::logMsg(2, $dl, "main: markRemoteFile(xfer.complete, ".$dest_user.", ".$dest_host.", ".$dest_dir.", ".$obs."/".$band.")");
                  ($result, $response) = markRemoteFile("xfer.complete", $dest_user, $dest_host, $dest_dir, $obs."/".$band);
                  Dada::logMsg(2, $dl, "main: markRemoteFile: ".$result." ".$response);

                }

                markState($obs, $band, "", "sent.to.".$dest);
                # Check if all bands have been trasnferred successfully, if so, mark 
                # the observation as sent.to.dest
                Dada::logMsg(2, $dl, "main: checkAllBands(".$obs.")");
                ($result, $response) = checkAllBands($obs);
                Dada::logMsg(2, $dl, "main: checkAllBands: ".$result." ".$response);

                if ($result ne "ok") {
                  Dada::logMsgWarn($warn, "main: checkAllBands failed: ".$response);

                } else {

                  if ($response ne "all bands sent") {
                    Dada::logMsg(2, $dl, "Obs ".$obs." not fully transferred: ".$response);

                  } else {
                    Dada::logMsg(0, $dl, "transferred to ".$dest.": ".$obs);
                    markState($obs, "", "obs.finished", "obs.transferred");
                    setStatus($obs." xfer success");

                  }
                }

              } else {
                $xfer_failure = 1;
                Dada::logMsgWarn($warn, "Transfer failed: ".$obs."/".$band." ".$response);

              }

            }

            # If we have a failed transfer, mark the /nfs/archives as failed
            if ($xfer_failure) {
              setStatus("WARN: ".$obs." xfer failed");
            } else {
              setStatus($obs." xfer success");
            }

            $curr_obs = "none";
            $curr_band = "none";
            
          }
        }
      }
    }

    # Dada::logMsg(1, $dl, "EARLY QUIT ENABLED FOR P427 TRANSFERS");
    # $quit_daemon = 1;

    # If we did not transfer, sleep 60
    if ($obs eq "none") {
  
      setStatus("Waiting for obs");
      Dada::logMsg(2, $dl, "Sleeping 60 seconds");
    
      $counter = 12;
      while ((!$quit_daemon) && ($counter > 0)) {
        sleep(5);
        $counter--;
      }
    }
  }

  setStatus("Script stopped");

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");

  # rejoin threads
  $control_thread->join();
                                                                                
  return 0;
}



###############################################################################
#
# Functions
#

sub markState($$$$) {

  my ($o, $b, $from, $to) = @_;

  Dada::logMsg(2, $dl, "markState(".$o.", ".$b.", ".$from.", ".$to.")");

  my $path = $o;
  my $cmd = "";
  my $result = "";
  my $response = "";

  if ($b ne "") {
    $path .= "/".$b;
  }

  if ($from ne "") {

    Dada::logMsg(1, $dl, $path.": ".$from." -> ".$to);
    $cmd = "rm -f ".$path."/".$from;
    Dada::logMsg(2, $dl, "markState: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "markState: ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "markState: ".$cmd." failed: ".$response);
    }

  } else {
    Dada::logMsg(1, $dl, $path." -> ".$to);

  }

  $cmd = "touch ".$path."/".$to;
  Dada::logMsg(2, $dl, "markState: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "markState: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "markState: ".$cmd." failed: ".$response);
  }

}


#
# Ensure the files sent matches the local
#
sub checkTransfer($$$$$) {

  my ($u, $h, $d, $obs, $band) = @_;

  my $cmd = "";
  my $result = "";
  my $rval = "";
  my $response = "";
  my $local_list = "local";
  my $remote_list = "remote";
  my $send_host = "";
  my $send_dir = $cfg{"CLIENT_ARCHIVE_DIR"};

  # Find the host on which the band directory is located
  $cmd = "find ".$obs."/".$band." -maxdepth 1 -printf \"\%l\" | awk -F/ '{print \$3}'";
  Dada::logMsg(2, $dl, "checkTransfer: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "checkTransfer: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($error, "could not determine host on which ".$obs."/".$band." resides");
    return ("fail", "could not determine host on which ".$obs."/".$band." resides");
  }

  # host for apsr_vsib_send
  $send_host = $response;

  # Get the listing of the files (assumes cwd is SERVER_ARCHIVES_NFS_MNT)
  $cmd = "find -L ".$obs."/".$band." -mindepth 1 -not -name 'obs.finished' -printf '\%h/\%f \%T@\\n' | sort | uniq";

  Dada::logMsg(2, $dl, "checkTransfer [".$send_host."]: (".$instrument.", ".$send_host.", ".$cmd.", ".$send_dir.")");
  ($result, $rval, $response) = Dada::remoteSshCommand($instrument, $send_host, $cmd, $send_dir);  
  Dada::logMsg(3, $dl, "checkTransfer [".$send_host."]: ".$result." ".$response);
  if ($result  ne "ok") {
    Dada::logMsgWarn($warn, "checkTransfer: local find failed ".$response);
  } else {
    $local_list = $response;
  }

  $cmd = "find ".$obs."/".$band." -mindepth 1 -printf '\%h/\%f \%T@\\n' | sort | uniq";
  # Get the remote listing of the files
  Dada::logMsg(2, $dl, "checkTransfer: [".$dest_host."] (".$u.", ".$h.", ".$cmd.", ".$d.")");
  ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd, $d);  
  Dada::logMsg(3, $dl, "checkTransfer: [".$dest_host."] ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkTransfer: ssh failed ".$response);
  } else {
    if ($rval != 0) {
      Dada::logMsgWarn($warn, "checkTransfer: getting remote list failed: ".$response); 
    } else {
      $remote_list = $response;
    }
  }

  $cmd = "rm -f ../WRITING";
  ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd, $d);
  if ($result  ne "ok") {
    Dada::logMsgWarn($warn, "checkRemoteArchive: ssh failure: ".$response);
  } else {
    if ($rval != 0) {
      Dada::logMsgWarn($warn, "checkRemoteArchive: could not remove ".$u."@".$h.":".$d."/../WRITING");
    }
  }

  if ($local_list eq $remote_list) {
    return ("ok","");

  } else {
    Dada::logMsgWarn($warn, "checkRemoteArchive: local files did not match remote ones after transfer");
    #Dada::logMsgWarn($warn, "local=".$local_list);
    #Dada::logMsgWarn($warn, "remote=".$remote_list);
    return ("fail", "archive mismatch");
  }
}

#
# Check the obs to see if all the bands have been sent to the dest
#
sub checkAllBands($) {

  my ($obs) = @_;

  my $cmd = "";
  my $find_result = "";
  my $band = "";
  my @bands = ();
  my $all_sent = 1;
  my $result = "";
  my $response = "";
  my $nbands = 0;
  my $nbands_mounted = 0;
  my $obs_pid = "";
  my @links = ();
  my $link = "";
  my $all_online = 0;
    
  # Determine the number of NFS links in the archives dir
  $cmd = "find ".$obs." -mindepth 1 -maxdepth 1 -type l | wc -l";
  Dada::logMsg(2, $dl, "checkAllBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "checkAllBands: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkAllBands: find command failed: ".$response);
    return ("fail", "find command failed");
  } 
  $nbands = $response;
  Dada::logMsg(2, $dl, "checkAllBands: Total number of bands ".$nbands);

  # Now find the number of mounted NFS links
  $cmd = "find -L ".$obs." -mindepth 1 -maxdepth 1 -type d -printf '\%f\\n' | sort";
  Dada::logMsg(2, $dl, "checkAllBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, $dl, "checkAllBands: ".$result." ".$response);
  if ($result ne "ok") { 
    Dada::logMsgWarn($warn, "checkAllBands: find command failed: ".$response);
     return ("fail", "find command failed");
  } 
  @bands = split(/\n/, $response);
  $nbands_mounted = $#bands + 1;
  Dada::logMsg(2, $dl, "checkAllBands: Total number of mounted bands: ".$nbands_mounted);
  
  # If a machine is not online, they cannot all be verified
  if ($nbands != $nbands_mounted) {

    # This may be because the directory has been deleted/lost or it may
    # be because the machine is offline

    $cmd = "find ".$obs." -mindepth 1 -maxdepth 1 -type l -printf '\%l\n' | awk -F/ '{ print \$1\"/\"\$2\"/\"\$3\"/\"\$4\"/\"\$5 }'";
    Dada::logMsg(2, $dl, "checkAllBands: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkAllBands: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkAllBands: find command failed: ".$response);
      return ("fail", "find command failed");
    }

    @links = split(/\n/, $response);
    $all_online = 1;
    foreach $link (@links) {
      $cmd = "ls -ld ".$link;
      Dada::logMsg(2, $dl, "checkAllBands: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      Dada::logMsg(3, $dl, "checkAllBands: ".$result." ".$response);
      
      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "checkAllBands: NFS dir ".$link." was offline");
        $all_online = 0;
      } 
    }

    if (!$all_online) {
      return ("ok", "all bands not mounted");
    } else {
      Dada::logMsgWarn($warn, "checkAllBands: ".($nbands - $nbands_mounted)." band(s) from ".$obs." were missing!");
    }
 
  }

  $all_sent = 1;
    
  # skip if no obs.info exists
  if (!( -f $obs."/obs.info")) {
    Dada::logMsgWarn($warn, "checkAllBands: Required file missing ".$obs."/obs.info");
    return ("fail", $obs."/obs.info did not exist");
  }

  # get the PID
  $cmd = "grep ^PID ".$obs."/obs.info | awk '{print \$2}'";
  Dada::logMsg(2, $dl, "checkAllBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "checkAllBands: ".$result." ".$response);
  if ($result ne "ok") {
    return ("fail", "could not determine PID");
  }
  $obs_pid = $response;

  my $i=0;
  for ($i=0; (($i<=$#bands) && ($all_sent)); $i++) {
    $band = $bands[$i];
    if (! -f $obs."/".$band."/sent.to.".$dest) {
      $all_sent = 0;
      Dada::logMsg(2, $dl, "checkAllBands: ".$obs."/".$band."/sent.to.".$dest." did not exist");
    }
  }

  if ($all_sent) {
    return ("ok", "all bands sent");
  } else { 
    return ("ok", "all bands not sent");
  }

}




# Adjust the directory structure to match the required storage format
sub changeSourceDirStructure($$$$$) {

  my ($u, $h, $d, $obs, $band) = @_;
  
  my $result = "";
  my $rval = 0;
  my $response = "";
  my $cmd = "";

  # determine if this is a multi fold observation
  $cmd = "server_apsr_archive_finalizer.csh ".$d." ".$obs." ".$band;
  Dada::logMsg(2, $dl, "changeSourceDirStructure: [".$h."] ".$cmd);
  ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd);
  Dada::logMsg(2, $dl, "changeSourceDirStructure: [".$h."] ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "changeSourceDirStructure: ssh failed ".$response);
    return ("fail", "");
  } else {
    if ($rval != 0) {
      Dada::logMsgWarn($warn, "changeSourceDirStructure: script failed: ".$response);
      return ("fail", "");
    }
  }

  return ("ok", "");  

}



#
# Find an observation to send, search chronologically. Look for observations that have 
# an obs.finished in them
#
sub getBandToSend() {

  Dada::logMsg(2, $dl, "getBandToSend()");

  my $obs_to_send = "none";
  my $band_to_send = "none";
  my $nfs_archives = $cfg{"SERVER_ARCHIVE_NFS_MNT"};

  my $cmd = "";
  my $result = "";
  my $response = "";
  my @obs_finished = ();
  my $i = 0;
  my $j = 0;
  my $obs_pid = "";
  my $proc_file = "";
  my $obs = "";
  my $band = "";
  my @bands = ();

  # Ensure the NFS directory is mounted
  $cmd = "ls -ld ".$nfs_archives;
  Dada::logMsg(2, $dl, "getBandToSend: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, $dl, "getBandToSend: ".$result.":".$response);

  # Look for all observations marked obs.finished in SERVER_NFS_RESULTS_DIR
  $cmd = "find ".$nfs_archives." -maxdepth 2 -name obs.finished ".
         "-printf \"%h\\n\" | sort | awk -F/ '{print \$NF}'";

  Dada::logMsg(2, $dl, "getBandToSend: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, $dl, "getBandToSend: ".$result.":".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "getBandToSend: ".$cmd." failed: ".$response);
    return ("none", "none");
  }

  # If there is nothing to transfer, simply return
  @obs_finished = split(/\n/, $response);
  if ($#obs_finished == -1) {
    return ("none", "none");
  }

  Dada::logMsg(2, $dl, "getBandToSend: found ".($#obs_finished+1)." observations marked obs.finished");

  # Go through the list of finished observations, looking for something to send
  for ($i=0; (($i<=$#obs_finished) && ($obs_to_send eq "none")); $i++) {

    $obs = $obs_finished[$i];
    Dada::logMsg(2, $dl, "getBandToSend: checking ".$obs);

    # skip if no obs.info exists
    if (!( -f $nfs_archives."/".$obs."/obs.info")) {
      Dada::logMsg(0, $dl, "Required file missing: ".$obs."/obs.info");
      next;
    }

    # Find the PID 
    $cmd = "grep ^PID ".$nfs_archives."/".$obs."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, $dl, "getBandToSend: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "getBandToSend: ".$result.":".$response);
    if (($result ne "ok") || ($response eq "")) {
      Dada::logMsgWarn($warn, "getBandToSend: failed to parse PID from obs.info [".$obs."] ".$response);
      next;
    }
    $obs_pid = $response;

    # If the PID doesn't match, skip it
    if ($obs_pid ne $pid) {
      Dada::logMsg(2, $dl, "getBandToSend: skipping ".$obs." PID mismatch [".$obs_pid." ne ".$pid."]");
      next;
    }

    # Check the PROC_FILE as we dont (currently) send single pulse observations
    $cmd = "grep ^PROC_FILE ".$nfs_archives."/".$obs."/obs.info | awk '{print \$2}'";
    Dada::logMsg(3, $dl, "getBandToSend: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "getBandToSend: ".$result.":".$response);
    if (($result ne "ok") || ($response eq "")) {
      Dada::logMsgWarn($warn, "getBandToSend: failed to parse PROC_FILE from obs.info [".$obs."] ".$response);
      next;
    }
    $proc_file = $response;

    # If the PID doesn't match, skip it
    if ($proc_file =~ m/single/) {
      Dada::logMsg(2, $dl, "getBandToSend: skipping ".$obs." PROC_FILE indicates single_pulse [".$proc_file."]");
      next;
    }

    # Get the sorted list of band nfs links for this obs
    # This command will timeout on missing NFS links (6 s), but wont print ones that are missing
    $cmd = "find -L ".$nfs_archives."/".$obs." -mindepth 1 -maxdepth 1 -type d -printf \"%f\\n\" | sort";
    Dada::logMsg(3, $dl, "getBandToSend: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "getBandToSend: ".$result.":".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "getBandToSend: failed to get list of NFS Band links ".$response);
      next;
    }

    @bands = split(/\n/, $response);
    Dada::logMsg(3, $dl, "getBandToSend: found ".($#bands+1)." bands in obs ".$obs);

    # See if we can find a band that matches
    for ($j=0; (($j<=$#bands) && ($obs_to_send eq "none")); $j++) {

      $band = $bands[$j];
      Dada::logMsg(3, $dl, "getBandToSend: checking ".$obs."/".$band);

      # If the remote NFS mount exists
      if (-f $nfs_archives."/".$obs."/".$band."/obs.start") {

        if (! -f $nfs_archives."/".$obs."/".$band."/sent.to.".$dest ) {
          Dada::logMsg(2, $dl, "getBandToSend: found ".$obs."/".$band);
          $obs_to_send = $obs;
          $band_to_send = $band;
        } else {
          Dada::logMsg(3, $dl, "getBandToSend: ".$obs."/".$band."/sent.to.".$dest." existed");
        }
      } else {
        Dada::logMsgWarn($warn, $obs."/".$band."/obs.start did not exist, or dir was not mounted");
      }
    }
  }

  Dada::logMsg(2, $dl, "getBandToSend: returning ".$obs_to_send.", ".$band_to_send);
  return ($obs_to_send, $band_to_send);
}

#
# Create the receiving directory on the remote machine
# and then run apsr_vsib_recv
#
sub run_apsr_vsib_recv($$$$$) {

  my ($u, $h, $d, $port, $dirs) = @_;

  my $cmd = "";
  my $ssh_cmd = "";
  my $pipe = "";
  my $result = "";
  my $rval = 0;
  my $response = "";

  # create the writing file
  $cmd = "touch ".$d."/../WRITING";
  Dada::logMsg(2, $dl, "run_apsr_vsib_recv [".$h."]: ".$cmd);
  ($result, $response) = Dada::remoteSshCommand($u, $h, $cmd);
  Dada::logMsg(2, $dl, "run_apsr_vsib_recv [".$h."]: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "run_apsr_vsib_recv [".$h."]: could not touch remote WRITING file");
  }

  # sleeping to avoid immediately consecutive ssh commands
  sleep(1);

  # if dirs have been specified, then we are not transferring with tar, but rather individual files
  if ($dirs) {

    $cmd = "ssh ".SSH_OPTS." -l ".$u." ".$h." \"cd ".$d."; mkdir -p ".$dirs."\"";
    Dada::logMsg(2, $dl, "run_apsr_vsib_recv [".$h."]: ".$cmd);
    system($cmd);

    # n.b. using -o means O_DIRECT
    $cmd = "apsr_vsib_recv -q -1 -w ".TCP_WINDOW." -o -p ".$port;

  } else {
    $cmd = "apsr_vsib_recv -q -1 -w ".TCP_WINDOW." -p ".$port." | tar -x";
  }


  # run apsr_vsib_recv 'manually' for the output piping to server_logger
  $ssh_cmd =  "ssh ".SSH_OPTS." -l ".$u." ".$h." \"cd ".$d."; ".$cmd."\" 2>&1 | ".$server_logger." -n apsr_vsib_recv";
  Dada::logMsg(2, $dl, "run_apsr_vsib_recv [".$h."]: ".$ssh_cmd);
  system($ssh_cmd);

  if ($? == 0) {
    $result = "ok";
  } elsif ($? == -1) {
    $result = "fail";
    Dada::logMsgWarn($error, "run_apsr_vsib_recv: failed to execute: $!");
  } elsif ($? & 127) {
    $result = "fail";
    Dada::logMsgWarn($error, "run_apsr_vsib_recv: child died with signal ".($? & 127));
  } else {
    $result = "fail";
    Dada::logMsg(1, $dl, "run_apsr_vsib_recv: child exited with value ".($? >> 8));
  }

  return $result;

  #Dada::logMsg(2, $dl, "run_apsr_vsib_recv remoteSshCommand(".$u.", ".$h.", ".$cmd.", ".$d.", ".$pipe.")");
  #($result, $response) = Dada::remoteSshCommand($u, $h, $cmd, $d, $pipe);
  #Dada::logMsg(2, $dl, "run_apsr_vsib_recv [".$h."]: ".$result." ".$response);
  #if ($result ne "ok") {
  #  Dada::logMsgWarn($error, "run_apsr_vsib_recv [".$h."]: ssh failure ".
  #                           $response);
  #  return ("fail");
  #}

  #if ($rval != 0) {
  #  Dada::logMsgWarn($warn, "apsr_vsib_recv returned a non zero exit value");
  #  return ("fail");
  #} else {
  #  Dada::logMsg(2, $dl, "apsr_vsib_recv succeeded");
  #  return ("ok");
  #}

}

#
# Runs apsr_vsib_proxy on the localhost, it will connect to s_host upon receiving 
# an incoming connection
#
sub run_apsr_vsib_proxy($$) {

  my ($host, $port) = @_;

  Dada::logMsg(2, $dl, "run_apsr_vsib_proxy()");

  my $vsib_args = "-1 -q -w ".TCP_WINDOW." -p ".$port." -H ".$host;
  my $cmd = "apsr_vsib_proxy ".$vsib_args." 2>&1 | ".$server_logger." -n apsr_vsib_proxy";
  Dada::logMsg(2, $dl, $cmd);

  # Run the command on the localhost
  system($cmd);

  # Check return value for "success"
  if ($? != 0) {

    Dada::logMsgWarn($error, "run_apsr_vsib_proxy: command failed");
    return ("fail");

  # On sucesss, return ok
  } else {

    Dada::logMsg(2, $dl, "run_apsr_vsib_proxy: succeeded");
    return ("ok");

  }

}


sub interrupt_all() {

  Dada::logMsg(2, $dl, "interrupt_all()");

  # kill all potential senders
  Dada::logMsg(2, $dl, "interrupt_all: interrupt_apsr_vsib_send()");
  interrupt_apsr_vsib_send($send_user, $send_host);

  Dada::logMsg(2, $dl, "interrupt_all: interrupt_apsr_vsib_proxy()");
  interrupt_apsr_vsib_proxy();

  Dada::logMsg(2, $dl, "interrupt_all: interrupt_apsr_vsib_recv()");
  interrupt_apsr_vsib_recv($dest_user, $dest_host, $dest_dir, "none", "none");

}

#
# ssh to host specified and kill all apsr_vsib_send's
#
sub interrupt_apsr_vsib_send($$) {

  my ($u, $h) = @_;
 
  Dada::logMsg(2, $dl, "interrupt_apsr_vsib_send(".$u.", ".$h.")");
  
  my $cmd = "";
  my $result = "";
  my $response = "";
  my $rval = 0;

  if (($send_user eq "none") || ($send_host eq "none")) {

    $result = "ok";
    $response = "apsr_vsib_send not running";

  } else {

    $cmd = "killall apsr_vsib_send";
    Dada::logMsg(2, $dl, "interrupt_apsr_vsib_send: ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd);
    Dada::logMsg(2, $dl, "interrupt_apsr_vsib_send: ".$result." ".$rval." ".$response);

    if ($result  ne "ok") {
      Dada::logMsgWarn($warn, "interrupt_apsr_vsib_send: ssh failure ".$response);
    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "interrupt_apsr_vsib_send: failed to kill apsr_vsib_send: ".$response);
      }
    }
  }

  return ($result, $response);
}


#
# ssh to host specified and kill all apsr_vsib_recv's, cleanup the obs
#
sub interrupt_apsr_vsib_recv($$$$$) {

  my ($u, $h, $d, $obs, $band) = @_;

  Dada::logMsg(2, $dl, "interrupt_apsr_vsib_recv(".$u.", ".$h.", ".$d.", ".$obs.", ".$band.")");

  my $cmd = "";
  my $result = "";
  my $rval = 0;
  my $response = "";

  if ($h ne "none") {

    # kill apsr_vsib_recv
    $cmd = "killall apsr_vsib_recv";
    Dada::logMsg(2, $dl, "interrupt_apsr_vsib_recv: ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd);
    if ($result  ne "ok") {
      Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: ssh failure ".$response);
    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: failed to kill apsr_vsib_recv: ".$response);
      }
    }

    # delete the WRITING flag
    $cmd = "rm -f ../WRITING";
    Dada::logMsg(2, $dl, "interrupt_apsr_vsib_recv: ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd, $d);
    if ($result  ne "ok") {
      Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: ssh failure ".$response);
    } else {
      if ($rval != 0) {
        Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: failed to remove WRITING flag: ".$response);
      } 
    }

    # optionally delete the transfer specied, this is a band
    if (!(($obs eq "none") || ($obs eq ""))) {
      $cmd = "rm -rf ".$obs."/".$band;
      Dada::logMsg(2, $dl, "interrupt_apsr_vsib_recv: ".$cmd);
      ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd, $d);
      if ($result  ne "ok") {
        Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: ssh failure ".$response);
      } else {
        if ($rval != 0) {
          Dada::logMsgWarn($warn, "interrupt_apsr_vsib_recv: failed to remove WRITING flag: ".$response);
        } 
      }
    }
  }
}

sub interrupt_apsr_vsib_proxy() {

  Dada::logMsg(2, $dl, "interrupt_apsr_vsib_proxy()");
 
  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "killall apsr_vsib_proxy";
  Dada::logMsg(1, $dl, "interrupt_apsr_vsib_proxy: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "interrupt_apsr_vsib_proxy: ".$result." ".$response);

  return ($result, $response);
}

#
# run apsr_vsib_send  with tar -c obs feeding stdin to port to the recv_host
#
sub run_apsr_vsib_send($$$$$$) {

  my ($obs, $band, $s_host, $r_host, $port, $files) = @_;

  Dada::logMsg(2, $dl, "run_apsr_vsib_send(".$obs.", ".$band.", ".$s_host.
                       ", ".$r_host.", ".$port.", ".$files.")");

  my $cmd = "";
  my $ssh_cmd = "";
  my $pipe = "";
  my $result = "";
  my $rval = 0;
  my $response = "";
  my $s_dir = $cfg{"CLIENT_ARCHIVE_DIR"};

  if ($files ne "") {

    $cmd .= "apsr_vsib_send -q -w ".TCP_WINDOW." -p ".$port." -H ".$r_host." -z ".$rate." ".$files;

  } else {

    $cmd .= "tar --exclude obs.finished -c ".$obs." | ";
    $cmd .= "apsr_vsib_send -q -w ".TCP_WINDOW." -p ".$port." -H ".$r_host." -z ".$rate;

  }

  $send_user = $instrument;
  $send_host = $s_host;

  $ssh_cmd =  "ssh ".SSH_OPTS." -l ".$instrument." ".$s_host." \"cd ".$s_dir."; ".$cmd."\" 2>&1 | ".$server_logger." -n apsr_vsib_send";
  Dada::logMsg(2, $dl, "run_apsr_vsib_send [".$s_host."]: ".$ssh_cmd);
  system($ssh_cmd);

  if ($? == 0) {
    $result = "ok";
  } elsif ($? == -1) {
    $result = "fail";
    Dada::logMsgWarn($error, "run_apsr_vsib_send: failed to execute: $!");
  } elsif ($? & 127) {
    $result = "fail";
    Dada::logMsgWarn($error, "run_apsr_vsib_send: child died with signal ".($? & 127));
  } else {
    $result = "fail";
    Dada::logMsg(1, $dl, "run_apsr_vsib_send: child exited with value ".($? >> 8));
  }

  #Dada::logMsg(2, $dl, "run_apsr_vsib_send: remoteSshCommand(".$instrument.", ".$s_host.", ".$cmd.", ".$s_dir.", ".$pipe.")");
  #($result, $rval, $response) = Dada::remoteSshCommand($instrument, $s_host, $cmd, $s_dir, $pipe);
  #Dada::logMsg(2, $dl, "run_apsr_vsib_send: remoteSshCommand: ".$result." ".$rval." ".$response);

  $send_user = "none";
  $send_host=  "none";

  return $result;

  #if ($result ne "ok") {
  #  Dada::logMsgWarn($warn, "run_apsr_vsib_send: apsr_vsib_send failed ".$response);
  #  return("fail");
  #} else {
  #  return("ok");
  #}
}


#
# Write the current status into the /nfs/control/apsr/xfer.state file
#
sub setStatus($) {

  (my $message) = @_;

  Dada::logMsg(2, $dl, "setStatus(".$message.")");

  my $result = "";
  my $response = "";
  my $cmd = "";
  my $dir = "/nfs/control/apsr";
  my $file = "xfer.state";

  $cmd = "rm -f ".$dir."/".$file;
  Dada::logMsg(2, $dl, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "setStatus: ".$result." ".$response);

  $cmd = "echo '".$message."' > ".$dir."/".$file;
  Dada::logMsg(2, $dl, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "setStatus: ".$result." ".$response);

  return ("ok", "");
    
}

#
# Transfer the specified obs/band on port to hosts
#
sub transferBand($$$) {

  my ($obs, $band, $port) = @_;

  my $localhost = Dada::getHostMachineName();
  my $path      = $obs."/".$band;
  my $result = "";
  my $response = "";
  my $rval = 0;
  my $cmd = "";
  my $dirs = "";
  my $files = "";

  # Thread ID's
  my $send_thread = 0;
  my $proxy_thread = 0;
  my $recv_thread = 0;

  my $send_host = "";
  my $xfer_failure = 0;

  my $u = $dest_user;
  my $h = $dest_host;
  my $d = $dest_dir;

  Dada::logMsg(1, $dl, "Transferring ".$path." to ".$u."@".$h.":".$d);

  # Find the host on which the band directory is located
  $cmd = "find ".$path." -maxdepth 1 -printf \"\%l\" | awk -F/ '{print \$3}'";
  Dada::logMsg(2, $dl, "transferBand: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "transferBand: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($error, "could not determine host on which ".$path." resides");
    return ("fail", "could not determine host on which ".$path." resides");
  }

  # host for apsr_vsib_send
  $send_host = $response;

  # If transferring via apsr_vsib_send, do direct files, and not via tar 
  if ($pid eq "P427_disabled") {

    $cmd = "find -L ".$path." -type d -printf '\%h/\%f '";
    Dada::logMsg(2, $dl, "transferBand: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "transferBand: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($error, "could not determine the dirs of ".$path);
      return ("fail", "could not determine the dirs of ".$path);
    } 
    $dirs = $response;
    Dada::logMsg(1, $dl, "transferBand: dirs=".$dirs);

    $cmd = "find -L ".$path." -type f -printf '\%h/\%f '";
    Dada::logMsg(2, $dl, "transferBand: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "transferBand: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($error, "could not determine the files of ".$path);
      return ("fail", "could not determine the files of ".$path);
    }
    $files = $response;
    Dada::logMsg(1, $dl, "transferBand: files=".$files);
  }


  Dada::logMsg(2, $dl, "run_apsr_vsib_recv(".$u.", ".$h.", ".$d.", ".$port.", ".$dirs.")");
  $recv_thread = threads->new(\&run_apsr_vsib_recv, $u, $h, $d, $port, $dirs);

  # If we are sending to swin, need to start a proxy on the localhost
  if ($dest eq "swin") {
    $proxy_thread = threads->new(\&run_apsr_vsib_proxy, $h, $port);
  }

  # sleep a small amount to allow startup of the receiver
  sleep(4);

  # Wait for the recv threads to be "ready"
  my $receiver_ready = 0;
  my $recv_wait = 10;
  while ((!$receiver_ready) && ($recv_wait > 0) && (!$quit_daemon)) {

    $receiver_ready = 1;

    # check that the socket is open/listening on the specified port
    $cmd = "netstat -tln | grep ".$port;
    Dada::logMsg(2, $dl, "main: ".$cmd);
    ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd);
    Dada::logMsg(2, $dl, "main: $result, $rval, $response");
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "ssh cmd failed: ".$response);
      $receiver_ready = 0;
    } else {
      if ($rval != 0) {
        $receiver_ready = 0;
      }
    } 

    if (!$receiver_ready) {
      Dada::logMsg(1, $dl, "Waiting 2 seconds for receiver [".$h."] to be ready");
      sleep(2);
      $recv_wait--;
    }
  }

  if ($quit_daemon) {
    Dada::logMsgWarn($warn, "daemon asked to quit whilst waiting for apsr_vsib_recv");
    $xfer_failure = 1;

  } elsif (!$receiver_ready) {
    Dada::logMsgWarn($error, "apsr_vsib_recv was not ready after 20 seconds of waiting");
    $xfer_failure = 1;

  } else {

  }

  if ($xfer_failure) {
    # Ensure the receiving thread is not hung waiting for an xfer
    interrupt_apsr_vsib_recv($u, $h, $d, $curr_obs, $curr_band);

  } else {

    sleep(2);

    # start the sender
    if ($proxy_thread) {
      Dada::logMsg(2, $dl, "run_apsr_vsib_send(".$obs.", ".$band.", ".$send_host.", ".$localhost.", ".$port.", ".$files.")");
      $send_thread = threads->new(\&run_apsr_vsib_send, $obs, $band, $send_host, $localhost, $port, $files);
    } else {
      Dada::logMsg(2, $dl, "run_apsr_vsib_send(".$obs.", ".$band.", ".$send_host.", ".$h.", ".$port.", ".$files.")");
      $send_thread = threads->new(\&run_apsr_vsib_send, $obs, $band, $send_host, $h, $port, $files);
    }

    # Transfer runs now
    Dada::logMsg(1, $dl, "Transfer now running: ".$path);

    # join the sending thread
    Dada::logMsg(2, $dl, "joining send_thread");
    $result = $send_thread->join();
    Dada::logMsg(2, $dl, "send_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "send_thread failed for ".$obs);
    }
  }

  # join the proxy thread, if it exists
  if ($proxy_thread) {
    $result = $proxy_thread->join();
    $proxy_thread = 0;
    Dada::logMsg(2, $dl, "proxy_thread: ".$result);
    if ($result ne "ok") {
      $xfer_failure = 1;
      Dada::logMsgWarn($warn, "proxy_thread failed for ".$path);
    }
  }

  # join the recv thread, if it exists
  Dada::logMsg(2, $dl, "joining recv_thread");
  $result = $recv_thread->join();
  $recv_thread = 0;
  Dada::logMsg(2, $dl, "recv_thread: ".$result);
  if ($result ne "ok") {
    $xfer_failure = 1;
    Dada::logMsgWarn($warn, "recv_thread failed for ".$path);
  }

  if ($xfer_failure) {
    Dada::logMsgWarn($warn, "Transfer error for obs: ".$path);
  } else {
    Dada::logMsg(1, $dl, "Transfer complete: ".$path);
  }

  # Increment the vsib port to prevent SO_REUSEADDR based issues
  $port++;
  if ($port == VSIB_MAX_PORT) {
    $port = VSIB_MIN_PORT;
  }

  return ($port, $xfer_failure);

} 


#
# check user@host:dir for connectivity and space
#
sub checkDestination($$$) {

  my ($u, $h, $d) = @_;

  my $result = "";
  my $rval = 0;
  my $response = "";
  my $cmd = "";

  $cmd = "df -B 1048576 -P ".$d." | tail -n 1 | awk '{print \$4}'";

  # test how much space is remaining on this disk
  ($result, $rval, $response) = Dada::remoteSshCommand($u, $h, $cmd);

  if ($result ne "ok") {
    return ("fail", "could not ssh to ".$u."@".$h.": ".$response);
  }

  # check there is 100 * 1GB free
  if (int($response) < (100*1024)) {
    return ("fail", "less than 100 GB remaining on ".$u."@".$h.":".$d);
  }

  # check if this is being used for reading
  $cmd = "ssh ".SSH_OPTS." -l ".$u." ".$h." \"ls ".$d."/../READING\" 2>&1";
  $result = `$cmd`;
  chomp $result;
  if ($result =~ m/No such file or directory/) {
    Dada::logMsg(2, $dl, "checkDestination: no READING file existed in ".$d."/../READING");
  } else {
    Dada::logMsg(1, $dl, "checkDestination: READING file existed in ".$d."/../READING");
    return ("fail", "Disk is being used for READING");
  }

  return ("ok", "");

}

sub markRemoteFile($$$$$) {

  my ($file, $user, $host, $dir, $subdir) = @_;
  Dada::logMsg(2, $dl, "markRemoteCompleted(".$file.", ".$user.", ".$host.", ".$dir.", ".$subdir.")");

  my $cmd = "cd ".$dir."; touch ".$subdir."/".$file;
  my $ssh_cmd = "ssh ".SSH_OPTS." -l ".$user." ".$host." \"".$cmd."\"";

  Dada::logMsg(2, $dl, "markRemoteCompleted: ".$cmd);
  my ($result, $response) = Dada::mySystem($ssh_cmd);
  Dada::logMsg(2, $dl, "markRemoteCompleted: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "could not mark ".$host.":".$dir."/".$subdir." as completed");
  }

  return ($result, $response);
}


#
# Looks for fully transferred observations to see if they have been deleted yet
#
sub checkFullyTransferred() {

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $i = 0;
  my $n_bands = 0;
  my $n_deleted = 0;
  my $n_swin = 0;
  my $n_parkes = 0;
  my $o = "";
  my $obs_pid = "";

  Dada::logMsg(2, $dl, "checkFullyTransferred()");

  # Find all observations marked as obs.transferred at least 14 days ago...
  $cmd = "find ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}." -maxdepth 2 -name 'obs.transferred' -printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";
  Dada::logMsg(2, $dl, "checkFullyTransferred: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, $dl, "checkFullyTransferred: ".$result." ".$response);

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
    Dada::logMsg(2, $dl, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "checkFullyTransferred: ".$result." ".$response);
    if ($result ne "ok") {
      return ("fail", "could not determine PID");
    }
    $obs_pid = $response;

    Dada::logMsg(3, $dl, "checkFullyTransferred: getObsDestinations(".$obs_pid.", ".$cfg{$obs_pid."_DEST"}.")");
    my ($want_swin, $want_parkes) = Dada::getObsDestinations($obs_pid, $cfg{$obs_pid."_DEST"});
    Dada::logMsg(2, $dl, "checkFullyTransferred: getObsDestinations want swin:".$want_swin." parkes:".$want_parkes);

    # find out how many band directories we have
    $cmd = "ls -1d ".$o."/*/ | wc -l";
    Dada::logMsg(3, $dl, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkFullyTransferred: ".$result." ".$response);
  
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyTransferred: could not determine number of band directories: ".$response);
      next;
    }
    chomp $response;
    $n_bands = $response;

    # find out how many band.deleted files we have
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 2 -name 'band.deleted' | wc -l";
    Dada::logMsg(3, $dl, "checkFullyTransferred: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkFullyTransferred: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyTransferred: could not count band.deleted files: ".$response);
      next;
    }
    chomp $response;
    $n_deleted = $response;

    Dada::logMsg(2, $dl, "checkFullyTransferred: n_bands=".$n_bands.", n_deleted=".$n_deleted);

    if ($n_deleted == $n_bands) {
      $cmd = "touch ".$o."/obs.deleted";
      Dada::logMsg(2, $dl, "checkFullyTransferred: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      Dada::logMsg(3, $dl, "checkFullyTransferred: ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "checkFullyTransferred: could not touch ".$o."/obs.deleted: ".$response);
      }

      if (-f $o."/obs.transferred") {
        Dada::logMsg(2, $dl, "checkFullyTransferred: removing ".$o."/obs.transferred");
        unlink $o."/obs.transferred";
      }
      Dada::logMsg(1, $dl, $o.": transferred -> deleted");
    }
  }
  
  return ("ok", "");
}

#
# Looks for observations that have been marked obs.deleted and moves them
# to /nfs/old_archives/apsr and /nfs/old_results/apsr if they are deleted
# and > 1 month old
#
sub checkFullyDeleted() {
  
  my $i = 0;
  my $cmd = "";
  my $result = "";
  my $response = "";
  
  Dada::logMsg(2, $dl, "checkFullyDeleted()");
  
  # Find all observations marked as obs.deleted and > 30*24 hours since being modified 
  $cmd = "find ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}."  -maxdepth 2 -name 'obs.deleted' -mtime +30 -printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";

  Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);
  
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkFullyDeleted: find command failed: ".$response);
    return ("fail", "find command failed: ".$response);
  }
  
  chomp $response; 
  my @observations = split(/\n/,$response);
  my @sources = ();
  my $n_band_tres = 0;
  my $req_band_tres = 0;
  my $n_bands = 0;
  my $o = "";
  my $curr_time = time;
  
  for ($i=0; (($i<=$#observations) && (!$quit_daemon)); $i++) {

    $o = $observations[$i];

    my @t = split(/-|:/,$o);
    my $unixtime = timelocal($t[5], $t[4], $t[3], $t[2], ($t[1]-1), $t[0]);

    Dada::logMsg(2, $dl, "checkFullyDeleted: testing ".$o." curr=".$curr_time.", unix=".$unixtime);
    # if UTC_START is less than 30 days old, dont delete it
    if ($unixtime + (30*24*60*60) > $curr_time) {
      Dada::logMsg(2, $dl, "checkFullyDeleted: Skipping ".$o.", less than 30 days old");
      next;
    }

    # find out how many band directories we have
    $cmd = "ls -1d ".$o."/*/ | wc -l";
    Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);
    
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyDeleted : could not determine number of band directories: ".$response);
      next;
    }
    chomp $response;
    $n_bands = $response;
    Dada::logMsg(2, $dl, "checkFullyDeleted: n_bands=".$n_bands);

    # find out how many sources in this observation (i.e. multifold)
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 2 -type d -printf '\%f\\n' | sort | uniq";
    Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyDeleted : could not determine number of sources: ".$response);
      next;
    }
    @sources = ();
    if ($response ne "") {
      @sources = split(/\n/, $response);
      $req_band_tres = ($#sources + 1) * $n_bands;
    } else {
      $req_band_tres = $n_bands;
    }
    Dada::logMsg(2, $dl, "checkFullyDeleted: expected number of band.tres files =".$req_band_tres);

    # find out how many band.tres files we have
    $cmd = "find -L ".$o." -mindepth 2 -maxdepth 3 -name 'band.tres' | wc -l";
    Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyDeleted: could not count band.tres files: ".$response);
      next;
    }
    chomp $response;
    $n_band_tres = $response;
    Dada::logMsg(2, $dl, "checkFullyDeleted: n_band_tres=".$n_band_tres);

    # If we have all the required band.tres files, regenerated the band summed archives
    if ($n_band_tres != $req_band_tres) {
      Dada::logMsgWarn($warn, "checkFullyDeleted: num of band.tres mismatch for ".$o.": n_band_tres [".$n_band_tres."]  != req_band_tres [".$req_band_tres."]");
    } 

    $cmd = "/home/dada/psrdada/apsr/scripts/server_apsr_band_processor.pl ".$o;
    Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "checkFullyDeleted: ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "checkFullyDeleted: server_apsr_band_processor.pl ".$o." failed: ".$response);
      return ("fail", "Failed to process ".$o);
    } 

    #$cmd = "mv ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$o." /nfs/old_archives/apsr/".$o;
    #Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    #($result, $response) = Dada::mySystem($cmd);
    #Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);

    #if ($result ne "ok") {
    #  return ("fail", "failed to move ".$o." to old_archives");
    #}

    #$result = "ok";
    #$response = "";
    #$cmd = "mv ".$cfg{"SERVER_RESULTS_NFS_MNT"}."/".$o." /nfs/old_results/apsr/".$o;
    #Dada::logMsg(2, $dl, "checkFullyDeleted: ".$cmd);
    #($result, $response) = Dada::mySystem($cmd);
    #Dada::logMsg(3, $dl, "checkFullyDeleted: ".$result." ".$response);

    #if ($result ne "ok") {
    #  return ("fail", "failed to move ".$o." to old_results");
    #}

    Dada::logMsg(1, $dl, $o.": deleted -> old");
  
  }
  return ("ok", "");
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

  # try to cleanly stop things
  if (!(($curr_obs eq "none") || ($curr_obs eq ""))) {
    Dada::logMsg(1, $dl, "controlThread: interrupting obs ".$curr_obs."/".$curr_band);
    interrupt_apsr_vsib_send($send_user, $send_host);
    interrupt_apsr_vsib_proxy();
    interrupt_apsr_vsib_recv($dest_user, $dest_host, $dest_dir, $curr_obs, $curr_band);
    sleep(2);

  } else {
    Dada::logMsg(1, $dl, "controlThread: not interrupting due to no current observation");
  }

  if ( -f $pid_file) {
    Dada::logMsg(2, $dl ,"controlThread: unlinking PID file");
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
  
}

# 
# Handle a SIGPIPE
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";

} 


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

  # ensure required 'external' variables have been set
  if (($server_logger eq "") || ($pid eq "") || ($dest eq "") || ($rate eq "")) {
    return ("fail", "Error: not all package globals were set");
  }

  # check the PID is a valid group for the specified instrument
  my @groups = ();
  my $i = 0;
  my $valid_pid = 0;
  @groups = Dada::getProjectGroups($instrument);
  for ($i=0; $i<=$#groups; $i++) {
    if ($groups[$i] eq $pid) {
      $valid_pid = 1;
    }  
  }
  if (!$valid_pid) {
    return ("fail", "Error: specified PID [".$pid."] was not a group for ".$instrument);
  }

  # check the dest is swin/parkes
  if (($dest ne "swin") && ($dest ne "parkes")) {
    return ("fail", "Error: configured dest [".$dest."] was not swin or parkes");
  }

  # check that only 1 destination has been configured
  if ($cfg{"NUM_".uc($dest)."_DIRS"} < 1) {
    return ("fail", "Error: ".$cfg{"NUM_".uc($dest)."_DIRS"}." were configured. Only 1 is permitted");
  }

  # get the user/host/dir for the specified destination
  ($dest_user, $dest_host, $dest_dir) = split(/:/,$cfg{uc($dest)."_DIR_".$dest_id});

  # if an optional dir has been configured on the command line
  if ($optdir ne "") {
    $dest_dir = $optdir;
  }

  my ($result, $response) = checkDestination($dest_user, $dest_host, $dest_dir);

  if (($result ne "ok") && (!($response =~ m/Disk is being used for READING/))) {
    return ("fail", "Error: ".$response);
  }

  # Ensure more than one copy of this daemon is not running
  ($result, $response) = Dada::checkScriptIsUnique(basename($0));
  if ($result ne "ok") {
    return ($result, $response);
  }

  return ("ok", "");
}


END { }

1;  # return value from file
