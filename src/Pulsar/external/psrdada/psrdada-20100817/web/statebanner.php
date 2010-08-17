<?PHP
include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$host = $config["SERVER_HOST"];
$port = $config["TCS_STATE_INFO_PORT"];

?>
<HTML>
<HEAD>
  <? echo STYLESHEET_HTML?>
  <? echo FAVICO_HTML?>
  <script type="text/javascript">


  // This URL will return the names of the 5 current timestamped images();
  var url ="http://<?echo $_SERVER["HTTP_HOST"]?>/<?echo INSTRUMENT?>/stateupdate.php?host=<?echo $host?>&port=<?echo $port?>";
  function looper() {
    request()
    setTimeout('looper()',5000)
  }

  function request() {
    if (window.XMLHttpRequest)
      http_request = new XMLHttpRequest()
    else
      http_request = new ActiveXObject("Microsoft.XMLHTTP");
    http_request.onreadystatechange = function() {
      handle_data(http_request)
    }
    http_request.open("GET", url, true)
    http_request.send(null)
 }
                                                                                                                                                                   
  function handle_data(http_request) {
    if (http_request.readyState == 4) {
      var response = String(http_request.responseText)
      document.getElementById("globalstatus").innerHTML = response
    }
  }

</script>

</HEAD>
<BODY onload="looper()">
<table cellspacing=0 cellpadding=0 border=0 width="100%">
  <tr>
    <td width=480px height=60px><img src="<?echo BANNER_IMAGE?>" width=480 height=60></td>
    <td width="100%" height=60px background="<?echo BANNER_IMAGE_REPEAT?>" class="largetext"><span id="globalstatus" class="largetext"></span></td>
  </tr>
</table>
</BODY>
</HTML>
