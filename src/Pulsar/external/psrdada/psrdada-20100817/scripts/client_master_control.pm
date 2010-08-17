package Dada::client_master_control;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use IO::Socket;
use IO::Select;
use Net::hostent;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

  # your exported package globals go here,
  # as well as any optionally exported functions
  @EXPORT_OK   = qw($dl $daemon_name $pwc_add %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our $pwc_add;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon;
our $host;
our $port;
our $sock;
our @daemons;
our @helper_daemons;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
$pwc_add = "";
%cfg = ();

#
# initialize other variables
#
$quit_daemon = 0;
$host = "";
$port = 0;
$sock = 0;
@daemons = ();
@helper_daemons = ();


###############################################################################
#
# package functions
# 

sub main() {

  my $log_file       = $cfg{"CLIENT_LOG_DIR"}."/".$daemon_name.".log";;
  my $pid_file       = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $archive_dir    = $cfg{"CLIENT_ARCHIVE_DIR"};   # hi res archive storage
  my $results_dir    = $cfg{"CLIENT_RESULTS_DIR"};   # dspsr output directory

  $host = Dada::getHostMachineName();
  $port = $cfg{"CLIENT_MASTER_PORT"};
  $sock = 0;

  @daemons = split(/ /,$cfg{"CLIENT_DAEMONS"});
  if (defined $cfg{"HELPER_DAEMONS"}) {
    @helper_daemons = split(/ /,$cfg{"HELPER_DAEMONS"});
  }

  my $result = "";
  my $response = "";

  # sanity check on whether the module is good to go
  ($result, $response) = good();
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;

  # become a daemon
  Dada::daemonize($log_file, $pid_file);

  logMsg(0, "STARTING SCRIPT");
 
  my $read_set = new IO::Select();
  my $rh = 0;
  my $handle = 0;
  my $peeraddr = "";
  my $hostinfo = "";
  my $hostname = "";
  my $command = "";

  $read_set->add($sock);

  while (!$quit_daemon) {

   # Get all the readable handles from the server
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 1);

    foreach $rh (@$readable_handles) {

      if ($rh == $sock) {

        $handle = $rh->accept();
        $handle->autoflush(1);
        $read_set->add($handle);
  
        # Get information about the connecting machine
        $peeraddr = $handle->peeraddr;
        $hostinfo = gethostbyaddr($peeraddr);
        $hostname = $hostinfo->name;
  
        logMsg(2, "Accepting connection from ".$hostname);
        $handle = 0; 
     
      } else {

        $command = <$rh>;

        if (! defined $command) {
          logMsg(2, "Lost connection");
          $read_set->remove($rh);
          $rh->close();

        } else {

          # clean the command
          $command =~ s/\r//;
          $command =~ s/\n//;
          $command =~ s/ +$//;

          if (!($command =~ m/get_status/)) {
            logMsg(1, " <- ".$command);
          }

          ($result, $response) = handleCommand($command); 

          if ($result ne "ok") {
            logMsg(0, "command: ".$command);
            logMsg(0, "result: ".$result.", response: ".$response);

          } else {
            if (!($command =~ m/get_status/)) {
              logMsg(1, " -> ".$result.":".$response);
            }
          }

          if ($response ne "") {
            print $rh $response."\r\n";
          }
          print $rh $result."\r\n";

          # logMsg(2, "Closing Connection");
          # $handler->close;

        }
      }
    }
  }

  logMsg(0, "STOPPING SCRIPT");
  close($sock);

  return 0;

}


sub handleCommand($) {

  (my $string) = @_;

  logMsg(2, "Command = \"".$string."\"");

  my @cmds = split(/ /,$string, 2);
  my $key = $cmds[0];

  my $current_binary_dir = Dada::getCurrentBinaryVersion();
  my $cmd = "";
  my $result = "ok";
  my $response = "";

  if ($key eq "clean_scratch") {
    $result = "fail";
    $response = "command deprecated/disbaled";
  }

  elsif ($key eq "clean_archives") {
    $result = "fail";
    $response = "command deprecated/disbaled";
  }

  elsif ($key eq "clean_rawdata") {
    $result = "fail";
    $response = "command deprecated/disbaled";
  }

  elsif ($key eq "clean_logs") {
    $result = "fail";
    $response = "command deprecated/disbaled";
  }

  elsif ($key eq "stop_iface") {
    $cmd = "sudo /sbin/ifdown ".$cfg{"PWC_DEVICE"};
    logMsg(1, $cmd);
    ($result, $response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "stop_pwcs") {
    $cmd = "killall ".$cfg{"PWC_BINARY"};
    ($result,$response) = Dada::mySystem($cmd);  
  }

  elsif ($key eq "stop_pwc") {
    $cmd = "killall -KILL ".$cmds[1];
    ($result,$response) = Dada::mySystem($cmd);  
  }

  elsif ($key eq "stop_dfbs") {
    $cmd = "killall -KILL ".$cfg{"DFB_SIM_BINARY"};
    ($result,$response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "stop_srcs") {
    $cmd = "killall dada_dbdisk dspsr dada_dbnic";
    ($result,$response) = Dada::mySystem($cmd);  
  }

  elsif ($key eq "kill_process") {
    ($result,$response) = Dada::killProcess($cmds[1]);
  }

  elsif ($key eq  "start_bin") {
    $cmd = $current_binary_dir."/".$cmds[1];
    ($result,$response) = Dada::mySystem($cmd);  
  }

  elsif ($key eq "start_iface") {
    $cmd = "sudo /sbin/ifup ".$cfg{"PWC_DEVICE"};
    logMsg(1, $cmd);
    ($result,$response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "start_pwcs") {
    $cmd = $current_binary_dir."/".$cfg{"PWC_BINARY"}." -d".
          " -k ".lc($cfg{"RECEIVING_DATA_BLOCK"}).
          " -p ".$cfg{"CLIENT_UDPDB_PORT"}." -c ".$cfg{"PWC_PORT"}.
          " -l ".$cfg{"PWC_LOGPORT"};
  
    # add any instrument specific options here (see bpsr)
    $cmd .= $pwc_add;
    
    ($result,$response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "set_bin_dir") {
    ($result, $response) = Dada::setBinaryDir($cmds[1]);
  }

  elsif ($key eq "get_bin_dir") {
    ($result, $response) = Dada::getCurrentBinaryVersion();
  }

  elsif ($key eq "get_bin_dirs") {
    ($result, $response) = Dada::getAvailableBinaryVersions();
  }

  elsif ($key eq "destroy_db") {

    my @dbs = split(/ /,$cfg{"DATA_BLOCKS"});
    my $db = "";
    my $temp_result = "";
    my $temp_response = "";

    foreach $db (@dbs) {
      
      if ( (defined $cfg{$db."_BLOCK_BUFSZ"}) && (defined $cfg{$db."_BLOCK_NBUFS"}) ) {
        $cmd = "sudo ".$current_binary_dir."/dada_db -d -k ".lc($db);
        ($temp_result,$temp_response) = Dada::mySystem($cmd);

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

  elsif ($key eq "init_db") {

    my @dbs = split(/ /,$cfg{"DATA_BLOCKS"});
    my $db;
    my $temp_result = "";
    my $temp_response = "";

    foreach $db (@dbs) {

      if ( (defined $cfg{$db."_BLOCK_BUFSZ"}) && (defined $cfg{$db."_BLOCK_NBUFS"}) ) {
        $cmd = "sudo ".$current_binary_dir."/dada_db -l -k ".lc($db).
               " -b ".$cfg{$db."_BLOCK_BUFSZ"}." -n ".$cfg{$db."_BLOCK_NBUFS"};
        ($temp_result,$temp_response) = Dada::mySystem($cmd);

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

  elsif ($key eq "stop_daemon") {
    if ($cmds[1] =~ m/_master_control/) {
      $quit_daemon = 1;
    } else {
      ($result,$response) = stopDaemon($cmds[1], 10);
    }
  }

  elsif ($key eq "stop_daemons") {
    ($result,$response) = stopDaemons(\@daemons);
  }

  elsif ($key eq "stop_helper_daemons") {
    ($result,$response) = stopDaemons(\@helper_daemons);
  }

  elsif ($key eq "daemon_info") {
    ($result,$response) = getDaemonInfo(\@daemons);
  }

  elsif ($key eq "start_daemon") {
    $cmd = "client_".$cmds[1].".pl";
    ($result, $response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "start_daemons") {
    ($result, $response) = startDaemons(\@daemons);
  }

  elsif ($key eq "start_helper_daemons") {
    ($result, $response) = startDaemons(\@helper_daemons);
  }

  elsif ($key eq "dfbsimulator") {
    $cmd = $current_binary_dir."/".$cfg{"DFB_SIM_BINARY"}." ".$cmds[1];
    ($result,$response) = Dada::mySystem($cmd);
  }

  elsif ($key eq "system") {
    ($result,$response) = Dada::mySystem($cmds[1], 0);  
  }

  elsif ($key eq "get_disk_info") {
    ($result,$response) = Dada::getDiskInfo($cfg{"CLIENT_RECORDING_DIR"});
  }

  elsif ($key eq "get_db_info") {
    ($result,$response) = Dada::getDBInfo(lc($cfg{"PROCESSING_DATA_BLOCK"}));
  }

  elsif ($key eq "get_alldb_info") {
    ($result,$response) = Dada::getAllDBInfo($cfg{"DATA_BLOCKS"});
  }

  elsif ($key eq "db_info") {
    ($result,$response) = Dada::getDBInfo(lc($cmds[1]));
  }

  elsif ($key eq "get_db_xfer_info") {
   ($result,$response) = Dada::getXferInfo();
  }

  elsif ($key eq "get_load_info") {
    ($result,$response) = Dada::getLoadInfo();
  }

  elsif ($key eq "set_udp_buffersize") {
    $cmd = "sudo /sbin/sysctl -w net.core.wmem_max=67108864";
    ($result,$response) = Dada::mySystem($cmd);

    if ($result eq "ok") {
      $cmd = "sudo /sbin/sysctl -w net.core.rmem_max=67108864";
      ($result,$response) = Dada::mySystem($cmd);
    }
  }

  elsif ($key eq "get_all_status") {
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

  elsif ($key eq "get_status") {

    my $subresult = "";
    my $subresponse = "";

    ($result,$subresponse) = Dada::getRawDisk($cfg{"CLIENT_RECORDING_DIR"});
    $response = $subresponse.";;;";

    ($subresult,$subresponse) = Dada::getAllDBInfo(lc($cfg{"PROCESSING_DATA_BLOCK"}));
    $response .= $subresponse.";;;";
    if ($subresult eq "fail") {
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

  elsif ($key eq "stop_master_script") {
    $quit_daemon = 1;
  }

  else {
    $result = "fail";
    $response = "Unrecognized command ".$string;
  } 

  return ($result,$response);

}

#   
# stops the specified client daemon, optionally kills it after a period
#
sub stopDaemon($;$) {
    
  (my $daemon, my $timeout=10) = @_;
    
  my $pid_file  = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".pid";
  my $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon.".quit";
  my $script    = "client_".$daemon.".pl";
  my $ps_cmd    = "ps auxwww | grep ".$cfg{"USER"}." | grep '".$script."' | grep -v grep";
  my $cmd = "";
    
  system("touch ".$quit_file);
    
  my $counter = $timeout;
  my $running = 1;

  while ($running && ($counter > 0)) {
    `$ps_cmd`;
    if ($? == 0) {
      logMsg(0, "daemon ".$daemon." still running");
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
    logMsg(0, "Error: ".$response);
  } 
    
  return ($result, $response);
}


sub stopDaemons(\@) {

  my ($ref) = @_;
  my @ds = @$ref;

  my $threshold = 20;
  my $all_stopped = 0;
  my $quit_file = "";
  my $d = "";
  my $cmd = "";
  my $result = "";
  my $response = "";

  # Touch the quit files for each daemon
  foreach $d (@ds) {
    $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$d.".quit";
    $cmd = "touch ".$quit_file;
    system($cmd);
  }

  while ((!$all_stopped) && ($threshold > 0)) {

    $all_stopped = 1;
    foreach $d (@ds) {
      $cmd = "ps aux | grep ".$cfg{"USER"}." | grep 'client_".$d.".pl' | grep -v grep";
      `$cmd`;
      if ($? == 0) {
        logMsg(1, $d." is still running");
        $all_stopped = 0;
        if ($threshold < 10) {
          ($result, $response) = Dada::killProcess($d);
        }
      }
    }
    $threshold--;
    sleep(1);
  }

  # Clean up the quit files
  foreach $d (@ds) {
    $quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".$d.".quit";
    unlink($quit_file);
  }

  # If we had to resort to a "kill", send an warning message back
  if (($threshold > 0) && ($threshold < 10)) {
    $result = "ok";
    $response = "KILL signal required to terminate some daemons";

  } elsif ($threshold <= 0) {
    $result = "fail";
    $response = "KILL signal did not terminae all daemons";

  } else {
    $result = "ok";
    $response = "Daemons exited correctly";
  }

  return ($result, $response);

}


sub startDaemons(\@) {

  my ($ref) = @_;
  my @ds = @$ref;

  my $d = ""; 
  my $cmd = "";
  my $result = "ok";
  my $response = "";
  my $daemon_result = "";
  my $daemon_response = "";

  foreach $d (@ds) {
    $cmd = "client_".$d.".pl";
    logMsg(2, "Starting daemon: ".$cmd);
    ($daemon_result, $daemon_response) = Dada::mySystem($cmd);
    logMsg(2, "Result: ".$daemon_result.":".$daemon_response);

     if ($daemon_result eq "fail") {
       $result = "fail";
       $response .= $daemon_response;
    }
  }

  return ($result, $response);

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

sub getDaemonInfo(\@) {

  my ($ref) = @_;
  my @ds = @$ref;

  my $d = "";
  my $control_dir = $cfg{"CLIENT_CONTROL_DIR"}; 
  my $cmd;
  my %array = ();

  foreach $d (@ds) {

    # Check to see if the process is running
    $cmd = "ps aux | grep client_".$d.".pl | grep -v grep > /dev/null";
    logMsg(2, $cmd);
    `$cmd`;
    if ($? == 0) {
      $array{$d} = 1;
    } else {
      $array{$d} = 0;
    }

    # check to see if the PID file exists
    if (-f $control_dir."/".$d.".pid") {
      $array{$d}++;
    }
  }

  # Add the PWC as a daemon
  $cmd = "ps aux | grep ".$cfg{"PWC_BINARY"}." | grep -v grep > /dev/null";
  logMsg(2, $cmd);
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

#
# logs a message to the nexus logger and prints to stdout
#
sub logMsg($$) {

  my ($level, $msg) = @_;
  if ($level <= $dl) {
    print "[".Dada::getCurrentDadaTime()."] ".$msg."\n";
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
  
  if ($sock) {
    close($sock);
  } 
  
  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}


#
# Test to ensure all module variables are set before main
#
sub good() {

  # the calling script must have set this
  if (! defined($cfg{"INSTRUMENT"})) {
    return ("fail", "Error: package global hash cfg was uninitialized");
  }

  if ( $daemon_name eq "") {
    return ("fail", "Error: a package variable missing [daemon_name]");
  }

  if (! -d $cfg{"CLIENT_ARCHIVE_DIR"} ) {
    return("fail", "Error: archive dir ".$cfg{"CLIENT_ARCHIVE_DIR"}." did not exist");
  }

  if (defined( $cfg{"CLIENT_RESULTS_DIR"} ) && (! -d $cfg{"CLIENT_RESULTS_DIR"} )) {
    return ("fail", "Error: results dir ".$cfg{"CLIENT_RESULTS_DIR"}." did not exist");
  } 

  # open a socket for listening connections
  $sock = new IO::Socket::INET (
    LocalHost => $host,
    LocalPort => $port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$sock) {
    return ("fail", "Could not create listening socket: ".$host.":".$port);
  }
  logMsg(2, "Opened socket on ".$host.":".$port);

  return ("ok", "");

}

END { }

1;  # return value from file
