<?PHP
include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG, TRUE);

?>
<html>
<? include("header_i.php"); ?>


<script type="text/javascript">

  var url="/apsr/infoupdate.php?results_dir=<?echo $config["SERVER_RESULTS_DIR"]?>"

  function looper() {

    request()
    setTimeout('looper()',5000)

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

  function handle_data(http_request) {
    if (http_request.readyState == 4) {
      var response = String(http_request.responseText)

      var lines = response.split(";;;")

      for (i=0; i<lines.length; i++) {

        var values = lines[i].split(":::")
        if (values[0]) {
          if (document.getElementById(values[0])) {
            document.getElementById(values[0]).innerHTML = values[1]
          }
        }
      }
    }
  }




</script>
<body onload="looper()">
<? 
?>
  <table border=0 cellspacing=0 cellpadding=5>
  <tr>
    <td>
      <table cellpadding=0 cellspacing=0 border=0>
        <tr>
          <td align="right" class="smalltext"><b>Source</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="SOURCE"><?echo $spec["SOURCE"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>RA</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="RA"><?echo $spec["RA"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>DEC</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="DEC"><?echo $spec["DEC"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>CFREQ</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="CFREQ"><?echo $spec["CFREQ"]?></span> MHz</td>
        </tr>
      </table>
    </td>
    <td width=20>&nbsp;</td>
    <td>
      <table cellpadding=0 cellspacing=0 border=0>
        <tr>
          <td align="right" class="smalltext"><b>Period</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="P0"><?echo $spec["P0"]?></span> ms</td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>DM</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="DM"><?echo $spec["DM"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>Bandwidth</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="BANDWIDTH"><?echo $spec["BANDWITH"]?></span> MHz</td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>UTC START</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="UTC_START"><?echo $spec["UTC_START"]?></span></td>
        </tr>
      </table>
    </td>
    <td width=20>&nbsp;</td>
    <td>
      <table cellpadding=0 cellspacing=0 border=0>
        <tr>
          <td align="right" class="smalltext"><b>Num PWCs</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="NUM_PWC"><?echo $conf["NUM_PWC"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>Project ID</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="PID"><?echo $spec["PID"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>Npol</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="NPOL"><?echo $spec["NPOL"]?></span></td>
        </tr>
        <tr>
          <td align="right" class="smalltext"><b>Nbit</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="NBIT"><?echo $spec["NBIT"]?></span></td>
        </tr>
      </table>
    </td>
    <td width=20>&nbsp;</td>
    <td valign=top>
      <table cellpadding=0 cellspacing=0 border=0>
        <tr>
          <td align="right" class="smalltext"><b>Integrated</b></td>
          <td width=10>&nbsp;</td>
          <td align="left" class="smalltext"><span class="smalltext" id ="INTEGRATED"><?echo $spec["INTEGRATED"]?></span></td>
        </tr>
      </table>
    </td>
  </tr>
  </table>
</body>
</html>

