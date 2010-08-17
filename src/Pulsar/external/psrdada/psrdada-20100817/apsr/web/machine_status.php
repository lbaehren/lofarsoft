<?PHP

/*
 * Displays the load, data block and disk information for each PWC, also for the 2 servers
 */

include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$machine = $_GET["machine"];

$pwcs = array();
$servers = array();
$helpers = array();

for ($i=0; $i<$config["NUM_PWC"]; $i++) {
  $pwcs[$i] = $config["PWC_".$i];
}
for ($i=0; $i<$config["NUM_HELP"]; $i++) {
  $helpers[$i] = $config["HELP_".$i];
}
for ($i=0; $i<$config["NUM_SRV"]; $i++) {
  $servers[$i] = $config["SRV_".$i];
}

$machines = array_merge($pwcs, $servers, $helpers);

$gang_base =  "http://".$_SERVER["HTTP_HOST"]."/ganglia/";
$clients_network = $gang_base."graph.php?g=network_report&z=medium&c=APSR%20Clients&m=&r=hour&s=descending&hc=4";
$clients_load = $gang_base."graph.php?g=load_report&z=medium&c=APSR%20Clients&m=&r=hour&s=descending&hc=4";
$srv0_network = $gang_base."graph.php?g=network_report&z=medium&c=APSR%20Servers&h=srv0.apsr.edu.au&m=&r=hour&s=descending&hc=4";
$srv0_load = $gang_base."graph.php?g=load_report&z=medium&c=APSR%20Servers&h=srv0.apsr.edu.au&m=&r=hour&s=descending&hc=4";

// Don't allow this page to be cached, since it should always be fresh.
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT"); // Date in the past

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <link rel="STYLESHEET" type="text/css" href="style_log.css">

  <style>
    th {
      text-align: center;
      vertical-align: top;
    }
  </style>

  <!-- jsProgressBarHandler prerequisites : prototype.js -->
  <script type="text/javascript" src="/js/prototype.js"></script>
  <!-- jsProgressBarHandler core -->
  <script type="text/javascript" src="/js/jsProgressBarHandler.js"></script>

  <script type="text/javascript">

  var url = "http://<?echo $_SERVER["HTTP_HOST"]?>/<?echo INSTRUMENT?>/machineupdate.php";

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
      var i = 0;
      var now = new Date();
      var lines;

      if (response.indexOf("Could not connect to") == 0) {

        lines = new Array();
<?      
        $j = 0;
        for ($i=0; $i<count($pwcs); $i++) {
          echo "        lines[$j] = \"".$pwcs[$i].":stopped:0 0 0;;;0 0;;;0.00,0.00,0.00;;;0.0;;;0.0\";\n";
          $j++;
        }
        for ($i=0; $i<count($servers); $i++) {
          echo "        lines[$j] = \"".$servers[$i].":stopped:0 0 0;;;0 0;;;0.00,0.00,0.00;;;0.0;;;0.0\";\n";
          $j++;
        }
        for ($i=0; $i<count($helpers); $i++) {
          echo "        lines[$j] = \"".$helpers[$i].":stopped:0 0 0;;;0 0;;;0.00,0.00,0.00;;;0.0;;;0.0\";\n";
          $j++;
        }
?>
      } else {
        lines = response.split("\n");
      }

      for (i=0; i<lines.length; i++) {
      
        if (lines[i].length > 0) {
     
          var values = lines[i].split(":");
    
          var host = values[0];
          var result = values[1];
          var statuses = values[2].split(";;;");

          var disks = new Array(0,0,1);
          var dbs = new Array(0,0);
          var loads = new Array(0.00, 0.00, 0.00);
          var unproc_gb = 0; 
          var temp = 0;

          var disk_percent = 0;
          var disk_gb = 0;
          var db_percent = 0;
          var load_percent = 0;
          var mb_per_block =  0;
          var gb_unprocessed = 0;


          // if the machine is offline
          if (result != "ok") {
            document.getElementById(host+"_row").bgColor = "#FF0000";
            document.getElementById(host+"_state").innerHTML = result;

          } else {
            document.getElementById(host+"_row").bgColor = "";
            document.getElementById(host+"_state").innerHTML = "";

            disks = statuses[0].split(" ");
            dbs   = statuses[1].split(" ");
            loads = statuses[2].split(",");
            unproc_gb = parseFloat(statuses[3])/1024;
            temp  = statuses[4];

            disk_percent = Math.floor((parseInt(disks[0]) - parseInt(disks[2])) / parseInt(disks[0])*100); 
            disk_gb = (parseInt(disks[2])) / 1024.0; 

            // Special case for no data block
            if ((parseInt(dbs[1]) == 0) && (parseInt(dbs[0]) == 0) ) {
              db_percent = 0;
            } else {
              db_percent = Math.floor(parseInt(dbs[1]) / parseInt(dbs[0])*100);
            }
            load_percent = Math.floor((parseFloat(loads[0])/8)*100);

            mb_per_block = <?echo $config[$config["PROCESSING_DATA_BLOCK"]."_BLOCK_BUFSZ"]?> / (1024*1024*1024);
            gb_unprocessed = (mb_per_block*parseFloat(dbs[1]) + unproc_gb);
          }

<?
for ($i=0; $i<count($pwcs); $i++) {
  $m = $pwcs[$i];
  echo "if (host == \"".$m."\") {\n";
  echo "  ".$m."_db.setPercentage(db_percent);\n";
  echo "  document.getElementById(\"".$m."_db_value\").innerHTML = \"&nbsp;\"+dbs[1]+\"&nbsp;of&nbsp;\"+dbs[0]\n";
  echo "  ".$m."_disk.setPercentage(disk_percent);\n";
  echo "  document.getElementById(\"".$m."_disk_unproc\").innerHTML = \"&nbsp;\"+unproc_gb.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "  ".$m."_load.setPercentage(load_percent);\n";
  echo "  document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "  document.getElementById(\"".$m."_gb_unproc\").innerHTML = \"&nbsp;\"+gb_unprocessed.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "}\n";
}

for ($i=0; $i<count($servers); $i++) {
  $m = $servers[$i];
  echo "if (host == \"".$m."\") {\n";
  echo "  ".$m."_disk.setPercentage(disk_percent);\n";
  echo "  document.getElementById(\"".$m."_disk_unproc\").innerHTML = \"&nbsp;\"+unproc_gb.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "  ".$m."_load.setPercentage(load_percent);\n";
  echo "  document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "  document.getElementById(\"".$m."_gb_unproc\").innerHTML = \"&nbsp;\"+gb_unprocessed.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "}\n";
}

for ($i=0; $i<count($helpers); $i++) {
  $m = $helpers[$i];
  echo "if (host == \"".$m."\") {\n";
  echo "  ".$m."_disk.setPercentage(disk_percent);\n";
  echo "  document.getElementById(\"".$m."_disk_unproc\").innerHTML = \"&nbsp;\"+unproc_gb.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_disk_left\").innerHTML = \"&nbsp;\"+disk_gb.toFixed(1)\n";
  echo "  ".$m."_load.setPercentage(load_percent);\n";
  echo "  document.getElementById(\"".$m."_load_value\").innerHTML = \"&nbsp;\"+loads[0]\n";
  echo "  document.getElementById(\"".$m."_gb_unproc\").innerHTML = \"&nbsp;\"+gb_unprocessed.toFixed(1)\n";
  echo "  document.getElementById(\"".$m."_temp_value\").innerHTML = temp\n";
  echo "}\n";
}

?>
        }
      }
      var theTime = now.getTime();
      document.getElementById("clients_load").src = "<?echo $clients_load?>?"+theTime;
      document.getElementById("clients_network").src = "<?echo $clients_network?>?"+theTime;
      document.getElementById("servers_load").src = "<?echo $srv0_load?>?"+theTime;
      document.getElementById("servers_network").src = "<?echo $srv0_network?>?"+theTime;
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

<table cellpadding=0 border=0 width=100%>
<tr>
  <th colspan=2>Machine</th>
  <th colspan=2>Load</th>
  <td width=20px></td>
  <th>Temp</th>
  <td width=20px></td>
  <th colspan=2>Data Block</th>
  <td width=20px></td>
  <th>Disk [GB]</th><td align=right>Used</td><td align=right>Free</td><td align=right>Total.</td>
</tr>

<?
for ($i=0; $i<count($machines); $i++) {
  $pwc = 0;
  if (in_array($machines[$i],$pwcs)) {
    $pwc = 1;
  }
?>
<tr id="<?echo $machines[$i]?>_row">
  <td width="30px" align="right"><?echo $machines[$i].": "?></td>

  <td><span id="<?echo $machines[$i]?>_state"></span></td>

  <td width="80px">
  <? echo "<span id=\"".$machines[$i]."_load_progress_bar\">[  Loading Progress Bar ]</span>\n"; ?>
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
  <? if ($pwc) { echo "<span id=\"".$machines[$i]."_db_progress_bar\">[  Loading Progress Bar ]</span>\n"; } ?>
  </td>

  <td width="60px">
   <? if ($pwc) {echo "<span id=\"".$machines[$i]."_db_value\"></span>\n"; } ?>
  </td>

  <td></td>

  <td width="80px">
  <?  echo "<span id=\"".$machines[$i]."_disk_progress_bar\">[  Loading Progress Bar ]</span>\n";?>
  </td>

  <td width="30px" align=right>
   <? echo "<span id=\"".$machines[$i]."_disk_unproc\"></span>\n"?>
  </td>

  <td width="40px" align=right>
  <?echo "<span id=\"".$machines[$i]."_disk_left\"></span>\n";?>
  </td>

  <td width="30px" align=right>
  <?echo "<span id=\"".$machines[$i]."_gb_unproc\"></span>\n";?>
  </td>

</tr>

<? } ?>
</table>

<br><br>

<center>
<table cellpadding=10px>
<tr><td>
<img id="clients_load" src="<?echo $clients_load?>" width="298px" height="115px">
</td><td>
<img id="clients_network" src="<?echo $clients_network?>" width="298px" height="115px">
</td></tr>
<tr><td>
<img id="servers_load" src="<?echo $srv0_load?>" width="298px" height="115px">
</td><td>
<img id="servers_network" src="<?echo $srv0_network?>" width="298px" height="115px">
</td></tr>
</table>
</center>

</body>
</html>
