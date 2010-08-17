<?PHP
include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

define(RESULTS_PER_PAGE,20);
define(PAGE_REFRESH_SECONDS,20);

# Get the system configuration (dada.cfg)
$cfg = getConfigFile(SYS_CONFIG,TRUE);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);


$length = RESULTS_PER_PAGE;
if (isset($_GET["length"])) {
  $length = $_GET["length"];
}

$offset = 0;
if (isset($_GET["offset"])) {
  $offset = $_GET["offset"];
} 

$basedir = $cfg["SERVER_RESULTS_DIR"];
$archive_ext = ".lowres";
$total_num_results = exec("ls -1 -I web_style.txt ".$basedir." | wc -l");

/* If we are on page 2 or more */
$newest_offset = "unset";
$newest_length = "unset";
if ($offset - $length >= RESULTS_PER_PAGE) {
  $newest_offset = 0;
  $newest_length = RESULTS_PER_PAGE;
}

/* If we are on page 1 or more */
$newer_offset = "unset";
$newer_length = "unset";
if ($offset > 0) {
  $newer_offset = $offset-RESULTS_PER_PAGE;
  $newer_length = RESULTS_PER_PAGE;
}

/* If we have at least 1 page of older results */
$older_offset = "unset";
$older_length = "unset";
if ($offset + RESULTS_PER_PAGE < $total_num_results) {
  $older_offset = $offset + RESULTS_PER_PAGE;
  $older_length = MIN(RESULTS_PER_PAGE, $total_num_results - $older_offset);
}

/* If we have at least 2 pages of older results */
$oldest_offset = "unset";
$oldest_length = "unset";
if ($offset + (RESULTS_PER_PAGE*2) < $total_num_results) {
  $oldest_offset = RESULTS_PER_PAGE * floor ($total_num_results / RESULTS_PER_PAGE);
  $oldest_length = $total_num_results - $oldest_offset;
}

$results = getResultsArray($basedir, $offset, $length);

$keys = array_keys($results);
$num_results = count($keys);

$types = array("bp", "pvf");
$images = getBPSRResults($basedir, $keys, $types, "400x300", "1");

?>

<html>

<?

$title = "BPSR | Recent Results";
$refresh = PAGE_REFRESH_SECONDS;
include("header_i.php");

?>

<body>
  <!--  Load tooltip module -->
  <script type="text/javascript" src="/js/wz_tooltip.js"></script>
<? 
$text = "Recent Results";
include("banner.php");

?>
<div align=right>
<table>
 <tr>
<?
  if ($newest_offset !== "unset") {
    echo "<td><a href=/bpsr/results.php?offset=".$newest_offset."&length=".$newest_length.">&#171; Newest</a></td>\n";
    echo "<td width=5>&nbsp;</td>\n";
  }

  if ($newer_offset !== "unset") {
    echo "<td><a href=/bpsr/results.php?offset=".$newer_offset."&length=".$newer_length.">&#8249; Newer</a></td>\n";
    echo "<td width=5>&nbsp;</td>\n";
  }
?>

   <td width=10>&nbsp;</td>
   <td><? echo "Showing <b>".$offset."</b> - <b>".($offset+$length)."</b> of <b>".$total_num_results."</b> results";?></td>
   <td width=10>&nbsp;</td>
<?

  if ($older_offset !== "unset") {
    echo "<td><a href=/bpsr/results.php?offset=".$older_offset."&length=".$older_length.">Older &#8250;</a></td>\n";
    echo "<td width=5>&nbsp;</td>\n";
  }

  if ($oldest_offset !== "unset") {
    echo "<td><a href=/bpsr/results.php?offset=".$oldest_offset."&length=".$oldest_length.">Oldest &#187;</a></td>\n";
  }
                                                                                                                                       

?>

 </tr>
</table>
</div>

<br>

<center>
<table class="datatable">
<tr>
  <th>Source</th>
  <th>UTC Start</th>
  <th>CFREQ</th>
  <th>Int</th>
  <th class="trunc">Annotation</th>
</tr>

<?

$keys = array_keys($results);

for ($i=0; $i < count($keys); $i++) {

  $header = array();
  $header_file = $results[$keys[$i]]["obs_start"];

  $data = $results[$keys[$i]];

  if (file_exists($header_file)) {
    $header = getConfigFile($header_file,TRUE);
  }

  $nbeams = $dada["nbeams"];

  $freq_keys = array_keys($results[$keys[$i]]);
  $url = "/bpsr/result.php?utc_start=".$keys[$i]."&imagetype=bp";
  $pos = strpos($images[$keys[$i]][0]["bp_400x300"], "blankimage");
  if ($pos !== FALSE) {
    $img = $images[$keys[$i]][0]["pvf_400x300"];
  } else {
    $img = $images[$keys[$i]][0]["bp_400x300"];
  }
  $mousein = "onmouseover=\"Tip('<img src=\'".$img."\' width=400 height=300>')\"";
  $mouseout = "onmouseout=\"UnTip()\"";

  /* If archives have been finalised and its not a brand new obs */
  if ( (count($results[$keys[$i]][$freq_keys[0]]) === 0) && 
       ($data["bp"] != "/images/blankimage.gif") ) {
    echo "  <tr bgcolor=\"#cae2ff\">\n";
  } else {
    echo "  <tr class=\"new\">\n";
  }

  /* SOURCE */
  echo "    <td>\n";

  echo "      <a href=\"".$url."\" ".$mousein." ".$mouseout.">".$header["SOURCE"]."</a></td>\n";

  /* UTC_START */
  echo "    <td>".$keys[$i]."</td>\n";

  /* CFREQ */
  echo "    <td>".$header["CFREQ"]."</td>\n";

  /* INTERGRATION LENGTH */
  echo "    <td>".getRecordingLength($img)."</td>\n";

  /* ANNOTATION */
  echo "    <td class=\"trunc\"><div>".$results[$keys[$i]]["annotation"]."</div></td>\n";

  echo "  </tr>\n";

}
?>
</table>

<br>

<table>
 <tr><td colspan=3 align=center>Legend</td></tr>
 <tr><td class="smalltext">CFREQ</td><td width=20></td><td class="smalltext">Centre frequency of the observation [MHz]</td></tr>
 <tr><td class="smalltext">Int</td><td width=20></td><td class="smalltext">Est. ntergration [seconds]</td></tr>
 <tr><td class="smalltext">White</td><td width=20></td><td class="smalltext">Newer results, may still be updated</td></tr>
 <tr><td class="smalltext">Blue</td><td width=20></td><td class="smalltext">Finalised results, no new archives received for 24 hours</td></tr>
</table> 

</body>
</html>

</center>
<?

function getResultsArray($results_dir, $offset=0, $length=0) {

  $all_results = array();

  $observations = array();
  $dir = $results_dir;

  $observations = getSubDirs($results_dir, $offset, $length, 1);

  /* For each observation get a list of frequency channels present */   
  for ($i=0; $i<count($observations); $i++) {

    $dir = $results_dir."/".$observations[$i];
    $freq_channels = getSubDirs($dir);

    $all_results[$observations[$i]]["nbeams"] = count($freq_channels);

    /* If no archives have been produced */
    if (count($freq_channels) == 0) {
      $all_results[$observations[$i]]["obs_start"] = "unset";
    }
  } 

  for ($i=0; $i<count($observations); $i++) {

    $dir = $results_dir."/".$observations[$i];
    $cmd = "find ".$dir." -name \"obs.start\" | tail -n 1";
    $an_obs_start = exec($cmd);
    $all_results[$observations[$i]]["obs_start"] = $an_obs_start;

    if (file_exists($dir."/obs.txt")) {
      $all_results[$observations[$i]]["annotation"] = file_get_contents($dir."/obs.txt");
    } else {
      $all_results[$observations[$i]]["annotation"] = "";
    }
  }

  return $all_results;
}


function getRecordingLength($image_name) {

  if ($image_name == "/images/blankimage.gif") {
    return 0;
  } else {

    $array = split("/",$image_name);

    $utc_start = $array[3];
    $image_basename = $array[5];
    $array = split("\.",$image_basename);
    $image_utc = $array[0];

    $offset = 0;
    if (strpos($image_basename, "pvf") !== FALSE) {
      $offset = (11*60*60);
    }

    # add ten as the 10 second image file has a UTC referring to the first byte of the file 
    $length = (unixTimeFromGMTime($image_utc)+(10-$offset)) - unixTimeFromGMTime($utc_start);

    return $length; 

  }

}




?>
