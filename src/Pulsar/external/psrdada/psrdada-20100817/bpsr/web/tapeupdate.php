<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* Need to clear PHP's internal cache */
clearstatcache();

/* Find most recent result in results dir */
$control_dir = $_GET["control_dir"];

if (isset($_GET["tapeinserted"])) {

  if (isset($_GET["location"])) {
    $location = strtolower($_GET["location"]);
  } else {
    $location = "parkes"; 
  }

  $fname = $control_dir."/".$location.".response.tmp";
  if (!($fp = @fopen($fname, 'w'))) {
    echo "ERROR: could not open response file for writing: $fname<BR>\n";
    exit();
  }

  fwrite($fp, $_GET["tapeinserted"]."\n");
  fclose($fp);
  chmod($fname,0775);
  rename($fname, $control_dir."/".$location.".response");

} else {

$parkes_file = $control_dir."/parkes.state";
$swin_file = $control_dir."/swin.state";
$xfer_file = $control_dir."/xfer.state";

$parkes_state = "unknown";
$swin_state = "unknown";
$xfer_state = "unknown";

if (file_exists($parkes_file)) {
  $parkes_state = rtrim(file_get_contents($parkes_file));
}
if (file_exists($swin_file)) {
  $swin_state = rtrim(file_get_contents($swin_file));
}
if (file_exists($xfer_file)) {
  $xfer_state = rtrim(file_get_contents($xfer_file));
}

echo "PARKES:::".$parkes_state.";;;";
echo "SWIN:::".$swin_state.";;;";
echo "XFER:::".$xfer_state.";;;";

}
