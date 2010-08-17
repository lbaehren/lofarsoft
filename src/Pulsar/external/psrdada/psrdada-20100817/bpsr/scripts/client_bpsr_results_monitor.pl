#!/usr/bin/env perl

###############################################################################
#
# client_bpsr_results_monitor.pl 
#
# This script transfers the output of the_decimator to the results directory
# on the server

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;
use Getopt::Std;
use File::Basename;
use Bpsr;               # BPSR/DADA Module for configuration options
use strict;             # strict mode (like -Wall)
use threads;
use threads::shared;


#
# Constants
#
use constant DAEMON_NAME => "bpsr_results_monitor";

#
# Global Variables
#
our $dl : shared;
our $quit_daemon : shared;
our %cfg : shared;
our $log_host;
our $log_port;
our $log_sock;


#
# Initialize globals
#
$dl = 1;
$quit_daemon = 0;
%cfg = Bpsr::getBpsrConfig();
$log_host = $cfg{"SERVER_HOST"};
$log_port = $cfg{"SERVER_SYS_LOG_PORT"};
$log_sock = 0;


#
# Local Variables
#
my $log_file = "";
my $pid_file = "";
my $quit_file = "";
my $control_thread = 0;
my $client_archive_dir = $cfg{"CLIENT_ARCHIVE_DIR"};

# Autoflush STDOUT
$| = 1;

if (!(-d($client_archive_dir))) {
  print STDERR "Error: CLIENT_ARCHIVE_DIR ".$client_archive_dir." did not exist\n";
  exit(-1);
}


# Register signal handlers
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;

$log_file = $cfg{"CLIENT_LOG_DIR"}."/".DAEMON_NAME.".log";
$pid_file = $cfg{"CLIENT_CONTROL_DIR"}."/".DAEMON_NAME.".pid";
$quit_file = $cfg{"CLIENT_CONTROL_DIR"}."/".DAEMON_NAME.".quit";

# Redirect standard output and error
Dada::daemonize($log_file, $pid_file);

# Open a connection to the server_sys_monitor.pl script
$log_sock = Dada::nexusLogOpen($log_host, $log_port);
if (!$log_sock) {
  print STDERR "Could open log port: ".$log_host.":".$log_port."\n";
}

logMsg(0,"INFO", "STARTING SCRIPT");

# Change to the local archive directory
chdir $client_archive_dir;

#
# Main Control Loop
#

if ( -f $quit_file ) {

  logMsg(0,"INFO", "STOPPING SCRIPT");
  Dada::nexusLogClose($log_sock);
  exit(1);

}

$control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

my $mon_files_processed = 0;
my $dspsr_files_processed = 0;

my $sleep_total = 5;
my $sleep_count = 0;

# Loop until daemon control thread asks us to quit
while (!($quit_daemon)) {

  $mon_files_processed = processMonFiles();
  $dspsr_files_processed = process_dspsr_files();

  # If nothing is happening, have a snooze
  if (($mon_files_processed + $dspsr_files_processed) < 1) {

    $sleep_count = 0;
    while (!$quit_daemon && ($sleep_count < $sleep_total)) {
      sleep(1);
      $sleep_count++;
    }
  }
}

# Rejoin our daemon control thread
$control_thread->join();

logMsg(0, "INFO", "STOPPING SCRIPT");

# Close the nexus logging connection
Dada::nexusLogClose($log_sock);


exit (0);


###############################################################################################
##
## Functions
##

#
# Look for mon files produced by the_decimator, plot them and send the plots to the server
#
sub processMonFiles() {

  my $result = "";
  my $response = "";
  my $cmd = "";

  my %unprocessed = ();
  my @mon_files = ();
  my $mon_file = "";
  my @keys = ();
  my $key = "";
  my $plot_file = "";
  my @plot_files = ();

  # get ALL unprocessed mon files (not checking aux dirs)
  $cmd = "find . -maxdepth 3 -regex '.*[ts|bp|bps][0|1]' -printf '\%P\n' | sort";
  logMsg(3, "INFO", "processMonFiles: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  logMsg(3, "INFO", "processMonFiles: ".$result." ".$response);

  @mon_files = split(/\n/, $response);

  # dont bother if no mon files exist
  if ($#mon_files == -1) {
    return 0;
  }

  # add up how many of each pol we have
  foreach $mon_file (@mon_files) {
    logMsg(3, "INFO", "processMonFiles: ".$mon_file);
    $key = substr $mon_file, 0, -1;
    if (! exists ($unprocessed{$key})) {
      $unprocessed{$key} = 1;
    } else {
      $unprocessed{$key} += 1;
    }
  }

  # remove ones with only 1 pol
  @keys = keys (%unprocessed);
  foreach $key (@keys) {

    if ($unprocessed{$key} == 1) {
      # sometimes only 1 pol is outputted. if the file is > 1 minute in age
      # then delete it too
      $cmd = "stat -c \%Y ".$key."*";
      logMsg(2, "INFO", "processMonFiles: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      logMsg(2, "INFO", "processMonFiles: ".$result." ".$response);
      if ((time - $response) > 60) {
        logMsg(2, "INFO", "processMonFiles: unlinking orphaned mon file ".$key);
        $cmd = "rm -f ".$key."0 ".$key."1";
        system($cmd);
      }

      delete($unprocessed{$key});
    }
  }

  @keys = keys (%unprocessed);
  logMsg(2, "INFO", "Processing ".(($#keys+1)*2)." mon files");

  my $utc = "";
  my $beam = "";
  my $file = "";
  my $pol0_file = "";
  my $pol1_file = "";
  my $file_dir = "";

  foreach $key (@keys) {

    logMsg(2, "INFO", "processMonFiles: processing ".$key."[0|1]");
    ($utc, $beam, $file) = split( /\//, $key);
    $file_dir = $utc."/".$beam;

    # Plot the mon files and return a list of plot files that have been created
    logMsg(2, "INFO", "processMonFiles: plotMonFile($file_dir, $file)");
    @plot_files = plotMonFile($file_dir, $file);
    logMsg(2, "INFO", "processMonFiles: plotMonFile returned ".($#plot_files+1)." files");

    foreach $plot_file (@plot_files) {

      logMsg(2, "INFO", "NFS copy ".$file_dir."/".$plot_file);
      ($result, $response) = sendToServerViaNFS($file_dir."/".$plot_file, $cfg{"SERVER_RESULTS_NFS_MNT"}, $file_dir);
      logMsg(3, "INFO", "send result: ".$result." ".$response);
      unlink($file_dir."/".$plot_file);

    }

    $pol0_file = $file."0";
    $pol1_file = $file."1";

    my $aux_dir = $file_dir."/aux";
    my $aux_tar = $file_dir."/aux.tar";

    if (! -d $aux_dir) {
      logMsg(0, "WARN", "Aux file dir for ".$utc.", beam ".$beam." did not exist, creating...");
      `mkdir -p $aux_dir`;
    }

    # move both pols of the mon files to the aux dir
    $cmd  = "mv ".$file_dir."/".$pol0_file." ".$file_dir."/".$pol1_file." ".$file_dir."/aux/";
    ($result, $response) = Dada::mySystem($cmd,0);

    if ($result ne "ok") {

      logMsg(0, "ERROR", "Could not move file ($file) to aux dir \"".$response."\"");
      unlink($file_dir."/".$file);

    } else {

      # add the mon file to the .tar archive
      chdir $file_dir ;

      if (! -f "./aux.tar") {
        $cmd = "tar -cf ./aux.tar aux/".$pol0_file." aux/".$pol1_file;
      } else {
        $cmd = "tar -rf ./aux.tar aux/".$pol0_file." aux/".$pol1_file;
      }

      ($result, $response) = Dada::mySystem($cmd,0);
      if ($result ne "ok") {
        logMsg(0, "ERROR", "Could not add file (".$pol0_file.", ".$pol1_file.") to aux.tar: ".$response);
      }

      chdir $cfg{"CLIENT_ARCHIVE_DIR"};
    }
    logMsg(2, "INFO", "Decimator mon file processed: ".$key);
  }

  # return the number
  return (($#keys+ 1)*2);
}

#
# process any dspsr output files produced
#
sub process_dspsr_files() {

  my $cmd = "find . -maxdepth 3 -name \"20*\.ar\" | sort";
  my $find_result = `$cmd`;

  my @lines = split(/\n/,$find_result);
  my $line = "";

  my $result = "";
  my $response = "";

  my $bindir = Dada::getCurrentBinaryVersion();

  foreach $line (@lines) {

    $line = substr($line,2);

    logMsg(2, "INFO", "Processing dspsr file \"".$line."\"");

    my ($utc, $beam, $file) = split( /\//, $line);
                                                                                                                    
    my $file_dir = $utc."/".$beam;

    my $sumd_archive = $file_dir."/integrated.ar";
    my $curr_archive = $file_dir."/".$file;
    my $temp_archive = $file_dir."/temp.ar";

    if (! -f $sumd_archive) {
      $cmd = "cp ".$curr_archive." " .$temp_archive;
    } else {
      $cmd = $bindir."/psradd -s -f ".$temp_archive." ".$sumd_archive." ".$curr_archive;
    }    

    logMsg(2, "INFO", $cmd); 
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {

      logMsg(0, "ERROR", "process_dspsr_files: ".$cmd." failed: ".$response);
      unlink($temp_archive);

    } else {

      logMsg(2, "INFO", "process_dspsr_files: archive added");

      logMsg(2, "INFO", "unlink(".$sumd_archive.")");
      unlink($sumd_archive);
      logMsg(2, "INFO", "rename(".$temp_archive.", ".$sumd_archive.")");
      rename($temp_archive, $sumd_archive);

      my $plot_file = "";
      my $plot_binary = $bindir."/psrplot";
      my $time_str = Dada::getCurrentDadaTime();

      $plot_file = makePhaseVsFreqPlotFromArchive($plot_binary, $time_str, "112x84", $sumd_archive, $file_dir);
      if ($plot_file ne "none") {
         ($result, $response) = sendToServerViaNFS($file_dir."/".$plot_file, $cfg{"SERVER_RESULTS_NFS_MNT"}, $file_dir);        
         unlink($plot_file);
      }

      $plot_file = makePhaseVsFreqPlotFromArchive($plot_binary, $time_str, "400x300", $sumd_archive, $file_dir);
      if ($plot_file ne "none") {
         ($result, $response) = sendToServerViaNFS($file_dir."/".$plot_file, $cfg{"SERVER_RESULTS_NFS_MNT"}, $file_dir);
         unlink($plot_file);
      }

      $plot_file = makePhaseVsFreqPlotFromArchive($plot_binary, $time_str, "1024x768", $sumd_archive, $file_dir);
      if ($plot_file ne "none") {
         ($result, $response) = sendToServerViaNFS($file_dir."/".$plot_file, $cfg{"SERVER_RESULTS_NFS_MNT"}, $file_dir);
         unlink($plot_file);
      }
    }

    unlink ($curr_archive);

  }

  return ($#lines+1);

}


#
# create a phase vs freq plot from a dspsr archive
#
sub makePhaseVsFreqPlotFromArchive($$$$$) {

  my ($binary, $timestr, $res, $archive, $dir) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $add = "";
  my $args = "";
  my $plotfile = "";


  # remove axes, labels and outside spaces for lil'plots
  if ($res eq "112x84") {
    $add = " -s ".$cfg{"SCRIPTS_DIR"}."/web_style.txt -c below:l=unset";
  }

  $args = "-g ".$res.$add." -jp -p freq -jT -j\"zap median,F 256,D\"";
  $plotfile = $timestr.".pvf_".$res.".png";

  $cmd = $binary." ".$args." -D ".$dir."/".$plotfile."/png ".$archive;

  logMsg(2, "INFO", $cmd);
  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    logMsg(0, "ERROR", "plot cmd \"".$cmd."\" failed: ".$response);
    $plotfile = "none";
  }

  return $plotfile;

}

sub plotMonFile($$) {

  my ($file_dir, $mon_file) = @_;

  logMsg(3, "INFO", "plotMonFile(".$file_dir.", ".$mon_file.")");

  my @plot_files = ();
  my $filetype = "";
  my $filebase = "";
  my $bindir =  Dada::getCurrentBinaryVersion();
  my $binary = $bindir."/plot4mon";
  my $cmd = "";
  my $result = "";
  my $response = "";

  chdir $file_dir;

  # Delete any old images in this directory
  my $response;

  if ($mon_file =~ m/bp$/) {
    $filetype = "bandpass";
    $filebase = substr $mon_file, 0, -3;

  } elsif ($mon_file =~ m/bps$/) {
    $filetype = "bandpass_rms";
    $filebase = substr $mon_file, 0, -4;

  } elsif ($mon_file =~ m/ts$/) {
    $filetype = "timeseries";
    $filebase = substr $mon_file, 0, -3;

  } else {
    $filetype = "unknown";
    $filebase = "";
  }

  logMsg(2, "INFO", "plotMonFile: ".$filetype." ".$filebase);

  if ($filetype eq "unknown") {
    # skip the file
    logMsg(2, "INFO", "plotMonFile: unknown");

  } else {

    # Create the low resolution file
    $cmd = $binary." ".$mon_file."0 ".$mon_file."1 -G 112x84 -nobox -nolabel -g /png";
    if ($filetype eq "timeseries") {
      $cmd = $cmd." -mmm";
    }

    logMsg(2, "INFO", "plotMonFile: ".$cmd);
    $response = `$cmd 2>&1`;
    if ($? != 0) {
      logMsg(0, "WARN", "plotMonFile: ".$cmd." failed: ".$response);

    } else {

      ($result, $response) = renamePgplotFile($mon_file.".png", $mon_file."_112x84.png");
      if ($result eq "ok") {
        push @plot_files, $mon_file."_112x84.png"; 
      } else {
        logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
      }

      if ($filetype eq "timeseries") {

        ($result, $response) = renamePgplotFile($filebase.".fft.png", $filebase.".fft_112x84.png");
        if ($result eq "ok") {
          push @plot_files, $filebase.".fft_112x84.png";
        } else {
          logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
        }
      }
    }

    # Create the mid resolution file
    $cmd = $binary." ".$mon_file."0 ".$mon_file."1 -G 400x300 -g /png";
    if ($filetype eq "timeseries") {
      $cmd = $cmd." -mmm";
    }

    logMsg(2, "INFO", "plotMonFile: ".$cmd);
    $response = `$cmd 2>&1`;
    if ($? != 0) {
      logMsg(0, "WARN", "plotMonFile: ".$cmd." failed: ".$response);
    } else {

      ($result, $response) = renamePgplotFile($mon_file.".png", $mon_file."_400x300.png");
      if ($result eq "ok") {
        push @plot_files, $mon_file."_400x300.png";
      } else {
        logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
      }

      if ($filetype eq "timeseries") {
        ($result, $response) = renamePgplotFile($filebase.".fft.png", $filebase.".fft_400x300.png");
        if ($result eq "ok") {
          push @plot_files, $filebase.".fft_400x300.png";
        } else {
          logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
        }
      }
    }

    # Create the high resolution file
    $cmd = $binary." ".$mon_file."0 ".$mon_file."1 -G 1024x768 -g /png";
    if ($filetype eq "timeseries") {
      $cmd = $cmd." -mmm";
    }

    logMsg(2, "INFO", "plotMonFile: ".$cmd);
    $response = `$cmd 2>&1`;
    if ($? != 0) {
      logMsg(0, "WARN", "plotMonFile: ".$cmd." failed: ".$response);
    } else {
      ($result, $response) = renamePgplotFile($mon_file.".png", $mon_file."_1024x768.png");
      if ($result eq "ok") {
        push @plot_files, $mon_file."_1024x768.png";
      } else {
        logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
      }
    
      if ($filetype eq "timeseries") {
        ($result, $response) = renamePgplotFile($filebase.".fft.png", $filebase.".fft_1024x768.png");
        if ($result eq "ok") {
          push @plot_files, $filebase.".fft_1024x768.png";
        } else {
          logMsg(1, "INFO", "plotMonFile: rename failed: ".$response);
        }
      }
    }
  }

  chdir $cfg{"CLIENT_ARCHIVE_DIR"};
  return @plot_files;

}


#
# renames a file from src to dst, but waits for the file to appear as PGPLOT
# programs seems to return before the file has appeared on disk
#
sub renamePgplotFile($$) {

  my ($src, $dst) = @_;

  my $waitMax = 5;   # seconds
  my $cmd = "";
  my $result = "";
  my $response = "";

  while ($waitMax) {
    if (-f $src) {
      $waitMax = 0;
    } else {
      $waitMax--;
      sleep(1);
    }
  }
 
  if ( -f $src ) {
     $cmd = "mv ".$src." ".$dst;
    ($result, $response) = Dada::mySystem($cmd);
  } else {
    $result = "fail";
    $response = "renamePgplotFile: ".$src." did not appear after 5 seconds";
  }

  return ($result, $response);

}


sub sendToServerViaNFS($$$) {

  (my $file, my $nfsdir,  my $dir) = @_;

  my $result = "";
  my $response = "";

  # If the nfs dir isn't automounted, ensure it is
  if (! -d $nfsdir) {
    `ls $nfsdir >& /dev/null`;
  }

  if (! -d $nfsdir."/".$dir) {
    `mkdir -p $nfsdir/$dir`;
  }

  my $tmp_file = $file.".tmp";

  my $cmd = "cp ".$file." ".$nfsdir."/".$tmp_file;
  logMsg(3, "INFO", "NFS copy \"".$cmd."\"");
  ($result, $response) = Dada::mySystem($cmd,0);
  if ($result ne "ok") {
    return ("fail", "Command was \"".$cmd."\" and response was \"".$response."\"");
  } else {
    $cmd = "mv ".$nfsdir."/".$tmp_file." ".$nfsdir."/".$file;
    logMsg(3, "INFO", $cmd);
    ($result, $response) = Dada::mySystem($cmd,0);
    if ($result ne "ok") {
      return ("fail", "Command was \"".$cmd."\" and response was \"".$response."\"");
    } else {
      return ("ok", "");
    }
  }

}


sub controlThread($$) {

  my ($quit_file, $pid_file) = @_;

  logMsg(2, "INFO", "controlThread : starting");

  while ((!$quit_daemon) && (!(-f $quit_file))) {
    sleep(1);
  }

  $quit_daemon = 1;

  if ( -f $pid_file) {
    logMsg(2, "INFO", "controlThread: unlinking PID file");
    unlink($pid_file);
  } else {
    logMsg(1, "WARN", "controlThread: PID file did not exist on script exit");
  }

  logMsg(2, "INFO", "controlThread: exiting");

}


#
# Logs a message to the nexus logger and print to STDOUT with timestamp
#
sub logMsg($$$) {

  my ($level, $type, $msg) = @_;

  if ($level <= $dl) {

    my $time = Dada::getCurrentDadaTime();
    if (!($log_sock)) {
      $log_sock = Dada::nexusLogOpen($log_host, $log_port);
    }
    if ($log_sock) {
      Dada::nexusLogMessage($log_sock, $time, "sys", $type, "results mon", $msg);
    }
    print "[".$time."] ".$msg."\n";
  }
}
 

sub sigHandle($) {
  
  my $sigName = shift;
  print STDERR DAEMON_NAME." : Received SIG".$sigName."\n";

  # if we CTRL+C twice, just hard exit
  if ($quit_daemon) {
    print STDERR DAEMON_NAME." : Recevied 2 signals, Exiting\n";
    exit 1;

  # Tell threads to try and quit
  } else {

    $quit_daemon = 1;
    if ($log_sock) {
      close($log_sock);
    }
  }
}

sub sigPipeHandle($) {

  my $sigName = shift;
  print STDERR DAEMON_NAME." : Received SIG".$sigName."\n";
  $log_sock = 0;
  if ($log_host && $log_port) {
    $log_sock = Dada::nexusLogOpen($log_host, $log_port);
  }

}
