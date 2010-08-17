<?PHP 
include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");
$config = getConfigFile(SYS_CONFIG);


/* We may want all the current images for just one beam
   or we may want all the latest images of one type for
   all beams */
$type = "all";
$beam = "all";
$ibob = "all";
$obs  = "latest";

$n_results = $config["NUM_PWC"];
$i_result  = 0;
$url = "http://".$_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"];

if (isset($_GET["type"])) 
  $type = $_GET["type"];

if (isset($_GET["beam"]))
  $beam = $_GET["beam"];

if (isset($_GET["obs"]))
  $obs = $_GET["obs"];

/* if we just want images for 1 beam */
if ($beam != "all") {
  for ($j=0; $j<$config["NUM_PWC"]; $j++) {
    if ($config["BEAM_".$j] == $beam) 
      $i_result = $j;
  }

  $ibob = $config["IBOB_DEST_".$i_result];
  $n_results = 1;

} 

/* Find the latest files in the plot file directory */
for ($i=0; $i<$n_results; $i++) {
  $lowres_imgs[$i] = "/images/blankimage.gif";
  $midres_imgs[$i] = "/images/blankimage.gif";
  $hires_imgs[$i] = "/images/blankimage.gif";
}

$result = "";
$results_dir = $_GET["results_dir"];

if ($obs == "latest") {
  $cmd = "ls -I stats -1 ".$results_dir." | tail -n 1";
  $result = exec($cmd);
} else {
  $result = $obs;
}

/* If we want all image types for one beam only (beamwindow.php) */
if ($type == "all") {

  $obs_results = getBPSRResults($results_dir, $obs, "all", "all", $beam);

  $actual_obs_results = array_pop(array_pop($obs_results));

  $stats_results = getBPSRStatsResults($results_dir, $ibob);  

  $actual_stats_results = array_pop($stats_results);

  $results = array_merge($actual_obs_results, $actual_stats_results);

  $types = array("bp","ts","fft","dts","pdbp","pvf");
  $sizes = array("112x84", "400x300", "1024x768");

  for ($i=0; $i<count($types); $i++) {
    for ($j=0; $j<count($sizes); $j++) {
      $key = $types[$i]."_".$sizes[$j];
      echo ":::".$key.":::".$results[$key];
    }
    echo ";;;";
  }

/* We want all beams, but just one type of image (otherstuff.php) */
} else {

  $results = array();

  if ($type == "pdbp") {
    $results = getBPSRStatsResults($results_dir, "all");
    for ($i=0; $i<$config["NUM_PWC"]; $i++) {
      $ibob = $config["IBOB_DEST_".$i];
      echo "img".$i.":::".$url.$results[$ibob][$type."_400x300"].":::".$url.$results[$ibob][$type."_112x84"].";;;";
    }
  } else {
    $results = array_pop(getBPSRResults($results_dir, "latest", $type, "all", "all"));
    for ($i=0; $i<$config["NUM_PWC"]; $i++) {
      echo "img".$i;

      if ($results[$i][$type."_400x300"] != "" ) {
        echo ":::".$url.$results[$i][$type."_400x300"];
      } else {
        echo ":::".$url."/images/blankimage.gif";
      }

      if ($results[$i][$type."_112x84"] != "") {
        echo ":::".$url.$results[$i][$type."_112x84"];
      } else {
        echo ":::".$url."/images/blankimage.gif";
      }
      echo ";;;";
    }
  }
  echo $result.";;;";
}


