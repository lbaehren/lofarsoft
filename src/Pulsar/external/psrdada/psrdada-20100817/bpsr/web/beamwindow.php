<?PHP

include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

$obsid = $_GET["obsid"];    // UTC Start
$beam_id = sprintf("%02d", $_GET["beamid"]); // Beam number [1-13]

$config = getConfigFile(SYS_CONFIG);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

$text = "";
$base_dir = $config["SERVER_RESULTS_DIR"]."/".$obsid."/".$beam_id;

if (file_exists($base_dir)) {
  $nbeams = exec("find ".$config["SERVER_RESULTS_DIR"]."/".$obsid."/* -type d | wc -l");
                                                                                                                                                
  $img_base = "/bpsr/results/".$obsid."/".$beam_id."/";
                                                                                                                                                
  $obs_info =  $config["SERVER_RESULTS_DIR"]."/".$obsid."/obs.info";
  $obs_start = $base_dir."/obs.start";

  $obs_results = array_pop(array_pop(getBPSRResults($config["SERVER_RESULTS_DIR"], $obsid, "all", "all", $beam_id)));
  $stats_results = array_pop(getBPSRStatsResults($config["SERVER_RESULTS_DIR"], $ibob));
  $results = array_merge($obs_results, $stats_results);

  if ( file_exists($obs_start) ) {
    $header = getConfigFile($obs_start);
    $text = "Beam ".$beam_id." for ".$header["SOURCE"];
  } else {
    $header = array();
    $text = "Beam ".$beam_id." for ".$obsid;
  }

} else {
  $text = "No observation found";
}

?>
<html>
<head>
  <title>BPSR | Result <?echo $obsid?></title>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML?>

  <script type="text/javascript">
  /* Looping function to try and refresh the images */
  function looper() {
    request()
    setTimeout('looper()',5000)
  }

 /* Parses the HTTP response and makes changes to images
   * as requried */
  function handle_data(http_request) {
                                                                                                                                                                 
    if (http_request.readyState == 4) {
      var response = String(http_request.responseText)
      var lines = response.split(";;;");

      var img_bp_line = lines[0].split(":::");
      var img_ts_line = lines[1].split(":::");
      var img_fft_line = lines[2].split(":::");
      var img_dts_line = lines[3].split(":::");
      var img_pdbp_line = lines[4].split(":::");
      var img_pvf_line = lines[5].split(":::");

      var img_bp_mid = img_bp_line[4]
      var img_bp =  document.getElementById("bp_img");

      var img_ts_mid = img_ts_line[4]
      var img_ts =  document.getElementById("ts_img");

      var img_fft_mid = img_fft_line[4]
      var img_fft =  document.getElementById("fft_img");
      
      var img_dts_mid = img_dts_line[4]
      var img_dts =  document.getElementById("dts_img");

      var img_pdbp_mid = img_pdbp_line[4]
      var img_pdbp =  document.getElementById("pdbp_img");

      var img_pvf_mid = img_pvg_line[4]
      var img_pvf =  document.getElementById("pvf_img");

      if (img_bp.src != img_bp_mid) {
        img_bp.src = img_bp_mid
      }

      if (img_ts.src != img_ts_mid) {
        img_ts.src = img_ts_mid
      }

      if (img_fft.src != img_fft_mid) {
        img_fft.src = img_fft_mid
      }

      if (img_dts.src != img_dts_mid) {
        img_dts.src = img_dts_mid
      }

      if (img_pdbp.src != img_pdbp_mid) {
        img_pdbp.src = img_pdbp_mid
      }

      if (img_pvf.src != img_pvf_mid) {
        img_pvf.src = img_pvf_mid
      }
    }
  }

  /* Gets the data from the URL */
  function request() {
    if (window.XMLHttpRequest)
      http_request = new XMLHttpRequest()
    else
      http_request = new ActiveXObject("Microsoft.XMLHTTP");

    http_request.onreadystatechange = function() {
      handle_data(http_request)
    }

    /* This URL will return the names of the 5 current */
    var url = "/bpsr/plotupdate.php?results_dir=<?echo $config["SERVER_RESULTS_DIR"]?>&type=all&obs=<?echo $obsid?>&beam=<?echo $beam_id?>"

    http_request.open("GET", url, true)
    http_request.send(null)
  }

  </script>
</head>

<body onload="looper()">


<? 

include("banner.php"); 

if (! (file_exists($base_dir))) {

?>
<center>
<table>
  <tr> <td colspan=2 bgcolor=red>Could not find the observation</td></tr>
  <tr> <td>UTC_START</td><td><?echo $obsid?></td> </tr>
  <tr> <td>BEAM_ID</td><td><?echo $beam_id?></td> </tr>
  <tr> <td>BASE_DIR</td><td><?echo $base_dir?></td> </tr>
</table>
</center>
<?

} else {

?>

<center>
<table border=0>
  <tr valign=middle>
    <td>Beam</td>
<?

  for ($i=1; $i <= $nbeams; $i++) {
    if ($i != $_GET["beamid"]) {
      echo "    <td width=40><div class=\"btns\">\n";
      echo "      <a href=\"/bpsr/beamwindow.php?beamid=".$i."&obsid=".$obsid."\" class=\"btn\" > <span>".sprintf("%02d",$i)."</span></a>";
      echo "    </div></td>";
    } else {
      echo "    <td width=40 align=center><b>".sprintf("%02d",$i)."</b></td>";
    }
  }
?>
  </tr>
</table>

<hr>

</center>


<table cellpadding=5>
<tr><td valign=top align=center>

  <table class="datatable" style="width:300px">
    <tr><th colspan=2>Beam / Obs Info</th></tr>
    <tr><td width=50%>Source</td><td align=left width=50%><?echo $header["SOURCE"]?></td></tr>
    <tr><td>UTC_START</td><td align=left><?echo $obsid?></td></tr>
    <tr><td>ACC_LEN</td><td align=left><?echo $header["ACC_LEN"]?></td></tr>
    <tr><td>RA</td><td align=left><?echo $header["RA"]?></td></tr>
    <tr><td>DEC</td><td align=left><?echo $header["DEC"]?></td></tr>
    <tr><td>FA</td><td align=left><?echo $header["FA"]?></td></tr>
    <tr><td>Beam</td><td align=left><?echo $beam_id?> of <?echo $nbeams?></td></tr>
  </table>

  </td>
  <td valign=top>

   <table class="datatable" style="width:300px">
    <tr><th colspan=2>Obs State Information</th></tr>
    <tr><td width=50%>Finalized</td><td align=left width=50%><?echo $state["FINALIZED"]?></td></tr>
    <tr><td>Transferred to swin</td><td align=left><?echo $state["sent.to.swin"]?></td></tr>
    <tr><td>Transferred to parkes</td><td align=left><?echo $state["sent.to.parkes"]?></td></tr>
    <tr><td>On tape at swin</td><td align=left><?echo $state["on.tape.swin"]?></td></tr>
    <tr><td>On tape at parkes</td><td align=left><?echo $state["on.tape.parkes"]?></td></tr>
  </table>

  </td>
  <td>
  </td>
</tr>
<tr>

  <td align=center width="33%">
    Bandpass<br>
    <img id="bp_img" src="<?echo $results["bp_400x300"]?>" width=320 height=240>

  </td>
  <td align=center width="33%">
    DM0 Timeseries<br>
    <img id="ts_img" src="<?echo $results["ts_400x300"]?>" width=320 height=240>
  </td>
  <td align=center width="33%">
    Phase vs Freq<br>
    <img id="pvf_img" src="<?echo $results["pvf_400x300"]?>" width=320 height=240>
  </td>
</tr>

<tr>
  <td align=center>
    PD Bandpass (live)<br>
    <img id="pdbp_img" src="<?echo $results["pdbp_400x300"]?>" width=320 height=240>
  </td>

  <td align=center>
    Fluctuation Power Spectrum<br>
    <img id="fft_img" src="<?echo $results["fft_400x300"]?>" width=320 height=240>
  </td>
  <td align=center>
    Digitizer Statistics<br>
    <img id="dts_img" src="<?echo $results["dts_400x300"]?>" width=320 height=240>
  </td>
</tr>

</table>

</td></tr></table>

<? 
}
?>

</body>
</html>

<?

function getImages($dir, $img_base) {

  $data = array();

  /* Find the latest files in the plot file directory */
  $types = array("bp", "ts", "fft", "dts", "pvf");

  for ($i=0; $i<count($types); $i++) {
    $data[$types[$i]."_low"] = "/images/blankimage.gif";
    $data[$types[$i]."_mid"] = "/images/blankimage.gif";
    $data[$types[$i]."_hi"] = "/images/blankimage.gif";
  }

  /* Get into a relative dir... */
  $cwd = getcwd();
  chdir($dir);

  for ($i=0; $i<count($types); $i++) {
    /* Find the hi res images */
    $cmd = "find . -name \"*.".$types[$i]."_1024x768.png\" -printf \"%P\n\" | sort | tail -n 1";
    $find_result = exec($cmd, $array, $return_val);
    if (($return_val == 0) && (strlen($find_result) > 1)) {
      $data[$types[$i]."_hi"] = $img_base.$find_result;
    }

    /* Find the mid res images */
    $cmd = "find . -name \"*.".$types[$i]."_400x300.png\" -printf \"%P\n\" | sort | tail -n 1";
    $find_result = exec($cmd, $array, $return_val);
    if (($return_val == 0) && (strlen($find_result) > 1))  {
      $data[$types[$i]."_mid"] = $img_base.$find_result;
    }

    /* Find the low res images */
    $cmd = "find . -name \"*.".$types[$i]."_112x84.png\" -printf \"%P\n\" | sort | tail -n 1";
    $find_result = exec($cmd, $array, $return_val);
    if (($return_val == 0) && (strlen($find_result) > 1))  {
      $data[$types[$i]."_low"] = $img_base.$find_result;
    }
  }
  chdir($cwd);

  return $data;
}

function imageWithRollover($img_low, $img_low_x, $img_low_y, $img_hi, $img_hi_x, $img_hi_y) {


  $string = "";

  if ($img_hi != "/images/blankimage.gif") {
    $mousein = "onmouseover=\"Tip('<img src=\'".$img_hi."\' width=".$img_hi_x." height=".$img_hi_y.">')\"";
    $mouseout = "onmouseout=\"UnTip()\"";
    $string = "<a href=".$img_hi.">"; 
  }

  $string .= "<img src=\"".$img_low."\" width=".$img_low_x." height=".$img_low_y." ".$mousein." ".$mouseout.">";

  if ($img_hi != "/images/blankimage.gif") {
    $string .= "</a>\n";
  }

  return $string;

}
?>
