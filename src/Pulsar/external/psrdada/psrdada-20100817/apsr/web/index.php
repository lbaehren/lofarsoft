<?PHP

include("definitions_i.php");
include("functions_i.php");

?>
<HTML>
<?
$title = "APSR : ATNF Parkes Swinburne Recorder";
include("header_i.php");
?>

<FRAMESET ROWS="1" COLS="800px,*" frameborder=1>
  <FRAMESET COLS=1 ROWS="60px,95px,40px,110px,230px,*" border=0>
    <FRAME name="banner" src="statebanner.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="statuswindow" src="statuswindow.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="controlwindow" src="controlwindow.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="infowindow" src="infowindow.php">
    <FRAME name="plotwindow" src="plotwindow.php">
    <FRAME name="gainwindow" src="gain_reporter.php">
  </FRAMESET>
  <FRAMESET name=rightwindow COLS=1 ROWS="70px,*" border=0>
    <FRAME name="logheader" src="logheader.php" frameborder=0 marginheight=0 marginwidth=0>
    <FRAME name="logwindow" src="machine_status.php?machine=nexus">
  </FRAMESET>
</FRAMESET>
</HTML>
