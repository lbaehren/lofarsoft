#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use Net::hostent;
use Apsr;           # APSR/DADA Module for configuration options
use threads;
use threads::shared;
use strict;         # strict mode (like -Wall)


#
# Constants
#

use constant  DEBUG_LEVEL         => 1;

#
# Global Variables
#
our %cfg : shared = Apsr::getApsrConfig();      # dada.cfg in a hash


my $cmd = "";
my $arg = "";
my $host_offset = 1;

my $i;
my $j;
my @machines = ();
my @threads = ();
my @results = ();
my @responses = ();
my $failure = "false";
my $comm_string = "";

# get the command
$cmd = $ARGV[0];


# some commands may have arguments before the machine list
if (($cmd eq "start_daemon") || ($cmd eq "stop_daemon") || ($cmd eq "db_info")) {
  $arg = $ARGV[1];
  $host_offset = 2;
}

# get the hosts from the arguements
for ($i=$host_offset; $i<=$#ARGV; $i++) {
  push(@machines, $ARGV[$i]);
}

for ($i=0; $i<=$#machines; $i++) {

  # special case for starting the master control script
  if (($cmd eq "start_daemon") && ($arg eq "apsr_master_control")) {

    $comm_string = "ssh -x -l apsr ".$machines[$i]." \"client_apsr_master_control.pl\"";
    @threads[$i] = threads->new(\&sshCmdThread, $comm_string);

  } else {

    if ($arg ne "") {
      $comm_string = $cmd." ".$arg;
    } else {
      $comm_string = $cmd;
    }

    @threads[$i] = threads->new(\&commThread, $comm_string, $machines[$i]);
  }

  if ($? != 0) {
    @results[$i] = "dnf"; 
    @responses[$i] = "dnf"; 
  }
}

for($i=0;$i<=$#machines;$i++) {
  if ($results[$i] ne "dnf") {  
    (@results[$i],@responses[$i]) = @threads[$i]->join;
  }
}

for($i=0;$i<=$#machines;$i++) {
  print $machines[$i].":".$results[$i].":".$responses[$i]."\n";
  if (($results[$i] eq "fail") || ($results[$i] eq "dnf")) {
    $failure = "true";
  }
}

if ($failure eq "true") {
  exit 0;
} else {
  exit 0;
}


sub sshCmdThread($) {

  (my $command) = @_;

  my $result = "fail";
  my $response = "Failure Message";

  $response = `$command`;
  if ($? == 0) {
    $result = "ok";
  }
  return ($result, $response);
  
}

sub commThread($$) {

  (my $command, my $machine) = @_;

  my $result = "fail";
  my $response = "Failure Message";
 
  my $handle = Dada::connectToMachine($machine, $cfg{"CLIENT_MASTER_PORT"}, 0);
  # ensure our file handle is valid
  if (!$handle) { 
    return ("fail","Could not connect to machine ".$machine.":".$cfg{"CLIENT_MASTER_PORT"}); 
  }

  ($result, $response) = Dada::sendTelnetCommand($handle,$command);

  $handle->close();

  return ($result, $response);

}

