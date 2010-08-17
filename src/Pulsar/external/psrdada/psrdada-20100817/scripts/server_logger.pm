package Dada::server_logger;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($log_name $dl $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $log_name;
our $dl;
our $daemon_name;
our %cfg;

#
# non-exported package globals go here
#

#
# initialize package globals
#
$log_name = "";
$dl = 1; 
$daemon_name = "";
%cfg = ();

#
# initialize other variables
#


###############################################################################
#
# package functions
# 

sub main() {

  my $line     = "";
  my $time     = "";

  $time = Dada::getCurrentDadaTime();
  logMsg(2, $time, "server_logger::".$daemon_name);

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;

  logMsg(2, $time, "server_logger: begin reading from STDIN");

  while (defined($line = <STDIN>)) {

    chomp $line;

    if ($line =~ m/^\[(\d\d\d\d)\-(\d\d)\-(\d\d)\-(\d\d):(\d\d):(\d\d)\]/) {
      $time = substr($line,1,19);
      $line = substr($line,22);

      if ($line =~ m/^WARN: /) {
        $line = substr($line,6);
      }

      if ($line =~ m/^ERROR: /) {
        $line = substr($line,7);
      }
  
    } else {
      $time = Dada::getCurrentDadaTime();

    }

    logMsg(0, $time, $line);

  }

  $time = Dada::getCurrentDadaTime();
  logMsg(2,  $time, "server_logger: finished reading from STDIN");
  
  return  0;
}  


#
# logs a message to the nexus logger and prints to stdout
#
sub logMsg($$$) {

  my ($level, $time, $msg) = @_;

  if ($level <= $dl) {

    if ($daemon_name ne "") {
      print STDOUT "[".$time."] [".$log_name."] ".$msg."\n";
    } else {
      print STDOUT "[".$time."] ".$msg."\n";
    }
  }
}


#
# Handle a SIGINT or SIGTERM
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}


END { }

1;  # return value from file
