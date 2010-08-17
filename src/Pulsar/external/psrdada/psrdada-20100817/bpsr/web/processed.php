<?PHP
include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

define(U,"USER");
define(H,"HOST");
define(P,"PATH");
define(O,"ONTAPE");
define(F,"FILE");

define(TAPE,"T");
define(SIZE,"S");
define(DATE_ARCHIVED,"D");
define(FILE_NO,"F");
define(SOURCE,"S");

# Get the system configuration (dada.cfg)
$cfg = getConfigFile(SYS_CONFIG,TRUE);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

if (isset($_GET["current_day"])) {
  $current_day = $_GET["current_day"];
} else {
  $current_day = "all";
}

if (isset($_GET["type"])) {
  $type = $_GET["type"];
} else {
  $type = "all";
}

if (isset($_GET["show_processed"])) {
  $show_processed = $_GET["show_processed"];
} else {
  $show_processed = "yes";
}

if (isset($_GET["action"])) {
  $action = $_GET["action"];
} else {
  $action = "none";
}


echo "<html>\n";
$title = "BPSR | Processed Beams";
include("header_i.php");
echo "<body>\n";

$text = "Processed Beams";
include("banner.php");

echo "<center>\n";


# swin files.db info
$array = split(":",$cfg["SWIN_DB_DIR"]);
$s = array();
$s[U] = $array[0];
$s[H] = $array[1];
$s[P] = $array[2];
$s[F] = $array[2]."/files.db";

# swin staging area info
$sd = array();
$sd_num =  $cfg["NUM_SWIN_DIRS"];

for ($i=0; $i<$sd_num; $i++) {
  $arr = split(":",$cfg["SWIN_DIR_".$i]);
  $sd[$i] = array();
  $sd[$i][U] = $arr[0];
  $sd[$i][H] = $arr[1];
  $sd[$i][P] = $arr[2];
  $sd[$i][O] = str_replace("from_parkes", "on_tape", $arr[2]);
}

echo "<span id=\"progress\">\n";

echo "Retreiving swin files.db<br>\n";
flush();
$array = array();
$cmd = "ssh -l ".$s[U]." ".$s[H]." \"cat ".$s[F]." | sort\" | awk -F/ '{print $1\" \"$2 $3}'";
$lastline = exec($cmd, $array, $return_var);
$swin = array();
$sizes = array();
for ($i=0; $i<count($array); $i++) {
  $a = split(" ", $array[$i]);
  if (!array_key_exists($a[0],$swin)) {
    $swin[$a[0]] = array();
  }
  if (!array_key_exists($a[1],$swin[$a[0]])) {
    $swin[$a[0]][$a[1]] = array();
  }

  $swin[$a[0]][$a[1]][TAPE] = substr($a[2],3,3);
  $swin[$a[0]][$a[1]][DATE_ARCHIVED] = $a[3];
  $swin[$a[0]][$a[1]][FILE_NO] = $a[5];
  $sizes[$a[0]] = sprintf("%4.2f",$a[4]);

}

echo "Retreiving SOURCE names<BR>\n";

$cmd = "cd /nfs/archives/bpsr; grep SOURCE */obs.info | awk -F/ '{print $1\" \"$2}' | awk '{print $1\" \"$3}'";
$array = array();
$sources = array();
$lastline = exec($cmd, $array, $return_val);
for ($i=0; $i<count($array); $i++) {
  $a = split(" ", $array[$i]);
  $sources[$a[0]] = $a[1];
}

$beam_dir_find_suffix = "find -mindepth 2 -maxdepth 2 -type d' | awk '{print substr($1,3)}' | sort";

# swin from_parkes
$swin_from_parkes = array();
$swin_from_parkes_count = array();
for ($i=0; $i<$sd_num; $i++) {
  echo "Retreiving listing of ".$sd[$i][U]." ".$sd[$i][H]." ".$sd[$i][P]."<br>\n";
  flush();
  $cmd = "ssh -l ".$sd[$i][U]." ".$sd[$i][H]." 'cd ".$sd[$i][P]."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);

  for ($j=0; (($return_var == 0) && ($j<count($array))); $j++) {

    $a = split("/", $array[$j]);
    if (!array_key_exists($a[0],$swin_from_parkes)) {
      $swin_from_parkes[$a[0]] = array();
      for ($k=1; $k<=13; $k++) {
        $swin_from_parkes[$a[0]][sprintf("%02d",$k)] = 0;
      }
    }
    $swin_from_parkes[$a[0]][$a[1]] = 1;
  }
}

# swin on_tape
$swin_on_tape = array();
$swin_on_tape_count = array();
for ($i=0; $i<$sd_num; $i++) {
  echo "Retreiving listing of ".$sd[$i][U]." ".$sd[$i][H]." ".$sd[$i][O]."<br>\n";
  flush();
  $cmd = "ssh -l ".$sd[$i][U]." ".$sd[$i][H]." 'cd ".$sd[$i][O]."; ".$beam_dir_find_suffix;
  echo $cmd."<BR>\n";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);

  for ($j=0; (($return_var == 0) && ($j<count($array))); $j++) {
    $a = split("/", $array[$j]);
    if (!array_key_exists($a[0],$swin_on_tape)) {
      $swin_on_tape[$a[0]] = array();
      for ($k=1; $k <= 13; $k++) {
        $swin_on_tape[$a[0]][sprintf("%02d",$k)] = 0;
      }
    }
    $swin_on_tape[$a[0]][$a[1]] = 1;
  }
}

echo "Retreiving Lina's processed beams<br>";
flush();

$llevin = array();

$cmd = "ssh -l ".$s[U]." ".$s[H]." \"find /nfs/cluster/pulsar/hitrun -name '*llevin*' -printf '%f\\n' | sort\" | awk -F. '{print \$1\" \"\$2}'";
$lastline = exec($cmd, $llevin, $return_var);

$llevin_beams = array();
for ($i=0; $i<count($llevin); $i++) {
  $arr = split(" ", $llevin[$i]);
  if (!array_key_exists($arr[0], $llevin_beams)) {
    $llevin_beams[$arr[0]] = array();
    for ($j=1; $j <= 13; $j++) {
      $llevin_beams[$arr[0]][sprintf("%02d",$j)] = 0;
    }
  }
  $llevin_beams[$arr[0]][$arr[1]] = 1;
}

echo "DONE, formatting page<br>\n";
echo "</span>";

?>
<script type="text/javascript">

  function mySubmit(value) {
    document.day_select.submit();
  }
</script>

<table cellpadding=10>
  <tr>
    <td valign=top align=center>
      <form name="day_select" action="processed.php" method="GET">
      <input type="hidden" id="action" name="action" value="none"></input>

      <table cellpadding=5>
        <tr><td>
          Select a day:
          <select name="current_day" onChange="form.submit()">
          <?
          for ($i=0; $i<count($days); $i++) {
            if ($current_day == $days[$i]) {
              echo "<option selected>".$days[$i]."</option>\n";
            } else {
              echo "<option>".$days[$i]."</option>\n";
            }
          }
          ?>
          </select>

        </td></tr>

        <tr><td>
        Show Processed ? <input type="radio" name="show_processed" value="yes" <? if($show_processed == "yes") echo " checked"?>>Yes</input>
                         <input type="radio" name="show_processed" value="no" <?  if($show_processed == "no")  echo " checked"?>>No</input>
        </td></tr>

        <tr><td colspan=2 align=center><input type="button" value="Submit" onClick="mySubmit('none')"></input></td></tr>

      </table> 
      </form>

      <table class="datatable">
        <tr><th colspan=3>Legend</th></tr>
        <tr><th>Key</th><th>Value</th><th>Meaning</th></tr>
        <tr><td>Num</td><td>INT</td><td>Count of obs</td></tr>
        <tr><td>UTC</td><td>STR</td><td>UTC START of the pointing</td></tr>
        <tr><td>Beams</td><td>Tape ID</td><td>3 digit tape ID (e.g. HRE***S4)</td></tr>
        <tr><td>Beams</td><td bgcolor='lightgreen'></td><td>Beam processed</td></tr>
        <tr><td>Beams</td><td bgcolor='yellow'></td><td>Beam awaiting processing</td></tr>
        <tr><td>Beams</td><td bgcolor='#FFCC66'></td><td>Beam archived and deleted, but not processed</td></tr>
        <tr><td>Beams</td><td bgcolor='red'></td><td>Beam missing</td></tr>
        <tr><td>Beams</td><td></td><td>Beam not yet received</td></tr>
      </table>

    </td>
    <td>

<table border=1 cellpadding=3 class="datatable">
<tr>
  <th colspan=3>&nbsp;</th>
  <th colspan=13>Beams</th>
  <th></th>
</tr>
<tr>
  <th>Num</th>
  <th>SOURCE</th>
  <th>UTC Start</th>
<?
  for ($i=1; $i<=13; $i++) {
    echo "  <th>".sprintf("%02d",$i)."</th>\n";
  }
?>
  <th>Size [GB]</th>
</tr>

<?

$keys = array_keys($swin);
sort($keys);

for ($i=0; $i < count($keys); $i++) {
  
  $k = $keys[$i];
  $o = $swin[$k];
  $l = $llevin_beams[$k];
  $size = $sizes[$k];

  echo "<tr>";

  echo "<td>".$i."</td>";

  /* SOURCE  */
  echo "<td>".$sources[$k]."</td>";

  /* UTC_START */
  echo "<td>".$keys[$i]."</td>";

  /* BEAMS */
  for ($j=1; $j<=13; $j++) {
    $b = sprintf("%02d", $j);

    if (array_key_exists($b, $o)) {
      if ($l[$b] == 1) {
        echo "<td bgcolor='lightgreen'>";
      } else {
        if ($swin_on_tape[$k][$b] == 1) {
          echo "<td bgcolor='yellow'>";
        } else {
          echo "<td bgcolor='#FFCC66;'>";
        }
      }
      echo $o[$b][TAPE]."</td>";
    } else {
      if ($swin_from_parkes[$k][$b] == 1) {
        echo "<td></td>";
      } else {
        echo "<td bgcolor='red'></td>";
      }
    }
  }
  echo "<td>".$size."</td>";
  echo "</tr>\n";
}
?>
</table>

</td></tr>
</table>

<script type="text/javascript">
  document.getElementById("progress").innerHTML = "";
</script>

</body>
</html>
