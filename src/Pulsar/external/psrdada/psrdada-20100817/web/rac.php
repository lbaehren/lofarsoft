<?PHP
  $data = array();

?>
<html>
  <head>
    <title>PSRDADA : RAC Information and Links</title>
    <link rel="STYLESHEET" type="text/css" href="/style.css">
</head>
<body bgcolor="#EEEEEE">
<center>
<span id="gatherer">
<?
echo "Retrieving version information from the RAC's<BR>\n";
flush();
$nodes = array("srv0", "srv1", "apsr00", "apsr01", "apsr02", "apsr03", "apsr04", "apsr05", "apsr06", "apsr07", "apsr08", "apsr09", "apsr10", "apsr11", "apsr12", "apsr13", "apsr14", "apsr15", "apsr16", "apsr17");

for ($i=0; $i<count($nodes); $i++) {
  echo $nodes[$i]."<BR>\n";
  flush();
  $data[$nodes[$i]] = dynamicReport($nodes[$i]);
} 

?>
</span>

<script type="text/javascript">
  var span = document.getElementById("gatherer");
  span.innerHTML = "";
</script>

<table class="datatable">
  <tr>
    <th>Node</th><th>Service Tag</th>
    <th>BIOS</th><th>DRAC F/W</th><th>BMC F/W</th><th>PERC 5i F/W</th><th>OMSA Ver.</th><th>PERC 5i Drv</th><th>OS Type</th><th>OS Ver</th><th>Temp</th><th>Disk0 ID</th><th>Disk0 F/W</th><th>Disk1 ID</th><th>Disk1 F/W</th>
  </tr>
<?
  $keys = array_keys($data);
  for ($i=0; $i<count($keys); $i++) {
    printRow($keys[$i], $data[$keys[$i]]);
  }
?>
</table>
</center>
</body>
</HTML>
<?

function printRow($node, $array) {
  echo "<tr>\n";
  echo "  <td><a href='https://".$node.".apsr.edu.au:1311/'>".$node."</a></td>\n";
  echo "  <td>".$array["Chassis_Service_Tag"]."</td>\n";
  echo "  <td>".$array["BIOS_Version"]."</td>\n";
  echo "  <td>".$array["DRAC_5_Version"]."</td>\n";
  echo "  <td>".$array["BMC_Version"]."</td>\n";
  echo "  <td>".$array["PERC_Firmware_Version"]."</td>\n";
  echo "  <td>".$array["OMSA_VERSION"]."</td>\n";
  echo "  <td>".$array["PERC_Driver_Version"]."</td>\n";
  echo "  <td>".$array["OS_TYPE"]."</td>\n";
  echo "  <td>".$array["OS_VERSION"]."</td>\n";
  echo "  <td>".$array["TEMP"]."</td>\n";
  echo "  <td>".$array["DISK0_PID"]."</td>\n";
  echo "  <td>".$array["DISK0_FW"]."</td>\n";
  echo "  <td>".$array["DISK1_PID"]."</td>\n";
  echo "  <td>".$array["DISK1_FW"]."</td>\n";
  echo "</tr>\n";
}

/*
 * ssh to the node and obtain RAC information about the system
 */
function dynamicReport($node) {

  $cmd = "ssh ".$node." '/usr/local/firmware/om_custom_report.csh'";
  $array=array();
  $last = exec($cmd, $array, $rval);
  $results = array();

  for ($i=0; $i<count($array); $i++) {
    $line = $array[$i];
    $arr = split(" ",$line, 2);
    $key = $arr[0];
    $val = $arr[1];
    $results[$key] = $val;
  }

  return $results;
}
