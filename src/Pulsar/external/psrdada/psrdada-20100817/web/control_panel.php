<?PHP

include("definitions_i.php");
include("functions_i.php");

include(INSTRUMENT.".lib.php");
$inst = new $instrument();

?>
<html>
  <head>
  <? $inst->print_head_int("Control Panel", 0); ?>
  <script type="text/javascript">
    function popUp(URL) {
      id = "ContolWindow";
      eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=1,location=0,statusbar=1,menubar=0,resizable=1,width=1024,height=768');");
    }
  </script>
  </head>
<body>

<table border=0 width="100%" cellpadding=5>
  <tr>
    <td align="center"> 
      <input type='button' value='Ann. Obs' onClick="popUp('annotate_obs.php')">
      <input type='button' value='Client Status' onClick="popUp('client_status.php')">
      <input type='button' value='Ganglia' onClick="popUp('http://<?echo $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"]?>/ganglia/')">
      <input type='button' value='Results' onClick="popUp('results.php')">
      <input type='button' value='Controls' onClick="popUp('control/')">
      <input type='button' value='Test' onClick="popUp('testing/')">
      <input type='button' value='Commandeer' onClick="popUp('commandeer.php')">
      <input type='button' value='Help' onClick="popUp('support.html')">
    </td>
  </tr>
</table>
</body>
</html>
