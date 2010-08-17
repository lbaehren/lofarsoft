package Dada::server_web_monitor;

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
  @EXPORT_OK   = qw($dl $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $server_sock;
our $server_host;
our $server_port;
our $warn;
our $error;
our $node_info : shared;
our $curr_obs : shared;
our $status_info : shared;
our $image_info : shared;
our $gain_info : shared;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
%cfg = ();

#
# initialize other variables
#
$warn = ""; 
$error = ""; 
$server_sock = 0;
$server_host = 0;
$server_port = 0;
$quit_daemon = 0;
$node_info = "";
$curr_obs = "";
$status_info = "";
$image_info = "";
$gain_info = "";


use constant STATUS_OK    => 0;
use constant STATUS_WARN  => 1;
use constant STATUS_ERROR => 2;


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
  $server_host = $cfg{"SERVER_HOST"};
  $server_port = $cfg{"SERVER_WEB_MONITOR_PORT"};

  my $control_thread = 0;
  my $node_info_thread = 0;   #  Node Disk space, DB Info, Load info [R]
  my $image_info_thread = 0;  # Current images for web display [L]
  my $curr_info_thread = 0;   # Current source information [L]
  my $status_info_thread = 0; # Current status file information [L]
  my $gain_info_thread = 0;   # Current status file information [L]

  my $result = "";
  my $response = "";
  my $cmd = "";
  my $i = 0;
  my $handle = 0;
  my $rh = "";
  my $string = "";

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
  Dada::logMsg(1, $dl, "Listening on socket: ".$server_host.":".$server_port);

  # start the control thread
  Dada::logMsg(2, $dl ,"starting controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  my $read_set = new IO::Select();  # create handle set for reading
  $read_set->add($server_sock);   # add the main socket to the set

  Dada::logMsg(2, $dl, "Waiting for connection on: ".$server_host.":".$server_port);

  # launch the threads
  $curr_info_thread = threads->new(\&currentInfoThread, $cfg{"SERVER_RESULTS_DIR"});
  $status_info_thread = threads->new(\&statusInfoThread);
  $node_info_thread = threads->new(\&nodeInfoThread);
  $image_info_thread = threads->new(\&imageInfoThread, $cfg{"SERVER_RESULTS_DIR"});
  $gain_info_thread = threads->new(\&gainInfoThread);

  # Sleep for a few seconds to allow the threads to start and collect
  # their first iteration of data
  sleep(3);


  while (!$quit_daemon) {

    # Get all the readable handles from the server
    my ($rh_set) = IO::Select->select($read_set, undef, undef, 2);

    foreach $rh (@$rh_set) {

      # if it is the main socket then we have an incoming connection and
      # we should accept() it and then add the new socket to the $Read_Handles_Object
      if ($rh == $server_sock) { 

        $handle = $rh->accept();
        $handle->autoflush();
        Dada::logMsg(2, $dl, "Accepting connection");

        # Add this read handle to the set
        $read_set->add($handle); 
        $handle = 0;

      } else {

        $string = Dada::getLine($rh);
  
        if (! defined $string) {
          Dada::logMsg(2, $dl, "Closing a connection");
          $read_set->remove($rh);
          close($rh);

        } else {
          Dada::logMsg(2, $dl, "<- '".$string."'");
          my $r = "";

          if ($string eq "node_info") {
            $r = $node_info; 
          } elsif ($string eq "img_info") { 
            $r = $image_info; 
          } elsif ($string eq "curr_obs") { 
            $r = $curr_obs; 
          } elsif ($string eq "status_info") { 
            $r = $status_info; 
          } elsif ($string eq "gain_info") { 
            $r = $gain_info; 
          } else {
            Dada::logMsgWarn($warn, "unexpected command: ".$string);
          } 
          print $rh $r."\r\n";

          if ($dl < 3) {
            $r = substr($r, 0, 80);
          }
          Dada::logMsg(2, $dl, "-> ".$r." ...");
        }
      }
    }
  }

  # Rejoin our daemon control thread
  $control_thread->join();

  # Rejoin other threads
  $curr_info_thread->join();
  $status_info_thread->join();
  $node_info_thread->join();
  $image_info_thread->join();
  $gain_info_thread->join();

  close($server_sock);
                                                                                
  Dada::logMsg(0, $dl, "STOPPING SCRIPT");
                                                                                
  return 0;
}


#
# maintains the curr_info string
#
sub currentInfoThread($) {

  (my $results_dir) = @_;

  Dada::logMsg(1, $dl, "currentInfoThread: starting");

  my $sleep_time = 4;
  my $sleep_counter = 0;

  my $cmd = "";
  my $obs = "";
  my $tmp_str = "";
  my $source = "";
  my %cfg_file = ();
  my $result = "";
  my $response = "";
  my $P0 = "unknown";
  my $DM = "unknown";
  my $integrated = 0;
  my $snr = 0;

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      $sleep_counter--;
      sleep(1);
    
    } else {
      $sleep_counter = $sleep_time;

      $cmd = "find ".$results_dir." -maxdepth 1 -type d -name '2*' -printf '\%f\n' | sort | tail -n 1";
      Dada::logMsg(3, $dl, "currentInfoThread: ".$cmd);
      $obs = `$cmd`;
      chomp $obs ;
      Dada::logMsg(3, $dl, "currentInfoThread: ".$obs);

      if (-f $results_dir."/".$obs."/obs.info") {

        $tmp_str = "";
        %cfg_file = Dada::readCFGFile($results_dir."/".$obs."/obs.info"); 

        # Determine the P0 for this source
        $P0 = Dada::getPeriod($cfg_file{"SOURCE"});

        # Determine the DM of this source
        $DM = Dada::getDM($cfg_file{"SOURCE"});
  
        # Determine how much data has been intergrated so far
        $integrated = 0;
        $source = $cfg_file{"SOURCE"};
        $source =~ s/^[JB]//;
        #$source =~ s/[a-zA-Z]*$//;

        Dada::logMsg(2, $dl, "currentInfoThread: [".$results_dir."/".$obs."/".$source."_t.ar]");

        if (-f $results_dir."/".$obs."/".$source."_t.ar") {
          $cmd = "vap -c length -n ".$results_dir."/".$obs."/".$source."_t.ar | awk '{print \$2}'";
          Dada::logMsg(2, $dl, "currentInfoThread: ".$cmd);
          ($result, $response) = Dada::mySystem($cmd);
          Dada::logMsg(2, $dl, "currentInfoThread: ".$result." ".$response);
          chomp $response;
          if ($result eq "ok") {
            $integrated = sprintf("%5.1f",$response);
          }
        }

        Dada::logMsg(2, $dl, "currentInfoThread: [".$results_dir."/".$obs."/".$source."_f.ar]");
        if (-f $results_dir."/".$obs."/".$source."_f.ar") {
          $cmd = "psrstat -j 'zap median' -j FTp -qc snr ".$results_dir."/".$obs."/".$source."_f.ar 2>&1 | grep snr= | awk -F= '{print \$2}'";
          Dada::logMsg(2, $dl, "currentInfoThread: ".$cmd);
          ($result, $response) = Dada::mySystem($cmd);
          Dada::logMsg(2, $dl, "currentInfoThread: ".$result." ".$response);
          chomp $response;
          if ($result eq "ok") {
            $snr = sprintf("%5.1f",$response);
          }
        }

        $tmp_str .= "SOURCE:::".$cfg_file{"SOURCE"}.";;;";
        $tmp_str .= "RA:::".$cfg_file{"RA"}.";;;";
        $tmp_str .= "DEC:::".$cfg_file{"DEC"}.";;;";
        $tmp_str .= "CFREQ:::".$cfg_file{"CFREQ"}.";;;";
        $tmp_str .= "BANDWIDTH:::".$cfg_file{"BANDWIDTH"}.";;;";
        $tmp_str .= "NUM_PWC:::".$cfg_file{"NUM_PWC"}.";;;";
        $tmp_str .= "NPOL:::".$cfg_file{"NPOL"}.";;;";
        $tmp_str .= "NBIT:::".$cfg_file{"NBIT"}.";;;";
        $tmp_str .= "PID:::".$cfg_file{"PID"}.";;;";
        $tmp_str .= "UTC_START:::".$obs.";;;";
        $tmp_str .= "P0:::".$P0.";;;";
        $tmp_str .= "DM:::".$DM.";;;";
        $tmp_str .= "INTEGRATED:::".$integrated.";;;";
        $tmp_str .= "SNR:::".$snr.";;;";
        $tmp_str .= "CONFIG:::".$cfg_file{"CONFIG"}.";;;";
        $tmp_str .= "PROC_FILE:::".$cfg_file{"PROC_FILE"}.";;;";
 
        # update the global variable 
        $curr_obs = $tmp_str;

        Dada::logMsg(3, $dl, "currInfoThread: ".$curr_obs);
        
      }
    }
  }

  Dada::logMsg(1, $dl, "currentInfoThread: exiting");

}


#
# Maintains a listing of the current images
#
sub imageInfoThread($) {

  my ($results_dir) = @_;

  Dada::logMsg(1, $dl, "imageInfoThread: starting");

  my $sleep_time = 4;
  my $sleep_counter = 0;

  my $cmd = "";
  my $image_string = "";
  my @images = ();
  my @image_types = qw(bp ts fft dts pvf);
  my $obs = "";
  my $tmp_str = "";

  my $i = 0;
  my $k = 0;
  my $img = "";
  my $src = "";
  my %pvfl_lo = ();
  my %pvfl_hi = ();
  my %pvfr_lo = ();
  my %pvfr_hi = ();
  my %pvt_lo = ();
  my %pvt_hi = ();
  my @keys = ();
  my %srcs = ();
  my @parts = ();

  chdir $results_dir;

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      $sleep_counter--;
      sleep(1);
    } else {

      $sleep_counter = $sleep_time;

      $cmd = "find . -maxdepth 1 -type d -name '2*' -printf '\%f\n' | sort | tail -n 1";
      $obs = `$cmd`;
      chomp $obs;

      $cmd = "find ".$obs." -name '*.png' | sort";
      $image_string = `$cmd`;
      @images = split(/\n/, $image_string);

      %pvfl_lo = ();
      %pvfl_hi = ();
      %pvfr_lo = ();
      %pvfr_hi = ();
      %pvt_lo = ();
      %pvt_hi = ();
      %srcs = ();

      for ($i=0; $i<=$#images; $i++) {
        $img = $images[$i];
        @parts = split(/_/,$img);
        $src = $parts[3];
        $srcs{$src} = 1;

        if (($img =~ m/phase_vs_flux/) && ($img =~ m/240x180/)) { $pvfl_lo{$src} = $img; }
        if (($img =~ m/phase_vs_flux/) && ($img =~ m/1024x768/)) { $pvfl_hi{$src} = $img; }
        if (($img =~ m/phase_vs_freq/) && ($img =~ m/240x180/)) { $pvfr_lo{$src} = $img; }
        if (($img =~ m/phase_vs_freq/) && ($img =~ m/1024x768/)) { $pvfr_hi{$src} = $img; }
        if (($img =~ m/phase_vs_time/) && ($img =~ m/240x180/)) { $pvt_lo{$src} = $img; }
        if (($img =~ m/phase_vs_time/) && ($img =~ m/1024x768/)) { $pvt_hi{$src} = $img; }
      }

      @keys = ();
      @keys = sort keys %srcs;
      for ($i=0; $i<=$#keys; $i++) {
        $k = $keys[$i];
        if (! exists $pvfl_lo{$k}) { $pvfl_lo{$k} = "../../images/blankimage.gif"; }
        if (! exists $pvfl_hi{$k}) { $pvfl_hi{$k} = "../../images/blankimage.gif"; }
        if (! exists $pvfr_lo{$k}) { $pvfr_lo{$k} = "../../images/blankimage.gif"; }
        if (! exists $pvfr_hi{$k}) { $pvfr_hi{$k} = "../../images/blankimage.gif"; }
        if (! exists $pvt_lo{$k})  { $pvt_lo{$k} = "../../images/blankimage.gif"; }
        if (! exists $pvt_hi{$k})  { $pvt_hi{$k} = "../../images/blankimage.gif"; }
      }

      # now update the global variables for each image type
      $tmp_str = "utc_start:::".$obs.";;;";
      $tmp_str .= "npsrs:::".($#keys+1).";;;";
      for ($i=0; $i<=$#keys; $i++) {
        $k = $keys[$i];
        $tmp_str .= "psr".$i.":::".$k.";;;";
        $tmp_str .= "pvfl_240x180:::".$pvfl_lo{$k}.";;;";
        $tmp_str .= "pvt_240x180:::".$pvt_lo{$k}.";;;";
        $tmp_str .= "pvfr_240x180:::".$pvfr_lo{$k}.";;;";
        $tmp_str .= "pvfl_1024x768:::".$pvfl_hi{$k}.";;;";
        $tmp_str .= "pvt_1024x768:::".$pvt_hi{$k}.";;;";
        $tmp_str .= "pvfr_1024x768:::".$pvfr_hi{$k}.";;;";
      }

      $image_info = $tmp_str;
      Dada::logMsg(3, $dl, "imageInfoThread: ".$tmp_str);
    }
  }

  Dada::logMsg(1, $dl, "imageInfoThread: exiting");

}


#
# Monitors the STATUS_DIR for warnings and errors
#
sub statusInfoThread() {

  my $status_dir = $cfg{"STATUS_DIR"};

  Dada::logMsg(1, $dl, "statusInfoThread: starting");

  my @server_daemons = split(/ /,$cfg{"SERVER_DAEMONS"});
  my %clients = ();
  my @files = ();
  my %warnings= ();
  my %errors= ();
  my @arr = ();
  my $sleep_time = 2;
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
  my $cmd = "";

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

      Dada::logMsg(2, $dl, "statusInfoThread: ".$status_info);

    }
  }

  Dada::logMsg(1, $dl, "statusInfoThread: exiting");
}

#
# Maintains information with clients about DB, Disk and load
#
sub nodeInfoThread() {

  Dada::logMsg(1, $dl, "nodeInfoThread: starting");

  my $sleep_time = 4;
  my $sleep_counter = 0;
  my $port = $cfg{"CLIENT_MASTER_PORT"};

  my @machines = ();
  my @results = ();
  my @responses = ();
  my $result = "";
  my $response = "";
  my $tmp_str = "";
  my $i = 0;

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

      for ($i=0; $i<=$#machines; $i++) {

        $handle = Dada::connectToMachine($machines[$i], $port, 0);
        # ensure our file handle is valid
        if (!$handle) {

          $response = "0 0 0;;;0 0;;;0.00,0.00,0.00;;;0.0;;;0.0";

          # check if offline, or scripts not running
          $handle = Dada::connectToMachine($machines[$i], 22, 0);
          if (!$handle) {
            $result = "offline";
          } else {
            $result = "stopped";
            $handle->close();
          }
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

      Dada::logMsg(3, $dl, "nodeInfoThread: ".$node_info);

    }
  }  

  Dada::logMsg(1, $dl, "nodeInfoThread: exiting");

}

sub gainInfoThread() {

  Dada::logMsg(1, $dl, "gainInfoThread: starting");
                                                                                                                                                                          
  my $sleep_time = 4;
  my $sleep_counter = 0;
  my $host = $cfg{"SERVER_HOST"};
  my $port = $cfg{"SERVER_GAIN_REPORT_PORT"};
  my $handle = 0;
  my $result = "";
  my $response = ""; 

  while (!$quit_daemon) {

    if ($sleep_counter > 0) {
      sleep(1);
      $sleep_counter--;

    # The time has come to check the status warnings
    } else {
      $sleep_counter = $sleep_time;

      Dada::logMsg(2, $dl, "gainInfoThread: connecting to ".$host.":".$port);
      $handle = Dada::connectToMachine($host, $port, 0);

      # ensure our file handle is valid
      if (!$handle) {
        $result = "fail";
      } else {
        Dada::logMsg(2, $dl, "gainInfoThread: -> REPORT GAINS");
        print $handle "REPORT GAINS\r\n";
        $response = Dada::getLine($handle);
        if ($response) {
          Dada::logMsg(2, $dl, "gainInfoThread: <- ".$response);
          $result = "ok";
        } else {
          $result = "fail";
        }
        $handle->close();
      } 

      if ($result ne "ok") {
        $response = "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50";
      } else {
        chomp $response;
      }
      Dada::logMsg(2, $dl, "gainInfoThread: ".$response);
      $gain_info = $response;
    }
  }

  Dada::logMsg(1, $dl, "gainInfoThread: exiting");

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

  $server_sock = new IO::Socket::INET (
    LocalHost => $server_host,
    LocalPort => $server_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$server_sock) {
    return ("fail", "Could not create listening socket: ".$server_host.":".$server_port);
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
