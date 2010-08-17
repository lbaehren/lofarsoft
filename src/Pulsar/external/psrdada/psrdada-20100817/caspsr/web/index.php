<?PHP

include("caspsr.lib.php");

$inst = new caspsr();

echo "<html>\n";
# $inst->print_head("CASPSR : Casper Parkes Swinburne Recorder");
?>
<FRAMESET ROWS="1" COLS="800px,*" frameborder=1>
  <FRAMESET COLS=1 ROWS="60px,95px,40px,110px,250px,*" border=1>
    <FRAME name="banner" src="banner_new.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="statuswindow" src="status_window.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="controlpanel" src="control_panel.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="infowindow" src="">
    <FRAME name="plotwindow" src="">
    <FRAME name="gainwindow" src="">
  </FRAMESET>
  <FRAMESET name=rightwindow COLS=1 ROWS="70px,*" border=1>
    <FRAME name="logheader" src="" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="logwindow" src="">
  </FRAMESET>
</FRAMESET>
</html>
