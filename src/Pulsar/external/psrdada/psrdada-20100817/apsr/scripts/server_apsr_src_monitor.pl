#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";


###############################################################################
#
# server_src_monitor.pl
#
# This script monitors messages from the Secondary Read Clients and logs them
# to files on the server. Messages conatin a severity and a text string. 
#

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Apsr;           # Apsr Module for configuration options
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Constants
#
use constant DEBUG_LEVEL => 1;
use constant PIDFILE     => "apsr_src_monitor.pid";
use constant LOGFILE     => "nexus.src.log";


#
# Global Variables
#
our %cfg = Apsr::getApsrConfig();      # Apsr.cfg
our $quit_daemon : shared  = 0;


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
    LocalPort => $cfg{"SERVER_SRC_LOG_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);
die "Could not create listening socket: $!\n" unless $server_socket;

# Redirect standard output and error
Dada::daemonize($logfile, $pidfile);

debugMessage(0, "STARTING SCRIPT: ".Dada::getCurrentDadaTime(0));

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

my $read_set = new IO::Select();  # create handle set for reading
$read_set->add($server_socket);   # add the main socket to the set

debugMessage(2, "Waiting for connection on ".$cfg{"SERVER_HOST"}.":".$cfg{"SERVER_SRC_LOG_PORT"});

while (!$quit_daemon) {

  # Get all the readable handles from the server
  my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);

  foreach $rh (@$readable_handles) {

    # if it is the main socket then we have an incoming connection and
    # we should accept() it and then add the new socket to the $Read_Handles_Object
    if ($rh == $server_socket) { 

      my $handle = $rh->accept();
      $handle->autoflush();
      my $hostinfo = gethostbyaddr($handle->peeraddr);
      my $hostname = $hostinfo->name;

      debugMessage(2, "Accepting connection from ", $hostname);

      # Add this read handle to the set
      $read_set->add($handle); 

    } else {

      my $hostinfo = gethostbyaddr($rh->peeraddr);
      my $hostname = $hostinfo->name;
      my @parts = split(/\./,$hostname);
      my $machine = $parts[0];
      $string = Dada::getLine($rh);

      if (! defined $string) {
        $read_set->remove($rh);
        close($rh);
      } else {
        debugMessage(2, "Received String \"".$string."\"");
        logMessage($machine, $string);
      }
    }
  }
}

# Rejoin our daemon control thread
$daemon_control_thread->join();

close($server_socket);
                                                                                
debugMessage(0, "STOPPING SCRIPT: ".Dada::getCurrentDadaTime(0));
                                                                                
exit(0);


###############################################################################
#
# Functions
#

sub logMessage($$) {

  (my $machine, my $string) = @_;

  my $statusfile_dir = $cfg{"STATUS_DIR"};
  my $logfile_dir = $cfg{"SERVER_LOG_DIR"};
  my $statusfile = "";
  my $logfile = "";

  # determine the source machine
  my @array = split(/\|/,$string,5);
  my $timestamp = $array[0];
  my $type = lc($array[1]);
  my $level = $array[2]; 
  my $source = $array[3]; 
  my $message = $array[4];

  # This daemon will only handle SRC messages
  if ($type eq "src") {

    # Log file for this message
    $logfile = $logfile_dir."/".$machine.".src.log";

    if ($level eq "WARN") {
      $statusfile = $statusfile_dir."/".$machine.".src.warn";
    } elsif ($level eq "ERROR") {
      $statusfile = $statusfile_dir."/".$machine.".src.error" ;
    } elsif ($level eq "INFO")  {
      ;
    } else {
      $logfile = "";
    }

    # Log the message to the logfile for the machine and type
    if ($logfile ne "") {
      if (-f $logfile) {
        open(FH,">>".$logfile);
      } else {
        open(FH,">".$logfile);
      }

      if ($level eq "INFO") {
        print FH "[".$timestamp."] ".$source.": ".$message."\n";
      } else {
        print FH "[".$timestamp."] ".$source.": ".$level.": ".$message."\n";
      }
      close FH;
    }

    my $msg = $machine." [".$timestamp."] ";
    if (($level eq "WARN") || ($level eq "ERROR")) {
      $msg .= $level." ";
    }
    $msg .= $source.": ".$message;

    debugMessage(0, $msg);

    # If the file is a warning or error, we create a warn/error file too
    if ($statusfile ne "") {
      if (-f $statusfile) {
        open(FH,">>".$statusfile);
      } else {
        open(FH,">".$statusfile);
      }
      print FH $source.": ".$message."\n";
      close FH;
    }
  } else {
    debugMessage(1,"Ignoring message \"".$source.": ".$string."\"from ".$machine);
  }
}

return 0;

sub daemonControlThread() {

  debugMessage(2, "Daemon control thread starting");

  my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;

  my $daemon_quit_file = Dada::getDaemonControlFile($cfg{"SERVER_CONTROL_DIR"});
  # Poll for the existence of the control file
  while ((!-f $daemon_quit_file) && (!$quit_daemon)) {
    sleep(1);
  }

  # set the global variable to quit the daemon
  $quit_daemon = 1;

  debugMessage(2, "Unlinking PID file: ".$pidfile);
  unlink($pidfile);

  debugMessage(2, "Daemon control thread ending");

}

sub debugMessage($$) {
  (my $level, my $message) = @_;

  if ($level <= DEBUG_LEVEL) {

    if ($message =~ m/^apsr/) {
      print $message."\n";
    } else {
      my $time = Dada::getCurrentDadaTime();
      print "nexus  [".$time."] ".$message."\n";
    }

  }

}

#
# Handle INT AND TERM signals
#
sub sigHandle($) {
                                                                                
  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(3);
  print STDERR basename($0)." : Exiting\n";
  exit(1);
                                                                                
}
