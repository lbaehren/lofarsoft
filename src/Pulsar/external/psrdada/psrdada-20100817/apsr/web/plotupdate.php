<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* Find the latest files in the plot file directory */
$img1 = "/images/blankimage.gif";
$img2 = "/images/blankimage.gif";
$img3 = "/images/blankimage.gif";
$img4 = "/images/blankimage.gif";
$img1_hi = "/images/blankimage.gif";
$img2_hi = "/images/blankimage.gif";
$img3_hi = "/images/blankimage.gif";
$img4_hi = "/images/blankimage.gif";

$results_dir = $_GET["results_dir"];
$cmd = "ls -1 ".$results_dir." | tail -n 1";
$result = exec($cmd);

$dir = $results_dir."/".$result;

$got_images = 3;
while ($got_images) {
  if ($handle = opendir($dir)) {

    while (false !== ($file = readdir($handle))) {
      if ($file != "." && $file != "..") {
        if (ereg("^phase_vs_flux([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $img1 = "/apsr/results/".$result."/".$file;
        }
        if (ereg("^phase_vs_time([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $img2 = "/apsr/results/".$result."/".$file;
        }
        if (ereg("^phase_vs_freq([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $img3 = "/apsr/results/".$result."/".$file;
        }
        if (ereg("^phase_vs_flux([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $img1_hi = "/apsr/results/".$result."/".$file;
        }
        if (ereg("^phase_vs_time([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $img2_hi = "/apsr/results/".$result."/".$file;
        }
        if (ereg("^phase_vs_freq([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $img3_hi = "/apsr/results/".$result."/".$file;
        }
      }
    }
    closedir($handle);
  } else {
    echo "Could not open plot directory: ".$dir."<BR>\n";
  }

  if ( ($img1 == "/images/blankimage.gif") ||
       ($img2 == "/images/blankimage.gif") ||
       ($img3 == "/images/blankimage.gif") ) {
    $got_images--;
  } else {
    $got_images = 0;
  }
}

$url = "http://".$_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"];

echo "img1:::".$url.$img1.";;;";
echo "img2:::".$url.$img2.";;;";
echo "img3:::".$url.$img3.";;;";
echo "img4:::".$url.$img4.";;;";
echo "img1_hi:::".$url.$img1_hi.";;;";
echo "img2_hi:::".$url.$img2_hi.";;;";
echo "img3_hi:::".$url.$img3_hi.";;;";
echo "img4_hi:::".$url.$img4_hi.";;;";
