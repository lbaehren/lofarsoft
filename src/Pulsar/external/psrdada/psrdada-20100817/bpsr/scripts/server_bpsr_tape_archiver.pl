#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_tape_archiver.pl
#

use lib $ENV{"DADA_ROOT"}."/bin";

use Net::hostent;
use File::Basename;
use Bpsr;           # BPSR Module 
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Constants
#
use constant DEBUG_LEVEL  => 1;
use constant PIDFILE      => "bpsr_tape_archiver.pid";
use constant LOGFILE      => "bpsr_tape_archiver.log";
use constant QUITFILE     => "bpsr_tape_archiver.quit";
use constant TAPES_DB     => "tapes.db";
use constant FILES_DB     => "files.db";
use constant TAPE_SIZE    => "750.00";

#
# Global Variables
#
our %cfg = Bpsr::getBpsrConfig();      # Bpsr.cfg
our $quit_daemon : shared  = 0;
our $dev = "";
our $robot = 0;                        # Flag for a tape robot
our $current_tape = "";                # Whatever tape is currently in the drive
our $type = "";
our $tape_id_pattern = "";
our $indent = "";                      # log message indenting
our $db_dir = "";
our $db_user = "";
our $db_host = "";
our $use_bk = 0;                       # define if bookkeeping is enabled
our $bkid = "";                        # ID of current bookkeepr tape
our $uc_type = "";
our $ssh_opts : shared = "-o BatchMode=yes";
our $apsr_ssh_prefix : shared = "";       # ssh prefix command to connect to apsr
our $apsr_ssh_suffix : shared = "";       # ssh suffix command to connect to apsr
our $apsr_user : shared = "";          # name of host to connect to talk to apsr
our $apsr_host : shared = "";
our $gw_user : shared = "";
our $gw_host : shared = "";

# Autoflush output
$| = 1;

# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;

if ($#ARGV != 0) {
  usage();
  exit(1);
} else {

  $type = @ARGV[0];

  if (($type eq "swin") || ($type eq "parkes")) {
    # OK :) 
  } else {

    if ($type eq "robot_init") {
      ($db_user, $db_host, $db_dir) = split(/:/, $cfg{"SWIN_DB_DIR"});
      print STDERR "WARNING: Initializing ALL tapes in robot in 15 seconds. Press CTRL+C to abort this\n";
      sleep(15);
    } elsif ($type eq "robot_verify") {
      ($db_user, $db_host, $db_dir) = split(/:/, $cfg{"SWIN_DB_DIR"});
      print STDERR "WARNING: Verifying ALL tapes in robot in 15 seconds. Press CTRL+C to abort this\n";
      sleep(15);
    } else {
      usage();
      exit(1);
    }
  }

}

my $required_host = "";

# set the pattern tape id pattern for each location
if (($type eq "swin") || ($type eq "robot_init") || ($type eq "robot_verify")) {
  $tape_id_pattern = "HRA[0-9][0-9][0-9]S4";
  $robot = 1;
  $uc_type = "SWIN";
  $required_host = "shrek202.ssi.swin.edu.au";
  $apsr_ssh_prefix = "ssh -l pulsar shrek210 'ssh -l dada apsr-evlbi.atnf.csiro.au \"";
  $apsr_ssh_suffix = "\"'";
}

if ($type eq "parkes") {
  $tape_id_pattern = "HRE[0-9][0-9][0-9]S4";
  $uc_type = "PARKES";
  $required_host = "jura";
  $apsr_ssh_prefix = "ssh -l dada apsr-srv0 \"";
  $apsr_ssh_suffix = "\"";
}

# sanity check the hostname
if ($ENV{'HOST'} ne $required_host) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOST'}."\n";
  print STDERR "       Must be run on ".$required_host."\n";
  exit(1);
}



#
# Local Varaibles
#

my $daemon_control_thread = 0;

my $i=0;
my $j=0;
my @dirs  = ();
my @hosts = ();
my @users = ();

my $user;
my $host;
my $path;

# setup the disks
for ($i=0; $i<$cfg{"NUM_".$uc_type."_DIRS"}; $i++) {
  ($user, $host, $path) = split(/:/,$cfg{$uc_type."_DIR_".$i},3);
  push (@dirs, $path);
  push (@hosts, $host);
  push (@users, $user);
}

# location of DB files
($db_user, $db_host, $db_dir) = split(/:/, $cfg{$uc_type."_DB_DIR"});

# set global variable for the S4 device name
$dev = $cfg{$uc_type."_S4_DEVICE"};

# setup the bookkeeper ID
my $bookkeepr = $cfg{$uc_type."_BOOKKEEPR"};

my $result;
my $response;

#
# Main
#

my $logfile = $db_dir."/".LOGFILE;
my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;

print "Running ".$type." Tape archiver\n";

if ($type eq "robot_init") {
  if (!$quit_daemon) {
    print STDERR "STARTING ROBOT INITIALISATION\n";
    robotInitializeAllTapes();
    print STDERR "FINISHED ROBOT INITIALISATION\n";
  } else {
    print STDERR "ABORTED ROBOT INITIALISATION\n";
  }
  exit_script(0);
}

if ($type eq "robot_verify") {
  if (!$quit_daemon) {
    print STDERR "STARTING ROBOT VERIFICATION\n";
    ($result, $response) = robotVerifyAllTapes();
    print STDERR "FINISHED ROBOT VERIFICATION: ".$result." ".$response."\n";
  } else {
    print STDERR "ABORTED ROBOT VERIFICATION\n";
  }
  exit_script(0);
}


Dada::daemonize($logfile, $pidfile);

logMessage(0, "STARTING SCRIPT");

setStatus("Scripting starting");

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

# Force a re-read of the current tape. This rewinds the tape
logMessage(1, "main: checking current tape");
($result, $response) = getCurrentTape();

if ($result ne "ok") {
  logMessage(0, "main: getCurrentTape() failed: ".$response);
  ($result, $response) = newTape();
  if ($result ne "ok") {
    logMessage(0, "main: getNewTape() failed: ".$response);
    exit_script(1);
  }
}

$current_tape = $response;
logMessage(1, "main: current tape = ".$current_tape);

setStatus("Current tape: ".$current_tape);

# Get the tape information from the tape database
($result, $response) = getTapeInfo($current_tape);

if ($result ne "ok") {
  logMessage(0, "main : getTapeInfo() failed: ".$response);
  exit_script(1);
}

my ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

# If the current tape is full, we need to switch to the next empty one
if (int($full) == 1) {

  logMessage(1, "tape ".$id." marked full, selecting new tape");

  ($result, $response) = newTape();

  if ($result ne "ok") {
    logMessage(0, "Could not load a new tape: ".$response);
    exit_script(1);
  }

  $current_tape = $response;
  logMessage(1, "New tape selected: ".$current_tape);

  
} else {

  # Move forward to be ready to write on the next available file number
  # the getTapeID function will have placed us on fsf 1, as the
  # binary label is on fsf 0, so we *do* need to increment by n-1
  logMessage(2, "main: tapeFSF(".($nfiles-1).")");
  ($result, $response) = tapeFSF(($nfiles-1));
  logMessage(2, "main: tapeFSF() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "main : tapeFSF failed: ".$response);
    exit_script(1);
  }
}


my $obs = "";
my $beam = "";
my $dir = "";
my $cmd = "";

$i = 0;
$j = 0;
my $waiting = 0;
my $give_up = 0;

while (!$quit_daemon) {

  # look for a file sequentially in each of the @dirs
  $dir = $dirs[$i];
  $host = $hosts[$i];
  $user = $users[$i];

  # Look for files in the @dirs
  logMessage(2, "main: getBeamToTar(".$user.", ".$host.", ".$dir.")");
  ($obs, $beam) = getBeamToTar($user, $host, $dir);
  logMessage(2, "main: getBeamToTar() ".$obs);

  # If we have one, write to tape
  if (($obs ne "none") && ($beam ne "none")) {

    # lock this dir for READING
    $cmd = "touch ".$dir."/READING";
    logMessage(2, "main: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "main: localSshCommand() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "main: could not touch READING flag in ".$dir.": ".$response);
      exit_script(1);
    }

    logMessage(2, "main: tarBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
    ($result, $response) = tarBeam($user, $host, $dir, $obs, $beam);
    logMessage(2, "main: tarBeam() ".$result." ".$response);
    $waiting = 0;

    if ($result eq "ok") {

      logMessage(2, "main: moveCompeltedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
      ($result, $response) = moveCompletedBeam($user, $host, $dir, $obs, $beam);
      logMessage(2, "main: moveCompeltedBeam() ".$result." ".$response);
      if ($result ne "ok") {
        logMessage(0, "main: moveCompletedBeam() failed: ".$response);
        setStatus("Error: could not move beam: ".$obs."/".$beam);
        $give_up = 1;
      }

      # DISABLED FOR TESTING
      # If at parkes, we delete the beam after moving it
      if (($robot eq 0) && ($result eq "ok"))  {
        logMessage(2, "main: deleteCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
        ($result, $response) = deleteCompletedBeam($user, $host, $dir, $obs, $beam);
        logMessage(2, "main: deleteCompletedBeam() ".$result." ".$response);
        if ($result ne "ok") {
          logMessage(0, "main: deleteCompletedBeam() failed: ".$response);
          setStatus("Error: could not delete beam: ".$obs."/".$beam);
          $give_up = 1;
        }
      }

    } else {
      setStatus("Error: ".$response);
      logMessage(0, "main: tarBeam() failed: ".$response);
      $give_up = 1;
    }

    $cmd = "rm -f ".$dir."/READING";
    logMessage(2, "main: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "main: localSshCommand() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "main: could not unlink READING flag in ".$dir.": ".$response);
      $give_up = 1;
    }

    if ($give_up) {
      exit_script(1);
    }

  } else {

    # Just log that we are in the main loop waiting for data
    if (!$waiting) {
      logMessage(1, "main: waiting for obs to transfer");
      setStatus("Waiting for new");
      $waiting = 1;
    }

  }

  $j++;

  # only move onto the next holding area after the 13th
  # beam or handling 13 beams
  if (($beam eq "13") || ($j == 13)) {

    $j=0;
    $i++;
    # reset the dirs counter
    if ($i >= ($#dirs+1)) {
      $i = 0;
    }
    if ($obs eq "none") {
      # Wait 10 secs if we didn't find a file
      sleep (10);
    }
  }
} # main loop

# rejoin threads
$daemon_control_thread->join();

setStatus("Script stopped");

logMessage(0, "STOPPING SCRIPT");

exit 0;



###############################################################################
#
# Functions
#


#
# try and find an observation to tar
#
sub getBeamToTar($$$) {

  indent();

  my ($user, $host, $dir) = @_;
  logMessage(2, "getBeamToTar(".$user.", ".$host.", ".$dir.")");

  my $obs = "none";
  my $beam = "none";
  my $subdir = "";
  my @subdirs = ();
  my $result = "";
  my $response = "";
  my $cmd = "";

  $cmd = "ls -1 ".$dir."/WRITING";
  logMessage(2, "getBeamToTar: localSshCommand(".$user.", ".$host.", ".$cmd);
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "getBeamToTar: localSshCommand() ".$result." ".$response);

  # If the file existed
  if ($result eq "ok") {
    logMessage(2, "getBeamToTar: ignoring dir ".$dir." as this is being written to");

  } else {

    $cmd = "cd ".$dir."; find . -mindepth 3 -maxdepth 3 -name 'xfer.complete' ".
           "-printf '\%h\\n' | sort -r | tail -n 1";
    logMessage(2, "getBeamToTar: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "getBeamToTar: localSshCommand() ".$result." ".$response);

    # if the ssh command failed
    if ($result ne "ok") {
      logMessage(0, "getBeamToTar: ssh cmd [".$user."@".$host."] failed: ".$cmd);
      logMessage(0, "getBeamToTar: ssh response ".$response);
      sleep(1);

    # ssh worked
    } else {

      # find worked 
      if ($response ne "") {
        chomp $response;
        my @arr = split(/\//,$response);
        $obs = $arr[1];
        $beam = $arr[2];
        # remove the leading ./ if it exists
        #$response =~ s/^\.\///;
        #$obs = $response;
        logMessage(2, "getBeamToTar: found ".$obs."/".$beam);
    
      # find failed
      } else {
        logMessage(2, "getBeamToTar: could not find any xfer.complete's");
      } 

    }
  }
  logMessage(3, "getBeamToTar: returning ".$obs.", ".$beam);

  unindent();
  return ($obs, $beam);
}


#
# tars the beam to the tape drive
#
sub tarBeam($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  indent();
  logMessage(2, "tarBeam: (".$user." , ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  logMessage(1, "tarBeam: archiving ".$obs.", beam ".$beam);

  my $cmd = "";
  my $result = "";
  my $response = "";

  # Check if this beam has already been archived
  logMessage(2, "tarBeam: checkIfArchived(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  ($result, $response) = checkIfArchived($user, $host, $dir, $obs, $beam);
  logMessage(2, "tarBeam: checkIfArchived() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tarBeam: checkIfArchived failed: ".$response);
    unindent();
    return ("fail", "checkIfArchived failed: ".$response);
  } 

  # If this beam has been archived, skip it 
  if ($response eq "archived") {
    logMessage(1, "Skipping archival of ".$obs."/".$beam.", already archived");
    unindent();
    return("ok", "beam already archived");
  }

  setStatus("Archiving ".$obs." ".$beam);

  # Check the tape is the expected one
  my $expected_tape = getExpectedTape(); 

  if ($expected_tape ne $current_tape) {
    
    # try to load it?
    logMessage(2, "tarBeam: expected tape mismatch \"".$expected_tape."\" != \"".$current_tape."\"");

    # try to get the tape loaded (robot or manual)
    ($result, $response) = loadTape($expected_tape);

    if ($result ne "ok") {
      unindent();
      return ("fail", "Could not load tape: ".$expected_tape);
    }

    $current_tape = $expected_tape;
  }

  my $tape = $expected_tape;

  # Find the combined file size in bytes
  $cmd = "du -sLb ".$dir."/".$obs."/".$beam;
  logMessage(2, "tarBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "tarBeam: localSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tarBeam: ".$cmd. "failed: ".$response);
    unindent();
    return ("fail", "Could not determine archive size for ".$obs."/".$beam);
  } 

  # get the upper limit on the archive size
  my $size_est_bytes = tarSizeEst(4, int($response));
  my $size_est_gbytes = $size_est_bytes / (1024*1024*1024);

  # check if this beam will fit on the tape
  ($result, $response) = getTapeInfo($tape);

  if ($result ne "ok") {
    logMessage(0, "tarBeam: getTapeInfo() failed: ".$response);
    unindent();
    return ("fail", "could not determine tape information from database");
  } 

  my ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

  logMessage(2, "tarBeam: ".$free." GB left on tape");
  logMessage(2, "tarBeam: size of this beam is estimated at ".$size_est_gbytes." GB");

  if ($free < $size_est_gbytes) {

    logMessage(0, "tarBeam: tape ".$tape." full. (".$free." < ".$size_est_gbytes.")");

    # Mark the current tape as full and load a new tape;
    logMessage(2, "tarBeam: updateTapesDB(".$id.", ".$size.", ".$used.", ".$free.", ".$nfiles);
    ($result, $response) = updateTapesDB($id, $size, $used, $free, $nfiles, 1);
    logMessage(2, "tarBeam: updateTapesDB() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "tarBeam: updateTapesDB() failed: ".$response);
      unindent();
      return ("fail", "Could not mark tape full");
    }

    # Get the next expected tape
    logMessage(1, "tarBeam: getExpectedTape()");
    my $new_tape = getExpectedTape();
    logMessage(1, "tarBeam: getExpectedTape(): ".$new_tape);

    logMessage(1, "tarBeam: loadTape(".$new_tape.")");
    ($result, $response) = loadTape($new_tape);
    logMessage(1, "tarBeam: loadTape() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "tarBeam: newTape() failed: ".$response);
      unindent();
      return ("fail", "New tape was not loaded");
    }

    $tape = $new_tape; 
    $current_tape = $tape;

    # check if this beam will fit on the tape
    logMessage(2, "tarBeam: getTapeInfo(".$tape.")");
    ($result, $response) = getTapeInfo($tape);
    logMessage(2, "tarBeam: getTapeInfo() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "tarBeam: getTapeInfo() failed: ".$response);
      unindent();
      return ("fail", "Could not determine tape information from database");
    }

    # Setup the ID information for the new tape    
    ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

    # If for some reason, this tape has files on it, forward seek
    if ($nfiles > 1) {

      logMessage(2, "tarBeam: tapeFSF(".($nfiles-1).")");
      ($result, $response) = tapeFSF(($nfiles-1));
      logMessage(2, "tarBeam: tapeFSF() ".$result." ".$response);
                                                                                                                                                                           
      if ($result ne "ok") {
        logMessage(0, "tarBeam: tapeFSF failed: ".$response);
        unindent();
        return ("fail", "Could not FSF seek to correct place on tape");
      }
    }
  }

  my $filenum = -1;
  my $blocknum = -1;
  ($result, $filenum, $blocknum) = getTapeStatus();
  if ($result ne "ok") {
    logMessage(0, "TarBeam: getTapeStatus() failed.");
    unindent();
    return ("fail", "Could not get tape status");
  }

  my $ntries = 3;
  while (($filenum ne $nfiles) || ($blocknum ne 0)) {

    # we are not at 0 block of the next file!
    logMessage(0, "tarBeam: WARNING: Tape out of position! (f=".$filenum.
                  ", b=".$blocknum.") Attempt to get to right place.");

    $cmd = "mt -f ".$dev." rewind; mt -f ".$dev." fsf $nfiles";
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      logMessage(0, "TarBeam: tape re-wind/skip failed: ".$response);
      unindent();
      return ("fail", "tape re-wind/skip failed: ".$response);
    }

    ($result, $filenum, $blocknum) = getTapeStatus();
    if ($result ne "ok") { 
      logMessage(0, "TarBeam: getTapeStatus() failed.");
      unindent();
      return ("fail", "getTapeStatus() failed");
    }

    if ($ntries < 1) {
      unindent();
      return ("fail", "TarBeam: Could not get to correct place on tape!");
    }

    $ntries--;
  }


  # Try to ensure that no nc is running on the port we want to use
  my $nc_ready = 0;
  my $tries = 10;
  my $port = 25128;

  while ( (!$nc_ready)  && ($tries > 0) && (!$quit_daemon) ) {

    # This tries to connect to any type of service on specified port
    logMessage(2, "tarBeam: testing nc on ".$host.":".$port);
    if ($robot eq 0) {
      $cmd = "nc -z ".$host." ".$port." > /dev/null";
    } else {
      $cmd = "nc -zd ".$host." ".$port." > /dev/null";
    }
    ($result, $response) = Dada::mySystem($cmd);

    if ($result eq "ok") {
      # This is an error condition!

      logMessage(0, "tarBeam: something running on the NC port ".$port);

      if ($tries < 5) {
        logMessage(0, "tarBeam: trying to increment port number");
        $port += 1;
      } else {
        logMessage(0, "tarBeam: trying again, now that we have tested once");
      }
      $tries--;
      sleep(1);

    # the command failed, meaning there is nothing on that port
    } else {
      logMessage(2, "tarBeam: nc will be available on ".$host.":".$port);
      $nc_ready = 1;

    }
  }

  logMessage(2, "tarBeam: nc_thread(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$port.")");
  my $remote_nc_thread = threads->new(\&nc_thread, $user, $host, $dir, $obs, $beam, $port);

  # Allow some time for this thread to start-up, ssh and launch tar + nc
  sleep(10);

  my $localhost = Dada::getHostMachineName();

  $tries=10;
  while ($tries > 0) {

    # For historical reasons, HRE(robot==0) tapes are written with a slight 
    # different command to HRA (robot==1) tapes
    #
    if ($robot eq 0) {
      $cmd = "nc ".$host." ".$port." | dd of=".$dev." bs=64K";
    } else {
      $cmd = "nc -d ".$host." ".$port." | dd of=".$dev." obs=64k";
    }
      
    logMessage(2, "tarBeam: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);

    # Fatal errors, give up straight away
    if ($response =~ m/Input\/output error/) {
      logMessage(0, "tarBeam: fatal tape error: ".$response);
      $tries = -1;

    # Non fatal errors
    } 
    elsif (($result ne "ok") || ($response =~ m/refused/) || ($response =~ m/^0\+0 records in/)) {
      logMessage(2, "tarBeam: ".$result." ".$response);
      logMessage(0, "tarBeam: failed to write archive to tape: ".$response);
      $tries--;
      $result = "fail";
      $response = "failed attempt at writing archive";
      sleep(2);

    } else {
      # Just the last line of the DD command is relvant:
      my @temp = split(/\n/, $response);
      logMessage(1, "tarBeam: ".$result." ".$temp[2]);
      my @vals = split(/ /, $temp[2]);
      my $bytes_written = int($vals[0]);
      my $bytes_gb = $bytes_written / (1024*1024*1024);

      # If we didn't write anything, its due to the nc client connecting 
      # before the nc server was ready to provide the data, simply try to reconnect
      if ($bytes_gb == 0) {
        logMessage(0, "tarBeam: nc server not ready, sleeping 2 seconds");
        $tries--;
        sleep(2);

      # If we did write something, but it didn't match bail!
      } elsif ( ($size_est_gbytes - $bytes_gb) > 0.01) {
        $result = "fail";
        $response = "not enough data received by nc: ".$size_est_gbytes.
                    " - ".$bytes_gb." = ".($size_est_gbytes - $bytes_gb);
        logMessage(0, "tarBeam: ".$result." ".$response);
        $tries = -1;

      } else {

        logMessage(2, "tarBeam: est_size ".sprintf("%7.4f GB", $size_est_gbytes).
                      ", size = ".sprintf("%7.4f GB", $bytes_gb));
        $tries = 0;
      }
    }
  }

  if ($result ne "ok") {

    logMessage(0, "tarBeam: failed to write archive to tape: ".$response);
    logMessage(0, "tarBeam: attempting to clear the current nc server command");

    if ($robot eq 0) {
      $cmd = "nc -z ".$host." ".$port." > /dev/null";
    } else {
      $cmd = "nc -zd ".$host." ".$port." > /dev/null";
    }
    logMessage(0, "tarBeam: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMessage(0, "tarBeam: ".$result." ".$response);

    $remote_nc_thread->detach();
    unindent();
    return ("fail","Archiving failed");
  }
  
  logMessage(2, "tarBeam: joining nc_thread()");
  ($result, $response) = $remote_nc_thread->join();
  logMessage(2, "tarBeam: nc_thread() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tarBeam: remote tar/nc thread failed: ".$response);
    unindent();
    return ("fail","Archiving failed");
  }

  # Now check that the File number has been incremented one. Sometimes the
  # file number is not incremented, which usually means an error...
  ($result, my $filenum, my $blocknum) = getTapeStatus();
  if ($result ne "ok") {
    logMessage(0, "tarBeam: getTapeStatus() failed.");
    unindent();
    return ("fail", "getTapeStatus() failed");
  }
  
  if (($blocknum ne 0) || ($filenum ne ($nfiles+1))) {
    logMessage(0, "tarBeam: positioning error after write: filenum=".$filenum.", blocknum=".$blocknum);
    unindent();
    return ("fail", "write failed to complete EOF correctly");
  }

  # remove the xfer.complete in the beam dir if it exists
  $cmd = "ls -1 ".$dir."/".$obs."/".$beam."/xfer.complete";
  logMessage(2, "tarBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "tarBeam: localSshCommand() ".$result." ".$response);

  if ($result eq "ok") {
    $cmd = "rm -f ".$dir."/".$obs."/".$beam."/xfer.complete";
    logMessage(2, "tarBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "tarBeam: localSshCommand() ".$result." ".$response);
    if ($result ne "ok") {
      logMessage(2, "tarBeam: couldnt unlink ".$obs."/".$beam."/xfer.complete");
    }
  }

  # Else we wrote files to the TAPE in 1 archive and need to update the database files
  $used += $size_est_gbytes;
  $free -= $size_est_gbytes;
  $nfiles += 1;

  # If less than 100 MB left, mark tape as full
  if ($free < 0.1) {
    $full = 1;
  }

  # Log to the bookkeeper if defined
  if ($use_bk) {
    logMessage(2, "tarBeam: updateBookKeepr(".$obs."/".$beam."/".$obs."/.psrxml");
    ($result, $response) = updateBookKeepr($obs."/".$beam."/".$obs."/.psrxml",$bookkeepr,$id,$uc_type,($nfiles-1));
    logMessage(2, "tarBeam: updateBookKeepr(): ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "tarBeam: updateBookKeepr() failed: ".$response);
      unindent();
      return("fail", "error ocurred when updating BookKeepr: ".$response);
    }
  }

  logMessage(2, "tarBeam: updatesTapesDB($id, $size, $used, $free, $nfiles, $full)");
  ($result, $response) = updateTapesDB($id, $size, $used, $free, $nfiles, $full);
  logMessage(2, "tarBeam: updatesTapesDB(): ".$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "tarBeam: updateTapesDB() failed: ".$response);
    unindent();
    return("fail", "error ocurred when updating tapes DB: ".$response);
  }

  logMessage(2, "tarBeam: updatesFilesDB(".$obs."/".$beam.", ".$id.", ".$size_est_gbytes.", ".($nfiles-1).")");
  ($result, $response) = updateFilesDB($obs."/".$beam, $id, $size_est_gbytes, ($nfiles-1));
  logMessage(2, "tarBeam: updatesFilesDB(): ".$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "tarBeam: updateFilesDB() failed: ".$response);
    unindent();
    return("fail", "error ocurred when updating filesDB: ".$response);
  }

  logMessage(2, "tarBeam: markSentToTape(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  ($result, $response) = markSentToTape($user, $host, $dir, $obs, $beam);
  logMessage(2, "tarBeam: markSentToTape(): ".$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "tarBeam: markSentToTape failed: ".$response);
  }

  unindent();
  return ("ok",""); 

}


#
# move a completed obs on the remote dir to the on_tape directory
#
sub moveCompletedBeam($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;
  indent();
  logMessage(2, "moveCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $result = "";
  my $response = "";
  my $cmd = "";

  # ensure the remote directory is created
  $cmd = "mkdir -p ".$dir."/../on_tape/".$obs;
  logMessage(2, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

   # move the beam
  $cmd = "cd ".$dir."; mv ".$obs."/".$beam." ../on_tape/".$obs."/";
  logMessage(2, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "moveCompletedBeam: failed to move ".$obs."/".$beam," to on_tape dir: ".$response);
  } else {

    # if there are no other beams in the observation directory, then we can delete it
    # old, then we can remove it also

    $cmd = "find ".$dir."/".$obs."/ -mindepth 1 -type d | wc -l";
    logMessage(2, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

    if (($result eq "ok") && ($response eq "0")) {

      # delete the remote directory
      $cmd = "rmdir ".$dir."/".$obs;
      logMessage(2, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
      ($result, $response) = localSshCommand($user, $host, $cmd);
      logMessage(2, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

      if ($result ne "ok") {
        logMessage(0, "moveCompletedBeam: could not delete ".$user."@".$host.":".$dir."/".$obs.": ".$response);
      }
    }
  }

  unindent();
  return ($result, $response);

}

#
# Delete the beam from the specified location
#
sub deleteCompletedBeam($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;
  indent();
  logMessage(2, "deleteCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $result = "";
  my $response = "";
  my $cmd = "";

  # ensure the remote directory exists
  $cmd = "ls -1d ".$dir."/../on_tape/".$obs."/".$beam;
  logMessage(2, "deleteCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "deleteCompletedBeam: localSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "deleteCompletedBeam: ".$user."@".$host.":".$dir."/".$obs."/".$beam." did not exist");
    
  } else {

    # delete the remote directory (just do whole obs)
    $cmd = "rm -rf ".$dir."/../on_tape/".$obs;
    logMessage(2, "deleteCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    logMessage(2, "deleteCompletedBeam: localSshCommand() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "deleteCompletedBeam: could not delete ".$user."@".$host.":".$dir."/".
                    $obs."/".$beam.": ".$response);
    }
  }

  unindent();
  return ($result, $response);
}

#
# Rewinds the current tape and reads the first "index" file
#
sub getCurrentTape() {
 
  indent(); 
  logMessage(2, "getCurrentTape()"); 

  my $result = "";
  my $response = "";
  my $tape_id = "";

  # First we need to check whether a tape exists in the robot
  logMessage(2, "getCurrentTape: tapeIsLoaded()");
  ($result, $response) = tapeIsLoaded();
  logMessage(2, "getCurrentTape: tapeIsLoaded ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "getCurrentTape: tapeIsLoaded failed: ".$response);
    unindent();
    return ("fail", "could not determine if tape is loaded in drive");
  }

  # If there is a tape in the drive...
  if ($response eq 1) {
    logMessage(2, "getCurrentTape: tapeGetID()");
    ($result, $response) = tapeGetID();
    logMessage(2, "getCurrentTape: tapeGetID() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "getCurrentTape: tapeGetID failed: ".$response);
      unindent();
      return ("fail", "bad binary label on current tape");
    }

    # we either have a good ID or no ID
    $tape_id = $response;

  # no tape is loaded in the drive
  } else {

    # get the expected tape to be loaded
    logMessage(2, "getCurrentTape: getExpectedTape()");
    ($result, $response) = getExpectedTape();
    logMessage(2, "getCurrentTape: getExpectedTape() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "getCurrentTape: getExpectedTape() failed: ".$response);
      unindent();
      return ("fail", "could not determine the tape to be loaded");
    }

    my $expected_tape = $response;
    logMessage(2, "getCurrentTape: loadTape(".$expected_tape.")");
    ($result, $response) = loadTape($expected_tape);
    logMessage(2, "getCurrentTape: loadTape() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "getCurrentTape: loadTape() failed: ".$response);
      unindent();
      return ("fail", "could not load the expected tape");
    }

    logMessage(2, "getCurrentTape: expected tape now loaded");
    $tape_id = $expected_tape;
  }

  # Robot / Swinburne
  if ($robot) {

    my $robot_id = "";

    # also get the current tape in the robot
    logMessage(2, "getCurrentTape: getCurrentRobotTape()");
    ($result, $response) = getCurrentRobotTape();
    logMessage(2, "getCurrentTape: getCurrentRobotTape() ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(2, "getCurrentTape: getCurrentRobotTape failed: ".$response);
      unindent();
      return ("fail", "robot could not determine tape label");

    } else {
      $robot_id = $response;
    }

    # check for binary label
    if ($tape_id eq "") {

      # if the robots id was sensible, use it
      if ($robot_id =~ m/^$tape_id_pattern$/) {

        logMessage(2, "getCurrentTape: tapeInit(".$robot_id.")");
        ($result, $response) = tapeInit($robot_id);
        logMessage(2, "getCurrentTape: tapeInit() ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "getCurrentTape: tapeInit() failed: ".$response);
          unindent();
          return ("fail", "could not initialise tape");

        # tape was successfully initialized
        } else {
          logMessage(1, "getCurrentTape: successfully intialized tape: ".$response);
          unindent();
          return ("ok", $response);
        }

      } else {

        logMessage(0, "getCurrentTape: no binary label and physical label was malformed");
        unindent();
        return ("fail", "no binary label and physical label was malformed");

      }
 
    # the binary label existed  
    } else {

      # check that the binary label matches the pattern
      if (!($tape_id =~ m/^$tape_id_pattern$/)) {
        logMessage(0, "getCurrentTape: binary label ".$tape_id." did not match pattern");
        unindent();
        return ("fail", "binary label on tape ".$tape_id." was malformed");
      }

      # if the robots id was sensible, use it
      if (!($robot_id =~ m/^$tape_id_pattern$/)) {
        logMessage(0, "getCurrentTape: physical label ".$robot_id." did not match pattern");
        unindent();
        return ("fail", "physical label on tape ".$robot_id." was malformed");
      }

      if ($robot_id ne $tape_id) {
        logMessage(0, "getCurrentTape: physical label ".$robot_id." did not match binary label".$tape_id);
        unindent();
        return ("fail", "physical and binary labels did not match (".$robot_id." != ".$tape_id.")");
      }

      # ID's matched and we of the right form
    }

  # if we have no robot
  } else {

    # check for empty tape with no binary label
    if ($tape_id eq "") {
      logMessage(0, "getCurrentTape: no binary label existed on tape");
      unindent();
      return ("fail", "no binary label existed on tape");
    }

    # check that the binary label matched the pattern
    if (!($tape_id =~ m/^$tape_id_pattern$/)) {
      logMessage(0, "getCurrentTape: binary label ".$tape_id." did not match pattern");
      unindent();
      return ("fail", "binary label on tape ".$tape_id." was malformed")
    } 

  }

  logMessage(2, "getCurrentTape: current tape = ".$tape_id);
  unindent();
  return ("ok", $tape_id);

}

#
# Get tape specified loaded into the drive. fail on timeout
#
sub loadTape($) {

  (my $tape) = @_;
  indent();

  logMessage(2, "loadTape (".$tape.")");
  my $cmd = "";

  if ($robot) {

    logMessage(2, "loadTape: robotGetStatus()");
  
    my %status = robotGetStatus();
    my @keys = keys (%status);

    my $slot = "none";

    # find the tape
    my $i=0;
    for ($i=0; $i<=$#keys; $i++) {
      if ($tape eq $status{$keys[$i]}) {
        $slot = $keys[$i];
      }
    }

    if ($slot eq "none") {

      logMessage(0, "loadTape: tape ".$tape." did not exist in robot");
      unindent();
      return ("fail", "tape not in robot") ;

    } elsif ($slot eq "transfer") {

      logMessage(2, "loadTape: tape ".$tape." was already in transfer slot");
      unindent();
      return ("ok","");

    } else {

      logMessage(2, "loadTape: tape ".$tape." in slot ".$slot);

      # if a tape was actually loaded
      if ($status{"transfer"} ne "Empty") {

        # unload the current tape
        logMessage(2, "loadTape: robotUnloadCurrentTape()");
        ($result, $response) = robotUnloadCurrentTape();
        if ($result ne "ok") {
          logMessage(0, "loadTape: robotUnloadCurrentTape failed: ".$response);
          unindent();
          return ("fail", "Could not unload current robot tape: ".$response);
        }
      }

      # load the tape in the specified slot
      logMessage(2, "loadTape: robotLoadTapeFromSlot(".$slot.")");
      ($result, $response) = robotLoadTapeFromSlot($slot);
      logMessage(2, "loadTape: robotLoadTapeFromSlot: ".$result." ".$response);
      if ($result ne "ok") {
        logMessage(0, "loadTape: robotLoadTapeFromSlot failed: ".$response);
        unindent();
        return ("fail", "Could not load tape in robot slot ".$slot);
      }

      # Now that its loaded ok, check the ID matches the barcode
      logMessage(2, "loadTape: tapeGetID()");
      ($result, $response) = tapeGetID();
      logMessage(2, "loadTape: tapeGetID: ".$result." ".$response);

      if ($result ne "ok") {
        logMessage(2, "loadTape: tapeGetID() failed: ".$response);
        unindent();
        return ("fail", "could not get ID from newly loaded tape: ".$response);
      }

      my $id = $response;

      # Test that the new tapes ID matched
      if ($id ne $tape) {

        # Initialize the tape with the specified ID 
        logMessage(0, "ID on tape \"".$id."\" did not match tape label \"".$tape."\"");

        logMessage(2, "loadTape: tapeInit(".$tape.")");
        ($result, $response) = tapeInit($tape);
        logMessage(2, "loadTape: tapeInit: ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "loadTape: tapeInit failed: ".$response);
          unindent();
          return ("fail", "could not initialize tape");
        }
      }
      unindent();
      return ("ok", "");
    }

  } else {

    # No tape robot
    my $inserted_tape = "none";
    my $n_tries = 10;

    while (($inserted_tape ne $tape) && ($n_tries >= 0) && (!$quit_daemon)) {

      # Ask the user to insert the tape
      logMessage(2, "loadTape: manualInsertTape()");
      ($result, $response) = manualInsertTape($tape);
      logMessage(2, "loadTape: manualInsertTape: ".$result." ".$response);
      if ($result ne "ok") {
        logMessage(0, "loadTape: manualInsertTape() failed: ".$response);
      }

      logMessage(2, "loadTape: tapeGetID()");
      ($result, $response) = tapeGetID();
      logMessage(2, "loadTape: tapeGetID() ".$result." ".$response);
      if ($result ne "ok") {
        logMessage(0, "loadTape: tapeGetID() failed: ".$response);
      } else {
        $inserted_tape = $response;
      }

      $n_tries--;

    }

    unindent();
    if ($inserted_tape eq $tape) {
      return ("ok", "");
    } else {
      return ("fail", $tape." was not inserted after 10 attempts");
    }
  }
}


#
# Read the local tape database and determine what the current
# tape should be
#
sub getExpectedTape() {
  
  indent();
  my $fname = $db_dir."/".TAPES_DB;
  my $expected_tape = "none";
  my $newbkid;
  $bkid="";

  logMessage(2, "getExpectedTape: ()");

  open FH, "<".$fname or return ("fail", "Could not read tapes db ".$fname); 
  my @lines = <FH>;
  close FH;

  my $line = "";
  # parse the file
  foreach $line (@lines) {

    chomp $line;

    if ($line =~ /^#/) {
      # ignore comments
    } else {

      logMessage(3, "getExpectedTape: testing ".$line);

      if ($expected_tape eq "none") {
        my ($id, $size, $used, $free, $nfiles, $full, $newbkid) = split(/ +/,$line);
     
        if (int($full) == 1) {
          logMessage(2, "getExpectedTape: skipping tape ".$id.", marked full");
        } elsif ($free < 0.1) {
          logMessage(1, "getExpectedTape: skipping tape ".$id." only ".$free." MB left");
        } else {
          $expected_tape = $id;
          $bkid=$newbkid;
        }
      }
    }
  } 
  logMessage(2, "getExpectedTape: returning ".$expected_tape);
  unindent();
  if ($expected_tape ne "none") {
    return ("ok", $expected_tape);
  } else {
    return ("fail", "could not find acceptable tape");
  }
}

#
# Determine what the next tape should be from tapes.db
# and try to get it loaded
#
sub newTape() {

  indent();
  logMessage(2, "newTape()");

  my $result = "";
  my $response = "";

  # Determine what the "next" tape should be
  logMessage(2, "newTape: getExpectedTape()");
  ($result, $response) = getExpectedTape();
  logMessage(2, "newTape: getExpectedTape() ".$result." ".$response);

  if ($result ne "ok") {
    unindent();
    return ("fail", "getExpectedTape failed: ".$response);
  }
  
  my $new_tape = $response;

  # Now get the tape loaded
  logMessage(2, "newTape: loadTape(".$new_tape.")");
  ($result, $response) = loadTape($new_tape);
  logMessage(2, "newTape: loadTape(): ".$result." ".$response);

  if ($result ne "ok") {
    unindent();
    return ("fail", "loadTape failed: ".$response);
  }

  unindent();
  return ("ok", $new_tape);

}

################################################################################
##
## DATABASE FUNCTIONS
##


#
# Not sure what this does!
#
sub updateBookKeepr($$){
  my ($psrxmlfile,$bookkeepr,$number) = @_;
  my $cmd="";
  my $result="";
  my $response="";
  my $psrxmlid="";
  if ($bkid=="0"){
    $cmd="book_create_tape $bookkeepr $id $uc_type | & sed -e 's:.*<id>\\([^>]*\\)</id>.*:\\1:p' -e 'd'";
    ($result,$response) = Dada::mySystem($cmd);
    if($result ne "ok"){
      return ($result,$response);
    }
    $bkid=$response
  }
  $psrxmlid=`sed -e 's:.*<id>\\([^>]*\\)</id>.*:\\1:p' -e 'd' $psrxmlfile`;
  if($psrxmlid==""){
    return ("fail","<id> not set in the psrxml file");
  }
  $cmd="book_write_to_tape $bookkeepr $psrxmlid $bkid $number";
  ($result,$response) = Dada::mySystem($cmd);
  if($result ne "ok"){
          return ($result,$response);
  }

}

#
# Get a hash of the Tapes DB
#
sub readTapesDB() {

  indent();

  logMessage(2, "readTapesDB()");

  my $fname = $db_dir."/".TAPES_DB;

  open FH, "<".$fname or return ("fail", "Could not read tapes db ".$fname);
  my @lines = <FH>;
  close FH;

  my $id = "";
  my $size = "";
  my $used = "";
  my $free = "";
  my $nfiles = "";
  my $full = "";
  my $line;

  my @db = ();
  my $i = 0;

  foreach $line (@lines) {

    logMessage(3, "readTapesDB: processing line: ".$line);
    ($id, $size, $used, $free, $nfiles, $full) = split(/ +/,$line);

    $db[$i] = {
      id => $id,
      size => $size,
      used => $used,
      free => $free,
      nfiles => int($nfiles),
      full => $full,
    };
    $i++

  }

  unindent();
  return @db;

}


#
# Update the tapes database with the specified information
#
sub updateTapesDB($$$$$$) {

  my ($id, $size, $used, $free, $nfiles, $full) = @_;

  indent();

  logMessage(2, "updateTapesDB: ($id, $size, $used, $free, $nfiles, $full)");

  my $fname = $db_dir."/".TAPES_DB;
  my $expected_tape = "none";

  open FH, "<".$fname or return ("fail", "Could not read tapes db ".$fname);
  my @lines = <FH>;
  close FH;

  my $newline = $id."  ";

  $newline .= floatPad($size, 3, 2)."  ";
  $newline .= floatPad($used, 3, 2)."  ";
  $newline .= floatPad($free, 3, 2)."  ";
  $newline .= sprintf("%06d",$nfiles)."  ";
  $newline .= $full;
  $newline .= "    $bkid";

  #my $newline = $id."  ".sprintf("%05.2f",$size)."  ".sprintf("%05.2f",$used).
  #              "  ".sprintf("%05.2f",$free)."  ".$nfiles."       ".$full."\n";

  open FH, ">".$fname or return ("fail", "Could not write to tapes db ".$fname);

  # parse the file
  my $line = "";
  foreach $line (@lines) {

    if ($line =~ /^$id/) {
      logMessage(1, "updateTapesDB: ".$newline);
      print FH $newline."\n";
    } else {
      print FH $line;
    }
  
  }

  close FH;

  unindent();
  return ("ok", "");

}

#
# update the Files DB
#
sub updateFilesDB($$$$) {

  my ($archive, $tape, $fsf, $size) = @_;

  indent();
  logMessage(2, "updateFilesDB(".$archive.", ".$tape.", ".$fsf.", ".$size.")");

  my $fname = $db_dir."/".FILES_DB;

  my $date = Dada::getCurrentDadaTime();

  my $newline = $archive." ".$tape." ".$date." ".$fsf." ".$size;

  open FH, ">>".$fname or return ("fail", "Could not write to tapes db ".$fname);
  logMessage(1, "updateFilesDB: ".$newline);
  print FH $newline."\n";
  close FH;

  unindent();
  return ("ok", "");
}


sub getTapeInfo($) {

  my ($id) = @_;

  indent();
  logMessage(2, "getTapeInfo: (".$id.")");

  my $fname = $db_dir."/".TAPES_DB;

  open FH, "<".$fname or return ("fail", "Could not read tapes db ".$fname);
  my @lines = <FH>;
  close FH;

  my $size = -1;
  my $used = -1;
  my $free = -1;
  my $nfiles = 0;
  my $full = 0;

  # parse the file
  my $line = "";
  foreach $line (@lines) {
    
    chomp $line;

    if ($line =~ m/^$id/) {

      logMessage(3, "getTapeInfo: processing line: ".$line);
      ($id, $size, $used, $free, $nfiles, $full) = split(/ +/,$line);
      
    } else {

      logMessage(3, "getTapeInfo: ignoring line: ".$line);
      #ignore
    }
  }

  $nfiles = int($nfiles);

  if ($size eq -1) {
    unindent();
    return ("fail", "could not determine space from tapes.db");
  } else {

    logMessage(2, "getTapeInfo: id=".$id.", size=".$size.", used=".$used.", free=".$free.", nfiles=".$nfiles.", full=".$full);
    unindent();
    return ("ok", $id.":".$size.":".$used.":".$free.":".$nfiles.":".$full);
  }

}





#
# get the current tape in the robot
#
sub getCurrentRobotTape() {

  indent();
  logMessage(2, "getCurrentRobotTape()");

  my $cmd = "mtx status | grep 'Data Transfer Element' | awk '{print \$10}'";
  logMessage(2, "getCurrentRobotTape: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {

    logMessage(0, "getCurrentRobotTape: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "could not determine current tape in robot");

  }

  logMessage(2, "getCurrentRobotTape: ID = ".$response);
  unindent();
  return ("ok", $response);
   
}

#
# Checks the beam directory to see if it has been marked as archived
# and also checks the files.db to check if it has been recorded as
# archived. Returns an error on mismatch.
#
sub checkIfArchived($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;
  indent();
  logMessage(2, "checkIfArchived(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $cmd = "";

  my $archived_db = 0;    # If the obs/beam is recorded in FILES_DB
  my $archived_disk = 0;  # If the obs/beam has been marked with shent.to.tape file 

  # Check the files.db to see if the beam is recorded there
  $cmd = "grep '".$obs."/".$beam."' ".$db_dir."/".FILES_DB;
  logMessage(2, "checkIfArchived: ".$cmd);
  my $grep_result = `$cmd`;
    
  # If the grep command failed, probably due to the beam not existing in the file
  if ($? != 0) {

    logMessage(2, "checkIfArchived: ".$obs."/".$beam." did not exist in ".$db_dir."/".FILES_DB);
    $archived_db = 0;

  } else {

    logMessage(2, "checkIfArchived: ".$obs."/".$beam." existed in ".$db_dir."/".FILES_DB);
    $archived_db = 1;

    # check there is only 1 entry in files.db
    my @lines = split(/\n/, $grep_result);
    if ($#lines != 0) {
      logMessage(0, "checkIfArchived: more than 1 entry for ".$obs."/".$beam." in ".$db_dir."/".FILES_DB);
      unindent();
      return("fail", $obs."/".$beam." had more than 1 entry in FILES database");
    } 

  }

  # Check the directory for a on.tape.type file
  $cmd = "ls -1 ".$dir."/".$obs."/".$beam."/on.tape.".$type;
  logMessage(2, "checkIfArchived: localSshCommand(".$user.", ".$host.", ".$cmd);
  ($result, $response) = localSshCommand($user, $host, $cmd);
  logMessage(2, "checkIfArchived: localSshCommand() ".$result." ".$response);

  if ($result eq "ok") {
    $archived_disk = 1;
    logMessage(2, "checkIfArchived: ".$user."@".$host.":".$dir."/".$obs."/".$beam."/on.tape.".$type." existed");
  } else {
    logMessage(2, "checkIfArchived: ".$user."@".$host.":".$dir."/".$obs."/".$beam."/on.tape.".$type." did not exist");
    $archived_disk = 0;
  }

  unindent();

  if (($archived_disk == 0) && ($archived_db == 0)) {
    return ("ok", "not archived");
  } elsif (($archived_disk == 1) && ($archived_db == 1)) {
    return ("ok", "archived");
  } else {
    return ("fail", "FILES database does not match flagged files on disk");
  }

}




###############################################################################
##
## ROBOT FUNCTIONS
##


#
# Return array of current robot status
#
sub robotGetStatus() {

  indent();
  logMessage(2, "robotGetStatus()");
  my $cmd = "";
  my $result = "";
  my $response = "";
  
  $cmd = "mtx status";
  logMessage(2, "robotGetStatus: ".$cmd);

  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    logMessage(2, "robotGetStatus: ".$cmd." failed: ".$response);
    unindent();
    return "fail";
  }

  # parse the response
  my $line = "";
  my @lines = split(/\n/,$response);

  my %results = ();

  foreach $line (@lines) {

    my @tokens = ();
    @tokens = split(/ +/,$line);

    if ($line =~ m/^Data Transfer Element/) {

      logMessage(3, "Transfer: $line");
      if ($tokens[3] eq "0:Full") {
        $results{"transfer"} = $tokens[9];
      } else {
        $results{"transfer"} = "Empty";
      }

    } elsif ($line =~ m/Storage Element/) {

      logMessage(3, "Storage: $line");
      my ($slotid, $state) = split(/:/,$tokens[3]);
      
      if ($state eq "Empty") {
        $results{$slotid} = "Empty";
      } else {
        my ($junk, $tapeid) = split(/=/,$tokens[4]);
        $results{$slotid} = $tapeid;
      } 
    } else {
      # ignore
    }
  } 

  unindent();
  return %results;
}



#
# Unloads the tape currently in the robot
#
sub robotUnloadCurrentTape() {

  indent();

  logMessage(2, "robotUnloadCurrentTape()");
  my $cmd = "";

  $cmd = "mt -f ".$dev." eject";
  logMessage(2, "robotUnloadCurrentTape: ".$cmd);

  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    logMessage(0, "robotUnloadCurrentTape: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "eject command failed");
  }

  $cmd = "mtx unload";
  logMessage(2, "robotUnloadCurrentTape: ".$cmd);
                                                                                                                             
  ($result, $response) = Dada::mySystem($cmd);
                                                                                                                             
  if ($result ne "ok") {
    logMessage(0, "robotUnloadCurrentTape: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "mtx unload command failed");;
  }

  unindent();
  return ("ok", "");

}

#
# load the tape in the specified slot
#
sub robotLoadTapeFromSlot($) {

  (my $slot) = @_;
  indent();
  
  logMessage(2, "robotLoadTapeFromSlot(".$slot.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mtx load ".$slot." 0";
  logMessage(2, "robotLoadTapeFromSlot: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "robotLoadTapeFromSlot: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "robotLoadTapeFromSlot: ".$cmd." failed: ".$response);
    unindent();
    return "fail";
  }

  unindent();
  return ("ok", "");

}

#
# For each tape in the robot, read its label via mtx and write the
# binary label to the first file on the tape. Update the tapes DB
#
sub robotInitializeAllTapes() {

  logMessage(1, "robotInitializeAllTapes()");
  my $result = "";
  my $response = "";
  my $init_error = 0;

  # Get a listing of all tapes
  logMessage(2, "robotInitializeAllTapes: robotGetStatus()");
  my %status = robotGetStatus();

  if ($status{"transfer"} ne "Empty") {

    # Unload whatever tape is in the drive
    logMessage(2, "robotInitializeAllTapes: robotUnloadCurrentTape()");
    ($result, $response) = robotUnloadCurrentTape();
    logMessage(2, "robotInitializeAllTapes: robotUnloadCurrentTape() ".$result." ".$response);
    
    if ($result ne "ok") {
      $init_error = 1;
      logMessage(0, "robotInitializeAllTapes: robotUnloadCurrentTape failed: ".$response);
      return ($result, $response);
    }

  }

  # Get a listing of all tapes
  logMessage(2, "robotInitializeAllTapes: robotGetStatus()");
  %status = robotGetStatus();
  my @keys = keys (%status);

  my $i=0;

  # Go through the sloats, and initialize each tape
  for ($i=0; (($i<=$#keys) && (!$init_error) && (!$quit_daemon)); $i++) {
    my $slot = $keys[$i];

    if ($slot eq "transfer") {
      # ignore the (now empty) transfer slot

    } elsif ($status{$slot} eq "Empty") {
      # ignore empty slots

    } else {

      my $tape = $status{$slot};
      logMessage(1, "robotInitializeAllTapes: initializing tape ".$tape);

      logMessage(2, "robotInitializeAllTapes: robotLoadTapeFromSlot(".$slot.")");
      ($result, $response) = robotLoadTapeFromSlot($slot);
      logMessage(2, "robotInitializeAllTapes: robotLoadTapeFromSlot() ".$result." ".$response);

      if ($result ne "ok") {
        logMessage(0, "robotInitializeAllTapes: robotLoadTapeFromSlot failed: ".$response);
        $init_error = 1;

      } else {
  
        logMessage(2, "robotInitializeAllTapes: tapeInit(".$tape.")");
        ($result, $response) = tapeInit($tape);
        logMessage(2, "robotInitializeAllTapes: tapeInit() ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "robotInitializeAllTapes: tapeInit failed: ".$response);
          $init_error = 1;

        } else {

          # Unload whatever tape is in the drive
          logMessage(2, "robotInitializeAllTapes: robotUnloadCurrentTape()");
          ($result, $response) = robotUnloadCurrentTape();
          logMessage(2, "robotInitializeAllTapes: robotUnloadCurrentTape() ".$result." ".$response);

          if ($result ne "ok") {
            logMessage(0, "robotInitializeAllTapes: robotUnloadCurrentTape() failed: ".$response);
            $init_error = 1;
          }

        }
      }
    }
  }

  unindent();
  if ($init_error) {
    return ("fail", $response);
  } else {
    return ("ok", "");
  }
  
}


#
# Verify all the tapes in the robot, untarring each and every observation and
# checking the file size vs the files.da
#
sub robotVerifyAllTapes() {

  indent();
  logMessage(1, "robotVerifyAllTapes()");

  # verifyLog("TAPE FILE OBSERVATION RESULT TAPE_SIZE FILES.DBSISZE AUX_TAR_MD5SUM");

  my $result = "";
  my $response = "";
  my $local_dir = "/lfs/data0/pulsar/tape_verify";
  my $cmd = "";
  
  if (! -d $local_dir) {
    $cmd = "mkdir -p ".$local_dir;
    logMessage(1, "robotVerifyAllTapes: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);

    if ($result ne "ok") {
      logMessage(0, "robotVerifyAllTapes: could not create tape verification dir");
      unindent();
      return ("fail", "could not create local temp dir");
    }
  }

  chdir $local_dir;
  if ($? != 0) {
    logMessage(0, "robotVerifyAllTapes: could not chdir to local tape verifcation dir");
    unindent();
    return ("fail", "could not chdir to ".$local_dir);
  }

  # TODO Check for enough local disk space...
  # 20 GB should be enough

  my $i = 0;
  my $curr_tape = "none";
  my $nfiles = 0;
  my $ifile  = 0;

  # get the full tapes.db listing to determine the tapes to check
  my @tapes_db = readTapesDB();

  # foreach tape
  TAPES: for ($i=0; (($i<$#tapes_db) && (!$quit_daemon)); $i++) {

    $curr_tape = $tapes_db[$i]{"id"};
    $nfiles = $tapes_db[$i]{"nfiles"};

    # If this tape has files on it
    if ($tapes_db[$i]{"nfiles"} > 1) {

      # load the tape
      logMessage(2, "robotVerifyAllTapes: loadTape(".$curr_tape.")");
      ($result, $response) = loadTape($curr_tape);
      logMessage(2, "robotVerifyAllTapes:loadTape() ".$result." ".$response);

      if ($result ne "ok") {
        logMessage(0, "robotVerifyAllTapes: loadTape failed: ".$response);
        unindent();
        return ("fail", "could not load tape ".$curr_tape);
      }

      # ensure tape is rewinded
      logMessage(2, "robotVerifyAllTapes: tapeGetID()");
      ($result, $response) = tapeGetID();
      logMessage(2, "robotVerifyAllTapes: tapeGetID() ".$result." ".$response);
      
      # We have our tape loaded and a ready to go through each file checking
      # with the database

      # foreach FSF
      my $skipper = 0;
      FILES: for ($ifile=1; (($ifile<$nfiles) && (!$quit_daemon)); $ifile++) {

        logMessage(1, "robotVerifyAllTapes: ".$curr_tape.": file no ".$ifile);

        # If we have already verified this, skip
        $cmd = "grep \"".$curr_tape." ".$ifile." \" ".$db_dir."/verify.log | grep \" OK \"";
        logMessage(3, "robotVerifyAllTapes: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMessage(3, "robotVerifyAllTapes: ".$result." ".$response);

        if ($result eq "ok") {
          $skipper++;
          logMessage(1, "robotVerifyAllTapes: skipping ".$curr_tape." ".$ifile.", previously verified");
          next FILES;
        }

        logMessage(2, "robotVerifyAllTapes: tapeFSF(".$skipper.")");
        ($result, $response) = tapeFSF($skipper);
        logMessage(2, "robotVerifyAllTapes: tapeFSF(".$skipper.")");
        
        # Something is wrong with the file count on the tape, move to the next tape
        if ($result ne "ok") {
          logMessage(0, "robotVerifyAllTapes: tapeFSF(".$skipper.") failed: ".$response);
          verifyLog($curr_tape." ".$ifile." FSF(".$skipper.") failed");
          next TAPES;
        }

        $skipper = 1;

        # check ifile vs mt's tape identifier
        logMessage(2, "robotVerifyAllTapes: ".$cmd);
        $cmd =  "mt -f /dev/nst0 status | grep 'File number' | awk -F, '{print \$1}' | awk -F= '{print \$2}'";
         ($result, $response) = Dada::mySystem($cmd);
        logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "robotVerifyAllTapes: could not extract file number from mt status command");
          unindent();
          return ("fail", "could not extract file number from mt status command");
        }

        chomp $response;
        my $mt_file_no = $response;

        if ($mt_file_no != $ifile) {
          logMessage(0, "robotVerifyAllTapes: File number mismatch tape_file_no=".$mt_file_no.", ifile=".$ifile);
          verifyLog($curr_tape." ".$ifile." tape file_no ".$mt_file_no." did not match ifile ".$ifile);
          next FILES;
        }

        # ensure there is nothing in the localdir
        $cmd = "rm -rf ".$local_dir."/*";
        logMessage(2, "robotVerifyAllTapes: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);


        # extract the tar archive on tape
        $cmd = "tar -b 128 -xvf ".$dev; 
        logMessage(2, "robotVerifyAllTapes: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "robotVerifyAllTapes: untar failed: ".$response);
          verifyLog($curr_tape." ".$ifile." untar command failed");
          next FILES;
        }

        # determine what was on the archive
        my @tarred_files = split(/\n/, $response);
      
        # the first file should be the <UTC_START>/<BEAM>
        my $utc_beam = $tarred_files[0];

        # remove the preceeding ./ if possible
        if ($utc_beam =~ /^\.\//) {
          logMessage(2, "robotVerifyAllTapes: removing preceeding ./");
          $utc_beam = substr($utc_beam, 2);
        }
        
        # remove the trailing slash if possible
        if ($utc_beam =~ /\/$/) {
          logMessage(2, "robotVerifyAllTapes: removing trailing slash");
          chop $utc_beam;
        }

        # check it exists
        if (! -d $utc_beam) {
          logMessage(0, "robotVerifyAllTapes: untar failed as expected dir did not exist after tar command");
          verifyLog($curr_tape." ".$ifile." untar failed unexpected dir");
          next FILES;
        } 

        # it exists, check the size
        $cmd = "du -sLb ./".$utc_beam." | awk '{print \$1}'";
        logMessage(2, "robotVerifyAllTapes: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "robotVerifyAllTapes: du command failed: ".$response);
          unindent();
          return ("fail", "could not determine size of the untarred dir");
        }

        chomp $response;
        my $tape_size_gbytes = int($response) / (1024*1024*1024);

        # now compared to the size in the files.db
        $cmd = "grep '".$utc_beam."' ".$db_dir."/".FILES_DB;
        logMessage(2, "robotVerifyAllTapes: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);

        if ($result ne "ok") {
          logMessage(0, "robotVerifyAllTapes: grep command failed: ".$response);
          verifyLog($curr_tape." ".$ifile." ".$utc_beam." dir did not exist in files.db");
          next FILES;
        }

        my $grep_result = $response;
        chomp $grep_result;

        my ($obs, $tape, $date, $size, $fileno) = split(" ", $grep_result);
        logMessage(2, "robotVerifyAllTapes: files.db result: obs=".$obs.", tape=".$tape.", date=".$date.", size=".$size.", fileno=".$fileno);

        my $size_gbytes = $size;

        # If the size difference is greater than 100 MB
        if (abs($size_gbytes - $tape_size_gbytes) > 0.1) {
          logMessage(0, "robotVerifyAllTapes: size mismatch: tape=".$tape_size_gbytes.", files.db=".$size_gbytes);
          verifyLog($curr_tape." ".$ifile." ".$utc_beam." size mismatch (tape=".$tape_size_gbytes.", files.db=".$size_gbytes.")");
          next FILES;
        }

        my $md5sum_result = "";

        # Do a md5sum on the aux.tar file
        if (-f ($utc_beam."/aux.tar")) {
          $cmd = "md5sum ./".$utc_beam."/aux.tar | awk '{print \$1}'";
          logMessage(2, "robotVerifyAllTapes: ".$cmd);
          ($result, $response) = Dada::mySystem($cmd);
          logMessage(2, "robotVerifyAllTapes: ".$result." ".$response);

          if ($result ne "ok") {
            logMessage(0, "robotVerifyAllTapes: md5sum failed: ".$response);
            verifyLog($curr_tape." ".$ifile." ".$utc_beam." md5sum failed on ".$utc_beam."/aux.tar");
            next FILES;
          }

        }
      
        my $md5sum_result = $response; 
        chomp($md5sum_result);

        verifyLog($curr_tape." ".$ifile." ".$utc_beam." OK ".sprintf("%6.4f",$tape_size_gbytes)." ".sprintf("%6.4f",$size_gbytes)." ".$md5sum_result);

      }
    }
  }
}

sub verifyLog($) {

  (my $string) = @_;
  indent();
  
  my $cmd = "echo '".$string."' >> ".$db_dir."/verify.log";
  logMessage(2, "verifyLog: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "verifyLog: ".$result." ".$response);

  unindent();


}


##
## ROBOT FUNCTIONS
##
###############################################################################


###############################################################################
##
## MANUAL TAPE FUNCTIONS
##


#
# Contact the User interface and ask it to manually insert the requested
# tape. Requires a direct connection to the machine hosting the web interace
#
sub manualInsertTape($) {

  (my $tape) = @_;

  my $cmd = "mt -f ".$dev." offline";
  ($result, $response) = Dada::mySystem($cmd);
  if ($result ne "ok") {
    logMessage(0, "TarBeam: tape offline failed: ".$response);
  }

  indent();
  logMessage(2, "manualInsertTape()");
  logMessage(1, "manualInsertTape: asking for tape ".$tape);

  logMessage(2, "manualInsertTape: manualClearResponse()");
  ($result, $response) = manualClearResponse();

  if ($result ne "ok") {
    logMessage(0, "manualInsertTape: manualClearResponse() failed: ".$response);
    unindent();
    return ("fail", "could not clear response file in web interface");
  }

  my $string = "Insert Tape:::".$tape;
  logMessage(2, "manualInsertTape: setStatus(".$string.")");
  ($result, $response) = setStatus($string);
  logMessage(2, "manualInsertTape: setStatus() ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "manualInsertTape: setStatus() failed: ".$response);
    unindent();
    return ("fail", "could not set status on web interface");
  }

  my $have_response = 0;
  while ((!$have_response) && (!$quit_daemon)) {

    logMessage(2, "manualInsertTape: tapeIsLoaded()");
    ($result, $response) = tapeIsLoaded();
    logMessage(2, "manualInsertTape: tapeIsLoaded ".$result." ".$response);

    if ($result ne "ok") {
      logMessage(0, "getCurrentTape: tapeIsLoaded failed: ".$response);
      unindent();
      return ("fail", "could not determine if tape is loaded in drive");
    }

    if ($response ne 1) {
      logMessage(2, "manualInsertTape: sleeping 10 seconds for tape online");
      sleep(10);
    }
    else {
      $have_response=1;
    }
  } 

  $string = "Current Tape: ".$tape;
  logMessage(2, "manualInsertTape: setStatus(".$string.")");
  ($result, $response) = setStatus($string);
  logMessage(2, "manualInsertTape: setStatus() ".$result." ".$response);
  
  if ($result ne "ok") {
    logMessage(0, "manualInsertTape: setStatus() failed: ".$response);
    unindent();
    return ("fail", "could not set status on web interface");
  }

  unindent();
  return ("ok", $tape);

}


#
# get the .response file from the web interface
#
sub manualGetResponse() {

  indent();
  logMessage(2, "manualGetResponse()");

  my $dir = "/nfs/control/bpsr/";
  my $file = $type.".response";
  my $remote_cmd = "";
  my $cmd = "";
  my $result = "";
  my $response = "";

  # Delete the existing state file
  $remote_cmd = "cat ".$dir."/".$file;
  $cmd = $apsr_ssh_prefix.$remote_cmd.$apsr_ssh_suffix;

  # Wait for a response to appear from the user
  logMessage(2, "manualGetResponse: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "manualGetResponse: ".$result." ".$response);

  unindent();
  if ($result ne "ok") {
    return ("fail", "could not read file: ".$file);
  }

  return ($result, $response);

}

#
# delete the .response file from the web interface
#
sub manualClearResponse() {

  indent();
  logMessage(2, "manualClearResponse()"); 

  my $result = ""; 
  my $response = "";
  my $dir = "/nfs/control/bpsr/";
  my $file = $type.".response";
  my $remote_cmd = ""; 
  my $cmd = ""; 

  # Delete the existing state file
  $remote_cmd = "rm -f ".$dir."/".$file;
  $cmd = $apsr_ssh_prefix.$remote_cmd.$apsr_ssh_suffix;

  # Wait for a response to appear from the user
  logMessage(2, "manualClearResponse: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "manualClearResponse: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "manualClearResponse: could not delete the existing state file ".$file.": ".$response);
    unindent();
    return ("fail", "could not remove state file: ".$file);
  }

  unindent();
  return ("ok", "");
}


##
## MANUAL TAPE FUNCTIONS
##
###############################################################################

################################################################################
##
## TAPE functions
##

#
# seek forward the specified number of files
#
sub tapeFSF($) {

  (my $nfiles) = @_;

  indent();
  logMessage(2, "tapeFSF: (".$nfiles.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." fsf ".$nfiles;
  logMessage(2, "tapeFSF: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    logMessage(0, "tapeFSF: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "FSF failed: ".$response);
  }

  unindent();
  return ("ok", "");

}


#
# checks to see if the tape drive thinks it has a tape in it
#
sub tapeIsLoaded() {

  indent();
  logMessage(2, "tapeIsLoaded()");

  my $is_loaded = 0;

  my $cmd = "mt -f ".$dev." status | grep ' ONLINE ' > /dev/null";

  logMessage(2, "tapeIsLoaded: ".$cmd);
  `$cmd`;
  if ($? == 0) {
    $is_loaded = 1;
  } else {
    $is_loaded = 0;
  }

  logMessage(2, "tapeIsLoaded: returning ".$is_loaded);

  unindent();
  return ("ok", $is_loaded);

}


#
# Initialise the tape, writing the ID to the first file on the
# tape
#

sub tapeInit($) {

  (my $id) = @_;

  indent();
  logMessage(2, "tapeInit(".$id.")");

  my $result = "";
  my $response = "";

  logMessage(2, "tapeInit: tapeWriteID(".$id.")");
  ($result, $response) = tapeWriteID($id);
  logMessage(2, "tapeInit: tapeWriteID: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tapeInit: tapeWriteID() failed: ".$response);
    unindent();
    return ("fail", "could not write tape ID: ". $response);
  }
                                              
  logMessage(2, "tapeInit: tapeGetID()");
  ($result, $response) = tapeGetID();
  logMessage(2, "tapeInit: tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tapeInit: tapeGetID() failed: ".$response);
    unindent();
    return ("fail", "could not get tape ID from tape");
  }

  if ($id ne $response) {
    logMessage(0, "tapeInit: newly written ID did not match specified");
    unindent();
    return ("fail", "could not write tape ID to tape");
  }

  unindent();
  return ("ok", $id);

}

 
#
# Rewind, and read the first file from the tape
#
sub tapeGetID() {

  indent();
  logMessage(2, "tapeGetID()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  my $cmd = "mt -f ".$dev." rewind";
  logMessage(2, "tapeGetID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tapeGetID: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "mt rewind command failed: ".$response);;
  }

  $cmd = "tar -tf ".$dev;
  logMessage(2, "tapeGetID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {

    # if there is no ID on the tape this command will fail, 
    # but we can test the output message
    if ($response =~ m/tar: At beginning of tape, quitting now/) {

      logMessage(0, "tapeGetID: No ID on Tape");
      unindent();
      return ("ok", "");

    } else {

      logMessage(0, "tapeGetID: ".$cmd." failed: ".$response);
      unindent();
      return ("fail", "tar list command failed: ".$response);

    }
  }

  logMessage(2, "tapeGetID: ID = ".$response);
  my $tape_label = $response;

  logMessage(2, "tapeGetID: tapeFSF(1)");
  ($result, $response) = tapeFSF(1);
  if ($result ne "ok") {
    logMessage(0, "tapeGetID: tapeFSF failed: ".$response);
    unindent();
    return ("fail", "tapeFSF() failed to move forward 1 file");
  }

  ($result, my $filenum, my $blocknum) = getTapeStatus();
  if ($result ne "ok") { 
    logMessage(0, "tapeGetID: getTapeStatus() failed.");
    unindent();
    return ("fail", "getTapeStatus() failed");
  }

  while ($filenum ne 1 || $blocknum ne 0){
    # we are not at 0 block of file 1...
    logMessage(1, "tapeGetID: Tape out of position (f=$filenum, b=$blocknum), rewinding and skipping to start of data");
    $cmd="mt -f ".$dev." rewind; mt -f ".$dev." fsf 1";
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      logMessage(0, "tapeGetID: tape re-wind/skip failed: ".$response);
      unindent();
      return ("fail", "tape re-wind/skip failed: ".$response);
    }

    ($result, $filenum, $blocknum) = getTapeStatus();
    if ($result ne "ok") { 
      logMessage(0, "tapeGetID: getTapeStatus() failed.");
      unindent();
      return ("fail", "getTapeStatus() failed");
    }
  }
  # The tape MUST now be in the right place to start
  logMessage(2, "tapeGetID: ID = ".$tape_label);
  unindent();

  return ("ok", $tape_label);
}

#
# Rewind, and write the first file from the tape
#
sub tapeWriteID($) {

  (my $tape_id) = @_;

  indent();
  logMessage(2, "tapeWriteID()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  my $cmd = "mt -f ".$dev." rewind";
  logMessage(2, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeWriteID: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "tapeWriteID: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "mt rewind failed: ".$response);
  }

  # create an emprty file in the CWD to use
  $cmd = "touch ".$tape_id;
  logMessage(2, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeWriteID: ".$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "tapeWriteID: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "could not create tmp file in cwd: ".$response);
  }

  # write the empty file to tape
  $cmd = "tar -cf ".$dev." ".$tape_id;
  logMessage(2, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeWriteID: ".$result." ".$response);

  unlink($tape_id);

  if ($result ne "ok") {
    logMessage(0, "tapeWriteID: ".$cmd." failed: ".$response);
    unindent();
    return ("fail", "could not write ID to tape: ".$response);
  } 

  # write the EOF marker
  $cmd = "mt -f ".$dev." weof";
  logMessage(2, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "tapeWriteID: ".$result." ".$response);

  # Initialisze the tapes DB record also
  logMessage(2, "tapeWriteID: updatesTapesDB(".$tape_id.", ".TAPE_SIZE.", 0, ".TAPE_SIZE.", 1, 0)");
  ($result, $response) = updateTapesDB($tape_id, TAPE_SIZE, 0, TAPE_SIZE, 1, 0);
  logMessage(2, "tapeWriteID: updatesTapesDB(): ".$result.", ".$response);

  unindent();
  return ("ok", $response);

}

##
## TAPE functions
##
################################################################################


#
# Polls for the "quitdaemons" file in the control dir
#
sub daemonControlThread() {

  logMessage(2, "daemon_control: thread starting");

  my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
  my $quitfile = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;

  # poll for the existence of the control file
  while ((!-f $quitfile) && (!$quit_daemon)) {
    logMessage(3, "daemon_control: Polling for ".$quitfile);
    sleep(1);
  }

  # signal threads to exit
  $quit_daemon = 1;

  logMessage(2, "daemon_control: Unlinking PID file ".$pidfile);
  unlink($pidfile);

  logMessage(2, "daemon_control: exiting");

}

#
# Logs a message to the Nexus
#
sub logMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    my $time = Dada::getCurrentDadaTime();
    print "[".$time."] ".$indent.$message."\n";
    # system("echo '[".$time."] ".$indent.$message."' >> ".$db_dir."/bpsr_tape_archiver.log");
  }
}
sub indent() {
  $indent .= "  ";
}

sub unindent() {
  $indent = substr($indent,0,-2);
}

#
# Handle INT AND TERM signals
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  my $dir;

  # force an unlink of any READING control files on exit
  #foreach $dir (@dirs) {
  #  if (-f $dir."/READING") {
  #    unlink ($dir."/READING");
  #  }
  #}
  sleep(3);
  print STDERR basename($0)." : Exiting: ".Dada::getCurrentDadaTime(0)."\n";

}

sub usage() {
  print STDERR "Usage:\n";
  print STDERR basename($0)." [swin|parkes]\n";
}

sub exit_script($) {

  my $val = shift;
  print STDERR "exit_script(".$val.")\n";
  $quit_daemon = 1;
  sleep(3);
  exit($val);

}


sub floatPad($$$) {

  my ($val, $n, $m) = @_;

  my $str = "";

  if (($val >= 10.00000) && ($val < 100.00000)) {
    $str = " ".sprintf("%".($n-1).".".$m."f", $val);
  } elsif ($val < 10.0000) {
    $str = "  ".sprintf("%".($n-2).".".$m."f", $val);
  } else {
    $str = sprintf("%".$n.".".$m."f", $val)
  }

  return $str;
}


#
# Estimate the archive size based on file size and number of files
#
sub tarSizeEst($$) {

  my ($nfiles, $files_size) = @_;

  # 512 bytes for header and up to 512 bytes padding for data
  my $tar_overhead_files = (1024 * $nfiles);

  # all archives are a multiple of 10240 bytes, add for max limit
  my $tar_overhead_archive = 10240;           

  # upper limit on archive size in bytes
  my $size_est = $files_size + $tar_overhead_files + $tar_overhead_archive;

  # Add 1 MB for good measure
  $size_est += (1024*1024);

  return $size_est;

}

#
# sets status on web interface
#

sub setStatus($) {

  (my $string) = @_;
  indent();
  logMessage(2, "setStatus(".$string.")");

  my $dir = "/nfs/control/bpsr";
  my $file = $type.".state";

  my $result = "";
  my $response = "";
  my $remote_cmd = "";
  my $cmd = "";

  # Delete the existing state file
  $remote_cmd = "rm -f ".$dir."/".$file;
  $cmd = $apsr_ssh_prefix.$remote_cmd.$apsr_ssh_suffix;

  logMessage(2, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "setStatus: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "setStatus: could not delete the existing state file ".$file.": ".$response);
    unindent();
    return ("fail", "could not remove state file: ".$file);
  }

  # Write the new file
  $remote_cmd = "echo '".$string."' > ".$dir."/".$file;
  $cmd = $apsr_ssh_prefix.$remote_cmd.$apsr_ssh_suffix;

  logMessage(2, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "setStatus: ".$result." ".$response);

  unindent();
  return ("ok", "");

}

#
# Launches NC on the remote end
#
sub nc_thread($$$$$$) {

  my ($user, $host, $dir, $obs, $beam, $port) = @_;

  # Firstly, try to ensure that no NC is running on the port we want to use

  my $result = "";
  my $response = "";

  my $cmd = "ssh ".$ssh_opts." -l ".$user." ".$host." \"ls ".$dir." > /dev/null\"";
  logMessage(2, "nc_thread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "nc_thread: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "nc_thread: could not automount the /nfs/cluster/shrek??? raid disk");
  }

  $cmd = "ssh ".$ssh_opts." -l ".$user." ".$host." \"cd ".$dir."; ".
         "tar -b 128 -c ".$obs."/".$beam." | nc -l ".$port."\"";

  logMessage(2, "nc_thread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "nc_thread: ".$result." ".$response);

  if ($result ne "ok") {
    logMessage(0, "nc_thread: cmd \"".$cmd."\" failed: ".$response);
    return ("fail");
  }
  return ("ok");
}

sub getTapeStatus() {
  my $cmd="";
  my $filenum;
  my $blocknum;

  indent();
  logMessage(2, "getTapeStatus()");

  # Parkes robot has a different print out than the swinburne one
  if ($robot eq 0) {
    $cmd="mt -f ".$dev." status | grep 'file number' | awk '{print \$4}'";
  } else {
    $cmd="mt -f ".$dev." status | grep 'File number' | awk -F, '{print \$1}' | awk -F= '{print \$2}'";
  }
  logMessage(3, "getTapeStatus: cmd= $cmd");

  my ($result,$response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    logMessage(0, "getTapeStatus: Failed $response");
    $filenum=-1;
    $blocknum=-1;
  } else {
    $filenum = $response;
    if ($robot eq 0) {
      $cmd="mt -f ".$dev." status | grep 'block number' | awk '{print \$4}'";
    } else {
      $cmd="mt -f ".$dev." status | grep 'block number' | awk -F, '{print \$2}' | awk -F= '{print \$2}'";
    }
    my ($result,$response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      logMessage(0, "getTapeStatus: Failed $response");
      $filenum=-1;
      $blocknum=-1;
    } else {
      $blocknum=$response;
    }
  }
  logMessage(2, "getTapeStatus: ".$result." ".$filenum." ".$blocknum);
  unindent();
  return ($result,$filenum,$blocknum);
}

#
# Mark the obsevation/beam as on.tape.type, and also remotely in /nfs/archives
#
sub markSentToTape($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  indent();
  logMessage(2, "markSentToTape(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $cmd = "";
  my $remote_cmd = "";
  my $result = "";
  my $respnose = "";
  my $gw_cmd = "";
  my $to_touch = "";

  if ($beam ne "") {
    $to_touch = $obs."/".$beam."/on.tape.".$type;
  } else {
    $to_touch = $obs."/on.tape.".$type;
  }

  $cmd = "ssh -x ".$ssh_opts." -l ".$user." ".$host." \"touch ".$dir."/".$to_touch."\"";
  logMessage(2, "markSentToTape:" .$cmd);
  my ($result,$response) = Dada::mySystem($cmd);
  logMessage(2, "markSentToTape:" .$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "markSentToTape: could not touch ".$to_touch);
  }

  # if the beam is set, touch the beams on.tape.type
  $remote_cmd = "touch ".$cfg{"SERVER_ARCHIVE_DIR"}."/".$to_touch;
  $cmd = $apsr_ssh_prefix.$remote_cmd.$apsr_ssh_suffix;

  logMessage(2, "markSentToTape: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMessage(2, "markSentToTape: ".$result." ".$response);
  if ($result ne "ok") {
    logMessage(0, "markSentToTape: could not touch remote on.tape.".$type.": ".$response);
  }
  
  unindent();
  return ($result, $response);

}


#
#
#
sub localSshCommand($$$) {

  my ($user, $host, $command) = @_;
  indent();
  logMessage(2, "localSshCommand(".$user.", ".$host.", ".$command);

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "ssh -x ".$ssh_opts." -l ".$user." ".$host." \"".$command."\"";

  logMessage(2, "localSshCommand:" .$cmd);
  ($result,$response) = Dada::mySystem($cmd);
  logMessage(2, "localSshCommand:" .$result." ".$response);

  unindent();
  return ($result, $response);

}

sub getToTape($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

} 
