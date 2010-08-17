#!/usr/bin/env perl

###############################################################################
#
# pwc_control.pl
#
# This script will act on specific textual commands recieved on a 
# pre-determined port. If an action is required, then the script will attempt 
# to perform that  action and provide a response as well


use lib $ENV{"DADA_ROOT"}."/bin";

use strict;         # strict mode (like -Wall)
use POSIX qw(setsid);
use IO::Socket;     # Standard perl socket library
use Net::hostent;   # To determine the hostname of the server
use Switch;
use Apsr;           # APSR/DADA Module for configuration options, includes Dada.pm


#
# Constants
#
use constant RUN_SYS_COMMANDS => "true";
use constant DEBUG_LEVEL      => 1;              # 0 == no debug
use constant PIDFILE          => "apsr_master_control.pid";
use constant LOGFILE          => "apsr_master_control.log";


#
# Global Variables
#
our $quit = 0;
our %cfg = Apsr::getApsrConfig();

our @daemons = split(/ /,$cfg{"CLIENT_DAEMONS"});
our @helper_daemons = split(/ /,$cfg{"HELPER_DAEMONS"});
our $am_pwc = 0;

#
# Local Variables
#
my $logfile = $cfg{"CLIENT_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"CLIENT_CONTROL_DIR"}."/".PIDFILE;
my $localhost = Dada::getHostMachineName();
my $val;

setupDirectories();

# Turn the script into a daemon
Dada::daemonize($logfile, $pidfile);

# Check if a client master controller is already running
my $cmd = "ps aux | grep $0";

my $server = new IO::Socket::INET (
    LocalHost => $localhost,
    LocalPort => $cfg{"CLIENT_MASTER_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);

# determine if I am a PWC 
my $i= 0;
for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
  if ($cfg{"PWC_".$i} eq $localhost) {
    $am_pwc = 1;
  }
}



die "Could not create listening socket: $!\n" unless $server;

my $handler;
my $line = "";
my $rVal;
my $rString;

logMessage(0, "STARTING SCRIPT");

while (!$quit) {

  $handler = "Null";

  logMessage(2, "Waiting for connection ".$localhost.":".$cfg{"CLIENT_MASTER_PORT"});

  # Listen for a connection...
  $handler = $server->accept() or die "accept $!\n";
  $handler->autoflush(1);

  my $peeraddr = $handler->peeraddr;
  my $hostinfo = gethostbyaddr($peeraddr);
  logMessage(2, "Accepting connection from ".$hostinfo->name);
  logMessage(3, "Waiting for command string");

  my $command = <$handler>;
  $/="\r\n";
  chomp($command);
  $/="\n";

  my $result = "fail";
  my $response = "";

  ($result, $response) = handleCommand($command);
  chomp $response;

  if ($result ne "ok") {
    logMessage(0, "command: ".$command);
    logMessage(0, "result: ".$result.", response: ".$response);
  } else {
    if (!($command =~ m/get_status/)) {
      logMessage(1, $command." -> ".$result.":".$response);
    }
  }

  sendReplyToServer($result,$response,$handler,$hostinfo->name);
  
  logMessage(2, "Closing Connection"); 

  $handler->close;

}

logMessage(0, "STOPPING SCRIPT");

exit 0;

sub sendReplyToServer($$$$) {

  (my $result, my $response, my $handler,my $hostname) = @_;

  if ($response ne "") {
    print $handler $response."\r\n";
  }
  print $handler $result."\r\n";

}

sub handleCommand($) {

  (my $string) = @_;

  logMessage(2, "Command = \"".$string."\"");

  my @commands = split(/ /,$string, 2);

  my $current_binary_dir = Dada::getCurrentBinaryVersion();
  my $cmd = "";
  my $result = "";
  my $response = "";

  
  if ($commands[0] eq "clean_scratch") {
    $cmd = "rm -rf ".$cfg{"CLIENT_SCRATCH_DIR"}."/*";
    if (-d $cfg{"CLIENT_SCRATCH_DIR"}) {
      ($result,$response) = mysystem($cmd, 0);  
    } else {
      $result = "fail";
      $response = "Dir \"".$cfg{"CLIENT_SCRATCH_DIR"}."\" did not exist";
    }
  }

  elsif ($commands[0] eq "clean_archives") {
    $cmd = "rm -rf ".$cfg{"CLIENT_ARCHIVE_DIR"}."/*";
    if (-d $cfg{"CLIENT_ARCHIVE_DIR"}) {
      ($result,$response) = mysystem($cmd, 0);
    } else {
      $result = "fail";
      $response = "Dir \"".$cfg{"CLIENT_ARCHIVE_DIR"}."\" did not exist";
    }
  }

  elsif ($commands[0] eq "clean_rawdata") {
    $cmd = "rm -rf ".$cfg{"CLIENT_RECORDING_DIR"}."/*";
    if (-d $cfg{"CLIENT_RECORDING_DIR"}) {
      ($result,$response) = mysystem($cmd, 0);
    } else {
      $result = "fail";
      $response = "Dir \"".$cfg{"CLIENT_RECORDING_DIR"}."\" did not exist";
    }
  }


  elsif ($commands[0] eq "clean_logs") {
    $cmd = "rm -rf ".$cfg{"CLIENT_LOG_DIR"}."/*";
    if (-d $cfg{"CLIENT_LOG_DIR"}) {
      ($result,$response) = mysystem($cmd, 0);
    } else {
      $result = "fail";
      $response = "Dir \"".$cfg{"CLIENT_LOG_DIR"}."\" did not exist";
    }
    ($result,$response) = mysystem($cmd, 0);
  }

  elsif ($commands[0] eq "stop_pwcs") {
    $cmd = "killall ".$cfg{"PWC_BINARY"};
    ($result,$response) = mysystem($cmd, 0);  
  }

  elsif ($commands[0] eq "stop_pwc") {
    $cmd = "killall -KILL ".$commands[1];
    ($result,$response) = mysystem($cmd, 0);  
  }

  elsif ($commands[0] eq "stop_dfbs") {
    $cmd = "killall -KILL ".$cfg{"DFB_SIM_BINARY"};
    ($result,$response) = mysystem($cmd, 0);
  }

  elsif ($commands[0] eq "stop_srcs") {
    $cmd = "killall dada_dbdisk dspsr dada_dbnic";
    ($result,$response) = mysystem($cmd, 0);  
  }

  elsif ($commands[0] eq "kill_process") {
    ($result,$response) = Dada::killProcess($commands[1]);
  }

  elsif ($commands[0] eq "start_bin") {
    $cmd = $current_binary_dir."/".$commands[1];
    ($result,$response) = mysystem($cmd, 0);  
  }

  elsif ($commands[0] eq "start_pwcs") {
    $cmd = $current_binary_dir."/".$cfg{"PWC_BINARY"}." -d -k dada -p ".$cfg{"CLIENT_UDPDB_PORT"}." -c ".$cfg{"PWC_PORT"}." -l ".$cfg{"PWC_LOGPORT"};
    ($result,$response) = mysystem($cmd, 0);
  }
  
  elsif ($commands[0] eq "set_bin_dir") {
    ($result, $response) = Dada::setBinaryDir($commands[1]);
  }
                                                                                                                                                      
  elsif ($commands[0] eq "get_bin_dir") {
    ($result, $response) = Dada::getCurrentBinaryVersion();
  }
                                                                                                                                                      
  elsif ($commands[0] eq "get_bin_dirs") {
    ($result, $response) = Dada::getAvailableBinaryVersions();
  }
                                                                                                                                                      
  elsif ($commands[0] eq "destroy_db") {

    my $temp_result = "";
    my $temp_response = "";

    $result = "ok";
    $response = "";

    my @datablocks = split(/ /,$cfg{"DATA_BLOCKS"});
    my $db;

    foreach $db (@datablocks) {

      if ( (defined $cfg{$db."_BLOCK_BUFSZ"}) && (defined $cfg{$db."_BLOCK_NBUFS"}) ) {
    
        # Create the dada data block
        $cmd = "sudo ".$current_binary_dir."/dada_db -d -k ".lc($db);
        ($temp_result,$temp_response) = mysystem($cmd, 0);
   
      } else {
  
        $temp_result = "fail";
        $temp_response = "config file configuration error";
 
      }

      if ($temp_result eq "fail") {
        $result = "fail";
      }
      $response = $response.$temp_response;

    }

  }

  elsif ($commands[0] eq "init_db") {

    my $temp_result = "";
    my $temp_response = "";

    $result = "ok";
    $response = "";

    my @datablocks = split(/ /,$cfg{"DATA_BLOCKS"});
    my $db;

    foreach $db (@datablocks) {

      if ( (defined $cfg{$db."_BLOCK_BUFSZ"}) && (defined $cfg{$db."_BLOCK_NBUFS"}) ) {

        # Create the dada data block
        $cmd = "sudo ".$current_binary_dir."/dada_db -l -k ".lc($db)." -b ".$cfg{$db."_BLOCK_BUFSZ"}." -n ".$cfg{$db."_BLOCK_NBUFS"};
        ($temp_result,$temp_response) = mysystem($cmd, 0);

      } else {

        $temp_result = "fail";
        $temp_response = "config file configuration error";

      }        

      if ($temp_result eq "fail") {
        $result = "fail";
      }
      $response = $response.$temp_response;

    }

  }
  
  elsif ($commands[0] eq "stop_daemon") {
    if ($commands[1] eq "apsr_master_control") {
      $quit = 1;
      $result = "ok";
      $response = "";
    } else {
      ($result,$response) = stopDaemon($commands[1], 10);
    }
  }

  elsif ($commands[0] eq "stop_daemons") {
    ($result,$response) = stopDaemons();
  }

  elsif ($commands[0] eq "daemon_info") {
    ($result,$response) = getDaemonInfo();
  }

  elsif ($commands[0] eq "start_daemon") {
    $cmd = "client_".$commands[1].".pl";
    ($result, $response) = mysystem($cmd, 0);
  }

  elsif ($commands[0] eq "start_daemons") {
    chdir($cfg{"SCRIPTS_DIR"});

    $result = "ok";
    $response = "ok";
 
    my $daemon_result = "";
    my $daemon_response = "";

    my $cmd = "";
    my $daemon;

    foreach $daemon (@daemons) {

      $cmd = "./client_".$daemon.".pl";
      ($daemon_result, $daemon_response) = mysystem($cmd, 0);

      if ($daemon_result eq "fail") {
        $result = "fail";
        $response .= $daemon_response;
      }
    }
  }

  elsif ($commands[0] eq "start_helper_daemons") {

    chdir($cfg{"SCRIPTS_DIR"});

    $result = "ok";
    $response = "ok";

    my $daemon_result = "";
    my $daemon_response = "";

    my $cmd = "";
    my $daemon;

    foreach $daemon (@helper_daemons) {

      $cmd = "./client_".$daemon.".pl";
      ($daemon_result, $daemon_response) = mysystem($cmd, 0);

      if ($daemon_result eq "fail") {
        $result = "fail";
        $response .= $daemon_response;
      }
    }
  }

  elsif ($commands[0] eq "dfbsimulator") {
    $cmd = $current_binary_dir."/".$cfg{"DFB_SIM_BINARY"}." ".$commands[1];
    ($result,$response) = mysystem($cmd, 0);
  }

  elsif ($commands[0] eq "system") {
    $cmd = $commands[1];
    ($result,$response) = mysystem($cmd, 0);  
  }

  elsif ($commands[0] eq "get_disk_info") {
    ($result,$response) = Dada::getDiskInfo($cfg{"CLIENT_RECORDING_DIR"});
  }

  elsif ($commands[0] eq "get_db_info") {
    ($result,$response) = Dada::getDBInfo(lc($cfg{"PROCESSING_DATA_BLOCK"}));
  }

  elsif ($commands[0] eq "db_info") {
     ($result,$response) = Dada::getDBInfo(lc($commands[1]));
  }

  elsif ($commands[0] eq "get_alldb_info") {
    ($result,$response) = Dada::getAllDBInfo($cfg{"DATA_BLOCKS"});
  }

  elsif ($commands[0] eq "get_db_xfer_info") {
     ($result,$response) = Dada::getXferInfo();
  }

  elsif ($commands[0] eq "get_load_info") {
    ($result,$response) = Dada::getLoadInfo();
  }

  elsif ($commands[0] eq "set_udp_buffersize") {
    $cmd = "sudo /sbin/sysctl -w net.core.wmem_max=67108864";
    ($result,$response) = mysystem($cmd, 0);
    if ($result eq "ok") {
      $cmd = "sudo /sbin/sysctl -w net.core.rmem_max=67108864";
      ($result,$response) = mysystem($cmd, 0);
    }
  }

  elsif ($commands[0] eq "get_all_status") {
    my $subresult = "";
    my $subresponse = "";

    ($result,$subresponse) = Dada::getDiskInfo($cfg{"CLIENT_RECORDING_DIR"});
    $response = "DISK_INFO:".$subresponse."\n";
    
    ($subresult,$subresponse) = Dada::getALLDBInfo($cfg{"DATA_BLOCKS"});
    $response .= "DB_INFO:".$subresponse."\n";
    if ($subresult eq "fail") {
      $result = "fail";
    }

    ($subresult,$subresponse) = Dada::getLoadInfo();
    $response .= "LOAD_INFO:".$subresponse;
    if ($subresult eq "fail") {
      $result = "fail";
    }

  }

  elsif($commands[0] eq "get_status") {
    my $subresult = "";
    my $subresponse = "";
                                                                                                                                                               
    ($result,$subresponse) = Dada::getRawDisk($cfg{"CLIENT_RECORDING_DIR"});
    $response = $subresponse.";;;";

    ($subresult,$subresponse) = Dada::getAllDBInfo(lc($cfg{"PROCESSING_DATA_BLOCK"}));
    $response .= $subresponse.";;;";
    if (($subresult eq "fail") && ($am_pwc == 1)) {
      $result = "fail";
    }

    ($subresult,$subresponse) = Dada::getLoadInfo();
    $response .= $subresponse.";;;";
    if ($subresult eq "fail") {
      $result = "fail";
    }

    ($subresult,$subresponse) = Dada::getUnprocessedFiles($cfg{"CLIENT_RECORDING_DIR"});
    $response .= $subresponse.";;;";
    if ($subresult eq "fail") {
      $result = "fail";
    }

    ($subresult,$subresponse) = Dada::getTempInfo();
    $response .= $subresponse;
    if ($subresult eq "fail") {
      $result = "fail";
    }

  }

  elsif ($commands[0] eq "stop_master_script") {
    $quit = 1;
    $result = "ok";
    $response = "";
  }

  elsif ($commands[0] eq "help") {
    $response .= "Available Commands:\n";
    $response .= "clean_scratch      delete all data in ".$cfg{"CLIENT_SCRATCH_DIR"}."\n";
    $response .= "clean_archives     delete all temporary archives in ".$cfg{"CLIENT_ARCHIVES_DIR"}."\n";
    $response .= "clean_rawdata      delete all raw data files in ".$cfg{"CLIENT_RECORDING_DIR"}."\n";
    $response .= "clean_logs         delete all logfiles in ".$cfg{"CLIENT_LOGS_DIR"}."\n";
    $response .= "start_pwcs         runs ".$cfg{"PWC_BINARY"}." -d -p ".$cfg{"CLIENT_UDPDB_PORT"}."\n";
    $response .= "stop_pwcs          send a kill signal to all PWCS\n";
    $response .= "stop_pwc process   send a kill signal to the named process\n";
    $response .= "stop_srcs          send a kill signal to all SRCS [dada_dbdisk dspsr dada_dbnic]\n";
    $response .= "stop_dfbs          kill the dfb simulator [".$cfg{"DFB_SIM_BINARY"}."]\n";
    $response .= "start_bin cmdline  runs the \"cmdline\" binary file from the current bin dir\n";
    $response .= "set_bin_dir dir    sets the current binary directory to \"dir\"\n";
    $response .= "get_bin_dir        returns the current binary directory\n";
    $response .= "get_bin_dirs       returns a list of all valid binary directories\n";
    $response .= "destroy_db         attempts to destroy the data block\n";
    $response .= "init_db            attempts to create a data block with ".$cfg{"DATA_BLOCK_NBUFS"}." nbufs of ".$cfg{"DATA_BLOCK_BUFSZ"}."bytes each\n";
    $response .= "stop_daemons       send a kill signal to the various agent daemons\n";
    $response .= "start_daemons      starts the various agent daemons\n";
    $response .= "system cmdline     runs the command \"cmdline\" in a tcsh shell\n";
    $response .= "get_disk_info      returns capacity information about the recording disk\n";
    $response .= "get_db_info        returns capacity information about the data block\n";
    $response .= "get_alldb_info     returns num_blocks and num_full for each
data block\n";
    $response .= "get_db_xfer_info   returns information about current xfers in the data block\n";
    $response .= "get_load_info      returns capacity information about the data block\n";
    $response .= "get_all_status     returns disk, data block and load information\n";
    $response .= "set_udp_buffersize sets the kernels udp buffer size to 67108864 bytes\n";
    $response .= "help               print this help listing";
    $response .= "stop_master_script exit this daemon!";
    $result = "ok";
  }

  else {
    $result = "fail";
    $response = "Unrecognized command ".$commands[0];
  } 

  return ($result,$response);

}

sub mysystem($$) {

  (my $cmd, my $background=0) = @_;

  my $rVal = 0;
  my $result = "ok";
  my $response = "";
  my $realcmd = $cmd." 2>&1";

  if ($background) { $realcmd .= " &"; }

  if (RUN_SYS_COMMANDS eq "true") {

    logMessage(2, "About to run ".$realcmd);
    $response = `$realcmd`;
    $rVal = $?;
    $/ = "\n";
    chomp $rString;
  } else {
    logMessage(0, "CMD = ".$realcmd);
  }

  # If the command failed
  if ($rVal != 0) {
    $result = "fail";
  }
  logMessage(2, "response = $response");

  return ($result,$response);
                                                                                                                          
}


#
# stops the specified client daemon, optionally kills it after a period
#
sub stopDaemon($;$) {

  (my $daemon, my $timeout=10) = @_;

  my $pid_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".pid";
  my $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".quit";
  my $script = "client_".$daemon.".pl";

  my $ps_cmd = "ps auxwww | grep apsr | grep '".$script."' | grep -v grep";

  $cmd = "touch ".$quit_file;
  system($cmd);

  my $counter = $timeout;
  my $running = 1;

  while ($running && ($counter > 0)) {
    `$ps_cmd`;
    if ($? == 0) {
      logMessage(0, "daemon ".$daemon." still running");
    } else {
      $running = 0;
    }
    $counter--;
    sleep(1);
  }

  my $result = "ok";
  my $response = "daemon exited";

  # If the daemon is still running after the timeout, kill it
  if ($running) {
    ($result, $response) = Dada::killProcess($daemon);
    $response = "daemon had to be killed";
    $result = "fail";
  }

  if (unlink($quit_file) != 1) {
    $response = "Could not unlink the control file ".$quit_file;
    $result = "fail";
    logMessage(0, "Error: ".$response);
  }

  return ($result, $response);
}



sub stopDaemons() {

  my $allStopped = "false";
  my $threshold = 20; # seconds
  my $daemon = "";
  my $allStopped = "false";
  my $result = "";
  my $response = "";
  my $quit_file = "";
  my $cmd = "";
  my $message = "";

  # Touch the quit files for each daemon
  foreach $daemon (@daemons) {

    $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".quit";
    $cmd = "touch ".$quit_file;
    system($cmd);

  }

  while (($allStopped eq "false") && ($threshold > 0)) {

    $allStopped = "true";
    foreach $daemon (@daemons) {
      my $cmd = "ps aux | grep apsr| grep 'client_".$daemon.".pl' | grep -v grep";
      `$cmd`;
      if ($? == 0) {
        logMessage(1, $daemon." is still running");
        $allStopped = "false";
        if ($threshold < 10) {
          ($result, $response) = Dada::killProcess($daemon);
        }
      }
      
    }
    $threshold--;
    sleep(1);
  }

  # Clean up the quit files
  foreach $daemon (@daemons) {
    $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".quit";
    unlink($quit_file);
  }

  # If we had to resort to a "kill", send an warning message back
  if (($threshold > 0) && ($threshold < 10)) {
    $message = "Daemons did not exit cleanly within ".$threshold." seconds, a KILL signal was used and they exited";
    logMessage(0, "Error: ".$message);
    return ("fail", $message);
  } 

  if ($threshold <= 0) {
    $message = "Daemons did not exit cleanly after ".$threshold." seconds, a KILL signal was used and they exited";
    logMessage(0, "Error: ".$message);
    return ("fail", $message);
  }

  return ("ok", "Daemons exited correctly");

}


sub setupDirectories() {

  if (($cfg{"CLIENT_SCRATCH_DIR"}) && (! -d $cfg{"CLIENT_SCRATCH_DIR"})) {
    `mkdir -p $cfg{"CLIENT_SCRATCH_DIR"}`;
  }
  if (($cfg{"CLIENT_CONTROL_DIR"}) && (! -d $cfg{"CLIENT_CONTROL_DIR"})) {
    `mkdir -p $cfg{"CLIENT_CONTROL_DIR"}`;
  }
  if (($cfg{"CLIENT_RECORDING_DIR"}) && (! -d $cfg{"CLIENT_RECORDING_DIR"})) {
    `mkdir -p $cfg{"CLIENT_RECORDING_DIR"}`;
  }
  if (($cfg{"CLIENT_LOG_DIR"}) && (! -d $cfg{"CLIENT_LOG_DIR"})) {
    `mkdir -p $cfg{"CLIENT_LOG_DIR"}`;
  }
  if (($cfg{"CLIENT_ARCHIVE_DIR"}) && (! -d $cfg{"CLIENT_ARCHIVE_DIR"})) {
    `mkdir -p $cfg{"CLIENT_ARCHIVE_DIR"}`;
  }

}

sub logMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    print "[".Dada::getCurrentDadaTime()."] ".$message."\n";
  }
}



sub getDaemonInfo() {

  my $control_dir = $cfg{"CLIENT_CONTROL_DIR"}; 
  my $daemon;
  my $perl_daemon;
  my $daemon_pid_file;
  my $cmd;
  my %array = ();

  foreach $daemon (@daemons) {
    $perl_daemon = "client_".$daemon.".pl";
    $daemon_pid_file = $daemon.".pid";

    # Check to see if the process is running
    $cmd = "ps aux | grep ".$perl_daemon." | grep -v grep > /dev/null";
    logMessage(2, $cmd);
    `$cmd`;
    if ($? == 0) {
      $array{$daemon} = 1;
    } else {
      $array{$daemon} = 0;
    }

    # check to see if the PID file exists
    if (-f $control_dir."/".$daemon_pid_file) {
      $array{$daemon}++;
    }
  }

  # Add the PWC as a daemon
  $cmd = "ps aux | grep ".$cfg{"PWC_BINARY"}." | grep -v grep > /dev/null";
  logMessage(2, $cmd);
  `$cmd`;
  if ($? == 0) {
    $array{$cfg{"PWC_BINARY"}} = 2;
  } else {
    $array{$cfg{"PWC_BINARY"}} = 0;
  }

  my $i=0;
  my $result = "ok";
  my $response = "";

  my @keys = sort (keys %array);
  for ($i=0; $i<=$#keys; $i++) {
    if ($array{$keys[$i]} != 2) {
      $result = "fail";
    }
    $response .= $keys[$i]." ".$array{$keys[$i]}.",";
  }


  return ($result, $response);

}
                                                                                                   


