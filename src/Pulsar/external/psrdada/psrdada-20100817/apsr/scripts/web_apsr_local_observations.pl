#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use Apsr;
use strict;
use warnings;

my %cfg = Apsr::getApsrConfig();      # dada.cfg in a hash

my $cmd = "";
my $result = "";
my $response = "";
my @list = ();
my $obs = "";
my %observations = ();
my @parts = ();
my $i = 0;
my @keys = ();

chdir $cfg{"CLIENT_ARCHIVE_DIR"};

$cmd = "find . -mindepth 3 -maxdepth 3 -type f -name 'obs.start' -o -type f -name 'sent.to.*'".
       " -o -type f -name 'error.to.*' -o -type f -name 'band.deleted'";
($result, $response) = Dada::mySystem($cmd);
  
if ($result eq "ok") {

  @list = split(/\n/, $response);
  foreach $obs ( @list ) {
    @parts = ();
    @parts = split(/\//, $obs);
    if ( defined $observations{$parts[1]} ) {
      $observations{$parts[1]} .= " ".$parts[3];  
    } else {
      $observations{$parts[1]} = $parts[2]." ".$parts[3];
    }
  }

  @keys = sort keys %observations;
  for ($i=0; $i<=$#keys; $i++) {
    print $keys[$i]." ".$observations{$keys[$i]}."\n";
  }

}

