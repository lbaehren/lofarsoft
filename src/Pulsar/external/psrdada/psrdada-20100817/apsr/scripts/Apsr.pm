package Apsr;

use lib $ENV{"DADA_ROOT"}."/bin";

use IO::Socket;     # Standard perl socket library
use IO::Select;     # Allows select polling on a socket
use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);
use Dada;

require Exporter;
require AutoLoader;

@ISA = qw(Exporter AutoLoader);

@EXPORT_OK = qw(
  &getConfig
  &getApsrConfig
  &getApsrConfigFile
  &makePlotsFromArchives
);

$VERSION = '0.01';

my $DADA_ROOT = $ENV{'DADA_ROOT'};

use constant MAX_VALUE        => 1;
use constant AVG_VALUES       => 2;


sub getApsrConfig() {
  my $config_file = getApsrCFGFile();
  my %config = Dada::readCFGFileIntoHash($config_file, 0);
  return %config;
}

sub getApsrCFGFile() {
  return $DADA_ROOT."/share/apsr.cfg";
}


sub makePlotsFromArchives($$$$$) {

  my ($dir, $source, $total_f_res, $total_t_res, $res) = @_;

  my %local_cfg = Apsr::getApsrConfig();
  my $web_style_txt = $local_cfg{"SCRIPTS_DIR"}."/web_style.txt";
  my $psrplot_args = "-g ".$res." -jpC";
  my $cmd = "";

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
  `$cmd`;

  # PHASE vs FREQ
  $cmd = $bin." -p freq -jTD -D ".$dir."/pvfr_tmp/png ".$total_f_res;
  `$cmd`;

  # PHASE vs TOTAL INTENSITY
  $cmd = $bin." -p flux -jTF -D ".$dir."/pvfl_tmp/png ".$total_f_res;
  `$cmd`;


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

}


#
# Removes files that match pattern with age > specified age
#
sub removeFiles($$$;$) {

  (my $dir, my $pattern, my $age, my $loglvl=0) = @_;

  Dada::logMsg(2, $loglvl, "removeFiles(".$dir.", ".$pattern.", ".$age.")");

  my $cmd = "";
  my $result = "";
  my @array = ();

  # find files that match the pattern in the specified dir.
  $cmd  = "find ".$dir." -name '".$pattern."' -printf \"%T@ %f\\n\" | sort -n -r";
  Dada::logMsg(2, $loglvl, "removeFiles: ".$cmd);

  $result = `$cmd`;
  @array = split(/\n/,$result);

  my $time = 0;
  my $file = "";
  my $line = "";
  my $i = 0;

  # foreach file, check the age, always leave the oldest alone :)
  for ($i=1; $i<=$#array; $i++) {

    $line = $array[$i];
    ($time, $file) = split(/ /,$line,2);

    if (($time+$age) < time) {
      $file = $dir."/".$file;
      Dada::logMsg(2, $loglvl, "removeFiles: unlink ".$file);
      unlink($file);
    }
  }

  Dada::logMsg(2, $loglvl, "removeFiles: exiting");
}


sub getConfig() {
  my $config_file = $DADA_ROOT."/share/apsr.cfg";
  my %config = Dada::readCFGFileIntoHash($config_file, 0);
  return %config;
}


__END__
