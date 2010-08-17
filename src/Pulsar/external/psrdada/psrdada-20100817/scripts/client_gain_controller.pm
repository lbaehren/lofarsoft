package Dada::client_gain_controller;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use IO::Socket;
use threads;
use threads::shared;
use Dada;

BEGIN {

  require Exporter;
  our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

  require AutoLoader;

  $VERSION = '1.00';

  @ISA         = qw(Exporter AutoLoader);
  @EXPORT      = qw(&main);
  %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

  # your exported package globals go here,
  # as well as any optionally exported functions
  @EXPORT_OK   = qw($dl $nchan $gain_min $gain_max $gain_default $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl : shared;
our $nchan;
our $gain_min;
our $gain_max;
our $gain_default;
our $daemon_name : shared;
our %cfg : shared;

#
# non-exported package globals go here
#
our $log_host : shared;
our $log_port : shared;
our $log_sock;
our $gain_host : shared;
our $gain_port : shared;
our $gain_sock;

#
# initialize package globals
#
$dl = 1; 
$nchan = 0;
$gain_min = 0;
$gain_max = 0;
$gain_default = 0;
$daemon_name = 0;
%cfg = ();

#
# initialize other variables
#
$log_host = 0;
$log_port = 0;
$log_sock = 0;
$gain_host = 0;
$gain_port = 0;
$gain_sock = 0;


###############################################################################
#
# package functions
# 

sub main() {

  my $log_file   = $cfg{"CLIENT_LOG_DIR"}."/".$daemon_name.".log";;
  my $pid_file   = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file  = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".quit";

  my $result = "";
  my $response = "";

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }

  $log_host = $cfg{"SERVER_HOST"};
  $log_port = $cfg{"SERVER_SYS_LOG_PORT"};
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  if (!$log_sock) {
    print STDERR "Could open log port: ".$log_host.":".$log_port."\n";
  }

  $gain_host = $cfg{"SERVER_HOST"};
  $gain_port = $cfg{"SERVER_GAIN_CONTROL_PORT"};
  $gain_sock = Dada::connectToMachine($gain_host, $gain_port, 1);
  if (!$gain_sock) {
    logMsg(0, "ERROR", "Could not connect to the server gain manager ".
                        $gain_host.":".$gain_port);
    close($log_sock);
    return 1;
  }

  logMsg(2, "INFO", "STARTING GAIN CONTROLLER");

  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;

  my @pol0_gains = ();
  my @pol1_gains = ();
  my $cmd = "";
  my $line = "";
  my $chan_base = 0;
  my $chan_abs  = 0;
  my $dfb_response = 0;
  my $current_gain = 0;
  my $new_gain = 0;
  my $gain_step = 0;
  my $max_gain = 0;
  my $i = 0;

  # initialize the gains levels
  for ($i=0; $i < $nchan; $i++) {
    $pol0_gains[$i] = $gain_default;
    $pol1_gains[$i] = $gain_default;
  }

  # <STDIN> command is GAIN <chan> <pol> <dim> where:
  #   <chan> varies from 0 to $nchan
  #   <pol>  is 0 or 1     
  #   <dim   is always 0
  #
  #  Therefore we always have a max on 2*nchan gain values to maintain 

  # Ask the server gain manager what channel we are
  $cmd = "CHANNEL_BASE";
  logMsg(1, "INFO", "srv0 <- ".$cmd);
  print $gain_sock $cmd."\r\n";
  $chan_base = Dada::getLine($gain_sock);
  logMsg(1, "INFO", "srv0 -> ".$chan_base);

  logMsg(2, "INFO", "Asking for initial gains");

  # Get the initial gain setting for every channel
  for ($i=0; $i < $nchan; $i++) {
    $chan_abs = ($nchan * $chan_base) + $i;
    ($result, $pol0_gains[$i]) = get_gain($gain_sock, $chan_abs, 0);
    ($result, $pol1_gains[$i]) = get_gain($gain_sock, $chan_abs, 1);
  }

  logMsg(2, "INFO", "Received initial gains");

  my $ignore = "";
  my $chan = 0;
  my $pol = 0;
  my $dim = 0;
  my $val = 0;
  #my $last_val = 0;

  # we now read gain commands from digimon's STDOUT which
  # has been piped to this scripts STDIN
  while (defined($line = <STDIN>)) {

    chomp $line;

    logMsg(2, "INFO", "STDIN : ".$line);

    # Check that we got a gain string from STDIN
    if ($line =~ /^GAIN (\d) (0|1) (0|1) (\d|\.)+$/) {

      ($ignore, $chan, $pol, $dim, $val) = split(/ /,$line);

      $chan_abs = ($nchan * $chan_base) + $chan;

      #if (($requested_val > $last_val + 5) || ($val < $last_val - 5)) {
      #  print $socket "BIG CHANGE: pol".$pol." ".$last_val." => ".$val."\r\n";
      #}

      #$last_val = $val;

      if ($pol eq 0) {
        $current_gain = $pol0_gains[$chan];
      } else {
        $current_gain = $pol1_gains[$chan];
      }

      logMsg(2, "INFO", "CHAN=".$chan.", CHAN_ABS=".$chan_abs.
                            ", POL=".$pol.", DIM=".$dim.", VAL=".$val);

      $new_gain = int($current_gain * $val);

      # clamp the values around min/max

      if ($new_gain > $gain_max) {
        $new_gain = $gain_max;
      }

      if ($new_gain < $gain_min) {
        $new_gain = $gain_min;
      }

      # Only forward message if gain is different
      if ($new_gain != $current_gain) {

        if ($new_gain >= $gain_max) {
          logMsg(0, "WARN", "Gain set to max ".$gain_max);
        }

        if ($new_gain <= $gain_min) {
          logMsg(0, "WARN", "Gain set to min ".$gain_min);
        }

        logMsg(2, "INFO", "gain change ".$current_gain." -> ".$new_gain);

        $cmd = "GAIN ".$chan_abs." ".$pol." ".$new_gain;
        logMsg(2, "INFO", "srv0 <- ".$cmd);

        $result = set_gain($gain_sock, $chan_abs, $pol, $current_gain, $new_gain);  

        logMsg(2, "INFO", "srv0 -> ".$result);

        if ($pol eq 0) {
          $pol0_gains[$chan] = $result;
        } else {
          $pol1_gains[$chan] = $result;
        }
      }
    
    } elsif ($line =~ /^LEVEL (\d) (0|1) (0|1) (-|\d|\.)+$/) {

      # Ignore LEVEL commands - only relevant to CPSR/2

    } else {

      logMsg(0, "WARN", "STDIN: Ignoring line ".$line);

    }
  }

  logMsg(2, "INFO", "STOPPING GAIN CONTROLLER");
  Dada::nexusLogClose($log_sock);
  close ($gain_sock);

  return 0;

}


sub usage() {
  print "Usage: ".$0." nchan\n";
  print "   chan   Total number of channels on this node\n";
}


sub get_gain($$$) {

  (my $sock, my $chan, my $pol) = @_;

  my $result = "";
  my $dfb_chan = "";
  my $dfb_pol = "";
  my $dfb_gain = "";
  my $cmd = "";


  $cmd = "GAIN ".$chan." ".$pol;
  logMsg(2, "INFO", "srv0 <- ".$cmd);

  print $sock $cmd."\r\n";

  # The DFB3 should return a string along the lines of
  # OK <chan> <pol> <value>
  my $dfb_response = Dada::getLine($sock);

  logMsg(2, "INFO", "srv0 -> ".$dfb_response);

  ($result, $dfb_chan, $dfb_pol, $dfb_gain) = split(/ /,$dfb_response);

  if ($result ne "OK") {
    logMsg(0, "WARN", "DFB3 returned an error: \"".$dfb_response."\"");
    return ("fail", 0);
  }

  if ($dfb_chan ne $chan) {
    logMsg(0, "WARN", "DFB3 returned an channel mismatch: requested ".$chan.", received ".$dfb_chan);
    return ("fail", 0);
  }

  if ($dfb_pol ne $pol) {
    logMsg(0, "WARN", "DFB3 returned an pol mismatch: requested ".$pol.", received ".$dfb_pol);
    return ("fail", 0);
  }

  return ("ok", $dfb_gain);

}

sub set_gain($$$$$) {

  my ($sock, $chan, $pol, $curr_val, $val) = @_;

  my $cmd = "GAIN ".$chan." ".$pol." ".$val;

  # logMsg(2, "INFO", "srv0 <- ".$cmd);
  
  print $sock $cmd."\r\n";
 
  # The DFB3 should return a string along the lines of
  # OK 

  my $dfb_response = Dada::getLine($sock);

  # logMsg(2, "INFO", "srv0 -> ".$dfb_response);

  if ($dfb_response ne "OK") {

    logMsg(0, "WARN", "DFB3 returned an error: \"".$dfb_response);
    return $curr_val;

  } else {

    return $val;
  }

}








#
# logs a message to the nexus logger and prints to stdout
#
sub logMsg($$$) {

  my ($level, $type, $msg) = @_;
  if ($level <= $dl) {
    my $time = Dada::getCurrentDadaTime();
    if (! $log_sock ) {
      $log_sock = Dada::nexusLogOpen($log_host, $log_port);
    }
    if ($log_sock) {
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "gain mon", $msg);
    }
    print "[".$time."] ".$msg."\n";
  }
}


#
# Handle a SIGINT or SIGTERM
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
 
  if ($log_sock) {
    close($log_sock);
  } 
  if ($gain_sock) {
    close($gain_sock);
  }

  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}

#
# Handle a SIGPIPE
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";

  close($log_sock);
  $log_sock = 0;
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);

  close($gain_sock);
  $gain_sock = 0;
  $gain_sock = Dada::connectToMachine($gain_host, $gain_port, 1);
  if (!$gain_sock) {
    logMsg(0, "WARN", "Could not connect to the server gain manager ".
                      $gain_host.":".$gain_port);
  }

}

sub good($) {

  my ($quit_file) = @_;

  my $result = "ok";
  my $response = "";

  if (-f $quit_file) {
    $result = "fail";  
    $response = "Error: quit file ".$quit_file." existed at startup";
  }

  # the calling script must have set this
  if (! defined($cfg{"INSTRUMENT"})) {
    return ("fail", "Error: package global hash cfg was uninitialized");
  }

  if ($nchan == 0) {
    $result = "fail";
    $response = "Error: package global scalar nchan was uninitialized";
  }

  return ($result, $response);

}


END { }

1;  # return value from file
