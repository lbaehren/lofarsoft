<?PHP
include("../definitions_i.php");
include("../functions_i.php");
?>
<html>
<head>
  <title>APSR</title>
  <link rel="STYLESHEET" type="text/css" href="/apsr/style_log.css">
</head>
<?

if (isset($_GET["file"])) {
  $file = $_GET["file"];
} else {
  $file = "";
}

if ($file != "") {
?>
<body>
<pre>
<?                                                                                        

  $sys_config = getConfigFile(SYS_CONFIG); 
  $config = getRawTextFile($sys_config["CONFIG_DIR"]."/".$file);

  for ($i=0; $i<count($config);$i++) {
    echo $config[$i]."\n";
  }
}

?>
</pre>

</body>
</html>
