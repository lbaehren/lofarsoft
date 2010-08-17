#!/usr/bin/env perl

###############################################################################
#
# server_dfb_gain_simulator.pl
#
# This script monitors masquerades as the DFB3 gain interface.

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;
use Apsr;           # APSR/DADA Module for configuration options


#
# Constants
#
use constant DEBUG_LEVEL => 1;
use constant PIDFILE     => "apsr_dfb_gain_simulator.pid";
use constant LOGFILE     => "apsr_dfb_gain_simulator.log";
use constant QUITFILE    => "apsr_dfb_gain_simulator.quit";


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
my $dfb3_host = $cfg{"HWGAIN_HOST"};
my $dfb3_port = $cfg{"HWGAIN_PORT"};

my $dfb_sim_host = $cfg{"DFB_SIM_HOST"};
my $dfb_sim_port = $cfg{"DFB_SIM_GAIN_PORT"};

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
if ($ENV{'HOSTNAME'} ne $dfb3_host) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured host: ".$dfb3_host."\n";
  exit(1);
}

$server_socket = new IO::Socket::INET (
    LocalHost => $dfb3_host,
    LocalPort => $dfb3_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);

die "Could not create listening socket: $!\n" unless $server_socket;

# Redirect standard output and error
# Dada::daemonize($logfile, $pidfile);

debugMessage(0, "STARTING SCRIPT");

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

my $read_set = new IO::Select();  # create handle set for reading
$read_set->add($server_socket);   # add the main socket to the set

debugMessage(2, "Waiting for connection on ".$dfb3_host.":".$dfb3_port);

# Internal memory of the current DFB3 gains
my @pol0_gains = ();
my @pol1_gains = ();

# setup the default values
my $nchan = 1 * $cfg{"NUM_PWC"};
my $i=0;

for ($i=0; $i<$nchan; $i++) {

  @pol0_gains[$i] = 33;
  @pol1_gains[$i] = 33;

}

my $curr_time = time();
my $prev_time = $curr_time;
my $last_avg = -1;

while (!$quit_daemon) {

  # Get all the readable handles from the server
  my ($readable_handles) = IO::Select->select($read_set, undef, undef, 1);

  # each second, adjust the gain
  $curr_time = time();
  if ($curr_time > $prev_time) {

    my $avg = 0;
    my $str = "";

    # Calculate the average gain.
    for ($i=0; $i<$nchan; $i++) {

      $avg += $pol0_gains[$i];
      $avg += $pol1_gains[$i];
      $str .= $pol0_gains[$i]." ".$pol1_gains[$i]." ";

    }

    $avg /= ($nchan*2);
    my $int_avg = int($avg);
    $str .= "=> average of ".$int_avg;
    debugMessage(2, $str);

    if ($last_avg != $int_avg) {

      debugMessage(2, "Average gain is ".$avg.", intavg = ".$int_avg);

      my $handle = Dada::connectToMachine($dfb_sim_host, $dfb_sim_port);
      if ($handle) {
        debugMessage(1, "DFB <- APSRGAIN ".$int_avg);
        print $handle "APSRGAIN ".$int_avg."\r\n";
        my $line = Dada::getLine($handle);
        debugMessage(0, "Reply from DFB3 simulator: ".$line);
        close($handle);
        $last_avg = $int_avg;

      } else {
        debugMessage(2, "Couldn't connect to the dfb3 simulator");
        for ($i=0; $i<$nchan; $i++) {
          @pol0_gains[$i] = 33;
          @pol1_gains[$i] = 33;
        }
      }
    } else {
      my $handle = Dada::connectToMachine($dfb_sim_host, $dfb_sim_port);
      if ($handle) {
        print $handle "APSRGAIN\r\n";
        my $line = Dada::getLine($handle);
        debugMessage(2, "Current Gain: ".$line);
        close($handle);
        $last_avg = int($avg);
      } else {
        debugMessage(2, "dfb3 simulator not running, gain set to 33");
        for ($i=0; $i<$nchan; $i++) {
          @pol0_gains[$i] = 33;
          @pol1_gains[$i] = 33;
        }
      }
    }
    $prev_time = $curr_time;
  }

  foreach $rh (@$readable_handles) {

    # if it is the main socket then we have an incoming connection and
    # we should accept() it and then add the new socket to the $Read_Handles_Object
    if ($rh == $server_socket) { 

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
        if ( $string =~ m/^APSRGAIN (\d+) (0|1)$/) {

          my ($ignore, $chan, $pol) = split(/ /, $string);
          debugMessage(2, $machine." -> ".$string);

          if (($chan >= 0) && ($chan < $nchan)) {

            if ($pol eq 0) {
              $response = "OK ".$chan." ".$pol." ".$pol0_gains[$chan];
            } elsif ($pol eq 1) {
              $response = "OK ".$chan." ".$pol." ".$pol1_gains[$chan];
            } else {
              $response = "Error: invalid pol specified ".$pol;
            }
          } else {
            $response = "Error: invalid channel specified ".$chan;
          }

          debugMessage(1, $machine." <- ".$response);

        } elsif ($string =~ m/^APSRGAIN (\d+) (0|1) (\d+)$/) {

          debugMessage(1, $machine." -> ".$string);

          my ($ignore, $chan, $pol, $val) = split(/ /, $string);

          if (($chan >= 0) && ($chan < $nchan)) {

            if ($pol eq 0) {
              $pol0_gains[$chan] = $val;
              $response = "OK" 
            } elsif ($pol eq 1) {
              $pol1_gains[$chan] = $val;
              $response = "OK";
            } else {
              $response = "Error: invalid pol specified ".$pol;
            }
          } else {
            $response = "Error: invalid channel specified ".$chan;
          }

          debugMessage(2, $machine." <- ".$response);

        } else {
          debugMessage(2, "Unknown request received");
          $response = "Unknown request: ".$string;
          
        }
        print $rh $response."\n";
      }
      debugMessage(2, "=========================================");
    }
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
  my $daemon_quit_file = $cfg{"SERVER_CONTROL_DIR"}."/".QUITFILE;
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
