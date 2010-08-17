<?

/* This page ensures that only 1 person can actively view the page */
function hasControl() {
  if (file_exists(CONTROL_FILE)) {
    $hostname = strtolower(gethostbyaddr($_SERVER["REMOTE_ADDR"]));
    $filehostname = strtolower(rtrim(file_get_contents(CONTROL_FILE)));
    return ($hostname == $filehostname); 
  } else {
    return true;
  }
}

function validate($user, $password) {

  # $db["ajameson"] = "\$1\$wVQraJ5l\$m53BFYC6wCl6wxYc09gn2.";
  # $db["wstraten"] = "";
  $db["apsr"] = "\$1\$wVQraJ5l\$m53BFYC6wCl6wxYc09gn2.";

  if (crypt($password, $db[$user]) == $db[$user]) {
    return true;
  } else {
    return false;
  } 

}

function isTrustedUsers($user) {

  if (in_array($user, $trusted_users) ) {
    return true;
  } else {
    return false;
  }

}

function isTrustedHost($host) {
  
  $trusted_hosts = array( "perseus.atnf.csiro.au", "pavo.atnf.csiro.au" );

  if (in_array(strtolower($host), $trusted_hosts) ) {
    return true;
  } else {
    return false;
  }
}

function getTrustedUsers() {
  $trusted_users = array( "ajameson", "wstraten");
  return $trusted_users;
}

function getTrustedHosts() {
  return $trusted_hosts;
}
?>
