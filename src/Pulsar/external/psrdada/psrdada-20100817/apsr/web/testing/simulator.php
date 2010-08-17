<?PHP
include("../definitions_i.php");
include("../functions_i.php");
?>

<html> 

<?

$title = "APSR | TCS Simulator";
include("../header_i.php");

if (!IN_CONTROL) { ?>
<h3><font color=red>Test system disabled as your host is not in control of the instrument</font></h3>
</body>
</html>
<?
  exit(0);
}

?>

<body>
<table width=90% border=0 cellpadding=0 cellspacing=0 class="datatable">
 <tr>
  <th width="50%">TCS Simulator</th>
  <th width="50%">TCS Interface</th>
 </tr>

<?

$sys_config = getConfigFile(SYS_CONFIG);

$dfb3_config_file = "";
$spec_file = "";
$duration = 30;

/* Client master control sockets */
$cmc_sockets = array(); 

if (isset($_GET["cfg"])) {
  $dfb3_config_file = $_GET["cfg"];
}
if (isset($_GET["spec"])) {
  $spec_file = $sys_config["CONFIG_DIR"]."/".$_GET["spec"];
}
if (isset($_GET["duration"])) {
  $duration = $_GET["duration"];
} else {
  printTR("Error: duration not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}


if (isset($_GET["nbit"])) {
  $nbit = $_GET["nbit"];
} else {
  printTR("Error: nbit not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}


if (isset($_GET["ndim"])) {
  $ndim = $_GET["ndim"];
} else {
  printTR("Error: ndim not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}

if (isset($_GET["npol"])) {
  $npol = $_GET["npol"];
} else {
  printTR("Error: npol not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}

if (isset($_GET["bandwidth"])) {
  $bandwidth = $_GET["bandwidth"];
} else {
  printTR("Error: bandwidth not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}

if (!(file_exists($sys_config["CONFIG_DIR"]."/".$dfb3_config_file))) {
  printTR("Error: DFB3 Configuration File \"".$_GET["cfg"]."\" did not exist","");
  printTF();
  printFooter();
  exit(-1);
} else {
  
}

if (!(file_exists($spec_file))) {
  printTR("Error: Specification File \"".$_GET["spec"]."\" did not exist","");
  printTF();
  printFooter();
  exit(-1);
}

$specification = getRawTextFile($spec_file);
$spec = getConfigFile($spec_file);

# Add NBIT, NDIM & NPOL to the specification
array_push($specification,"NBIT        ".$nbit); 
array_push($specification,"NPOL        ".$npol); 
array_push($specification,"NDIM        ".$ndim); 
array_push($specification,"BANDWIDTH   ".$bandwidth); 


/* Open a connection to the TCS interface script */
$host = $sys_config["TCS_INTERFACE_HOST"];
$port = $sys_config["TCS_INTERFACE_PORT"];

$tcs_interface_socket = 0;

list ($tcs_interface_socket,$message) = openSocket($host,$port,2);
if (!($tcs_interface_socket)) {
  printTR("Error: opening socket to TCS interface script \"".$message."\"","");
  printTF();
  printFooter();
  exit(-1);
} 

# Send the DFB3 Configuration File to the tcs_interface socket
$cmd = "CONFIG ".$dfb3_config_file."\r\n";
socketWrite($tcs_interface_socket,$cmd);
$result = rtrim(socketRead($tcs_interface_socket));
printTR($cmd,$result);

if ($result != "ok") {
  stopCommand($tcs_interface_socket);
  exit(-1);
}


# Send each header parameter to TCS interface
for ($i=0;$i<count($specification);$i++) {
  $cmd = $specification[$i]."\r\n";
  socketWrite($tcs_interface_socket,$cmd);
  $result = rtrim(socketRead($tcs_interface_socket));
  printTR($cmd,$result);

  if ($result != "ok") {
    printTR($cmd, $result);
    stopCommand($tcs_interface_socket);
    exit(-1);
  }
}

# Issue START command to server_tcs_interface 
$cmd = "START\r\n";
socketWrite($tcs_interface_socket,$cmd);
$result = rtrim(socketRead($tcs_interface_socket));
if ($result != "ok") {
  printTR("START command failed on nexus ", $result.": ".rtrim(socketRead($tcs_interface_socket)));
  stopCommand($tcs_interface_socket);
  exit(-1);
} else {
  printTR("Send START to nexus", "ok");
}

printTR("Sleeping for 4 seconds","");
sleep(4);


# If we are using the DFB simulator, then the server_tcs_interface
# script will launch the dfb simulator and generate the correct
# UTC_START.
#
# If we are not using the DFB simulator (i.e. the DFB3 should be 
# sending packets then we need to make a BAT/UTC_START command. 
# For the moment, we shall generate a UTC_START command

$utc_start = "";

if ($sys_config["USE_DFB_SIMULATOR"] == 0) {

  # Use "now" as the UTC_START 
  $time_unix = time() - (10*60*60);
  $time_string = date(DADA_TIME_FORMAT,$time_unix);
  $result = $time_string; 

  # Issue Start command
  $cmd = "SET_UTC_START ".$result."\r\n";
  $utc_start = $result;

  socketWrite($tcs_interface_socket,$cmd);
  $result = rtrim(socketRead($tcs_interface_socket));

  if ($result != "ok") {
    $result .= "<BR>\n".rtrim(socketRead($tcs_interface_socket));
    printTR("SET_UTC_START failed: ", $result);
    printTR(rtrim(socketRead($tcs_interface_socket)),"");
    stopCommand($tcs_interface_socket);
    exit(-1);
  } else {
    printTR("Sent \"SET_UTC_START ".$utc_start."\" to nexus","ok");
  }

  $stop_time = addToDadaTime($utc_start,$duration);

  printTR("Running from ".$utc_start." => ".$stop_time." (".$duration." secs)", "");

}

# Now run for the duration of the observation
#$have_set_duration = 0;
for ($i=0;$i<$duration;$i++) {
  sleep(1);
  # Every 15 seconds, set the time limit of the script back to 30 seconds
  if ($i % 15 == 0) {
    set_time_limit(30);

    #if (!$have_set_duration) {
    #  $cmd = "DURATION ".$duration."\r\n";
    #  socketWrite($tcs_interface_socket,$cmd);
    #  $result = rtrim(socketRead($tcs_interface_socket));

    #  if ($result != "ok") {
    #    printTR($cmd, $result);
    #    printTR("STOPPING","");
    #    exit(-1);
    #  }
    #  printTR($cmd, $result);
    #  $have_set_duration = 1;
    #}
  }
  if ($i % 60 == 0) {
    printTR("Recording: ".(($duration - $i)/60)." minutes remaining","");
  }
}


# 10 extra seconds to ensure things have stopped!
# sleep(10);


stopCommand($tcs_interface_socket);

printTF();
printFooter();
flush();
sleep(2);
socket_close($tcs_interface_socket);
exit(0);

function printFooter() {

  echo "</body>\n";
  echo "</html>\n";
}

function printTR($tcs_simulator,$tcs_interface) {
  echo " <tr bgcolor=\"white\">\n";
  echo "  <td >".$tcs_simulator."</td>\n";
  echo "  <td align=\"left\">".$tcs_interface."</td>\n";
  echo " </tr>\n";
  echo '<script type="text/javascript">self.scrollBy(0,100);</script>';
  flush();
}

function printTF() {
  echo "</table>\n";
}


# Issue the STOP command
function stopCommand($socket) {

  $cmd = "STOP\r\n";
  socketWrite($socket,$cmd);

  $result = rtrim(socketRead($socket));
  if ($result != "ok") {
    printTR("\"$cmd\" failed",$result.": ".rtrim(socketRead($socket)));
  } else {
    printTR("Sent \"".$cmd."\" to nexus","ok");
  }
  return $result;
}

?>
