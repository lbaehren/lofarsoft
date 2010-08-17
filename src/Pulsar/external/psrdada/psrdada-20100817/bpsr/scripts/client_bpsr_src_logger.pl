#!/usr/bin/env perl

#
# Author:   Andrew Jameson
# Created:  1 Feb 2008
# Modified: 1 Feb 2008
# 

#
# Include Modules
#

use lib $ENV{"DADA_ROOT"}."/bin";

use Bpsr;           # BPSR/DADAModule for configuration options
use strict;         # strict mode (like -Wall)
use File::Basename;
use Getopt::Std;


#
# Constants
#
use constant DEBUG_LEVEL   => 1;
use constant LOGFILE       => "bpsr_src_logger.log";


#
# Global Variable Declarations
#
our %cfg : shared = Bpsr::getBpsrConfig();      # Bpsr.cfg in a hash
our $log_socket;
our $log_fh;

#
# Local Variable Declarations
#
my $logfile = $cfg{"CLIENT_LOG_DIR"}."/".LOGFILE;
my $type = "INFO";
my $line = "";

#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

# Auto flush output
$| = 1;

my %opts;
getopts('e', \%opts);

if ($opts{e}) {
  $type = "ERROR";
}


# Open a connection to the nexus logging facility
$log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SRC_LOG_PORT"});
if (!$log_socket) {
  print "Could not open a connection to the nexus SRC log: $log_socket\n";
}

while (defined($line = <STDIN>)) {

  chomp $line;
  logMessage(0,$type,$line, $logfile);

}

exit 0;


#
# Logs a message to the Nexus
#
sub logMessage($$$$) {
  (my $level, my $type, my $message, my $logfile) = @_;
  if ($level <= DEBUG_LEVEL) {
    my $time = Dada::getCurrentDadaTime();
    if (!($log_socket)) {
      $log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SRC_LOG_PORT"});
    }
    if ($log_socket) {
      Dada::nexusLogMessage($log_socket, $time, "src", $type, "proc", $message);
    }
    open $log_fh, ">>".$logfile;
    print $log_fh "[".$time."] ".$message."\n";
    close $log_fh
  }
}

sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";

  if ($log_socket) {
    close($log_socket);
  }

  print STDERR basename($0)." : Exiting\n";

  exit 1;

}

sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  $log_socket = 0;
  $log_socket = Dada::nexusLogOpen($cfg{"SERVER_HOST"},$cfg{"SERVER_SRC_LOG_PORT"});

}


