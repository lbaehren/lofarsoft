<?PHP
include("definitions_i.php");
include("functions_i.php");

# Get the system configuration (dada.cfg)
$cfg = getConfigFile(SYS_CONFIG);

$basedir = $_GET["basedir"];
$observation = $_GET["observation"];
$archive_ext = $_GET["archive_ext"];

?>
<html>
<head>
  <title>APSR | Result <?echo $observation?></title>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML ?>
  <script type="text/javascript">

  window.name = "resultwindow";

  function popWindow(URL) {
    day = new Date();
    id = day.getTime();
    eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=1,scrollbars=1,location=1,statusbar=1,menubar=1,resizable=1,width=640,height=520');");
  }

  </script>
</head>
<body>
<? 

include("banner.php");

$obs_dir = $basedir."/".$observation;
$cfg = getConfigFile(SYS_CONFIG, TRUE);

$results_dir = $cfg["SERVER_RESULTS_DIR"]."/".$observation;
$archive_dir = $cfg["SERVER_ARCHIVE_DIR"]."/".$observation;

$results_data = getObservationData($results_dir, ".lowres");
$archive_data = getObservationData($archive_dir, ".ar");

$results_data["UTC_START"] = $observation;
$archive_data["UTC_START"] = $observation;
$header = getConfigFile($results_dir."/".$results_data["obs.start"], TRUE);

/* If less than 5 minutes since the last archive was recieved */
$locked = "";
if ($results_data["most_recent"] < 120) {
  $locked = " disabled";
}


?>

<table border=0>
  <tr>
    <td align="left" valign="top"> 
<?
    printSummary($results_dir, $results_data, $archive_dir, $archive_data, $cfg);
  
    echo "<br>\n";
    printSourceInfo($header);

    if (IN_CONTROL) {
      echo "<br>\n";
      printActions($results_data, $locked);
    }
?></td>
    <td align="right"> <?printHeader($header);?> </td>
  </tr>
  <tr>
    <td colspan=2><? printPlots($cfg,$observation,$results_data); ?></td>
  </tr>
</table>

</body>
</html>

<?

function getObservationData($obs_dir, $archive_ext) { 

  $data = array();
  $data["phase_vs_flux"] = "../../images/blankimage.gif";
  $data["phase_vs_time"] = "../../images/blankimage.gif";
  $data["phase_vs_freq"] = "../../images/blankimage.gif";
  $data["tres_archive"] = "total_t_res.ar";
  $data["most_recent"] = getMostRecentResult($obs_dir);

  /* Determine the total number of archives recieved in all bands */
  $cmd = "find ".$obs_dir."/* -name \"*".$archive_ext."\" | wc -l"; 
  $data["total_num_archives"] = exec($cmd);

  /* Determine the maximum number of results received in any band */
  $data["max_num_archives"] = 0;

  if ($handle = opendir($obs_dir)) {
    while (false !== ($file = readdir($handle))) {
      if ($file != "." && $file != "..") {
        # First handle the images:
        if (ereg("^phase_vs_flux([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $data["phase_vs_flux"] = $file;
        }
        if (ereg("^phase_vs_time([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $data["phase_vs_time"] = $file;
        }
        if (ereg("^phase_vs_freq([A-Za-z0-9\_\:-]*)240x180.png$",$file)) {
          $data["phase_vs_freq"] = $file;
        }
        if (ereg("^phase_vs_flux([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $data["phase_vs_flux_hires"] = $file;
        }
        if (ereg("^phase_vs_time([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $data["phase_vs_time_hires"] = $file;
        }
        if (ereg("^phase_vs_freq([A-Za-z0-9\_\:-]*)1024x768.png$",$file)) {
          $data["phase_vs_freq_hires"] = $file;
        }
        if (strpos($file,"processed.txt") !== FALSE) {
          $data["processed"] = $file;
        }
      }
    }
  }
  closedir($handle);

  $freq_channels = getSubDirs($obs_dir);

  /* Now get all the .lowres files for each freq channel */
  for ($j=0; $j<count($freq_channels); $j++) {

    if ((file_exists($obs_dir."/".$freq_channels[$j]."/obs.start")) && ($data["obs.start"] == "")) {
      $data["obs.start"] = $freq_channels[$j]."/obs.start"; 
    }
    
    $data[$freq_channels[$j]] = array();

    $dir = $obs_dir."/".$freq_channels[$j];
    $files = array();

    if (is_dir($dir)) {
      if ($dh = opendir($dir)) {
        while (($file = readdir($dh)) !== false) {
          if (($file != ".") && ($file != "..") && ( (strpos($file, $archive_ext, (strlen($file)-strlen($archive_ext))) !== FALSE))) {
            array_push($files, $file);
          }
        }
        closedir($dh);
      }
    }

    $data["max_num_archives"] = max($data["max_num_archives"],count($files));

    sort($files);

    for ($k=0; $k<count($files); $k++) {
      array_push($data[$freq_channels[$j]],$files[$k]);
    }
  }
                                                                                                                                                                                       
  return $data;
}

function printSummary($obs_dir, $results_data, $archive_dir, $archive_data, $cfg) {

  $keys = array_keys($results_data);

  $num_channels=0;
  $num_archives=0;

  for($i=0; $i<count($keys); $i++) {
    if (is_array($results_data[$keys[$i]])) {
      if (count($results_data[$keys[$i]]) > $num_archives) {
        $num_archives = count($results_data[$keys[$i]]);
      }
      $num_channels++;
    }
  }

  $archives_processed = 0;
  if (array_key_exists("processed",$results_data)) {
    $processed = getConfigFile($obs_dir."/processed.txt");
    $keys = array_keys($processed);
    for($i=0; $i<count($keys); $i++) {
       $archives_processed += count(split(" ",$processed[$keys[$i]]));
    }
  }

  $archives_received = $archive_data["total_num_archives"];
  $max_num_archives = $archive_data["max_num_archives"] * $num_channels;

  $results_received = $results_data["total_num_archives"];
  $max_num_results = $results_data["max_num_archives"] * $num_channels;

?>
  <table class="results" width=100% border=0>
    <tr><th colspan=2 class="results">OBSERVATION SUMMARY</th></tr>
    <tr><td align="right" class="results" width="50%">UTC Start</td><td class="results" width=50%><?echo $results_data["UTC_START"]?></td></tr>
    <tr><td align="right" class="results">Local Time Start</td><td class="results"><?echo localTimeFromGmTime($results_data["UTC_START"])?></td></tr>
    <tr><td align="right" class="results">Time Since Last Result</td><td class="results"><?echo makeTimeString($results_data["most_recent"])?></td></tr>
    <tr><td align="right" class="results">Total Intergrated</td><td class="results"><?echo getIntergrationLength($obs_dir."/".$results_data["tres_archive"])." seconds"?></td></tr>
    <tr><td align="right" class="results">Num Channels</td><td class="results"><? echo $num_channels?></td></tr>
    <tr><td align="right" class="results">Archives</td><td class="results"><? echo $archives_received?> of <?echo $max_num_archives?></td></tr>
    <tr><td align="right" class="results">Results</td><td class="results"><? echo $results_received?> of <?echo $max_num_results?></td></tr>
    <tr><td align="right" class="results">Archive Dir</td><td class="results"><? echo $cfg["SERVER_ARCHIVE_NFS_MNT"]."/".$results_data["UTC_START"]."/";?></td></tr>
    <tr><td align="right" class="results">Results Dir</td><td class="results"><? echo $cfg["SERVER_RESULTS_NFS_MNT"]."/".$results_data["UTC_START"]."/";?></td></tr>
  </table>

<?
}

function printActions($data, $locked) {
?>
  <table class="results" width=100% border=0 cellpadding=5>
    <tr><th class="results">ACTIONS</th></tr>
    <tr>
      <td align="center">
        <input type="button" onclick="popWindow('/apsr/processresult.php?observation=<?echo $data["UTC_START"]?>&action=plot')" value="Create Plots"<?echo $locked?>></td>
    </tr>
    <tr>
      <td align="center">
        <input type="button" onclick="popWindow('/apsr/processresult.php?observation=<?echo $data["UTC_START"]?>&action=reprocess_low')" value="Process low-res Archives"<?echo $locked?>>&nbsp;&nbsp;&nbsp;
        <input type="button" onclick="popWindow('/apsr/processresult.php?observation=<?echo $data["UTC_START"]?>&action=reprocess_hi')" value="Process hi-res Archives *"<?echo $locked?>>
      </td>
    </tr>
    <tr>
      <td align="center">
        <input type="button" onclick="popWindow('/apsr/processresult.php?observation=<?echo $data["UTC_START"]?>&action=annotate')" value="Annotate Obs.">&nbsp;&nbsp;&nbsp;
        <input type="button" onclick="popWindow('/apsr/processresult.php?observation=<?echo $data["UTC_START"]?>&action=delete_obs')" value="Delete Observation"<?echo $locked?>>
      </td>
    </tr>
    <tr><td align=center class="smalltext">* This may take a long time to complete</td></tr>
  </table>

<?

}


function printHeader($header) {

  $keys = array_keys($header);

  $keys_to_ignore = array("HDR_SIZE","FILE_SIZE","HDR_VERSION","FREQ","RECV_HOST");

?>
<table class="results">
  <tr> <th class="results" colspan=2>DADA HEADER</th></tr>
<?
  for ($i=0; $i<count($keys); $i++) {

    if (!(in_array($keys[$i], $keys_to_ignore))) {

      echo "  <tr>";
      echo "<td align=\"right\" class=\"results\">".$keys[$i]."</td>";
      echo "<td class=\"results\">".$header[$keys[$i]]."</td>";
      echo "</tr>\n";
    }
  }
?>
</table>
<?

}


function printPlots($cfg, $observation, $obs_data) {

?>

<table>
  <tr>
    <th class="results" colspan=3>PLOTS</th>
  </tr>
  <tr>
    <th class="results">Phase vs Flux</th>
    <th class="results">Phase vs Time</th>
    <th class="results">Phase vs Freq</th>
  </tr>
  <tr>
<? 

  printPlotRow($cfg, $observation, $obs_data["phase_vs_flux"], $obs_data["phase_vs_flux_hires"]);
  printPlotRow($cfg, $observation, $obs_data["phase_vs_time"], $obs_data["phase_vs_time_hires"]);
  printPlotRow($cfg, $observation, $obs_data["phase_vs_freq"], $obs_data["phase_vs_freq_hires"]);
?>
  </tr>
</table>

<?

}

function printPlotRow($cfg, $utc_start, $image, $image_hires) {

  $have_hires = 0;
  $full_hires_path = $cfg["SERVER_RESULTS_DIR"]."/".$utc_start."/".$image_hires;

  if ((strlen($image_hires) > 1) && (file_exists($full_hires_path))) {
    $have_hires = 1;
  } 

  echo "    <td class=\"results\" align=\"center\">\n";

  if ($have_hires) {
    echo "      <a href=\"/apsr/results/".$utc_start."/".$image_hires."\">";
  }

  echo "      <img src=\"/apsr/results/".$utc_start."/".$image."\">";

  if ($have_hires) {
    echo "    </a><br>\n";
    echo "    Click for hi-res result\n";
  }

  echo "    </td>\n";

}

function printSourceInfo($header) {

  $bin_dir = DADA_ROOT."/bin";

  # Determine the Period (P0)
  $cmd = $bin_dir."/psrcat -x -c \"P0\" ".$header["SOURCE"]." | awk '{print \$1}'";
  $P0 = rtrim(exec($cmd, $resturn_var));
  if ($P0 == "Warning:") {
    $P0 = "N/A";
  }
                                                                                                           
  # And the DM
  $cmd = $bin_dir."/psrcat -x -c \"DM\" ".$header["SOURCE"]." | awk '{print \$1}'";
  $DM = rtrim(`$cmd`);
  if ($DM == "Warning:") {
    $DM = "N/A";
  }

?>
  <table class="results" width="100%" border=0>
    <tr><th colspan=2 class="results">SOURCE SUMMARY</th></tr>
<?
  echo "    <tr><td align=right class=results>Source</td><td class=results>".$header["SOURCE"]."</td></tr>\n";
  echo "    <tr><td align=right class=results>DM</td><td class=results>".$DM."</td></tr>\n";
  echo "    <tr><td align=right class=results>P0</td><td class=results>".sprintf("%5.4f",$P0)."</td></tr>\n";
?>
    </tr>
  </table>
<?

}

function getMostRecentResult($dir) {

  $cmd = "find ".$dir."/*/ -name \"*.lowres\" -printf \"%T@\\n\" | sort | tail -n 1";
  $time_of_most_recent = rtrim(`$cmd`); 

  if (!is_numeric($time_of_most_recent)) {
    $cmd = "find ".$dir."/*/ -name \"obs.start\" -printf \"%T@\\n\" | sort | tail -n 1";
    $time_of_most_recent = `$cmd`;
  }

  $current_time = time();
  $difference = $current_time - $time_of_most_recent;
  return $difference;

}


?>
