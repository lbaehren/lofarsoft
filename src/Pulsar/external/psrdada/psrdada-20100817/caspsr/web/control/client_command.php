<?PHP
include("../definitions_i.php");
include("../functions_i.php");
include("../".INSTRUMENT.".lib.php");
$inst = new $instrument();
$config = $inst->configFileToHash();

# Input parameters
$cmd = "";
$name = "";
$host = "";
$raw = 0;
$flush = 0;
$arg = "";
$servers_too = 0;

# Parse the GET parameters
if (isset($_GET["cmd"]))  { $cmd= $_GET["cmd"]; }
if (isset($_GET["name"]))  { $name = $_GET["name"]; }
if (isset($_GET["host"]))  { $host = $_GET["host"]; }
if (isset($_GET["arg"]))   { $arg = $_GET["arg"]; }
if (isset($_GET["raw"]))   { $raw = 1; }
if (isset($_GET["flush"])) { $flush = 1; }

if ($cmd == "get_status") { $servers_too = 1; }

# Do a little bit of error checking
if (($cmd == "start_daemon") && ($name == "")) {
  echo "Error: start_daemon command had no name specified<br>\n";
  exit(0);
}

# Some commands are read only
$readonly = array("get_disk_info","get_db_info", "get_alldb_info", 
                  "get_db_xfer_info", "get_load_info", "get_all_status");

# test to see if the request command is allowed
if ( (! IN_CONTROL) && (!(in_array($cmd, $readonly)))) {
  noControlMessage($cmd);
  exit(0);
}

$dests = getCommandDestinations($cmd, $name);

if (!$raw) {
  echo "<html>\n";
  $inst->print_head("CASPSR Client Controls");
}

# Get machines from the dada.cfg file
$machines = array();

$hosts = determineRunningHosts($config, $dests, $host);

if (count($hosts) == 0) {
  echo "ERROR: no valid hosts<BR>\n";
  $output = array();

} else {

  # build a string of the machines to run on
  $host_string = $hosts[0];
  for ($i=1;$i<count($hosts);$i++) {
    $host_string .= " ".$hosts[$i];
  }

  if (strlen($arg > 0)) {
    $cmd .= " ".$arg;
  }

  if (strlen($name) > 0) {
    $cmd .= " ".$name;
  }

  $script = "client_caspsr_command.pl ".$cmd." ".$host_string;
  $output = array();
  $output = runPerlScript($script);
}

# For raw output, just print the string
if ($raw) {

  for ($i=0; $i<count($output); $i++) {
    print $output[$i]."\n";
  }

} else {

?>
<body>

<script type="text/javascript">
  function finish(){
    parent.control.location.href=parent.control.location.href;
  }
</script>

<center>
<?
echo "<h3>".$cmd."<h3>\n";;
flush();

printResponse($hosts, $output);

if ((!$return_var) && ($flush)) {
  flush();
  //sleep(2);
?>
<script type="text/javascript">finish()</script>
<? } ?>
</center>
</body>
</html>
</center>
</body>
</html> 

<? }

function noControlMessage($cmd) {

  $cilent = strtolower(gethostbyaddr($_SERVER["REMOTE_ADDR"]));
  $controlling_host = strtolower(rtrim(file_get_contents(CONTROL_FILE)));

  echo "<html>\n";
  $inst->print_head("CASPSR Client Controls")
  ?>
  <br>
  <h3><font color="red">You cannot make any changes to the instrument if your host is not in control.</font></h3>

  <p>Controlling host: <?echo $controlling_hostname?>
     Your host: <?echo $hostname?></p>

  <!-- Force reload to prevent additional control attempts -->
  <script type="text/javascript">
  parent.control.location.href=parent.control.location.href;
  </script>

  </body>
  </html>
  <?
}

#
# Returns the type of destianations the command should be run on
#
function getCommandDestinations($cmd, $name) {

  $dests = array();

  # For starting and stopping a particular client daemon
  $dests["start_daemon"]         = array("pwc");
  $dests["stop_daemon"]          = array("pwc");
  $dests["init_db"]              = array("pwc", "help");
  $dests["destroy_db"]           = array("pwc", "help");
  $dests["start_pwcs"]           = array("pwc");
  $dests["stop_pwcs"]            = array("pwc");
  $dests["start_daemons"]        = array("pwc");
  $dests["start_helper_daemons"] = array("help");
  $dests["stop_helper_daemons"]  = array("help");
  $dests["stop_daemons"]         = array("pwc", "help");
  $dests["stop_dfbs"]            = array("dfb");
  $dests["get_disk_info"]        = array("pwc");
  $dests["get_db_info"]          = array("pwc", "help");
  $dests["get_alldb_info"]       = array("pwc");
  $dests["get_db_xfer_info"]     = array("pwc");
  $dests["get_load_info"]        = array("pwc");
  $dests["get_all_status"]       = array("pwc");
  $dests["default"]              = array("pwc");

  $result = array();
  if (!array_key_exists($cmd, $dests)) {
    echo "Error: unknown command $cmd<BR>\n";

  } else {

    $result = $dests[$cmd];

    # Special cases
    if (($cmd == "start_daemon") || ($cmd =="stop_daemon")) {
      if ($name == "caspsr_master_control") {
        array_push($result, "help", "dfb", "srv");
      } 
    }
  }

  return $result;

}

function determineRunningHosts($config, $dests, $host) {

  # If we have been asked to run a specific host, just do it
  if ($host != "") {
    return array($host);
  } 

  # Otherwise, work out which hosts we are to run on
  $pwcs = getConfigMachines($config, "PWC");
  $dfbs = getConfigMachines($config, "DFB");
  $help = getConfigMachines($config, "HELP");
  $srvs = array("srv0");

  $result = array();

  if (in_array("pwc", $dests)) {
    $result = array_merge($result, $pwcs);
  }

  if (in_array("dfb", $dests)) {
    $result = array_merge($result, $dfbs);
  }

  if (in_array("help", $dests)) {
    $result = array_merge($result, $help);
  }

  if (in_array("srv", $dests)) {
    $result = array_merge($result, $srvs);
  }

  $unique_result = array_unique($result);
  asort($unique_result);
  return ($unique_result);

} 

function printResponse($hosts, $output) {

  echo "<table width=90% cellpadding=0 cellspacing=0 class='datatable'>\n";
  echo "  <tr><th>Machine</th><th>Result</th><th>Response</th></tr>\n";

  $multiline = 0;
  for ($i=0; $i<count($output); $i++) {
    $array = split(":",$output[$i],3);
    $host     = $array[0];
    $result   = $array[1];
    $response = $array[2];

    # if we have newlines, need to ensure the text is handled properly
    if (!in_array($host, $hosts)) {
      echo "<BR>\n".$output[$i];
      $multiline = 1;

    } else {

      if ($multiline) {
        echo "</td></tr>\n";
      }

      if ($result == "fail") {
        $bg = " bgcolor='#ff4d4d'";
      } else {
        $bg = "";
      }
      echo "  <tr>\n";
      echo "  <td class='smalltext' align=center".$bg.">".$host."</td>\n";
      echo "  <td class='smalltext' align=center".$bg.">".$result."</td>\n";
      echo "  <td class='smalltext' align=left".$bg.">".$response;
      $multiline = 1;
    }
  }
  echo "</table>\n";
}
?>
