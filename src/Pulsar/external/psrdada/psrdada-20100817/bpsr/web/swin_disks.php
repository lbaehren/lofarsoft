<?PHP
include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

# Get the system configuration (dada.cfg)
$cfg = getConfigFile(SYS_CONFIG,TRUE);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

$archives_dir = $cfg["SERVER_ARCHIVE_DIR"];
$results_dir = $cfg["SERVER_RESULTS_DIR"];

if (isset($_GET["du"])) {
  $du = 1;
} else {
  $du = 0;
}

echo "<html>\n";
$title = "BPSR | Swinburne Disks & Tapes";
include("header_i.php");
echo "<body>\n";

$text = "Swin Archives";
include("banner.php");

echo "<center>\n";

# Get the disk listing information
$num_dirs = $cfg["NUM_SWIN_DIRS"];
$num_f_dirs = $cfg["NUM_SWIN_FOLD_DIRS"];

$names = array();
$f_names = array();
$disks = array();
$f_disks = array();
$users = array();
$f_users = array();
$hosts = array();
$f_hosts = array();

$llevin = array();
#$sbates = array();

for ($i=0; $i<$num_dirs; $i++) {
  $array = split(":",$cfg["SWIN_DIR_".$i]);
  array_push($users, $array[0]);
  array_push($hosts, $array[1]);
  array_push($disks, $array[2]);
  $array = split("/",$disks[$i]);
  array_push($names, $array[3]);
}

for ($i=0; $i<$num_f_dirs; $i++) {
  $array = split(":",$cfg["SWIN_FOLD_DIR_".$i]);
  array_push($f_users, $array[0]);
  array_push($f_hosts, $array[1]);
  array_push($f_disks, $array[2]);
  $array = split("/",$f_disks[$i]);
  array_push($f_names, $array[3]);
}

# Get the bookkeeping db information
$array = split(":",$cfg["SWIN_DB_DIR"]);
$db_user = $array[0];
$db_host = $array[1];
$db_dir  = $array[2];
$db_file = $array[2]."/files.db";

echo "<span id=\"progress\">\n";
# Get the listing of archives on tape @ swinburne
echo "Retreiving files.db from swin<br>\n";
flush();
$cmd = "ssh -l ".$db_user." ".$db_host." \"cat ".$db_file." | grep '01 HRA' | sort\" | awk -F/ '{print $1}'";
$on_tape = array();
$lastline = exec($cmd, $on_tape, $return_var);

# Get the listing of archives in from_parkes @ swinburne
$from_parkes = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls ".$disks[$i]." > /dev/nulll; ls ".$disks[$i]." | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes[$i] = $array;
}

# Get the listing of archives in from_parkes_pulsars @ swinburne
$from_parkes_pulsars = array();
for ($i=0; $i<$num_f_dirs; $i++) {
  echo "Retreiving listing of ".$f_users[$i]." ".$f_hosts[$i]." ".$f_disks[$i]."<br>\n";
  flush();
  $cmd = "ssh -l ".$f_users[$i]." ".$f_hosts[$i]." 'ls ".$f_disks[$i]." > /dev/null; ls ".$f_disks[$i]." | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes_pulsars[$i] = $array;
}

# Get the listing of archives with xfer.complete in from_parkes @ swinburne
$from_parkes_complete = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/20*/xfer.complete <br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls ".$disks[$i]."/20*/xfer.complete' | awk -F / '{print \$6}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes_complete[$i] = $array;
}

# Get the listing of archives in on_tape disk @ swin
$on_disk = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/../on_tape/<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls -1 ".$disks[$i]."/../on_tape | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $on_disk[$i] = $array;
}

# Get the count of beam directories in on_tape
$on_disk_count = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving beam count of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/../on_tape/<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'cd ".$disks[$i]."/../on_tape; find . -maxdepth 2 -name \"??\" -type d' | awk -F/ '{print $2\" \"$3}' | sort";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split(" ", $array[$j]);
    $on_disk_count[$arr[0]] .= $arr[1]." ";
  }
}

$disk_sizes = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving disk capacity for ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'df -h ".$disks[$i]." | tail -n 1' | awk '{print $2, $3, $4, $5}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $disk_sizes[$i] = $array[0];
}

$f_disk_sizes = array();
for ($i=0; $i<$num_f_dirs; $i++) {
  echo "Retreiving disk capacity for ".$f_users[$i]." ".$f_hosts[$i]." ".$f_disks[$i]."/<br>\n";
  flush();
  $cmd = "ssh -l ".$f_users[$i]." ".$f_hosts[$i]." 'df -h ".$f_disks[$i]." | tail -n 1' | awk '{print $2, $3, $4, $5}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $f_disk_sizes[$i] = $array[0];
}

echo "Retreiving llevin's processed beams<br>";
flush();
$cmd = "ssh -l ".$users[0]." ".$hosts[0]." 'cd /nfs/cluster/pulsar/hitrun; find . -name \"*llevin*\" -printf \"%f\\n\" | sort' | awk -F. '{print \$1\" \"\$2}'";
$lastline = exec($cmd, $llevin, $return_var);

echo "Retreiving llevin's cleared + processed  beams<br>";
flush();
$cmd = "ssh -l ".$users[0]." ".$hosts[0]." 'cd /nfs/cluster/pulsar/hitrun/cleared; find . -name \"*llevin*\" -printf \"%f\\n\" | sort' | awk -F. '{print \$1\" \"\$2}'";
$lastline = exec($cmd, $array, $return_var);

$llevin = array_merge($llevin, $array);

$llevin_beams = array();
for ($i=0; $i<count($llevin); $i++) {
  $arr = split(" ", $llevin[$i]);
  $llevin_beams[$arr[0]] .= $arr[1]." ";
}


#echo "Retreiving sbates processed beams";
#flush();
#$cmd = "ssh -l ".$users[0]." ".$hosts[0]." 'cd /nfs/cluster/pulsar/hitrun; find . -name \"*sbates*\" -printf \"%f\\n\" | sort' | awk -F. '{print \$1}'";
#$lastline = exec($cmd, $sbates, $return_var);

# Determine which of Sam/Linas obs are on which disk
$l_counts = array();
#$s_counts = array();
$llevin_keys = array_keys($llevin_beams);
for ($i=0; $i<$num_dirs; $i++) {
  $l_counts[$i] = 0;
  #$s_counts[$i] = 0;
  for ($j=0; $j<count($on_disk[$i]); $j++) {
    if (in_array($on_disk[$i][$j], $llevin)) {
      $l_counts[$i] = $l_counts[$i] + 1;
    }
    #if (in_array($on_disk[$i][$j], $sbates)) {
    #  $s_counts[$i] = $s_counts[$i] + 1;
    #}
  }
}


# Get the sizes of the on_tape and from_parkes dirs
$on_tape_sizes = array();
$from_parkes_sizes = array();
if ($du == 1) {
  for ($i=0; $i<$num_dirs; $i++) {
    echo "Retreiving dir sizes of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."<br>\n";
    flush();

    $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'du -sLH ".$disks[$i]."' | awk '{print $1}'";
    $array = array();
    $lastline = exec($cmd, $array, $return_var);
    $from_parkes_sizes[$i] = $array[0];

    echo "Retreiving dir sizes of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/../on_tape/<br>\n";
    $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'du -sLh ".$disks[$i]."/../on_tape' | awk '{print $1}'";
    $array = array();
    $lastline = exec($cmd, $array, $return_var);
    $on_tape_sizes[$i] = $array[0];
  }

  for ($i=0; $i<$num_f_dirs; $i++) {
    echo "Retreiving dir sizes of ".$f_users[$i]." ".$f_hosts[$i]." ".$f_disks[$i]."<br>\n";
    $cmd = "ssh -l ".$f_users[$i]." ".$f_hosts[$i]." 'du -sLh ".$f_disks[$i]."' | awk '{print $1}'";
    $array = array();
    $lastline = exec($cmd, $array, $return_var);
    $from_parkes_pulsars_sizes[$i] = $array[0];
  }
}

echo "DONE, formatting page<br>\n";
echo "</span>";

//print_r($llevin);

?>

<table cellpadding=10>
  <tr>
    <td>

<table border=1 style="cellpadding: 0px; cellspacing=0px;" class="datatable">
<tr>
  <th></th>
  <?
  for ($i=0; $i<$num_dirs; $i++) {
    echo "  <th colspan=2>".$names[$i]." [".$disk_sizes[$i]."]</th>\n";
    if (count($on_disk[$i]) > 0) {
      $l_percent = sprintf("%5.2f",($l_counts[$i] / count($on_disk[$i]))*100);
      #$s_percent = sprintf("%5.2f",($s_counts[$i] / count($on_disk[$i]))*100);
    } else {
      $l_percent = 0.0;
      #$s_percent = 0.0;
    }
    echo "  <th colspan=13>[".$l_percent." %]</th>\n";
   # echo "  <th>[".$s_percent." %]</th>\n";

  }
  for ($i=0; $i<$num_f_dirs; $i++) {
    echo "  <th colspan=1>".$f_names[$i]." [".$f_disk_sizes[$i]."]</th>\n";
  }
  ?>
</tr>
<tr>
  <th></th>
  <?
  for ($i=0; $i<$num_dirs; $i++) {
    echo "  <th>from_parkes [".$from_parkes_sizes[$i]."]</th><th>on_tape [".$on_tape_sizes[$i]."]</th>";
    for ($j=1; $j<=13; $j++) {
      $beamname = sprintf("%02d",$j);
      echo "<td width=2px></td>";
    }
  }
  for ($i=0; $i<$num_f_dirs; $i++) {
    echo "<th>from_parkes_puslars [".$fron_parkes_pulsars_sizes[$i]."]</th>\n";
  }
  ?>
</tr>

<?
$max_obs = 0;
for ($i=0; $i<$num_dirs; $i++) {
  if (count($from_parkes[$i]) > $max_obs) { $max_obs = count($from_parkes[$i]); }
  if (count($from_parkes_complete[$i]) > $max_obs) { $max_obs = count($from_parkes_complete[$i]); }
  if (count($on_disk[$i]) > $max_obs) { $max_obs = count($on_disk[$i]); }
}
#for ($i=0; $i<$num_f_dirs; $i++) {
#  if (count($from_parkes_pulsars[$i]) > $max_obs) { $max_obs = count($from_parkes_pulsars[$i]); }
#}

//print_r($llevin);
//print_r($sbates);

for ($i=0; $i<$max_obs; $i++) {

  echo "<tr>\n";

  echo "<td>".$i."</td>\n";
  for ($j=0; $j<$num_dirs; $j++) {
    echo "<td>".$from_parkes[$j][$i]."</td>\n";

    $array = array_keys($llevin, $on_disk[$j][$i]);
    $l_count = count($array);
    #$array = array_keys($sbates, $on_disk[$j][$i]);
    #$s_count = count($array);

    if (($l_count == 13) && ($s_count == 13)) {
      $bgcolor = " bgcolor=lightgreen";
    } else if ($l_count == 13) {
      $bgcolor = " bgcolor='#FFFF99;'";
    #} else if ($s_count == 13) {
    #  $bgcolor = " bgcolor=#FFFF99;'";
    #} else {
      $bgcolor = "";
    }

    if (!($on_disk[$j][$i])) {
      $bgcolor = "";
    }

    echo "<td".$bgcolor.">".$on_disk[$j][$i]."</td>\n";

    for ($k=1; $k<=13; $k++) {
      $beamname = sprintf("%02d",$k);

      # check if the UTC_START/BEAM exists in the on_tape dir
      if (strpos($on_disk_count[$on_disk[$j][$i]], $beamname) !== FALSE) {
        # If so we do a green light if lina has processed it
        if (strpos($llevin_beams[$on_disk[$j][$i]], $beamname) !== FALSE) {
          echo "<td bgcolor=green></td>";
        } else {
          echo "<td bgcolor=yellow></td>";
        }
      } else {
        echo "<td></td>\n";
      }

    }

    #if ($s_count == 13) {
    #  echo "<td><img src='/images/green_light.png'>13/13</td>\n";
    #} else if ($s_count > 0) {
    #  echo "<td><img src='/images/yellow_light.png'>".$s_count."/13</td>\n";
    #} else {
    #  echo "<td>0/13</td>\n";
    #}


  }
  for ($j=0; $j<$num_f_dirs; $j++) {
    echo "<td>".$from_parkes_pulsars[$j][$i]."</td>\n";
  }
  echo "</tr>\n";
}

//print_r($llevin_beams);


?>
</table>

</td></tr>
</table>

<script type="text/javascript">
  document.getElementById("progress").innerHTML = "";
</script>

</body>
</html>
