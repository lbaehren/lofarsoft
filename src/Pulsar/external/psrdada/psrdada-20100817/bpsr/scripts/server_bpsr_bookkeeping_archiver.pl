#!/usr/bin/env perl

###############################################################################
#
# server_bpsr_bookkeeping_archiver.pl
#
# Archives the files.db and tapes.db from swin and parkes in the logs 
# directory in case of disk failure on the remote archiving machines.
# This script is run by cron as user dada every 1 hour and will create
# a copy of the swin/parkes files if they differ from the last
#

use lib $ENV{"DADA_ROOT"}."/bin";

use Net::hostent;
use File::Basename;
use Bpsr;           # BPSR Module 
use strict;         # strict mode (like -Wall)

#
# Constants
#
use constant DEBUG_LEVEL  => 2;
use constant TAPES_DB     => "tapes.P630.db";
use constant FILES_DB     => "files.P630.db";

#
# Global Variables
#
our %cfg = Bpsr::getBpsrConfig();      # Bpsr.cfg

# Autoflush output
$| = 1;

# Sanity check for this script
if (index($cfg{"SERVER_ALIASES"}, $ENV{'HOSTNAME'}) < 0 ) {
  print STDERR "ERROR: Cannot run this script on ".$ENV{'HOSTNAME'}."\n";
  print STDERR "       Must be run on the configured server: ".$cfg{"SERVER_HOST"}."\n";
  exit(1);
}


#
# Local Varaibles
#
my $logdir = $cfg{"SERVER_LOG_DIR"}."/bookkeeping";;
my $p_user;
my $p_host;
my $p_path;
my $s_user;
my $s_host;
my $s_path;
my $cmd = "";
my $result = "";
my $response = "";

($p_user, $p_host, $p_path) = split(/:/,$cfg{"PARKES_DB_DIR"});
($s_user, $s_host, $s_path) = split(/:/,$cfg{"PARKES_DB_DIR"});

if ((! -d $logdir ) || (! -d $logdir."/archived"))  {
  print "Required directories did not exist";
  exit(1);
}

chdir $logdir;

checkFile($p_user, $p_host, $p_path, "parkes", FILES_DB);
checkFile($p_user, $p_host, $p_path, "parkes", TAPES_DB);
checkFile($s_user, $s_host, $s_path, "swin", FILES_DB);
checkFile($s_user, $s_host, $s_path, "swin", TAPES_DB);

exit(0);



sub checkFile($$$$) {
  
  my ($user, $host, $path, $type, $file) = @_;

  # Copy the current remote files to the current logdir
  my $cmd = "scp -q ".$user."@".$host.":".$path."/".$file." ./".$type.".".$file.".current";
  my $result = "";
  my $response = "";
  ($result, $response) = Dada::mySystem($cmd);

  if ($result ne "ok") {
    print $cmd." failed: ".$response."\n";
    exit(1);
  }

  my $curr = $type.".".$file.".current";
  my $prev = $type.".".$file.".previous";
  my $time = $type.".".$file.".".Dada::getCurrentDadaTime();
  my $diff = 0;

  if (-f $prev) {
    my $diff_result = `diff $curr $prev`;
    $diff = $?;
  } else {
    $diff = 1;
  }

  # If the files are the same (rval == 0)
  if ($diff == 0) {
    unlink $curr;

  # the files are different, make a timestamped backup;
  } else {

    unlink $prev;
    rename $curr, $prev;
    `cp $prev archived/$time`;
    `gzip archived/$time`;
  }

}
