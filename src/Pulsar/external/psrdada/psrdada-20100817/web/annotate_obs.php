<?PHP

  include("definitions_i.php");
  include("functions_i.php");

  # Get the system configuration (dada.cfg)
  $action = "read_annotation";
  if (isset($_POST["action"])) {
    $action  = $_POST["action"];
  }

  $cfg = getConfigFile(SYS_CONFIG, TRUE);
  $observation = $_GET["observation"];

  $results_dir = $cfg["SERVER_RESULTS_NFS_MNT"]."/".$observation;
  $text_file = $results_dir."/obs.txt";

  if (($action == "write_annotation") && (IN_CONTROL)) {

    $annotation = $_POST["annotation"];

    $handle = fopen($text_file, "w");
    if ($handle) {
      fwrite($handle,$annotation);
      fclose($handle);
      echo "<html>\n";
      echo "<script type=\"text/javascript\">\n";
      echo "  window.close()\n";
      echo "</script>\n";
      echo "</html>\n";
    } else {
      echo "Error: could not write to annotation file: ".$text_file."<BR>\n";
    }

  } else {

    $text_array = array();
    # If there is an existing annotation, read it

    if (file_exists($text_file)) {
      $handle = fopen($text_file, "r");
      while (!feof($handle)) {
        $buffer = fgets($handle, 4096);
        array_push($text_array, $buffer);
      }
      fclose($handle);
    }

    $title = strtoupper(INSTRUMENT)." | Annotate Obs. ".$observation;
    include("header_i.php");
    $text = "Annotate Observation";
    include("banner.php");
?>
<br>
<p>Annotation for Observation (UTC <?echo $observation?>)<br>File <?echo $text_file?></p>
<form name="annotation" action="annotate_obs.php?observation=<?echo $observation?>" method="post">
<textarea name="annotation" cols="80" rows="20" wrap="soft" <?if (!IN_CONTROL) echo "READONLY"?>>
<?
  for ($i=0; $i<count($text_array); $i++) {
    echo $text_array[$i];
  }
?>
</textarea>
  <br><br>
  <input type="hidden" name="action" value="write_annotation">
  <input type="hidden" name="observation" value="<?echo $observation?>">
<?if (IN_CONTROL) { ?>
  <input type="submit" value="Save"></input>
<? } ?>
  <input type="button" value="Close" onclick="window.close()"></input>
  </form>
</body>
</html>
<? } ?>

