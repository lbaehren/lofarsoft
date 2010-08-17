<?PHP
include("functions_i.php");
include("definitions_i.php");
?>
<html>
<?
include("header_i.php");
?>
<body>
<?

  # Get machines from the dada.cfg file
  $config = getConfigFile(SYS_CONFIG);

  $nmachines = $config["NUM_PWC"];
  $machines = "";

  for($i=0; $i<$nmachines; $i++) {
    $machines .= $config["PWC_".$i]." ";
    $data[$config["PWC_".$i]] = array();
  }
                                                                                                              
  chdir($config["SCRIPTS_DIR"]);
  $script = "export DADA_ROOT=".DADA_ROOT."; ./client_command.pl \"get_disk_info\" ".$machines;
  $string = exec($script, $output, $return_var);

  for ($i=0;$i<count($output);$i++) {
    $array = split(":",$output[$i],3);
    $array2 = split(" ",$array[2],4);

    if ($array[1] == "ok") {
      $data[$array[0]]["DISK_TOTAL"] = $array2[0];
      $data[$array[0]]["DISK_USED"] = $array2[1];
      $data[$array[0]]["DISK_FREE"] = $array2[2];
      $data[$array[0]]["DISK_PERCENT"] = $array2[3];
    } else {
      $data[$array[0]]["DISK_ERROR"] = $array[2];
    }
  }

  $script = "export DADA_ROOT=".DADA_ROOT."; ./client_command.pl \"get_db_info\" ".$machines;
  $string = exec($script, $output, $return_var);

  for ($i=0;$i<count($output);$i++) {
    $array = split(":",$output[$i],3);
    $array2 = split(",",$array[2],10);
    if ($array[1] == "ok") {
      $data[$array[0]]["DB_TOTAL"] = $array2[0];
      $data[$array[0]]["DB_FILLED"] = $array2[1];
      $data[$array[0]]["DB_CLEARED"] = $array2[2];
      $data[$array[0]]["HB_TOTAL"] = $array2[5];
      $data[$array[0]]["HB_FILLED"] = $array2[6];
      $data[$array[0]]["HB_CLEARED"] = $array2[7];
    } else {
      $data[$array[0]]["DB_ERROR"] = "Data block not initialized";
    }
  }

  ?>
<table class="datatable">
  <tr><th colspan=14>APSR Client Status</th></tr>
  <tr><th></th><th colspan=4 style="text-align: center;">Recording Hard Disk Space</th><th colspan=3 style="text-align: center;">Header Block</th><th colspan=3  style="text-align: center;">DSPSR Data Block</th></tr>
  <tr><th>Machine</th><th>Total</th><th>Used</th><th>Free</th><th>Percent Full</th><th>Total</th><th>Full</th><th>Clear</th><th>Total</th><th>Full</th><th>Clear</th></tr>
  <?
  for ($i=0;$i<$nmachines;$i++) {
    $mach = $config["PWC_".$i];
    echo "  <tr bgcolor=\"white\">\n";
    echo "    <td>".$mach."</td>\n";
    if (isset($data[$mach]["DISK_ERROR"])) {
      echo "     <td colspan=4 bgcolor=\"red\">".$data[$mach]["DISK_ERROR"]."</td>\n";
    } else {
      echo "    <td>".$data[$mach]["DISK_TOTAL"]."</td>\n";
      echo "    <td>".$data[$mach]["DISK_USED"]."</td>\n";
      echo "    <td>".$data[$mach]["DISK_FREE"]."</td>\n";
      echo "    <td>".$data[$mach]["DISK_PERCENT"]."</td>\n";
    }
    if (isset($data[$mach]["DB_ERROR"])) {
      echo "    <td colspan=6 bgcolor=\"red\">".$data[$mach]["DB_ERROR"]."</td>\n";
    } else {
      echo "    <td>".$data[$mach]["HB_TOTAL"]."</td>\n";
      echo "    <td>".$data[$mach]["HB_FILLED"]."</td>\n";
      echo "    <td>".$data[$mach]["HB_CLEARED"]."</td>\n";
      echo "    <td>".$data[$mach]["DB_TOTAL"]."</td>\n";
      echo "    <td>".$data[$mach]["DB_FILLED"]."</td>\n";
      echo "    <td>".$data[$mach]["DB_CLEARED"]."</td>\n";
    }
    echo "  </tr>\n";
  }
  ?>
</table>
</body>
</html>
