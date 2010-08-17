#!/usr/bin/env perl

###############################################################################
#
# server_aux_manager.pl
#
# This script manages the use of auxiliary processing nodes. It will maintain
# a list of currently available auxiliary nodes that can be made available for
# assistive processing for primary nodes
#

use lib $ENV{"DADA_ROOT"}."/bin";


use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use Net::hostent;
use File::Basename;
use Apsr;           # APSR/DADA Module for configuration options
use strict;         # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Constants
#
use constant DEBUG_LEVEL => 1;
use constant PIDFILE     => "apsr_aux_manager.pid";
use constant LOGFILE     => "apsr_aux_manager.log";


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
my $client_socket = "";   # PWC's contact this socket for help
my $assist_socket = "";   # Aux processors register helpfulness here
my $rh = "";
my $string = "";

my @helpers = ();
my @helper_rhs = ();

my @clients = ();
my @client_rhs = ();

# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}



# PWC's who require assistance will request on this socket
$client_socket = new IO::Socket::INET (
    LocalHost => $cfg{"SERVER_HOST"}, 
    LocalPort => $cfg{"SERVER_AUX_CLIENT_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);
die "Could not create SERVER_AUX_CLIENT socket on port ".$cfg{"SERVER_AUX_CLIENT_PORT"}.": $!\n" unless $client_socket;

# Clients who can assist processing will connect on this socket
$assist_socket = new IO::Socket::INET (
    LocalHost => $cfg{"SERVER_HOST"},
    LocalPort => $cfg{"SERVER_AUX_ASSIST_PORT"},
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
);
die "Could not create SERVER_AUX_ASSIST socket on port ".$cfg{"SERVER_AUX_ASSIST_PORT"}.": $!\n" unless $assist_socket;

# Redirect standard output and error
Dada::daemonize($logfile, $pidfile);

debugMessage(0, "STARTING SCRIPT: ".Dada::getCurrentDadaTime(0));

# Start the daemon control thread
$daemon_control_thread = threads->new(\&daemonControlThread);

my $read_set = new IO::Select();  # create handle set for reading
$read_set->add($client_socket);
$read_set->add($assist_socket);

debugMessage(2, "Listening for clients ".$cfg{"SERVER_HOST"}.":".$cfg{"SERVER_AUX_CLIENT_PORT"});
debugMessage(2, "Listening for helpers ".$cfg{"SERVER_HOST"}.":".$cfg{"SERVER_AUX_ASSIST_PORT"});

while (!$quit_daemon) {

  # Get all the readable handles from the server
  my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);

  foreach $rh (@$readable_handles) {

    # if it is the main socket then we have an incoming connection and
    # we should accept() it and then add the new socket to the $Read_Handles_Object

    my $handle = $rh->accept();

    if ($rh == $client_socket) { 

      my $hostinfo = gethostbyaddr($handle->peeraddr);
      my $hostname = $hostinfo->name;
      $handle->autoflush();
      debugMessage(2, "Client connection from: ". $hostname);

      # Add this read handle to the set
      $read_set->add($handle);

    } elsif ($rh == $assist_socket) {

      my $hostinfo = gethostbyaddr($handle->peeraddr);
      my $hostname = $hostinfo->name;
      $handle->autoflush();
      debugMessage(2, "Assist connection from: ".$hostname);

      $read_set->add($handle);

    } else {

      my $string = Dada::getLine($rh);

      if (! defined $string) {

        for ($i=0; $i<=$#helpers; $i++) {
          if ($helper_rhs[$i] eq $rh) {

            debugMessage(2, "Helper disconnect: ".$helpers[$i]);
            splice(@helpers, $i, 1);
            splice(@helper_rhs, $i, 1);
          }
        }

        for ($i=0; $i<=$#clients; $i++) {
          if ($client_rhs[$i] eq $rh) {
                                                                                                            
            debugMessage(2, "Client disconnect: ".$clients[$i]);
            splice(@clients, $i, 1);
            splice(@client_rhs, $i, 1);
          }
        }



        $read_set->remove($rh);
        close($rh);

      } else {
        debugMessage(3, "rh string: \"".$string."\"");

        if ($string =~ /apsr(\d\d):help/) {

          my @arr = split(/:/,$string);

          push(@clients, $arr[0]);
          push(@client_rhs, $rh);


          # If we have at least 1 machine to help 
          if ($#helpers >= 0) {

            debugMessage(1, $arr[0]." --> ".$string);
            debugMessage(3, "We have help available");
          
            my $helper = shift @helpers;
            my $helper_rh = shift @helper_rhs;

            my @helper_arr = split(/:/, $helper);
            
            debugMessage(3, "helper: ".$helper);
            debugMessage(3, "rh : ".$helper_rh);
          
            debugMessage(1, $helper_arr[0]." <-- ".$arr[0]);

            print $helper_rh "ack\r\n";
             
            debugMessage(1, $arr[0]." <-- ".$helper);

            print $rh $helper."\r\n";

          } else {

            debugMessage(2, $arr[0]." --> ".$string);
            debugMessage(2, $arr[0]." <-- none");
            print $rh "none\r\n";

          }
        }

        # if a client is offering assistance
        if ($string =~ m/apsr\d\d:(\d+)/) {

          my @arr = split(/:/,$string);

          debugMessage(1, $arr[0]." --> ".$string);

          # add this resource to the array
          push(@helpers, $string);
          push(@helper_rhs, $rh);

          # Note that we dont send an "ack" to the helper
          # until we need its help

        }

      }

    }
  }
}

my $i=0;
for ($i=0; $i<=$#helper_rhs; $i++) {
  close $helper_rhs[$i];
}

# Rejoin our daemon control thread
$daemon_control_thread->join();
                                                                                
debugMessage(0, "STOPPING SCRIPT: ".Dada::getCurrentDadaTime(0));
                                                                                
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
  my $time = Dada::getCurrentDadaTime();
  if ($level <= DEBUG_LEVEL) {
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
