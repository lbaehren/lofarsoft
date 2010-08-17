#!/usr/bin/env perl

###############################################################################
#
# dspsr_command_line.pl
#
use lib $ENV{"DADA_ROOT"}."/bin";

use strict;         # strict mode (like -Wall)
use Getopt::Std;
use Dada;

#
# Constants
#
use constant DEBUG_LEVEL => 0;
use constant NUM_CORES   => 7;
use constant BIN_DIR     => "/home/dada/linux_64/bin";

#
# Local Variables
#

my $psrcat = BIN_DIR."/psrcat";
my $dmsmear = BIN_DIR."/dmsmear";

my $source = "";
my $nchan  = 0;  
my $bw     = 0;     # Mhz
my $freq   = 0.0; # Mhz
my $dm     = 0.0;
my $mode   = "PSR";
my $full_cmd_line = 0;

my %opts;
getopts('fh', \%opts);


if ($opts{f}) {
  $full_cmd_line = 1;
}

if ($opts{h}) {
  usage();
  exit(0);
}


if ($#ARGV ne 3) {
  print "Incorrect number of command line arguments\n";
  usage();
  exit 1;

} else {

  $source = $ARGV[0];
  $bw = $ARGV[1];
  $freq = $ARGV[2];
  $mode = $ARGV[3];

  if (DEBUG_LEVEL >= 1) {
    print "source = $source\n";
    print "bw     = $bw\n";
    print "freq   = $freq\n";
    print "mode   = $mode\n";
  }
}

$nchan = abs($bw * 2);

my $cmd = "";
my $str = "";
my $nsmear = 0;
my $dedisp = "";
my $fft_length = 0;

# If we are not doing coherrent dedispersion
if ($mode ne "PSR") {

  $dedisp = $nchan;
  $fft_length = 128;
  my $nsmear = 0;

} else {

  $dedisp = "D";

  $dm = Dada::getDM($source);

  if ($dm eq "NA") {
    print "ERROR: source $source not in catalogue\n";
    exit(1);
  }

  # Determine the source name from the catalogue
  # $cmd = $psrcat." -x -c DM ".$source." 2>&1";
  # $str = `$cmd`;
  # chomp $str;

  # Check to see if the source is in the catalogue
  # if ($str =~ m/not in catalogue/) {

  #   if ($source eq "J1731-1845") {
  #     $dm = "106.37";

  #   } else {
  #     print "ERROR: source $source not in catalogue\n";
  #     exit(1);
  #   }

  # } else {

  #   ($dm, $str) = split(/ /,$str,2);

  #   if (DEBUG_LEVEL >= 1) {
  #     print "$source has DM of $dm\n";
  #   }

  # }

  $cmd = $dmsmear." -f ".$freq." -d ".$dm." -b ".$bw." -n ".$nchan.
         " 2>&1 | grep \"Minimum Kernel Length\" | awk '{print \$4}'";
  $str = `$cmd`;
  chomp $str;

  $nsmear = $str;

  if (DEBUG_LEVEL >= 1) {
    print "Nsmear = $nsmear\n";
  }

  # determine the FFT length for the given FBC and nsmear
  # my %fbc32 = (2 => 512, 4 => 128, 8 => 256, 16 => 256, 32 => 256, 64 => 256, 128 => 256, 256 => 512, 512 => 1024);
  # my %fbc64 = (2 => 256, 4 => 256, 8 => 256, 16 => 256, 32 => 256, 64 => 256, 128 => 256, 256 => 512, 512 => 1024);
  # my %fbc128 = (2 => 128, 4 => 128, 8 => 128, 16 => 128, 32 => 128, 64 => 128, 128 => 256, 256 => 512, 512 => 1024);
  my %fbc8 =   (2 => 64, 4 => 64,  8 => 128, 16 => 128,  32 => 256, 64 => 512, 128 => 256, 256 => 512, 512 => 1024, 1024 => 2048);
  my %fbc32 =  (2 => 32, 4 => 32,  8 => 64,  16 => 64,   32 => 128, 64 => 256, 128 => 256, 256 => 512, 512 => 1024, 1024 => 2048);
  my %fbc64 =  (2 => 32, 4 => 32 , 8 => 32,  16 => 256,  32 => 64,  64 => 128, 128 => 256, 256 => 512, 512 => 1024, 1024 => 2048);
  my %fbc128 = (2 => 64, 4 => 64,  8 => 64,  16 => 64,   32 => 64,  64 => 128, 128 => 256, 256 => 512, 512 => 1024, 1024 => 2048);

  $fft_length = 256;
  if ($nchan == 8) {
    $fft_length = $fbc8{$nsmear};
  } elsif ($nchan == 32) {
    $fft_length = $fbc32{$nsmear};
  } elsif ($nchan == 64) {
    $fft_length = $fbc64{$nsmear};
  } elsif ($nchan == 128) {
    $fft_length = $fbc128{$nsmear};
  }  else {
    $fft_length = 256;
  }
}

my $x_arg = "-x ".$fft_length;
#if ($nsmear > 0) {
#  $x_arg .= ":".$nsmear;
#}

my $F_arg = "-F".$nchan.":".$dedisp;

my $output = "";

if ($mode eq "PSR") {
  $output = "-Z IPP ".$F_arg." ".$x_arg." -U minX2";
} else {
  $output = "-Z IPP ".$F_arg." ".$x_arg." -U minX2";
}

if ($full_cmd_line) {
  $output = "dspsr -t ".NUM_CORES." ".$output;
}

print $output."\n";

exit 0;

sub usage() {

  print "Usage: dspsr_command_line.pl [-f] source bw freq mode\n";
  print "         -f       generate a full command line\n";
  print "         source   PSR source name\n";
  print "         bw       bandwidth\n";
  print "         freq     centre frequency\n";
  print "         mode     MODE header variable e.g. PSR, CAL\n";

}
