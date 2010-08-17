#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use File::Basename;
use Getopt::Std;
use Caspsr;
use Dada::server_logger qw(%cfg);

sub usage() {
  print "Usage: ".basename($0)." [options]\n";
  print "  -h         print help text\n";
  print "  -n name    prefix the log message with [name]\n";
}

#
# Global Variable Declarations
#
%cfg = Caspsr->getConfig();

#
# Initialize module variables
#
$Dada::server_logger::dl = 2;
$Dada::server_logger::daemon_name = Dada->daemonBaseName($0);
$Dada::server_logger::log_name = "";


#
# Parse command line
#
my %opts;
getopts('hn:', \%opts);

if ($opts{h}) {
  usage();
  exit(0);
}

if ($opts{n}) {
  $Dada::server_logger::log_name = $opts{n};
}


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::server_logger->main();

exit($result);

