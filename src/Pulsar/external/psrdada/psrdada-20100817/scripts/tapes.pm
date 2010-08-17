package Dada::tapes;

##############################################################################
#
# Dada::tapes
#
# Generic tape functinos
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use threads;
use threads::shared;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&loadTapeGeneral &unloadCurrentTape &tapeFSF &tapeIsLoaded &tapeGetID &getTapeStatus);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $dev $robot $quit_daemon);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $dev;
our $robot;
our $quit_daemon : shared;

#
# non-exported package globals go here
#

#
# initialize package globals
#
$dl = 1;
$dev = "/dev/nst0";
$robot = 0;
$quit_daemon = 0;

#
# initialize other variables
#


###################################################################3
#
# Package methods
#
# tapeFSF   seeks forward the specified number of files
#


#
# seek forward the specified number of files
#
sub tapeFSF($) {

  (my $nfiles) = @_;

  Dada::logMsg(2, $dl, "tapeFSF: (".$nfiles.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." fsf ".$nfiles;
  Dada::logMsg(2, $dl, "tapeFSF: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeFSF: ".$cmd." failed: ".$response);
    return ("fail", "FSF failed: ".$response);
  }

  return ("ok", "");

}


#
# checks to see if the tape drive thinks it has a tape in it
#
sub tapeIsLoaded() {

  Dada::logMsg(2, $dl, "tapeIsLoaded()");

  my $is_loaded = 0;
  my $cmd = "mt -f ".$dev." status 2>&1 | grep ' ONLINE ' > /dev/null";
  my $result = "";
  my $response = "";

  Dada::logMsg(2, $dl, "tapeIsLoaded: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeIsLoaded: ".$result." ".$response);
  if ($result eq "ok") {
    $is_loaded = 1;
  }

  Dada::logMsg(2, $dl, "tapeIsLoaded: returning ".$is_loaded);

  return ("ok", $is_loaded);

}


#
# Initialise the tape, writing the ID to the first file on the
# tape
#

sub tapeInit($) {

  (my $id) = @_;

  Dada::logMsg(2, $dl, "tapeInit(".$id.")");

  my $result = "";
  my $response = "";

  Dada::logMsg(2, $dl, "tapeInit: tapeWriteID(".$id.")");
  ($result, $response) = tapeWriteID($id);
  Dada::logMsg(2, $dl, "tapeInit: tapeWriteID: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeInit: tapeWriteID() failed: ".$response);
    return ("fail", "could not write tape ID: ". $response);
  }
                                              
  Dada::logMsg(2, $dl, "tapeInit: tapeGetID()");
  ($result, $response) = tapeGetID();
  Dada::logMsg(2, $dl, "tapeInit: tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeInit: tapeGetID() failed: ".$response);
    return ("fail", "could not get tape ID from tape");
  }

  if ($id ne $response) {
    Dada::logMsg(0, $dl, "tapeInit: newly written ID did not match specified");
    return ("fail", "could not write tape ID to tape");
  }

  return ("ok", $id);

}

 
#
# Rewind, and read the first file from the tape
#
sub tapeGetID() {

  Dada::logMsg(2, $dl, "tapeGetID()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." rewind";
  Dada::logMsg(2, $dl, "tapeGetID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeGetID: ".$cmd." failed: ".$response);
    return ("fail", "mt rewind command failed: ".$response);;
  }

  $cmd = "tar -tf ".$dev;
  Dada::logMsg(2, $dl, "tapeGetID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeGetID: ".$result." ".$response);

  if ($result ne "ok") {

    # if there is no ID on the tape this command will fail, 
    # but we can test the output message
    if ($response =~ m/tar: At beginning of tape, quitting now/) {

      Dada::logMsg(0, $dl, "tapeGetID: No ID on Tape");
      return ("ok", "");

    } else {

      Dada::logMsg(0, $dl, "tapeGetID: ".$cmd." failed: ".$response);
      return ("fail", "tar list command failed: ".$response);

    }
  }

  Dada::logMsg(2, $dl, "tapeGetID: ID = ".$response);
  my $tape_label = $response;

  Dada::logMsg(2, $dl, "tapeGetID: tapeFSF(1)");
  ($result, $response) = tapeFSF(1);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeGetID: tapeFSF failed: ".$response);
    return ("fail", "tapeFSF() failed to move forward 1 file");
  }

  ($result, my $filenum, my $blocknum) = getTapeStatus();
  if ($result ne "ok") { 
    Dada::logMsg(0, $dl, "tapeGetID: getTapeStatus() failed.");
    return ("fail", "getTapeStatus() failed");
  }

  while ($filenum ne 1 || $blocknum ne 0){
    # we are not at 0 block of file 1...
    Dada::logMsg(1, $dl, "tapeGetID: Tape out of position (f=$filenum, b=$blocknum), rewinding and skipping to start of data");
    $cmd="mt -f ".$dev." rewind; mt -f ".$dev." fsf 1";
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "tapeGetID: tape re-wind/skip failed: ".$response);
      return ("fail", "tape re-wind/skip failed: ".$response);
    }

    ($result, $filenum, $blocknum) = getTapeStatus();
    if ($result ne "ok") { 
      Dada::logMsg(0, $dl, "tapeGetID: getTapeStatus() failed.");
      return ("fail", "getTapeStatus() failed");
    }
  }
  # The tape MUST now be in the right place to start
  Dada::logMsg(2, $dl, "tapeGetID: ID = ".$tape_label);

  return ("ok", $tape_label);
}

#
# Rewind, and write the first file from the tape
#
sub tapeWriteID($) {

  (my $tape_id) = @_;

  Dada::logMsg(2, $dl, "tapeWriteID()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." rewind";
  Dada::logMsg(2, $dl, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeWriteID: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeWriteID: ".$cmd." failed: ".$response);
    return ("fail", "mt rewind failed: ".$response);
  }

  # create an emprty file in the CWD to use
  $cmd = "touch ".$tape_id;
  Dada::logMsg(2, $dl, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeWriteID: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeWriteID: ".$cmd." failed: ".$response);
    return ("fail", "could not create tmp file in cwd: ".$response);
  }

  # write the empty file to tape
  $cmd = "tar -cf ".$dev." ".$tape_id;
  Dada::logMsg(2, $dl, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeWriteID: ".$result." ".$response);

  unlink($tape_id);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "tapeWriteID: ".$cmd." failed: ".$response);
    return ("fail", "could not write ID to tape: ".$response);
  } 

  # write the EOF marker
  $cmd = "mt -f ".$dev." weof";
  Dada::logMsg(2, $dl, "tapeWriteID: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "tapeWriteID: ".$result." ".$response);

  # Initialisze the tapes DB record also
  #Dada::logMsg(2, $dl, "tapeWriteID: updatesTapesDB(".$tape_id.", ".TAPE_SIZE.", 0, ".TAPE_SIZE.", 1, 0)");
  #($result, $response) = updateTapesDB($tape_id, TAPE_SIZE, 0, TAPE_SIZE, 1, 0);
  #Dada::logMsg(2, $dl, "tapeWriteID: updatesTapesDB(): ".$result.", ".$response);

  return ("ok", $response);

}

sub getTapeStatus() {

  my $cmd = "";
  my $filenum = 0;
  my $blocknum = 0;

  Dada::logMsg(2, $dl, "getTapeStatus()");

  # Parkes robot has a different print out than the swinburne one
  if ($robot eq 0) {
    $cmd="mt -f ".$dev." status | grep 'file number' | awk '{print \$4}'";
  } else {
    $cmd="mt -f ".$dev." status | grep 'File number' | awk -F, '{print \$1}' | awk -F= '{print \$2}'";
  }
  Dada::logMsg(3, $dl, "getTapeStatus: cmd= $cmd");

  my ($result,$response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "getTapeStatus: Failed $response");
    $filenum = -1;
    $blocknum = -1;

  } else {
    $filenum = $response;
    if ($robot eq 0) {
      $cmd="mt -f ".$dev." status | grep 'block number' | awk '{print \$4}'";
    } else {
      $cmd="mt -f ".$dev." status | grep 'block number' | awk -F, '{print \$2}' | awk -F= '{print \$2}'";
    }

    my ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "getTapeStatus: Failed $response");
      $filenum = -1;
      $blocknum = -1;
    } else {
      $blocknum = $response;
    }
  }

  Dada::logMsg(2, $dl, "getTapeStatus: ".$result." ".$filenum." ".$blocknum);
  return ($result,$filenum,$blocknum);
}



###############################################################################
#
# General Tape Functions
#


sub loadTapeGeneral($) {
  
  (my $tape) = @_;
  Dada::logMsg(2, $dl, "Dada::tapes::loadTapeGeneral(".$tape.")");
  my $result = "";
  my $response = "";
  if ($robot) {
    ($result, $response) = loadTapeRobot($tape);
  } else {
    Dada::logMsg(2, $dl, "Dada::tapes::loadTapeGeneral: loadTapeManual(".$tape.")");
    ($result, $response) = loadTapeManual($tape);
    Dada::logMsg(2, $dl, "Dada::tapes::loadTapeGeneral: loadTapeManual() ".$result." ".$response);
  }

  Dada::logMsg(2, $dl, "Dada::tapes::loadTapeGeneral() ".$result." ".$response);
  return ($result, $response);


}  

sub unloadCurrentTape() {

  my $result = "";
  my $response = "";
  if ($robot) {
    ($result, $response) = unloadCurrentTapeRobot();
  } else {
    ($result, $response) = unloadCurrentTapeManual();
  }
  return ($result, $response);
}



#
# General Tape Functions
#

###############################################################################

###############################################################################
#
# Robot Tape Functions
#

#
# get the current tape in the robot
#
sub getCurrentTapeRobot() {

  Dada::logMsg(2, $dl, "getCurrentTapeRobot()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mtx status | grep 'Data Transfer Element' | awk '{print \$10}'";
  Dada::logMsg(2, $dl, "getCurrentTapeRobot: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "getCurrentTapeRobot: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "getCurrentTapeRobot: ".$cmd." failed: ".$response);
    return ("fail", "could not determine current tape in robot");
  }

  Dada::logMsg(2, $dl, "getCurrentTapeRobot: ID = ".$response);
  return ("ok", $response);
   
} 


# 
# Return array of current robot status
#   
sub getStatusRobot() {

  Dada::logMsg(2, $dl, "getStatusRobot()");
  my $cmd = "";
  my $result = "";
  my $response = "";
  my $line = "";
  my @lines = ();
  my %results = (); 
  my @tokens = ();
  my $slotid = "";
  my $tapeid = "";
  my $state = "";
  my $junk = "";
 
  $cmd = "mtx status";
  Dada::logMsg(2, $dl, "getStatusRobot: ".$cmd);

  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    Dada::logMsg(2, $dl, "getStatusRobot: ".$cmd." failed: ".$response);
    return "fail";
  }

  # parse the response
  $line = "";
  @lines = split(/\n/,$response);

  %results = ();

  foreach $line (@lines) {

    @tokens = ();
    @tokens = split(/ +/,$line);

    if ($line =~ m/^Data Transfer Element/) {

      Dada::logMsg(3, $dl, "Transfer: $line");
      if ($tokens[3] eq "0:Full") {
        $results{"transfer"} = $tokens[9];
      } else {
        $results{"transfer"} = "Empty";
      }

    } elsif ($line =~ m/Storage Element/) {

      Dada::logMsg(3, $dl, "Storage: $line");
      ($slotid, $state) = split(/:/,$tokens[3]);

      if ($state eq "Empty") {
        $results{$slotid} = "Empty";
      } else {
        ($junk, $tapeid) = split(/=/,$tokens[4]);
        $results{$slotid} = $tapeid;
      }
    } else {
      # ignore
    }
  }

  return %results;
}




#
# load the specified $tape 
#
sub loadTapeRobot($) {

  (my $tape) = @_;
  Dada::logMsg(2, $dl, "loadTapeRobot: getStatusRobot()");

  my $result = "";
  my $response = "";
  my %status = getStatusRobot();
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

    Dada::logMsg(0, $dl, "loadTapeRobot: tape ".$tape." did not exist in robot");
    return ("fail", "tape not in robot") ;

  } elsif ($slot eq "transfer") {

    Dada::logMsg(2, $dl, "loadTapeRobot: tape ".$tape." was already in transfer slot");
    return ("ok","");

  } else {

    Dada::logMsg(2, $dl, "loadTapeRobot: tape ".$tape." in slot ".$slot);

    # if a tape was actually loaded
    if ($status{"transfer"} ne "Empty") {

      # unload the current tape
      Dada::logMsg(2, $dl, "loadTapeRobot: unloadCurrentTape()");
      ($result, $response) = unloadCurrentTape();
      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "loadTapeRobot: unloadCurrentTape failed: ".$response);
        return ("fail", "Could not unload current tape: ".$response);
      }
    }

    # load the tape in the specified slot
    Dada::logMsg(2, $dl, "loadTapeRobot: robotLoadTapeFromSlot(".$slot.")");
    ($result, $response) = loadTapeFromSlot($slot);
    Dada::logMsg(2, $dl, "loadTapeRobot: robotLoadTapeFromSlot: ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "loadTapeRobot: robotLoadTapeFromSlot failed: ".$response);
      return ("fail", "Could not load tape in robot slot ".$slot);
    }

    return ("ok", $tape);
  }

}

#
# load the tape in the specified slot
#
sub loadTapeFromSlot($) {

  (my $slot) = @_;
  
  Dada::logMsg(2, $dl, "loadTapeFromSlot(".$slot.")");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mtx load ".$slot." 0";
  Dada::logMsg(2, $dl, "loadTapeFromSlot: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "loadTapeFromSlot: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "loadTapeFromSlot: ".$cmd." failed: ".$response);
    return ("fail", $response);
  }

  return ("ok", "");

}

sub unloadCurrentTapeRobot() {

  Dada::logMsg(2, $dl, "unloadCurrentTapeRobot()");

  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." eject";
  Dada::logMsg(2, $dl, "unloadCurrentTapeRobot: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "unloadCurrentTapeRobot: ".$result." ".$response);
  
  if ($result ne "ok") { 
    Dada::logMsg(0, $dl, "unloadCurrentTapeRobot: ".$cmd." failed: ".$response);
    return ("fail", "eject command failed");
  }

  $cmd = "mtx unload";
  Dada::logMsg(2, $dl, "unloadCurrentTapeRobot: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "unloadCurrentTapeRobot: ".$result." ".$response);
  
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "unloadCurrentTapeRobot: ".$cmd." failed: ".$response);
    return ("fail", "mtx unload command failed");
  }

  return ("ok", "");
  
}



#
# Robot Tape Functions
#
###############################################################################


###############################################################################
#
# Manual Tape Functions
#

#
# Unloads the tape currently in the robot
#
sub unloadCurrentTapeManual() {

  Dada::logMsg(2, $dl, "unloadCurrentTapeManual()");
  my $cmd = "";
  my $result = "";
  my $response = "";

  $cmd = "mt -f ".$dev." eject";
  Dada::logMsg(2, $dl, "unloadCurrentTapManuale: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "unloadCurrentTapeManual: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "unloadCurrentTapeManual: ".$cmd." failed: ".$response);
    return ("fail", "eject command failed");
  }

  return ("ok", "");

}

sub loadTapeManual($) {

  (my $tape) = @_;

  Dada::logMsg(2, $dl, "loadTapeManual(".$tape.")");
  Dada::logMsg(1, $dl, "loadTapeManual: asking for tape ".$tape);

  my $cmd = "mt -f ".$dev." offline";
  my $result = "";
  my $response = "";
  my $string = "";

  Dada::logMsg(2, $dl, "loadTapeManual: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "loadTapeManual: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "loadTapeManual: tape offline failed: ".$response);
  }

  my $n_tries = 10;
  my $inserted_tape = "none";

  while (($inserted_tape ne $tape) && ($n_tries >= 0) && (!$quit_daemon)) {

    Dada::logMsg(2, $dl, "loadTapeManual: tapeIsLoaded() [qd=".$quit_daemon."]");
    ($result, $response) = tapeIsLoaded();
    Dada::logMsg(2, $dl, "loadTapeManual: tapeIsLoaded ".$result." ".$response);

    if ($result ne "ok") { 
      Dada::logMsg(0, $dl, "loadTapeManual: tapeIsLoaded failed: ".$response);
      return ("fail", "could not determine if tape is loaded in drive");
    }

    # If a tape was not loaded
    if ($response ne 1) {
      $inserted_tape = "none";
      Dada::logMsg(1, $dl, "loadTapeManual: sleeping 10 seconds for tape online");
      sleep(10);

    } else {

      Dada::logMsg(2, $dl, "loadTapeManual: tapeGetID()");
      ($result, $response) = tapeGetID();
      Dada::logMsg(2, $dl, "loadTapeManual: tapeGetID() ".$result." ".$response);

      if ($result ne "ok") {
        Dada::logMsg(0, $dl, "loadTapeManual: tapeGetID() failed: ".$response);
        $inserted_tape = "none";
      } else {
        $inserted_tape = $response;
      }
    }
  }

  Dada::logMsg(2, $dl, "loadTapeManual: qd=".$quit_daemon);

  if ($inserted_tape eq $tape) {
    return ("ok", $inserted_tape);
  } else {
    return ("fail", "failed to insert tape: ".$tape);
  }

}



#
# Manual Tape Functions
#
###############################################################################



END { }

1;  # return value from file

