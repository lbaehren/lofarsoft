<?PHP

include("definitions_i.php");
include("functions_i.php");

if (isset($_GET["newproject"])) {
  changeProject($_GET["newproject"]);
  $current_project = getProject();
}

?>
<html>
<?
include("header_i.php");
?>
<body>

<SCRIPT LANGUAGE="JavaScript">
<!-- Begin
function updateProject(){
  var projIndex = document.getElementById("currentproject").selectedIndex
  var newProject = document.getElementById("currentproject").options[projIndex].value
  document.location = "controlwindow.php?newproject="+newProject
}

function popUp(URL) {
  id = "ContolWindow";
  eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=1,location=0,statusbar=1,menubar=0,resizable=1,width=640,height=480');");
}

function popWindow(URL,width,height) {

  var width = width || "1024";
  var height = height || "768";

  if (URL == "annotate_obs.php") {
    URL = URL+"?observation="+parent.infowindow.document.getElementById("UTC_START").innerHTML
  }

  day = new Date();
  id = day.getTime();
  eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=1,scrollbars=1,location=1,statusbar=1,menubar=1,resizable=1,width="+width+",height="+height+"');");
}
// End -->
</script>

<?
$config = getConfigFile(DADA_CONFIG);
$groups = getProjects($config["DADA_USER"]);
?>
                                                                                   
  <table border=0 width="100%" cellpadding=0 cellspacing=0>
  <!--
  <tr>
    <td align="center"> <a href="javascript:popUp('client_status.php')" class="button" ><span></span><i>Client Status</i><b></b></a> </td>
    <td align="center"> <a target="_ganglia" href="http://<?echo $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"]?>/ganglia/" class="button" ><span></span><i>Ganglia</i><b></b></a> </td>
    <td align="center"> <a href="javascript:popWindow('results.php',800, 800)" class="button" ><span></span><i>Results</i><b></b></a> </td>
    <td align="center"> <a href="javascript:popWindow('control/', 1280, 650)" class="button" ><span></span><i>Controls</i><b></b></a>
    <td align="center"> <a href="javascript:popWindow('testing/', 1024, 768)" class="button" ><span></span><i>Test System</i><b></b></a>
</td>-->

<tr>
    <td align="center"> 
    <div class="btns">
      <a href="javascript:popWindow('annotate_obs.php', 800, 600)" class="btn" > <span>Ann. Obs</span> </a>
      <a href="javascript:popUp('client_status.php')"  class="btn" > <span>Client Status</span> </a>
      <a target="_ganglia" href="http://<?echo $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"]?>/ganglia/" class="btn" > <span>Ganglia</span> </a>
      <a href="javascript:popWindow('results.php', 1200, 800)" class="btn" > <span>Results</span> </a>
      <a href="javascript:popWindow('control/', 1200, 800)" class="btn" > <span>Controls</span> </a>
      <a href="javascript:popWindow('testing/', 1024, 800)" class="btn" > <span>Test</span> </a>
      <a href="javascript:popWindow('commandeer.php', 800, 480)" class="btn" > <span>Commandeer</span> </a>

      <a href="javascript:popWindow('support.html', 1200, 800)" class="btn" > <span>Help</span> </a>
    </div>

    <!--
    <td width="25%" align="center" valign="center"> Project: <select id="currentproject" onChange="updateProject()">
<?
  for($i=0; $i<count($groups);$i++) {
    echo "      <option value=".$groups[$i];
    if ($groups[$i] == $current_project) {
       echo " selected";
     }
    echo ">".$groups[$i]."</option>\n";
  }
?>
    </select>
    </td>-->
  </tr>
<!--
  <tr>
    <td align="center"> <a href="javascript:popUp('client_command.php?cmd=init_db')" class="button" ><span></span><i>Init DB</i><b></b></a> </td>
    <td align="center"> <a href="javascript:popUp('client_command.php?cmd=destroy_db')" class="button" ><span></span><i>Destroy DB</i><b></b></a> </td>
    <td align="center"> <a href="javascript:popUp('client_command.php?cmd=shutdown')" class="button" ><span></span><i>Shutdown System</i><b></b></a> </td>
    <td align="center"> <a href="" class="button" ><span></span><i>Donate</i><b></b></a> </td>  </tr>
-->
  </table>
</body>
</html>
