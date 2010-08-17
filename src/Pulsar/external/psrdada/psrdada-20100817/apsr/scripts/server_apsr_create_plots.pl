#!/usr/bin/env perl 

#
# Author:   Andrew Jameson
# Created:  29 Feb, 2008
#
# This script produces the 3 standard plots used by the web 
# front end

use lib $ENV{"DADA_ROOT"}."/bin";


use strict;               # strict mode (like -Wall)
use File::Basename;
use Apsr;                 # APSR/DADA Module for configuration options

use constant DEBUG_LEVEL         => 0;
use constant IMAGE_TYPE          => ".png";


#
# Global Variable Declarations
#
our %cfg = Apsr::getApsrConfig();


#
# Local Variable Declarations
#

my $bindir              = Dada::getCurrentBinaryVersion();

#
# Main 
#


# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}


# Ensure we have both arguements
if ($#ARGV!=1) {
    usage();
    exit 1;
}

my ($results_dir, $resolution) = @ARGV;

debugMessage(2,"results_dir = ".$results_dir);
debugMessage(2,"resolution = ".$resolution);

#
# Check inputs
#

if (!(-d $results_dir )) {
  print STDERR "Results directory did not exist\n";
  exit(1);
}

my $tres_archive = $results_dir."/total_t_res.ar";
my $fres_archive = $results_dir."/total_f_res.ar";

if (!(-f $fres_archive)) {
  print STDERR "Frequency resolution archive \"".$fres_archive."\" did not exist\n";
  exit(1);
}

if (!(-f $tres_archive)) {
  print STDERR "Time resolution archive \"".$tres_archive."\" did not exist\n";
  exit(1);
}

if (!($resolution =~ m/(\d)+x(\d)+/)) {
  print STDERR "Resolution must be in a <int>x<int> format. e.g. 240x180\n";
  exit(1);
}
debugMessage(2, "tres archive = ".$tres_archive);
debugMessage(2, "fres archive = ".$fres_archive);
debugMessage(2, "Inputs valid");

chdir $cfg{"SERVER_RESULTS_DIR"};

Apsr::makePlotsFromArchives($results_dir, $fres_archive, $tres_archive, $resolution);
Apsr::removeFiles($results_dir, "phase_vs_flux_*_".$resolution.".png", 0);
Apsr::removeFiles($results_dir, "phase_vs_time_*_".$resolution.".png", 0);
Apsr::removeFiles($results_dir, "phase_vs_freq_*_".$resolution.".png", 0);

exit(0);

sub usage() {

  print "Usage: ".$0." results_directory resolution\n";
  print "   results_directory   location of tres and fres archives\n";
  print "   resolution          desired resolution of plots (e.g. 240x180)\n";
  print "\n";

}

sub debugMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    print $message."\n";
  }
}

