<?PHP


include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

#$nbeam = $config["NUM_PWC"];
$nbeam = 13;

?>
<html>
<head>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML?>

  <script type="text/javascript">


    /* Creates a pop up window */
    function popWindow(URL,width,height) {

      var width = "1300";
      var height = "820";

      URL = URL + "&obsid=" + document.getElementById("utc_start").value;

      day = new Date();
      id = day.getTime();
      eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=1,scrollbars=1,location=1,statusbar=1,menubar=1,resizable=1,width="+width+",height="+height+"');");
    }

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
        var lines = response.split(";;;")

<?
        for ($i=0; $i<$config["NUM_PWC"]; $i++) {
          echo "        var img".$i."_line = lines[".$i."].split(\":::\")\n";
        }

        for ($i=0; $i<$config["NUM_PWC"]; $i++) {
          echo "        var img".$i."_hires = img".$i."_line[1]\n";
          echo "        var img".$i."_lowres = img".$i."_line[2]\n";
          echo "        var img".$i." = document.getElementById(\"beam".$config["BEAM_".$i]."\")\n";
        }

        for ($i=0; $i<$config["NUM_PWC"]; $i++) {

          echo "        if (img".$i.".src != img".$i."_lowres) {\n";
          echo "          img".$i.".src = img".$i."_lowres\n";
          echo "        }\n";
        }
?>
        if (document.imageform.imagetype[4].checked != true) {
<?
          echo "         var utc_start = lines[".$config["NUM_PWC"]."]\n";
?>
          document.getElementById("utc_start").value = utc_start;
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

      var type = "bp";

      if (document.imageform.imagetype[0].checked == true) {
        type = "bp";
      }

      if (document.imageform.imagetype[1].checked == true) {
        type = "ts";
      }

      if (document.imageform.imagetype[2].checked == true) {
        type = "fft";
      }

      if (document.imageform.imagetype[3].checked == true) {
        type = "dts";
      }

      if (document.imageform.imagetype[4].checked == true) {
        type = "pdbp";
      }

      if (document.imageform.imagetype[5].checked == true) {
        type = "pvf";
      }

      /* This URL will return the names of the 5 current */
      var url = "plotupdate.php?results_dir=<?echo $config["SERVER_RESULTS_DIR"]?>&type="+type;

      http_request.open("GET", url, true)
      http_request.send(null)
    }


  </script>

</head>
<body onload="looper()">
<script type="text/javascript" src="/js/wz_tooltip.js"></script>
<input id="utc_start" type="hidden" value="">
<center>
<table border=0 cellspacing=0 cellpadding=5>

  <tr>
    <td rowspan=3 valign="top">
      <form name="imageform" class="smalltext">
      <input type="radio" name="imagetype" id="imagetype" value="bp" checked onClick="request()">Bandpass<br>
      <input type="radio" name="imagetype" id="imagetype" value="ts" onClick="request()">Time Series<br>
      <input type="radio" name="imagetype" id="imagetype" value="fft" onClick="request()">Fluct. PS<br>
      <input type="radio" name="imagetype" id="imagetype" value="dts" onClick="request()">Digitizer Stats<br>
      <input type="radio" name="imagetype" id="imagetype" value="pdbp" onClick="request()">PD Bandpass<br>
      <input type="radio" name="imagetype" id="imagetype" value="pvf" onClick="request()">Phase v Freq<br>
      </form>
    </td>

    <?//echoBlank()?>
    <?echoBeam(13, $nbeam)?>
    <?echoBlank()?>
    <?echoBeam(12, $nbeam)?>
    <?echoBlank()?> 
  </tr>
  <tr height=42>
    <?//echoBlank()?>
    <?echoBeam(6, $nbeam)?>
    <?echoBlank()?>
  </tr>
  <tr height=42>
    <?//echoBlank()?>
    <?echoBeam(7, $nbeam)?>
    <?echoBeam(5, $nbeam)?>
    <?echoBlank()?> 
  </tr>

  <tr height=42>
    <?echoBeam(8, $nbeam)?>
    <?echoBeam(1, $nbeam)?>
    <?echoBeam(11, $nbeam)?>
  </tr>

  <tr height=42>
    <?echoBeam(2, $nbeam)?>
    <?echoBeam(4, $nbeam)?>
  </tr>

  <tr height=42>
    <?echoBlank()?>
    <?echoBeam(3, $nbeam)?>
    <?echoBlank()?>
  </tr>

  <tr height=42>
    <?echoBlank()?>
    <?echoBeam(9, $nbeam)?>
    <?echoBeam(10, $nbeam)?>
    <?echoBlank()?>
  </tr>
  
  <tr height=42>
    <?echoBlank()?>
    <?echoBlank()?>
    <?echoBlank()?>
  </tr>
</table>
</center>



</body>
</html>

<?

function echoBlank() {

  echo "<td ></td>\n";
}

function echoBeam($beam_no, $num_beams) {

  if ($beam_no <= $num_beams) {
    //$mousein = "onmouseover=\"Tip('<img src=/images/blankimage.gif width=241 height=181>')\"";
    //$mouseout = "onmouseout=\"UnTip()\"";

    $beam_str = sprintf("%02d", $beam_no);

    echo "<td rowspan=2 align=right>";
    echo "<a border=0px href=\"javascript:popWindow('beamwindow.php?beamid=".$beam_no."')\">";

    echo "<img src=\"/images/blankimage.gif\" border=0 width=112 height=84 id=\"beam".$beam_str."\" TITLE=\"Beam ".$beam_str."\" alt=\"Beam ".$beam_no."\" ".$mousein." ".$mouseout.">\n";
    echo "</a></td>\n";
  } else {
    echo "<td rowspan=2></td>\n";
  }

}

