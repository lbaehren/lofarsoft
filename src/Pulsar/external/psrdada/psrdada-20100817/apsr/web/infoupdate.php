<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* Need to clear PHP's internal cache */
clearstatcache();

/* Find most recent result in results dir */

$results_dir = $_GET["results_dir"];

$cmd = "ls -1 ".$results_dir." | tail -n 1";
$result = exec($cmd, $return_var);

$obs_info = $results_dir."/".$result."/obs.info";
$curr_archive = $results_dir."/".$result."/total_t_res.ar";

$bin_dir = DADA_ROOT."/bin";

if (file_exists($obs_info)) {

  $header = getConfigFile($obs_info);

  $P0 = "N/A";
  $DM = "N/A";

  if (count($header["SOURCE"]) > 0) {
  
  # Determine the Period (P0)
    $cmd = "source /home/dada/.bashrc; ".$bin_dir."/psrcat -x -c \"P0\" ".$header["SOURCE"]." | awk '{print \$1}'";
    $P0 = rtrim(`$cmd`);
    if ($P0 == "WARNING:") {
      $P0 = "N/A";
    } else {
      $P0 *= 1000;
    }

    # And the DM
    $cmd = "source /home/dada/.bashrc; ".$bin_dir."/psrcat -x -c \"DM\" ".$header["SOURCE"]." | awk '{print \$1}'";
    $DM = rtrim(`$cmd`);
    if ($DM == "WARNING:") {
      $DM = "N/A";
    }
  }

  echo "SOURCE:::".$header["SOURCE"].";;;";
  echo "RA:::".$header["RA"].";;;";
  echo "DEC:::".$header["DEC"].";;;";
  echo "CFREQ:::".$header["CFREQ"].";;;";
  echo "BANDWIDTH:::".$header["BANDWIDTH"].";;;";
  echo "NUM_PWC:::".$header["NUM_PWC"].";;;";
  echo "NPOL:::".$header["NPOL"].";;;";
  echo "NBIT:::".$header["NBIT"].";;;";
  echo "PID:::".$header["PID"].";;;";
  echo "UTC_START:::".$result.";;;";
  echo "P0:::".sprintf("%5.4f",$P0).";;;";
  echo "DM:::".$DM.";;;";

  $int_length = "0";
  if (file_exists($curr_archive)) {
    $int_length = getIntergrationLength($curr_archive);
  }  
  echo "INTEGRATED:::".$int_length." seconds;;;";

} else {

  echo "SOURCE:::None;;;";
  echo "RA:::None;;;";
  echo "DEC:::None;;;";
  echo "CFREQ:::None;;;";
  echo "BANDWIDTH:::None;;;";
  echo "NUM_PWC:::None;;;";
  echo "NPOL:::None;;;";
  echo "NBIT:::None;;;";
  echo "PID:::None;;;";
  echo "UTC_START:::None;;;";
  echo "INTEGRATED:::0;;;";
  echo "P0:::N/A;;;";
  echo "DM:::N/A;;;";
}
