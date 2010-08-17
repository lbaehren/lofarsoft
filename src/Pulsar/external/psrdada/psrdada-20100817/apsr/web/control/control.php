<?PHP

include("../definitions_i.php");
include("../functions_i.php");
include("../apsr_functions_i.php");
?>
<html>
<?
include("../header_i.php");
$text = "APSR Controls";
include("../banner.php");

$config = getConfigFile(SYS_CONFIG);
$server_daemons = split(" ",$config["SERVER_DAEMONS"]);
$server_daemons_info = getServerLogInformation();
$server_daemon_status = getServerStatus($config);

?>
<body>
<SCRIPT LANGUAGE="JavaScript">
function popUp(URL) {
  parent.output.document.location = URL
}
</script>

<table border=0 cellpadding=20>
<tr><td valign="top">

  <table border=0 class="datatable">
  <tr><th colspan=2>Server Daemons</th></tr>
<?

  for ($i=0; $i < count($server_daemons); $i++) {
    $d = $server_daemons[$i];
    echo "  <tr> <td>".$server_daemons_info[$d]["name"]."</td><td>".statusLight($server_daemon_status[$d])."</td></tr>\n";
  }


?>

<? if (IN_CONTROL) { ?>
  <tr>
    <td align="center" colspan=2> 
      <div class="btns">
        <a href="javascript:popUp('server_command.php?cmd=start_daemons')"  class="btn" > <span>Start</span> </a>
        <a href="javascript:popUp('server_command.php?cmd=stop_daemons')"  class="btn" > <span>Stop</span> </a>
      </div>
    </td>
  </tr>
<? } ?>
  </table>

</td><td valign="top">

<table border=0 cellspacing=2  class="datatable">
<tr>
  <th colspan=<?echo ($config{"NUM_PWC"}+2)?>>Client Daemons</th>
</tr>

<?
printClientStatus($config, IN_CONTROL);
?>
</table>

</td></tr>
</table>

<? if  (IN_CONTROL) { ?>
<table border=0 cellpadding=10>

<tr><td>

<table border=0 cellpadding=0 class="datatable">
<tr><th colspan=4>Admins Only</th></tr>
</table>
                                                                                                                                    
<table border=0 cellpadding=2>
  <tr><td>
    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=stop_pwc&arg=dspsr')"  class="btn" > <span>Kill Dspsr</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=stop_dfbs')"  class="btn" > <span>Stop DFB Sim</span> </a>
    </div>
    <div class="btns">
      <a href="javascript:popUp('server_command.php?cmd=reset_pwcc')"  class="btn" > <span>Reset PWCs</span> </a>
      <a href="javascript:popUp('server_command.php?cmd=restart_all')"  class="btn" > <span>Restart Everything</span> </a>
      
  </td></tr>
</table>
                                                                                                                                    
</td><td>

<table border=0 cellpadding=0 class="datatable">
<tr><th colspan=4>Client Controls</th></tr>
</table>  

<table border=0 cellpadding=2>
  <tr><td style="vertical-align: middle; text-align: right">Data Block</td><td>
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
      <a href="javascript:popUp('client_command.php?cmd=clean_archives')"  class="btn" > <span>Rm Archives</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_logs')"  class="btn" > <span>Rm Logs</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=clean_rawdata')"  class="btn" > <span>Rm Rawdata</span> </a>
    </div>
  </td></tr>

  <tr><td style="vertical-align: middle; text-align: right">Other</td><td>

    <div class="btns">
      <a href="javascript:popUp('client_command.php?cmd=get_load_info')"  class="btn" > <span>Load Info</span> </a>
      <a href="javascript:popUp('../client_status.php')"  class="btn" > <span>Client Status</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=get_bin_dir')"  class="btn" > <span>Get Bin Dir</span> </a>
      <a href="javascript:popUp('client_command.php?cmd=daemon_info')"  class="btn" > <span>Daemon Info</span> </a>
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
  $control_dir = $config["SERVER_CONTROL_DIR"];
  $results = array();

  for ($i=0; $i<count($daemons); $i++) {
    if (file_exists($control_dir."/".$daemons[$i].".pid")) {
      $results[$daemons[$i]] = 1;
    } else {
      $results[$daemons[$i]] = 0;
    }

    $perl_daemon = "server_".$daemons[$i].".pl";

    $cmd = "ps aux | grep ".$perl_daemon." | grep -v grep > /dev/null";
    $lastline = system($cmd, $retval);
    if ($retval == 0) {
      $results[$daemons[$i]]++;
    }

  }
  return $results;
}

function printClientStatus($config, $control) {

  $machines = "";
  $helpers = "";
  $cds = split(" ",$config["CLIENT_DAEMONS"]);
  array_push($cds, $config["PWC_BINARY"]);

  $client_daemons = getClientLogInformation();
  $client_daemons["apsr_master_control"] = array("name" => "Master Control");
  $client_daemons[$config["PWC_BINARY"]] = array("name" => "PWC (".$config["PWC_BINARY"].")");

  $results = array();

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
  echo "</tr>";

  # Get the status of all the daemons
  chdir($config["SCRIPTS_DIR"]);
  $script = "source /home/dada/.bashrc; ./client_apsr_command.pl \"daemon_info\" ".$machines;
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

        $results[$machine]["apsr_master_control"] = 2;
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
  echo "  <td align=right style='vertical-align: middle;'>".$client_daemons["apsr_master_control"]["name"]."</td>\n";
  for ($j=0; $j<$config["NUM_PWC"]; $j++) {
    echo "  <td width=17 style='vertical-align: middle;'>".statusLight($results[$config["PWC_".$j]]["apsr_master_control"])."</td>\n";
  }
  if ($control) {
    ?>
    <td><div class="btns">
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
