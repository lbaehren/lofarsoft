package Dada::client_archive_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use IO::Socket;
use File::Basename;
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
  @EXPORT_OK   = qw($dl $daemon_name %cfg);

}

our @EXPORT_OK;

#
# exported package globals
#
our $dl : shared;
our $daemon_name : shared;
our %cfg : shared;

#
# non-exported package globals go here
#
our $quit_daemon : shared;
our $log_host;
our $log_port;
our $log_sock;

#
# initialize package globals
#
$dl = 1; 
$daemon_name = 0;
%cfg = ();

#
# initialize other variables
#
$quit_daemon = 0;
$log_host = "";
$log_port = 0;
$log_sock = 0;


###############################################################################
#
# package functions
# 

sub main() {

  my $log_file       = $cfg{"CLIENT_LOG_DIR"}."/".$daemon_name.".log";;
  my $pid_file       = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".pid";
  my $quit_file      = $cfg{"CLIENT_CONTROL_DIR"}."/".$daemon_name.".quit";
  my $archive_dir    = $cfg{"CLIENT_ARCHIVE_DIR"};   # hi res archive storage
  my $results_dir    = $cfg{"CLIENT_RESULTS_DIR"};   # dspsr output directory

  $log_host = $cfg{"SERVER_HOST"};
  $log_port = $cfg{"SERVER_SYS_LOG_PORT"};

  my $control_thread = 0;
  my @lines = ();
  my $line;
  my $found_something;
  my $i=0;
  my $result = "";
  my $response = "";
  my $cmd = "";

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

  # Open a connection to the nexus logging port
  $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  if (!$log_sock) {
    print STDERR "Could open log port: ".$log_host.":".$log_port."\n";
  }

  logMsg(0,"INFO", "STARTING SCRIPT");

  # start the control thread
  logMsg(2, "INFO", "starting controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  # Change to the dspsr output directory
  chdir $results_dir;

  # Loop until daemon control thread asks us to quit
  while (!($quit_daemon)) {

    @lines = ();
    $found_something = 0;

    # Look for dspsr archives (both normal and pulse_ archives)
    $cmd = "find . -name \"*.ar\" | sort";
    logMsg(2, "INFO", "main: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "main: ".$result." ".$response);

    if ($result ne "ok") {
      logMsg(2, "WARN", "find command ".$cmd." failed ".$response);

    } else {

      @lines = split(/\n/,$response);
      for ($i=0; (($i<=$#lines) && (!$quit_daemon)); $i++) {

        $line = $lines[$i];
        $found_something = 1;

        # strip the leading ./ if it exists
        $line =~ s/^.\///;

        if (!($line =~ /pulse_/)) {
          logMsg(1, "INFO", "Processing archive ".$line);
        }

        ($result, $response) = processArchive($line, $archive_dir);
      }
    }

    # Look for 2 minute old .dada files that may have been produced in the results dir via baseband mode (i.e. P427)
    $cmd = "find . -name \"*.dada\" -cmin +1 | sort";
    logMsg(2, "INFO", "main: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "main: ".$result." ".$response);

    if ($result ne "ok") {
      logMsg(2, "WARN", "find command ".$cmd." failed ".$response);

    } else {
       @lines = split(/\n/,$response);
      for ($i=0; (($i<=$#lines) && (!$quit_daemon)); $i++) {

        $line = $lines[$i];
        $found_something = 1;

        # strip the leading ./ if it exists
        $line =~ s/^.\///;

        if (!($line =~ /pulse_/)) {
          logMsg(1, "INFO", "Processing baseband file ".$line);
        }

        ($result, $response) = processBasebandFile($line, $archive_dir);
      }
    }

    # If we didn't find any archives, sleep.
    if (!($found_something)) {
      sleep(5);
    }

  }

  # Rejoin our daemon control thread
  $control_thread->join();

  logMsg(0, "INFO", "STOPPING SCRIPT");

  Dada::nexusLogClose($log_sock);

  return 0;

}


#
# Decimate the archive with the psh_script
#
sub decimateArchive($$) {

  my ($file, $psh_script) = @_;

  logMsg(2, "INFO", "decimateArchive(".$file.", ".$psh_script.")");
  my $cmd = "";
  my $result = "";
  my $response = "";
  my $bindir = Dada::getCurrentBinaryVersion();

  logMsg(2, "INFO", "Decimating archive ".$file);

  # if there are more than 16 channels
  $cmd = $bindir."/".$psh_script." ".$file;

  logMsg(2, "INFO", "decimateArchive: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "decimateArchive ".$result." ".$response);

  if ($result ne "ok") {
    logMsg(0, "WARN", $psh_script." failed to decimate ".$file.": ".$response);
    return ("fail", "decimation failed ".$response);
  } else {
    $file =~ s/.ar$/.lowres/;
    return ("ok", $file);
  }
}


#
# Process an archive. Decimate it, send the decimated archive to srv via
# NFS then move the high res archive to the storage area
#
sub processArchive($$) {

  my ($path, $archive_dir) = @_;

  logMsg(2, "INFO", "processArchive(".$path.", ".$archive_dir.")");

  my $result = "";
  my $response = "";
  my $dir = "";
  my $file = "";
  my $file_decimated = "";
  my $i = 0;
  my $nfs_results =  $cfg{"SERVER_RESULTS_NFS_MNT"};
  my $cmd = "";

  # get the directory the file resides in. This may be 2 subdirs deep
  my @parts = split(/\//, $path);

  for ($i=0; $i<$#parts; $i++) {
    # skip the leading ./
    if ($parts[$i] ne ".") {
      $dir .= $parts[$i]."/";
    }
  }

  # remove the trailing slash
  $dir =~ s/\/$//;

  $file = $parts[$#parts];
  logMsg(2, "INFO", "processArchive: ".$dir."/".$file);

  # We never decimate pulse archives, or transfer them to the server
  if ($file =~ m/^pulse_/) {

    # do nothing

  } else {
    $file_decimated = decimateArchive($dir."/".$file, "lowres.psh");

    if (-f $file_decimated) {
      logMsg(2, "INFO", "processArchive: nfsCopy(".$file_decimated.", ".$nfs_results.", ".$dir);
      ($result, $response) = nfsCopy($file_decimated, $nfs_results, $dir);
      logMsg(2, "INFO", "processArchive: nfsCopy() ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", "procesArchive: nfsCopy() failed: ".$response);
      }

      logMsg(2, "INFO", "processArchive: unlinking ".$file_decimated);
      unlink($file_decimated);
    } else {
      logMsg(0, "WARN", "procesArchive: decimateArchive failed: ".$response);
    }

  }

  logMsg(2, "INFO", "processArchive: moving ".$file." to archive");

  if (! -d ($archive_dir."/".$dir)) {
    logMsg(2, "INFO", "processArchive: creating archive dir ".$archive_dir."/".$dir);

    $cmd = "mkdir -p ".$archive_dir."/".$dir;
    logMsg(2, "INFO", "processArchive: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    logMsg(2, "INFO", "processArchive: ".$result." ".$response);
    if ($result ne "ok") {
      logMsg(0, "WARN", "failed to create archive dir: ".$response);
    }
  }
  
  $cmd = "mv ".$dir."/".$file." ".$archive_dir."/".$dir."/";
  logMsg(2, "INFO", "processArchive: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "processArchive: ".$result." ".$response);
  
  if ($result ne "ok") {
    logMsg(0, "WARN", "failed to move ".$file." to archive dir ".$response);
  }
  
  # Add the archive to the accumulated totals
  if (!($file =~ m/^pulse_/)) {
    my $fres_archive = $archive_dir."/".$dir."/band.fres";
    my $tres_archive = $archive_dir."/".$dir."/band.tres";

    # If this is the first file, simply copy it
    if (! -f ($fres_archive)) {

      # The T scrunched (fres) archive
      $cmd = "cp ".$archive_dir."/".$dir."/".$file." ".$fres_archive;
      logMsg(2, "INFO", "processArchive: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "processArchive: ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", $cmd." failed: ".$response);
      }

      # The F scrunched (tres) archive
      $cmd = "pam -F -e tres ".$fres_archive;
      logMsg(2, "INFO", "processArchive: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "processArchive: ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", $cmd." failed: ".$response);
      }

    # Otherwise we are appending to the archive
    } else {

      # Add the archive to the T scrunched total
      $cmd = "psradd -T -o ".$fres_archive." ".$fres_archive." ".$archive_dir."/".$dir."/".$file;
      logMsg(2, "INFO", "processArchive: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "processArchive: ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", $cmd." failed: ".$response);
      }

      # Add the archive to the F scrunched total
      $cmd = "psradd -jF -o ".$tres_archive." ".$tres_archive." ".$archive_dir."/".$dir."/".$file;
      logMsg(2, "INFO", "processArchive: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "processArchive: ".$result." ".$response);
      if ($result ne "ok") {
        logMsg(0, "WARN", $cmd." failed: ".$response);
      }

    }
  }
  
  return ("ok", "");
}

#
# Process an archive. Decimate it, send the decimated archive to srv via
# NFS then move the high res archive to the storage area
#   
sub processBasebandFile($$) {

  my ($path, $archive_dir) = @_;
    
  logMsg(1, "INFO", "processBasebandFile(".$path.", ".$archive_dir.")");

  my $result = "";
  my $response = "";
  my $dir = ""; 
  my $file = "";
  my $file_decimated = "";
  my $i = 0;
  my $cmd = "";

  # get the directory the file resides in. This may be 2 subdirs deep
  my @parts = split(/\//, $path);

  for ($i=0; $i<$#parts; $i++) {
    # skip the leading ./
    if ($parts[$i] ne ".") {
      $dir .= $parts[$i]."/";
    }
  }

  # remove the trailing slash
  $dir =~ s/\/$//;

  $file = $parts[$#parts];
  logMsg(1, "INFO", "processBasebandFile: ".$dir."/".$file);

   $cmd = "mv ".$dir."/".$file." ".$archive_dir."/".$dir."/";
  logMsg(1, "INFO", "processBasebandFile: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(2, "INFO", "processBasebandFile: ".$result." ".$response);

  return ($result, $response); 

}




#
# Copy a the file in dir to nfsdir
#
sub nfsCopy($$$) {
  
  (my $file, my $nfsdir, my $dir) = @_;
  
  my $result = "";
  my $response = "";
  
  # If the nfs dir isn't automounted, ensure it is
  if (! -d $nfsdir) {
    `ls $nfsdir >& /dev/null`;
  }
  
  if (! -d $nfsdir."/".$dir) {
     logMsg(0, "WARN", "nfsCopy: ".$nfsdir."/".$dir." did not exist, creating");
    `mkdir -p $nfsdir/$dir`;
  }
     
  my $tmp_file = $file.".tmp";
  
  my $cmd = "cp ".$file." ".$nfsdir."/".$tmp_file;
  logMsg(2, "INFO", "NFS copy \"".$cmd."\"");
  ($result, $response) = Dada::mySystem($cmd,0);

  if ($result ne "ok") {
    return ("fail", "Command was \"".$cmd."\" and response was \"".$response."\"");
  } else {
    $cmd = "mv ".$nfsdir."/".$tmp_file." ".$nfsdir."/".$file;
    logMsg(2, "INFO", $cmd);
    ($result, $response) = Dada::mySystem($cmd,0);
    if ($result ne "ok") {
      return ("fail", "Command was \"".$cmd."\" and response was \"".$response."\"");
    } else {
      return ("ok", "");
    }
  }

}



sub controlThread($$) {

  logMsg(1, "INFO", "controlThread: starting");

  my ($quit_file, $pid_file) = @_;

  logMsg(2, "INFO", "controlThread(".$quit_file.", ".$pid_file.")");
  
  # Poll for the existence of the control file
  while ((!(-f $quit_file)) && (!$quit_daemon)) {
    sleep(1);
  }

  # ensure the global is set
  $quit_daemon = 1;

  logMsg(1, "INFO", "controlThread: unlinking PID file");
  if (-f $pid_file) {
    unlink($pid_file);
  }
  
  return 0;
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
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "arch mngr", $msg);
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
  
  # Tell threads to try and quit
  $quit_daemon = 1;
  sleep(3);
  
  if ($log_sock) {
    close($log_sock);
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
  $log_sock = 0;
  if ($log_host && $log_port) {
    $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  }

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

  if ( $daemon_name eq "") {
    return ("fail", "Error: a package variable missing [daemon_name]");
  }

  if (! -d $cfg{"CLIENT_ARCHIVE_DIR"} ) {
    return("fail", "Error: archive dir ".$cfg{"CLIENT_ARCHIVE_DIR"}." did not exist");
  }

  if (! -d $cfg{"CLIENT_RESULTS_DIR"} ) {
    return ("fail", "Error: results dir ".$cfg{"CLIENT_RESULTS_DIR"}." did not exist");
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
