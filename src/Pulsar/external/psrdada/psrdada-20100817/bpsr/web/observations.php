<?PHP
include("definitions_i.php");
include("functions_i.php");
include("bpsr_functions_i.php");

ini_set("memory_limit","128M");

define(OBF, 0);   # obs.finished
define(OBT, 1);   # obs.txt
define(OBP, 2);   # obs.problem
define(OBX, 3);   # obs.transferred
define(OBA, 4);   # obs.archived
define(OBD, 5);   # obs.delted
define(OBI, 6);   # obs.info
define(OBS, 7);   # obs.start
define(SRC, 8);   # souce name
define(SRV, 9);   # flag for survey pointing 
define(STS, 10);   # sent.to.swin
define(STP, 11);   # sent.to.parkes
define(OTS, 12);   # on.tape.swin
define(OTP, 13);   # on.tape.aparkes
define(ETS, 14);   # error.to.swin
define(ETP, 15);   # error.to.parkes
define(INA, 16);   # in /nfs/archives/bpsr
define(INR, 17);   # in /nfs/results/bpsr
define(INL, 18);   # in /lfs/data0/bpsr/archives
define(NBM, 19);   # number of beams

define(STS_COUNT, 20);
define(STP_COUNT, 21);

# Get the system configuration (dada.cfg)
$cfg = getConfigFile(SYS_CONFIG,TRUE);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

$archives_dir = $cfg["SERVER_ARCHIVE_DIR"];
$results_dir = $cfg["SERVER_RESULTS_DIR"];
$apsr01_dir = "/nfs/apsr01/bpsr/archives";
$staging_suffix = "/P630/staging_area";
$on_tape_suffix = "/P630/on_tape";
$pulsars_suffix = "/P630/pulsars";

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

if (isset($_GET["show_test"])) {
  $show_test = $_GET["show_test"];
} else {
  $show_test = "yes";
}

if (isset($_GET["show_survey"])) {
  $show_survey = $_GET["show_survey"];
} else {
  $show_survey = "yes";
}

if (isset($_GET["show_finalized"])) {
  $show_finalized = $_GET["show_finalized"];
} else {
  $show_finalized = "yes";
}

if (isset($_GET["show_archived"])) {
  $show_archived = $_GET["show_archived"];
} else {
  $show_archived = "yes";
}

if (isset($_GET["show_errors_only"])) {
  $show_errors_only = $_GET["show_errors_only"];
} else {
  $show_errors_only = "no";
}

if (isset($_GET["show_beams"])) {
  $show_beams = $_GET["show_beams"];
} else {
  $show_beams = "no";
}


?>
<html>
<head>
  <title>BPSR | Observations</title>
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

$text = "Mega Archives Page";
include("banner.php");

echo "<center>\n";

# Get the disk listing information
$num_swin_dirs = $cfg["NUM_SWIN_DIRS"];
$num_parkes_dirs = $cfg["NUM_PARKES_DIRS"];

$swin_dirs = array();
$parkes_dirs = array();
$swin_db = array();
$parkes_db = array();

for ($i=0; $i<$num_swin_dirs; $i++) {
  $arr = split(":",$cfg["SWIN_DIR_".$i]);
  $swin_dirs[$i] = array();
  $swin_dirs[$i]["user"] = $arr[0];
  $swin_dirs[$i]["host"] = $arr[1];
  $swin_dirs[$i]["disk"] = $arr[2];
}

for ($i=0; $i<$num_parkes_dirs; $i++) {
  $arr = split(":",$cfg["PARKES_DIR_".$i]);
  $parkes_dirs[$i] = array();
  $parkes_dirs[$i]["user"] = $arr[0];
  $parkes_dirs[$i]["host"] = $arr[1];
  $parkes_dirs[$i]["disk"] = $arr[2];
}

# Get the bookkeeping db information
$arr = split(":",$cfg["SWIN_DB_DIR"]);
$swin_db["user"] = $arr[0];
$swin_db["host"] = $arr[1];
$swin_db["dir"]  = $arr[2];
$swin_db["file"] = $arr[2]."/files.P630.db";

$arr = split(":",$cfg["PARKES_DB_DIR"]);
$parkes_db["user"] = $arr[0];
$parkes_db["host"] = $arr[1];
$parkes_db["dir"]  = $arr[2];
$parkes_db["file"] = $arr[2]."/files.P630.db";

echo "<span id=\"progress\">\n";

echo "Retreiving swin files.db<br>\n";
flush();
$array = array();
$cmd = "ssh -l ".$swin_db["user"]." ".$swin_db["host"]." \"cat ".$swin_db["file"]." | sort\" | awk '{print $1\"/\"$2}'";
$lastline = exec($cmd, $array, $return_var);
$swin_files_db = array();
$swin_files_db_count = array();
for ($i=0; $i<count($array); $i++) {
  $arr = split("/", $array[$i]);
  $swin_files_db[$arr[0]] .= $arr[1]." ";
  if (!array_key_exists($arr[0],$swin_files_db_count)) $swin_files_db_count[$arr[0]] = 0;
  $swin_files_db_count[$arr[0]] += 1;
}

echo "Retreiving parkes files.db<br>\n";
flush();
$array = array();
$cmd = "ssh -l ".$parkes_db["user"]." ".$parkes_db["host"]." \"cat ".$parkes_db["file"]." | sort\" | awk '{print $1\"/\"$2}'";
$lastline = exec($cmd, $array, $return_var);
$parkes_files_db = array();
$parkes_files_db_count = array();
for ($i=0; $i<count($array); $i++) {
  $arr = split("/", $array[$i]);
  $parkes_files_db[$arr[0]] .= $arr[1]." ";
  if (!array_key_exists($arr[0], $parkes_files_db_count)) $parkes_files_db_count[$arr[0]] = 0;
  $parkes_files_db_count[$arr[0]] += 1;
}

$beam_dir_find_suffix = "find -mindepth 2 -maxdepth 2 -type d' | awk '{print substr($1,3)}' | sort";

# Get the swin staging_area listing 
$swin_staging = array();
$swin_staging_count = array();
for ($i=0; $i<$num_swin_dirs; $i++) {
  echo "Retreiving listing of ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." ".$swin_dirs[$i]["disk"].$staging_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." 'cd ".$swin_dirs[$i]["disk"].$staging_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; (($return_var == 0) && ($j<count($array))); $j++) {
    $arr = split("/", $array[$j]);
    $swin_staging[$arr[0]] .= $arr[1]." ";
    if (!array_key_exists($arr[0], $swin_staging_count)) $swin_staging_count[$arr[0]] = 0;
    $swin_staging_count[$arr[0]] += 1;
  }
}

# Get the parkes staging_area listing 
$parkes_staging = array();
$parkes_staging_count = array();
for ($i=0; $i<$num_parkes_dirs; $i++) {
  echo "Retreiving listing of ".$parkes_dirs[$i]["user"]." ".$parkes_dirs[$i]["host"]." ".$parkes_dirs[$i]["disk"].$staging_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$parkes_dirs[$i]["user"]." ".$parkes_dirs[$i]["host"]." 'cd ".$parkes_dirs[$i]["disk"].$staging_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; (($return_var == 0) && ($j<count($array))); $j++) {
    $arr = split("/", $array[$j]);
    $parkes_staging[$arr[0]] .= $arr[1]." ";
    if (!array_key_exists($arr[0], $parkes_staging_count)) $parkes_staging_count[$arr[0]] = 0;
    $parkes_staging_count[$arr[0]] += 1;
  }
}

/*
# swin "on_tape"
$swin_on_tape = array();
$swin_on_tape_count = array();
for ($i=0; $i<$num_swin_dirs; $i++) {
  echo "Retreiving listing of ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." ".$swin_dirs[$i]["disk"].$on_tape_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." 'cd ".$swin_dirs[$i]["disk"].$on_tape_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split("/", $array[$j]);
    $swin_on_tape[$arr[0]] .= $arr[1]." ";
    if (!array_key_exists($arr[0], $swin_on_tape_count)) $swin_on_tape_count[$arr[0]] = 0;
    $swin_on_tape_count[$arr[0]] += 1; 
  }
}


# Parkes "on_tape"
$parkes_on_tape = array();
$parkes_on_tape_count = array();
for ($i=0; $i<$num_parkes_dirs; $i++) {
  echo "Retreiving listing of ".$parkes_dirs[$i]["user"]." ".$parkes_dirs[$i]["host"]." ".$parkes_dirs[$i]["disk"].$on_tape_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$parkes_dirs[$i]["user"]. " ".$parkes_dirs[$i]["host"]." 'cd ".$parkes_dirs[$i]["disk"].$on_tape_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split("/", $array[$j]);
    $parkes_on_tape[$arr[0]] .= $arr[1]." ";
     if (!array_key_exists($arr[0], $parkes_on_tape_count)) $parkes_on_tape_count[$arr[0]] = 0;
     $parkes_on_tape_count[$arr[0]] += 1; 
  }
}
*/

# Get the listing of archives in pulsars @ swin
$swin_pulsars = array();
$swin_pulsars_count = array();
for ($i=0; $i<$num_swin_dirs; $i++) {
  echo "Retreiving listing of ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." ".$swin_dirs[$i]["disk"].$pulsars_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$swin_dirs[$i]["user"]." ".$swin_dirs[$i]["host"]." 'cd ".$swin_dirs[$i]["disk"].$pulsars_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split("/", $array[$j]);
    $swin_pulsars[$arr[0]] .= $arr[1]." ";
     if (!array_key_exists($arr[0], $swin_pulsars_count)) $swin_pulsars_count[$arr[0]] = 0;
     $swin_pulsars_count[$arr[0]] += 1;
  }
}

# Get the listing of archives in pulsars @ parkes
$parkes_pulsars = array();
$parkes_pulsars_count = array();
for ($i=0; $i<$num_parkes_dirs; $i++) {
  echo "Retreiving listing of ".$parkes_dirs[$i]["user"]." ".$parkes_dirs[$i]["host"]." ".$parkes_dirs[$i]["disk"].$pulsars_suffix."<br>\n";
  flush();
  $cmd = "ssh -l ".$parkes_dirs[$i]["user"]." ".$parkes_dirs[$i]["host"]." 'cd ".$parkes_dirs[$i]["disk"].$pulsars_suffix."; ".$beam_dir_find_suffix;
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split("/", $array[$j]);
    $parkes_pulsars[$arr[0]] .= $arr[1]." ";
     if (!array_key_exists($arr[0], $parkes_pulsars_count)) $parkes_pulsars_count[$arr[0]] = 0;
     $parkes_pulsars_count[$arr[0]] += 1;
  }
}


echo "Retreiving local archives listing<br>\n";
flush();
$archives = getArchivesArray($archives_dir, $results_dir, $apsr01_dir);

# Try to make a complete list of archives
$swin_files_db_keys = array_keys($swin_files_db);
$parkes_files_db_keys = array_keys($parkes_files_db);
$archives_keys = array_keys($archives);

$all_archives = array_merge($swin_files_db_keys, $parkes_files_db_keys, $archives_keys);

unset($swin_files_db_keys);
unset($parkes_files_db_keys);
unset($archives_keys);

$keys = array_unique($all_archives);
unset($all_archives);
sort($keys);
$num_results = count($keys);

$days = array("all");

# Now make a drop down list of each day
for ($i=0; $i<count($keys); $i++) {
  $day = substr($keys[$i],0,10);
  if (! in_array($day, $days) ) {
    array_push($days, $day);
  }
}


echo "DONE, formatting page<br>\n";
echo "</span>";

?>
<script type="text/javascript">

  /* Toggle all the test observations as hidden/visible */
  function toggleRow(type) {
    var rows = document.getElementsByTagName('td');
    for (i=0; i<rows.length; i++) {
      if (rows[i].id == type) {
        if (rows[i].className == "inactive") {
          rows[i].className = "active";
        } else {
          rows[i].className = "inactive";
        }
      } else {
        // do nothing
      }
    }
  }

  document.getElementById("progress").innerHTML = "";

</script>

<table cellpadding=10>
  <tr>
    <td valign=top align=center>
      <form name="day_select" action="observations.php" method="GET">
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
        Show Test Obs? <input type="radio" name="show_test" value="yes"<? if($show_test == "yes") echo " checked"?> onClick="toggleRow('testobs')">Yes</input>
                       <input type="radio" name="show_test" value="no"<? if($show_test == "no") echo " checked"?> onClick="toggleRow('testobs')">No</input>
        </td></tr>

        <tr><td>
        Show Survey Obs? <input type="radio" name="show_survey" value="yes"<? if($show_survey == "yes") echo " checked"?>>Yes</input>
                         <input type="radio" name="show_survey" value="no"<? if($show_survey == "no") echo " checked"?>>No</input>
        </td></tr>

        <tr><td>
        Show Finalized Obs? <input type="radio" name="show_finalized" value="yes"<? if($show_finalized == "yes") echo " checked"?> onClick="toggleRow('finalized')">Yes</input>
                            <input type="radio" name="show_finalized" value="no"<? if($show_finalized == "no") echo " checked"?> onClick="toggleRow('finalized')">No</input>
        </td></tr> 

        <tr><td>
        Show Fully Archived? <input type="radio" name="show_archived" value="yes"<? if($show_archived == "yes") echo " checked"?> onClick="toggleRow('archived')">Yes</input>
                             <input type="radio" name="show_archived" value="no"<? if($show_archived == "no") echo " checked"?> onClick="toggleRow('archived')">No</input>
        </td></tr>

        <tr><td>
        Show Errors Only? <input type="radio" name="show_errors_only" value="yes"<? if($show_errors_only == "yes") echo " checked"?>>Yes</input>
                          <input type="radio" name="show_errors_only" value="no"<? if($show_errors_only == "no") echo " checked"?>>No</input>
        </td></tr> 

        <tr><td>
        Show Beams? <input type="radio" name="show_beams" value="yes"<? if($show_beams== "yes") echo " checked"?>>Yes</input>
                    <input type="radio" name="show_beams" value="no" <? if($show_beams == "no") echo " checked"?>>No</input>
        </td></tr> 

        <tr><td colspan=2 align=center><input type="button" value="Submit"></input></td></tr>

      </table> 
      </form>

      <table class="datatable">
        <tr><th colspan=3>Legend</th></tr>
        <tr><td>Key</td><td>Value</td><td>Meaning</td></tr>
        <tr><td>obs.</td><td>i</td><td>obs.info</td></tr>
        <tr><td>obs.</td><td>f</td><td>obs.finished</td></tr>
        <tr><td>obs.</td><td>d</td><td>obs.deleted</td></tr>
        <tr><td>obs.</td><td>p</td><td>obs.problem</td></tr>
        <tr><td>obs.</td><td>x</td><td>obs.transferred</td></tr>
        <tr><td>obs.</td><td>a</td><td>obs.archived</td></tr>
        <tr><td>obs.</td><td>t</td><td>obs.txt</td></tr>
        <tr><td>Status</td><td>ARCHIVED</td><td>Backed up to tape at both Parkes &amp; Swinburne</td></tr>
        <tr><td>Status</td><td>FINALIZED</td><td>ARCHIVED, and has been deleted from apsr local disks</td></tr>
        <tr><td>Loc</td><td>A</td><td>Existed in /nfs/archives/bpsr</td></tr>
        <tr><td>Loc</td><td>R</td><td>Existed in /nfs/results/bpsr</td></tr>
        <tr><td>Loc</td><td>L</td><td>Existed in /nfs/apsr01/bpsr/archives/</td></tr>
        <tr><td>N</td><td></td><td>Number of beams existing</td></tr>
        <tr><td>S</td><td><img src="/images/green_light.png"></td><td>Obs marked as sent.to.&lt;loc&gt;</td></tr>
        <tr><td>S</td><td><img src="/images/yellow_light.png"></td><td>Obs ready to send</td></tr>
        <tr><td>S</td><td><img src="/images/red_light.png"></td><td>Obs marked as error.to.&lt;loc&gt;</td></tr>
        <tr><td>T</td><td><img src="/images/green_light.png"></td><td>Obs written to tape</td></tr>
        <tr><td>T</td><td><img src="/images/yellow_light.png"></td><td>Obs received, but not yet written</td></tr>
        <tr><td>T</td><td><img src="/images/red_light.png"></td><td>Obs received, not xfer.complete</td></tr>
        <tr><td>T</td><td></td><td>Obs not yet received</td></tr>
        <tr><td>D</td><td><img src="/images/green_light.png"></td><td>Obs exists in on_tape dir</td></tr>
        <tr><td>D</td><td></td><td>Obs does not exist in on_tape dir</td></tr>
      </table>

    </td>
    <td>

<table border=0 cellpadding=0 cellspacing=0 class='obstable' width="99%">
 <thead>
  <tr>
   <th colspan=7></th>
   <th colspan=7>obs.?</th>
   <th></th>
   <th colspan=3>Swin</th>
   <th colspan=3>Parkes</th>
   <th colspan=3>Loc</th>
   <th>&nbsp;&nbsp;&nbsp;&nbsp;</th><!-- for scrollbar-->
  </tr>
  <tr>
   <th>Num</th>
   <th>Source</th>
   <th>UTC Start</th>
   <th>Type</th>
   <th>Status</th>
   <th>Swin status</th>
   <th>Parkes status</th>
   <th>i</th>
   <th>f</th>
   <th>d</th>
   <th>p</th>
   <th>x</th>
   <th>t</th>
   <th>a</th>
   <th>N</th>
   <th>S</th>
   <th>D</th>
   <th>T</th>
   <th>S</th>
   <th>D</th>
   <th>T</th>
   <th>A</th>
   <th>R</th>
   <th>L</th>
   <th></th>
  </tr>
 </thead>
 <tbody style="height: 700px; overflow: auto; overflow-x: hidden;">
<?

$num = 1;

$swin_unsent = 0;
$parkes_unsent = 0;
$swin_unarchived = 0;
$parkes_unarchived = 0;

for ($i=0; $i<count($keys); $i++) {

  $k = $keys[$i];

  $show_this_obs = 1;
  $class = "active";
  $id = "unknown";

  if (($current_day == "all") || (strpos($k, $current_day) !== FALSE)) {

    $status_string = "&nbsp;";
    $status_swin = "&nbsp;";
    $status_parkes = "&nbsp;";

    # array of the current obs
    $d = $archives[$k];

    #if ($d[SRV] == 0) {
    #  $id = "testobs";
    #  $status_string = "TESTOBS";
    #}

    /*
    $sts = 0;
    $stp = 0;
    if (($d[OBX] == 2) || ($d[OBA] == 2) || ($d[OBD] == 2)) {
      $sts = 1;
      if ($d[SRV] == 1) 
        $stp = 1;
    }*/

    $ets = $d[ETS];
    $etp = $d[ETP];
    $ots = array_key_exists($k, $swin_files_db);
    $ots_count = 0;
    if ($ots) 
      $ots_count = $swin_files_db_count[$k];

    $otp = array_key_exists($k, $parkes_files_db);
    $otp_count = 0;
    if ($otp) 
      $otp_count = $parkes_files_db_count[$k];

    $fps_count = 0;
    $fpp_count = 0;

    if ($d[SRV] == 1) {
      $fps = array_key_exists($k, $swin_staging);
      if ($fps) 
        $fps_count =  $swin_staging_count[$k];
      $fpp = array_key_exists($k, $parkes_staging);
      if ($fpp) 
        $fpp_count = $parkes_staging_count[$k];
    } else {
      $fps  = array_key_exists($k, $swin_pulsars);
      if ($fps) 
        $fps_count = $swin_pulsars_count[$k];
      $fpp = array_key_exists($k, $parkes_pulsars);
      if ($fpp) 
        $fpp_count = $parkes_pulsars_count[$k];
    }

    # determine if the obs has been sent to swin/parkes
    $sts = 0;
    $sts = 0;
    # if obs. transferred, archived or deleted, then assume its been sent
    if (($d[OBX] == 2) || ($d[OBA] == 2) || ($d[OBD] == 2)) {
      $sts = 1;
      $stp = 1;
    }
    
    # if its in the process of sending
    if ($d[OBF]==2) {
      if ($fps_count>=$d[NBM]) {
        $sts = 1;
      }
      if ($fpp_count>=$d[NBM]) {
        $stp = 1;
      }
    }

    $in_a = $d[INA];
    $in_r = $d[INR];
    $in_l = $d[INL];

    /*
    if ($d[STS_COUNT] == "") {
      $d[STS_COUNT] = "0";
    }
    if (($d[STP_COUNT] == "") && ($d[SRV]==1)) {
      $d[STP_COUNT] = "0";
    }*/

    # sent.to.swin flag
    if (($d[STS_COUNT]>=$d[NBM]) || ($d[OBX] == 2) || ($d[OBA] == 2) || ($d[OBD] == 2)) {
      $to_s = 2;
    } else if ($d[STS_COUNT] > 0) {
      $to_s = 1;
    } else if ($d[STS_COUNT] == 0) {
      $to_s = -1;
    }
    if ($ets) {
      $to_s = 0;
    }

    # sent.to.parkes flag
    if (($d[STP_COUNT]>=$d[NBM]) && (($d[OBX] == 2) || ($d[OBA] == 2) || ($d[OBD] == 2))) {
      $to_p = 2;
    } else if ($d[STP_COUNT] > 0) {
      $to_p = 1;
    } else if ($d[STP_COUNT] == 0) {
      $to_p = -1;
    }
    if ($etp) {
      $to_p = 0;
    }

    # SENT flag
    /*
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
    }*/

    # ON DISK flag
    if ($fps)
      $od_s = ($d[NBM] <= $fps_count) ? 2 : 1;
    else
      $od_s = -1;
    if ($fpp) 
      $od_p = ($d[NBM] <= $fpp_count) ? 2 : 1;
    else 
      $od_p = -1;

    # ON TAPE flag
    if ($ots)
      $on_s = 2;
    else 
      $on_s = ($ots_count > 0 ) ? 1 : -1;
    if ($otp)
      $on_p = 2;
    else
      $on_p = ($otp_count > 0) ? 1 : -1;

    if (($d[OBA]==2 || $d[OBD]==2) && $to_s==2 && $to_p==2 && $on_s==2 && $on_p==2) {
      if ($d[OBD]==2) {
        $status_string = "FINALIZED";
        $id = "finalized";
      } else if ($d[OBD]<0 && $in_l>=$d[NBM]) {
        $status_string = "ARCHIVED";
        $id = "archived";
      } else {
        $status_string = "UNSURE";
        $id = "unsure";
      }
    }

    if ($to_s==2 && $ots_count>=$d[NBM])
      $status_swin = "Archived";

    if ($to_p==2 && $otp_count>=$d[NBM])
      $status_parkes = "Archived";

    if (($to_s==1 || $to_s==-1) && ($od_s==-1 || $od_s==1)) {
      $status_swin = "Send [".($d[NBM] - ($ots_count + $fps_count))."]";
      $swin_unsent += ($d[NBM] - ($ots_count + $fps_count));
    }

    if (($to_p==1 || $to_p==-1) && ($od_p==-1 || $od_p==1)) {
      $status_parkes = "Send [".($d[NBM] - ($otp_count + $fpp_count))."]";
      $parkes_unsent += ($d[NBM] - ($otp_count + $fpp_count));
    }
    
    if ($to_s==2 && $od_s==2 && $d[SRV]==1) {
      $status_swin = "Archive";
      $swin_unarchived += $d[NBM];
    }

    if ($to_s==2 && $od_s==2 && $d[SRV]==0) {
      $status_swin = "Archive [P]";
    }

    if ($to_p==2 && $od_p==2 && $d[SRV]==1) {
      $status_parkes = "Archive";
      $parkes_unarchived += $d[NBM];
    }

    if ($to_p==2 && $od_p==2 && $d[SRV]==0) {
      $status_parkes = "Archive [P]";
      $parkes_unarchived += $d[NBM];
    }

    if ($to_s==2 && $od_s==1 && $d[SRV]==1) {
      $status_swin = "Archiving [".($d[NBM] - $ots_count)."]";
      $swin_unarchived += ($d[NBM] - $ots_count);
    }

    if ($to_s==2 && $od_s==1 && $d[SRV]==0) {
      $status_swin = "Archiving [P ".($d[NBM] - $ots_count)."]";
    }   

    if ($to_p==2 && $od_p==1 && $d[SRV]==1) {
      $status_parkes = "Archiving [".($d[NBM] - $otp_count)."]";
      $parkes_unarchived += ($d[NBM] - $otp_count);
    }

    if ($to_p==2 && $od_p==1 && $d[SRV]==0) {
      $status_parkes = "Archiving [P ".($d[NBM] - $otp_count)."]";
      $parkes_unarchived += ($d[NBM] - $otp_count);
    }

    if ($ets)
      $status_swin = "ERROR";


    # some consistency checking
    #if (($to_s==2) && ($on_s<0) && ($ots!=2) && ($id != "testobs")) {
    #  $status_swin = "Flag Error";
    #} 
    #if (($to_p==2) && ($on_p<0) && ($otp!=2) && ($id != "testobs")) {
    #  $status_parkes = "Flag Error";
    #}

    if (($status_string == "FINALIZED") && ($show_finalized == "no")) {
      $show_this_obs = 0;
    }
    if (($status_string == "ARCHIVED") && ($show_archived == "no")) {
      $show_this_obs = 0;
    }

    if (($d[SRV] != 1) && ($show_test == "no")) {
      $show_this_obs = 0;
    }

    if (($d[SRV] == 1) && ($show_survey == "no")) {
      $show_this_obs = 0;
    }

    if (($in_l == 0) && ($status_string == "FINALIZED")) {
      $in_lcol = 2;
    } else if (($in_l > 0) && ($in_l <= $d[NBM])) {
      $in_lcol = 1;
    } else {
      $in_lcol = 0;
    } 

    if ($show_errors_only == "yes") {
      # Show if any error state and the obs wasn't previously excluded
      if ((($d[OBF] == 0) || ($d[OBI] == 0) || ($to_s == 0) || ($on_s == 0) || 
          ($to_p == 0) || ($on_p == 0) || ($in_a == 0) || 
          ($in_r == 0)) && ($show_this_obs)) {
        $show_this_obs = 1;
      } else {
        $show_this_obs = 0;
      }
    } else {

    }

    if ($show_this_obs == 1) {

      echo "  <tr>\n";

      /* INDEX */
      echo "    <td id=\"".$id."\" class=\"".$class."\">".$num."</td>\n";

      /* SOURCE */
      echo "    <td id=\"".$id."\" class=\"".$class."\">".$d[SRC]."</td>\n";

      /* UTC_START */
      echo "    <td id=\"".$id."\" class=\"".$class."\">".$k."</td>\n";

      /* UTC_START */
      echo "    <td id=\"".$id."\" class=\"".$class."\">".(($d[SRV]) ? "S" : "P")."</td>\n";

      /* OVERALL STATUS*/
      if ($status_string == "FINALIZED") {
        $color = " bgcolor='lightgreen'";
      } else if ($status_string == "ARCHIVED") {
        $color = " bgcolor='#FFFF99;'";
      } else {
        $color = "";
      }
      echo "    <td align=center".$color." id=\"".$id."\" class=\"".$class."\">".$status_string."</td>\n";

      /* SWIN STATUS */
      if ($status_swin == "Archived") {
        $color = " bgcolor='lightgreen'";
      } else if ((strpos($status_swin,"Send [") !== FALSE) || (strpos($status_swin, "Archiv") !== FALSE)) {
        $color = " bgcolor='#FFFF99;'";
      } else if (($status_swin == "Send Error") || ($status_swin == "Flag Error")) {
        $color = " bgcolor='#FF3366;'";
      } else  {
        $color = "";
    }
      echo "    <td align=center".$color." id=\"".$id."\" class=\"".$class."\">".$status_swin."</td>\n";

      if ($status_parkes == "Archived") {
        $color = " bgcolor='lightgreen'";
      } else if ((strpos($status_parkes,"Send [") !== FALSE) || (strpos($status_parkes, "Archiv") !== FALSE)) {
        $color = " bgcolor='#FFFF99;'";
      } else if (($status_parkes == "Send Error") || ($status_parkes == "Flag Error")) {
        $color = " bgcolor='#FF3366;'";
      } else  {
        $color = "";
      } 
      echo "    <td align=center".$color." id=\"".$id."\" class=\"".$class."\">".$status_parkes."</td>\n";

      echo "    ".statusTD($d[OBI],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBF],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBD],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBP],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBX],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBT],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($d[OBA],$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD("-1",$id,$class,$d[NBM])."\n";
      echo "    ".statusTD($to_s,$id,$class,$d[STS_COUNT])."\n";
      echo "    ".statusTD($od_s,$id,$class,$fps_count)."\n";
      echo "    ".statusTD($on_s,$id,$class,$ots_count)."\n";
      echo "    ".statusTD($to_p,$id,$class,$d[STP_COUNT])."\n";
      echo "    ".statusTD($od_p,$id,$class,$fpp_count)."\n";
      echo "    ".statusTD($on_p,$id,$class,$otp_count)."\n";
      echo "    ".statusTD($in_a,$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($in_r,$id,$class,"&nbsp;")."\n";
      echo "    ".statusTD($in_lcol,$id,$class,$in_l)."\n";
      echo "    <td></td>\n";
      echo "  </tr>\n";
      $num++;
    }
  }
}
?>
 </tbody>
</table>

<table class='datatable'>
  <tr><th>Type</th><th>Swin</th><th>Parkes</th></tr>
<?
  echo "<tr><td>Unsent</td><td>".$swin_unsent."</td><td>".$parkes_unsent."</td></tr>\n";
  echo "<tr><td>Unarchived</td><td>".$swin_unarchived."</td><td>".$parkes_unarchived."</td></tr>\n";
?>
</table>

</td></tr>
</table>

</body>
</html>

<?

function getArchivesArray($a_dir, $r_dir, $n_dir) {

  $results = array();

  $obs_a = getSubDirs($a_dir);
  $obs_r = getSubDirs($r_dir);
  $obs_n = getSubDirs($n_dir);

  $obs_all = array_merge($obs_a, $obs_r, $obs_n);
  $obs_all = array_unique($obs_all);
  sort($obs_all);

  # Get all the obs_a subdirs count
  $obs_a_subdir_count = array();
  $cmd = "find ".$a_dir." -mindepth 2 -maxdepth 2 -type l | awk -F/ '{print $5\" \"$6}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_var);
  for ($j=0; $j<count($array); $j++) {
    $arr = split(" ", $array[$j]);
    if (!array_key_exists($arr[0], $obs_a_subdir_count)) $obs_a_subdir_count[$arr[0]] = 0;
    $obs_a_subdir_count[$arr[0]] += 1;
  }

  # Get the remote file listings soas to not tax NFS too much
  $user = "bpsr";
  $remote = array();
  for ($i=0; $i<=17; $i++) {
    $host = sprintf("apsr%02d",$i);
    echo "$host [".memory_get_usage(true)."]<br>\n";
    flush();
    $temp_array = getRemoteListing($user, $host, "/lfs/data0/bpsr/archives", $remote);
    $remote = $temp_array;

  }
  echo "<BR>\n";
  flush();

  $local = getObsDotListing("/nfs/archives/bpsr");

  $tot = count($obs_all);

  $cnt = 0;
  echo "0%";
  for ($i=0; $i<$tot; $i++) {
    if (floor(($i/$tot)*100) > $cnt) {
      echo ".";
      flush();
      $cnt++;
    }
  }
  echo "100%<br>\n";
  flush();

  $cnt = 0;
 
  echo "0%";
  flush();
 
  /* For each observation get a list of frequency channels present */   
  for ($i=0; $i<$tot; $i++) {

    if (floor(($i/$tot)*100) > $cnt) {
      echo ".";
      flush();
      $cnt++;
    }

    /* the current obs */
    $o = $obs_all[$i];

    $results[$o] = array();

    $results[$o][NBM] = $obs_a_subdir_count[$o];

    /* Check that the observation is listed in the local array */
    if (array_key_exists($o, $local)) {
      $results[$o][OBF] = $local[$o][OBF];
      $results[$o][OBX] = $local[$o][OBX];
      $results[$o][OBA] = $local[$o][OBA];
      $results[$o][OBD] = $local[$o][OBD];
      $results[$o][OBI] = $local[$o][OBI];
      $results[$o][OBP] = $local[$o][OBP];
      $results[$o][OBT] = $local[$o][OBT];
      $results[$o][SRC] = $local[$o][SRC];
      $results[$o][SRV] = (substr($results[$o][SRC],0,1) == "G") ? 1 : 0;
    }
 
    $results[$o][STS_COUNT] = 0; 
    $results[$o][STP_COUNT] = 0; 
    if (array_key_exists($o, $remote)) {
      $results[$o][STS_COUNT] = $remote[$o][STS_COUNT];
      $results[$o][STP_COUNT] = $remote[$o][STP_COUNT];
    }

    $results[$o][INA] = 0;
    if (in_array($o,$obs_a)) {
      $results[$o][INA] = 2;
    }
    $results[$o][INR] = 0;
    if (in_array($o,$obs_r)) {
      $results[$o][INR] = 2;
    }
    $results[$o][INL] = 0;
    if (array_key_exists($o, $remote)) {
      $results[$o][INL] = $remote[$o][INL];
    }

    # check for P630 as the Project ID (PID)
    #if ($results[$o][OBI] == 2) {
    #  $cmd = "grep ^PID /nfs/archives/bpsr/".$o."/obs.info | awk '{print $2}'";
    #  $lastline = exec($cmd, $array, $return_val);
    #  if ($lastline != "P630") {
    #    echo "removing $o $lastline<BR>\n";
    #    flush();
    #    unset($results[$o]);
    #  }
    #}

    # now remove the obs from the remote and local arrays for memory reasons 
    unset($remote[$o]); 
    unset($local[$o]); 
  }

  echo "100%<br>\n";
  flush();

  # check project ID's, removing all that are not P630
  $array = array();
  $cmd = "find /nfs/archives/bpsr/ -type f -maxdepth 2 -name 'obs.info' -print0 | xargs -0 grep ^PID | grep -v P630 | awk -F/ '{print $(NF-1)}' | sort";
  $lastline = exec($cmd, $array, $return_val);

  echo "Found ".count($array)." non P630 observations<BR>\n";
  flush();

  for ($i=0; $i<count($array); $i++) {
    echo "removing ".$array[$i]."<BR>\n";
    flush();
    unset($results[$array[$i]]);
  }

  return $results;
}

function statusTD($on, $id, $class, $text) {

  if ($text === "") {
    $text = "&nbsp;";
  }

  if ($on == 2) {
    return "<td width=10px bgcolor='lightgreen' id='".$id."' class='".$class."'>".$text."</td>";
  } else if ($on == 1) {
    return "<td width=10px bgcolor='yellow' id='".$id."' class='".$class."'>".$text."</td>";
  } else if ($on == 0) {
    return "<td width=10px bgcolor='red' id='".$id."' class='".$class."'>".$text."</td>";
  } else { 
    return "<td width=10px id='".$id."' class='".$class."'>".$text."</td>";
  } 

}

function statusLight($on) {

  if ($on == 2) {
    return "<img src='/images/green_light.png'>";
  } else if ($on == 1) {
    return "<img src='/images/yellow_light.png'>";
  } else if ($on == 0) {
    return "<img src='/images/red_light.png'>";
  } else {
    return "";
  }
}

#
# SSH to the remote machine and get listing information on each of the files we need
#
function getRemoteListing($user, $host, $dir, $results) {

  $cmd = "ssh -l ".$user." ".$host." \"cd ".$dir.";  find . -mindepth 3 ".
         "-maxdepth 3 -type f -name 'obs.start' -o -type f -name 'sent.to.*'".
         " -o -type f -name 'error.to.*' -o -type f -name 'integrated.ar'".
         "  -o -type f -name '*.fil'\" | sort";

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


#
# Gets a listing of all the obs. files in the dir's subdirs
#
function getObsDotListing($dir) {

  $cmd = "cd $dir; find -mindepth 2 -maxdepth 2 -type f -name 'obs.*' -o -name 'error.to.*'";
  $array = array();
  $lastline = exec($cmd, $array, $return_val);

  $results = array();

  for ($i=0; $i<count($array); $i++) {

    $a = split("/", $array[$i]); 
    $o = $a[1];
    $f = $a[2];

    if (! array_key_exists($o, $results)) {
      $results[$o] = array();
      $results[$o][OBF] = -1;
      $results[$o][OBT] = -1;
      $results[$o][OBP] = -1;
      $results[$o][OBX] = -1;
      $results[$o][OBA] = -1;
      $results[$o][OBD] = -1;
      $results[$o][OBI] = -1;
      $results[$o][ETS] = 0;
      $results[$o][ETP] = 0;
    }

    if (($f == "obs.finished") || ($f == "obs.finalized")){
      $results[$o][OBF] = 2;
    }
    if ($f == "obs.txt") {
      $results[$o][OBT] = 2;
    }
    if ($f == "obs.problem") {
      $results[$o][OBP] = 0;
    }
    if ($f == "obs.transferred") {
      $results[$o][OBX] = 2;
    }
    if ($f == "obs.archived") {
      $results[$o][OBA] = 2;
    }
    if ($f == "obs.deleted") {
      $results[$o][OBD] = 2;
    }
    if ($f == "obs.info") {
      $results[$o][OBI] = 2;
    }
    if ($f == "error.to.swin") {
      $results[$o][ETS] = 1;
    }
    if ($f == "error.to.parkes") {
      $results[$o][ETP] = 1;
    }
  }


  $cmd = "cd $dir; find . -maxdepth 2 -type f -name 'obs.info' -print0 | xargs -0 grep SOURCE | awk -F/ '{print $(NF-1)\" \"\$NF}' | awk '{print $1\" \"$3}'";
  $array = array();
  $lastline = exec($cmd, $array, $return_val);

  for ($i=0; $i<count($array); $i++) {
    $a = split(" ", $array[$i]);
    $o = $a[0];
    $s = $a[1];
    $results[$o][SRC] = $s; 
  }

  unset($array);

  return $results;
}

?>
