<?PHP 

class instrument
{
  var $name;
  var $config_file;
  var $url;
  var $css_path = "";
  var $banner_image = "";
  var $banner_image_repeat = "";

  function instrument($name, $config_file, $url)
  {
    $this->name = $name;
    $this->config_file = $config_file;
    $this->url = $url;
  }

  function configFileToHash($fname="") {

    if ($fname == "") {
      $fname = $this->config_file;
    }
  
    $fptr = @fopen($fname,"r");
    $returnArray = array();

    if (!$fptr) {
      echo "Could not open file: $fname for reading<BR>\n";
    } else {
      while ($line = fgets($fptr, 1024)) {
     
        $comment_pos = strpos($line,"#");
        if ($comment_pos!==FALSE) {
          $line = substr($line, 0, $comment_pos);
        }

        // Remove trailing whitespace
        $line = chop($line);

        // skip blank lines
        if (strlen($line) > 0) {
          $array = split("[ \t]+",$line,2);   // Split into keyword/value
          $returnArray[$array[0]] = $array[1];
        }
      }
    }
    return $returnArray;
  }

  function print_css() {
    echo "<link rel='stylesheet' type='text/css' href='".$this->css_path."'>\n";
  }

  function print_favico() {
    echo "<link rel='shortcut icon' href='/images/favicon.ico'/>\n";
  }

  function print_banner() {

?>   
<table cellspacing=0 cellpadding=0 border=0 width="100%">
  <tr>
    <td width=480px height=60px><img src="<?echo $this->banner_image?>" width=480 height=60></td>
    <td width="100%" height=60px background="<?echo $this->banner_image_repeat?>" class="largetext"><span id="globalstatus" class="largetext"></span></td>
  </tr>
</table>
<?
  }

}

?>
