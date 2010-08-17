<?PHP

function getServerLogInformation() {
                                                                                                                   
  $arr = array();
  $arr["apsr_tcs_interface"] =   array("logfile" => "apsr_tcs_interface.log", "name" => "TCS Interface", "tag" => "server");
  $arr["apsr_results_manager"] = array("logfile" => "apsr_results_manager.log", "name" => "Results Mngr", "tag" => "server");
  $arr["dada_pwc_command"] =     array("logfile" => "dada_pwc_command.log", "name" => "dada_pwc_command", "tag" => "server");
  $arr["apsr_gain_manager"] =    array("logfile" => "apsr_gain_manager.log", "name" => "Gain Mngr", "tag" => "server");
  $arr["apsr_aux_manager"] =     array("logfile" => "apsr_aux_manager.log", "name" => "Aux Mngr", "tag" => "server");
  $arr["apsr_pwc_monitor"] =     array("logfile" => "nexus.pwc.log", "name" => "PWC Mon", "tag" => "pwc");
  $arr["apsr_sys_monitor"] =     array("logfile" => "nexus.sys.log", "name" => "SYS Mon", "tag" => "sys");
  $arr["apsr_src_monitor"] =     array("logfile" => "nexus.src.log", "name" => "SRC Mon", "tag" => "src");
  return $arr;

}

                                                                                                                   
function getClientLogInformation() {
                                                                                                                   
  $arr = array();
  #$arr["apsr_master_control"] = array("logfile" => "apsr_master_control.log", "name" => "master_control";
  $arr["apsr_observation_manager"]  = array("logfile" => "nexus.sys.log", "name" => "Obs Mngr", "tag" => "obs mngr");
  $arr["apsr_archive_manager"]      = array("logfile" => "nexus.sys.log", "name" => "Archive Mngr", "tag" => "arch mngr");
  $arr["apsr_background_processor"] = array("logfile" => "nexus.sys.log", "name" => "BG Processor", "tag" => "bg mngr");
  $arr["apsr_gain_monitor"]         = array("logfile" => "nexus.sys.log", "name" => "Gain Mon", "tag" => "gain mon");
  $arr["apsr_aux_monitor"]          = array("logfile" => "nexus.sys.log", "name" => "Aux Mon", "tag" => "aux mngr");
  $arr["apsr_processing_manager"]   = array("logfile" => "nexus.src.log", "name" => "Proc Mngr", "tag" => "proc mngr");
  $arr["processor"]                 = array("logfile" => "nexus.src.log", "name" => "Processor", "tag" => "proc");

  return $arr;

}

?>
