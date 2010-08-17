<?PHP
include("../definitions_i.php");
include("../functions_i.php");

?>

<HTML>

<?
$title = "BPSR | Testing";
include("../header_i.php");

?>

<FRAMESET COLS="1" ROWS="60px,70px,300px,*" border=1 bordercolor="#777777">
  <FRAME name="banner" src="/bpsr/statebanner.php" frameborder=0 marginheight=0 marginwidth=0></FRAME>
  <FRAME name="config" src="configuration.php"></FRAME>
  <FRAMESET name=details COLS="50%,50%" ROWS="1" border=5 bordercolor="gray">
    <FRAME name="currentcfg" src="display_text_file.php"></FRAME>
    <FRAME name="currentspec" src="display_text_file.php"></FRAME>
  </FRAMESET>
  <FRAME name="simulator" src=""></FRAME>
</FRAMESET>
</HTML>
