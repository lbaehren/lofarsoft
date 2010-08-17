#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_multibob_manager.pl
#

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Bpsr;           # BPSR Module 
use strict;         # strict mode (like -Wall)
use warnings;
use threads;
use threads::shared;

#
# Sanity check to prevent multiple copies of this daemon running
#
Dada::preventDuplicateDaemon(basename($0));

#
# Constants
#
use constant DEBUG_LEVEL  => 1;
use constant DL           => 1;
use constant PIDFILE      => "bpsr_multibob_manager.pid";
use constant LOGFILE      => "bpsr_multibob_manager.log";
use constant QUITFILE     => "bpsr_multibob_manager.quit";
use constant MULTIBOB_BIN => "multibob_server";


#
# Global Variables
#
our %cfg   = Bpsr::getBpsrConfig();
our $error = $cfg{"STATUS_DIR"}."/bpsr_multibob_manager.error";
our $warn  = $cfg{"STATUS_DIR"}."/bpsr_multibob_manager.warn";
our $quit_daemon : shared  = 0;

# Autoflush output
$| = 1;


# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;


#
# Local Varaibles
#
my $log_file = $cfg{"SERVER_LOG_DIR"}."/".LOGFILE;
my $pid_file = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;
my $quit_file = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;

my $control_thread = 0;
my $multibob_thread = 0;
my $multibob_plot_thread = 0;

#
# Main
#
Dada::daemonize($log_file, $pid_file);

Dada::logMsg(0, DL, "STARTING SCRIPT");

# Start the daemon control thread
$control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

# Start the multibob_server thread
$multibob_thread = threads->new(\&multibobThread);

# Start the multibob plotting thread
$multibob_plot_thread = threads->new(\&multibobPlotThread);

# Wait for threads to return
while (!$quit_daemon) {
  sleep(2);
}

# rejoin threads
Dada::logMsg(0, DL, "main: joining threads");
$control_thread->join();
$multibob_thread->join();
$multibob_plot_thread->join();

Dada::logMsg(0, DL, "STOPPING SCRIPT");

exit 0;



###############################################################################
#
# Functions
#


#
# Runs the multibob_server on localhost. If the server fails, then 
# try to relaunch it
#
sub multibobThread() {

  Dada::logMsg(1, DL, "multibobThread: starting");

  my $runtime_dir   = $cfg{"SERVER_STATS_DIR"};
  my $port          = $cfg{"IBOB_MANAGER_PORT"};
  my $npwc          = $cfg{"NUM_PWC"};

  my $config_thread = 0;
  my $cmd = "";
  my $result = "";
  my $response = "";

  Dada::logMsg(2, DL, "multibobThread: checkMultibobServer()");
  ($result, $response) = checkMultibobServer();
  Dada::logMsg(2, DL, "multibobThread: ".$result." ".$response);

  $cmd =  "cd ".$runtime_dir."; echo '' | ".MULTIBOB_BIN." -n ".$npwc." -p ".$port." 2>&1";
  $cmd .= " | server_bpsr_server_logger.pl";

  while (!$quit_daemon) {

    $config_thread = threads->new(\&configureMultibobServerWrapper);

    # This command should "hang" until the multibob_server command has terminated
    Dada::logMsg(1, DL, "multibobThread: ".$cmd);
    system($cmd);
    #($result, $response) = Dada::mySystem($cmd);
    #Dada::logMsg(1, DL, "multibobThread: ".$result." ".$response);

    if (!$quit_daemon) {
      Dada::logMsgWarn($warn, "multibobThread: ".MULTIBOB_BIN." exited unexpectedly, re-launching");
      sleep(1);
    }
    
    Dada::logMsg(1, DL, "multibobThread: joining config_thread");
    $config_thread->join();
    Dada::logMsg(1, DL, "multibobThread: config_thread joined");
    $config_thread = 0;
  }

  Dada::logMsg(1, DL, "multibobThread: exiting");

}

#
# Configure the mulibob_server
#
sub configureMultibobServerWrapper() 
{

  Dada::logMsg(1, DL, "configureMultibobServer: configuring multibob_server");
  my ($result, $response) = Bpsr::configureMultibobServer();

  if ($result ne "ok") {
    Dada::logMsgWarn($error, "configureMultibobServer: failed ".$response);

  } else {
    Dada::logMsg(1, DL, "configureMultibobServer: done");
  }
}

# 
# Monitors the /nfs/results/bpsr/stats directory creating the PD Bandpass plots
# as requried
#
sub multibobPlotThread()
{

  Dada::logMsg(1, DL, "multibobPlotThread: starting");

  my $bindir    = Dada::getCurrentBinaryVersion();
  my $stats_dir = $cfg{"SERVER_STATS_DIR"};
  my @bramfiles = ();
  my $bramfile  = "";
  my $plot_cmd  = "";
  my $cmd       = "";
  my $result    = "";
  my $response  = "";
  my $i = 0;

  while (!$quit_daemon) {
   
    Dada::logMsg(2, DL, "multibobPlotThread: looking for bramdump files in ".$stats_dir);
 
    # look for plot files
    opendir(DIR,$stats_dir);
    @bramfiles = sort grep { !/^\./ && /\.bramdump$/ } readdir(DIR);
    closedir DIR;

    if ($#bramfiles == -1) {
      Dada::logMsg(2, DL, "multibobPlotThread: no files, sleeping");
    }

    # plot any existing bramplot files
    for ($i=0; $i<=$#bramfiles; $i++) 
    {
      $bramfile = $stats_dir."/".$bramfiles[$i];

      $plot_cmd = $bindir."/bpsr_bramplot ".$bramfile;

      Dada::logMsg(2, DL, $plot_cmd);

      ($result, $response) = Dada::mySystem($plot_cmd);

      if ($result ne "ok") {
        Dada::logMsgWarn($warn, "plot of ".$bramfile." failed ".$response);
      } else {
        Dada::logMsg(2, DL, "bpsr_bramplot ".$bramfile.": ".$response);
      }
      unlink($bramfile);
    }

    sleep(1);

    my $ibob = "";

    for ($i=0; $i < $cfg{"NUM_PWC"}; $i++ ) {
      $ibob = $cfg{"IBOB_DEST_".$i};
      removeOldPngs($stats_dir, $ibob, "1024x768");
      removeOldPngs($stats_dir, $ibob, "400x300");
      removeOldPngs($stats_dir, $ibob, "112x84");
    }

    sleep(1);

  }

  Dada::logMsg(2, DL, "multibobPlotThread: exiting");
}


#
# Polls for the "quitdaemons" file in the control dir
#
sub controlThread($$) {

  Dada::logMsg(1, DL, "controlThread: starting");

  my ($quit_file, $pid_file) = @_;
  Dada::logMsg(2, DL ,"controlThread(".$quit_file.", ".$pid_file.")");

  my $result = "";
  my $response = "";

  # poll for the existence of the control file
  while ((!-f $quit_file) && (!$quit_daemon)) {
    sleep(1);
  }

  # signal threads to exit
  $quit_daemon = 1;

  # Get the multibob server to quit
  Dada::logMsg(1, DL, "controlThread: checkMultibobServer()");
  ($result, $response) = checkMultibobServer();
  Dada::logMsg(1, DL, "controlThread: checkMultibobServer: ".$result." ".$response);

  if (-f $pid_file) {
    Dada::logMsg(2, DL, "controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    Dada::logMsgWarn($warn, "controlThread: PID file did not exist on script exit");
  }

  Dada::logMsg(2, DL, "controlThread: exiting");

}

#
# Handle INT AND TERM signals
#
sub sigHandle($) {

  my $sigName = shift;

  Dada::logMsgWarn($warn, basename($0).": Received SIG".$sigName);

  $quit_daemon = 1;
  sleep(5);
 
  Dada::logMsgWarn($warn, basename($0).": Exiting");

  exit(1);

}

sub sigIgnore($) {
                                                                                                        
  my $sigName = shift;
  Dada::logMsgWarn($warn, basename($0)." Received SIG".$sigName);
  Dada::logMsgWarn($warn, basename($0)." Ignoring");

}


sub removeOldPngs($$$) {

  my ($stats_dir, $ibob, $res) = @_;

  Dada::logMsg(2, DL, "removeOldPngs(".$stats_dir.", ".$ibob.", ".$res.")");

  # remove any existing plot files that are more than 10 seconds old
  my $cmd  = "find ".$stats_dir." -name '*".$ibob."_".$res.".png' -printf \"%T@ %f\\n\" | sort -n -r";
  my $result = `$cmd`;
  my @array = split(/\n/,$result);

  my $time = 0;
  my $file = "";
  my $line = "";
  my $i = 0;

  # if there is more than one result in this category and its > 10 seconds old, delete it
  for ($i=1; $i<=$#array; $i++) {

    $line = $array[$i];
    ($time, $file) = split(/ /,$line,2);

    if (($time+20) < time)
    {
      $file = $stats_dir."/".$file;
      Dada::logMsg(2, DL, "removeOldPngs: unlink ".$file);
      unlink($file);
    }
  }

  Dada::logMsg(2, DL, "removeOldPngs: exiting");
}


#
# Ensures that the multibob server is not running
#
sub checkMultibobServer() {

  Dada::logMsg(1, DL, "checkMultibobServer()");

  my $localhost = Dada::getHostMachineName();
  my $port      = $cfg{"IBOB_MANAGER_PORT"};

  my $result = "";
  my $response = "";
  my $cmd = "";
  my $handle = 0;

  # Check if the binary is running
  $cmd = "ps aux | grep ".MULTIBOB_BIN." | grep -v grep > /dev/null";
  Dada::logMsg(2, DL, "checkMultibobServer: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, DL, "checkMultibobServer: ".$result." ".$response);

  if ($result eq "ok") {

    Dada::logMsgWarn($warn, "checkMultibobServer: ".MULTIBOB_BIN." process existed");

    # set the global variable to quit the daemon
    $handle = Dada::connectToMachine($localhost, $port, 1);
    if (!$handle) {
      Dada::logMsgWarn($warn, "checkMultibobServer: could not connect to ".MULTIBOB_BIN." on ".$localhost.":".$port);

    } else {

      # ignore welcome message
      $response = <$handle>;
                                                                                                          
      Dada::logMsg(0, DL, "checkMultibobServer: multibob <- close");
      ($result, $response) = Dada::sendTelnetCommand($handle, "close");
      Dada::logMsg(0, DL, "checkMultibobServer: multibob -> ".$result.":".$response);

      sleep(1);

      Dada::logMsg(0, DL, "checkMultibobServer: multibob_server <- quit");
      print $handle "quit\r\n";
      close($handle);
    }

    sleep(1);

    # try again to ensure it exited
    Dada::logMsg(2, DL, "checkMultibobServer: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, DL, "checkMultibobServer: ".$result." ".$response);

    # if is it still running
    if ($result eq "ok") {
      Dada::logMsgWarn($warn, "checkMultibobServer: multibob_server, refused to exit gracefully, killing...");
      $cmd = "killall -KILL multibob_server";
      Dada::logMsg(1, DL, "checkMultibobServer: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      Dada::logMsg(1, DL, "checkMultibobServer: ".$result." ".$response);

      return ("ok", "process had to be killed"); 

    } else {
      return ("ok", "process exited correctly"); 
    }

  } else {

    return ("ok", "process not running");
  }

}
