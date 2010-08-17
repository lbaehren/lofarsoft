<?PHP

include("../definitions_i.php");
include("../functions_i.php");
include("../".INSTRUMENT.".lib.php");
$inst = new $instrument();

?>

<html>
<? 

$inst->print_head("CASPSR Controls"); 
$config = $inst->configFileToHash();

$server_daemons = split(" ",$config["SERVER_DAEMONS"]);
if (array_key_exists("SERVER_DAEMONS_PERSIST", $config)) {
  $server_daemons_persist = split(" ",$config["SERVER_DAEMONS_PERSIST"]);
} else {
  $server_daemons_persist = array();
}
$server_daemons_info = $inst->getServerLogInfo();
$client_daemons_info = $inst->getClientLogInfo();
$server_daemon_status = getServerStatus($config);

include("../banner_new.php");

?>
<body>
<script type="text/javascript"> 
  function popUp(URL) {
    parent.output.document.location = URL
  }
</script>

<table border=0 cellpadding=10>

<tr>

<!-- THE SERVER DAEMON CONTROLS -->
<td valign="top">

  <table border=0 class="datatable" cellpadding=0 cellspacing=0 marginwidth=0 marginheight=0>
  <tr><th colspan=3>Server Daemons</th></tr>
  <?
  for ($i=0; $i < count($server_daemons); $i++) {
    $d = $server_daemons[$i];
    printServerDaemonControl($d, $server_daemons_info[$d]["name"], $server_daemon_status[$d]);
  }
  ?>

  <? if (IN_CONTROL) { ?>
  <tr>
    <td align="center" colspan=3>
      <input type='button' value='Start All' onClick="popUp('server_command.php?cmd=start_daemons')">
      <input type='button' value='Stop All' onClick="popUp('server_command.php?cmd=stop_daemons')">
    </td>
  </tr>
  <? } ?>
  </table>

</td>


<!-- THE CLIENT DAEMON CONTROLS -->
<td valign="top">

  <table border=0 cellspacing=2  class="datatable">
  <?
  printClientDaemons($config, $client_daemons_info, IN_CONTROL);
  ?>
  </table>

</td>


<!-- THE HELPER DAEMON CONTROLS -->
<td valign="top">
  <table border=0 cellspacing=2  class="datatable">
  <?
  printClientHelpers($config, IN_CONTROL);
  ?>
  </table>

</td>
</tr>

<tr>
<td colspan=3>

<? if  (IN_CONTROL) { ?>
<table border=0 cellpadding=0>
<tr>

  <!-- PERSISTENT SERVER DAEMONS -->
  <? if (count($server_daemons_persist) > 0) { ?>
  <td valign=top>
    <table border=0 class="datatable" cellpadding=0 cellspacing=0 marginwidth=0 marginheight=0>
      <tr><th colspan=3>Persistent Server Daemons</th></tr>
      <?
      for ($i=0; $i < count($server_daemons_persist); $i++) {
        $d = $server_daemons_persist[$i];
        printServerDaemonControl($d, $server_daemons_info[$d]["name"], $server_daemon_status[$d]);
      }
      ?>
    </table>
  </td>
  
  <td width=10px>&nbsp;</td>
<? } ?>

  <!-- INSTRUMENT CONTROLS -->
  <td valign=top>

    <table border=0 cellpadding=0 class="datatable">
    <tr><th colspan=4>Instrument Controls</th></tr>
    <tr><td align=center>
      <input type='button' value='Restart Everything' onClick="popUp('server_command.php?cmd=restart_all')">
      <input type='button' value='Start CASPSR' onClick="popUp('server_command.php?cmd=start_instrument')">
      <input type='button' value='Stop CASPSR' onClick="popUp('server_command.php?cmd=stop_instrument')">
    </td></tr>
    </table>
  </td>

  <td width=10px>&nbsp;</td>

  <!-- CLIENT CONTROLS -->
  <td valign=top>

    <table border=0 cellpadding=0 class="datatable">
    <tr><th colspan=4>Client Controls</th></tr>
    <tr><td style="vertical-align: middle; text-align: right">Disk</td><td>
    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=get_disk_info')"  class="btn" > <span>Info</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_scratch')"  class="btn" > <span>Rm Scratch</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_logs')"  class="btn" > <span>Rm Logs</span> </a>
    </div>
  </td></tr>

  <tr><td style="vertical-align: middle; text-align: right">Other</td><td>

    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=get_load_info')"  class="btn" > <span>Load Info</span> </a>
      <a href="javascript:popUp('../client_status.php')"  class="btn" > <span>Client Status</span> </a>
    </div>

  </td></tr>
</table>

</td></tr>
</table>

<? } ?>

</body>
</html>

<?

function getServerStatus($config) {

  $daemons = split(" ",$config["SERVER_DAEMONS"]);
  if (array_key_exists("SERVER_DAEMONS_PERSIST", $config)) {
    $daemons_persist = split(" ",$config["SERVER_DAEMONS_PERSIST"]);
  } else {
    $daemons_persist = array();
  }
  $daemons = array_merge($daemons, $daemons_persist);

  $control_dir = $config["SERVER_CONTROL_DIR"];
  $results = array();

  for ($i=0; $i<count($daemons); $i++) {
    if (file_exists($control_dir."/".$daemons[$i].".pid")) {
      $results[$daemons[$i]] = 1;
    } else {
      $results[$daemons[$i]] = 0;
    }

    $perl_daemon = "server_".$daemons[$i].".pl";

    $cmd = "ps aux | grep perl | grep ".$perl_daemon." | grep -v grep > /dev/null";
    $lastline = system($cmd, $retval);
    if ($retval == 0) {
      $results[$daemons[$i]]++;
    }

  }
  return $results;
}


#
# prints a table of the client dameons and their status with control buttons
# 
function printClientDaemons($config, $d_names, $control) {

  $pwcs    = getConfigMachines($config, "PWC");
  $dbs     = split(" ",$config["DATA_BLOCKS"]);
  $daemons = split(" ",$config["CLIENT_DAEMONS"]);
  $d_info  = getDaemonInfo($pwcs, INSTRUMENT);
  # $d_names = getClientLogInformation();
  $db_info = getDBInfo($pwcs, "eada", INSTRUMENT);

  # Table headers
  echo "<tr><th></th>\n";
  echo "  <th colspan='".count($pwcs)."'>Clients</th>\n";
  echo "<th></th></tr>\n";

  # Host names
  echo "<tr><td></td>\n";
  for ($i=0; $i<count($pwcs); $i++) {
    echo "  <td>".substr($pwcs[$i],4,2)."</td>\n";
  }
  echo "<td></td></tr>\n";

  # Print the master control first
  $d = "caspsr_master_control";
  printClientDaemonControl($d, "Master Control", $pwcs, $d_info, "daemon&name=".$d);

  # Print the master control first
  printClientDBControl("Data Block", $pwcs, $db_info);

  # Print the PWC binary next
  printClientDaemonControl($config["PWC_BINARY"], "PWC", $pwcs, $d_info, "pwcs");

  # Print the client daemons
  for ($i=0; $i<count($daemons); $i++) {

    $d = $daemons[$i];
    $n = $d_names[$d]["name"];

    printClientDaemonControl($d, $n, $pwcs, $d_info, "daemon&name=".$d);

  }
}

#
# prints a table of the client helpers and their status with control buttons
# 
function printClientHelpers($config, $control) {

  # $help    = getConfigMachines($config, "HELP");
  $dfbs    = getConfigMachines($config, "DFB");
  $srvs    = array("srv0");
  $hosts   = array_merge($srvs, $dfbs);

  $d_info  = getDaemonInfo($hosts, INSTRUMENT);

  # Table headers
  echo "<tr><th></th>\n";
  echo "  <th colspan='".count($hosts)."'>Other</th>\n";
  echo "</tr>\n";

  # Host names
  echo "<tr><td></td>\n";
  for ($i=0; $i<count($hosts); $i++) {
    echo "  <td>".$hosts[$i]."</td>\n";
  }
  echo "</tr>\n";

  # Print the master control first
  $d = "caspsr_master_control";
  printClientDaemonControl($d, "Master Control", $hosts, $d_info, "");

}


function statusLight($value) {

  if ($value == 2) {
    return "<img src=\"/images/green_light.png\" width=\"15\" height=\"15\" border=\"none\">";
  } else if ($value == 1) {
    return "<img src=\"/images/yellow_light.png\" width=\"15\" height=\"15\" border=\"none\">";
  } else {
    return "<img src=\"/images/red_light.png\" width=\"15\" height=\"15\" border=\"none\">";
  }
}

function printServerDaemonControl($daemon, $name, $status) {

  echo "  <tr>\n";
  echo "    <td style='vertical-align: middle'>".$name."</td>\n";
  echo "    <td style='vertical-align: middle'>".statusLight($status)."</td>\n";
  echo "    <td>\n";
  echo "      <input type='button' value='Start' onClick=\"popUp('server_command.php?cmd=start_daemon&name=".$daemon."')\">\n";
  echo "      <input type='button' value='Stop' onClick=\"popUp('server_command.php?cmd=stop_daemon&name=".$daemon."')\">\n";
  echo "    </td>\n";
  echo "  </tr>\n";

}


#
# Print a row of client daemons
#
function printClientDaemonControl($daemon, $name, $hosts, $statuses, $cmd) {

  echo "  <tr>\n";
  echo "    <td style='vertical-align: middle'>".$name."</td>\n";
  for ($i=0; $i<count($hosts); $i++) {
    $s = $statuses[$hosts[$i]][$daemon];
    echo "    <td style='vertical-align: middle'>".statusLight($s)."</td>\n";
  }
  if ($cmd != "" ) {
    echo "    <td style='text-align: middle;'>\n";
    echo "      <input type='button' value='Start' onClick=\"popUp('client_command.php?cmd=start_".$cmd."&flush=1')\">\n";
    echo "      <input type='button' value='Stop' onClick=\"popUp('client_command.php?cmd=stop_".$cmd."&flush=1')\">\n";
    echo "    </td>\n";
  }
  echo "  </tr>\n";

}

#
# Print the data block row
#
function printClientDBControl($name, $hosts, $statuses) {

  echo "  <tr>\n";
  echo "    <td style='vertical-align: middle'>".$name."</td>\n";
  for ($i=0; $i<count($hosts); $i++) {
    $s = $statuses[$hosts[$i]];
    echo "    <td style='vertical-align: middle'>".statusLight($s)."</td>\n";
  }
  echo "    <td style='text-align: middle;'>\n";
  echo "      <input type='button' value='Init' onClick=\"popUp('client_command.php?cmd=init_db&flush=1')\">\n";
  echo "      <input type='button' value='Destroy' onClick=\"popUp('client_command.php?cmd=destroy_db&flush=1')\">\n";
  echo "    </td>\n";
  echo "  </tr>\n";

}

