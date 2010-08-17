<?PHP
include("../definitions_i.php");
include("../functions_i.php");

?>
<html>
<head>
  <title>BPSR</title>

  <?echo STYLESHEET_HTML?>
  <?echo FAVICO_HTML?>

  <script type="text/javascript">

function mySubmit(type) {

  var type
  var i
  var val

  if (type == "config") {

    var i = document.getElementsByName("current_config")[0].selectedIndex;
    val = document.getElementsByName("current_config")[0].options[i].value
    parent.currentcfg.location = "/bpsr/testing/display_text_file.php?file="+val

  } else if (type == "spec") {

    var i = document.getElementsByName("current_spec")[0].selectedIndex;
    val = document.getElementsByName("current_spec")[0].options[i].value
    parent.currentspec.location = "/bpsr/testing/display_text_file.php?file="+val

  } else {

  }
}

function cmdPopUp(URL) {

  id = "ControlWindow"
  eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=1,location=0,statusbar=1,menubar=0,resizable=0,width=800,height=600');");

}
                                                                                                                                        
function popUp(URL) {

  var i
  var cfg
  var spec
  var dfb
  var duration
  var acc_len

  i = document.getElementsByName("current_config")[0].selectedIndex;
  cfg = document.getElementsByName("current_config")[0].options[i].value
  i = document.getElementsByName("current_spec")[0].selectedIndex;
  spec = document.getElementsByName("current_spec")[0].options[i].value
  duration = document.getElementById("running_time").value;

  i = document.getElementById("acc_len").selectedIndex;
  acc_len = document.getElementById("acc_len").options[i].value;
  
  if (cfg == "") {
    alert ("Please select a iBOB Configation to use")
  } else if (spec == "") {
    alert ("Please select a test specifcation to use")
  } else if (duration == "") {
    alert ("Please input a running time")
  } else {
    var new_url = URL + "&cfg="+cfg+"&spec="+spec+"&duration="+duration+"&acc_len="+acc_len
    parent.simulator.document.location = new_url
  }
}

function loadDefaults() {

  mySubmit('config')
  mySubmit('spec')

}
                                                                                                                                        
  </script>

</head>

<?

if (isset($_GET["type"])) {
  $actiontype = $_GET["type"];
} else {
  $actiontype = "show";
}

$sys_config = getConfigFile(SYS_CONFIG);
$configfiles = getFileListing($sys_config["CONFIG_DIR"] ,"/^ibob_/");
$specfiles = getFileListing($sys_config["CONFIG_DIR"]."/testing","/.spec$/");

sort($configfiles);
sort($specfiles);


for ($i=0;$i<count($configfiles);$i++) {
  $configfiles[$i] = $configfiles[$i];
}
for ($i=0;$i<count($specfiles);$i++) {
  $specfiles[$i] = "testing/".$specfiles[$i];
}

?>
<body onload="loadDefaults()">
<?
if (!IN_CONTROL) { ?>
<h3><font color=red>Test system disabled as your host is not in control of the instrument</font></h3>
</body>
</html>
<?
  exit(0);
}
?>
<table border=0 cellpadding=0 cellspacing=0 width=100%>
  <tr>
    <td style="vertical-align: middle">
      Run For <input type="text" id="running_time" size="5" maxlength="5" value="30"></input> sec
    </td>
    <td>
      <div class="btns">
        <a href="javascript:popUp('simulator.php?cmd=start')"  class="btn" > <span>Start</span> </a>
        <a href="javascript:popUp('simulator.php?cmd=stop')"  class="btn" > <span>Stop</span> </a>
      </div>
    </td>

    <td align=left>CFG: 
      <select style="font-family: courier; font-weight: bold" name="current_config" onChange="mySubmit('config')">
<?
      for($i=0;$i<count($configfiles);$i++) {
        echo "<option value=\"".$configfiles[$i]."\">".$configfiles[$i]."</option>\n";
      }
?>
      </select>
    </td>
    <td align=left>SPEC:
      <select style="font-family: courier; font-weight: bold" name="current_spec" onChange="mySubmit('spec')">
<?
      for($i=0;$i<count($specfiles);$i++) {
        echo "<option value=\"".$specfiles[$i]."\">".$specfiles[$i]."</option>\n";
      }
?>
      </select>
    </td>

  </tr>

  <tr>
    <td colspan=2></td>
    <td colspan=2>Accumulation Length: 
      <select name="ACC_LEN" style="font-family: courier; font-weight: bold" id="acc_len">
        <option value="4">4 (10.24 usec)</option>
        <option value="25" selected>25 (64 usec)</option>
        <option value="256">256 (655.36 usec)</option>
        <option value="1024">1024 (2.62 msec)</option>
      </select>
    </td>
</table>
<center>
</center>
                                                                                                                
</body>
</html>
                                                                                                                
