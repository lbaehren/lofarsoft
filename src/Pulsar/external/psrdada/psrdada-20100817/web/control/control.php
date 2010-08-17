<?PHP

include("../definitions_i.php");
include("../functions_i.php");
$instrument = strtoupper(INSTRUMENT);
include("../".INSTRUMENT."_functions_i.php");
?>
<html>
<?
include("../header_i.php");
$text = $instrument." Controls";
include("../banner.php");

$config = getConfigFile(SYS_CONFIG);
$server_daemons = split(" ",$config["SERVER_DAEMONS"]);
if (array_key_exists("SERVER_DAEMONS_PERSIST", $config)) {
  $server_daemons_persist = split(" ",$config["SERVER_DAEMONS_PERSIST"]);
} else {
  $server_daemons_persist = array();
}
$server_daemons_info = getServerLogInformation();
$server_daemon_status = getServerStatus($config);

# determine the PIDS that this instrument has configured from the unix groups
$cmd = "groups ".INSTRUMENT;
$output = array();
$string = exec($cmd, $output, $return_var);
$array = split(" ",$string);
$groups = array();
for ($i=0; $i<count($array); $i++) {
  if (strpos($array[$i], "P") === 0) {
    array_push($groups, $array[$i]);
  }
}

?>
<body>
<SCRIPT LANGUAGE="JavaScript">
function popUp(URL) {
  parent.output.document.location = URL
}
function popUpPID(URL) {
  var pid_index = document.getElementById("pid").selectedIndex;
  var pid = document.getElementById("pid").options[pid_index].value;
  parent.output.document.location = URL+"&pid="+pid
}
</script>

<table border=0 cellpadding=10>
<tr><td valign="top">

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
      <div class="btns" style="text-align: middle">
        <a href="javascript:popUp('server_command.php?cmd=start_daemons')"  class="btn" > <span>Start All</span> </a>
        <a href="javascript:popUp('server_command.php?cmd=stop_daemons')"  class="btn" > <span>Stop All</span> </a>
      </div>
    </td>
  </tr>
<? } ?>
  </table>

</td><td valign="top">

<table border=0 cellspacing=2  class="datatable">
  <?
  printClientStatus($config, IN_CONTROL);
  ?>
</table>
<br>

<? if  (IN_CONTROL) { ?>
<table border=0 cellpadding=0>

<tr>

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
  <tr> 
    <td style="vertical-align: middle">Project ID</td>
    <td align=left colspan=2><select name="pid" id="pid">
<?
for ($i=0; $i<count($groups); $i++) {
  echo "      <option value=".$groups[$i].">".$groups[$i]."</option>\n";
}
?>
    </select></td>
  </tr>
  </table>

</td><td width=10px>&nbsp;</td>
<? } ?>

<td valign=top>

<table border=0 cellpadding=0 class="datatable">
<tr><th colspan=4>Instrument Controls</th></tr>
</table>
                                                                                                                                    
<table border=0 cellpadding=2>
  <tr><td align=center>
    <div class="btns">
      <a href="javascript:popUp('server_command.php?cmd=restart_all')"  class="btn" > <span>Restart Everything</span> </a>
    </div>
    <div class="btns">
      <a href="javascript:popUp('server_command.php?cmd=start_instrument')"  class="btn" > <span>Start <?echo $instrument?></span> </a>
      <a href="javascript:popUp('server_command.php?cmd=stop_instrument')"  class="btn" > <span>Stop <?echo $instrument?></span> </a>
    </div>
  </td></tr>
</table>

</td><td width=10px>&nbsp;</td><td valign=top>

<table border=0 cellpadding=0 class="datatable">
<tr><th colspan=4>Client Controls</th></tr>
</table>  

<table border=0 cellpadding=2>
  <tr><td style="vertical-align: middle; text-align: right">DB</td><td>
    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=reset_db')"  class="btn" > <span>Reset</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=init_db')"  class="btn" > <span>Init</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=destroy_db')"  class="btn" > <span>Destroy</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=get_db_info')"  class="btn" > <span>Info</span> </a>
    </div>
  </td></tr>

  <tr><td style="vertical-align: middle; text-align: right">Disk</td><td>
    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=get_disk_info')"  class="btn" > <span>Info</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_scratch')"  class="btn" > <span>Rm Scratch</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_logs')"  class="btn" > <span>Rm Logs</span> </a>
      <? if (INSTRUMENT == "apsr") { ?>
      <a href="javascript:popUp('client_command.php?cmd=clean_archives')"  class="btn" > <span>Rm Archives</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_rawdata')"  class="btn" > <span>Rm Rawdata</span> </a>
      <? } ?>
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
function printClientDaemons($config, $control) {

  $pwcs    = getConfigMachines($config, "PWC");
  $dbs     = split(" ",$config["DATA_BLOCKS"]);
  $daemons = split(" ",$config["CLIENT_DAEMONS"]);
  $d_info  = getDaemonInfo($pwcs, INSTRUMENT);
  $d_names = getClientLogInformation();

  # Table headers
  echo "<tr><th></th>\n";
  echo "  <th colspan='".count($pwcs)."'>Clients [apsr##]</th>\n";
  echo "<th colspan='2'></th></tr>\n";

  # Host names
  echo "<tr><td></td>\n";
  for ($i=0; $i<count($pwcs); $i++) {
    echo "  <td>".substr($pwcs[$i],4,2)."</td>\n";
  }
  echo "<td colspan='2'></td></tr>\n";

  # Print the master control first
  echo "<tr><td>Master Control</td>\n";
  for ($i=0; $i<count($pwcs); $i++) {
    echo "<td>".statusLight($d_info[$pwcs[$i]][INSTRUMENT."_master_control"])."</td>";
  }
  ?>
  <td width="130px"><div class="btns">
    <a href="javascript:popUp('client_command.php?cmd=start_master_script&autoclose=1')"  class="btn" > <span>Start</span> </a>
    <a href="javascript:popUp('client_command.php?cmd=stop_master_script&autoclose=1')"  class="btn" > <span>Stop</span> </a>
  </div></td>
  <td></td>
  <?

  # Print the PWC binary next
  echo "<tr>\n";
  echo "  <td>".$config["PWC_BINARY"]."</td>\n";
  for ($j=0; $j<$config["NUM_PWC"]; $j++) {
    echo "  <td>".statusLight($results[$pwcs[$i]][$config["PWC_BINARY"]])."</td>\n";
  }
  ?>
  <td width="130px"><div class="btns"> 
    <a href="javascript:popUp('client_command.php?cmd=start_master_script&autoclose=1')"  class="btn" > <span>Start</span> </a>
    <a href="javascript:popUp('client_command.php?cmd=stop_master_script&autoclose=1')"  class="btn" > <span>Stop</span> </a>
  </div></td>
  <td></td>
  <?

  for ($i=0; $i<count($daemons); $i++) {
    $d = $daemons[$i];

    echo "<tr>\n";
      echo "  <td>".$d_names[$d]["name"]."</td>\n";
      for ($j=0; $j<$pwcs; $j++) {
        echo "  <td>".statusLight($results[$pwcs[$j]][$d])."</td>\n";
      }
      echo "<td><div class='btns'>\n";
      echo "<a href=\"javascript:popUp('client_command.php?cmd=start_master_script&autoclose=1')\" class='btn; ><span>Start</span></a>\n";


      if (($i==0) && ($control)) {
?>
        <td rowspan="<?echo (count($cds)-1)?>" style='vertical-align: middle;'>
          <div class="btns">
            <a href="javascript:popUp('client_command.php?cmd=start_daemons&autoclose=1')"  class="btn" > <span>Start</span> </a>
            <a href="javascript:popUp('client_command.php?cmd=stop_daemons&autoclose=1')"  class="btn" > <span>Stop</span> </a>
          </div>
        </td>
<?
      }

    echo "</tr>\n";

  }




  




  



}

function printClientStatus($config, $control) {

  $machines = "";
  $helpers = "";
  $cds = split(" ",$config["CLIENT_DAEMONS"]);
  array_push($cds, $config["PWC_BINARY"]);
  $servers = array("srv0", "srv1");

  $client_daemons = getClientLogInformation();
  $client_daemons[INSTRUMENT."_master_control"] = array("name" => "Master Control");
  $client_daemons[$config["PWC_BINARY"]] = array("name" => "PWC (".$config["PWC_BINARY"].")");

  $results = array();

  echo "<tr>\n";
  echo "  <th colspan=\"".($config["NUM_PWC"]+1)."\">Client Daemons</th>\n";
  echo "  <th colspan=\"2\">Servers</th>\n";
  echo "  <th colspan=\"".$config["NUM_HELP"]."\">Helpers</th>\n";
  echo "</tr>\n";

  # Print the numeric Row
  echo "<tr><td align=right></td>";
  for ($i=0; $i<$config["NUM_PWC"]; $i++) {

    $m = $config["PWC_".$i];
    $results[$m] = array();
    for ($j=0; $j<count($cds); $j++) {
      $results[$m][$cds[$j]] = 0;
    }
    $machines .= $m." ";
    echo "<td align=center>".sprintf("%2d",$i)."</td>";
  }

  for ($i=0; $i<count($servers); $i++) {
    $m = $servers[$i];
    $results[$m] = array();
    for ($j=0; $j<count($cds); $j++) {
      $results[$m][$cds[$j]] = 0;
    }
    $machines .= $m." ";
    echo "<td align=center>".sprintf("%2d",$i)."</td>";
  }

  for ($i=0; $i<$config["NUM_HELP"]; $i++) {

    $m = $config["HELP_".$i];
    $results[$m] = array();
    for ($j=0; $j<count($cds); $j++) {
      $results[$m][$cds[$j]] = 0;
    }
    $machines .= $m." ";
    echo "<td align=center>".sprintf("%2d",$i)."</td>";

  }
  echo "</tr>";

  # Get the status of all the daemons
  chdir($config["SCRIPTS_DIR"]);
  $script = "source /home/dada/.bashrc; ./client_".INSTRUMENT."_command.pl \"daemon_info\" ".$machines;
  $string = exec($script, $output, $return_var);

  if ($return_var == 0) {

    for($i=0; $i<count($output); $i++) {

      $array = split(":",$output[$i],3);
      $machine = $array[0]; 
      $result = $array[1];
      $string = $array[2];

      if ( (strpos($string, "Could not connect to machine")) !== FALSE) {
	      # We could not contact the master control script
      } else {

        $results[$machine][INSTRUMENT."_master_control"] = 2;
        $daemon_results = split(",",$string);

        for ($j=0; $j<count($daemon_results); $j++) {
          if (strlen($daemon_results[$j]) > 2) {
            $arr = split(" ",$daemon_results[$j]);
            $results[$machine][$arr[0]] = $arr[1];
          }
        }
      }
    }
  }

  # Print the Master Control Script First
  echo "<tr>\n";
  echo "  <td align=right style='vertical-align: middle;'>".$client_daemons[INSTRUMENT."_master_control"]["name"]."</td>\n";
  for ($j=0; $j<$config["NUM_PWC"]; $j++) {
    echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$config["PWC_".$j]][INSTRUMENT."_master_control"])."</td>\n";
  }
  for ($j=0; $j<count($servers); $j++) {
    echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$servers[$j]][INSTRUMENT."_master_control"])."</td>\n";
  }
  for ($j=0; $j<$config["NUM_HELP"]; $j++) {
    echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$config["HELP_".$j]][INSTRUMENT."_master_control"])."</td>\n";
  }
  if ($control) {
    ?>
    <td width="130px"><div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=start_master_script&autoclose=1')"  class="btn" > <span>Start</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=stop_master_script&autoclose=1')"  class="btn" > <span>Stop</span> </a>
    </div></td>
    <?
  }
  echo "</tr>\n";
                                                                                                                    

  # Print the PWC binary next
  echo "<tr>\n";
  echo "  <td align=right style='vertical-align: middle;'>".$client_daemons[$config["PWC_BINARY"]]["name"]."</td>\n";
  for ($j=0; $j<$config["NUM_PWC"]; $j++) {
    echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$config["PWC_".$j]][$config["PWC_BINARY"]])."</td>\n";
  }
  for ($j=0; $j<count($servers); $j++) {
    echo "  <td></td>\n";
  }
  for ($j=0; $j<$config["NUM_HELP"]; $j++) {
    echo "  <td></td>\n";
  }
  if ($control) {
    ?>
    <td><div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=start_pwcs&autoclose=1')"  class="btn" > <span>Start</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=stop_pwcs&autoclose=1')"  class="btn" > <span>Stop</span> </a>
    </div></td>
    <?
  }
  echo "</tr>\n";
                                                                                                                    

  # Print all other daemons

  for ($i=0; $i<count($cds)-1; $i++) {

    echo "<tr>\n";
      echo "  <td align=right>".$client_daemons[$cds[$i]]["name"]."</td>\n";
      for ($j=0; $j<$config["NUM_PWC"]; $j++) {
        echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$config["PWC_".$j]][$cds[$i]])."</td>\n";
      }
      for ($j=0; $j<count($servers); $j++) {
        echo "  <td></td>\n";
      }
      for ($j=0; $j<$config["NUM_HELP"]; $j++) {
        echo "  <td></td>\n";
      }
      if (($i==0) && ($control)) {
?>
        <td rowspan="<?echo (count($cds)-1)?>" style='vertical-align: middle;'>
          <div class="btns">
            <a href="javascript:popUp('client_command.php?cmd=start_daemons&autoclose=1')"  class="btn" > <span>Start</span> </a>
            <a href="javascript:popUp('client_command.php?cmd=stop_daemons&autoclose=1')"  class="btn" > <span>Stop</span> </a>
          </div>
        </td>
<?
      }
      
    echo "</tr>\n";

  }

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
  echo "    <td style=\"vertical-align: middle\">".$name."</td>\n";
  echo "    <td style=\"vertical-align: middle\">".statusLight($status)."</td>\n";
  echo "    <td width=130px>\n";
  echo "      <div class=\"btns\">\n";
  if ( (strpos($daemon, "_tape_") !== FALSE) || (strpos($daemon,"bpsr_transfer_manager") !== FALSE) ){
    echo "        <a href=\"javascript:popUpPID('server_command.php?cmd=start_daemon&name=".$daemon."')\"  class=\"btn\" > <span>Start</span> </a>\n";
    echo "        <a href=\"javascript:popUpPID('server_command.php?cmd=stop_daemon&name=".$daemon."')\"  class=\"btn\" > <span>Stop</span> </a>\n";
  } else {
    echo "        <a href=\"javascript:popUp('server_command.php?cmd=start_daemon&name=".$daemon."')\"  class=\"btn\" > <span>Start</span> </a>\n";
    echo "        <a href=\"javascript:popUp('server_command.php?cmd=stop_daemon&name=".$daemon."')\"  class=\"btn\" > <span>Stop</span> </a>\n";
  }
  echo "      </div>\n";
  echo "    </td>\n";
  echo "  </tr>\n";

}
