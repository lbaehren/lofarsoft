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
$title = "BPSR | Parkes Disks & Tapes";
include("header_i.php");
echo "<body>\n";

$text = "Parkes Archives";
include("banner.php");

echo "<center>\n";

# Get the disk listing information
$num_dirs = $cfg["NUM_PARKES_DIRS"];
$num_f_dirs = $cfg["NUM_PARKES_FOLD_DIRS"];

$names = array();
$f_names = array();
$disks = array();
$f_disks = array();
$users = array();
$f_users = array();
$hosts = array();
$f_hosts = array();

for ($i=0; $i<$num_dirs; $i++) {
  $array = split(":",$cfg["PARKES_DIR_".$i]);
  array_push($users, $array[0]);
  array_push($hosts, $array[1]);
  array_push($disks, $array[2]);
  array_push($names, $array[1]);
}

for ($i=0; $i<$num_f_dirs; $i++) {
  $array = split(":",$cfg["PARKES_FOLD_DIR_".$i]);
  array_push($f_users, $array[0]);
  array_push($f_hosts, $array[1]);
  array_push($f_disks, $array[2]);
  array_push($f_names, $array[1]);
}

# Get the bookkeeping db information
$array = split(":",$cfg["PARKES_DB_DIR"]);
$db_user = $array[0];
$db_host = $array[1];
$db_dir  = $array[2];
$db_file = $array[2]."/files.db";

echo "<span id=\"progress\">\n";
# Get the listing of archives on tape @ parkesburne
echo "Retreiving files.db from parkes<br>\n";
flush();
$cmd = "ssh -l ".$db_user." ".$db_host." \"cat ".$db_file." | grep '01 HRA' | sort\" | awk -F/ '{print $1}'";
$on_tape = array();
$lastline = exec($cmd, $on_tape, $return_var);

# Get the listing of archives in from_parkes @ parkesburne
$from_parkes = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls ".$disks[$i]." > /dev/nulll; ls ".$disks[$i]." | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes[$i] = $array;
}

# Get the listing of archives in from_parkes_pulsars @ parkesburne
$from_parkes_pulsars = array();
for ($i=0; $i<$num_f_dirs; $i++) {
  echo "Retreiving listing of ".$f_users[$i]." ".$f_hosts[$i]." ".$f_disks[$i]."<br>\n";
  flush();
  $cmd = "ssh -l ".$f_users[$i]." ".$f_hosts[$i]." 'ls ".$f_disks[$i]." > /dev/null; ls ".$f_disks[$i]." | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes_pulsars[$i] = $array;
}

# Get the listing of archives with xfer.complete in from_parkes @ parkesburne
$from_parkes_complete = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/20*/xfer.complete <br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls ".$disks[$i]."/20*/xfer.complete' | awk -F / '{print \$6}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $from_parkes_complete[$i] = $array;
}

# Get the listing of archives in on_tape disk @ parkes
$on_disk = array();
for ($i=0; $i<$num_dirs; $i++) {
  echo "Retreiving listing of ".$users[$i]." ".$hosts[$i]." ".$disks[$i]."/../on_tape/<br>\n";
  flush();
  $cmd = "ssh -l ".$users[$i]." ".$hosts[$i]." 'ls -1 ".$disks[$i]."/../on_tape | grep 20'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  $on_disk[$i] = $array;
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

?>

<table cellpadding=10>
  <tr>
    <td>

<table border=1 cellpadding=3 class="datatable">
<tr>
  <th>Count</th>
  <?
  for ($i=0; $i<$num_dirs; $i++) {
    echo "  <th colspan=2>".$names[$i]." [".$disk_sizes[$i]."]</th>\n";
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
for ($i=0; $i<$num_f_dirs; $i++) {
  if (count($from_parkes_pulsars[$i]) > $max_obs) { $max_obs = count($from_parkes_pulsars[$i]); }
}

for ($i=0; $i<$max_obs; $i++) {

  echo "<tr>\n";

  echo "<td>".$i."</td>\n";
  for ($j=0; $j<$num_dirs; $j++) {
    echo "<td>".$from_parkes[$j][$i]."</td>\n";
    echo "<td>".$on_disk[$j][$i]."</td>\n";
  }
  for ($j=0; $j<$num_f_dirs; $j++) {
    echo "<td>".$from_parkes_pulsars[$j][$i]."</td>\n";
  }
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
