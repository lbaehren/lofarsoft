<?PHP
include("definitions_i.php");
include("functions_i.php");

$config = getConfigFile(SYS_CONFIG, TRUE);
?>
<html>
<head>
  <title>BPSR | Archive Monitor</title>
  <link rel="STYLESHEET" type="text/css" href="/bpsr/style.css">
  <link rel="shortcut icon" href="/images/favicon.ico"/>
  <style>
    .archivetable {
      text-align: center;
      vertical-align: top;
    }
  </style>

<script type="text/javascript">

  var url="/bpsr/archiverupdate.php"

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
      
      for (i=0; i<lines.length; i++) {
        var values = lines[i].split(":::");
        
        if ((values[0]) && (document.getElementById(values[0]))) {
          document.getElementById(values[0]).innerHTML = values[1]

          if (values[1] == "Insert Tape") {

            var html = "    <div class=\"btns\">\n"
            html += "<a href=\"/bpsr/tapeupdate.php?tapeinserted="
            html += values[2]+"&location="+values[0]+"\" class=\"btn\" >"
            html += " <span>Load "+values[2]+"</span></a>\n";
            html += "</div>\n"

            document.getElementById(values[0]).innerHTML = html
            document.getElementById(values[0]+"_td").style.backgroundColor = "red"
            //soundManager.play('changetape');
          }  else {

            if (values[1].substring(0,5) == "Error") {
              document.getElementById(values[0]+"_td").style.backgroundColor = "red"
            } else {
              document.getElementById(values[0]+"_td").style.backgroundColor = ""
            }
          }
        }
      }
    }
  }

</script>
</head>

<body>
<? 
?>
  <center>
  <table cellpadding=10 cellspacing=0 border=0 width=800px class="archivetable">
 
    <tr colspan=5>
      <td></td>
      <td colspan=3id="TOSEND_td">
        <b>Archives To Send (<span id="TOSEND_COUNT_span">9</span>)</b><br>
        <span id="TOSEND_LIST_span">2009-01-15-04:15:18<br>
2009-01-15-04:16:49<br>
2009-01-15-04:18:15<br>
2009-01-15-04:19:50<br>
2009-01-15-04:21:20<br>
2009-01-15-04:22:57<br>
2009-01-15-04:24:28<br>
2009-01-15-04:26:01<br>
2009-01-15-04:28:37</span>
      </td>
      <td></td>
    </tr>

    <tr>
      <td colspan=2></td>
      <td align=center><img src="/bpsr/images/arrow_south_50x30.png" width="30px" height="50px"><td>
      <td colspan=2></td>
    </tr>

    <tr>
     <td></td>
     <td colspan=3id="XFER_td">
        <b>Transferring: <span id="XFER_span">2009-01-15-04:13:47</span></b>
     </td>
     <td></td>
    </tr>

    <tr>
      <td></td>
      <td style="text-align: right"><img src="/bpsr/images/arrow_southwest_50x50.png" width="50px" height="50px"</td>
      <td></td>
      <td style="text-align: left"><img src="/bpsr/images/arrow_southeast_50x50.png" width="50px" height="50px"</td>
      <td></td>
    </tr>
      
    <tr>
     <td colspan=2 id="SWIN_td">
        <b>Swin Archiving: <span id="SWIN_ARCHIVING_td">2009-01-15-04:24:28</span></b><br>
        <span id="SWIN_span">2009-01-15-04:18:15<br>
2009-01-15-04:19:50<br>
2009-01-15-04:21:20<br>
2009-01-15-04:22:57<br>
2009-01-15-04:24:28<br>
2009-01-15-04:26:01</span>
     </td>
     <td></td>
     <td colspan=2 id="PARKES_td">
        <b>Parkes Archiving: <span id="PARKES_ARCHIVING_td">2009-01-15-04:24:28</span></b><br>
        <span id="PARKES_span">2009-01-15-04:19:50<br>
2009-01-15-04:21:20<br>
2009-01-15-04:22:57<br>
2009-01-15-04:24:28</span>
      </td>
    </tr>
  </table>
  </center>
  
</body>
</html>
