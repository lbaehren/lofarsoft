<?PHP

include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$machine = $_GET["machine"];
$machines = array();

for ($i=0; $i<$config["NUM_PWC"]; $i++) {
  array_push($machines,$config["PWC_".$i]);
}  

// Don't allow this page to be cached, since it should always be fresh.
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT"); // Date in the past
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <link rel="STYLESHEET" type="text/css" href="style_log.css">
  <!-- jsProgressBarHandler prerequisites : prototype.js -->
  <script type="text/javascript" src="/js/prototype.js"></script>
  <!-- jsProgressBarHandler core -->
  <script type="text/javascript" src="/js/jsProgressBarHandler.js"></script>

  <script type="text/javascript">

  var url = "http://<?echo $_SERVER["HTTP_HOST"]?>/apsr/gain_update.php"

  function looper() {
    request()
    setTimeout('looper()',2000)
  }

  function request() {

    if (window.XMLHttpRequest)
      http_request = new XMLHttpRequest();
    else
      http_request = new ActiveXObject("Microsoft.XMLHTTP");

    http_request.onreadystatechange = function() {
      handle_data(http_request)
    };
    http_request.open("GET", url, true);
    http_request.send(null);

  }
                                                                                                                                    
  function handle_data(http_request) {
    if (http_request.readyState == 4) {
      var response = String(http_request.responseText)
      var values = response.split(" ");
      var i = 0;
      var now = new Date();

<?
      for ($i=0; $i< $config["NUM_PWC"]; $i++) {
        echo "      var ".$machines[$i]."_pol0_value = values[".(2*$i)."]\n";
        echo "      var ".$machines[$i]."_pol0_percent = Math.floor(100 * parseInt(".$machines[$i]."_pol0_value) / 65535)\n";
        echo "      ".$machines[$i]."_pol0.setPercentage(".$machines[$i]."_pol0_percent)\n";
        echo "      document.getElementById(\"".$machines[$i]."_pol0_value\").innerHTML = \"&nbsp;\"+".$machines[$i]."_pol0_value\n";

        echo "      var ".$machines[$i]."_pol1_value = values[".((2*$i)+1)."]\n";
        echo "      var ".$machines[$i]."_pol1_percent = Math.floor(100 * parseInt(".$machines[$i]."_pol1_value) / 65535)\n";
        echo "      ".$machines[$i]."_pol1.setPercentage(".$machines[$i]."_pol1_percent)\n";
        echo "      document.getElementById(\"".$machines[$i]."_pol1_value\").innerHTML = \"&nbsp;\"+".$machines[$i]."_pol1_value\n";
      }
?>
    }
  }
  </script>

</head>
<body onload="looper()">

  <!-- Progress Bars for All 3 data blocks -->
  <script type="text/javascript">
    Event.observe(window, 'load', function() {
<?

for ($i=0; $i<count($machines); $i++) {

  echo $machines[$i]."_pol0 = new JS_BRAMUS.jsProgressBar($('".$machines[$i]."_pol0_bar'), 0, ";
  echo " { width : 80, showText : false, barImage : Array( '/images/jsprogress/percentImage_back1_80.png', '/images/jsprogress/percentImage_back2_80.png', '/images/jsprogress/percentImage_back3_80.png', '/images/jsprogress/percentImage_back4_80.png') } );\n";
 
  echo $machines[$i]."_pol1 = new JS_BRAMUS.jsProgressBar($('".$machines[$i]."_pol1_bar'), 0, ";
  echo " { width : 80, showText : false, barImage : Array( '/images/jsprogress/percentImage_back1_80.png', '/images/jsprogress/percentImage_back2_80.png', '/images/jsprogress/percentImage_back3_80.png', '/images/jsprogress/percentImage_back4_80.png') } );\n";

}

?>
  }, false);
  </script>

<center>
<table cellpadding=0 border=0 marginwidth=0 marginheight=0>
<tr>
  <th></th>
  <th colspan=2 align=center>Pol 0 Gain</th>
  <th></th>
  <th colspan=2 align=center>Pol 1 Gain</th>
</tr>

<?
for ($i=0; $i<count($machines); $i++) {
?>
<tr>
  <td align="right"><?echo $machines[$i].": "?></td>

  <td width="80">
  <? echo "<span id=\"".$machines[$i]."_pol0_bar\">[  Loading Progress Bar ]</span></td>\n"; ?>
  </td>

  <td>
  <? echo "<span id=\"".$machines[$i]."_pol0_value\"></span>\n"; ?>
  </td>

  <td width=100px></td>

  <td width="80px">
  <? echo "<span id=\"".$machines[$i]."_pol1_bar\">[  Loading Progress Bar ]</span></td>\n"; ?>
  </td>
                                                                                                                                                                                       
  <td>
  <? echo "<span id=\"".$machines[$i]."_pol1_value\"></span>\n"; ?>
  </td>

</tr>
<? } ?>
</table>
</center>

</body>
</table>
</html>
  
