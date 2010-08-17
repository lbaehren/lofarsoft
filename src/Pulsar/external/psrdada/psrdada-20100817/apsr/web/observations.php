<?PHP
include("definitions_i.php");
include("functions_i.php");

ini_set("memory_limit","128M");

$cfg = getConfigFile(SYS_CONFIG,TRUE);

define(YES, "Y");
define(NO, "N");
define(MAYBE, "M");
define(OFF, "_");

define(NFS_RESULTS,  "NFSR");
define(NFS_ARCHIVES, "NFSA");
define(LFS_ARCHIVES, "LFSA");
define(ON_SWIN,      "ONSW");
define(OBS_INFO,     "OBSI");
define(OBS_TXT,      "OBST");
define(OBS_XFER,     "OBSX");
define(OBS_DEL,      "OBSD");
define(OBS_FINI,     "OBSF");
define(OBS_FAIL,     "OBSN");
define(OBS_PROC,     "OBSP");
define(SOURCE,       "SRC");
define(ARCHIVE_SIZE, "SIZE");


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

if (isset($_GET["action"])) {
  $action = $_GET["action"];
} else {
  $action = "none";
}

?>
<html>
<head>
  <title>APSR | Observations</title>
  <link rel="STYLESHEET" type="text/css" href="/bpsr/style.css">
  <link rel="shortcut icon" href="/images/favicon.ico"/>
  <style>
    .active {
      font-family: arial,helvetica,sans-serif,times;
      font-size: 8pt;
      height: 10px;
      padding: 1px 6px 1px 6px;
      border: 1px solid #C6C6C6;
    }
    .inactive {
      font-family: arial,helvetica,sans-serif,times;
      font-size: 0pt;
      height: 0px;
      padding: 0px 0px 0px 0px;
      border: 0px;
    }
    table.obstable {
      margin: 0px 0px 0px 0px;
      padding: 0px 0px 0px 0px;
      border-collapse: separate;
      width: 100%;
      clear: both;
    }
  </style>
</head>
<body>

<?
echo "<body>\n";

$text = "APSR Archives Page";
include("banner.php");

echo "<center>\n";

# Get the disk listing information
$num_swin_dirs = $cfg["NUM_SWIN_DIRS"];
$num_parkes_dirs = $cfg["NUM_PARKES_DIRS"];

echo "<span id=\"progress\">\n";

$results = array();

###############################################################################

echo $cfg["SERVER_RESULTS_DIR"]." listing<br>\n";
flush();
$cmd = "find ".$cfg["SERVER_RESULTS_DIR"]." -maxdepth 1 -type d -name \"2*\" -printf \"%f\\n\" | sort";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $u = $array[$i];
  if (!array_key_exists($u, $results)) {
    $results[$u] = array();
    $results[$u][NFS_RESULTS] = NO;
    $results[$u][NFS_ARCHIVES] = NO;
    $results[$u][LFS_ARCHIVES] = 0;
    $results[$u][OBS_INFO] = NO;
    $results[$u][OBS_TXT] = OFF;
    $results[$u][OBS_XFER] = OFF;
    $results[$u][OBS_DEL] = OFF;
    $results[$u][OBS_FINI] = OFF;
    $results[$u][OBS_FAIL] = OFF;
    $results[$u][ON_SWIN] = OFF;
    $results[$u][OBS_PROC] = OFF;
  }
  $results[$u][NFS_RESULTS] = YES;
}
$obs_res_count = count($array);

###############################################################################

echo $cfg["SERVER_ARCHIVE_DIR"]." listing<br>\n";
flush();
$cmd = "find ".$cfg["SERVER_ARCHIVE_DIR"]." -maxdepth 1 -type d -name \"2*\" -printf \"%f\\n\" | sort";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $u = $array[$i];
  if (!array_key_exists($u, $results)) {
    $results[$u] = array();
    $results[$u][NFS_RESULTS] = NO;
    $results[$u][NFS_ARCHIVES] = NO;
    $results[$u][LFS_ARCHIVES] = 0;
    $results[$u][OBS_INFO] = NO;
    $results[$u][OBS_TXT] = OFF;
    $results[$u][OBS_XFER] = OFF;
    $results[$u][OBS_DEL] = OFF;
    $results[$u][OBS_FINI] = OFF;
    $results[$u][OBS_FAIL] = OFF;
    $results[$u][ON_SWIN] = OFF;
    $results[$u][OBS_PROC] = OFF;
  }
  $results[$u][NFS_ARCHIVES] = YES;
}
$obs_arc_count = count($array);


##############################################################################

$obs_lar_count = 0;
for ($i=0; $i<=17; $i++) {
  $h = sprintf("apsr%02d", $i);
  echo $h.":".$cfg["CLIENT_ARCHIVE_DIR"]." listing<br>\n";
  flush();
  $cmd = "ssh ".$h." \"find ".$cfg["CLIENT_ARCHIVE_DIR"]." -maxdepth 1 -type d -name '2*' -printf '%f\\n'\" | sort";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $u = $array[$j];
    if (array_key_exists($u, $results)) {
      $results[$u][LFS_ARCHIVES]++;
    }
    if ($results[$u][LFS_ARCHIVES] == 1)
      $obs_lar_count += count($array);
  }
}

###############################################################################


$obs_fin_count = 0;
$obs_xfe_count = 0;
$obs_txt_count = 0;
$obs_inf_count = 0;
$obs_del_count = 0;
$obs_fail_count = 0;
$obs_unk_count = 0;
$obs_proc_count = 0;

echo  $cfg["SERVER_ARCHIVE_DIR"]."/*/obs.*<br>\n";
flush();
$cmd = "find ".$cfg["SERVER_ARCHIVE_DIR"]." -maxdepth 2 -name \"obs.*\" -printf \"%h %f\\n\" | awk -F/ '{print \$NF}'";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $z = $array[$i];
  $a = split(" ",$z);
  $u = $a[0];
  $f = $a[1];
  if (!array_key_exists($u, $results)) {
    $results[$u] = array();
    $results[$u][NFS_RESULTS] = NO;
    $results[$u][NFS_ARCHIVES] = NO;
    $results[$u][LFS_ARCHIVES] = 0;
    $results[$u][OBS_INFO] = NO;
    $results[$u][OBS_TXT] = OFF;
    $results[$u][OBS_XFER] = OFF;
    $results[$u][OBS_DEL] = OFF;
    $results[$u][OBS_FINI] = OFF;
    $results[$u][OBS_FAIL] = OFF;
    $results[$u][ON_SWIN] = OFF;
    $results[$u][OBS_PROC] = OFF;
  }
  if ($f == "obs.info") {
    $results[$u][OBS_INFO] = YES;
    $obs_inf_count++;
  } else if ($f == "obs.txt") {
    $results[$u][OBS_TXT] = YES;
    $obs_txt_count++;
  } else if ($f == "obs.transferred") {
    $results[$u][OBS_XFER] = YES;
    $obs_xfe_count++;
  } else if ($f == "obs.finished") {
    $results[$u][OBS_FINI] = YES;
    $obs_fin_count++;
  } else if ($f == "obs.deleted") {
    $results[$u][OBS_DEL] = YES;
    $obs_del_count++;
  } else if ($f == "obs.failed") {
    $results[$u][OBS_FAIL] = NO;
    $obs_fail_count++;
  } else if ($f == "obs.processing") {
    $results[$u][OBS_PROC] = MAYBE;
    $obs_proc_count++;
  } else {
    $obs_unk_count++;
  }
}

###############################################################################


echo "Parsing obs.info for parameters<br>\n";
flush();
$str = "";
for ($i=0; $i<100; $i++) {
  $str .= "&nbsp;";
}

echo "0% [<span id='progress_bar'><tt>".$str."</tt></span>] 100%\n";
flush();

$keys = array_keys($results);
$params = array("SOURCE", "BANDWIDTH", "NBIT", "PID", "CFREQ", "PROC_FILE");
$tot = count($keys);

for ($i=0; $i<count($keys); $i++) {

  if (floor(($i/$tot)*100) > $cnt) {
    $new_str = "";
    for ($j=0; $j<100; $j++) {
      if ($j<$cnt) {
        $new_str .= ".";
      } else {
        $new_str .= "&nbsp;";
      }
    }
    echo "<script type='text/javascript'>document.getElementById(\"progress_bar\").innerHTML = \"<tt>".$new_str."</tt>\"</script>\n";
    flush();
    $cnt++;
  }
  $o = $keys[$i];

  if ($results[$o][OBS_INFO] == YES) {
    $cmd = "cat ".$cfg["SERVER_RESULTS_DIR"]."/".$o."/obs.info";
    $array = array();
    $lastline = exec($cmd, $array, $return_val);
    for ($j=0; $j<count($array); $j++) {
      $a = split("[ \t]+",$array[$j],2);
      if (in_array($a[0],$params)) {
        $results[$o][$a[0]] = $a[1];
      }
    }
  }
  
  # try to get the PROC_FILE command from an obs.start if it exists
  /* 
  $cmd = "find ".$cfg["SERVER_RESULTS_DIR"]."/".$o."/ -name 'obs.start' | head -n 1";
  $lastline = exec($cmd, $array, $return_val);
  $results[$o][PROC_FILE] = "";
  if (($return_val == 0) && (file_exists($lastline))) {
    $cmd = "grep PROC_FILE ".$lastline." | awk '{print $2}'";
    $lastline = exec($cmd, $array, $return_val);
    if ($return_val == 0) {
      $results[$o][PROC_FILE] = $lastline;
    }
  }*/

}
echo "<br>\n";

###############################################################################

echo "Checking Swinburne archives<br>\n";

$cmd = "ssh -l pulsar shrek210-evlbi \"cd /export/shrek210b/apsr/; find . -mindepth 2 -maxdepth 2 -type d -name '2*' -printf '%f\\n'\" | sort";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $u = $array[$i];
  if (array_key_exists($u, $results)) {
    $results[$u][ON_SWIN] = YES;
  }
}
$on_swin_count = count($array);

# P630 data
$cmd = "ssh -l pulsar shrek211-evlbi \"cd /export/shrek211a/apsr/; find . -mindepth 2 -maxdepth 2 -type d -name '2*' -printf '%f\\n'\" | sort";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $u = $array[$i];
  if (array_key_exists($u, $results)) {
    $results[$u][ON_SWIN] = YES;
  }
}
$on_swin_count += count($array);

# P661 data
$cmd = "ssh -l pulsar shrek211-evlbi \"cd /export/shrek211b/apsr/; find . -mindepth 2 -maxdepth 2 -type d -name '2*' -printf '%f\\n'\" | sort";
$array = array();
$lastline = exec($cmd, $array, $return_var);
for ($i=0; $i<count($array); $i++) {
  $u = $array[$i];
  if (array_key_exists($u, $results)) {
    $results[$u][ON_SWIN] = YES;
  }
}
$on_swin_count += count($array);



###############################################################################


echo "Determining Intergration Lengths<br>\n";
$str = "";
for ($i=0; $i<100; $i++) {
  $str .= "&nbsp;";
}

echo "0% [<span id='progress_bar2'><tt>".$str."</tt></span>] 100%\n";
flush();

$cnt = 0;
for ($i=0; $i<count($keys); $i++) {

  if (floor(($i/$tot)*100) > $cnt) {
    $new_str = "";
    for ($j=0; $j<100; $j++) {
      if ($j<$cnt) {
        $new_str .= ".";
      } else {
        $new_str .= "&nbsp;";
      }
    }
    echo "<script type='text/javascript'>document.getElementById(\"progress_bar2\").innerHTML = \"<tt>".$new_str."</tt>\"</script>\n";
    flush();
    $cnt++;
  }

  $o = $keys[$i];
  $results[$o][ARCHIVE_SIZE] = "OFF";
  /*
  $tres = $cfg["SERVER_RESULTS_DIR"]."/".$o."/total_t_res.ar";
  if (file_exists($tres)) {
     $results[$o][ARCHIVE_SIZE] = getIntergrationLength($tres);
  } else {
    $src = $results[$o]["SOURCE"];
    if (strlen($src) > 5) {
      $src = preg_replace("/^[JB]/","", $src);
      $tres = $cfg["SERVER_RESULTS_DIR"]."/".$o."/".$src."_t.ar";
      if (file_exists($tres)) {
         $results[$o][ARCHIVE_SIZE] = getIntergrationLength($tres);
      }
    }
  }*/

  #if ($results[$o][NFS_ARCHIVES] == YES) {
    #$cmd = "du -skL /nfs/apsr05/apsr/archives/".$o." | awk '{print \$1}'";
    #$lastline = exec($cmd, $array, $return_val);
    #$results[$o][ARCHIVE_SIZE] = ($lastline * 16);
  #}
}

echo "<br>\n";

###############################################################################

echo "DONE, formatting page<br>\n";
echo "</span>";

?>

<script type="text/javascript">
  document.getElementById("progress").innerHTML = "";
</script>

<table cellpadding=10>
  <tr>
    <td>
      <table class="datatable">
        <tr><th colspan=3>Legend</th></tr>
        <tr><td>Key</td><td>Count</td><td>Meaning</td></tr>
        <tr><td>F</td><td><?echo $obs_fail_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.failed existed</td></tr>
        <tr><td>r</td><td><?echo $obs_res_count?></td><td><?echo $cfg["SERVER_RESULTS_DIR"]?>/[OBS] existed</td></tr>
        <tr><td>a</td><td><?echo $obs_arc_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS] existed</td></tr>
        <tr><td>l</td><td><?echo $obs_lar_count?></td><td><?echo $cfg["CLIENT_ARCHIVE_DIR"]?>/[OBS] existed</td></tr>
        <tr><td>i</td><td><?echo $obs_inf_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.info existed</td></tr>
        <tr><td>f</td><td><?echo $obs_fin_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.finished existed</td></tr>
        <tr><td>x</td><td><?echo $obs_xfe_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.transferred existed</td></tr>
        <tr><td>d</td><td><?echo $obs_del_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.deleted existed</td></tr>
        <tr><td>t</td><td><?echo $obs_txt_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.txt existed</td></tr>
        <tr><td>p</td><td><?echo $obs_proc_count?></td><td><?echo $cfg["SERVER_ARCHIVE_DIR"]?>/[OBS]/obs.processing existed</td></tr>
        <tr><td>s</td><td><?echo $on_swin_count?></td><td>shrek210:/export/shrek210b/apsr/*/[OBS] existed</td></tr>
        <tr><td></td><td><?echo $obs_unk_count?></td><td>Number of unmatched obs.* files</td></tr>
      </table>
    </td>
    <td>

<table border=0 cellpadding=0 cellspacing=0 class='obstable' width="99%">
 <thead>
  <tr>
    <th colspan=2></th>
    <th colspan=<?echo count($params)?>></th>
    <th></th>
    <th colspan=4></th>
    <td class="active" colspan=2 align=center><b>OBS.</b></td>
    <td class="active" colspan=5 align=center><b>STATE</b></td>
    <th></th>
  </tr>
  <tr>
   <th></th>
   <th>UTC_START</th>
<?
for ($j=0; $j<count($params); $j++) {
  if ($params[$j] == "BANDWIDTH") 
    echo "    <th>BW</th>\n";
  else
    echo "    <th>".$params[$j]."</th>\n";
}
?>
   <th>Int</th>
   <th>F</th>
   <th>r</th>
   <th>a</th>
   <th>l</th>
   <td class="active"><b>i</b></td>
   <td class="active"><b>t</b></td>
   <td class="active"><b>p</b></td>
   <td class="active"><b>f</b></td>
   <td class="active"><b>x</b></td>
   <td class="active"><b>d</b></td>
   <td class="active"><b>s</b></td>
   <th>&nbsp;&nbsp;&nbsp;</th>
  </tr>
 </thead>
 <tbody style="height: 700px; overflow: auto; overflow-x: hidden;">
<?

$keys = array_keys($results);
sort($keys);
$num = 0;
$class = "active";

for ($i=0; $i<count($keys); $i++) {

  $o = $keys[$i];
  $d = $results[$o];

  echo "  <tr>\n";

  /* COUNT */
  echo "    <td class=\"".$class."\">".$num."</td>\n";

  /* UTC_START */
  echo "    <td class=\"".$class."\"><a href='/apsr/result.php?utc_start=".$o."'>".$o."</a></td>\n";
   
  /* PARAMS */
  for ($j=0; $j<count($params); $j++) {
    if ($d[$params[$j]] == "") 
      $d[$params[$j]] = "&nbsp;";
    echo "    <td class=\"".$class."\">".$d[$params[$j]]."</td>\n";
  }

  /* ARCHIVE SIZE*/
  echo "    <td class=\"".$class."\" style='text-align: right'>".$d[ARCHIVE_SIZE]."</td>\n";

  /* OBS_FAILED */
  echo "    ".statusTD($d[OBS_FAIL],$class,"&nbsp;")."\n";

  /* NFS_RESULTS */
  echo "    ".statusTD($d[NFS_RESULTS],$class,"&nbsp;")."\n";

  /* NFS_ARCHIVES */
  echo "    ".statusTD($d[NFS_ARCHIVES],$class,"&nbsp;")."\n";

  /* LFS_ARCHIVES */
  echo "    ".statusTD(($d[LFS_ARCHIVES]>0) ? "Y" : "N",$class,$d[LFS_ARCHIVES])."\n";

  /* OBS_INFO */
  echo "    ".statusTD($d[OBS_INFO],$class,"&nbsp;")."\n";

  /* OBS_TXT */
  echo "    ".statusTD($d[OBS_TXT],$class,"&nbsp;")."\n";

  /* OBS_PROCESSING */
  echo "    ".statusTD($d[OBS_PROC],$class,"&nbsp;")."\n";

  /* OBS_FINSIHED */
  echo "    ".statusTD($d[OBS_FINI],$class,"&nbsp;")."\n";

  /* OBS_TRANSFERRED */
  echo "    ".statusTD($d[OBS_XFER],$class,"&nbsp;")."\n";

  /* OBS_DELETED */
  echo "    ".statusTD($d[OBS_DEL],$class,"&nbsp;")."\n";

  /* ON_SWIN */
  echo "    ".statusTD($d[ON_SWIN],$class,"&nbsp;")."\n";

  echo "    <td></td>\n";
  echo "  </tr>\n";
  $num++;
}

?>
 </tbody>
</table>

</td></tr>
</table>


</body>
</html>

<?

function statusTD($on, $class, $text) {

  if ($text === "") {
    $text = "&nbsp;";
  }

  if ($on == YES) {
    return "<td width=10px bgcolor='lightgreen' class='".$class."'>".$text."</td>";
  } else if ($on == MAYBE) {
    return "<td width=10px bgcolor='yellow' class='".$class."'>".$text."</td>";
  } else if ($on == NO) {
    return "<td width=10px bgcolor='red' class='".$class."'>".$text."</td>";
  } else { 
    return "<td width=10px class='".$class."'>".$text."</td>";
  } 

}


function getRemoteListing($user, $host, $dir, $results) {

  # Return a listing in the format: UTC_START BAND [FILES OF SIGNIFICANCE]
  $cmd = "ssh -l ".$user." ".$host." \"web_apsr_local_observations.pl\"";

  $array = array();
  $lastline = exec($cmd, $array, $return_val);

  for ($i=0; $i<count($array); $i++) {

    $a = split("/", $array[$i]);

    $o = $a[1];
    $b = $a[2];

    if (! array_key_exists($o, $results)) {
      $results[$o] = array();
      $results[$o][OBS] = 0;
      $results[$o][INL] = 0;
      $results[$o][STS_COUNT] = 0;
      $results[$o][STP_COUNT] = 0;
    }
    if (! array_key_exists($b, $results[$o])) {
      $results[$o][$b] = array();
      $results[$o][$b]["sts"] = 0;
      $results[$o][$b]["stp"] = 0;
    }
    if ($a[3] == "obs.start") {
      $results[$o][OBS]++;
    } else if ($a[3] == "sent.to.swin") {
      $results[$o][$b][STS] = 1;
      $results[$o][STS_COUNT] += 1;
    } else if ($a[3] == "sent.to.parkes") {
      $results[$o][$b][STP] = 1;
      $results[$o][STP_COUNT] += 1;
    } else if ($a[3] == "error.to.swin") {
      $results[$o][$b][ETS] = 1;
    } else if ($a[3] == "error.to.parkes") {
      $results[$o][$b][ETP] = 1;
    } else if (($a[3] == "integrated.ar") || (substr($a[3], ".fil") !== FALSE)) {
      $results[$o][INL]++;
    } else {
      echo "getRemoteListing: ".$a." was unmatched<BR>\n";
    }
  }

  return $results;



}

?>
