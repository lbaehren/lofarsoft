<?PHP

include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

$utc_start = $_GET["utc_start"];
if (! isset($_GET["imagetype"])) {
  $imagetype = "bandpass";
} else {
  $imagetype = $_GET["imagetype"];
}

if (! isset($_GET["brief"])) {
  $brief = 1;
} else {
  $brief = $_GET["brief"];
}

$data = getResultsInfo($utc_start, $config["SERVER_RESULTS_DIR"]);
$images = getBPSRResults($config["SERVER_RESULTS_DIR"], $utc_start, "all", array("400x300","112x84"), "all");

$nbeam = $data["nbeams"];
$header = getConfigFile($data["obs_start"], TRUE);

$obs_info_file = $config["SERVER_RESULTS_DIR"]."/".$utc_start."/obs.info";
$obs_info = getConfigFile($obs_info_file);


?>
<html>

<?
$title = "BPSR | Observation ".$utc_start;
include("header_i.php"); 
?>

<body>
<script type="text/javascript" src="/js/wz_tooltip.js"></script>
<script type="text/javascript">

  function changeImage(type) {
<?
    $patterns = array("/&imagetype=bp/", "/&imagetype=ts/", "/&imagetype=fft/", "/&imagetype=dts/", "/&imagetype=pvf/");
    $replacements= array("", "", "", "");
    $cleaned_uri = preg_replace($patterns, $replacements, $_SERVER["REQUEST_URI"]);
    echo "var newurl = \"".$cleaned_uri."&imagetype=\"+type\n";
?>
    document.location = newurl

  }

</script>
<? 
$text = "Observation ".$utc_start;
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
  <td width=50px>&nbsp;</td>
  <td valign="top">
<?printHeader($header, $brief);?>
  </td>
</tr>
</table>
</center>

<center>
<table border=0 cellspacing=10 cellpadding=0 class="multibeam">

  <tr>
    <td rowspan=3>
      <form name="imageform" class="smalltext">
<?
      echoRadio("imagetype","bp", "Bandpass", $imagetype); 
      echo "<br>\n";
      echoRadio("imagetype","ts", "Time Series", $imagetype); 
      echo "<br>\n";
      echoRadio("imagetype","fft", "Power Spectrum", $imagetype); 
      echo "<br>\n";
      echoRadio("imagetype","dts", "Digitizer Statistics", $imagetype);
      echo "<br>\n";
      echoRadio("imagetype","pvf", "Phase v Freq", $imagetype);
  ?>
      </form>
    
    <?echoBeam(13, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?>
    <?echoBeam(12, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?> 
  </tr>
  <tr>
   
    <?echoBeam(6, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?>
  </tr>
  <tr>
  
    <?echoBeam(7, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBeam(5, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?> 
  </tr>

  <tr>
    <?echoBeam(8, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBeam(1, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBeam(11, $nbeam, $imagetype, $data, $images, $utc_start)?>
  </tr>

  <tr>
    <?echoBeam(2, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBeam(4, $nbeam, $imagetype, $data, $images, $utc_start)?>
  </tr>

  <tr>
    <?echoBlank()?>
    <?echoBeam(3, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?>
  </tr>

  <tr>
    <?echoBlank()?>
    <?echoBeam(9, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBeam(10, $nbeam, $imagetype, $data, $images, $utc_start)?>
    <?echoBlank()?>
  </tr>
  
  <tr>
    <?echoBlank()?>
    <?echoBlank()?>
    <?echoBlank()?>
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

function echoBeam($beam_no, $num_beams, $imagetype, $data, $images, $utc_start) {

  if ($beam_no <= $num_beams) {

    $img_med = $images[$utc_start][($beam_no-1)][$imagetype."_400x300"];
    $img_low = $images[$utc_start][($beam_no-1)][$imagetype."_112x84"];

    $mousein = "onmouseover=\"Tip('<img src=".$img_med." width=400 height=300>')\"";
    $mouseout = "onmouseout=\"UnTip()\"";

    echo "<td rowspan=2 class=\"multibeam\" height=84>";
    echo "<a class=\"multibeam\" href=\"javascript:popWindow('bpsr/beamwindow.php?beamid=".$beam_no."')\">";

    echo "<img src=\"".$img_low."\" width=112 height=84 id=\"beam".$beam_no."\" border=0 TITLE=\"Beam ".$beam_no."\" alt=\"Beam ".$beam_no."\" ".$mousein." ".$mouseout.">\n";

    echo "</a>";
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

