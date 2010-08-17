package Dada::server_tape_archiver;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use threads;
use threads::shared;
use Dada;
use Bpsr;
use Dada::tapes;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $daemon_name $type $robot $pid %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our $robot;
our $type;
our $pid;
our $required_host;
our $ssh_prefix;
our $ssh_suffix;
our $apsr_user;
our $apsr_host;
our $gw_user;
our $gw_host;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $warn;
our $error;
our $dev;
our $ssh_opts;
our $current_tape;
our $db_dir;
our $db_user;
our $db_host;
our $use_bk;
our $bkid;
our $tapes_db;
our $files_db;
our $bookkeepr;

#
# initialize package globals
#
$dl = 1;
$daemon_name = 0;
$robot = 0;
$pid = "";
$type = "";
$ssh_prefix = "";
$ssh_suffix = "";
$apsr_user = "";
$apsr_host = "";
$gw_user = "";
$gw_host = "";

#
# initialize other variables
#
$quit_daemon = 0;
$warn = "";
$error = "";
$dev = 0;
$ssh_opts = "-o BatchMode=yes";
$current_tape = "";
$db_dir = "";
$db_user = "";
$db_host = "";
$use_bk = 0;
$bkid = "";
$tapes_db = "";
$files_db = "";
$bookkeepr = "";

use constant TAPE_SIZE    => "750.00";


###############################################################################
#
# Package Methods
#

sub main() {

  # location of DB files
  ($db_user, $db_host, $db_dir) = split(/:/, $cfg{uc($type)."_DB_DIR"});

  my $pid_file    = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file   = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $log_file    = $db_dir."/".$daemon_name.".log";
  my $result      = "";
  my $response    = "";

  $tapes_db = "tapes.".$pid.".db";
  $files_db = "files.".$pid.".db";

  $dev = $cfg{uc($type)."_S4_DEVICE"};
  $bookkeepr = $cfg{uc($type)."_BOOKKEEPR"};

  # Initialise tapes module variables
  $Dada::tapes::dl = $dl;
  $Dada::tapes::dev = $dev;
  $Dada::tapes::robot = $robot;
  Dada::logMsg(2, $dl, "main: setDada::tapes dl=".$Dada::tapes::dl." dev=".$Dada::tapes::dev." robot=".$Dada::tapes::robot);

  

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;

  #
  # Local Varaibles
  #
  my $control_thread = 0;
  my $completed_thread = 0;
  my $i = 0;
  my $j = 0;
  my @hosts = ();
  my @users = ();
  my @paths = ();
  my @direxts = ();
  my $user = "";
  my $host = "";
  my $path = "";
  my $direxts = "";
  my $path_pid = "";

  # setup the disks
  for ($i=0; $i<$cfg{"NUM_".uc($type)."_DIRS"}; $i++) {
    ($user, $host, $path) = split(/:/,$cfg{uc($type)."_DIR_".$i},3);
    $hosts[$j] = $host;
    $users[$j] = $user;
    $paths[$j] = $path;
    $direxts[$j] = "staging_area";
    $j++;
  }

  # add the pulsars' directory also
  for ($i=0; $i<$cfg{"NUM_".uc($type)."_DIRS"}; $i++) {
    ($user, $host, $path) = split(/:/,$cfg{uc($type)."_DIR_".$i},3);
    $hosts[$j] = $host;
    $users[$j] = $user;
    $paths[$j] = $path;
    $direxts[$j] = "pulsars";
    $j++;
  }

  $j = 0;

  Dada::daemonize($log_file, $pid_file);
  Dada::logMsg(0, $dl, "STARTING SCRIPT");
  setStatus("Scripting starting");

  # Start the daemon control thread
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # Force a re-read of the current tape. This rewinds the tape
  Dada::logMsg(1, $dl, "main: checking current tape");
  ($result, $response) = getCurrentTape();

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "main: getCurrentTape() failed: ".$response);
    ($result, $response) = newTape();
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "main: getNewTape() failed: ".$response);
      exit_script(1);
    }
  }
  $current_tape = $response;
  Dada::logMsg(1, $dl, "main: current tape = ".$current_tape);

  # get the expected tape to be loaded
  Dada::logMsg(2, $dl, "main: getExpectedTape()");
  ($result, $response) = getExpectedTape();
  Dada::logMsg(2, $dl, "main: getExpectedTape() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "main: getExpectedTape() failed: ".$response);
    exit_script(1);
  }

  my $expected_tape = $response;
  Dada::logMsg(1, $dl, "main: expected tape = ".$expected_tape);

  if ($current_tape ne $expected_tape) {
    Dada::logMsg(2, $dl, "main: loadTape(".$expected_tape.")");
    ($result, $response) = loadTape($expected_tape);
    Dada::logMsg(2, $dl, "main: loadTape() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getCurrentTape: loadTape() failed: ".$response);
      exit_script(1);
    }
    $current_tape = $expected_tape;
  }

  setStatus("Current tape: ".$current_tape);

  # Get the tape information from the tape database
  ($result, $response) = getTapeInfo($current_tape);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "main : getTapeInfo() failed: ".$response);
    exit_script(1);
  }

  my ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

  # If the current tape is full, we need to switch to the next empty one
  if (int($full) == 1) {

    Dada::logMsg(1, $dl, "tape ".$id." marked full, selecting new tape");

    ($result, $response) = newTape();

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "Could not load a new tape: ".$response);
      exit_script(1);
    }

    $current_tape = $response;
    Dada::logMsg(1, $dl, "New tape selected: ".$current_tape);

  
  } else {

    # Move forward to be ready to write on the next available file number
    # the getTapeID function will have placed us on fsf 1, as the
    # binary label is on fsf 0, so we *do* need to increment by n-1
    Dada::logMsg(2, $dl, "main: tapeFSF(".($nfiles-1).")");
    ($result, $response) = Dada::tapes::tapeFSF(($nfiles-1));
    Dada::logMsg(2, $dl, "main: tapeFSF() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "main : tapeFSF failed: ".$response);
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
  my $disks_tried = 0;
  my $give_up = 0;
  my $counter = 0;

  while (!$quit_daemon) {

    # look for a file sequentially in each of the @dirs
    $host = $hosts[$i];
    $user = $users[$i];
    $path = $paths[$i];
    $path_pid = $path."/".$pid."/".$direxts[$i];

    # Look for files in the @dirs
    Dada::logMsg(2, $dl, "main: getBeamToTar(".$user.", ".$host.", ".$path_pid.")");
    ($obs, $beam) = getBeamToTar($user, $host, $path_pid);
    Dada::logMsg(2, $dl, "main: getBeamToTar() ".$obs."/".$beam);

    # If we have one, write to tape
    if (($obs ne "none") && ($beam ne "none")) {

      $disks_tried = 0;

      # lock this dir for READING
      $cmd = "touch ".$path."/../READING";
      Dada::logMsg(2, $dl, "main: localSshCommand(".$user.", ".$host.", ".$cmd.")");
      ($result, $response) = localSshCommand($user, $host, $cmd);
      Dada::logMsg(2, $dl, "main: localSshCommand() ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "main: could not touch READING flag in ".$path.": ".$response);
        exit_script(1);
      }

      Dada::logMsg(2, $dl, "main: tarBeam(".$user.", ".$host.", ".$path_pid.", ".$obs.", ".$beam.")");
      ($result, $response) = tarBeam($user, $host, $path_pid, $obs, $beam);
      Dada::logMsg(2, $dl, "main: tarBeam() ".$result." ".$response);
      $waiting = 0;

      if ($result eq "ok") {

        # if the completion thread has been previously launched, join it
        if ($completed_thread ne 0) {
          $completed_thread->join();
	        $completed_thread = 0;
        }

        # launch the completion thread
        $completed_thread = threads->new(\&completed_thread, $user, $host, $path_pid, $obs, $beam);

      } else {
        setStatus("Error: ".$response);
        Dada::logMsg(0, $dl, "main: tarBeam() failed: ".$response);
        $give_up = 1;
      }

      $cmd = "rm -f ".$path."/../READING";
      Dada::logMsg(2, $dl, "main: localSshCommand(".$user.", ".$host.", ".$cmd.")");
      ($result, $response) = localSshCommand($user, $host, $cmd);
      Dada::logMsg(2, $dl, "main: localSshCommand() ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "main: could not unlink READING flag in ".$path.": ".$response);
        $give_up = 1;
      }

      if ($give_up) {
        exit_script(1);
      }

      $j++;

    } else {

      $disks_tried++;
      Dada::logMsg(2, $dl, "main: source [".$i."] had no beams ".$users[$i]."@".$hosts[$i].":".$paths[$i]."/".$pid."/".$direxts[$i]);
      # If we have cycled through all the disks and no files exist 

      if ($disks_tried > ($#hosts)) {

        Dada::logMsg(2, $dl, "main: tried [".$disks_tried." of ".($#hosts+1)."] disks, doing a long sleep");
        $disks_tried = 0;

        # Just log that we are in the main loop waiting for data
        if (!$waiting) {
          Dada::logMsg(1, $dl, "main: waiting for obs to transfer");
          setStatus("Waiting for new");
          $waiting = 1;
        }

        $counter = 60;
        while (!$quit_daemon && ($counter>0)) {
          sleep 1;
          $counter--;
        }

      } else {

        # increment to the next disk
        sleep(1);

      } 
      $i++;

    }

    # only move onto the next holding area after the 13th
    # beam or handling 13 beams
    if (($beam eq "13") || ($j == 13)) {
      $j=0;
      $i++;
    }

    # reset the dirs counter
    if ($i >= ($#paths+1)) {
      $i = 0;
    }

  } # main loop

  # rejoin threads
  $control_thread->join();

  if ($completed_thread ne 0) {
    $completed_thread->join();
  }

  setStatus("Script stopped");

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");

  return 0;
}


###############################################################################
#
# Functions
#


#
# try and find an observation to tar
#
sub getBeamToTar($$$) {

  my ($user, $host, $dir) = @_;
  Dada::logMsg(2, $dl, "getBeamToTar(".$user.", ".$host.", ".$dir.")");

  my $obs = "none";
  my $beam = "none";
  my $subdir = "";
  my @subdirs = ();
  my $result = "";
  my $response = "";
  my $cmd = "";

  $cmd = "ls -1 ".$dir."/../../WRITING";
  Dada::logMsg(2, $dl, "getBeamToTar: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "getBeamToTar: localSshCommand() ".$result." ".$response);

  # If the file existed
  if ($result eq "ok") {
    Dada::logMsg(2, $dl, "getBeamToTar: ignoring dir ".$dir." as this is being written to");

  } else {

    $cmd = "cd ".$dir."; find . -mindepth 3 -maxdepth 3 -name 'xfer.complete' ".
           "-printf '\%h\\n' | sort -r | tail -n 1";
    Dada::logMsg(2, $dl, "getBeamToTar: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    Dada::logMsg(2, $dl, "getBeamToTar: localSshCommand() ".$result." ".$response);

    # if the ssh command failed
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getBeamToTar: ssh cmd [".$user."@".$host."] failed: ".$cmd);
      Dada::logMsg(0, $dl, "getBeamToTar: ssh response ".$response);
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
        Dada::logMsg(2, $dl, "getBeamToTar: found ".$obs."/".$beam);
    
      # find failed
      } else {
        Dada::logMsg(2, $dl, "getBeamToTar: could not find any xfer.complete's");
      } 

    }
  }
  Dada::logMsg(3, $dl, "getBeamToTar: returning ".$obs.", ".$beam);

  return ($obs, $beam);
}


#
# tars the beam to the tape drive
#
sub tarBeam($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  Dada::logMsg(2, $dl, "tarBeam: (".$user." , ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  Dada::logMsg(1, $dl, "tarBeam: archiving ".$obs.", beam ".$beam);

  my $cmd = "";
  my $result = "";
  my $response = "";

  # Check if this beam has already been archived
  Dada::logMsg(2, $dl, "tarBeam: checkIfArchived(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  ($result, $response) = checkIfArchived($user, $host, $dir, $obs, $beam);
  Dada::logMsg(2, $dl, "tarBeam: checkIfArchived() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: checkIfArchived failed: ".$response);
    return ("fail", "checkIfArchived failed: ".$response);
  } 

  # If this beam has been archived, skip it 
  if ($response eq "archived") {
    Dada::logMsg(1, $dl, "Skipping archival of ".$obs."/".$beam.", already archived");
    return("ok", "beam already archived");
  }

  setStatus("Archiving ".$obs." ".$beam);

  # Check the tape is the expected one
  my $expected_tape = getExpectedTape(); 

  if ($expected_tape ne $current_tape) {
    
    # try to load it?
    Dada::logMsg(2, $dl, "tarBeam: expected tape mismatch \"".$expected_tape."\" != \"".$current_tape."\"");

    # try to get the tape loaded (robot or manual)
    ($result, $response) = loadTape($expected_tape);

    if ($result ne "ok") {
      return ("fail", "Could not load tape: ".$expected_tape);
    }

    $current_tape = $expected_tape;
  }

  my $tape = $expected_tape;

  # Find the combined file size in bytes
  my $rval = 0;
  $cmd = "du -sLb ".$dir."/".$obs."/".$beam;
  my $pipe = "awk '{print \$1}'";
  Dada::logMsg(2, $dl, "tarBeam: remoteSshCommand(".$user.", ".$host.", ".$cmd.", \"\", ".$pipe.")");
  ($result, $rval, $response) = Dada::remoteSshCommand($user, $host, $cmd, "", $pipe);
  Dada::logMsg(2, $dl, "tarBeam: remoteSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: ".$cmd. "failed: ".$response);
    return ("fail", "Could not determine archive size for ".$obs."/".$beam);
  } 

  my $junk = "";
  my $size_bytes = "";
  ($size_bytes, $junk) = split(/ /,$response,2);

  # get the upper limit on the archive size
  Dada::logMsg(2, $dl, "tarBeam: tarSizeEst(4, ".$size_bytes.")");
  my $size_est_bytes = tarSizeEst(4, $size_bytes);
  my $size_est_gbytes = $size_est_bytes / (1024*1024*1024);
  Dada::logMsg(2, $dl, "tarBeam: tarSizeEst() ".$size_est_gbytes." GB");

  # check if this beam will fit on the tape
  ($result, $response) = getTapeInfo($tape);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: getTapeInfo() failed: ".$response);
    return ("fail", "could not determine tape information from database");
  } 

  my ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

  Dada::logMsg(2, $dl, "tarBeam: ".$free." GB left on tape");
  Dada::logMsg(2, $dl, "tarBeam: size of this beam is estimated at ".$size_est_gbytes." GB");

  if ($free < $size_est_gbytes) {

    Dada::logMsg(0, $dl, "tarBeam: tape ".$tape." full. (".$free." < ".$size_est_gbytes.")");

    # Mark the current tape as full and load a new tape;
    Dada::logMsg(2, $dl, "tarBeam: updateTapesDB(".$id.", ".$size.", ".$used.", ".$free.", ".$nfiles);
    ($result, $response) = updateTapesDB($id, $size, $used, $free, $nfiles, 1);
    Dada::logMsg(2, $dl, "tarBeam: updateTapesDB() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "tarBeam: updateTapesDB() failed: ".$response);
      return ("fail", "Could not mark tape full");
    }

    # Get the next expected tape
    Dada::logMsg(1, $dl, "tarBeam: getExpectedTape()");
    my $new_tape = getExpectedTape();
    Dada::logMsg(1, $dl, "tarBeam: getExpectedTape(): ".$new_tape);

    Dada::logMsg(1, $dl, "tarBeam: loadTape(".$new_tape.")");
    ($result, $response) = loadTape($new_tape);
    Dada::logMsg(1, $dl, "tarBeam: loadTape() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "tarBeam: newTape() failed: ".$response);
      return ("fail", "New tape was not loaded");
    }

    $tape = $new_tape; 
    $current_tape = $tape;

    # check if this beam will fit on the tape
    Dada::logMsg(2, $dl, "tarBeam: getTapeInfo(".$tape.")");
    ($result, $response) = getTapeInfo($tape);
    Dada::logMsg(2, $dl, "tarBeam: getTapeInfo() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "tarBeam: getTapeInfo() failed: ".$response);
      return ("fail", "Could not determine tape information from database");
    }

    # Setup the ID information for the new tape    
    ($id, $size, $used, $free, $nfiles, $full) = split(/:/,$response);

    # If for some reason, this tape has files on it, forward seek
    if ($nfiles > 1) {

      Dada::logMsg(2, $dl, "tarBeam: tapeFSF(".($nfiles-1).")");
      ($result, $response) = tapeFSF(($nfiles-1));
      Dada::logMsg(2, $dl, "tarBeam: tapeFSF() ".$result." ".$response);
                                                                                                                                                                           
      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "tarBeam: tapeFSF failed: ".$response);
        return ("fail", "Could not FSF seek to correct place on tape");
      }
    }
  }

  my $filenum = -1;
  my $blocknum = -1;
  ($result, $filenum, $blocknum) = Dada::tapes::getTapeStatus();
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "TarBeam: getTapeStatus() failed.");
    return ("fail", "Could not get tape status");
  }

  my $ntries = 3;
  while (($filenum ne $nfiles) || ($blocknum ne 0)) {

    # we are not at 0 block of the next file!
    Dada::logMsg(0, $dl, "tarBeam: WARNING: Tape out of position! (f=".$filenum.
                  ", b=".$blocknum.") Attempt to get to right place.");

    $cmd = "mt -f ".$dev." rewind; mt -f ".$dev." fsf $nfiles";
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "TarBeam: tape re-wind/skip failed: ".$response);
      return ("fail", "tape re-wind/skip failed: ".$response);
    }

    ($result, $filenum, $blocknum) = Dada::tapes::getTapeStatus();
    if ($result ne "ok") { 
      Dada::logMsg(0, $dl, "TarBeam: getTapeStatus() failed.");
      return ("fail", "getTapeStatus() failed");
    }

    if ($ntries < 1) {
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
    Dada::logMsg(2, $dl, "tarBeam: testing nc on ".$host.":".$port);
    if ($robot eq 0) {
      $cmd = "nc -z ".$host." ".$port." > /dev/null";
    } else {
      $cmd = "nc -zd ".$host." ".$port." > /dev/null";
    }
    ($result, $response) = Dada::mySystem($cmd);

    if ($result eq "ok") {
      # This is an error condition!

      Dada::logMsg(0, $dl, "tarBeam: something running on the NC port ".$port);

      if ($tries < 5) {
        Dada::logMsg(0, $dl, "tarBeam: trying to increment port number");
        $port += 1;
      } else {
        Dada::logMsg(0, $dl, "tarBeam: trying again, now that we have tested once");
      }
      $tries--;
      sleep(1);

    # the command failed, meaning there is nothing on that port
    } else {
      Dada::logMsg(2, $dl, "tarBeam: nc will be available on ".$host.":".$port);
      $nc_ready = 1;

    }
  }

  Dada::logMsg(2, $dl, "tarBeam: nc_thread(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$port.")");
  my $remote_nc_thread = threads->new(\&nc_thread, $user, $host, $dir, $obs, $beam, $port);

  # Allow some time for this thread to start-up, ssh and launch tar + nc
  sleep(5);

  my $localhost = Dada::getHostMachineName();

  $tries=10;
  while ($tries > 0) {

    # For historical reasons, HRE(robot==0) tapes are written with a slight 
    # different command to HRA (robot==1) tapes
    #
    if ($robot eq 0) {
      $cmd = "nc -w 10 ".$host." ".$port." | dd of=".$dev." bs=64K";
    } else {
      $cmd = "nc -d ".$host." ".$port." | dd of=".$dev." obs=64k";
    }
      
    Dada::logMsg(2, $dl, "tarBeam: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);

    # Fatal errors, give up straight away
    if ($response =~ m/Input\/output error/) {
      Dada::logMsg(0, $dl, "tarBeam: fatal tape error: ".$response);
      $tries = -1;

    # Non fatal errors
    } 
    elsif (($result ne "ok") || ($response =~ m/refused/) || ($response =~ m/^0\+0 records in/)) {
      Dada::logMsg(2, $dl, "tarBeam: ".$result." ".$response);
      Dada::logMsg(0, $dl, "tarBeam: failed to write archive to tape: ".$response);
      $tries--;
      $result = "fail";
      $response = "failed attempt at writing archive";
      sleep(2);

    } else {
      # Just the last line of the DD command is relvant:
      my @temp = split(/\n/, $response);
      Dada::logMsg(1, $dl, "tarBeam: ".$result." ".$temp[2]);
      my @vals = split(/ /, $temp[2]);
      my $bytes_written = int($vals[0]);
      my $bytes_gb = $bytes_written / (1024*1024*1024);

      # If we didn't write anything, its due to the nc client connecting 
      # before the nc server was ready to provide the data, simply try to reconnect
      if ($bytes_gb == 0) {
        Dada::logMsg(0, $dl, "tarBeam: nc server not ready, sleeping 2 seconds");
        $tries--;
        sleep(2);

      # If we did write something, but it didn't match bail!
      } elsif ( ($size_est_gbytes - $bytes_gb) > 0.01) {
        $result = "fail";
        $response = "not enough data received by nc: ".$size_est_gbytes.
                    " - ".$bytes_gb." = ".($size_est_gbytes - $bytes_gb);
        Dada::logMsg(0, $dl, "tarBeam: ".$result." ".$response);
        $tries = -1;

      } else {

        Dada::logMsg(2, $dl, "tarBeam: est_size ".sprintf("%7.4f GB", $size_est_gbytes).
                      ", size = ".sprintf("%7.4f GB", $bytes_gb));
        $tries = 0;
      }
    }
  }

  if ($result ne "ok") {

    Dada::logMsg(0, $dl, "tarBeam: failed to write archive to tape: ".$response);
    Dada::logMsg(0, $dl, "tarBeam: attempting to clear the current nc server command");

    if ($robot eq 0) {
      $cmd = "nc -z ".$host." ".$port." > /dev/null";
    } else {
      $cmd = "nc -zd ".$host." ".$port." > /dev/null";
    }
    Dada::logMsg(0, $dl, "tarBeam: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(0, $dl, "tarBeam: ".$result." ".$response);

    $remote_nc_thread->detach();
    return ("fail","Archiving failed");
  }
  
  Dada::logMsg(2, $dl, "tarBeam: joining nc_thread()");
  $result = $remote_nc_thread->join();
  Dada::logMsg(2, $dl, "tarBeam: nc_thread() ".$result);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: remote tar/nc thread failed");
    return ("fail","Archiving failed");
  }

  # Now check that the File number has been incremented one. Sometimes the
  # file number is not incremented, which usually means an error...
  ($result, $filenum, $blocknum) = Dada::tapes::getTapeStatus();
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: getTapeStatus() failed.");
    return ("fail", "getTapeStatus() failed");
  }
  
  if (($blocknum ne 0) || ($filenum ne ($nfiles+1))) {
    Dada::logMsg(0, $dl, "tarBeam: positioning error after write: filenum=".$filenum.", blocknum=".$blocknum);
    return ("fail", "write failed to complete EOF correctly");
  }

  # remove the xfer.complete in the beam dir if it exists
  $cmd = "ls -1 ".$dir."/".$obs."/".$beam."/xfer.complete";
  Dada::logMsg(2, $dl, "tarBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "tarBeam: localSshCommand() ".$result." ".$response);

  if ($result eq "ok") {
    $cmd = "rm -f ".$dir."/".$obs."/".$beam."/xfer.complete";
    Dada::logMsg(2, $dl, "tarBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    Dada::logMsg(2, $dl, "tarBeam: localSshCommand() ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsg(2, $dl, "tarBeam: couldnt unlink ".$obs."/".$beam."/xfer.complete");
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
    Dada::logMsg(2, $dl, "tarBeam: updateBookKeepr(".$obs."/".$beam."/".$obs."/.psrxml");
    ($result, $response) = updateBookKeepr($obs."/".$beam."/".$obs."/.psrxml",$bookkeepr, $id, ($nfiles-1));
    Dada::logMsg(2, $dl, "tarBeam: updateBookKeepr(): ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "tarBeam: updateBookKeepr() failed: ".$response);
      return("fail", "error ocurred when updating BookKeepr: ".$response);
    }
  }

  Dada::logMsg(2, $dl, "tarBeam: updatesTapesDB($id, $size, $used, $free, $nfiles, $full)");
  ($result, $response) = updateTapesDB($id, $size, $used, $free, $nfiles, $full);
  Dada::logMsg(2, $dl, "tarBeam: updatesTapesDB(): ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: updateTapesDB() failed: ".$response);
    return("fail", "error ocurred when updating tapes DB: ".$response);
  }

  Dada::logMsg(2, $dl, "tarBeam: updatesFilesDB(".$obs."/".$beam.", ".$id.", ".$size_est_gbytes.", ".($nfiles-1).")");
  ($result, $response) = updateFilesDB($obs."/".$beam, $id, $size_est_gbytes, ($nfiles-1));
  Dada::logMsg(2, $dl, "tarBeam: updatesFilesDB(): ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: updateFilesDB() failed: ".$response);
    return("fail", "error ocurred when updating filesDB: ".$response);
  }

  Dada::logMsg(2, $dl, "tarBeam: markSentToTape(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");
  ($result, $response) = markSentToTape($user, $host, $dir, $obs, $beam);
  Dada::logMsg(2, $dl, "tarBeam: markSentToTape(): ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tarBeam: markSentToTape failed: ".$response);
  }

  return ("ok",""); 

}


#
# move a completed obs on the remote dir to the on_tape directory
#
sub moveCompletedBeam($$$$$$) {

  my ($user, $host, $dir, $obs, $beam, $dest) = @_;
  Dada::logMsg(2, $dl, "moveCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$dest.")");

  my $result = "";
  my $response = "";
  my $cmd = "";

  # ensure the remote directory is created
  $cmd = "mkdir -p ".$dir."/../".$dest."/".$obs;
  Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

   # move the beam
  $cmd = "cd ".$dir."; mv ".$obs."/".$beam." ../".$dest."/".$obs."/";
  Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "moveCompletedBeam: failed to move ".$obs."/".$beam." to ".$dest." dir: ".$response);
  } else {

    # if there are no other beams in the observation directory, then we can delete it
    # old, then we can remove it also

    $cmd = "find ".$dir."/".$obs."/ -mindepth 1 -type d | wc -l";
    Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

    if (($result eq "ok") && ($response eq "0")) {

      # delete the remote directory
      $cmd = "rmdir ".$dir."/".$obs;
      Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
      ($result, $response) = localSshCommand($user, $host, $cmd);
      Dada::logMsg(2, $dl, "moveCompletedBeam: localSshCommand() ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "moveCompletedBeam: could not delete ".$user."@".$host.":".$dir."/".$obs.": ".$response);
      }
    }
  }

  return ($result, $response);

}

#
# Delete the beam from the specified location
#
sub deleteCompletedBeam($$$$$$) {

  my ($user, $host, $dir, $obs, $beam, $dest) = @_;
  Dada::logMsg(2, $dl, "deleteCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$dest.")");

  my $result = "";
  my $response = "";
  my $cmd = "";

  # ensure the remote directory exists
  $cmd = "ls -1d ".$dir."/../".$dest."/".$obs."/".$beam;
  Dada::logMsg(2, $dl, "deleteCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "deleteCompletedBeam: localSshCommand() ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "deleteCompletedBeam: ".$user."@".$host.":".$dir."/".$obs."/".$beam." did not exist");
    
  } else {

    # delete the remote directory (just do whole obs)
    $cmd = "rm -rf ".$dir."/../".$dest."/".$obs;
    Dada::logMsg(2, $dl, "deleteCompletedBeam: localSshCommand(".$user.", ".$host.", ".$cmd.")");
    ($result, $response) = localSshCommand($user, $host, $cmd);
    Dada::logMsg(2, $dl, "deleteCompletedBeam: localSshCommand() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "deleteCompletedBeam: could not delete ".$user."@".$host.":".$dir."/".
                    $obs."/".$beam.": ".$response);
    }
  }

  return ($result, $response);
}

#
# Rewinds the current tape and reads the first "index" file
#
sub getCurrentTape() {
 
  Dada::logMsg(2, $dl, "getCurrentTape()"); 

  my $result = "";
  my $response = "";
  my $tape_id = "";

  # First we need to check whether a tape exists in the robot
  Dada::logMsg(2, $dl, "getCurrentTape: tapeIsLoaded()");
  ($result, $response) = Dada::tapes::tapeIsLoaded();
  Dada::logMsg(2, $dl, "getCurrentTape: tapeIsLoaded ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "getCurrentTape: tapeIsLoaded failed: ".$response);
    return ("fail", "could not determine if tape is loaded in drive");
  }

  # If there is a tape in the drive...
  if ($response eq 1) {
    Dada::logMsg(2, $dl, "getCurrentTape: tapeGetID()");
    ($result, $response) = Dada::tapes::tapeGetID();
    Dada::logMsg(2, $dl, "getCurrentTape: tapeGetID() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getCurrentTape: tapeGetID failed: ".$response);
      return ("fail", "bad binary label on current tape");
    }

    # we either have a good ID or no ID
    $tape_id = $response;

  # no tape is loaded in the drive
  } else {

    # get the expected tape to be loaded
    Dada::logMsg(2, $dl, "getCurrentTape: getExpectedTape()");
    ($result, $response) = getExpectedTape();
    Dada::logMsg(2, $dl, "getCurrentTape: getExpectedTape() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getCurrentTape: getExpectedTape() failed: ".$response);
      return ("fail", "could not determine the tape to be loaded");
    }

    my $expected_tape = $response;
    Dada::logMsg(2, $dl, "getCurrentTape: loadTape(".$expected_tape.")");
    ($result, $response) = loadTape($expected_tape);
    Dada::logMsg(2, $dl, "getCurrentTape: loadTape() ".$result." ".$response);

    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getCurrentTape: loadTape() failed: ".$response);
      return ("fail", "could not load the expected tape");
    }

    Dada::logMsg(2, $dl, "getCurrentTape: expected tape now loaded");
    $tape_id = $expected_tape;
  }

  # check for empty tape with no binary label
  if ($tape_id eq "") {
    Dada::logMsg(0, $dl, "getCurrentTape: no binary label existed on tape");
    return ("fail", "no binary label existed on tape");
  }

  Dada::logMsg(2, $dl, "getCurrentTape: current tape = ".$tape_id);
  return ("ok", $tape_id);

}


#
# Read the local tape database and determine what the current
# tape should be
#
sub getExpectedTape() {
  
  my $fname = $db_dir."/".$tapes_db;
  my $expected_tape = "none";
  my $newbkid;
  $bkid="";

  Dada::logMsg(2, $dl, "getExpectedTape: ()");

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

      Dada::logMsg(3, $dl, "getExpectedTape: testing ".$line);

      if ($expected_tape eq "none") {
        my ($id, $size, $used, $free, $nfiles, $full, $newbkid) = split(/ +/,$line);
     
        if (int($full) == 1) {
          Dada::logMsg(3, $dl, "getExpectedTape: skipping tape ".$id.", marked full");
        } elsif ($free < 0.1) {
          Dada::logMsg(1, $dl, "getExpectedTape: skipping tape ".$id." only ".$free." MB left");
        } else {
          $expected_tape = $id;
          $bkid=$newbkid;
        }
      }
    }
  } 
  Dada::logMsg(2, $dl, "getExpectedTape: returning ".$expected_tape);
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

  Dada::logMsg(2, $dl, "newTape()");

  my $result = "";
  my $response = "";

  # Determine what the "next" tape should be
  Dada::logMsg(2, $dl, "newTape: getExpectedTape()");
  ($result, $response) = getExpectedTape();
  Dada::logMsg(2, $dl, "newTape: getExpectedTape() ".$result." ".$response);

  if ($result ne "ok") {
    return ("fail", "getExpectedTape failed: ".$response);
  }
  
  my $new_tape = $response;

  # Now get the tape loaded
  Dada::logMsg(2, $dl, "newTape: loadTape(".$new_tape.")");
  ($result, $response) = loadTape($new_tape);
  Dada::logMsg(2, $dl, "newTape: loadTape(): ".$result." ".$response);

  if ($result ne "ok") {
    return ("fail", "loadTape failed: ".$response);
  }

  return ("ok", $new_tape);

}

################################################################################
##
## DATABASE FUNCTIONS
##


#
# Not sure what this does!
#
sub updateBookKeepr($$$$){
  my ($psrxmlfile,$bookkeepr,$id,$number) = @_;
  my $cmd="";
  my $result="";
  my $response="";
  my $psrxmlid="";
  if ($bkid=="0"){
    $cmd="book_create_tape ".$bookkeepr." ".$id." ".uc($type)." | & sed -e 's:.*<id>\\([^>]*\\)</id>.*:\\1:p' -e 'd'";
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


  Dada::logMsg(2, $dl, "readTapesDB()");

  my $fname = $db_dir."/".$tapes_db;

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

    Dada::logMsg(3, $dl, "readTapesDB: processing line: ".$line);
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

  return @db;

}


#
# Update the tapes database with the specified information
#
sub updateTapesDB($$$$$$) {

  my ($id, $size, $used, $free, $nfiles, $full) = @_;


  Dada::logMsg(2, $dl, "updateTapesDB: ($id, $size, $used, $free, $nfiles, $full)");

  my $fname = $db_dir."/".$tapes_db;
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
      Dada::logMsg(1, $dl, "updateTapesDB: ".$newline);
      print FH $newline."\n";
    } else {
      print FH $line;
    }
  
  }

  close FH;

  return ("ok", "");

}

#
# update the Files DB
#
sub updateFilesDB($$$$) {

  my ($archive, $tape, $fsf, $size) = @_;

  Dada::logMsg(2, $dl, "updateFilesDB(".$archive.", ".$tape.", ".$fsf.", ".$size.")");

  my $fname = $db_dir."/".$files_db;

  my $date = Dada::getCurrentDadaTime();

  my $newline = $archive." ".$tape." ".$date." ".$fsf." ".$size;

  open FH, ">>".$fname or return ("fail", "Could not write to tapes db ".$fname);
  Dada::logMsg(1, $dl, "updateFilesDB: ".$newline);
  print FH $newline."\n";
  close FH;

  return ("ok", "");
}


sub getTapeInfo($) {

  my ($id) = @_;

  Dada::logMsg(2, $dl, "getTapeInfo: (".$id.")");

  my $fname = $db_dir."/".$tapes_db;

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

      Dada::logMsg(3, $dl, "getTapeInfo: processing line: ".$line);
      ($id, $size, $used, $free, $nfiles, $full) = split(/ +/,$line);
      
    } else {

      Dada::logMsg(3, $dl, "getTapeInfo: ignoring line: ".$line);
      #ignore
    }
  }

  $nfiles = int($nfiles);

  if ($size eq -1) {
    return ("fail", "could not determine space from tapes.db");
  } else {

    Dada::logMsg(2, $dl, "getTapeInfo: id=".$id.", size=".$size.", used=".$used.", free=".$free.", nfiles=".$nfiles.", full=".$full);
    return ("ok", $id.":".$size.":".$used.":".$free.":".$nfiles.":".$full);
  }

}


#
# Checks the beam directory to see if it has been marked as archived
# and also checks the files.db to check if it has been recorded as
# archived. Returns an error on mismatch.
#
sub checkIfArchived($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;
  Dada::logMsg(2, $dl, "checkIfArchived(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  my $archived_db = 0;    # If the obs/beam is recorded in $files_db
  my $archived_disk = 0;  # If the obs/beam has been marked with shent.to.tape file 

  # Check the files.db to see if the beam is recorded there
  $cmd = "grep '".$obs."/".$beam."' ".$db_dir."/".$files_db;
  Dada::logMsg(2, $dl, "checkIfArchived: ".$cmd);
  my $grep_result = `$cmd`;
    
  # If the grep command failed, probably due to the beam not existing in the file
  if ($? != 0) {

    Dada::logMsg(2, $dl, "checkIfArchived: ".$obs."/".$beam." did not exist in ".$db_dir."/".$files_db);
    $archived_db = 0;

  } else {

    Dada::logMsg(2, $dl, "checkIfArchived: ".$obs."/".$beam." existed in ".$db_dir."/".$files_db);
    $archived_db = 1;

    # check there is only 1 entry in files.db
    my @lines = split(/\n/, $grep_result);
    if ($#lines != 0) {
      Dada::logMsg(0, $dl, "checkIfArchived: more than 1 entry for ".$obs."/".$beam." in ".$db_dir."/".$files_db);
      return("fail", $obs."/".$beam." had more than 1 entry in FILES database");
    } 

  }

  # Check the directory for a on.tape.type file
  $cmd = "ls -1 ".$dir."/".$obs."/".$beam."/on.tape.".$type;
  Dada::logMsg(2, $dl, "checkIfArchived: localSshCommand(".$user.", ".$host.", ".$cmd);
  ($result, $response) = localSshCommand($user, $host, $cmd);
  Dada::logMsg(2, $dl, "checkIfArchived: localSshCommand() ".$result." ".$response);

  if ($result eq "ok") {
    $archived_disk = 1;
    Dada::logMsg(2, $dl, "checkIfArchived: ".$user."@".$host.":".$dir."/".$obs."/".$beam."/on.tape.".$type." existed");
  } else {
    Dada::logMsg(2, $dl, "checkIfArchived: ".$user."@".$host.":".$dir."/".$obs."/".$beam."/on.tape.".$type." did not exist");
    $archived_disk = 0;
  }


  if (($archived_disk == 0) && ($archived_db == 0)) {
    return ("ok", "not archived");
  } elsif (($archived_disk == 1) && ($archived_db == 1)) {
    return ("ok", "archived");
  } else {
    return ("fail", "FILES database does not match flagged files on disk");
  }

}


#
# Polls for the "quitdaemons" file in the control dir
#
sub controlThread($$) {

  my ($quit_file, $pid_file) = @_;

  Dada::logMsg(2, $dl, "controlThread: thread starting");

  # poll for the existence of the control file
  while ((!-f $quit_file) && (!$quit_daemon)) {
    Dada::logMsg(3, $dl, "controlThread: Polling for ".$quit_file);
    sleep(1);
  }

  # signal threads to exit
  $quit_daemon = 1;
  $Dada::tapes::quit_daemon = 1;
  Dada::logMsg(2, $dl, "controlThread: quit daemon signaled");

  Dada::logMsg(2, $dl, "controlThread: Unlinking PID file ".$pid_file);
  unlink($pid_file);

  Dada::logMsg(2, $dl, "controlThread: exiting");

}


#
# Handle INT AND TERM signals
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $0." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  my $dir;

  # force an unlink of any READING control files on exit
  #foreach $dir (@dirs) {
  #  if (-f $dir."/READING") {
  #    unlink ($dir."/READING");
  #  }
  #}
  sleep(3);
  print STDERR $0." : Exiting: ".Dada::getCurrentDadaTime(0)."\n";

}

sub usage() {
  print STDERR "Usage:\n";
  print STDERR $0." [swin|parkes]\n";
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
  Dada::logMsg(2, $dl, "setStatus(".$string.")");

  my $dir = "/nfs/control/bpsr";
  my $file = $type.".state";

  my $result = "";
  my $response = "";
  my $remote_cmd = "";
  my $cmd = "";

  # Delete the existing state file
  $remote_cmd = "rm -f ".$dir."/".$file;
  $cmd = $ssh_prefix.$remote_cmd.$ssh_suffix;

  Dada::logMsg(2, $dl, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "setStatus: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "setStatus: could not delete the existing state file ".$file.": ".$response);
    return ("fail", "could not remove state file: ".$file);
  }

  # Write the new file
  $remote_cmd = "echo '".$string."' > ".$dir."/".$file;
  $cmd = $ssh_prefix.$remote_cmd.$ssh_suffix;

  Dada::logMsg(2, $dl, "setStatus: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "setStatus: ".$result." ".$response);

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
  Dada::logMsg(2, $dl, "nc_thread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "nc_thread: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "nc_thread: could not automount the /nfs/cluster/shrek??? raid disk");
  }

  $cmd = "ssh ".$ssh_opts." -l ".$user." ".$host." \"cd ".$dir."; ".
         "tar -b 128 -c ".$obs."/".$beam." | nc -l ".$port."\"";

  Dada::logMsg(2, $dl, "nc_thread: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "nc_thread: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "nc_thread: cmd \"".$cmd."\" failed: ".$response);
    return ("fail");
  }
  return ("ok");
}

#
# Mark the obsevation/beam as on.tape.type, and also remotely in /nfs/archives
#
sub markSentToTape($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  Dada::logMsg(2, $dl, "markSentToTape(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.")");

  my $cmd = "";
  my $remote_cmd = "";
  my $result = "";
  my $response = "";
  my $gw_cmd = "";
  my $to_touch = "";

  if ($beam ne "") {
    $to_touch = $obs."/".$beam."/on.tape.".$type;
  } else {
    $to_touch = $obs."/on.tape.".$type;
  }

  $cmd = "ssh -x ".$ssh_opts." -l ".$user." ".$host." \"touch ".$dir."/".$to_touch."\"";
  Dada::logMsg(2, $dl, "markSentToTape:" .$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "markSentToTape:" .$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "markSentToTape: could not touch ".$to_touch);
  }

  # if the beam is set, touch the beams on.tape.type
  $remote_cmd = "touch ".$cfg{"SERVER_ARCHIVE_DIR"}."/".$to_touch;
  $cmd = $ssh_prefix.$remote_cmd.$ssh_suffix;

  Dada::logMsg(2, $dl, "markSentToTape: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "markSentToTape: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "markSentToTape: could not touch remote on.tape.".$type.": ".$response);
  }
  
  return ($result, $response);

}

sub loadTape($) {

  (my $tape) = @_;
  
  my $result = "";
  my $response = "";
  my $string = "";

  if ($robot) {
    $string = "Changing to ".$tape;
  } else {
    Dada::logMsg(2, $dl, "loadTape: manualClearResponse()");
    ($result, $response) = manualClearResponse();
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "loadTape: manualClearResponse() failed: ".$response);
      return ("fail", "could not clear response file in web interface");
    }
    $string = "Insert Tape:::".$tape;
  }

  Dada::logMsg(2, $dl, "loadTape: setStatus(".$string.")");
  ($result, $response) = setStatus($string);
  Dada::logMsg(2, $dl, "loadTape: setStatus() ".$result." ".$response);

  Dada::logMsg(2, $dl, "loadTape: Dada::tapes::loadTapeGeneral(".$tape.")");
  ($result, $response) = Dada::tapes::loadTapeGeneral($tape);
  Dada::logMsg(2, $dl, "loadTape: Dada::tapes::loadTapeGeneral() ".$result." ".$response);

  if (($result eq "ok") && ($response eq $tape)) {
    $string = "Current Tape: ".$tape;
  } else {
    $string = "Failed to Load: ".$tape;
  }

  Dada::logMsg(2, $dl, "loadTape: setStatus(".$string.")");
  ($result, $response) = setStatus($string);
  Dada::logMsg(2, $dl, "loadTape: setStatus() ".$result." ".$response);

  if ($string =~ m/^Current Tape/) {
    return ("ok", $tape);
  } else {
    return ("fail", "failed to load ".$tape);
  }
}




#
#
#
sub localSshCommand($$$) {

  my ($user, $host, $command) = @_;
  Dada::logMsg(2, $dl, "localSshCommand(".$user.", ".$host.", ".$command);

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "ssh -x ".$ssh_opts." -l ".$user." ".$host." \"".$command."\"";

  Dada::logMsg(2, $dl, "localSshCommand:" .$cmd);
  ($result,$response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "localSshCommand:" .$result." ".$response);

  return ($result, $response);

}

#
# deletes the beam directory off of a host
#
sub completed_thread($$$$$) {

  my ($user, $host, $dir, $obs, $beam) = @_;

  my $result = "";
  my $response = "";
  my $dest = "on_tape";

  if (($robot eq 1) && ($dir =~ m/pulsars/)) {
    $dest = "on_tape_pulsars";
  }

  Dada::logMsg(2, $dl, "completed_thread: moveCompeltedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$dest.")");
  ($result, $response) = moveCompletedBeam($user, $host, $dir, $obs, $beam, $dest);
  Dada::logMsg(2, $dl, "completed_thread: moveCompeltedBeam() ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "completed_thread: moveCompletedBeam() failed: ".$response);
    setStatus("Error: could not move beam: ".$obs."/".$beam);
  }

  # We delete the beam if its at parkes (no processing there, or if it is not a survey pointing
  if (($result eq "ok") && ($robot eq 0)) {
    Dada::logMsg(2, $dl, "completed_thread: deleteCompletedBeam(".$user.", ".$host.", ".$dir.", ".$obs.", ".$beam.", ".$dest.")");
    ($result, $response) = deleteCompletedBeam($user, $host, $dir, $obs, $beam, $dest);
    Dada::logMsg(2, $dl, "completed_thread: deleteCompletedBeam() ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "completed_thread: deleteCompletedBeam() failed: ".$response);
      setStatus("Error: could not delete beam: ".$obs."/".$beam);
    }
  }
}

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
  if (index($required_host, Dada::getHostMachineName()) < 0 ) {
    return ("fail", "Error: script must be run on ".$required_host.
                    ", not ".Dada::getHostMachineName());
  }

  if (! (($type eq "swin") || ($type eq "parkes"))) {
    return ("fail", "Error: package global type [".$type."] was not swin or parkes");
  }

  if (! -f ($db_dir."/".$tapes_db)) {
    return ("fail", "tapes db file [".$db_dir."/".$tapes_db."] did not exist");
  }

  if (! -f ($db_dir."/".$files_db)) {
    return ("fail", "files db file [".$db_dir."/".$files_db."] did not exist");
  }

  # Ensure more than one copy of this daemon is not running
  my ($result, $response) = Dada::checkScriptIsUnique(basename($0));
  if ($result ne "ok") {
    return ($result, $response);
  }

  return ("ok", "");
}


#
# get the .response file from the web interface
#
sub manualGetResponse() {

  Dada::logMsg(2, $dl, "manualGetResponse()");

  my $dir = "/nfs/control/bpsr/";
  my $file = $type.".response";
  my $remote_cmd = "";
  my $cmd = "";
  my $result = "";
  my $response = "";

  # Delete the existing state file
  $remote_cmd = "cat ".$dir."/".$file;
  $cmd = $ssh_prefix.$remote_cmd.$ssh_suffix;

  # Wait for a response to appear from the user
  Dada::logMsg(2, $dl, "manualGetResponse: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl,"manualGetResponse: ".$result." ".$response);

  if ($result ne "ok") {
    return ("fail", "could not read file: ".$file);
  }

  return ($result, $response);

}

#
# delete the .response file from the web interface
#
sub manualClearResponse() {

  Dada::logMsg(2, $dl, "manualClearResponse()");

  my $result = "";
  my $response = "";
  my $dir = "/nfs/control/bpsr/";
  my $file = $type.".response";
  my $remote_cmd = "";
  my $cmd = "";

  # Delete the existing state file
  $remote_cmd = "rm -f ".$dir."/".$file;
  $cmd = $ssh_prefix.$remote_cmd.$ssh_suffix;

  # Wait for a response to appear from the user
  Dada::logMsg(2, $dl, "manualClearResponse: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "manualClearResponse: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "manualClearResponse: could not delete the existing state file ".$file.": ".$response);
    return ("fail", "could not remove state file: ".$file);
  }

  return ("ok", "");
}



END { }

1;  # return value from file

