package Dada::server_gain_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use IO::Socket;
use IO::Select;
use Net::hostent;
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
  %EXPORT_TAGS = ( );
  @EXPORT_OK   = qw($dl $daemon_name $hw_host $hw_port %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our $hw_gain_tag;
our $hw_host;
our $hw_port;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $warn;
our $error;
our $hw_sock;
our $gain_host;
our $gain_port;
our $gain_sock;
our $report_host;
our $report_port;
our $report_sock;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
$hw_gain_tag = "";
$hw_host = "";
$hw_port = 0;
%cfg = ();

#
# initialize other variables
#
$warn = ""; 
$error = ""; 
$quit_daemon = 0;
$hw_sock = 0;
$report_host = "";
$report_port = 0;
$report_sock = 0;
$gain_host = "";
$gain_port = 0;
$gain_sock = 0;

###############################################################################
#
# package functions
# 

sub main() {

  $warn  = $cfg{"STATUS_DIR"}."/".$daemon_name.".warn";
  $error = $cfg{"STATUS_DIR"}."/".$daemon_name.".error";

  my $pid_file    = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file   = $cfg{"SERVER_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $log_file    = $cfg{"SERVER_LOG_DIR"}."/".$daemon_name.".log";

  my $result = "";
  my $response = "";
  my $control_thread = 0;
  my $cmd = "";
  my $i = 0;
  my $rh = "";
  my $string = "";
  my @pol0_gains = ();
  my @pol1_gains = ();
  my $initial_gain = 2000;

  if ($cfg{"USE_DFB_SIMULATOR"} == 1) {
    $initial_gain = 33;
  }

  # set the host/port globals for the good() test
  $gain_host   = $cfg{"SERVER_HOST"};
  $gain_port   = $cfg{"SERVER_GAIN_CONTROL_PORT"};
  $report_host = $cfg{"SERVER_HOST"};
  $report_port = $cfg{"SERVER_GAIN_REPORT_PORT"};

  # sanity check on whether the module is good to go
  ($result, $response) = good($quit_file);
  if ($result ne "ok") {
    print STDERR $response."\n";
    return 1;
  }
  
  # install signal handlers
  $SIG{INT} = \&sigHandle;
  $SIG{TERM} = \&sigHandle;
  $SIG{PIPE} = \&sigPipeHandle;

  # become a daemon
  Dada::daemonize($log_file, $pid_file);
  
  # start the control thread
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  Dada::logMsg(0, $dl ,"STARTING SCRIPT");

  for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
    $pol0_gains[$i] = $initial_gain;
    $pol1_gains[$i] = $initial_gain;
  }

  Dada::logMsg(1, $dl, "Connecting to hardware gain interface: ".$hw_host.":".$hw_port);
  $hw_sock = Dada::connectToMachine($hw_host, $hw_port);
  if (!$hw_sock) {
    Dada::logMsgWarn($warn, "Could not connect to hardware gain interface, polling...");
    $hw_sock = 0;
  } else {
    Dada::logMsg(1, $dl, "Connection to hardware gain interface establised");
  }

  # create a read set to handle gain and report connectinos
  my $read_set = new IO::Select();
  $read_set->add($gain_sock);
  $read_set->add($report_sock);

  Dada::logMsg(1, $dl, "PWC gain socket opened ".$gain_host.":".$gain_port);
  Dada::logMsg(1, $dl, "PWC gain report opened ".$report_host.":".$report_port);

  my $handle = 0;
  my $hostinfo = 0;
  my $hostname = "";
  my $machine = "";
  my $ignore = "";
  my $gain = 0;
  my $chan = 0;
  my $pol = 0;
  my $val = 0;
  my $hw_string = "";
  my $hw_response = "";

  while (!$quit_daemon) {

    # If we haven't got a connection, try to open it
    if (!$hw_sock) {
      Dada::logMsg(2, $dl, "Trying to connect to HGI ".$hw_host.":".$hw_port);
      $hw_sock = Dada::connectToMachine($hw_host, $hw_port, 1);

      if ($hw_sock) {
        Dada::logMsg(0, $dl, "Connection to HGI re-established");
       $read_set->add($hw_sock);

      } else {
        Dada::logMsg(2, $dl, "Failed to connect to HGI");
        $hw_sock = 0;
      }
    }

    # Get all the readable handles from the server
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);

    foreach $rh (@$readable_handles) {

      if ($rh == $gain_sock) { 

        $handle = $rh->accept();
        $handle->autoflush();
        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;
        Dada::logMsg(2, $dl, "Accepting connection from ".$hostname);

        # Add this read handle to the set
        $read_set->add($handle); 
        $handle = 0;

      # If this is a connection on the socket asking for the current gains
      } elsif ($rh == $report_sock) {

        $handle = $rh->accept();
        $handle->autoflush();
        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;

        Dada::logMsg(2, $dl, "Accepting connection from ".$hostname);

        # Add this read handle to the set
        $read_set->add($handle);
        $handle = 0;

      # This is a connection from the hardware gain interface
      } elsif ($rh == $hw_sock) {

        $string = Dada::getLine($rh);
        if (! defined $string) {
          Dada::logMsg(0, $dl, "lost connection to hardware gain interface, re-polling...");
          $read_set->remove($rh);
          close($rh);
          $hw_sock = 0;
        } else {
          Dada::logMsgWarn($warn, "unexpected string from hardare gain interface ".$string);
        }

      } else {

        $hostinfo = gethostbyaddr($rh->peeraddr);
        $hostname = "unknown";
        $machine  = "unknown";

        if ($hostinfo) {
          $hostname = $hostinfo->name;
          my @parts = split(/\./,$hostname);
          $machine = $parts[0];
        }

        $string = Dada::getLine($rh);

        # If the string is not defined, then we have lost the connection.
        # remove it from the read_set
        if (! defined $string) {

          Dada::logMsg(2, $dl, "Lost connection from ".$hostname.", closing socket");
          $read_set->remove($rh);
          close($rh);

        # We have a request
        } else {

          Dada::logMsg(2, $dl, $machine.": ".$string);

          # If a client is asking what its CHANNEL base multiplier
          if ($string =~ m/CHANNEL_BASE/) {

            Dada::logMsg(2, $dl, $machine." -> ".$string);
            for ($i=0; $i<$cfg{"NUM_PWC"}; $i++) {
              if ($machine eq $cfg{"PWC_".$i}) {
                Dada::logMsg(2, $dl, $machine." <- ".$i);
                print $rh $i."\r\n";
                Dada::logMsg(2, $dl, "BASE: ".$machine." ".$i);
              }
            }

          # A gain "GET" command
          } elsif ( $string =~ m/^GAIN (\d+) (0|1)$/) {

            ($ignore, $chan, $pol) = split(/ /, $string);
            $hw_string = $hw_gain_tag." ".$chan." ".$pol;
            Dada::logMsg(2, $dl, $machine." -> ".$hw_string);

            $hw_response = "FAIL";

            if ($hw_sock) {
              Dada::logMsg(2, $dl, "  HWI <- ".$hw_string);
              print $hw_sock $hw_string."\n";
              $hw_response = Dada::getLine($hw_sock);
              Dada::logMsg(2, $dl, "  HWI -> ".$hw_response);
            }

            print $rh $hw_response."\r\n";

            Dada::logMsg(2, $dl, $machine." <- ".$hw_response);

          # A gain "SET" command
          } elsif ($string =~ m/^GAIN (\d+) (0|1) (\d)+$/) {

            Dada::logMsg(2, $dl, $machine." -> ".$string);

            ($ignore, $chan, $pol, $val) = split(/ /, $string);
            $hw_string = $hw_gain_tag." ".$chan." ".$pol." ".$val;
            $hw_response = "OK";

            if ($hw_sock) {
              Dada::logMsg(2, $dl, "  HWI <- ".$hw_string);
              print $hw_sock $hw_string."\n";
              $hw_response = Dada::getLine($hw_sock);
              Dada::logMsg(2, $dl, "  HWI -> ".$hw_response);
            }

            Dada::logMsg(2, $dl, $machine." <- ".$hw_response);
            print $rh $hw_response."\r\n";
            Dada::logMsg(2, $dl, $machine." -> ".$string."... ".$hw_response);

            if ($pol == 0) {
              $pol0_gains[$chan] = $val;
            } else {
              $pol1_gains[$chan] = $val;
            }


          } elsif ($string =~ m/^GAINHACK (\d+) (0|1) (\d)+$/) {
                                                                                      
            Dada::logMsg(2, $dl, $machine." -> ".$string);
                                                                                      
            ($ignore, $chan, $pol, $val) = split(/ /, $string);
            $hw_string = $hw_gain_tag." ".$chan." ".$pol." ".$val;
            $hw_response = "FAIL";
                                                                                      
            if ($hw_sock) {
              Dada::logMsg(2, $dl, "  HWI <- ".$hw_string);
              print $hw_sock $hw_string."\n";
              $hw_response = Dada::getLine($hw_sock);
              Dada::logMsg(2, $dl, "  HWI -> ".$hw_response);
            }
                                                                                      
            Dada::logMsg(2, $dl, $machine." <- ".$hw_response);
            print $rh $hw_response."\r\n";

            if ($pol == 0) {
              $pol0_gains[$chan] = $val;
            } else {
              $pol1_gains[$chan] = $val;
            }

          } elsif ($string =~ m/REPORT GAINS/) {

            my $j=0;
            my $tempstring = "";
            for ($j=0; $j<$cfg{"NUM_PWC"}; $j++) {
              $tempstring .= $pol0_gains[$j]." ".$pol1_gains[$j]." ";
            }
            print $rh $tempstring."\r\n";

          } else {
            Dada::logMsg(2, $dl, "Unknown request received");

          } 
        }
        Dada::logMsg(2, $dl, "=========================================");
      }
    }
  }

  # Rejoin our daemon control thread
  $control_thread->join();

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");
                                              
  return 0;
}


sub controlThread($$) {

  Dada::logMsg(1, $dl ,"controlThread: starting");

  my ($quit_file, $pid_file) = @_;

  Dada::logMsg(2, $dl ,"controlThread(".$quit_file.", ".$pid_file.")");

  # Poll for the existence of the control file
  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  # ensure the global is set
  $quit_daemon = 1;

  if ( -f $pid_file) {
    Dada::logMsg(2, $dl ,"controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    Dada::logMsgWarn($warn, "controlThread: PID file did not exist on script exit");
  }

  return 0;
}
  


#
# Handle a SIGINT or SIGTERM
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  $quit_daemon = 1;
  sleep(3);
  print STDERR $daemon_name." : Exiting\n";
  exit 1;
  
}

# 
# Handle a SIGPIPE
#
sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR $daemon_name." : Received SIG".$sigName."\n";
  close($hw_sock);
  $hw_sock = 0;

} 

#
# Test to ensure all module variables are set before main
#
sub good($) {

  my ($quit_file) = @_;

  # check the quit file does not exist on startup
  if (-f $quit_file) {
    return ("fail", "Error: quit file ".$quit_file." existed at startup");
  }

  # the calling script must have set this
  if (! defined($cfg{"INSTRUMENT"})) {
    return ("fail", "Error: package global hash cfg was uninitialized");
  }

  # this script can *only* be run on the configured server
  if (index($cfg{"SERVER_ALIASES"}, Dada::getHostMachineName()) < 0 ) {
    return ("fail", "Error: script must be run on ".$cfg{"SERVER_HOST"}.
                    ", not ".Dada::getHostMachineName());
  }

  # check some package globals
  if (($hw_host eq "") || ($hw_port eq 0)) {
    return ("fail", "Error: hardware host:port were not set: ".$hw_host.":".$hw_port);
  }

  # check sockets can be opened as per the configuration
  $gain_sock = new IO::Socket::INET (
    LocalHost => $gain_host,
    LocalPort => $gain_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$gain_sock) {
    return ("fail", "Error: could not open socket: ".$gain_host.":".$gain_port);
  }

  $report_sock = new IO::Socket::INET (
    LocalHost => $report_host,
    LocalPort => $report_port,
    Proto => 'tcp',
    Listen => 1, 
    Reuse => 1,
  );
  if (!$report_sock) {
    close ($gain_sock);
    return ("fail", "Error: could not open socket: ".$report_host.":".$report_port);
  }

  # Ensure more than one copy of this daemon is not running
  my ($result, $response) = Dada::checkScriptIsUnique(basename($0));
  if ($result ne "ok") {
    return ($result, $response);
  }

  return ("ok", "");

}


END { }

1;  # return value from file
