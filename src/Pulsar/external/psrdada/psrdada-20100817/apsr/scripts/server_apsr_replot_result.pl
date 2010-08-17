#!/usr/bin/env perl 

#
# plots the compiled archives
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
use constant PROCESSED_FILE_NAME => "processed.txt";
use constant IMAGE_TYPE          => ".png";
use constant TOTAL_F_RES         => "total_f_res.ar";
use constant TOTAL_T_RES         => "total_t_res.ar";


#
# Global Variable Declarations
#
our %cfg = Apsr::getApsrConfig();
our $dl  = 2;


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
my $result = "";
my $response = "";
my $i = 0;
my $src = "";
my @tres = ();
my @fres = ();
my @srcs = ();

# Autoflush output
$| = 1;

# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}


# Get command line
if ($#ARGV!=1) {
    usage();
    exit 1;
}

my ($utc_start, $basedir) = @ARGV;

my $dir = $basedir."/".$utc_start;

#
# Parse input
#

if (! (-d $dir)) {
  Dada::logMsg(0, $dl, "Error: Results directory \"".$dir."\" did not exist");
  exit 1;
}

Dada::logMsg(1, $dl, "Processing obs ".$utc_start." in ".$dir);

#
# Main
#


  # get the tres archives 
  $cmd = "find ".$dir." -maxdepth 1 -name '*_t.ar' -printf '%f\n' | sort";
  Dada::logMsg(2, $dl, "main: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd); 
  Dada::logMsg(2, $dl, "main: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(2, $dl, "ERROR: tres find command failed: ".$response);
    exit(1);
  } 
  @tres = split(/\n/,$response);

  # get the fres archives 
  $cmd = "find ".$dir." -maxdepth 1 -name '*_f.ar' -printf '%f\n' | sort";
  Dada::logMsg(2, $dl, "main: ".$cmd);
  ($result, $response) = Dada::mySystem($cmd);
  Dada::logMsg(2, $dl, "main: ".$result." ".$response);

  if ($result ne "ok") {
    Dada::logMsg(0, $dl, "ERROR: fres find command failed: ".$response);
    exit(1);
  }
  @fres = split(/\n/,$response);

  # check they match
  if ($#tres != $#fres) {
    Dada::logMsg(0, $dl, "ERROR: fres count [".($#fres+1)."] != tres count [".($#tres+1)."]");
    exit(1);
  }

  @srcs = ();
  for ($i=0; $i<=$#tres; $i++) {
    $src = $tres[$i];
    $src =~ s/_t\.ar//;
    if (!($fres[$i] eq $src."_f.ar")) {
      Dada::logMsg(0, $dl, "ERROR: fres file [".$fres[$i]."] did not match [".$src."_f.ar"."]");
      exit(1);
    }
    $src =~ s/^[JB]//;
    Dada::logMsg(1, $dl, "srcs[".$i."] = ".$src);

    $srcs[$i] = $src;
  }
      
  for ($i=0; $i<=$#srcs; $i++) {

    $src = $srcs[$i];

    Dada::logMsg(1, $dl, "Making final low res plots for ".$src." [".$fres[$i].", ".$tres[$i]."]");
    Apsr::makePlotsFromArchives($dir, $src, $dir."/".$fres[$i], $dir."/".$tres[$i], "240x180");
    Dada::logMsg(1, $dl, "Making final hi res plots for ".$src." [".$fres[$i].", ".$tres[$i]."]");
    Apsr::makePlotsFromArchives($dir, $src, $dir."/".$fres[$i], $dir."/".$tres[$i], "1024x768");

    Apsr::removeFiles($dir, "phase_vs_flux_".$src."*_240x180.png", 0);
    Apsr::removeFiles($dir, "phase_vs_time_".$src."*_240x180.png", 0);
    Apsr::removeFiles($dir, "phase_vs_freq_".$src."*_240x180.png", 0);
    Apsr::removeFiles($dir, "phase_vs_flux_".$src."*_1024x768.png", 0);
    Apsr::removeFiles($dir, "phase_vs_time_".$src."*_1024x768.png", 0);
    Apsr::removeFiles($dir, "phase_vs_freq_".$src."*_1024x768.png", 0);
  }


exit(0);

###############################################################################
#
# Functions
#


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
  print "Usage: ".basename($0)." utc_start basedir\n";
  print "  utc_start  utc_start of the observation\n";
  print "  dir        location of the utc_start dir\n";
}

                                                                                
