<?PHP

function getConfigFile($fname, $quiet=FALSE) {

  $fptr = @fopen($fname,"r");
  $returnArray = array();

  if (!$fptr) {
    if (!$quiet)
    echo "Could not open file: $fname for reading<BR>\n";
  } else {
    while ($line = fgets($fptr, 1024)) {
      // Search for comments and  remove them from the line to be parsed
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

function getRawTextFile($fname) {

  $fptr = @fopen($fname,"r");
  if (!$fptr) {
    return array("Could not open file: $fname for reading");
  } else {
    $returnArray = array();
    while ($line = fgets($fptr, 1024)) {
      // Remove newlines
      $line = chop($line);
      array_push($returnArray,$line);
    }
    return $returnArray;
  }
}


function getAllStatuses($pwc_config) {

  $num_pwc = $pwc_config["NUM_PWC"];

  for ($i=0; $i<$num_pwc; $i++) {

    $status["PWC_".$i."_STATUS"] = STATUS_OK;
    $status["PWC_".$i."_MESSAGE"] = "";
    $status["SRC_".$i."_STATUS"] = STATUS_OK;
    $status["SRC_".$i."_MESSAGE"] = "";
    $status["SYS_".$i."_STATUS"] = STATUS_OK;
    $status["SYS_".$i."_MESSAGE"] = "";

    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".pwc.warn";
    if (file_exists($fname)) {
      $status["PWC_".$i."_STATUS"] = STATUS_WARN;
      $status["PWC_".$i."_MESSAGE"] = getSingleStatusMessage($fname);
    }
    
    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".pwc.error";
    if (file_exists($fname)) {
      $status["PWC_".$i."_STATUS"] = STATUS_ERROR;
      $status["PWC_".$i."_MESSAGE"] =  getSingleStatusMessage($fname);
    }

    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".src.warn";
    if (file_exists($fname)) {
      $status["SRC_".$i."_STATUS"] = STATUS_WARN;
      $status["SRC_".$i."_MESSAGE"] = getSingleStatusMessage($fname);
    }
                                                                                                                                                           
    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".src.error";
    if (file_exists($fname)) {
      $status["SRC_".$i."_STATUS"] = STATUS_ERROR;
      $status["SRC_".$i."_MESSAGE"] =  getSingleStatusMessage($fname);
    }

    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".sys.warn";
    if (file_exists($fname)) {
      $status["SYS_".$i."_STATUS"] = STATUS_WARN;
      $status["SYS_".$i."_MESSAGE"] = getSingleStatusMessage($fname);
    }
                                                                                                                                                           
    $fname = STATUS_FILE_DIR."/".$pwc_config["PWC_".$i].".sys.error";
    if (file_exists($fname)) {
      $status["SYS_".$i."_STATUS"] = STATUS_ERROR;
      $status["SYS_".$i."_MESSAGE"] =  getSingleStatusMessage($fname);
    }
  }

  return $status;

}

function getAllServerStatuses($daemons) {

  $status = array();

  for ($i=0; $i<count($daemons); $i++) {

    $d = $daemons[$i];

    $status[$d."_STATUS"] = STATUS_OK;
    $status[$d."_MESSAGE"] = "";

    $fname = STATUS_FILE_DIR."/".$d.".warn";
    if (file_exists($fname)) {
      $status[$d."_STATUS"] = STATUS_WARN;
      $status[$d."_MESSAGE"] = getSingleStatusMessage($fname);
    }

    $fname = STATUS_FILE_DIR."/".$d.".error";
    if (file_exists($fname)) {
      $status[$d."_STATUS"] = STATUS_ERROR;
      $status[$d."_MESSAGE"] = getSingleStatusMessage($fname);
    }
  }

  return $status;

}

  


function getSingleStatusMessage($fname) {

  $result = "";

  if (file_exists($fname)) {

    $cmd = "tail -n 1 $fname";
    $result = rtrim(`$cmd`);
    
  } 

  return $result;

  /*$fptr = fopen($fname,"r");
  if (!$fptr) {
    echo "Could not open status file: $fname<BR>\n";
  } else {
    #echo "Opening $fname<BR>\n";
    $j = 0;
    while (!(feof($fptr))) {
      $string = rtrim(fgets($fptr));
      if (strlen($string) > 0) {
        $array[$j] = $string;
        $j++;
      }
    }
    fclose($fptr);
    // Roll back the j counter
    $j--;
  }
  return $array[$j];*/
}


function openSocket($host, $port, $timeout=2) {

  //create a socket
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if (!(socket_set_nonblock($socket))) {
    return array(0, "Unable to set nonblock on socket");
  }
  $time = time();

  while (!@socket_connect($socket, $host, $port)) {
    $err = socket_last_error($socket);
    if ($err == 115 || $err == 114) {
      if ((time() - $time) >= $timeout) {
        socket_close($socket);
        return array(0,"Connection timed out");
      }
      time_nanosleep(0,10000000);
      continue;
   
    }
    return array(0, socket_strerror($err));
  }                                                                                                                                        
  if (!(socket_set_block($socket))){
    return array(0, "Unable to set block on socket");
  } 
  return array ($socket,"ok");
}

function socketRead($socket) {

  if ($socket) {
    $string = socket_read ($socket, 8192, PHP_NORMAL_READ);
    if ($string == FALSE) {
      $string = "Error on socketRead()\n";
    }
    return $string;
  } else {
    return "socket closed before read\n";
  }
}

function socketWrite($socket, $string) {

  $bytes_to_write = strlen($string);
  $bytes_written = socket_write($socket,$string,$bytes_to_write);

  if ($bytes_written === FALSE) {
    echo "Error writing data with socket_write()<BR>\n";
    return -1;
  }
  if ($bytes_written != $bytes_to_write) {
    echo "Error, tried to write".$bytes_to_write.", but only ".$bytes_written." bytes were written<BR>\n";
    return -1;
  } 
  return $bytes_written;
}


/* Returns a list of all the unix groups this user is a member of */
function getProjects($user) {

  $string = exec("groups $user",$output,$return_var);
  $array = split(" ", $output[0]);

  for ($i=0;$i<count($array); $i++) {
    $groups[$i] = $array[$i];
  }
  return $groups;

}  

function getProject(){

  $fname = CURRENT_PROJECT;
  $fp = fopen($fname,"r");
  $counter = 10;
  while ((!flock($fp,LOCK_SH)) && ($counter > 0)) {
    sleep(1);
    $counter--;
  }
  if ($counter == 0) {
    return "none";
  } else {
    $current_project = fgets($fp,128);
    flock($fp,LOCK_UN);
  }
  fclose($fp);
  return $current_project;
}

function changeProject($group) {

  $fname = CURRENT_PROJECT;
  $fp = fopen($fname,"w");
  $counter = 10;
  while ((!flock($fp,LOCK_EX)) && ($counter > 0)) {
    sleep(1);
    $counter--;
  }
  if ($counter == 0) {
    return -1;
  } else {
    fwrite($fp,$group);
    flock($fp,LOCK_UN);
  }
  fclose($fp);
  return 0;

}

function echoList($name, $selected, $list, $indexlist="none", $readonly=FALSE,$onChange="") {

  echo "<SELECT name=\"".$name."\" id=\"".$name."\" $onChange>\n";
  $i = 0;
  while($i < count($list)) {
    if ($indexlist == "none") {
      $index = $i;
    } else {
      $index = $indexlist[$i];
    }
    $text = $list[$i];
    echoOption($index, $text, $readonly,$selected);
    $i++;
  }
  echo "</SELECT>";
}

function echoOption($value,$name,$readonly=FALSE,$selected="notAlwaysNeeded")
{
  echo  "<OPTION VALUE=\"".$value."\"";
  if ("$selected" == "$value") {
    echo " SELECTED";
  }
  if ($_SESSION["readonly"] == "true") {
    echo " DISABLED";
  }
  echo ">".$name."\n";
}

function getFileListing($dir,$pattern="/*/") {

  $arr = array();

  if ($handle = opendir($dir)) {
    while (false !== ($file = readdir($handle))) {
      if (($file != ".") && ($file != "..") && (preg_match($pattern,$file) > 0)) {
      //if (($file != ".") && ($file != "..")) {
        array_push($arr,$file);
      }
    }
    closedir($handle);
  }
  return $arr;
}

function addToDadaTime($time_string, $nseconds) {

  $a = split('[-:]',$time_string);
  $time_unix = mktime($a[3],$a[4],$a[5],$a[1],$a[2],$a[0]);
  $time_unix += $nseconds;
  $new_time_string = date(DADA_TIME_FORMAT,$time_unix);
  return $new_time_string;

}

function localTimeFromGmTime($time_string) {

  $a = split('[-:]',$time_string);
  $time_unix = gmmktime($a[3],$a[4],$a[5],$a[1],$a[2],$a[0]);
  $new_time_string = date(DADA_TIME_FORMAT, $time_unix);
  return $new_time_string;

}

function gmTimeFromLocalTime($time_string) {
  $a = split('[-:]',$time_string);
  $time_unix = mktime($a[3],$a[4],$a[5],$a[1],$a[2],$a[0]);
  $new_time_string = gmdate(DADA_TIME_FORMAT, $time_unix);
  return $new_time_string;
}

function unixTimeFromLocalTime($time_string) {

  $a = split('[-:]',$time_string);
  $time_unix = mktime($a[3],$a[4],$a[5],$a[1],$a[2],$a[0]);
  return $time_unix;
}

function unixTimeFromGMTime($time_string) {
  $a = split('[-:]',$time_string);
  $time_unix = gmmktime($a[3],$a[4],$a[5],$a[1],$a[2],$a[0]);
  return $time_unix;
}
                                                                                                                                            


function killProcess($pname) {

  $returnVal = 0;
  $cmd = "ps axu | grep ".$pname." | grep -v grep";
  $pid_to_kill = system($cmd,$returnVal);
  echo "pids to kill = ".$pids_to_kill ."<BR>\n";

  if ($returnVal == 0) {

    $cmd = $cmd." | awk '{print \$2}'";
    $pid_to_kill = system($cmd);
    echo "pid to kill = ".$pid_to_kill ."<BR>\n";

    $cmd = "kill -KILL ".$pid_to_kill;
    echo "cmd = $cmd<BR>\n";
    system($cmd);
    return "killed pid $pid_to_kill";

  } else {
    return "process did not exist";
  }

}

function makeTimeString($time_unix) {

  $ndays = gmdate("z",$time_unix);
  if ($ndays) {
    return $ndays." days, ".gmdate("H:i:s",$time_unix);
  } else {
    return gmdate("H:i:s",$time_unix);
  }
}


/* Get sub directories of dir 
     index is the dir index to start from (sorted)
     ndirs is the numner of dirs to return

 */
function getSubDirs($dir, $offset=0, $length=0, $reverse=0) {

  $subdirs = array();

  if (is_dir($dir)) {
    if ($dh = opendir($dir)) {
      while (($file = readdir($dh)) !== false) {
        if (($file != ".") && ($file != "..") && ($file != "stats") &&
            (is_dir($dir."/".$file))) {
          array_push($subdirs, $file);
        }
      }
      closedir($dh);
    }
  }

  if ($reverse) 
    rsort($subdirs);
  else 
    sort($subdirs);

  if (($offset >= 0) && ($length != 0)) {
    return array_slice($subdirs, $offset, $length);
  } else {
    return($subdirs);
  }

}

function getIntergrationLength($archive) {

  if (file_exists($archive)) {

    $cmd = "vap -c length -n ".$archive." | awk '{print $2}'";
    $script = "source /home/apsr/.bashrc; ".$cmd." 2>&1";
    $string = exec($script, $output, $return_var);
    $int_length = $output[0];

    if (is_numeric($int_length)) {
      $int_length = sprintf("%5.1f",$int_length);
    } else {
      $int_length = 0;
    }

    return $int_length;

  } else {

    return "0";

  }
}

#
# returns an array of the PWC machines as defined in a dada cfg file
#
function getConfigMachines($cfg, $type) {

  $num = $cfg["NUM_".$type];
  $array = array();
  for ($i=0; $i<$num; $i++) {
    $array[$i] = $cfg[$type."_".$i];
  }
  return $array;

}


#
# Runs a perl script and returns an array of the output
#
function runPerlScript($script) {

  $source = "source /home/dada/.bashrc";
  $cmd = $source."; ".$script." 2>&1";

  $output = array();
  $lastline = exec($cmd, $output, $rval);

  return ($output);

}

#
# return an array of the daemon_info from the machines
#
function getDaemonInfo($hosts, $instrument) {

  $hosts_str = $hosts[0];
  for ($i=1; $i<count($hosts); $i++) {
    $hosts_str .= " ".$hosts[$i];
  }

  $source = "source /home/dada/.bashrc";
  $script = "client_".$instrument."_command.pl 'daemon_info' ".$hosts_str;
  $cmd = $source."; ".$script;

  $output = array();
  $lastline = exec($cmd, $output, $rval); 

  $results = array();
  if ($rval == 0) {

    for ($i=0; $i<count($output); $i++) {

      $array = split(":",$output[$i],3);
      $h = $array[0];   # the host
      $result = $array[1];
      $string = $array[2];

      if ( (strpos($string, "Could not connect to machine")) !== FALSE) {
        # We could not contact the master control script
      } else {

        $results[$h][INSTRUMENT."_master_control"] = 2;
        $daemon_results = split(",",$string);

        for ($j=0; $j<count($daemon_results); $j++) {
          if (strlen($daemon_results[$j]) > 2) {
            $arr = split(" ",$daemon_results[$j]);
            $results[$h][$arr[0]] = $arr[1];
          }
        }
      }
    }
  }
  return $results;

}


#
# get the db_info for the specified key
#
function getDBInfo($hosts, $key, $instrument) {

  $hosts_str = $hosts[0];
  for ($i=1; $i<count($hosts); $i++) {
    $hosts_str .= " ".$hosts[$i];
  }

  $source = "source /home/dada/.bashrc";
  $script = "client_".$instrument."_command.pl db_info ".$key." ".$hosts_str;
  $cmd = $source."; ".$script;

  $output = array();
  $lastline = exec($cmd, $output, $rval);

  $results = array();
  if ($rval == 0) {

    for ($i=0; $i<count($output); $i++) {

      $array = split(":",$output[$i],3);
      $h = $array[0];   # the host
      $result = $array[1];
      $string = $array[2];

      if ( (strpos($string, "Could not connect to machine")) !== FALSE) {
        # We could not contact the master control script
        $results[$h] = 0;
      } else {

        if ($result == "ok") {
          $results[$h] = 2;
        }
      }
    }
  }

  return $results;

}
?>
