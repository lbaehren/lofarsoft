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
$apsr01_dir = "/nfs/apsr01/bpsr/archives";

if (isset($_GET["current_day"])) {
  $current_day = $_GET["current_day"];
} else {
  $current_day = "all";
}


# Get the listing of archives on tape @ swinburne
$cmd = "ssh pulsar@shrek211 \"cat ~/bookkeeping/tape_archiver/files.db | grep '01 HRA' | sort\" | awk -F/ '{print $1}'";
$swin_on_tape = array();
$lastline = exec($cmd, $swin_on_tape, $return_var);

# Get the listing of archives on tape @ parkes
$cmd = "ssh pulsar@jura \"cat /psr1/hitrun/bookkeeping/tape_archiver/files.db | grep '01 HRE' | sort\" | awk -F/ '{print $1}'";
$parkes_on_tape = array();
$lastline = exec($cmd, $parkes_on_tape, $return_var);

# Get the listing of archives in from_parkes @ swinburne
$cmd = "ssh pulsar@shrek211 'ls /nfs/cluster/shrek210a > /dev/null; ls /nfs/cluster/shrek210b > /dev/null; ls /nfs/cluster/shrek211a > /dev/null; ls /nfs/cluster/shrek21??/from_parkes | grep 200'";
$swin_from_parkes = array();
$lastline = exec($cmd, $swin_from_parkes, $return_var);

# Get the listing of archives in from_pakes @ parkes
$cmd = "ls /nfs/apsr14 > /dev/null; ls /nfs/apsr15 > /dev/null; ls /nfs/apsr15 > /dev/null; ls /nfs/apsr16 > /dev/null; ls /nfs/apsr1?/bpsr/from_parkes | grep 200";
$parkes_from_parkes = array();
$lastline = exec($cmd, $parkes_from_parkes, $return_var);


$archives = getArchivesArray($archives_dir, $results_dir, $apsr01_dir);

$keys = array_keys($archives);
$days = array("all");

# Now make a drop down list of each day
for ($i=0; $i<count($keys); $i++) {
  $day = substr($keys[$i],0,10);
  if (! in_array($day, $days) ) {
    array_push($days, $day);
  }
}


$num_results = count($keys);

?>

<html>

<?

$title = "BPSR | Archives Status";
include("header_i.php");

?>

<body>
  <!--  Load tooltip module -->
  <script type="text/javascript" src="/js/wz_tooltip.js"></script>
<? 
$text = "Recent Results";
include("banner.php");


?>
<center>

<table cellpadding=10>
  <tr>
    <td valign=top align=center>
      <form name="day_select" action="archives.php" method="GET">
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
      </form>
    </td>
    <td>

<table border=1 cellpadding=3 class="datatable">
<tr>
  <th colspan=6></th>
  <th colspan=2>Swin</th>
  <th colspan=2>Parkes</th>
</tr>
<tr>
  <th>Source</th>
  <th>UTC Start</th>
  <th>Status</th>
  <th>Swin status</th>
  <th>Parkes status</th>
  <th>obs.fin</th>
  <th>Sent</th>
  <th>Tape</th>
  <th>Sent</th>
  <th>Tape</th>
</tr>
<?

$keys = array_keys($archives);

for ($i=0; $i < count($keys); $i++) {

  if (($current_day == "all") || (strpos($keys[$i], $current_day) !== FALSE)) {

    $status_string = "";
    $status_swin = "";
    $status_parkes = "";

    $header = array();
    $header_file = $archives[$keys[$i]]["obs.info"];

    $data = $archives[$keys[$i]];

    if (file_exists($header_file)) {
      $header = getConfigFile($header_file,TRUE);
    }

    $nbeams = $data["nbeams"];

    $freq_keys = array_keys($archives[$keys[$i]]);
    $url = "/bpsr/result.php?utc_start=".$keys[$i]."&imagetype=bp";
    $mousein = "onmouseover=\"Tip('<img src=\'".$images[$keys[$i]][0]["bp_400x300"]."\' width=400 height=300>')\"";
    $mouseout = "onmouseout=\"UnTip()\"";

    $of  = ($archives[$keys[$i]]["obs.finalized"])*2;
    $sts = $archives[$keys[$i]]["sent.to.swin"];
    $stp = $archives[$keys[$i]]["sent.to.parkes"];
    $ets = $archives[$keys[$i]]["error.to.swin"];
    $etp = $archives[$keys[$i]]["error.to.parkes"];
    $ots = in_array($keys[$i], $swin_on_tape);
    $otp = in_array($keys[$i], $parkes_on_tape);
    $fps = in_array($keys[$i], $swin_from_parkes);
    $fpp = in_array($keys[$i], $parkes_from_parkes);

    if ($sts) {
      $to_s = 2;  # sent
    } else if ($ets) {
      $to_s = 0;  # error
      $status_swin = "Send Error";
    } else {
      $to_s = 1;  # pending
    }

    if ($stp) {
      $to_p = 2;  # sent
    } else if ($etp) {
      $to_p = 0;  # error
      $status_parkes = "Send Error";
    } else {
      $to_p = 1;  # pending
    }

    if ($ots) {
      $on_s = 2;  # on a tape
    } else if ($fps) {
      $on_s = 1;  # in from_parkes
    } else {
      $on_s = 0;  # not on tape or in from_parkes
    }

    if ($otp) {
      $on_p = 2;  # on a tape
    } else if ($fpp) {
      $on_p = 1;  # in from_parkes
    } else {
      $on_p = 0;  # not on tape or in from_parkes
    }

    # Set the status strings to something useful
    if ($of==2 && $to_s==2 && $to_p=2 && $on_s==2 && $on_p==2) {
      $status_string = "FINALIZED";
    }
    if ($to_s==2 && $on_s==2)
      $status_swin = "Archived";

    if ($to_p==2 && $on_p==2)
      $status_parkes = "Archived";

    if ($to_s==1 && $on_s==1)
      $status_swin = "Sending";

    if ($to_p==1 && $on_p==1)
      $status_parkes = "Sending";

    if ($to_s==1 && $on_s==0)
      $status_swin = "Send";

    if ($to_p==1 && $on_p==0)
      $status_parkes = "Send";

    echo "  <tr class=\"new\">\n";

    /* SOURCE */
    echo "    <td>".$header["SOURCE"]."</td>\n";

    /* UTC_START */
    echo "    <td>".$keys[$i]."</td>\n";

    /* OVERALL STATUS*/
    echo "    <td align=center>".$status_string."</td>\n";
    echo "    <td align=center>".$status_swin."</td>\n";
    echo "    <td align=center>".$status_parkes."</td>\n";

    echo "    <td align=center>".statusLight($of)."</td>\n";
    echo "    <td align=center>".statusLight($to_s)."</td>\n";
    echo "    <td align=center>".statusLight($on_s)."</td>\n";
    echo "    <td align=center>".statusLight($to_p)."</td>\n";
    echo "    <td align=center>".statusLight($on_p)."</td>\n";


    echo "  </tr>\n";
  }

}
?>
</table>

</td></tr>
</table>

</body>
</html>

</center>
<?

function getArchivesArray($a_dir, $r_dir, $n_dir) {

  $results = array();

  $obs_a = getSubDirs($a_dir);
  $obs_r = getSubDirs($r_dir);
  $obs_n = getSubDirs($n_dir);

  $obs = array_merge($obs_a, $obs_r, $obs_n);

  ksort($obs);

  /* For each observation get a list of frequency channels present */   
  for ($i=0; $i<count($obs); $i++) {

    $results[$obs[$i]] = array();

    if (file_exists($r_dir."/".$obs[$i]."/obs.info")) {
      $results[$obs[$i]]["obs.info"] = $r_dir."/".$obs[$i]."/obs.info";
    }
    if (file_exists($a_dir."/".$obs[$i]."/obs.info")) {
      $results[$obs[$i]]["obs.info"] = $a_dir."/".$obs[$i]."/obs.info";
    }
    if (file_exists($a_dir."/".$obs[$i]."/sent.to.parkes")) {
      $results[$obs[$i]]["sent.to.parkes"] = 1;
    } else {
      $results[$obs[$i]]["sent.to.parkes"] = 0;
    }
    if (file_exists($a_dir."/".$obs[$i]."/sent.to.swin")) {
      $results[$obs[$i]]["sent.to.swin"] = 1;
    } else {
      $results[$obs[$i]]["sent.to.swin"] = 0;
    }
    if (file_exists($r_dir."/".$obs[$i]."/obs.finalized")) {
      $results[$obs[$i]]["obs.finalized"] = 1;
    } else {
      $results[$obs[$i]]["obs.finalized"] = 0;
    }    
    if (file_exists($a_dir."/".$obs[$i]."/error.to.swin")) {
      $results[$obs[$i]]["error.to.swin"] = 1;
    } else {
      $results[$obs[$i]]["error.to.swin"] = 0;
    }    
    if (file_exists($a_dir."/".$obs[$i]."/error.to.parkes")) {
      $results[$obs[$i]]["error.to.parkes"] = 1;
    } else {
      $results[$obs[$i]]["error.to.parkes"] = 0;
    }    
  } 
  return $results;
}

function statusLight($on) {

  if ($on == 2) {
    return "<img src='/images/green_light.png'>";
  } else if ($on == 1) {
    return "<img src='/images/yellow_light.png'>";
  } else {
    return "<img src='/images/red_light.png'>";
  }
}

function getRemoteListing() {

}

?>
