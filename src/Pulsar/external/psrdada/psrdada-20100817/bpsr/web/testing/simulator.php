<?PHP
include("../definitions_i.php");
include("../functions_i.php");
?>

<html> 

<?

$title = "BPSR | TCS Simulator";
include("../header_i.php");

if (!IN_CONTROL) { ?>
<h3><font color=red>BPSR TCS Simulator disabled. Your host is not in control of the instrument</font></h3>
</body>
</html>
<?
  exit(0);
} ?>


<body>
<table width=90% border=0 cellpadding=0 cellspacing=0 class="datatable">
 <tr>
  <th width="50%">TCS Simulator</th>
  <th width="50%">TCS Interface</th>
 </tr>

<?

$sys_config = getConfigFile(SYS_CONFIG);

$ibob_config_file = "";
$spec_file = "";
$duration = 30;
$cmd = "";

if (isset($_GET["cfg"])) {
  $ibob_config_file = $_GET["cfg"];
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

if (isset($_GET["cmd"])) {
  $cmd = $_GET["cmd"];
} else {
  printTR("Error: cmd not specified in _GET parameters","");
  printTF();
  printFooter();
  exit(-1);
}

if ($cmd == "start") {

  if (!(file_exists($sys_config["CONFIG_DIR"]."/".$ibob_config_file))) {
    printTR("Error: ibob Configuration File \"".$_GET["cfg"]."\" did not exist","");
    printTF();
    printFooter();
    exit(-1);
  }

  if (!(file_exists($spec_file))) {
    printTR("Error: Specification File \"".$_GET["spec"]."\" did not exist","");
    printTF();
    printFooter();
    exit(-1);
  }

  $specification = getRawTextFile($spec_file);
  $spec = getConfigFile($spec_file);

} else if ($cmd == "stop")  {


} else {

  printTR("Error: Unrecognised cmd in _GET parameters: ".$_GET["cmd"],"");
  printTF();
  printFooter();
  exit(-1);

}

/* Open a connection to the TCS interface script */
$host = $sys_config["TCS_INTERFACE_HOST"];
$port = $sys_config["TCS_INTERFACE_PORT"];
$tcs_interface_socket = 0;

list ($tcs_interface_socket,$message) = openSocket($host,$port,2);
if (!($tcs_interface_socket)) {
  printTR("Error: opening socket to TCS interface script",$message);
  printTF();
  printFooter();
  exit(-1);
} 

if ($cmd == "start") {

  $utc_start = tcs_start($tcs_interface_socket, $ibob_config_file, $specification);

  if ($duration > 0) {
    tcs_wait($duration);
    tcs_stop($tcs_interface_socket);
  }

  tcs_close($tcs_interface_socket);

} else {

  tcs_stop($tcs_interface_socket);
  tcs_close($tcs_interface_socket);

}

exit(0);


/* ****************************************************************************
 *
 * Functions
 *
 */


/* 
 * Starts the TCS interface script
 */
function tcs_start($sock, $ibob_cfg, $spec) {

  # Send the ibob Configuration File to the tcs_interface socket
  $cmd = "CONFIG ".$ibob_cfg."\r\n";
  socketWrite($sock,$cmd);
  $result = rtrim(socketRead($sock));
  printTR($cmd,$result);
  if ($result != "ok") {
    exit(-1);
  }

  # Send each header parameter to TCS interface
  for ($i=0;$i<count($spec);$i++) {
    $cmd = $spec[$i]."\r\n";
    socketWrite($sock,$cmd);
    $result = rtrim(socketRead($sock));
    printTR($cmd,$result);
    if ($result != "ok") {
      exit(-1);
    }
  }

  # Issue START command to server_tcs_interface 
  $cmd = "START\r\n";
  socketWrite($sock,$cmd);
  $result = rtrim(socketRead($sock));
  if ($result != "ok") {
    printTR("START command failed on nexus ", $result);
    printTR(rtrim(socketRead($sock)),"");
    exit(-1);
  } else {
    printTR("Send START to nexus", "ok");
  }

  printTR("Waiting for start_utc response","");
  sleep(5);
                                                                                                                                                                                                                      
  $start_utc_string = rtrim(socketRead($sock));
  printTR("",$start_utc_string);
  sscanf($start_utc_string, "start_utc %s", $utc_string);

  return $utc_string;

}


/* 
 * Waits for the specified time
 */
function tcs_wait($duration) {

  # Now run for the duration of the observation
  for ($i=0;$i<$duration;$i++) {

    sleep(1);
    # Every 15 seconds, set the time limit of the script back to 30 seconds
    if ($i % 15 == 0) {
      set_time_limit(30);
    }
    if ($i % 60 == 0) {
      printTR("Recording: ".(($duration - $i)/60)." minutes remaining","");
    }
  }

}

function tcs_stop($sock) {

  # Issue the STOP command 
  $cmd = "STOP\r\n";
  socketWrite($sock,$cmd);
  $result = rtrim(socketRead($sock));

  if ($result != "ok") {
    printTR("\"$cmd\" failed",$result);
    printTR("",rtrim(socketRead($sock)));
    exit(-1);
  } else {
    printTR("Sent \"".$cmd."\" to nexus","ok");
  }
}

function tcs_close($sock) {

  printTF();
  printFooter();
  flush();
  sleep(2);
  socket_close($sock);

}


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

?>
