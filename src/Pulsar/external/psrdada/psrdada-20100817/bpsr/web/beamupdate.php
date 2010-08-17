<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* Find the latest files in the plot file directory */
$img1 = "/images/blankimage.gif";
$img2 = "/images/blankimage.gif";
$img3 = "/images/blankimage.gif";
$img4 = "/images/blankimage.gif";

$results_dir = $_GET["results_dir"];
$cmd = "ls -trA ".$results_dir." | tail -n 1";
$result = exec($cmd);

$dir = $results_dir."/".$result;

if ($handle = opendir($dir)) {
  while (false !== ($file = readdir($handle))) {
    if ($file != "." && $file != "..") {
      if (ereg("^phase_vs_flux([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
        $img1 = "bpsr/".$result."/".$file;
      }
      if (ereg("^phase_vs_time([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
        $img2 = "bpsr/".$result."/".$file;
      }
      if (ereg("^phase_vs_freq([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
        $img3 = "/bpsr/".$result."/".$file;
      }
    }
  }
  closedir($handle);
} else {
  echo "Could not open plot directory: ".$dir."<BR>\n";
}

$url = "http://".$_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"];

echo "img1:::".$url.$img1.";;;";
echo "img2:::".$url.$img2.";;;";
echo "img3:::".$url.$img3.";;;";
echo "img4:::".$url.$img4.";;;";
