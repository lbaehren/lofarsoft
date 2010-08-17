<?PHP

include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);
$basedir = $config["SERVER_RESULTS_DIR"];

# Get the UTC STARTS for each beam
$beams = array();
for ($i=1; $i<=13; $i++) {
  $b = sprintf("%02d",$i);
  if (isset($_GET["BEAM_".$i])) {
    array_push($beams, $_GET["BEAM_".$i]);
    $beam_utcs[$b] = $_GET["BEAM_".$i];
  } else {
    $beam_utcs[$b] = "";
  }
}

$num_beams = count($beams);

$brief = 1;
if (isset($_GET["brief"])) {
  $brief = $_GET["brief"];
}

# Get the pvf images for the specified utc's
$images = getBPSRResults($basedir, $beams, "pvf",  array("400x300","112x84"), "all");
$snrs = getBPSRSNRS($config["SERVER_ARCHIVE_DIR"], $beam_utcs);

# Get some information from the header
if (array_key_exists(0,$beams)) {
  $data = getResultsInfo($beams[0], $config["SERVER_RESULTS_DIR"]);
  $nbeam = $data[$beams[0]]["nbeams"];
  $header = getConfigFile($data[$beams[0]]["obs_start"], TRUE);

  $obs_info_file = $config["SERVER_RESULTS_DIR"]."/".$beams[0]."/obs.info";
  $obs_info = getConfigFile($obs_info_file);
}

for ($i=1; $i<=13; $i++) {
  $b = sprintf("%02d",$i);
  if ($beam_utcs[$b] == "") {
    $beam_utcs[$b] = "NONE";
  }
}


?>
<html>

<?
$title = "BPSR | Beam-O-Vision Observation ";
include("header_i.php"); 
?>

<body>
<script type="text/javascript" src="/js/wz_tooltip.js"></script>
<script type="text/javascript">
  /* Creates a pop up window */

  function popWindow(URL,width,height) {

    var width = "1300";
    var height = "820";

    day = new Date();
    id = day.getTime();
    eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=1,scrollbars=1,location=1,statusbar=1,menubar=1,resizable=1,width="+width+",height="+height+"');");
  }
</script>



<? 
$text = "Beam-O-Vision [".$num_beams."]<BR>".$beams[0]." - ".$beams[($num_beams-1)];
include("banner.php"); 
?>

<center>
<input id="utc_start" type="hidden" value="">
<table cellspacing=5>
<tr>
  <td valign="top">
    <table class="datatable"> 
      <tr><th colspan=2>Observation Summary</th></tr>
<?
        echo "    <tr><td>UTC_START</td><td>".$obs_info["UTC_START"]."</td></tr>\n";
        echo "    <tr><td>SOURCE</td><td>".$obs_info["SOURCE"]."</td></tr>\n";
        echo "    <tr><td>RA</td><td>".$obs_info["RA"]."</td></tr>\n";
        echo "    <tr><td>DEC</td><td>".$obs_info["DEC"]."</td></tr>\n";
        echo "    <tr><td>FA</td><td>".$obs_info["FA"]."</td></tr>\n";
        echo "    <tr><td>ACC_LEN</td><td>".$obs_info["ACC_LEN"]."</td></tr>\n";
        echo "    <tr><td>NUM BEAMS</td><td>".$obs_info["NUM_PWC"]."</td></tr>\n";
?>
    </table>
  </td>
  <td>
<table border=0 cellspacing=10 cellpadding=0 class="multibeam">

  <tr>
    <td rowspan=3>
    <?echoBeam(13, 13, $images, $beam_utcs["13"], $snrs["13"])?>
    <?echoBlank()?>
    <?echoBeam(12, 13, $images, $beam_utcs["12"], $snrs["12"])?>
    <?echoBlank()?> 
  </tr>
  <tr>
    <?echoBeam(6, 13, $images, $beam_utcs["06"], $snrs["06"])?>
    <?echoBlank()?>
  </tr>
  <tr>
    <?echoBeam(7, 13, $images, $beam_utcs["07"], $snrs["07"])?>
    <?echoBeam(5, 13, $images, $beam_utcs["05"], $snrs["05"])?>
    <?echoBlank()?> 
  </tr>

  <tr>
    <?echoBeam(8, 13, $images, $beam_utcs["08"], $snrs["08"])?>
    <?echoBeam(1, 13, $images, $beam_utcs["01"], $snrs["01"])?>
    <?echoBeam(11, 13, $images, $beam_utcs["11"], $snrs["11"])?>
  </tr>

  <tr>
    <?echoBeam(2, 13, $images, $beam_utcs["02"], $snrs["02"])?>
    <?echoBeam(4, 13, $images, $beam_utcs["04"], $snrs["04"])?>
  </tr>

  <tr>
    <?echoBlank()?>
    <?echoBeam(3, 13, $images, $beam_utcs["03"], $snrs["03"])?>
    <?echoBlank()?>
  </tr>

  <tr>
    <?echoBlank()?>
    <?echoBeam(9, 13, $images, $beam_utcs["09"], $snrs["09"])?>
    <?echoBeam(10, 13, $images, $beam_utcs["10"], $snrs["10"])?>
    <?echoBlank()?>
  </tr>
  
  <tr>
    <?echoBlank()?>
    <?echoBlank()?>
    <?echoBlank()?>
  </tr>
</table>

  <td valign="top">
<?printHeader($header, $brief);?>
  </td>
</tr>
</table>
</center>


</body>
</html>

<?

function echoRadio($id, $value, $title, $selected) {

  echo "<input type=\"radio\" name=\"".$id."\" id=\"".$id."\" value=\"".$value."\" onChange=\"changeImage('".$value."')\"";

  if ($value == $selected)
    echo " checked";
  
  echo ">".$title;

}

function echoBlank() {

  echo "<td><img src=\"/images/spacer.gif\" width=113 height=45></td>\n";
}

function echoBeam($beam_no, $num_beams, $images, $utc_start, $snr) {

  if ($beam_no <= $num_beams) {

    if (array_key_exists($utc_start, $images)) {
      $img_med = $images[$utc_start][($beam_no-1)]["pvf_400x300"];
      $img_low = $images[$utc_start][($beam_no-1)]["pvf_112x84"];
    } else {
      $img_med = "/images/blankimage.gif";
      $img_low = "/images/blankimage.gif";
    }

    $mousein = "onmouseover=\"Tip('<img src=".$img_med." width=400 height=300>')\"";
    $mouseout = "onmouseout=\"UnTip()\"";

    echo "<td rowspan=2 class=\"multibeam\" height=84>";
    echo "<a class=\"multibeam\" href=\"javascript:popWindow('/bpsr/beamwindow.php?beamid=".$beam_no."&obsid=".$utc_start."')\">";

    echo "<img src=\"".$img_low."\" width=112 height=84 id=\"beam".$beam_no."\" border=0 TITLE=\"Beam ".$beam_no."\" alt=\"Beam ".$beam_no."\" ".$mousein." ".$mouseout.">\n";

    echo "</a><br>SNR: ".$snr;
    echo "</td>\n";
  } else {
    echo "<td rowspan=2></td>\n";
  }

}

function printHeader($header, $brief) {

  $keys = array_keys($header);
  $keys_to_ignore = array("HDR_SIZE","FILE_SIZE","HDR_VERSION","FREQ","RECV_HOST","CONFIG",
                          "DSB", "INSTRUMENT", "NBIT", "NDIM", "NPOL", "PROC_FILE", "TELESCOPE",
                          "BEAM", "BW", "RA", "DEC", "FA", "UTC_START", "NCHAN", "BANDWIDTH",
                          "ACC_LEN");

  if (substr_count($_SERVER["REQUEST_URI"],"brief=") == 0) {

    $brief_url = $_SERVER["REQUEST_URI"]."&brief=1";
    $full_url = $_SERVER["REQUEST_URI"]."&brief=0";

  } else {
    $brief_url = str_replace("brief=0", "brief=1", $_SERVER["REQUEST_URI"]);
    $full_url  = str_replace("brief=1", "brief=0", $_SERVER["REQUEST_URI"]);
  }

  echo "<table cellpadding=0 cellspacing=0 class=\"datatable\">\n";
  echo "<tr>";
  if ($brief) 
    echo "<th colspan=2>Brief Header | <a href=".$full_url.">Full Header</a></th>\n";
  else 
    echo "<th colspan=2><a href=".$brief_url.">Brief Header</a> | Full Header</th>\n";

  for ($i=0; $i<count($keys); $i++) {
    if ( (!($brief)) || (!(in_array($keys[$i], $keys_to_ignore))) ) {
      echo "<tr><td align=right>".$keys[$i]."</td><td>".$header[$keys[$i]]."</td></tr>\n";
    }
  }
  echo "</table>\n";

  #$mousein = "onmouseover=\"Tip('".$header_text."')\"";
  #$mouseout = "onmouseout=\"UnTip()\"";
  #echo "<div color=blue ".$mousein." ".$mouseout.">DADA HEADER</div>\n";

}

function getBPSRSNRS($basedir, $beam_utcs) {

  $keys = array_keys($beam_utcs);
  $snrs = array();

  for ($i=0; $i<count($keys); $i++) {
    $beam = $keys[$i];
    $utc = $beam_utcs[$beam];
    $archive = $basedir."/".$utc."/".$beam."/integrated.ar";
    if (file_exists($archive)) {

      $cmd = "source /home/dada/.bashrc; psrstat -j'zap median' -j FTp -qc snr ".$archive." | grep '^ snr' | awk -F= '{printf(\"%6.2f\",$2)}'";
      $lastline = exec($cmd, $array, $return_var);
      if ($return_var == 0) {
        $snrs[$beam] = $lastline;
      } else {
        $snrs[$beam] = "psrstat error: ".$lastline;
      }
    } else {
      $snrs[$beam] = "unknown";
    }
  }
  return $snrs;

}

