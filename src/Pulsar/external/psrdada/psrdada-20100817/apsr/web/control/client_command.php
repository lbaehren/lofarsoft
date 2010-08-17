<?PHP
include("../definitions_i.php");
include("../functions_i.php");

$cmd = $_GET["cmd"];
$readonly_commands = array("get_disk_info","get_db_info", "get_alldb_info", "get_db_xfer_info", "get_load_info", "get_all_status");
if (in_array($cmd, $readonly_commands))

if ((!IN_CONTROL) && (!(in_array($cmd, $readonly_commands))) ) {

  $hostname = strtolower(gethostbyaddr($_SERVER["REMOTE_ADDR"]));
  $controlling_hostname = strtolower(rtrim(file_get_contents(CONTROL_FILE)));

  echo "<html>\n";
  include("../header_i.php");
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
  exit(0);
} 

$command_dests = array();

$command_dests["init_db"]              = array("pwc", "help");
$command_dests["destroy_db"]           = array("pwc", "help");

$command_dests["start_master_script"]  = array("pwc", "help", "dfb");
$command_dests["stop_master_script"]   = array("pwc", "help", "dfb");

$command_dests["start_pwcs"]           = array("pwc");
$command_dests["stop_pwcs"]            = array("pwc");

$command_dests["start_daemons"]        = array("pwc");
$command_dests["start_helper_daemons"] = array("help");
$command_dests["stop_daemons"]         = array("pwc", "help");

$command_dests["stop_dfbs"]            = array("dfb");
 
$command_dests["clean_scratch"]        = array("pwc", "help");
$command_dests["clean_archives"]       = array("pwc", "help");
$command_dests["clean_rawdata"]        = array("pwc", "help");
$command_dests["clean_logs"]           = array("pwc", "help");
$command_dests["get_disk_info"]        = array("pwc");
$command_dests["get_db_info"]          = array("pwc", "help");
$command_dests["get_alldb_info"]       = array("pwc");
$command_dests["get_db_xfer_info"]     = array("pwc");
$command_dests["get_load_info"]        = array("pwc");
$command_dests["get_all_status"]       = array("pwc");

$command_dests["default"]              = array("pwc");

$raw = 0;
if (isset($_GET["raw"])) {
  $raw = 1;
}

if (!$raw) {
  ?>
  <html>
  <?
  include("../header_i.php");
}

# Get machines from the dada.cfg file
$sys_config = getConfigFile(SYS_CONFIG);
$machines = array();
$cmd = $_GET["cmd"];

if (isset($_GET["singlemachine"])) {

  $machines = array($_GET["singlemachine"]);

} else {

  # select machines based on command_dests array
  if (array_key_exists($cmd, $command_dests))
    $dest_machines = $command_dests[$cmd];
  else
    $dest_machines = $command_dests["default"];

  if (in_array("pwc",$dest_machines)) {
    for ($i=0; $i<$sys_config["NUM_PWC"]; $i++) {
      $machine = $sys_config["PWC_".$i];
      if (! in_array($machine,$machines)) {
        array_push($machines,$machine);
      }
    }
  }

  if (in_array("dfb", $dest_machines)) {
    $machine = $sys_config["DFB_0"];
    if (! in_array($machine,$machines)) {
      array_push($machines,$machine);
    }
  }

  if (in_array("help", $dest_machines)) {
    for ($i=0; $i<$sys_config["NUM_HELP"]; $i++) {
      $machine = $sys_config["HELP_".$i];
      if (! in_array($machine,$machines)) {
        array_push($machines,$machine);
      }
    }
  }

}   

if (isset($_GET["arg"])) {
  $cmd .= " ".$_GET["arg"];
}

if (isset($_GET["autoclose"])) {
  $flush = 1;
} else {
  $flush = 0;
}


$all_machines = "";
for ($i=0;$i<count($machines);$i++) { 
  $all_machines .= $machines[$i]." ";
}

chdir($sys_config["SCRIPTS_DIR"]);

$script = "export DADA_ROOT=".DADA_ROOT."; ./client_apsr_command.pl \"".$cmd."\" ".$all_machines." 2>&1";

$string = exec($script, $output, $return_var);

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
    //window.opener.location.href=window.opener.location.href;
    //window.opener.focus();
    //window.close();
  }
</script>

<center>
<?
echo "<h3>".$cmd."<h3>\n";;
flush();
?>

<table width=90% cellpadding=0 cellspacing=0 class="datatable">
 <tr><th>Machine</th><th>Result</th><th>Response</th></tr>
<?
$newmachine = "init";
for($i=0; $i<count($output); $i++) {
  $array = split(":",$output[$i],3);

  if ($array[0] && (strpos($all_machines, $array[0])) !== FALSE) {
    if ($newmachine == "false") {
      echo "</td></tr>\n";
    }
    $newmachine = "true";
  }

  if ($newmachine == "true") {
    if ($array[1] == "fail") {
      $return_var = 1;
      $bg = " bgcolor=\"#ff4d4d\"";
    } else { 
      $bg = "";
    }
    echo " <tr style=\"background: white;\">\n";
    echo "  <td class=\"smalltext\" align=center".$bg.">".$array[0]."</td>\n";
    echo "  <td class=\"smalltext\" align=center".$bg.">".$array[1]."</td>\n";
    echo "  <td class=\"smalltext\" align=left".$bg.">".$array[2];
    $newmachine = "false";
  } else {
    echo "<BR>\n".$output[$i];
  }
}

?>
</table>
<?
if ((!$return_var) && ($flush)) {
  flush();
  sleep(2);
?>
<script type="text/javascript">finish()</script>
<? } ?>
</center>
</body>
</html>
</center>
</body>
</html> 

<? } ?>
