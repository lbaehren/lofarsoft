<?PHP

if (!$_CASPSR_LIB_PHP) { $_CASPSR_LIB_PHP = 1;


define(INSTRUMENT, "caspsr");
define(CFG_FILE, "/home/dada/linux_64/share/caspsr.cfg");
define(CSS_FILE, "caspsr/caspsr.css");

include("site_definitions_i.php");
include("instrument.lib.php");

class caspsr extends instrument
{

  function caspsr()
  {
    instrument::instrument(INSTRUMENT, CFG_FILE, URL);

    $this->css_path = "/caspsr/caspsr.css";
    $this->banner_image = "/caspsr/images/caspsr_logo_480x60.png";
    $this->banner_image_repeat = "/caspsr/images/caspsr_logo_1x60.png";

  }

  function print_head($title, $refresh=0) 
  {
      echo "<head>\n";
      $this->print_head_int($title, $refresh);
      echo "</head>\n";
  }

  function print_head_int($title, $refresh) {
    echo "  <title>".$title."</title>\n";
    instrument::print_css();
    if ($refresh > 0)
      echo "<meta http-equiv='Refresh' content='".$refresh."'>\n";
  }

  function getServerLogInfo() {

    $arr = array();
    $arr["caspsr_tcs_interface"] =      array("logfile" => "caspsr_tcs_interface.log", "name" => "TCS Interface", "tag" => "server", "shortname" => "TCS");
    $arr["caspsr_results_manager"] =    array("logfile" => "caspsr_results_manager.log", "name" => "Results Mngr", "tag" => "server", "shortname" => "Results");
    $arr["dada_pwc_command"] =          array("logfile" => "dada_pwc_command.log", "name" => "dada_pwc_command", "tag" => "server", "shortname" => "PWCC");
    $arr["caspsr_gain_manager"] =       array("logfile" => "caspsr_gain_manager.log", "name" => "Gain Mngr", "tag" => "server", "shortname" => "Gain");
    $arr["caspsr_auxiliary_manager"] =  array("logfile" => "caspsr_auxiliary_manager.log", "name" => "Aux Mngr", "tag" => "server", "shortname" => " Aux");
    $arr["caspsr_web_monitor"] =        array("logfile" => "caspsr_web_monitor.log", "name" => "Monitor", "tag" => "server", "shortname" => "Monitor");
    $arr["caspsr_pwc_monitor"] =        array("logfile" => "nexus.pwc.log", "name" => "PWC Mon", "tag" => "pwc", "shortname" => "PWC Mon");
    $arr["caspsr_sys_monitor"] =        array("logfile" => "nexus.sys.log", "name" => "SYS Mon", "tag" => "sys", "shortname" => "SYS Mon");
    $arr["caspsr_src_monitor"] =        array("logfile" => "nexus.src.log", "name" => "SRC Mon", "tag" => "src", "shortname" => "SRC Mon");
    $arr["caspsr_transfer_manager"] =   array("logfile" => "caspsr_transfer_manager.log", "name" => "Transfer Manager", "tag" => "src", "shortname" => "Xfer");
    return $arr;

  }

  function getClientLogInfo() {
    $arr = array();
    $arr["caspsr_observation_manager"]  = array("logfile" => "nexus.sys.log", "name" => "Obs Mngr", "tag" => "obs mngr");
    $arr["caspsr_archive_manager"]      = array("logfile" => "nexus.sys.log", "name" => "Archive Mngr", "tag" => "arch mngr");
    $arr["caspsr_background_processor"] = array("logfile" => "nexus.sys.log", "name" => "BG Processor", "tag" => "bg mngr");
    $arr["caspsr_gain_controller"]      = array("logfile" => "nexus.sys.log", "name" => "Gain Mon", "tag" => "gain mon");
    $arr["caspsr_auxiliary_manager"]    = array("logfile" => "nexus.sys.log", "name" => "Aux Mon", "tag" => "aux mngr");
    $arr["caspsr_processing_manager"]   = array("logfile" => "nexus.src.log", "name" => "Proc Mngr", "tag" => "proc mngr");
    $arr["processor"]                   = array("logfile" => "nexus.src.log", "name" => "Processor", "tag" => "proc");
    return $arr;
  }

} 

} // _CASPSR_LIB_PHP
