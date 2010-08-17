<?PHP
include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG, TRUE);

?>
<html>
<? include("header_i.php"); ?>

<script type="text/javascript">

  //soundManager.url = '/sounds/sm2-swf-movies/'; // directory where SM2 .SWFs live

  // disable debug mode after development/testing..
  //soundManager.debugMode = false;

  //soundManager.waitForWindowLoad = true;
  //soundManager.onload = function() {
    // SM2 has loaded - now you can create and play sounds!
    //soundManager.createSound('changetape','/sounds/tapechange.mp3');
  //}

  var url="/bpsr/tapeupdate.php"

  function looper() {

    request()
    setTimeout('looper()',10000)

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
      var span
      var td
      var values
      
      for (i=0; i<lines.length; i++) {
        values = lines[i].split(":::");
  
        if ((values[0]) && (document.getElementById(values[0]))) {  
          //alert(values[0]+" -> "+values[1])
          span = document.getElementById(values[0])
          td = document.getElementById(values[0]+"_td")

          span.innerHTML = values[1]
          
          if (values[1] == "Insert Tape") {
            var html = "<span>Load "+values[2]+"</span>\n";
            span.innerHTML = html
            td.style.backgroundColor = "orange"
            //soundManager.play('changetape');
          } else {
            if (values[1].substring(0,5) == "Error") {
              td.style.backgroundColor = "red"
            } else {
              td.style.backgroundColor = ""
            }
          }
        }
      }
    }
  }

</script>

<body onload="looper()">
<? 
?>
  <table cellpadding=0 cellspacing=0 border=0 width=100%>
    <tr>
      <td width=33% align="center" valign="top">
        <table width=100%> 
          <tr> <td colspan=3 align="center" class="smalltext"><b>Transfer Manager</b></td> </tr>
          <tr> <td colspan=3 id="XFER_td" align="center" class="smalltext"><span class="smalltext" id ="XFER"></span></td> </tr>
          <tr> 
            <td id="XFER_SWIN_td" align="center" class="smalltext">Swin: <span class="smalltext" id ="XFER_SWIN"></span></td> 
            <td id="XFER_BEAMS_td" align="center" class="smalltext">Parkes: <span class="smalltext" id ="XFER_PARKES"></span></td> 
            <td class="smalltext">[beams]</td>
          </tr>
        </table>
      </td>
      <td width=33% align="center" valign="top">
        <table width=100%> 
          <tr> 
            <td colspan=3 id="SWIN_TAPE_td" align="center" class="smalltext"><b>Swin Tape</b>&nbsp;&nbsp;&nbsp;<span class="smalltext" id ="SWIN_TAPE"></span></td> 
          </tr>
          <tr> <td colspan=3 id="SWIN_STATE_td" align="center" width=100% class="smalltext"><span class="smalltext" id ="SWIN_STATE"></span></td> </tr>
          <tr> 
            <td id="SWIN_NUM_td" align="center" width=33% class="smalltext">Queued: <span class="smalltext" id ="SWIN_NUM"></span></td>
            <td id="SWIN_PERCENT_td" align="center" width=33% class="smalltext"><span class="smalltext" id ="SWIN_PERCENT"></span> &#37; Full</td> 
            <td id="SWIN_TIME_LEFT_td" align="center" width=33% class="smalltext"><span class="smalltext" id ="SWIN_TIME_LEFT"></span> mins</td> 
          </tr>
        </table>
      </td>
      <td width=33% align="center" valign="top">
        <table width=100%> 
          <tr> 
            <td colspan=3 id="PARKES_TAPE_td" align="center" class="smalltext"><b>Parkes Tape</b>&nbsp;&nbsp;&nbsp;<span class="smalltext" id ="PARKES_TAPE"></span></td> 
          </tr>
          <tr> <td colspan=3 id="PARKES_STATE_td" align="center" width=100% class="smalltext"><span class="smalltext" id ="PARKES_STATE"></span></td> </tr>
          <tr> 
            <td id="PARKES_NUM_td" align="center" width=33% class="smalltext">Queued: <span class="smalltext" id ="PARKES_NUM"></span></td>
            <td id="PARKES_PERCENT_td" align="center" width=33% class="smalltext"><span class="smalltext" id ="PARKES_PERCENT"></span> &#37; Full</td> 
            <td id="PARKES_TIME_LEFT_td" align="center" width=33% class="smalltext"><span class="smalltext" id ="PARKES_TIME_LEFT"></span> mins</td> 
          </tr>
        </table>
      </td>
    </tr>
  </table>
</body>
</html>
