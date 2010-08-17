<?PHP

/*
 * Displays the load, data block and disk information for each PWC, also for the 2 servers
 */

include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$machine = $_GET["machine"];

$pwcs = array();
$servers = array("srv0", "srv1");
$helpers = array();

for ($i=0; $i<$config["NUM_PWC"]; $i++) {
  $pwcs[$i] = $config["PWC_".$i];
}
for ($i=0; $i<$config["NUM_HELP"]; $i++) {
  $helpers[$i] = $config["HELP_".$i];
}

$machines = array_merge($pwcs, $servers, $helpers);
$gang_base =  "http://".$_SERVER["HTTP_HOST"]."/ganglia/";
$gang_network = $gang_base."graph.php?g=network_report&z=medium&c=APSR%20Clients&m=&r=hour&s=descending&hc=4";
$gang_load = $gang_base."graph.php?g=load_report&z=medium&c=APSR%20Clients&m=&r=hour&s=descending&hc=4";

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

  var url = "http://<?echo $_SERVER["HTTP_HOST"]?>/<?echo INSTRUMENT?>/machineupdate.php"

  function PadDigits(n, totalDigits) 
  { 
    n = n.toString(); 
    var pd = ''; 
    if (totalDigits > n.length) 
    { 
      for (i=0; i < (totalDigits-n.length); i++) 
      { 
        pd += '0'; 
      } 
    } 
    return pd + n.toString(); 
  } 

  function looper() {
    request()
    setTimeout('looper()',4000)
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
      var lines = response.split("\n");
      var i = 0;
      var now = new Date();
      var data_rate = 32000000; // Mbytes per sec

      if (response.indexOf("Could not connect to") == -1) {

      for (i=0; i<lines.length; i++) {
      
        if (lines[i].length > 0) {
     
          var values = lines[i].split(":");
    
          var host = values[0];
          var result = values[1];

          var statuses= values[2].split(";;;");

          var disks
          var dbs
          var loads
          var temp
          //var unproc_gb

          var disk_percent = 0
          var disk_gb = 0
          var disk_mb = 0
          var db_percent = 0
          var load_percent = 0
          var mb_per_block =  0
          //var gb_unprocessed = 0
          var time_4 = new Date();
          var time_52 = new Date();
          var time_4_str = ""
          var time_52_str = ""

          if (statuses.length == 4) {

            disks = statuses[0].split(" ");
            dbs   = statuses[1].split(" ");
            loads = statuses[2].split(",");
            temp  = statuses[3];
            //unproc_gb = parseFloat(statuses[3])/1024;

            if ((disks[0] == 0) && (disks[1] == 0)) {
              disk_percent = 0;
              disk_mb = 0;
              disk_gb = 0;
            } else {
              disk_percent = Math.floor(parseInt(disks[1]) / parseInt(disks[0])*100); 
              //disk_mb = (parseInt(disks[0]) - parseInt(disks[1]));
              disk_mb = parseInt(disks[2]);
              disk_gb = disk_mb / 1024.0; 
            }

            if ((parseInt(dbs[1]) == 0) && (parseInt(dbs[0]) == 0) ) {
              db_percent = 0;
            } else {
              db_percent = Math.floor(parseInt(dbs[1]) / parseInt(dbs[0])*100);
            }

            load_percent = Math.floor((parseFloat(loads[0])/8)*100);

            mb_per_block = <?echo $config[$config["PROCESSING_DATA_BLOCK"]."_BLOCK_BUFSZ"]?> / (1024*1024*1024);

            time_4.setTime((disk_mb / 4.0) * 1000);
            time_52.setTime((disk_mb / 52.0) * 1000);

            time_4_str = PadDigits(time_4.getUTCDate()-1,2)+":"+PadDigits(time_4.getUTCHours(),2)+":"+PadDigits(time_4.getUTCMinutes(),2);
            time_52_str =  PadDigits(time_52.getUTCDate()-1,2)+":"+ PadDigits(time_52.getUTCHours(),2)+":"+ PadDigits(time_52.getUTCMinutes(),2);

          }

<?

for ($i=0; $i<count($pwcs); $i++) {
  $m = $pwcs[$i];
  echo "        if (host == \"".$m."\") {\n";
  echo "          document.getElementById(\"".$m."_time_left\").innerHTML = \"&nbsp;\"+time_4_str\n";
  echo "          ".$m."_db.setPercentage(db_percent);\n";
  echo "          document.getElementById(\"".$m."_db_value\").innerHTML = \"&nbsp;\"+dbs[1]+\"&nbsp;of&nbsp;\"+dbs[0]\n";
  echo "          ".$m."_disk.setPercentage(disk_percent);\n";
  echo "          document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "          ".$m."_load.setPercentage(load_percent);\n";
  echo "          document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "          document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "        }\n";
}

for ($i=0; $i<count($servers); $i++) {
  $m = $servers[$i];
  echo "        if (host == \"".$m."\") {\n";
  echo "          document.getElementById(\"".$m."_time_left\").innerHTML = \"&nbsp;\"\n";
  echo "          ".$m."_disk.setPercentage(disk_percent);\n";
  echo "          document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "          ".$m."_load.setPercentage(load_percent);\n";
  echo "          document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "          document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "        }\n";
}

for ($i=0;$i<count($helpers); $i++) {
  $m = $helpers[$i];
  echo "        if (host == \"".$m."\") {\n";
  echo "          document.getElementById(\"".$m."_time_left\").innerHTML = \"&nbsp;\"+time_52_str\n";
  echo "          ".$m."_disk.setPercentage(disk_percent);\n";
  echo "          document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "          ".$m."_load.setPercentage(load_percent);\n";
  echo "          document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "          document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "        }\n";
}
?>
        }
      }
      }
      var theTime = now.getTime();
      document.getElementById("load").src = "<?echo $gang_load?>?"+theTime;
      document.getElementById("network").src = "<?echo $gang_network?>?"+theTime;
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


  if (in_array($machines[$i], $pwcs)) {
    echo $machines[$i]."_db = new JS_BRAMUS.jsProgressBar($('".$machines[$i]."_db_progress_bar'), 0, ";
    echo " { width : 80, showText : false, barImage : Array( '/images/jsprogress/percentImage_back1_80.png', '/images/jsprogress/percentImage_back2_80.png', '/images/jsprogress/percentImage_back3_80.png', '/images/jsprogress/percentImage_back4_80.png') } );\n";
  }

  echo $machines[$i]."_disk = new JS_BRAMUS.jsProgressBar($('".$machines[$i]."_disk_progress_bar'), 0, ";
  echo " { width : 80, showText : false, barImage : Array( '/images/jsprogress/percentImage_back1_80.png', '/images/jsprogress/percentImage_back2_80.png', '/images/jsprogress/percentImage_back3_80.png', '/images/jsprogress/percentImage_back4_80.png') } );\n";

  echo $machines[$i]."_load = new JS_BRAMUS.jsProgressBar($('".$machines[$i]."_load_progress_bar'), 0, ";
  echo " { width : 80, showText : false, barImage  : Array( '/images/jsprogress/percentImage_back1_80.png', '/images/jsprogress/percentImage_back2_80.png', '/images/jsprogress/percentImage_back3_80.png', '/images/jsprogress/percentImage_back4_80.png') } );\n";

}

?>
  }, false);
  </script>

<center>
<table cellpadding=0 border=0>
<tr>
  <td colspan=3 align=center> <b>Machine Load</b> </td>
  <td width=20px>
  <td align=center> <b>Temp</b> </td>
  <td width=30px>
  <td colspan=2 align=center> <b>Data Block</b> </td>
  <td width=30px>
  <td align=center> <b>Disk [GB]</b> </td>
  <td align=right>Free</td>
  <td align=right>Time Left</td>
</tr>

<?
for ($i=0; $i<count($machines); $i++) {
  $pwc = 0;
  if (in_array($machines[$i],$pwcs)) {
    $pwc = 1;
  }
?>
<tr>
  <td width="30px" align="right"><?echo $machines[$i].": "?></td>

  <td width="80px">
  <? echo "<span id=\"".$machines[$i]."_load_progress_bar\">[  Loading Progress Bar ]</span></td>\n"; ?>
  </td>

  <td width="30px">
  <? echo "<span id=\"".$machines[$i]."_load_value\"></span>\n"; ?>
  </td>

  <td></td>

  <td width="30px" align=right>
  <? echo "<span id=\"".$machines[$i]."_temp_value\"></span>\n"; ?>
  </td>

  <td></td>

  <td width="80px">
  <? if ($pwc) { echo "<span id=\"".$machines[$i]."_db_progress_bar\">[  Loading Progress Bar ]</span></td>\n"; }?>
  </td>

  <td width="50px">
   <? if ($pwc) { echo "<span id=\"".$machines[$i]."_db_value\"></span>\n"; } ?>
  </td>

  <td></td>

  <td width="80px">
  <?  echo "<span id=\"".$machines[$i]."_disk_progress_bar\">[  Loading Progress Bar ]</span>\n";?>
  </td>

  <td width="40px" align=right>
  <?echo "<span id=\"".$machines[$i]."_disk_left\"></span>\n";?>
  </td>

  <td width=70px" align=right>
  <?echo "<span id=\"".$machines[$i]."_time_left\"></span>\n";?>
  </td>

</tr>
<tr height="100%"><td></td></tr>
<? } ?>
</table>
<br>
<br>
<img id="load" src="<?echo $gang_load?>">
<br>
<br>
<img id="network" src="<?echo $gang_network?>">
</center>

</body>
</table>
</html>
