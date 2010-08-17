package Dada::server_results_manager;

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use threads;
use threads::shared;
use File::Basename;
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

  my $obs_results_dir  = $cfg{"SERVER_RESULTS_DIR"};
  my $control_thread   = 0;
  my @observations = ();
  my $i = 0;
  my $o = "";
  my $t = "";
  my $n_bands = 0;
  my $result = "";
  my $response = "";
  my $counter = 5;
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
  
  Dada::logMsg(0, $dl ,"STARTING SCRIPT");

  # start the control thread
  Dada::logMsg(2, "INFO", "main: controlThread(".$quit_file.", ".$pid_file.")");
  $control_thread = threads->new(\&controlThread, $quit_file, $pid_file);

  chdir $obs_results_dir;

  while (!$quit_daemon) {

    # TODO check that directories are correctly sorted by UTC_START time
    Dada::logMsg(2, $dl, "Looking for obs.processing");

    # Only get observations that are marked as procesing
    $cmd = "find ".$obs_results_dir." -maxdepth 2 -name 'obs.processing' ".
           "-printf '\%h\\n' | awk -F/ '{print \$NF}' | sort";

    Dada::logMsg(2, $dl, "main: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "main: ".$result." ".$response);

    if ($result eq "ok") {

      @observations = split(/\n/,$response);

      # For process all valid observations
      for ($i=0; (($i<=$#observations) && (!$quit_daemon)); $i++) {

        $o = $observations[$i];

        # Get the number of band subdirectories
        $n_bands = countObsStart($o);

        # check how long ago the last result was received
        # negative values indicate that no result has ever been received
        # and is the age of the youngest file (but -ve)
        $t = getObsAge($o, $n_bands, "lowres");
  
        # newest archive was more than 5 minutes old, finish the obs.
        if ($t > 300) {

          ($result, $response) = checkBandFinished($o, $n_bands);

          if ($result eq "ok") {
            markObsState($o, "processing", "finished");

          # If some band.finished are missing for some reason, then mark finished after 3 hours
          } elsif ($t > 10800) {
            markObsState($o, "processing", "finished");

          } else {
            sleep(2);
          }

        # we are still receiving results from this observation
        } elsif ($t >= 0) {
          processObservation($o, $n_bands);

        # no archives yet received, wait
        } elsif ($t > -60) {

        # no archives have been received 60+ seconds after the
        # directories were created, something is wrong with this
        # observation, mark it as failed
        } else {
          markObsState($o, "processing", "failed");

        }
      }
    }
  
    # Only look for files every 5 seconds
    $counter = 5;
    while ($counter && !$quit_daemon) {
      sleep(1);
      $counter--;
    }

  }

  Dada::logMsg(0, $dl, "STOPPING SCRIPT");
                                                                                
  return 0;
}



#
# Returns the "age" of the observation. Return value is the age in seconds of the
# file of type $ext in the obs dir $o. If no files exist in the dir, then return
# the age of the newest dir in negative seconds
# 
sub getObsAge($$) {

  my ($o, $num_bands, $ext) = @_;
  Dada::logMsg(2, $dl, "getObsAge(".$o.", ".$num_bands.", ".$ext.")");

  my $result = "";
  my $response = "";
  my $age = 0;
  my $time = 0;
  my $cmd = "";

  # current time in "unix seconds"
  my $now = time;

  # If the band subdirectories do not yet exist, we return the age of the
  # obs dir
  if ($num_bands == 0) {
    $cmd = "find ".$o." -maxdepth 0 -type d -printf '\%T@\\n'";
    Dada::logMsg(2, $dl, "getObsAge: ".$cmd); 
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "getObsAge: ".$result." ".$response);
    $time = $response;
    $age = $time - $now;

  # We have some band subdirectories, see if we have any archives
  } else {
    $cmd = "find ".$o." -type f -name '*.".$ext."' -printf '\%T@\\n' | sort -n | tail -n 1";
    Dada::logMsg(2, $dl, "getObsAge: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "getObsAge: ".$result." ".$response);
  
    # If we didn't found a file
    if ($response ne "") {
      # we will be returning a positive value
      $time = $response;
      $age = $now - $time;

    # No files were found, use the age of the obs.start files instead
    } else {

      # check the PROC_FILE 
      $cmd = "grep ^PROC_FILE `find ".$o." -type f -name 'obs.start' | sort -n | tail -n 1` | awk '{print \$2}'";
      Dada::logMsg(2, $dl, "getObsAge: ".$cmd);
      ($result, $response) = Dada::mySystem($cmd);
      Dada::logMsg(2, $dl, "getObsAge: ".$result." ".$response);

      # If this is a single pulse observation or P427 observation
      if (($result eq "ok") && (($response =~ m/singleF/) || ($response =~ m/P427/))) {

        # only declare this as finished when the band.finished files are written
        $cmd = "find ".$o." -type f -name 'band.finished' | wc -l";
        Dada::logMsg(2, $dl, "getObsAge: [pulse] ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        Dada::logMsg(2, $dl, "getObsAge: [pulse] ".$result." ".$response);

        # if all the band.finished files have been touched
        if (($result eq "ok") && ($num_bands == $response)) {

          $cmd = "find ".$o." -type f -name 'band.finished' -printf '\%T@\\n' | sort -n | tail -n 1";
          Dada::logMsg(2, $dl, "getObsAge: [pulse] ".$cmd);
          ($result, $response) = Dada::mySystem($cmd);
          Dada::logMsg(2, $dl, "getObsAge: [pulse] ".$result." ".$response);
      
          $time = $response;
          $age = $now - $time;

        # else, hardcode the time to -1, whilst we wait for the band.finished to be written
        } else {

          Dada::logMsg(2, $dl, "getObsAge: [pulse] waiting for band.finished");
          $age = -1;
        }

      # this is a normal observation
      } else {

        $cmd = "find ".$o." -type f -name 'obs.start' -printf '\%T@\\n' | sort -n | tail -n 1";
        Dada::logMsg(2, $dl, "getObsAge: ".$cmd);
        ($result, $response) = Dada::mySystem($cmd);
        Dada::logMsg(2, $dl, "getObsAge: ".$result." ".$response);
  
        # we will be returning a negative value
        $time = $response;
        $age = $time - $now;

      }
    }
  }

  Dada::logMsg(2, $dl, "getObsAge: time=".$time.", now=".$now.", age=".$age);
  return $age;
}

#
# Marks an observation as finished
# 
sub markObsState($$$) {

  my ($o, $old, $new) = @_;

  Dada::logMsg(2, $dl, "markObsState(".$o.", ".$old.", ".$new.")");

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $archives_dir = $cfg{"SERVER_ARCHIVE_DIR"};
  my $results_dir  = $cfg{"SERVER_RESULTS_DIR"};
  my $state_change = $old." -> ".$new;
  my $old_file = "obs.".$old;
  my $new_file = "obs.".$new;
  my $file = "";
  my $ndel = 0;

  Dada::logMsg(1, $dl, $o." ".$old." -> ".$new);

  $cmd = "touch ".$results_dir."/".$o."/".$new_file;
  Dada::logMsg(2, $dl, "markObsState: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "markObsState: ".$result." ".$response);

  $cmd = "touch ".$archives_dir."/".$o."/".$new_file;
  Dada::logMsg(2, $dl, "markObsState: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "markObsState: ".$result." ".$response);

  $file = $results_dir."/".$o."/".$old_file;
  if ( -f $file ) {
    $ndel = unlink ($file);
    if ($ndel != 1) {
      Dada::logMsgWarn($warn, "markObsState: could not unlink ".$file);
    }
  } else {
    Dada::logMsgWarn($warn, "markObsState: expected file missing: ".$file);
  }

  $file = $archives_dir."/".$o."/".$old_file;
  if ( -f $file ) {
    $ndel = unlink ($file);
    if ($ndel != 1) {
      Dada::logMsgWarn($warn, "markObsState: could not unlink ".$file);
    }
  } else {
    Dada::logMsgWarn($warn, "markObsState: expected file missing: ".$file);
  }

  # TODO optinonly reprocess all the received results, and then delete the
  # archives from each band individual archives
  # if (($old eq "processing") && ($new eq "finished")) {
  #   ($fres, $tres) = processAllArchives($o);
  #   Dada::logMsg(2, $dl, "markObsState: Deleting .lowres archives");
  #   deleteArchives($o, ".lowres");
  # }

}


#
# Process all possible archives in the observation, combining the bands
# and plotting the most recent images. Accounts for multifold  PSRS
#

sub processObservation($$) {

  my ($o, $n_bands) = @_;

  Dada::logMsg(2, $dl, "processObservation(".$o.", ".$n_bands.")");

  my %unprocessed = ();
  my @keys = ();
  my $i = 0;
  my $k = "";
  my $process = 0;
  my $fres_ar = "";
  my $tres_ar = "";
  my @fres_plot = ();
  my @tres_plot = ();
  my $source = "";
  my $cmd = "";
  my $result = "";
  my $response = "";

  # Look for the oldest archives that have not been processed
  %unprocessed = countArchives($o,1);

  # Sort the files into time order.
  @keys = sort (keys %unprocessed);

  # Stop now if we dont have any archives
  if ($#keys == -1) {
    return 0;
  }

  Dada::logMsg(2, $dl, "processObservation: [".$o."] found ".($#keys+1)." unprocessed");

  # Process all the files, plot at the end
  for ($i=0;$i<=$#keys;$i++) {

    $k = $keys[$i];
    Dada::logMsg(2, $dl, "processObservation: file=".$k.", found=".$unprocessed{$k}."/".$n_bands);

    $process = 0;

    # If we are missing archives 
    if ($unprocessed{$k} < $n_bands) {
      # AND there are 2 subqeuent sets of archives
      if ($i < ($#keys-2)) {
        # then we will process it regardless...
        Dada::logMsg(1, $dl, "Processing late ".$o."/*/".$k);
        $process = 1;
      }
    } else {
      $process = 1;
    }

    # process the archive and summ it into the results archive for observation
    if ($process) {

      Dada::logMsg(2, $dl, "processObservation: processArchives(".$o.", ".$k.")");
      ($source, $fres_ar, $tres_ar) = processArchive($o, $k);
      Dada::logMsg(2, $dl, "processObservation: processArchives() ".$source." ".$fres_ar." ".$tres_ar);

      if (($fres_ar ne "none") && ($tres_ar ne "none") && (!(grep $_ eq $fres_ar, @fres_plot))) {
        push @fres_plot, $fres_ar;
        push @tres_plot, $tres_ar;
      }
    }
  }

  # If we produce at least one archive from processArchive()
  for ($i=0; $i<=$#tres_plot; $i++) {

    # determine the source name for this archive
    $cmd = "vap -n -c name ".$tres_plot[$i]." | awk '{print \$2}'";
    Dada::logMsg(2, $dl, "processObservation: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    Dada::logMsg(2, $dl, "processObservation: ".$result." ".$response);
    if ($result ne "ok") {
      Dada::logMsgWarn($warn, "processObservation: failed to determine source name: ".$response);
      $source = "UNKNOWN";
    } else {
      $source = $response;
      chomp $source;
    }
    $source =~ s/^[JB]//;

    Dada::logMsg(2, $dl, "processObservation: plotting [".$i."] (".$o.", ".$source.", ".$fres_plot[$i].", ".$tres_plot[$i].")");
    makePlotsFromArchives($o, $source, $fres_plot[$i], $tres_plot[$i], "240x180");
    makePlotsFromArchives($o, $source, $fres_plot[$i], $tres_plot[$i], "1024x768");
    Apsr::removeFiles($o, "phase_vs_flux_".$source."*_240x180.png", 40);
    Apsr::removeFiles($o, "phase_vs_time_".$source."*_240x180.png", 40);
    Apsr::removeFiles($o, "phase_vs_freq_".$source."*_240x180.png", 40);
    Apsr::removeFiles($o, "phase_vs_flux_".$source."*_1024x768.png", 40);
    Apsr::removeFiles($o, "phase_vs_time_".$source."*_1024x768.png", 40);
    Apsr::removeFiles($o, "phase_vs_freq_".$source."*_1024x768.png", 40);
  }
}

sub deleteObservation($) {

  (my $dir) = @_;
  my $cmd = "";

  Dada::logMsg(1, $dl, "Deleting observation: ".$dir);
  $cmd = "rm -rf $dir";
  `$cmd`;
  return $?;

}


#
# For the given utc_start ($dir), and archive (file) add the archive to the 
# summed archive for the observation
#
sub processArchive($$) {

  (my $dir, my $file) = @_;

  my $result = "";
  my $response = "";
  my $cmd = "";
  my @archives = ();

  Dada::logMsg(2, $dl, "processArchive(".$dir.", ".$file.")");

  my $bindir =      Dada::getCurrentBinaryVersion();
  my $results_dir = $cfg{"SERVER_RESULTS_DIR"};

  # Get the sub directories in the obs (bands)
  $cmd = "find ".$dir."/* -maxdepth 0 -type d";
  Dada::logMsg(2, $dl, "processArchive: ".$cmd);
  my $find_result = `$cmd`;
  my @sub_dirs = split(/\n/, $find_result);

  # Find out how many archives we actaully hae
  $cmd = "ls -1 ".$dir."/*/".$file;
  Dada::logMsg(2, $dl, "processArchive: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "processArchive: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "processArchive: '".$cmd."' failed: ".$response);
  } else {
    @archives = split(/\n/, $response);  
  }

  Dada::logMsg(2, $dl, "processArchive: found ".($#archives+1)." / ".($#sub_dirs+1)." archives");
  Dada::logMsg(1, $dl, "Processing ".$dir." -> ".$file." ".($#archives+1)." of ".($#sub_dirs+1));

  my $output = "";
  my $real_archives = "";
  my $subdir = "";
  my $plottable_archives = "";
  my $source = "";

  # determine the source name for this archive
  $cmd = "vap -n -c name ".$archives[0]." | awk '{print \$2}'";
  Dada::logMsg(2, $dl, "processArchive: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "processArchive: ".$result." ".$response);
  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "processArchive: failed to determine source name: ".$response);
    $source = "UNKNOWN";
  } else {
    $source = $response;
    chomp $source;
  }

  $source =~ s/^[JB]//;

  # The combined results for this observation (dir == utc_start) 
  my $total_f_res = $dir."/".$source."_f.ar";
  my $total_t_res = $dir."/".$source."_t.ar";

  # The frequency summed archive for this time period
  my $total_f_sum = $file;
  $total_f_sum =~ s/\//_/g;
  $total_f_sum = $dir."/".$total_f_sum;

  my $tmp_ar = "";
  my $out_ar = "";
  my $band_freq = "";
  my $dirs = "";
  my $suffix = "";

  # If we are missing some archives from some of the bands
  if ($#archives < $#sub_dirs) {

    foreach $subdir (@sub_dirs) {

     Dada::logMsg(2, $dl, "subdir = ".$subdir.", file = ".$file); 
     # If the archive does not exist in the frequency dir
      if (!(-f ($subdir."/".$file))) {

        Dada::logMsg(1, $dl, "archive ".$subdir."/".$file." was not present");

        ($band_freq, $dirs, $suffix) = fileparse($subdir);

        $tmp_ar = $archives[0];
        $tmp_ar =~ s/\.lowres$/\.zeroed/;
        $out_ar = $subdir."/".$file;

        # create a copy of archives[0] with .lowres -> .tmp
        $cmd = $bindir."/pam -o ".$band_freq." -e zeroed ".$archives[0]." 2>&1";
        Dada::logMsg(2, $dl, $cmd);
        $output = `$cmd`;
        Dada::logMsg(2, $dl, $output);

        # move the zeroed archive to the band subdir
        $cmd = "mv -f ".$tmp_ar." ".$out_ar." 2>&1";
        Dada::logMsg(2, $dl, $cmd);
        $output = `$cmd`;
        Dada::logMsg(2, $dl, $output);

        # set the weights in the zeroed archive to 0
        $cmd = $bindir."/paz -w 0 -m ".$out_ar." 2>&1";
        Dada::logMsg(2, $dl, $cmd);
        $output = `$cmd`;
        Dada::logMsg(2, $dl, $output);

        $plottable_archives .= " ".$out_ar;
        Dada::logMsg(2, $dl, "Added ".$out_ar." to plottable archives");

      } else {
        ($band_freq, $dirs, $suffix) = fileparse($subdir);
        $real_archives .= " ".$band_freq;
        $plottable_archives .= " ".$subdir."/".$file;
      }
    }

  # We have all the required archives
  } else {

    foreach $subdir (@sub_dirs) {
      ($band_freq, $dirs, $suffix) = fileparse($subdir);
      $real_archives .= " ".$band_freq;
      $plottable_archives .= " ".$subdir."/".$file;
    } 
  }

  # combine all thr frequency channels
  $cmd = $bindir."/psradd -R -o ".$total_f_sum." ".$plottable_archives;
  Dada::logMsg(2, $dl, $cmd);
  $output = `$cmd`;

  if ($#archives < $#sub_dirs) {
    # Delete any "temp" files that we needed to use to produce the result
    $cmd = "rm -f ".$dir."/*/*.zeroed ".$dir."/*/*/*.zeroed";
    Dada::logMsg(2, $dl, $cmd);
    $output = `$cmd`;
  }

  my $plot_this = 1;

  # Keep a pulse archive if applicable
  if ($file =~ m/^pulse/) {

    $cmd = "checkpulse.psh ".$total_f_sum;
    $output = `$cmd`;

    # Delete condition
    if ($output eq "") {
      $cmd = "echo 'rm -f */$file' >> ".$cfg{"SERVER_ARCHIVE_DIR"}."/".$dir."/deleteme.csh";
      Dada::logMsg(1, $dl, "pulse: ".$cmd);
      `$cmd`;
      $plot_this = 0;

    }
  } 

  if ($plot_this) {

  # If this is the first result for this observation
  if (!(-f $total_f_res)) {

    $cmd = "cp ".$total_f_sum." ".$total_f_res;
    Dada::logMsg(2, $dl, $cmd);
    $output = `$cmd`;

    # Fscrunc the archive
    $cmd = $bindir."/pam -F -m ".$total_f_sum;
    Dada::logMsg(2, $dl, $cmd);
    $output = `$cmd`;

    # Tres operations
    $cmd = "cp ".$total_f_sum." ".$total_t_res;
    Dada::logMsg(2, $dl, $cmd);
    $output = `$cmd`;

  } else {

    my $temp_ar = $dir."/temp.ar";

    # Fres Operations
    $cmd = $bindir."/psradd -T -o ".$temp_ar." ".$total_f_res." ".$total_f_sum;
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "psradd failed cmd=".$cmd);
      Dada::logMsg(0, $dl, "psradd output=".$response);
    }
    #Dada::logMsg(2, $dl, $cmd);
    #$output = `$cmd`;

    unlink($total_f_res);
    rename($temp_ar,$total_f_res);

    # Fscrunc the archive
    $cmd = $bindir."/pam -F -m ".$total_f_sum;
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "pam failed cmd=".$cmd);
      Dada::logMsg(0, $dl, "pam output=".$response);
    }
    #Dada::logMsg(2, $dl, $cmd);
    #$output = `$cmd`;

    # Tres Operations
    $cmd = $bindir."/psradd -o ".$temp_ar." ".$total_t_res." ".$total_f_sum;
    ($result, $response) = Dada::mySystem($cmd);
    if ($result ne "ok") {
      Dada::logMsg(0, $dl, "psradd failed cmd=".$cmd);
      Dada::logMsg(0, $dl, "psradd output=".$response);
    }
    #Dada::logMsg(2, $dl, $cmd);
    #$output = `$cmd`;

    unlink($total_t_res);
    rename($temp_ar,$total_t_res);

  }
  } else {
    $total_f_res = "none";
    $total_t_res = "none";
  }

  # clean up the current archive
  unlink($total_f_sum);
  Dada::logMsg(2, $dl, "unlinking $total_f_sum");

  # Record this archive as processed and what sub bands were legit
  recordProcessed($dir, $file, $real_archives);

  return ($source, $total_f_res, $total_t_res);

}

#
# Counts the numbers of *.lowres archives in total received
#
sub countArchives($$) {

  my ($dir, $skip_existing_archives) = @_;

  my $i=0;

  my @processed = getProcessedFiles($dir);

  my $cmd = "find ".$dir."/*/ -name \"*.lowres\" -printf \"%P\n\"";
  Dada::logMsg(2, $dl, "find ".$dir."/*/ -name \"*.lowres\"");

  my $find_result = `$cmd`;

  my %archives = ();

  my @files = split(/\n/,$find_result);
  my $file = "";

  foreach $file (@files) {
  
    my $has_been_processed = 0;
    # check that this file has not already been "processed";
    for ($i=0;$i<=$#processed;$i++) {
      if ($file eq $processed[$i]) {
        $has_been_processed = 1;
      }
    }

    # If we haven't processed this OR we want to get all archives
    if (($has_been_processed == 0) || ($skip_existing_archives == 0)) {
      if (!(exists $archives{$file})) {
        $archives{$file} = 1;
      } else {
        $archives{$file} += 1;
      }

    }
  }

  return %archives;

}


sub getProcessedFiles($) {

  (my $dir) = @_;
  my $i = 0;
  my @lines = ();
  my @arr = ();
  my @archives = ();
  my $fname = $dir."/".PROCESSED_FILE_NAME;

  open FH, "<".$fname or return @lines;
  @lines = <FH>;
  close FH;

  for ($i=0;$i<=$#lines;$i++) {
    @arr = split(/ /,$lines[$i]); 
    chomp($arr[0]);
    $archives[$i] = $arr[0];
    # print "Processed file $i = ".$archives[$i]."\n";
  }

  return @archives;

}


#
# The number of band subdirectories
#
sub countObsBands($) {
  
  my ($dir) = @_;

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $num_bands = 0;

  $cmd = "find ".$dir." -maxdepth 2 -type d | wc -l";
  Dada::logMsg(2, $dl, "countObsBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "countObsBands: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "countObsBands: ".$cmd." failed: ".$response);
    $num_bands = 0;
  } else {
    $num_bands = $response;
  }

  return $response;

}



sub countObsStart($) {

  my ($dir) = @_;

  my $cmd = "find ".$dir." -name \"obs.start\" | wc -l";
  my $find_result = `$cmd`;
  chomp($find_result);
  return $find_result;

}

sub deleteArchives($) {

  (my $dir) = @_;

  opendir(DIR,$dir);
  my @subdirs = sort grep { !/^\./ && -d $dir."/".$_ } readdir(DIR);
  closedir DIR;

  my $cmd = "";
  my $i=0;
  for ($i=0; $i <= $#subdirs; $i++) {
    $cmd  = "rm -rf ".$dir."/".$subdirs[$i];
    Dada::logMsg(2, $dl, "Deleting ".$dir."/".$subdirs[$i]);
    # system($cmd);
  }

  return 0;

}

sub recordProcessed($$$) {

  my ($dir, $archive, $subbands) = @_;

  my $FH = "";
  my $record = $dir."/".PROCESSED_FILE_NAME;
  my @lines = ();

  if (-f $record) {
    open FH, "<$record" or return ("fail","Could not read record file: $record");
    @lines = <FH>;
    close FH;
  }

  my $line = "";
  my $added = 0;

  open FH, ">$record" or return ("fail","Could not write record file: $record");
  foreach $line (@lines) {
    if ($line =~ m/$archive/) {
      print FH $archive.$subbands."\n";
      $added = 1;
    } else {
      print FH $line;
    }
  }
  if (!$added) {
    print FH $archive.$subbands."\n";
  }
  close FH;

  return ("ok","");
}


sub getMostRecentResult($$) {

  my ($dir, $ext) = @_;

  my $age = -1;

  # Check if any directories exist yet...
  my $cmd = "find ".$dir."/* -type d | wc -l";
  #Dada::logMsg(1, $dl, "find ".$dir."/* -type d | wc -l");
  my $num_dirs = `$cmd`;
  chomp($num_dirs);
  #Dada::logMsg(1, $dl, "\"".$num_dirs."\"");

  if ($num_dirs > 0) {
    
    $cmd = "find ".$dir."/*/ -name \"*.".$ext."\" -printf \"%T@\\n\" | sort | tail -n 1";
    #Dada::logMsg(1, $dl, "find ".$dir."/*/ -name \"*.".$ext."\" -printf \"\%T@\" | sort | tail -n 1");
    my $unix_time_of_most_recent_result = `$cmd`;
    chomp($unix_time_of_most_recent_result);
    #Dada::logMsg(1, $dl, "\"".$unix_time_of_most_recent_result."\"");

    my $current_unix_time = time;

    if ($unix_time_of_most_recent_result) {

      $age = $current_unix_time - $unix_time_of_most_recent_result;
  
    } else {

      #Dada::logMsg(1, $dl, "No .lowres files found in $dir");
      # Check the age of the directories - if > 5 minutes then likely
      # a dud obs.
      $cmd = "find ".$dir."/* -type d -printf \"%T@\\n\" | sort | tail -n 1";
      #Dada::logMsg(1, $dl, "find ".$dir."/* -type d -printf \"\%T@\" | sort | tail -n 1");
      $unix_time_of_most_recent_result = `$cmd`;
      chomp($unix_time_of_most_recent_result);
      #Dada::logMsg(1, $dl, "\"".$unix_time_of_most_recent_result."\"");

      # If we found no .lowres files, this may be a dud observation.
      # Check the age of the obs.start
      #$cmd = "find ".$dir."/*/ -name \"obs.start\" -printf \"%T@\\n\" | sort | tail -n 1";
      #$unix_time_of_most_recent_result = `$cmd`;

      if ($unix_time_of_most_recent_result) {
        $age = $current_unix_time - $unix_time_of_most_recent_result;
        #Dada::logMsg(1, $dl, "Dir age = ".$age);

        # If the obs.start is more than 5 minutes old, but we have no 
        # archives, then this observation is considered a dud
        if ($age > 5*60) {
          $age = -1;
        }

      } else {
        $age = -1;
      }
    }
    Dada::logMsg(2, $dl, "Observation: ".$dir.", age: ".$age." seconds");
  } else {
    $age = 0;
  }


  return $age;
 
} 

sub processAllArchives($) {

  (my $dir) = @_;

  Dada::logMsg(1, $dl, "processAllArchives(".$dir.")");
  my $cmd = "";

  # Delete the existing fres and tres files
  $cmd = "rm -f ".$dir."/*_t.ar";
  system($cmd);
  $cmd = "rm -f ".$dir."/*_f.ar";
  system($cmd);
 
  # Get ALL archives in the observation dir
  my %unprocessed = countArchives($dir,0);

  # Sort the files into time order.
  my @keys = sort (keys %unprocessed);
 
  my $i=0;
  my $curr_ar = "";
  my $source = "";
  my $fres_ar = "";
  my $tres_ar = "";
    
  for ($i=0; $i<=$#keys; $i++) {

    Dada::logMsg(1, $dl, "Finalising archive ".$dir."/*/".$keys[$i]);
    # process the archive and summ it into the results archive for observation
    ($source, $curr_ar, $fres_ar, $tres_ar) = processArchive($dir, $keys[$i]);

  }

  return ($fres_ar, $tres_ar);

}

#
#
#
sub makePlotsFromArchives($$$$$) {

  my ($dir, $source, $total_f_res, $total_t_res, $res) = @_;

  #my %local_cfg = Apsr->getApsrConfig();
  my $web_style_txt = $cfg{"SCRIPTS_DIR"}."/web_style.txt";
  my $psrplot_args = "-g ".$res." -jp";
  my $cmd = "";
  my $result = "";
  my $response = "";

  # If we are plotting hi-res - include
  if ($res eq "1024x768") {
    # No changes
  } else {
    $psrplot_args .= " -s ".$web_style_txt." -c below:l=unset";
  }

  my $bin = Dada::getCurrentBinaryVersion()."/psrplot ".$psrplot_args;
  my $timestamp = Dada::getCurrentDadaTime(0);

  my $pvt  = "phase_vs_time_".$source."_".$timestamp."_".$res.".png";
  my $pvfr = "phase_vs_freq_".$source."_".$timestamp."_".$res.".png";
  my $pvfl = "phase_vs_flux_".$source."_".$timestamp."_".$res.".png";

  # Combine the archives from the machine into the archive to be processed
  # PHASE vs TIME
  $cmd = $bin." -p time -jFD -D ".$dir."/pvt_tmp/png ".$total_t_res;
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$result." ".$response);

  # PHASE vs FREQ
  $cmd = $bin." -p freq -jTD -D ".$dir."/pvfr_tmp/png ".$total_f_res;
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$result." ".$response);

  # PHASE vs TOTAL INTENSITY
  $cmd = $bin." -p flux -jTF -D ".$dir."/pvfl_tmp/png ".$total_f_res;
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "makePlotsFromArchives: ".$result." ".$response);


  # Get plots to delete in the destination directory
  # $cmd = "find ".$plotDir." -name '*_".$resolution.IMAGE_TYPE."' -printf '\%h/\%f '";
  # my $curr_plots = `$cmd`;

  # wait for each file to "appear"
  my $waitMax = 5;
  while ($waitMax) {
    if ( (-f $dir."/pvfl_tmp") &&
         (-f $dir."/pvt_tmp") &&
         (-f $dir."/pvfr_tmp") )
    {
      $waitMax = 0;
    } else {
      $waitMax--;
      sleep(1);
    }
  }

  # rename the plot files to their correct names
  system("mv -f ".$dir."/pvt_tmp ".$dir."/".$pvt);
  system("mv -f ".$dir."/pvfr_tmp ".$dir."/".$pvfr);
  system("mv -f ".$dir."/pvfl_tmp ".$dir."/".$pvfl);

  #if ($curr_plots ne "") {
  #  $cmd = "rm -f ".$curr_plots;
  #  system($cmd);
  #}
}


#
# Checks that all the band.finished files exist. This ensures there
# are not unprocessed rawdata files that may have been dumped to disk
#
sub checkBandFinished($$) {

  my ($obs, $n_bands) = @_;

  Dada::logMsg(2, $dl ,"checkBandFinished(".$obs.", ".$n_bands.")");

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $dir = $cfg{"SERVER_RESULTS_NFS_MNT"};

  # only declare this as finished when the band.finished files are written
  $cmd = "find ".$dir."/".$obs." -type f -name 'band.finished' | wc -l";
  Dada::logMsg(2, $dl, "checkBandFinished: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "checkBandFinished: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsgWarn($warn, "checkBandFinished: ".$cmd." failed: ".$response);
  } else {
    if ($response eq $n_bands) {
      $result = "ok";
      $response = "";
    } else {
      $result = "fail";
      $response = "";
    }
  }

  return ($result, $response); 

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

  Dada::logMsg(1, $dl ,"controlThread: exiting");

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

  # Ensure more than one copy of this daemon is not running
  my ($result, $response) = Dada::checkScriptIsUnique(basename($0));
  if ($result ne "ok") {
    return ($result, $response);
  }

  return ("ok", "");
}


END { }

1;  # return value from file
