#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_web_monitor.pl
#
# This script maintains vital statistics for all the client/server machines
# and stores this information in memory. Web browsers will connect to a port
# on this script for this information
#

use lib $ENV{"DADA_ROOT"}."/bin";


use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Bpsr;           # Bpsr Module for configuration options
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Sanity check to prevent multiple copies of this daemon running
#
Dada::preventDuplicateDaemon(basename($0));


#
# Constants
#
use constant DL           => 1;
use constant PIDFILE      => "bpsr_web_monitor.pid";
use constant QUITFILE     => "bpsr_web_monitor.quit";
use constant LOGFILE      => "bpsr_web_monitor.log";
use constant STATUS_OK    => 0;
use constant STATUS_WARN  => 1;
use constant STATUS_ERROR => 2;



#
# Global Variables
#
our %cfg = Bpsr::getBpsrConfig();      # Bpsr.cfg
our $error                      = $cfg{"STATUS_DIR"}."/bpsr_web_monitor.error";
our $warn                       = $cfg{"STATUS_DIR"}."/bpsr_web_monitor.warn";
our $quit_daemon : shared       = 0;
our $node_info : shared         = "";
our $curr_obs : shared          = "";
our $status_info : shared       = "";
our $tape_info_string : shared  = "";
our $bp_image_string : shared   = "";
our $ts_image_string : shared   = "";
our $fft_image_string : shared  = "";
our $dts_image_string : shared  = "";
our $pvf_image_string : shared  = "";
our $pdbp_image_string : shared = "";

# Autoflush output
$| = 1;


# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;


#
# Local Varaibles
#
my $logfile = $cfg{"SERVER_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
my $daemon_control_thread = 0;
my $result = "";
my $response = "";
my $cmd = "";
my $i = 0;
my $server_socket = "";   # Server socket for new connections
my $rh = "";
my $string = "";


# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}

$server_socket = new IO::Socket::INET (
    LocalHost => $cfg{"SERVER_HOST"}, 
    LocalPort => $cfg{"SERVER_WEB_MONITOR_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);
die "Could not create listening socket: $!\n" unless $server_socket;

# Redirect standard output and error
Dada::daemonize($logfile, $pidfile);

Dada::logMsg(0, DL, "STARTING SCRIPT");

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

my $read_set = new IO::Select();  # create handle set for reading
$read_set->add($server_socket);   # add the main socket to the set

Dada::logMsg(2, DL, "Waiting for connection on ".$cfg{"SERVER_HOST"}.":".$cfg{"SERVER_WEB_MONITOR_PORT"});

#
# Data to "maintain"

#  * Disk space on nodes [R]
#  * Data block info on nodes [R]
#  * Load info on nodes [R]
my $node_info_thread = 0;

#  * Current images for web display [L]
my $image_info_thread = 0;

#  * Current source information [L]
my $curr_info_thread = 0;

#  * Current status file information [L]
my $status_info_thread = 0;

#  * Current tape/xfer information [L]
my $tape_info_thread = 0;

# Launch the current info thread
$curr_info_thread = threads->new(\&currentInfoThread, $cfg{"SERVER_RESULTS_DIR"});
$tape_info_thread = threads->new(\&tapeInfoThread);
$status_info_thread = threads->new(\&statusInfoThread);
$node_info_thread = threads->new(\&nodeInfoThread);
$image_info_thread = threads->new(\&imageInfoThread, $cfg{"SERVER_RESULTS_DIR"});

# Sleep for a few seconds to allow the threads to start and collect
# their first iteration of data
sleep(3);

my $handle = 0;

while (!$quit_daemon) {

  # Get all the readable handles from the server
  my ($rh_set) = IO::Select->select($read_set, undef, undef, 2);

  foreach $rh (@$rh_set) {

    # if it is the main socket then we have an incoming connection and
    # we should accept() it and then add the new socket to the $Read_Handles_Object
    if ($rh == $server_socket) { 

      $handle = $rh->accept();
      $handle->autoflush();
      # my $hostinfo = gethostbyaddr($handle->peeraddr);
      # my $hostname = $hostinfo->name;
      Dada::logMsg(2, DL, "Accepting connection");

      # Add this read handle to the set
      $read_set->add($handle); 
      $handle = 0;

    } else {

      # my $hostinfo = gethostbyaddr($rh->peeraddr);
      # my $hostname = $hostinfo->name;
      # my @parts = split(/\./,$hostname);
      # my $machine = $parts[0];
      $string = Dada::getLine($rh);

      if (! defined $string) {
        Dada::logMsg(2, DL, "Closing a connection");
        $read_set->remove($rh);
        close($rh);
      } else {

        Dada::logMsg(2, DL, "<- ".$string);
        my $r = "";

        if ($string eq "node_info")     { $r = $node_info; }
        elsif ($string eq "bp_img_info")   { $r = $bp_image_string; }
        elsif ($string eq "ts_img_info")   { $r = $ts_image_string; }
        elsif ($string eq "fft_img_info")  { $r = $fft_image_string; }
        elsif ($string eq "dts_img_info")  { $r = $dts_image_string; }
        elsif ($string eq "pvf_img_info")  { $r = $pvf_image_string; }
        elsif ($string eq "pdbp_img_info") { $r = $pdbp_image_string; }
        elsif ($string eq "curr_obs")      { $r = $curr_obs; }
        elsif ($string eq "status_info")   { $r = $status_info; }
        elsif ($string eq "tape_info")     { $r = $tape_info_string; }
        else    { Dada::logMsgWarn($warn, "unexpected command: ".$string); } 

        print $rh $r."\r\n";
        Dada::logMsg(2, DL, "-> ".$r);
      }
    }
  }
}

# Rejoin our daemon control thread
$daemon_control_thread->join();

# Rejoin other threads
$curr_info_thread->join();
$tape_info_thread->join();
$status_info_thread->join();
$node_info_thread->join();
$image_info_thread->join();

close($server_socket);
                                                                                
Dada::logMsg(0, DL, "STOPPING SCRIPT: ".Dada->getCurrentDadaTime(0));
                                                                                
exit(0);


###############################################################################
#
# Functions
#

sub daemonControlThread() {

  Dada::logMsg(2, DL, "daemonControlThread: starting");

  my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;

  my $daemon_quit_file = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;

  # Poll for the existence of the control file
  while ((!-f $daemon_quit_file) && (!$quit_daemon)) {
    sleep(1);
  }

  # set the global variable to quit the daemon
  $quit_daemon = 1;

  Dada::logMsg(2, DL, "Unlinking PID file: ".$pidfile);
  unlink($pidfile);

  Dada::logMsg(1, DL, "daemonControlThread: exiting");

}


#
# Handle INT AND TERM signals
#
sub sigHandle($) {
                                                                                
  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(5);
  print STDERR basename($0)." : Exiting\n";
  exit(1);
                                                                                
}


#
# maintains the curr_info string
#
sub currentInfoThread($) {

  (my $results_dir) = @_;

  Dada::logMsg(1, DL, "currentInfoThread: starting");

  my $sleep_time = 10;
  my $sleep_counter = 0;

  my $cmd = "find ".$results_dir." -maxdepth 1 -type d -name '2*' -printf '\%f\n' | sort | tail -n 1";
  my $obs = "";
  my $tmp_str = "";
  my %cfg_file = ();

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      $sleep_counter--;
      sleep(1);
    
    } else {

      $sleep_counter = $sleep_time;

      Dada::logMsg(3, DL, "currentInfoThread: ".$cmd);
      $obs = `$cmd`;
      chomp $obs ;
      Dada::logMsg(3, DL, "currentInfoThread: ".$obs);

      if (-f $results_dir."/".$obs."/obs.info") {
        $tmp_str = "";
        %cfg_file = Dada::readCFGFile($results_dir."/".$obs."/obs.info"); 

        $tmp_str .= "SOURCE:::".$cfg_file{"SOURCE"}.";;;";
        $tmp_str .= "RA:::".$cfg_file{"RA"}.";;;";
        $tmp_str .= "DEC:::".$cfg_file{"DEC"}.";;;";
        $tmp_str .= "CFREQ:::".$cfg_file{"CFREQ"}.";;;";
        $tmp_str .= "BANDWIDTH:::".$cfg_file{"BANDWIDTH"}.";;;";
        $tmp_str .= "ACC_LEN:::".$cfg_file{"ACC_LEN"}.";;;";
        $tmp_str .= "NUM_PWC:::".$cfg_file{"NUM_PWC"}.";;;";
        $tmp_str .= "PID:::".$cfg_file{"PID"}.";;;";
        $tmp_str .= "UTC_START:::".$cfg_file{"UTC_START"}.";;;";
        $tmp_str .= "INTERGRATED:::0;;;";
 
        Dada::logMsg(3, DL, $tmp_str); 
        # update the global variable 
        $curr_obs = $tmp_str;

        Dada::logMsg(2, DL, "currInfoThread: ".$curr_obs);
        
      }
    }
  }

  Dada::logMsg(1, DL, "currentInfoThread: exiting");

}


#
# Maintains a listing of the current images
#
sub imageInfoThread($) {

  my ($results_dir) = @_;

  Dada::logMsg(1, DL, "imageInfoThread: starting");

  my $sleep_time = 10;
  my $sleep_counter = 0;

  my $cmd = "";
  my $image_string = "";
  my @images = ();
  my @image_types = qw(bp ts fft dts pvf);
  my $obs = "";
  my $tmp_str = "";

  my $i = 0;
  my @arr = ();
  my %bp_images = ();
  my %ts_images = ();
  my %fft_images = ();
  my %dts_images = ();
  my %pvf_images = ();
  my %pdbp_images = ();
  my @keys = ();

  chdir $results_dir;

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      $sleep_counter--;
      sleep(1);

    } else {

      $sleep_counter = $sleep_time;

      # find the most recent directory (ls gives arg list too long)
      $cmd = "find . -maxdepth 1 -type d -name '2*' -printf '\%f\n' | sort | tail -n 1";
      $obs = `$cmd`;
      chomp $obs;

      $cmd = "find ".$obs." -name '*_112x84.png' | sort | awk -F/ '{print \$2, \$3}'";
      $image_string = `$cmd`;
      @images = split(/\n/, $image_string);

      %bp_images = ();
      %ts_images = ();
      %fft_images = ();
      %dts_images = ();
      %pvf_images = ();

      for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
        $bp_images{$cfg{"BEAM_".$i}} = "";
        $ts_images{$cfg{"BEAM_".$i}} = "";
        $fft_images{$cfg{"BEAM_".$i}} = "";
        $dts_images{$cfg{"BEAM_".$i}} = "";
        $pvf_images{$cfg{"BEAM_".$i}} = "";
        $pdbp_images{$cfg{"IBOB_DEST_".$i}} = "";
      }

      for ($i=0; $i<=$#images; $i++) {

        @arr = split(/ /,$images[$i]);

        if ($arr[1] =~ m/bp_112x84/) {
          $bp_images{$arr[0]} = $arr[1];
        }
        if ($arr[1] =~ m/ts_112x84/) {
          $ts_images{$arr[0]} = $arr[1];
        }
        if ($arr[1] =~ m/fft_112x84/) {
          $fft_images{$arr[0]} = $arr[1];
        }
        if ($arr[1] =~ m/dts_112x84/) {
          $dts_images{$arr[0]} = $arr[1];
        }
        if ($arr[1] =~ m/pvf_112x84/) {
          $pvf_images{$arr[0]} = $arr[1];
        }
      }

      # now update the global variables for each image type
      $tmp_str = "";
      @keys = keys %bp_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($bp_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::".$obs."/".$keys[$i]."/".$bp_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $bp_image_string = $tmp_str;

      $tmp_str = "";
      @keys = keys %ts_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($ts_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::".$obs."/".$keys[$i]."/".$ts_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $ts_image_string = $tmp_str;

      $tmp_str = "";
      @keys = keys %fft_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($fft_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::".$obs."/".$keys[$i]."/".$fft_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $fft_image_string = $tmp_str;

      $tmp_str = "";
      @keys = keys %dts_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($dts_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::".$obs."/".$keys[$i]."/".$dts_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $dts_image_string = $tmp_str;

      $tmp_str = "";
      @keys = keys %pvf_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($pvf_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::".$obs."/".$keys[$i]."/".$pvf_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $pvf_image_string = $tmp_str;

      # Now lookup the Pre Decimation Band Pass images
      $cmd = "find stats -name '*_112x84.png' -printf '\%f\n' | sort";
      $image_string = `$cmd`;
      @images = split(/\n/, $image_string);
  
      %pdbp_images = ();
      for ($i=0; $i<=$#images; $i++) {
        chomp $images[$i];
        @arr = split(/_/,$images[$i]);
        $pdbp_images{$arr[1]} = $images[$i];
      }

      $tmp_str = "";
      @keys = keys %pdbp_images;
      for ($i=0; $i<=$#keys; $i++) {
        if ($pdbp_images{$keys[$i]} ne "") {
          $tmp_str .= $keys[$i].":::stats/".$pdbp_images{$keys[$i]}.";;;";
        } else {
          $tmp_str .= $keys[$i].":::../../images/blankimage.gif;;;";
        }
      }
      $pdbp_image_string = $tmp_str;

      Dada::logMsg(2, DL, "imageInfoThread: collected images");

    }
  }

  Dada::logMsg(1, DL, "imageInfoThread: exiting");

}


sub tapeInfoThread() {

  Dada::logMsg(1, DL, "tapeInfoThread: starting");

  my $sleep_time = 30;
  my $sleep_counter = 0;
  my $secondary_freq = 6;
  my $secondary_counter = 0;
  my $tmp_str = "";
  my $cmd = "";

  my $control_dir = "/nfs/control/bpsr";
  my $parkes_file = $control_dir."/parkes.state";
  my $swin_file = $control_dir."/swin.state";
  my $xfer_file = $control_dir."/xfer.state";

  # Tape archiving
  my @parkes_db = split(/:/, $cfg{"PARKES_DB_DIR"});
  my @swin_db = split(/:/, $cfg{"SWIN_DB_DIR"});
  my @parkes_files_db = ();
  my @swin_files_db = ();
  my $parkes_tape = "";
  my $parkes_percent = 0;
  my $parkes_time_left = 0;
  my $swin_tape = "";
  my $swin_percent = 0;
  my $swin_time_left = 0;

  my $i = 0;
  my $j = 0;
  my @arr = ();
  my @arr2 = ();

  # Buffering areas
  my @p_users = ();
  my @p_hosts = ();
  my @p_paths = ();
  my %parkes = ();
  for ($i=0; $i<$cfg{"NUM_PARKES_DIRS"}; $i++) {
    @arr = split(/:/, $cfg{"PARKES_DIR_".$i});
    $p_users[$i] = $arr[0]; 
    $p_hosts[$i] = $arr[1]; 
    $p_paths[$i] = $arr[2]; 
  }

  my @s_users = ();
  my @s_hosts = ();
  my @s_paths = ();
  my %swin = ();
  for ($i=0; $i<$cfg{"NUM_SWIN_DIRS"}; $i++) {
    @arr = split(/:/, $cfg{"SWIN_DIR_".$i});
    $s_users[$i] = $arr[0];
    $s_hosts[$i] = $arr[1];
    $s_paths[$i] = $arr[2];
  }
  
  my $parkes_state = "";
  my $swin_state = "";
  my $xfer_state = "";
  my $parkes_pid = "";
  my $swin_pid = "";
  my $xfer_pid = "";
  my $num_parkes = 0;
  my $num_swin = 0;
  my $want_swin = 0;
  my $want_parkes = 0;

  my $ready_to_send = 0;
  my @obs_fin = ();
  my %sts_list = ();
  my %stp_list = ();
  my $swin_ready = 0;
  my $parkes_ready = 0;
  my $result = "";

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      $sleep_counter--;
      sleep(1);
    } else {

      $sleep_counter = $sleep_time;

      # determine the PID's of the current transfer manager, parkes tape and swin tapes
      $xfer_pid   = getDaemonPID($cfg{"SERVER_HOST"}, $cfg{"SERVER_XFER_PID_PORT"});
      $parkes_pid = getDaemonPID($cfg{"SERVER_HOST"}, $cfg{"SERVER_PARKESTAPE_PID_PORT"});
      $swin_pid   = getDaemonPID($cfg{"SERVER_HOST"}, $cfg{"SERVER_SWINTAPE_PID_PORT"});

      open FH, "<".$parkes_file;
      read FH, $parkes_state, 4096;
      close FH;
      chomp $parkes_state;

      open FH, "<".$swin_file;
      read FH, $swin_state, 4096;
      close FH;
      chomp $swin_state;

      open FH, "<".$xfer_file;
      read FH, $xfer_state, 4096;
      close FH;
      chomp $xfer_state;

      # Update this information less frequently
      if ($secondary_counter > 0 ) {
        $secondary_counter--;
      } else {
        $secondary_counter = $secondary_freq;

        # Get information from tapes.db
        $cmd = "ssh -x -l ".$parkes_db[0]." ".$parkes_db[1]." 'cat ".$parkes_db[2]."/tapes.".$parkes_pid.".db' | awk '{print \$1,\$2,\$3,\$6}'";
        $tmp_str = `$cmd`;
        chomp $tmp_str;
        @parkes_files_db = split(/\n/,$tmp_str);

        $cmd = "ssh -x -l ".$swin_db[0]." ".$swin_db[1]." 'cat ".$swin_db[2]."/tapes.".$swin_pid.".db' | awk '{print \$1,\$2,\$3,\$6}'";
        $tmp_str = `$cmd`;
        chomp $tmp_str;
        @swin_files_db = split(/\n/,$tmp_str);

        $parkes_tape = "none";
        $parkes_percent = 0;
        $parkes_time_left = 0;
        for ($i=0; (($parkes_tape eq "none") && ($i<=$#parkes_files_db)); $i++) {
          @arr = split(/ /,$parkes_files_db[$i]);
          if ($arr[3] eq "0") {
            $parkes_tape = $arr[0];
            $parkes_percent = sprintf("%5.2f",(int($arr[2]) / int($arr[1]))*100);
            $parkes_time_left = sprintf("%5.1f",(((int($arr[1]) - int($arr[2])) * 1024.0) / (40.0 * 60.0)));
          }
        }

        $swin_tape = "none";
        $swin_percent = 0;
        $swin_time_left = 0;
        for ($i=0; (($swin_tape eq "none") && ($i<=$#swin_files_db)); $i++) {
          @arr = split(/ /,$swin_files_db[$i]);
          if ($arr[3] eq "0") {
            $swin_tape = $arr[0];
            $swin_percent = sprintf("%5.2f",(int($arr[2]) / int($arr[1]))*100);
            $swin_time_left = sprintf("%5.1f",(((int($arr[1]) - int($arr[2])) * 1024.0) / (40.0 * 60.0)));
          }
        }

        Dada::logMsg(2, DL, "tapeInfoThread: percents parkes=".$parkes_percent."% swin=".$swin_percent."%");
        Dada::logMsg(2, DL, "tapeInfoThread: names parkes=".$parkes_tape." swin=".$swin_tape);

        # Find out how many are ready for sending of each type
        %stp_list = ();
        %sts_list = ();
        @obs_fin = ();

        for ($i=0; $i<=14; $i++) {

          # Get a list of "<UTC_START> sent.to.*" from apsr 00 -> 14
          $cmd = "ssh -l bpsr apsr".sprintf("%02d",$i)." \"cd ".$cfg{"CLIENT_ARCHIVE_DIR"}."; ".
                  "find -mindepth 3 -maxdepth 3 -type f -name 'sent.to.*' ".
                  "-printf '\%h/\%f\\n'\" | awk -F/ '{print \$2\" \"\$4}'";
          ($result, $tmp_str) = Dada::mySystem($cmd);

          if ($result eq "ok") {
            @arr = split(/\n/,$tmp_str);

            for ($j=0; $j<=$#arr; $j++) {
              @arr2 = split(/ /,$arr[$j]);
              if ($arr2[1] eq "sent.to.swin") {
                if (!defined $sts_list{$arr2[0]}) {
                  $sts_list{$arr2[0]} = 0;
                }
                $sts_list{$arr2[0]} += 1;
              } else {
                if (!defined $stp_list{$arr2[0]}) {
                  $stp_list{$arr2[0]} = 0;
                }
                $stp_list{$arr2[0]} += 1;
              }
            }
          }
        }
        
        # Now get a list of all observations that have obs.finished in them
        $cmd = "cd ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}."; find  -mindepth 2 -maxdepth 2 -name 'obs.finished' -printf '\%h\\n' | awk -F/ '{print \$2}'";
        ($result, $tmp_str) = Dada::mySystem($cmd);
        if ($result eq "ok") {
          @arr = split(/\n/,$tmp_str);
          for ($j=0; $j<=$#arr; $j++) {

            # check if the PID matches
            $cmd = "grep ^PID ".$cfg{"SERVER_ARCHIVE_NFS_MNT"}."/".$arr[$j]."/obs.info | grep ".$xfer_pid; 
            Dada::logMsg(2, DL, "tapeInfoThread: ".$cmd);
            ($result, $tmp_str) = Dada::mySystem($cmd);
            if ($result eq "ok") {
              push @obs_fin, $arr[$j];
            }
          }
        }

        # Determine how many beams are sent.to.*
        @obs_fin = sort @obs_fin;

        $swin_ready = 13 * ($#obs_fin+1);
        $parkes_ready = 13 * ($#obs_fin+1);

        for ($i=0; $i<=$#obs_fin; $i++) {
          if (defined $sts_list{$obs_fin[$i]}) { 
            $swin_ready -= $sts_list{$obs_fin[$i]};
          }
          if (defined $stp_list{$obs_fin[$i]}) { 
            $parkes_ready -= $stp_list{$obs_fin[$i]};
          }
        }

        # check if each destination is actually one that will be used
        ($want_swin, $want_parkes) = Bpsr::getObsDestinations($xfer_pid, $cfg{$xfer_pid."_DEST"});
        if (!$want_swin) {
          $swin_ready = 0;
        }
        if (!$want_parkes) {
          $parkes_ready = 0;
        }
        
        Dada::logMsg(2, DL, "tapeInfoThread: obs_fin=".($#obs_fin+1).", swin_ready=".$swin_ready.", parkes_ready=".$parkes_ready);

        $num_parkes = 0;
        $num_swin = 0;
        # number of beams in staging area for PARKES_DIRS
        for ($i=0; $i<=$#p_users; $i++) {
          $cmd = "ssh -x -l ".$p_users[$i]." ".$p_hosts[$i]." 'cd ".$p_paths[$i]."/".$parkes_pid."; find staging_area pulsars -mindepth 2 -maxdepth 2 -type d -printf \"\%f\\n\"' | wc -l";
          $tmp_str = `$cmd`;
          chomp $tmp_str;
          $num_parkes += int($tmp_str);
        } 

        Dada::logMsg(2, DL, "tapeInfoThread: num_parkes = ".$num_parkes);

        # number of beams in staging area for SWIN_DIRS
        for ($i=0; $i<=$#s_users; $i++) {
          $cmd = "ssh -x -l ".$s_users[$i]." ".$s_hosts[$i]." 'cd ".$s_paths[$i]."/".$swin_pid."; find staging_area pulsars -mindepth 2 -maxdepth 2 -type d -printf \"\%f\\n\"' | wc -l";
          $tmp_str = `$cmd`;
          chomp $tmp_str;
          $num_swin += int($tmp_str);
        } 
        Dada::logMsg(2, DL, "tapeInfoThread: num_swin= ".$num_swin);
      }

      $tmp_str = "";
      $tmp_str .= "PARKES_STATE:::".$parkes_state.";;;";
      $tmp_str .= "PARKES_TAPE:::".$parkes_tape.";;;";
      $tmp_str .= "PARKES_PERCENT:::".$parkes_percent.";;;";
      $tmp_str .= "PARKES_TIME_LEFT:::".$parkes_time_left.";;;";
      $tmp_str .= "PARKES_NUM:::".$num_parkes.";;;";
      $tmp_str .= "PARKES_PID:::".$parkes_pid.";;;";
      $tmp_str .= "SWIN_STATE:::".$swin_state.";;;";
      $tmp_str .= "SWIN_TAPE:::".$swin_tape.";;;";
      $tmp_str .= "SWIN_PERCENT:::".$swin_percent.";;;";
      $tmp_str .= "SWIN_TIME_LEFT:::".$swin_time_left.";;;";
      $tmp_str .= "SWIN_NUM:::".$num_swin.";;;";
      $tmp_str .= "SWIN_PID:::".$swin_pid.";;;";
      $tmp_str .= "XFER:::".$xfer_state.";;;";
      $tmp_str .= "XFER_SWIN:::".$swin_ready.";;;";
      $tmp_str .= "XFER_PARKES:::".$parkes_ready.";;;";
      $tmp_str .= "XFER_PID:::".$xfer_pid.";;;";

      $tape_info_string = $tmp_str;

      Dada::logMsg(2, DL, "tapeInfoThread: ".$tape_info_string);
    }
  }

  Dada::logMsg(1, DL, "tapeInfoThread: exiting");
}

#
# Monitors the STATUS_DIR for warnings and errors
#
sub statusInfoThread() {

  my $status_dir = $cfg{"STATUS_DIR"};

  Dada::logMsg(1, DL, "statusInfoThread: starting");

  my @server_daemons = split(/ /,$cfg{"SERVER_DAEMONS"}." ".$cfg{"SERVER_DAEMONS_PERSIST"});
  my %clients = ();
  my @files = ();
  my %warnings= ();
  my %errors= ();
  my @arr = ();
  my $sleep_time = 10;
  my $sleep_counter = 0;

  my $i = 0;
  my $host = "";
  my $statuses = "";
  my $file = "";
  my $msg = "";
  my $source = "";
  my $tmp_str = "";
  my $key = "";
  my $value = "";

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      sleep(1);
      $sleep_counter--;

    # The time has come to check the status warnings
    } else {

      $sleep_counter = $sleep_time;
      %warnings = ();
      %errors = ();
      @files = ();

      $cmd = "ls -1 ".$status_dir;
      $statuses = `$cmd`;
      @files = split(/\n/, $statuses);

      # get the current warnings and errors
      for ($i=0; $i<=$#files; $i++) {
        $file = $files[$i];
        $msg = `tail -n 1 $status_dir/$file`;
        chomp $msg;

        $source = "";
        @arr = ();
        @arr = split(/\./,$file);

        # for pwc, sys and src client errors
        if ($#arr == 2) {
          $source = $arr[1]."_".$arr[0];
        } elsif ($#arr == 1) {
          $source = $arr[0];
        }

        if ($file =~ m/\.warn$/) {
          $warnings{$source} = $msg;
        }
        if ($file =~ m/\.error$/) {
          $errors{$source} = $msg;
        }

      }

      # The results array is now complete, build the response
      # string
      $tmp_str = "";
      while (($key, $value) = each(%warnings)){
        $tmp_str .= $key.":::1:::".$value.";;;";
      }
      while (($key, $value) = each(%errors)){
        $tmp_str .= $key.":::2:::".$value.";;;";
      }
      #$tmp_str .= "\n";

      $status_info = $tmp_str;

      Dada::logMsg(2, DL, "statusInfoThread: ".$status_info);

    }
  }

  Dada::logMsg(1, DL, "statusInfoThread: exiting");
}

#
# Maintains information with clients about DB, Disk and load
#
sub nodeInfoThread() {

  Dada::logMsg(1, DL, "nodeInfoThread: starting");

  my $sleep_time = 4;
  my $sleep_counter = 0;
  my $port = $cfg{"CLIENT_MASTER_PORT"};

  my @machines = ();
  my @results = ();
  my @responses = ();

  my $tmp_str = "";

  # setup the list of machines that we will poll
  for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
    push(@machines, $cfg{"PWC_".$i});
  }
  for ($i=0; $i<$cfg{"NUM_HELP"}; $i++) {
    push(@machines, $cfg{"HELP_".$i});
  }
  push(@machines, "srv0");
  push(@machines, "srv1");

  my $handle = 0;

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      sleep(1);
      $sleep_counter--;
                                                                                                          
    # The time has come to check the status warnings
    } else {
      $sleep_counter = $sleep_time;

      @results = ();
      @responses = ();

      for ($i=0; ((!$quit_daemon) && $i<=$#machines); $i++) {

        $handle = Dada::connectToMachine($machines[$i], $port, 0);
        # ensure our file handle is valid
        if (!$handle) {
          $result = "fail";
          $response = "0 0 0;;;0 0;;;0.00,0.00,0.00;;;0.0";
        } else {
          ($result, $response) = Dada::sendTelnetCommand($handle, "get_status");
          $handle->close();
        }

        $results[$i] = $result;
        $responses[$i] = $response;
      }

      # now set the global string
      $tmp_str = "";
      for ($i=0; $i<=$#machines; $i++) {
        $tmp_str .= $machines[$i].":".$results[$i].":".$responses[$i].";;;;;;";
      }
      $node_info = $tmp_str;

      Dada::logMsg(2, DL, "nodeInfoThread: ".$node_info);

    }
  }  

  Dada::logMsg(1, DL, "nodeInfoThread: exiting");

}

#
# Opens a port to the machine, issues the command and collects the response
#
sub commThread($$$) {

  my ($command, $machine, $port) = @_;

  my $result = "fail";
  my $response = "Failure Message";

  my $handle = Dada::connectToMachine($machine, $port, 0);
  # ensure our file handle is valid
  if (!$handle) {
    # return ("fail","Could not connect to machine ".$machine.":".$port);
    return ("fail","0 0 0;;;0 0;;;0.00,0.00,0.00");
  }

  ($result, $response) = Dada::sendTelnetCommand($handle, $command);

  $handle->close();

  return ($result, $response);

}

#
# Opens a port to the machine, issues the command and collects the response
#
sub getDaemonPID($$) {

  my ($machine, $port) = @_;

  my $d_pid = "P630";

  my $response = "";

  Dada::logMsg(2, DL, "getDaemonPID: ".$machine.":".$port);
  my $handle = Dada::connectToMachine($machine, $port, 0);
  # ensure our file handle is valid
  if ($handle) {

    print $handle "get_pid\r\n";
    Dada::logMsg(2, DL, "getDaemonPID: -> get_pid");
    
    $response = Dada::getLine($handle);  
    Dada::logMsg(2, DL, "getDaemonPID: ".$response);

    $handle->close();

    if ($response ne "") {
      $d_pid = $response;
    }
  }

  Dada::logMsg(2, DL, "getDaemonPID: returning ".$d_pid);
  return $d_pid;

}





