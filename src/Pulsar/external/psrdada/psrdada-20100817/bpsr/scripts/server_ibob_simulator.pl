#!/usr/bin/env perl

###############################################################################
#
# server_ibob_simulator.pl
#
# This accepts config/start commands from PWC's and sends only one
# set of commands to the ibob_simulator (apsr16 normally)

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Bpsr;           # BPSR Module 
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Constants
#
use constant DEBUG_LEVEL => 2;
use constant PIDFILE     => "ibob_simulator.pid";
use constant LOGFILE     => "ibob_simulator.log";


#
# Global Variables
#
our %cfg = Bpsr::getBpsrConfig();      # Bpsr.cfg
our $quit_daemon : shared  = 0;


# Autoflush output
$| = 1;


# Signal Handler
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;


#
# Local Varaibles
#

# For PWC's to connect to this script asking for UTC_START
my $pwc_host = $cfg{"IBOB_SIM_HOST"};
my $pwc_port = $cfg{"IBOB_SIM_PORT"};

# Where the bpsr_udpgenerator is running
my $ibob_host = $cfg{"DFB_SIM_HOST"};
my $ibob_port = $cfg{"DFB_SIM_PORT"};

my $logfile = $cfg{"SERVER_LOG_DIR"}."/".LOGFILE;
my $pidfile = $cfg{"SERVER_CONTROL_DIR"}."/".PIDFILE;

my $daemon_control_thread = 0;

my $acc_len = 25;
my $eol = "\n";
my $utc_start = 0;

my $result = "";
my $response = "";
my $cmd = "";
my $i = 0;
my $pwc_socket = "";   # Server socket for new connections
my $rh = "";
my $string = "";


# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured host: ".$pwc_host."\n";
  exit(1);
}

# Socket for PWC's to connect to
$pwc_socket = new IO::Socket::INET (
    LocalHost => $pwc_host,
    LocalPort => $pwc_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);

die "Could not create listening socket: $!\n" unless $pwc_socket;

# Redirect standard output and error
Dada::daemonize($logfile, $pidfile);

debugMessage(0, "STARTING SCRIPT");

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

my $read_set = new IO::Select();  # create handle set for reading
$read_set->add($pwc_socket);      # add the main socket to the set

my @response_handles = ();
my $n_replies = 0;

debugMessage(2, "Opened socket for PWCs ".$pwc_host.":".$pwc_port);

my $start_ibob = 0;

while (!$quit_daemon) {

  # Get all the readable handles from the server
  my ($readable_handles) = IO::Select->select($read_set, undef, undef, 1);

  foreach $rh (@$readable_handles) {

    # if it is the main socket then we have an incoming connection and
    # we should accept() it and then add the new socket to the $Read_Handles_Object
    if ($rh == $pwc_socket) {

      my $handle = $rh->accept();
      $handle->autoflush();
      my $hostinfo = gethostbyaddr($handle->peeraddr);
      my $hostname = $hostinfo->name;

      debugMessage(2, "Accepting connection from ".$hostname);

      # Add this read handle to the set
      $read_set->add($handle); 

    } else {

      my $hostinfo = gethostbyaddr($rh->peeraddr);
      my $hostname = $hostinfo->name;
      my @parts = split(/\./,$hostname);
      my $machine = $parts[0];
      $string = Dada::getLine($rh);

      # If the string is not defined, then we have lost the connection.
      # remove it from the read_set
      if (! defined $string) {

        debugMessage(1, "Lost connection from ".$hostname.", closing socket");
        $read_set->remove($rh);
        close($rh);

      # We have a request
      } else {

        debugMessage(2, $machine.": \"".$string."\"");
        my $response = "";
        
        # Connect to the DFB3 and get the gain
        if ( $string =~ m/^START (\d+)$/) {

          # Add this handle for a reply after the levels are set
          push(@response_handles, $rh);
          $n_replies++;

          if ($start_ibob == 0) {
            $start_ibob = 1;
            my @arr = split(/ /,$string);
            $acc_len = $arr[1];
          }
        }
      }
    }
  }

  # If we need to start up the simulator
  if (($start_ibob == 1) && ($n_replies == $cfg{"NUM_PWC"})) {
    
    my $handle = Dada::connectToMachine($ibob_host, $ibob_port, 2);

    if ($handle) {

      debugMessage(1, "Setting iBob Levels");
      my ($s0, $s1, $bit) = Bpsr::set_ibob_levels($handle, $acc_len, $eol);
      debugMessage(1, "s0 = $s0, s1 = $s1, bit = $bit");

      debugMessage(1, "Starting on next 1 sec tick");
      $utc_start = Bpsr::start_ibob($handle, $eol);
      debugMessage(1, "utc_start = ".$utc_start);

    } else {
      debugMessage(0, "Could not connect to ibob ".$ibob_host.":".$ibob_port);
    }


    # Send replies to the ones that have connected so far
    debugMessage(1, "Sending ".$n_replies." replies \"".$utc_start."\"");    
    foreach $rh (@response_handles) {
      print $rh $utc_start."\n";
      $n_replies--;
    }
    @response_handles = ();
    $start_ibob = 0;

  }

}

# Rejoin our daemon control thread
$daemon_control_thread->join();

debugMessage(0, "STOPPING SCRIPT");

exit(0);


###############################################################################
#
# Functions
#


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
    my $time = Dada::getCurrentDadaTime();
    print "[".$time."] ".$message."\n";
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
