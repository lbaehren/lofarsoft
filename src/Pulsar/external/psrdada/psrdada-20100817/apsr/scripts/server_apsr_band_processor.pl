#!/usr/bin/env perl 

#
# server_apsr_band_processor
#
# Finalizes the observation, creating high resolution freq summed archives to
# store in the old_results/old_archives directories and deletes files from the
# local disks
#

#
# Author:   Andrew Jameson
# Created:  6 Dec, 2007
# Modified: 9 Jan, 2008
#

use lib $ENV{"DADA_ROOT"}."/bin";


use strict;               # strict mode (like -Wall)
use File::Basename;
use Apsr;


#
# Constants
#
use constant DEBUG_LEVEL         => 1;


#
# Global Variable Declarations
#
our %cfg = Apsr::getApsrConfig();


#
# Signal Handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
                                                                                                                    

#
# Local Variable Declarations
#

my $bindir              = Dada::getCurrentBinaryVersion();
my $cmd;
my $num_results = 0;
my $fres = "";
my $tres = "";

# Autoflush output
$| = 1;

# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}


# Get command line
if ($#ARGV!=0) {
    usage();
    exit 1;
}

my ($utc_start) = @ARGV;

my $results_dir = $cfg{"SERVER_RESULTS_DIR"}."/".$utc_start;
my $archive_dir = $cfg{"SERVER_ARCHIVE_DIR"}."/".$utc_start;


#
# Check that the expected flags existed
#
if (! (-d $archive_dir)) {
  debugMessage(0, "ERROR: Obs archive dir ".$archive_dir." did not exist");
  exit 1;
}

if (! (-d $results_dir)) {
  debugMessage(0, "ERROR: Obs results dir ".$results_dir." did not exist");
  exit 1;
}
if (! -f $archive_dir."/obs.info") {
  debugMessage(0, "ERROR: requried file [".$archive_dir."/obs.info] was missing");
  exit 1;
}

if (! -f $archive_dir."/obs.deleted") {
  debugMessage(0, "ERROR: requried file [".$archive_dir."/obs.deleted] was missing");
  exit 1;
}


debugMessage(2, "Processing obs ".$utc_start);

my $result = "";
my $response = "";
my $cmd = "";

my @dirs = ();
my @tres = ();
my @fres = ();
my @bits = ();
my %bands = ();
my %sources = ();
my @band_keys = ();
my @source_keys = ();
my $primary_source = "";
my $i = 0;
my $d = "";
my $b = "";

#
# Main
#
chdir $archive_dir;

# check to see if the archives dir has NFS links or not
$cmd = "find ".$archive_dir." -mindepth 1 -maxdepth 1 -type l | wc -l";
debugMessage(3, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$response);
if ($result ne "ok") {
  debugMessage(0, "ERROR: could not determine if archives are local or remote: ".$response);
  exit 1;
}
if ($response eq "0") {
  debugMessage(0, "ERROR: No NFS links were found for: ".$utc_start);
  exit 1;
}

# get the primary source from the obs.info file
$cmd = "grep ^SOURCE obs.info | awk '{print \$2}'";
debugMessage(3, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$response);
if ($result ne "ok") {
  debugMessage(0, "ERROR: could not determine primary source from obs.info ".$response);
  exit 1;
}

$primary_source = $response;
debugMessage(2, "main: primary_source=".$primary_source);
$sources{$primary_source} = 1;

# check if this is a multifold source
$cmd = "find -L . -mindepth 1 -type d";
debugMessage(3, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$response);
if ($result ne "ok") {
  debugMessage(0, "WARN: find command failed: ".$response);
  exit(1);
}
@dirs = split(/\n/, $response);
for ($i=0; $i<=$#dirs; $i++) {
  $d = $dirs[$i];
  @bits = split(/\//, $d);
  if ($#bits >= 1) {
    $bands{$bits[1]} = 1;
  }
  if ($#bits >= 2) {
    $sources{$bits[2]} = 1;
  }
}

# print what we have got, debug mostly
my $list = "";
@band_keys = sort keys %bands;
for ($i=0; $i<=$#band_keys; $i++) {
  $list .= $band_keys[$i]." ";
}
debugMessage(2, "main: BANDS: ".$list);

$list = "";
@source_keys = sort keys %sources;
for ($i=0; $i<=$#source_keys; $i++) {
  $list .= $source_keys[$i]." ";
}
debugMessage(2, "main: SOURCES: ".$list.", \$#source_keys=".$#source_keys);

my $r = 0;
my @regen_hosts = ();
my @regen_dirs = ();
my @local_dirs = ();
my $s = "";
my $ss = "";
my $j = 0;
my $rval = 0;
my $add_bands_ok = 1;

# if we have a multifold, then find band.tres for each source
for ($i=0; $i<=$#source_keys; $i++) {

  $s = $source_keys[$i];
  $ss = "";
  if ($#source_keys > 0) {
    $ss = $s;
  }

  for ($j=0; $j<=$#band_keys; $j++) {

    $b = $band_keys[$j];

    $rval = checkBandArchives($archive_dir, $b, $ss);

    # if we need to regenerate this one
    if ($rval == 1) {
      debugMessage(2, "main: checkBandArchives indicated archive should be regenerated. this is an error");
      exit(1);
    }

    # If we were missing a band.tres file, disk failure etc, then dont add them
    if ($rval == -1) {
      $add_bands_ok = 0;
    }
  }
}

if ($add_bands_ok) {

  # PSRadd the bands for each source
  for ($i=0; $i<=$#source_keys; $i++) {

    $s = $source_keys[$i];
    if ($#source_keys > 0) {
      $ss = $s;
    } else {
      $ss = "";
    }
    debugMessage(2, "main: addBands(".$archive_dir.", ".$s.", ".$ss,")");
    ($result, $response) = addBands($archive_dir, $s, $ss);
    debugMessage(2, "main: addBands() ".$result." " .$response);
    if ($result ne "ok") {
      debugMessage(0, "main: addBands for ".$s." ".$ss." failed: ".$response);
      exit(1);
    }
  }

  # clean up any old archive files from the results dir
  $cmd = "find ".$results_dir." -maxdepth 1 -type f -name '*.ar' -o -name '*.png'";
  debugMessage(2, "main: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "main: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "main: could not find any old results archives or png files".$cmd);
    exit(1);
  }

  $cmd = "find ".$results_dir." -maxdepth 1 -type f -name '*.ar' -delete -o -name '*.png' -delete";
  debugMessage(2, "main: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "main: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "main: could not deleted any old results archives or png files with ".$cmd);
    exit(1);
  }

  # copy the psradded fres and tres archives to the results dir
  for ($i=0; $i<=$#source_keys; $i++) {

    $s = $source_keys[$i];
    $s =~ s/^[JB]//;

    $cmd = "cp ".$archive_dir."/".$s."_t.ar ".$archive_dir."/".$s."_f.ar ".$results_dir;
    debugMessage(2, "main: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    debugMessage(2, "main: ".$result." ".$response);
    if ($result ne "ok") {
      debugMessage(0, "main: failed to copy archives from archives dir to results dir with ".$cmd);
      exit(1);
    }
  } 

# If we are not using the hi res archives, then just regenerate the plots
} else {

  $cmd = "find ".$results_dir." -maxdepth 1 -type f -name '*.png' -delete";
  debugMessage(2, "main: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "main: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "main: could not deleted any old png files with ".$cmd);
    exit(1);
  }
}

# Plot all the archives
my $plot_dir = $results_dir;
for ($i=0; $i<=$#source_keys; $i++) {

  $s = $source_keys[$i];
  $s =~ s/^[JB]//;

  $fres = $plot_dir."/".$s."_f.ar";
  $tres = $plot_dir."/".$s."_t.ar";

  debugMessage(2, "Making final low res plots");
  Apsr::makePlotsFromArchives($plot_dir, $s, $fres, $tres, "240x180");

  debugMessage(2, "Making final hi res plots");
  Apsr::makePlotsFromArchives($plot_dir, $s, $fres, $tres, "1024x768");

}

# Delete the low res archives in the results dir
$cmd = "find ".$results_dir." -mindepth 2 -type f -name '*.lowres' -delete";
debugMessage(2, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$result." ".$response);
if ($result ne "ok") {
  debugMessage(0, "main: ".$cmd." failed: ".$response);
  exit(1);
}

if (-f $results_dir."/processed.txt") {
  unlink ($results_dir."/processed.txt");
}

# Delete the local directories on the processing nodes
$cmd = "loopssh apsr\@apsr 0 17 'rm -rf /lfs/data0/apsr/archives/".$utc_start." /lfs/data0/apsr/results/".$utc_start."'";
debugMessage(2, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$result." ".$response);
if ($result ne "ok") {
  debugMessage(0, "main: ".$cmd." failed: ".$response);
  exit(1);
}

#$cmd = "ssh -l apsr apsr00 'rm -rf /lfs/data0/apsr/archives/".$utc_start." /lfs/data0/apsr/results/".$utc_start."'";
#debugMessage(2, "main: ".$cmd);
#($result, $response) = Dada::mySystem($cmd);
#debugMessage(3, "main: ".$result." ".$response);
#if ($result ne "ok") {
#  debugMessage(0, "main: ".$cmd." failed: ".$response);
#  exit(1);
#}

#my $all_gone = 0;
#while (!$all_gone) {

#  $cmd = "find -L  ".$archive_dir." -mindepth 2 -type f -name '*.ar' | wc -l";
#  debugMessage(2, "main: ".$cmd);
#  ($result, $response) = Dada::mySystem($cmd);
#  debugMessage(2, "main: ".$result." ".$response);
#  if ($result ne "ok") {
#    debugMessage(0, "main: ".$cmd." failed: ".$response);
#    exit(1);
#  }
#  if ($response eq "0")  {
#    debugMessage(2, "main: all local archives have been deleted");
#    $all_gone = 1;
#  } else {
#    debugMessage(2, "main: waiting for local archives have been deleted");
#    sleep(5);
#  }

#}


# Delete the archives directory
$cmd = "rm -rf ".$archive_dir;
debugMessage(2, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$result." ".$response);
if ($result ne "ok") {
  debugMessage(0, "main: ".$cmd." failed: ".$response);
  exit(1);
}

# move the results directory to the old_results 
$cmd = "mv ".$results_dir." /nfs/old_results/apsr/";
debugMessage(2, "main: ".$cmd);
($result, $response) = Dada::mySystem($cmd);
debugMessage(3, "main: ".$result." ".$response);
if ($result ne "ok") {
  debugMessage(0, "main: ".$cmd." failed: ".$response);
  exit(1);
}


print $utc_start." processed\n";
exit(0);

###############################################################################
#
# Functions
#

sub debugMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    print $message."\n";
  }
}

#
# check that the archives in the specified directory 
# are all "in" the band .tres and .fres files. return
# 0 if all ok, return 1 if regeneration required, -1
# on error
#
sub checkBandArchives($$$) {

  my ($base, $band, $subsource) = @_;
  debugMessage(2, "checkBandArchives(".$base.", ".$band.", ".$subsource.")");

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $dir = "";
  my $n_archives = 0;
  my $n_subints = 0;

  $dir = $base."/".$band;
  if ($subsource ne "") {
    $dir .= "/".$subsource;
  }

  # check if this is a multifold source
  $cmd = "find -L ".$dir." -type f -name '2*.ar' | wc -l";
  debugMessage(3, "checkBandArchives: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "checkBandArchives: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "checkBandArchives: find failed: ".$response);
    return(-1);
  }
  $n_archives = $response;

  if ( -f $dir."/band.tres" ) {
    $cmd = "psredit -Q -c nsubint ".$dir."/band.tres | awk '{print \$2}'";
    debugMessage(3, "checkBandArchives: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    debugMessage(3, "checkBandArchives: ".$result." ".$response);
    if ($result ne "ok") {
      debugMessage(2, "checkBandArchives: psredit failed: ".$response);
      return(-1);
    }
    $n_subints = $response;
  } else {
    $n_subints = -1;
  }

  debugMessage(2, "checkBandArchives: band=".$band.", n_archives=".$n_archives.", n_subints=".$n_subints);

  if ($n_subints == -1) {
    return -1;
  } elsif (($n_subints > 0) && ($n_subints > $n_archives)) {
    return 0;
  } else {
    return 1;
  }

}

#
# PSRadd all the band.?res files together
#
sub addBands($$$) {

  my ($base, $s, $ss) = @_;

  debugMessage(2, "addBands(".$base.", ".$s.", ".$ss.")");

  my $cmd = "";
  my $result = "";
  my $response = "";
  my $band_tres_files = "";
  my $band_fres_files = "";
  my $total_tres = "";
  my $total_fres = "";
  my $dir = "";

  $dir = "*";
  if ($ss ne "") {
    $dir = "*/".$s;
  }

  $cmd = "ls -1 ".$dir."/band.tres";
  debugMessage(3, "addBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "addBands: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "WARN: ls command failed: ".$response);
    exit 1;
  }
  $band_tres_files = $response;
  $band_tres_files =~ s/\n/ /g;

  $cmd = "ls -1 ".$dir."/band.fres";
  debugMessage(3, "addBands: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  debugMessage(3, "addBands: ".$result." ".$response);
  if ($result ne "ok") {
    debugMessage(0, "WARN: ls command failed: ".$response);
    exit 1;
  }
  $band_fres_files = $response;
  $band_fres_files =~ s/\n/ /g;

  $s =~ s/^[JB]//g;

  $total_tres = $s."_t.ar";
  $total_fres = $s."_f.ar";

  # create the total_tres from the band files
  if ($band_tres_files ne "") {
    if (-f $archive_dir."/".$total_tres) {
      unlink($total_tres);
    }
    $cmd = "psradd -R -m time -jp -f ".$total_tres." ".$band_tres_files;
    debugMessage(2, "addBands: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    debugMessage(3, "addBands: ".$result." ".$response);
    if ($result ne "ok") {
      debugMessage(0, "WARN: psradd TRES failed");
      return ("fail", "psradd TRES failed");
    }
  }

  # create the total_fres from the band files
  if ($band_fres_files ne "") {
    if (-f $archive_dir."/".$total_fres) {
      unlink($total_fres);
    }
    $cmd = "psradd -R -jp -f ".$total_fres." ".$band_fres_files;
    debugMessage(2, "addBands: ".$cmd);
    ($result, $response) = Dada::mySystem($cmd);
    debugMessage(3, "addBands: ".$result." ".$response);
    if ($result ne "ok") {
      debugMessage(0, "WARN: psradd FRES failed");
      return ("fail", "psradd FRES failed");
    }
  }
  
  return ("ok", $s);
}


#
# Handle INT AND TERM signals
#
sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";
  print STDERR basename($0)." : Exiting: ".Dada::getCurrentDadaTime(0)."\n";
  exit(1);

}

sub usage() {
  print "Usage: ".basename($0)." utc_start \n";
  print "  utc_start  utc_start of the observation\n";
}

