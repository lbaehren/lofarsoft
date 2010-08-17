<?PHP
include("definitions_i.php");
include("functions_i.php");

include(INSTRUMENT.".lib.php");
$inst = new $instrument();
$config = $inst->configFileToHash();

$pwc_status = getAllStatuses($config);
$nexus_status = STATUS_OK;
$nexus_message = "no probs mate";

/* find the non src/pwc/sys daemons */
$server_log_info = $inst->getServerLogInfo();

$keys = array_keys($server_log_info);
$server_daemons = array();
for ($i=0; $i<count($keys); $i++) {
  $k = $keys[$i];
  if ($server_log_info[$k]["tag"] == "server") {
    $server_daemons[$k] = $server_log_info[$k];
  }
}

?>

<html>
<head>
<? $inst->print_head_int("Status Update", 0); ?>

<script type="text/javascript">
  
  var url = "status_update.php";

  function looper() {
    request()
    setTimeout('looper()',2000)
  }

  function request() {  
  
    if (window.XMLHttpRequest)
      http_request = new XMLHttpRequest();
    else
      http_request = new ActiveXObject("Microsoft.XMLHTTP");

    http_request.onreadystatechange = function() {
      handle_data(http_request)
    };
    http_request.open("GET", url, true);
    http_request.send(null);
  }

  // Set all the imgs and links not in the excluded array to green
  function resetOthers(excluded) {

    var imgs = document.getElementsByTagName('img');
    var links = document.getElementsByTagName('a');
    var i=0;

    for (i=0; i<imgs.length; i++) {
      if (excluded.indexOf(imgs[i].id) == -1) {
        imgs[i].src = "/images/green_light.png";
      }
    }
    for (i=0; i<links.length; i++) {
      if (excluded.indexOf(links[i].id) == -1) {
        links[i].title = "OK";
        //links[i].disabled = true;
        links[i].href = "javascript:void(0)";
      }
    }
  }
  
  function handle_data(http_request) {
    if (http_request.readyState == 4) {
      var response = String(http_request.responseText)

      if (response.length > 0) {

        var lines = response.split("\n");
        var num_pwc = 0;
        var num_srv = 0;
        var values = "";

        var key;
        var state;
        var message;
        var host;
        var log_level;
        var img_id;
        var link_id;
        var instrument = "<?echo INSTRUMENT?>"
        var log_file;

        var loglengthIndex = parent.parent.logheader.document.getElementById("loglength").selectedIndex
        var loglength = parent.parent.logheader.document.getElementById("loglength").options[loglengthIndex].value
        var link_str = "";

        if (response.indexOf("Could not connect to") == -1) {

          var set = new Array();

          for (i=0; i < lines.length; i++) {

            values = lines[i].split(":::");
  
            key = values[0];
            state = values[1];
            message = values[2];

            keyparts = key.split("_");
            /* PWC, SRC or SYS message */
            if ((keyparts[0] == "pwc") || (keyparts[0] == "src") || (keyparts[0] == "sys")) {
              host = keyparts[1];
              log_file=instrument+"_"+keyparts[0]+"_monitor";
            } else {
              host = "nexus";
              log_file = key;
            }

            img_id = "img_"+key;
            link_id = "a_"+key;

            set.push("img_"+key);
            set.push("a_"+key);
  
            /* get light val */
            if (state == <?echo STATUS_OK?>) {
              document.getElementById(img_id).src = "/images/green_light.png";
              log_level="all";
            }
            if (state == <?echo STATUS_WARN?>) {
              document.getElementById(img_id).src = "/images/yellow_light.png";
              log_level="warn";
            }
            if (state == <?echo STATUS_ERROR?>) {
              document.getElementById(img_id).src = "/images/red_light.png";
              log_level="error";
            }

            document.getElementById(link_id).title = message;
            document.getElementById(link_id).href = "logwindow.php?machine="+host+"&loglevel="+log_level+"&loglength="+loglength+"&daemon="+log_file+"&autoscroll=true"
          }

          resetOthers(set);
        }
      }
    }
  }

  var activeMachine = "nexus";

  function setActiveMachine(machine) {

    var machine

    var oldmachineIndex = parent.parent.logheader.document.getElementById("active_machine").selectedIndex
    var oldmachine = parent.parent.logheader.document.getElementById("active_machine").options[oldmachineIndex].value
    var loglengthIndex = parent.parent.logheader.document.getElementById("loglength").selectedIndex
    var loglength = parent.parent.logheader.document.getElementById("loglength").options[loglengthIndex].value

    /* Unselect the currently selected machine */
    document.getElementById("td_txt_"+oldmachine).className = "notselected"
    document.getElementById("td_pwc_"+oldmachine).className = "notselected"
    document.getElementById("td_src_"+oldmachine).className = "notselected"
    document.getElementById("td_sys_"+oldmachine).className = "notselected"
   
    Select_Value_Set(parent.parent.logheader.document.getElementById("active_machine"), machine);                                                                               
    /* Select the new machine */
    document.getElementById("td_txt_"+machine).className = "selected"
    document.getElementById("td_pwc_"+machine).className = "selected"
    document.getElementById("td_src_"+machine).className = "selected"
    document.getElementById("td_sys_"+machine).className = "selected"
  }

  function Select_Value_Set(SelectObject, Value) {
    for(index = 0; index < SelectObject.length; index++) {
     if(SelectObject[index].value == Value)
       SelectObject.selectedIndex = index;
     }
  }


</script>
</head>
<!--<body onload="looper()">-->
<body>
<?PHP


?>

<table id="parent_table" border=0 cellspacing=0 cellpadding=0>
<tr><td>

<table border=0 cellspacing=0 cellpadding=0 id="client_status_table">
  <tr>
    <td></td>
    <td width="30px" align="center" valign="top" class="selected" id="td_txt_nexus"><?echo statusLink("nexus","Nexus");?></td>

<?
// Headings

  for($i=0; $i<$config["NUM_PWC"]; $i++) {
    echo "    <td width=\"30\" align=\"center\" class=\"notselected\" id=\"td_txt_".$config["PWC_".$i]."\">".statusLink($config["PWC_".$i],$i)."</td>\n";
  }
?>

  </tr>
  <tr height="25px">
    <td valign="center" class="notselected">PWC:</td>
    <td align="center" class="selected" id="td_nexus_pwc">
      <?echo statusLight($nexus_status, $nexus_message, "nexus", "a_nexus_pwc", "img_nexus_pwc")?>
    </td>

  <?

// PWC status
for($i=0; $i<$config["NUM_PWC"]; $i++) {

  $machine = $config["PWC_".$i];
  $status = $pwc_status["PWC_".$i."_STATUS"];
  $message = $pwc_status["PWC_".$i."_MESSAGE"];
  $linkid = "a_pwc_".$machine;
  $imgid = "img_pwc_".$machine;
  $tdid = "td_pwc_".$machine;

?>
    <td class="notselected" id="<?echo $tdid?>" height="20px">
<?    echo "        ".statusLight($status,$message,$machine,$linkid,$imgid);  ?> 
    </td>
<? } ?>

  </tr>
  <tr height="25px">
    <td valign="center" class="notselected">SRC:</td>
    <td align="center" class="selected" id="td_nexus_src">
      <?echo statusLight($nexus_status, $nexus_message, "nexus", "a_nexus_src", "img_nexus_src")?>
    </td>
<?
// SRC status
for($i=0; $i<$config["NUM_PWC"]; $i++) {

  $machine = $config["PWC_".$i];
  $status = $pwc_status["SRC_".$i."_STATUS"];
  $message = $pwc_status["SRC_".$i."_MESSAGE"];
  $linkid = "a_src_".$machine;
  $imgid = "img_src_".$machine;
  $tdid = "td_src_".$machine;

?>
    <td class="notselected" align="center" id="<?echo $tdid?>">
<?    echo "        ".statusLight($status,$message,$machine,$linkid,$imgid);  ?>
    </td>
<? } ?>                                                                                    

  </tr>

  <tr height="25px">
    <td valign="center" class="notselected">SYS:</td>
    <td align="center" class="selected" id="td_nexus_sys">
      <?echo statusLight($nexus_status, $nexus_message, "nexus", "a_nexus_sys", "img_nexus_sys")?>
    </td>

<?
// SYS status
for($i=0; $i<$config["NUM_PWC"]; $i++) {
                                                                                                                                                            
  $machine = $config["PWC_".$i];
  $status = $pwc_status["SYS_".$i."_STATUS"];
  $message = $pwc_status["SYS_".$i."_MESSAGE"];
  $linkid = "a_sys_".$machine;
  $imgid = "img_sys_".$machine;
  $tdid = "td_sys_".$machine
                                                                                                                                                            
?>
    <td class="notselected" align="center" id="<?echo $tdid?>">
<?    echo "        ".statusLight($status,$message,$machine,$linkid,$imgid);  ?>
    </td>
<? } ?>
                                                                                                                                                            
  </tr>
</table>

</td>

<td width="20px"</td>

<td>
                                                                                                                                                                                                  
<table border=0 cellspacing=0 cellpadding=0 id="server_status_table">
<tr><td class="notselected">&nbsp;</td></tr>
<?
$j = 0;
$keys = array_keys($server_daemons);

for ($i=0; $i<count($keys); $i++) {
  $k = $keys[$i];

  if ($j == 0) {
    echo "  <tr height=\"25px\">\n";
  }

  $link_id = "a_".$k;
  $img_id = "img_".$k;
  $td_id = "td_".$k;
  $name = $server_daemons[$k]["shortname"];

  echo "    <td id=\"".$td_id."\" class=\"notselected\" style=\"text-align: left\">".statusLight(0, "OK", "nexus", $link_id, $img_id)."</td>\n";
  echo "    <td class=\"notselected\"  style=\"text-align: left\">&nbsp;&nbsp;".$name."&nbsp;&nbsp;&nbsp;&nbsp;</td>\n";
                                                                                                                                                                                                  
  if ($j == 2) {
    echo "  </tr>\n";
  }

  if ($j == 2) {
    $j=0;
  } else {
    $j++;
  }
                                                                                                                                                                                                  
}
?>
</table>
</td>
</tr> 



</body>
</html>
<?

function statusLink($machine, $linktext) {

  $url = "machine_status.php?machine=".$machine;

  $string = '<a target="logwindow" href="'.$url.'" '.
            'onClick="setActiveMachine(\''.$machine.'\')">'.
            $linktext.'</a>';

  //return $string;
  return $linktext;

}

function statusLight($status, $message, $machine, $linkid, $imgid) {

  $daemon = "";
  if (strstr($linkid, "nexus_pwc") !== FALSE) {
    $daemon = "&daemon=pwc";
  }
  if (strstr($linkid, "nexus_src") !== FALSE) {
    $daemon = "&daemon=src";
  }
  if (strstr($linkid, "nexus_sys") !== FALSE) {
    $daemon = "&daemon=sys";
  }

  if ($status == STATUS_OK) {
    //$url = "machine_status.php?machine=".$machine;
    $url = "logwindow.php?machine=".$machine."&loglevel=all".$daemon;
    $title = "OK: ".$machine;
  } else if ($status == STATUS_WARN) {
    $url = "logwindow.php?machine=".$machine."&loglevel=warn".$daemon;
    $title = "Warning: ".$machine;
  } else if ($status == STATUS_ERROR) {
    $url = "logwindow.php?machine=".$machine."&loglevel=error".$daemon;
    $title = "Error: ".$machine;
  } else {
    $url = "logwindow.php?machine=".$machine."&loglevel=all".$daemon;
    $title = "Message";
  }
                                                                                   
  $string = '<a target="logwindow" id="'.$linkid.'" href="'.$url.'" '.
            'onClick="setActiveMachine(\''.$machine.'\')"';
    
  $string .= ' TITLE="'.$title.': '.$message.'">';

  #$string .= ' TITLE="header=['.$title.'] body=['.$message.'] '.
  #            'cssbody=[ttbody] cssheader=[ttheader]">';

  $string .= '
       <img id="'.$imgid.'" border="none" width="15px" height="15px" src="/images/';
                                                                                   
  if ($status == STATUS_OK) {
    return $string."green_light.png\" alt=\"OK\">\n      </a>\n";
  } else if ($status == STATUS_WARN) {
    return $string."yellow_light.png\" alt=\"WARN\">\n      </a>\n";
  } else {
    return $string."red_light.png\" alt=\"ERROR\">\n      </a>\n";
  }
                                                                                   
}
?>

