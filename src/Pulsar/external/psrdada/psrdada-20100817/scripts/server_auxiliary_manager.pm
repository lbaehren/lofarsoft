package Dada::server_auxiliary_manager;

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
  @EXPORT_OK   = qw($dl $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl;
our $daemon_name;
our %cfg;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $warn;
our $error;
our $server_host;
our $client_sock;
our $client_port;
our $assist_sock;
our $assist_port;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
%cfg = ();

#
# initialize other variables
#
$warn = ""; 
$error = ""; 
$quit_daemon = 0;
$server_host = "";
$client_port = 0;
$client_sock = 0;
$assist_port = 0;
$assist_sock = 0;

use constant PROCESSED_FILE_NAME => "processed.txt";


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

  $server_host = $cfg{"SERVER_HOST"};
  $client_port  = $cfg{"SERVER_AUX_CLIENT_PORT"};
  $assist_port  = $cfg{"SERVER_AUX_ASSIST_PORT"};

  # PWC's contact this socket for help
  my @client_hosts = ();
  my @client_rhs   = ();

  # Aux processors register helpfulness here
  my @helper_hosts = ();
  my @helper_ports = ();
  my @helper_rhs   = ();
  my $helper_host  = "";
  my $helper_port  = "";
  my $helper_rh    = 0;
  
  my $control_thread = 0;
  my $result = "";
  my $response = "";
  my $cmd = "";
  my $i = 0;
  my $rh = "";
  my $line = "";
  my $read_set = 0;
  my $handle = 0;
  my $hostinfo = 0;
  my $hostname = "";
  my $host = "";
  my $arg = "";

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

  Dada::logMsg(0, $dl ,"STARTING SCRIPT");

  # start the control thread
  Dada::logMsg(2, $dl ,"starting controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # create handle set for reading
  $read_set = new IO::Select();
  $read_set->add($client_sock);
  $read_set->add($assist_sock);

  Dada::logMsg(2, $dl, "Listening for clients ".$server_host.":".$client_port);
  Dada::logMsg(2, $dl, "Listening for helpers ".$server_host.":".$assist_port);

  while (!$quit_daemon) {

    # Get all the readable handles from the server
    my ($readable_handles) = IO::Select->select($read_set, undef, undef, 2);

    foreach $rh (@$readable_handles) {

      # if it is the main socket then we have an incoming connection and
      # we should accept() it and then add the new socket to the $Read_Handles_Object

      $handle = $rh->accept();

      # accept a conncection from a pwc client
      if ($rh == $client_sock) { 

        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;
        $handle->autoflush();
        Dada::logMsg(2, $dl, "Accepting client connection: ". $hostname);
        $read_set->add($handle);
        $handle = 0;

      # accept a conncection from an assisting node
      } elsif ($rh == $assist_sock) {

        $hostinfo = gethostbyaddr($handle->peeraddr);
        $hostname = $hostinfo->name;
        $handle->autoflush();
        Dada::logMsg(2, $dl, "Accepting assist connection: ". $hostname);
        $read_set->add($handle);
        $handle = 0;

      # read some data from a connceted node
      } else {

        $line = Dada::getLine($rh);

        # we have received an EOF on the socket, indicates closure
        if (! defined $line) {

          for ($i=0; $i<=$#helper_rhs; $i++) {
            if ($helper_rhs[$i] eq $rh) {
              Dada::logMsg(2, $dl, "Helper disconnect: ".$helper_hosts[$i]);
              splice(@helper_hosts, $i, 1);
              splice(@helper_ports, $i, 1);
              splice(@helper_rhs, $i, 1);
            }
          }

          for ($i=0; $i<=$#client_rhs; $i++) {
            if ($client_rhs[$i] eq $rh) {
              Dada::logMsg(2, $dl, "Client disconnect: ".$client_hosts[$i]);
              splice(@client_hosts, $i, 1);
              splice(@client_rhs, $i, 1);
            }
          }

          $read_set->remove($rh);
          close($rh);

        # we have received a communication string on the socket, should be
        #   apsr##:help      for clients requesting help
        #   apsr##:<port>    for assisting nodes offering help
        } else {

          ($host, $arg) = split(/:/,$line, 2);
          Dada::logMsg(1, $dl, "[".$host."] -> ".$arg);

          # if a node is asking for help 
          if ($arg eq "help") {

            push(@client_hosts, $host);
            push(@client_rhs, $rh);

            # If we have at least 1 machine to help 
            if ($#helper_hosts >= 0) {

              Dada::logMsg(3, $dl, "We have help available");
         
              # grab the first helper from the arary 
              $helper_host = shift @helper_hosts;
              $helper_port = shift @helper_ports;
              $helper_rh = shift @helper_rhs;

              # tell the helper to expect incoming dbnic connection
              print $helper_rh "ack\r\n";
              Dada::logMsg(1, $dl, "[".$helper_host."] <- ack");

              # tell the client the host:port of the helping node
              print $rh $helper_host.":".$helper_port."\r\n";
              Dada::logMsg(1, $dl, "[".$host."] <- ".$helper_host.":".$helper_port);

            # we have no helper nodes available :(
            } else {

              # tell the client we have no help
              Dada::logMsg(1, $dl, "[".$host."] <- none");
              print $rh "none\r\n";

            }

          # an offer of assistance
          } elsif ($arg =~ /(\d+)/) {

            # add this resource to the array
            ($helper_host,$helper_port) = split(/:/,$arg,2);
            push(@helper_hosts, $helper_host);
            push(@helper_ports, $helper_port);
            push(@helper_rhs, $rh);

            # we dont send an "ack" to the helper until we need its help
          }
        }
      }
    }
  }

  for ($i=0; $i<=$#helper_rhs; $i++) {
    close $helper_rhs[$i];
  }
  for ($i=0; $i<=$#client_rhs; $i++) {
    close $client_rhs[$i];
  }

  # Rejoin our daemon control thread
  $control_thread->join();

  Dada::logMsg(0, $dl ,"STOPPING SCRIPT");

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

  $client_sock = new IO::Socket::INET (
    LocalHost => $server_host,
    LocalPort => $client_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$client_sock) {
    return ("fail", "Could not create listening socket: ".$server_host.":".$client_port);
  }

  $assist_sock = new IO::Socket::INET (
    LocalHost => $server_host,
    LocalPort => $assist_port,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );
  if (!$assist_sock) {
    return ("fail", "Could not create listening socket: ".$server_host.":".$assist_port);
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
