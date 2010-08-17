<?PHP
if (!(isset($title))) 
  $title = "APSR";
?>
<head>
  <title><?echo $title?></title>
  <? echo STYLESHEET_HTML; ?>
  <? echo FAVICO_HTML?>

<?
if ((isset($refresh)) && ($refresh > 0)) {
?>
  <meta http-equiv="Refresh" content="<?echo $refresh?>">
<? } ?>
</head>
