<?PHP
include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG);
$conf = getConfigFile(DADA_CONFIG,TRUE);
$spec = getConfigFile(DADA_SPECIFICATION, TRUE);

?>
<html>
<head>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML?>
  <script type="text/javascript">

  // This URL will return the names of the 5 current timestamped images();
  var url = "plotupdate.php?results_dir=<?echo $config["SERVER_RESULTS_DIR"]?>";

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
      var lines = response.split(";;;")

      var img1_line = lines[0].split(":::")
      var img2_line = lines[1].split(":::")
      var img3_line = lines[2].split(":::")
      var img4_line = lines[3].split(":::")
      var img1_hi_line = lines[4].split(":::")
      var img2_hi_line = lines[5].split(":::")
      var img3_hi_line = lines[6].split(":::")
      var img4_hi_line = lines[7].split(":::")

      var img1 = img1_line[1]
      var img2 = img2_line[1]
      var img3 = img3_line[1]
      var img4 = img4_line[1]
      var img1_hi = img1_hi_line[1]
      var img2_hi = img2_hi_line[1]
      var img3_hi = img3_hi_line[1]
      var img4_hi = img4_hi_line[1]

      if (document.getElementById("img1").src != img1) {
        document.getElementById("img1").src = img1
      }
      if (document.getElementById("img2").src != img2) {
        document.getElementById("img2").src = img2
      }
      if (document.getElementById("img3").src != img3) {
        document.getElementById("img3").src = img3
      }
      //if (document.getElementById("img4").src != img4) {
      //  document.getElementById("img4").src = img4
      //}
      if (document.getElementById("img1_hi").href != "javascript:popWindow("+img1_hi+")") {
        document.getElementById("img1_hi").href = "javascript:popWindow('"+img1_hi+"')"
      }
      if (document.getElementById("img2_hi").href != "javascript:popWindow("+img2_hi+")") {
        document.getElementById("img2_hi").href = "javascript:popWindow('"+img2_hi+"')"
      }
      if (document.getElementById("img3_hi").href != "javascript:popWindow('"+img3_hi+"')") {
        document.getElementById("img3_hi").href = "javascript:popWindow('"+img3_hi+"')"
      }
      //if (document.getElementById("img4_hi").href != "javascript:popWindow('"+img4_hi+"')") {
      //  document.getElementById("img4_hi").href = "javascript:popWindow('"+img4_hi+"')"
      //}
    }
  }

  function popWindow(URL) {
    day = new Date();
    id = day.getTime();
    eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=0,location=1,statusbar=0,menubar=0,resizable=1,width=1080,height=800');");
  }
                                                                                                                 

</script>
</head>

<body onload="looper()">
<? 
?>
  <table border=0 width="100%" cellspacing=0 cellpadding=5>
  <tr>
    <td align="center" width=240px height=180px><font class="smalltext">Total Intensity vs Phase</font><br>
      <a href="javascript:popWindow('/images/blankimage.gif')" id="img1_hi"><img id="img1" src="/images/blankimage.gif" alt="No Data Available"></a></td>
    <td align="center" width=240px height=180px><font class="smalltext">Phase vs Time</font><br>
      <a href="javascript:popWindow('/images/blankimage.gif')" id="img2_hi"><img id="img2" src="/images/blankimage.gif"alt="No Data Available"></a></td>
    <td align="center" width=240px height=180px><font class="smalltext">Phase vs Frequency</font><br>
     <a href="javascript:popWindow('/images/blankimage.gif')" id="img3_hi"><img id="img3" src="/images/blankimage.gif" alt="No Data Available"></a></td>
  </tr>
  <!--<tr>
    <td colspan=3 align="center" height=180px><font class="smalltext">graph 4</font><br><img id="img4" src="/images/blankimage.gif" alt="No Data Available"></td>
  </tr>-->
  </table>
</body>
</html>

