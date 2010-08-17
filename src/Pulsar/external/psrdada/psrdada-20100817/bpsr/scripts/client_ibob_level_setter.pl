#!/usr/bin/env perl
#
# Author:   Andrew Jameson
# Created:  21 May 2008
# Modified: 21 May 2008
#

use lib $ENV{"DADA_ROOT"}."/bin";
                                                                                                                   
#
# Include Modules
#
use strict;         # strict mode (like -Wall)
use IO::Socket::INET;
use IO::Socket;
use Time::HiRes qw(gettimeofday);
use Socket qw(:all); 
use IO::Select;
use File::Basename;
use Getopt::Std;
use Dada;             # DADA Module for configuration options
use Bpsr;             # BPSR Module for configuration options


#
# Constants
#
use constant DEBUG_LEVEL     => 0;
use constant DEFAULT_ACC_LEN => 25;
use constant MAX_VALUE        => 1;
use constant AVG_VALUES       => 2;


#
# Global Variable Declarations
#
our %cfg = Bpsr::getBpsrConfig();      # dada.cfg in a hash
our $handle = 0;
our $eol = "\r";

#
# Local variable decalatrions
#
my @output = ();
my $pwc_host = "";
my $ibob_host = "";
my $ibob_port = "";
my $forced_bit_selection = -1;
my $acc_len = DEFAULT_ACC_LEN;
my $level_set = AVG_VALUES;
my $report_back_to_pwcc = 0;
my $use_simulator = $cfg{"USE_DFB_SIMULATOR"};

if ($use_simulator) {
  $eol = "\n";
}

#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

my %opts;
getopts('a:b:hm:c:p:s', \%opts);

if ($opts{h}) {
  usage();
  exit(0);
}

if ($opts{a}) {
  $acc_len = $opts{a};
}

if ($opts{b}) {
  $forced_bit_selection = $opts{b};
}

if ($opts{m}) {
  if ($opts{m} == 1) {
    $level_set = AVG_VALUES;
  }
}

if ($opts{s}) {
  $report_back_to_pwcc = 1;
}

if ($#ARGV != 0) {
  usage();
  exit(1);
}

$pwc_host = $ARGV[0];
$ibob_host = "";
$ibob_port = "";


#
# Main
#

if ($use_simulator) {

  $ibob_host = $cfg{"IBOB_SIM_HOST"};
  $ibob_port = $cfg{"IBOB_SIM_PORT"};

# Determine the ibob this pwc_host should connect to
} else {

  my $i=0;
  for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
    my $temp_host = $cfg{"PWC_".$i};
    if ($pwc_host =~ m/$temp_host/) {
      $ibob_host = $cfg{"IBOB_".$i};
      $ibob_port = $cfg{"IBOB_PORT"};
    }
  }
}


logMessage(2, "Tring to connect to ".$ibob_host.":".$ibob_port);
$handle = Dada::connectToMachine($ibob_host,$ibob_port);

my $utc_result = 0;

if (!$handle) {

  logMessage(0, "Could not connect to ".$ibob_host.":".$ibob_port);
  exit(1);

} else {

  logMessage(2, "Connected to ".$ibob_host.":".$ibob_port);

  if ($use_simulator) {

    logMessage(1, "Sending START ".$acc_len." to simulator");
    print $handle "START ".$acc_len.$eol;

  } else {

    logMessage(2, "Setting iBob Levels");
    my ($s0, $s1, $bit) = Bpsr::set_ibob_levels($handle, $acc_len, $eol); 
    logMessage(2, "s0 = $s0, s1 = $s1, bit = $bit");

  }

  #
  # Tell the server we are ready to start
  #
  if ($report_back_to_pwcc) {

    logMessage(2, "Reporting back to the PWCC");

    my $host = $cfg{"SERVER_HOST"};
    my $port = $cfg{"SERVER_PWC_RESPONSE_PORT"};

    my $tcs_interface_sock = Dada::connectToMachine($host, $port, 10);
    # ensure our file handle is valid
    if (!$tcs_interface_sock) {
      print "-1";
      logMessage(0, "Could not connect to machine ".$host.":".$port);
      exit(1);
    } else {
      logMessage(2, "Sending \"READY\" to ".$host.":".$port);
      print $tcs_interface_sock "READY\r\n";
      close $tcs_interface_sock;
      logMessage(2, "Sent \"READY\" to ".$host.":".$port);
    }
  }

  # If we are using the simulator, let it do all the talking to
  # bpsr_udpgenerator
  if ($use_simulator) {

    $utc_result = Dada::getLine($handle);
    logMessage(1, "Counter reset");

  } else {

    logMessage(2, "Starting on next 1 second tick");
    $utc_result = Bpsr::start_ibob($handle, $eol);
    logMessage(2, "UTC_START = ".$utc_result);
  }

  close($handle);

  logMessage(2, "Connection to ".$ibob_host.":".$ibob_port." closed");
  
  print $utc_result."\n";
}

exit 0;

#
# Logs a message to the Nexus
#
sub logMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    my @t2 = gettimeofday();
    my $time = Dada::printDadaTime($t2[0]);
    my $subsec = substr(sprintf("%.3f",$t2[1]/1000000.0),2);
    print "[".$time.".".$subsec."] ".$message."\n";
  }
}


sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";

  print STDERR "Closing connection\n";
  close($handle);

  print STDERR basename($0)." : Exiting\n";

  exit 1;

}


#
# Show usage
#
sub usage() {
  print "Usage: ".basename($0)." [options] pwc_host\n";
  print "   pwc_host      machine iBob is paired with as per dada.cfg\n";
  print "   -a acc_len    Accumulation length to use [default ".DEFAULT_ACC_LEN."]\n";
  print "   -b bits       Force a selection of bits [0,1,2 or 3]\n";
  print "   -m type       Type of level setting [0=max, 1=mean, default 0]\n";
  print "   -s            Attempt to report back to the PWCC notifying ready\n";
  print "   -h            print help text\n";
}    

