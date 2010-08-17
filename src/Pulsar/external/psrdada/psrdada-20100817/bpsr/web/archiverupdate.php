<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* special case for a click of the insert tape button */

if (isset($_GET["tapeinserted"])) {

  /* Need to clear PHP's internal cache */
  clearstatcache();
                                                                                              
  /* Find most recent result in results dir */
  $control_dir = "/nfs/control/bpsr";

  if (isset($_GET["location"])) {
    $location = strtolower($_GET["location"]);
  } else {
    $location = "parkes";
  }

  $fname = $control_dir."/".$location.".response.tmp";
  if (!($fp = @fopen($fname, 'w'))) {
    echo "ERROR: could not open response file for writing: $fname<BR>\n";
    exit();
  }
                                                                                              
  fwrite($fp, $_GET["tapeinserted"]."\n");
  fclose($fp);
  chmod($fname,0775);
  rename($fname, $control_dir."/".$location.".response");

} else {

  $config = getConfigFile(SYS_CONFIG,TRUE);
  $host = $config["SERVER_HOST"];
  $port = $config["SERVER_WEB_MONITOR_PORT"];

  list ($socket, $result) = openSocket($host, $port);

  if ($result == "ok") {

    $bytes_written = socketWrite($socket, "tape_info\r\n");
    $string = socketRead($socket);
    socket_close($socket);

  } else {
    $string = "Could not connect to $host:$port<BR>\n";
  }

  echo $string;
}
