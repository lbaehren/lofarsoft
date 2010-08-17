<?PHP

  include("definitions_i.php");
  include("functions_i.php");

  $hostname = strtolower(gethostbyaddr($_SERVER["REMOTE_ADDR"]));
  $title = strtoupper(INSTRUMENT)." | Commandeer Instrument";

  $valid = true;

  if ( (isset($_POST["action"])) && ($_POST["action"] == "commandeer") ) {

    if ( (isset($_POST["username"])) && (isset($_POST["password"])) ) {
      $username = $_POST["username"];
      $password = $_POST["password"];

      if ( !(validate($username, $password)) ){
        $valid = false;
      }

    } else if ( (isset($_POST["username"])) && (isTrustedHost($hostname)) ) {
      $username = $_POST["username"];

    } else {
      $valid = false;
    }

    if ($valid) {
      $fptr = @fopen(CONTROL_FILE,"w");
      fwrite($fptr,$hostname."\n");
      fclose($fptr);
      echo "<html>\n";
      echo "<script type=\"text/javascript\">\n";
      echo "window.close()\n";
      echo "</script>\n";
      echo "</html>\n";
      system("touch ".CHANGE_INSTRUMENT_FILE);
    } else {

      include("header_i.php");
      $text = "Commandeer Instrument";
      include("banner.php");
    }

  } else {

    include("header_i.php");
    $text = "Commandeer Instrument";
    include("banner.php");
  }


  if (file_exists(CONTROL_FILE)) {
    $current_hostname = strtolower(rtrim(file_get_contents(CONTROL_FILE)));
  } else {
    $current_hostname = "not set";
  }

?>
<body>
<br>
<p>Admin users and all users running on Control Room machines can commandeer the system</p>
<form name="annotation" action="commandeer.php" method="post">

<p>The host in control is: <?echo $current_hostname?><br>
   Your host is seen to be: <?echo $hostname?></p>
<br>
<center>
<table>
<? if (!($valid)) { ?>

  <tr><td colspan=2><font color=red>VALIDATION FAILED</font></td></tr>

<? }
  if ($current_hostname == $hostname) { ?>

  <tr><td colspan=2><font color=green>You already are running your brower from the Controlling Host</td></tr> 

<? } else if (isTrustedHost($hostname)) { ?>
  
  <tr><td colspan=2><font color=green>You ARE on a trusted host. Click Submit to take control</td></tr>
  <tr><td>User:</td><td>apsr<input type="hidden" name="username" value="apsr"></td></tr>
  <tr><td>Host:</td><td><?echo gethostbyaddr($_SERVER["REMOTE_ADDR"])?></td></tr>
  <tr><td align=center colspan=2>
    <input type="hidden" name="action" value="commandeer"></input>
    <input type="submit" value="Submit"></input>
    <input type="button" value="Cancel" onclick="window.close()"></input>
  </td></tr>

<? } else { ?>

  <tr><td colspan=2><font color=red>You are not on a trusted host. Username and password required</font></td></tr>
  <tr><td>Username:</td><td><input type="text" name="username" value=""></input></td></tr>
  <tr><td>Password:</td><td><input type="password" name="password"></input></td></tr>
  <tr><td align=center colspan=2>
    <input type="hidden" name="action" value="commandeer"></input>
    <input type="submit" value="Submit"></input>
    <input type="button" value="Cancel" onclick="window.close()"></input>
  </td></tr>

<? } ?>
</table>
</center>
</form>
</body>
</html>

