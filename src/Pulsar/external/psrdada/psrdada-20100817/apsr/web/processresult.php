<?PHP

include("definitions_i.php");
include("functions_i.php");

# Get the system configuration (dada.cfg)
$action = $_GET["action"];
$cfg = getConfigFile(SYS_CONFIG, TRUE);
$observation = $_GET["observation"];
$action_string = "No Action";

if ($action == "reprocess_low") $action_string = "Re-processing low-res results";
if ($action == "reprocess_hi") $action_string = "Re-processing hi-res results";
if ($action == "plot") $action_string = "Creating plots from results";
if ($action == "annotate") $action_string = "Create/Edit annotation";
if ($action == "write_annotation") $action_string = "Writing Annotation to file";
if ($action == "delete_results") $action_string = "Delete results only";
if ($action == "delete_obs") $action_string = "Delete results &amp; archives";

if (!IN_CONTROL) {

  $hostname = strtolower(gethostbyaddr($_SERVER["REMOTE_ADDR"]));
  $controlling_hostname = strtolower(rtrim(file_get_contents(CONTROL_FILE)));
                                                                                                                                          
  echo "<html>\n";
  include("header_i.php");
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

?>
<html>
<head>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML; ?>
  <title>APSR | Process Result <?echo $observation?></title>
  <script type="text/javascript">

    function finish(){
      window.opener.location.href=window.opener.location.href;
      window.opener.focus();
      window.close();
    }

    function finishParent() {

      window.opener.location.href = "/apsr/results.php";
      window.opener.opener.focus();
      window.close();
    }

  </script>
</head>
<body>
<? 

include("banner.php");

?>
<h3>Observation: <?echo $observation?></h3>

<?echo $action_string.": "?>

<?
flush();

# dont do anything if result is no action
if ($action_string == "No Action") {

} else if ($action == "reprocess_low") {

  chdir($cfg["SCRIPTS_DIR"]);
  $obs_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
  $script = "./server_apsr_process_result.pl ".$observation." ".$obs_dir." lowres";
  $cmd = "/bin/csh -c 'setenv HOME /home/dada; source \$HOME/.cshrc; ".$script."'";
  $string = exec($cmd, $array, $return_val);

  if ($return_val != 0) {
    echo "<BR>\nserver_apsr_process_results.pl returned a non zero exit value: ". $return_var."\n";
    flush();
  } else {
    echo "Low-res results reprocessed<BR>\n";
    flush();
    sleep(1);
    echo "<script type=\"text/javascript\"> finish(); </script>\n";
  }

} else if ($action == "reprocess_hi") {

  chdir($cfg["SCRIPTS_DIR"]);
  $obs_dir = $cfg["SERVER_ARCHIVE_NFS_MNT"]."/".$observation;
  $script = "./server_apsr_process_result.pl ".$observation." ".$obs_dir." ar";
  $cmd = "/bin/csh -c 'setenv HOME /home/dada; source \$HOME/.cshrc; ".$script."'";
  $string = exec($cmd, $array, $return_val);

  if ($return_val != 0) {
    echo $cmd."<BR>\n";
    echo "<BR>\nserver_apsr_process_results.pl returned a non zero exit value: ". $return_var."\n";
    echo $string."<BR>\n";
    print_r($array);
    
    flush();
  } else {
    echo "Hi-res results processed<BR>\n";
    flush();
    sleep(1);
    echo "<script type=\"text/javascript\"> finish(); </script>\n";
  }

} else if ($action == "plot") {

  chdir($cfg["SCRIPTS_DIR"]);
  $obs_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
  $script = "./server_apsr_create_plots.pl ".$obs_dir." 1024x768";
  $cmd = "/bin/csh -c 'setenv HOME /home/dada; source \$HOME/.cshrc; ".$script."'";
  $string = exec($cmd, $array, $return_val);

  $script = "./server_apsr_create_plots.pl ".$obs_dir." 240x180";
  $cmd = "/bin/csh -c 'setenv HOME /home/dada; source \$HOME/.cshrc; ".$script."'";
  $string = exec($cmd, $array, $return_val);

  if ($return_val != 0) {
    echo "<BR>\nserver_apsr_create_plots.pl returned a non zero exit value: ". $return_var."\n";
    flush();
  } else {
    echo "Plots created<BR>\n";
    flush();
    sleep(1);
    echo "<script type=\"text/javascript\"> finish(); </script>\n";
  }

} else if ($action == "annotate") {

  $results_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
  $text_file = $results_dir."/obs.txt";

  $text = array();
  # If there is an existing annotation, read it
  if (file_exists($text_file)) {
    $handle = fopen($text_file, "r");
    while (!feof($handle)) {
      $buffer = fgets($handle, 4096);
      array_push($text, $buffer);
    }
    fclose($handle);
  }
?>
  <br>
  <form name="annotation" action="processresult.php" method="get">
<textarea name="annotation" cols="80" rows="20" wrap="soft">
<?
  for ($i=0; $i<count($text); $i++) {
    echo $text[$i];
  }
?>
</textarea>
  <input type="hidden" name="action" value="write_annotation">
  <input type="hidden" name="observation" value="<?echo $observation?>">
  <input type="submit" value="Save"></input>
  <input type="button" value="Close" onclick="finish()"></input>
  </form>
<?

} else if ($action == "write_annotation") {

  if (isset($_GET["annotation"])) {
    $annotation = $_GET["annotation"];
    $results_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
    $text_file = $results_dir."/obs.txt";
    $handle = fopen($text_file, "w");
    if ($handle) {
      fwrite($handle,$annotation);
      fclose($handle);
      echo "<script type=\"text/javascript\">finish()</script>";
    } else {
      echo "Error: could not open annotation file: ".$text_file."<BR>\n";
    } 
  } else {
    echo "Error: misformed URL<BR>\n";
  }


} else if ($action == "delete_results") {


} else if ($action == "delete_obs") {

  $results_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
  $archive_dir = $cfg["SERVER_ARCHIVE_NFS_MNT"]."/".$observation;

  $cmd = "rm -rf \"".$results_dir."\" \"".$archive_dir."\"";
  $returnVal = system($cmd);

  if ($returnVal == 0) {

    flush();
?>
    <script type="text/javascript">
    finishParent();
    </script>
<?

  } else {

?>
    <p><font color=red>An Error Occurred whilst trying to delete observation</font></p>
<?

  }

} else {

}


?>

</body>
</html>

<?

function mySystem($string) {
  return exec("tcsh -c \"$string\"");
}

?>
