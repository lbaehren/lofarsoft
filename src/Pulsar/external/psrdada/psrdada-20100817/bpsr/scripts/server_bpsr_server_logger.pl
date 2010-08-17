#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

#
# Include Modules
#

use Bpsr;           # BPSR/DADAModule for configuration options
use strict;         # strict mode (like -Wall)
use File::Basename;
use Getopt::Std;


#
# Constants
#
use constant DEBUG_LEVEL   => 1;


#
# Global Variable Declarations
#


#
# Local Variable Declarations
#
my $line = "";
my $input_daemon_name = "none";


#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;


# Auto flush output
$| = 1;


#
# Parse command line
#
my %opts;
getopts('hn:', \%opts);
                                                                                                                                       
if ($opts{h}) {
  usage();
  exit(0);
}
                                                                                                                                       
if ($opts{n}) {
  $input_daemon_name = $opts{n};
}

#
# Main
# 
my $time = "";

while (defined($line = <STDIN>)) {

  chomp $line;
  $time = Dada::getCurrentDadaTime();

  if ($input_daemon_name ne "none") { 
    print STDERR "[".$time."] [".$input_daemon_name."] ".$line."\n";
    #Dada::logMsg(0, 0, "[".$input_daemon_name."] ".$line);
  } else {
    print STDERR "[".$time."] ".$line."\n";
    #Dada::logMsg(0, 0, $line);
  }

}

exit 0;

sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  print STDERR basename($0)." : Exiting\n";

  exit 1;

}

sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";

}


sub usage() {

  print "Usage: ".basename($0)." [options]\n";
  print "  -h         print help text\n";
  print "  -n name    prefix the log message with [name]\n";

}


