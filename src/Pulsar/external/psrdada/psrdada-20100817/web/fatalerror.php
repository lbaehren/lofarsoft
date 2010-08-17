<?PHP
include("definitions_i.php");
include("functions_i.php");
include(INSTRUMENT."_functions_i.php");

$config = getConfigFile(SYS_CONFIG,TRUE);
$host = $config["SERVER_HOST"];
$port = $config["SERVER_WEB_MONITOR_PORT"];

list ($socket, $result) = openSocket($host, $port);

if ($result == "ok") {

  # ensure the read is non blocking

  $bytes_written = socketWrite($socket, "status_info\r\n");

  $read = socketRead($socket);
  socket_close($socket);

  $messages = array();
  $messages = explode(";;;", $read);

}

?>
<html>
<? include("header_i.php"); ?>

<script type="text/javascript" src="/js/soundmanager2.js"></script>
<script type="text/javascript">

  soundManager.url = '/sounds/sm2-swf-movies/'; // directory where SM2 .SWFs live
  var loaded = "false";

  // disable debug mode after development/testing..
  soundManager.debugMode = false;
  soundManager.volume = 50;

  soundManager.waitForWindowLoad = true;
  soundManager.onload = function() {
    // SM2 has loaded - now you can create and play sounds!
    soundManager.createSound('fatalerror','/sounds/piglet.mp3');
    loaded = "true";
  }

  function delayed_start() {
    if ( loaded == "false" ) {
      setTimeout('delayed_start()',1000);
    } else {
      looper();
    }
  }

  function looper() {

    soundManager.play('fatalerror');
    setTimeout('looper()',60000);

  }

</script>

<body onload="delayed_start()">
<p><b>A FATAL ERROR has occurred with <?echo strtoupper(INSTRUMENT)?></b></p>
<p>This will most likely require that you should stop the current observation and take action before attempting to resume. This may require a Restart Everything</p>
<p>
<?

  echo "<table width=90% border=1 bgcolor=#efa2a2>\n";
  echo "  <tr><th colspan=3>Errors</th></tr>\n";
  echo "  <tr><th>Machine</th><th>Type</th><th>Message</th></tr>\n";
  for ($i=0; $i<count($messages); $i++) {
    $msg = $messages[$i];
    $parts = explode(":::", $msg);
    $source = explode("_", $parts[0]);
    if ($parts[1] == 2) {
      echo "<tr><td>".$source[0]."</td><td>".$source[1]."</td><td>".$parts[2]."</td></tr>\n";
    }
  }
  echo "</table>\n";

  echo "</p><p>\n";

  echo "<table width=90% border=1 bgcolor=#fbff8e>\n";
  echo "  <tr><th colspan=3>Warnings</th></tr>\n";
  echo "  <tr><th>Machine</th><th>Type</th><th>Message</th></tr>\n";
  for ($i=0; $i<count($messages); $i++) {
    $msg = $messages[$i];
    $parts = explode(":::", $msg);
    $source = explode("_", $parts[0]);
    if ($parts[1] == 1) {
      echo "<tr><td>".$source[0]."</td><td>".$source[1]."</td><td>".$parts[2]."</td></tr>\n";
    }
  }
  echo "</table>\n";

?>
</p>
</body>
</html>
