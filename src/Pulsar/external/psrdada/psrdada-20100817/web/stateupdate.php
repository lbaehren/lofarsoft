<?PHP
include("definitions_i.php");
include("functions_i.php");

clearstatcache();

if (!(isset($_GET["port"])) || (!(isset($_GET["host"])))) {
  $string = "Malformed HTTP GET parameters";
} else {

  $host = $_GET["host"];
  $port = $_GET["port"];

  list ($socket, $result) = openSocket($host, $port);

  if ($result == "ok") {
    $bytes_written = socketWrite($socket, "state\r\n");
    $string = "State: ".socketRead($socket);
    socket_close($socket);
  } else {
    $string = "TCS INTERFACE STOPPED\n";
  }

  $control_info = "";
  if (file_exists(CONTROL_FILE)) {
    $control_info = "<BR>Host: ".file_get_contents(CONTROL_FILE);
  }
}
echo $string.$control_info;
flush();

