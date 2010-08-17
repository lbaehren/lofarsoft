<?PHP 
include("definitions_i.php");
include("functions_i.php");

/* Find the latest files in the plot file directory */


define(DEBUG,0);
define(HOST,"srv0.apsr.edu.au");
define(PORT,"52008");


if (DEBUG > 0) 
  echo "Opening socket<BR>\n";

list($socket, $result) = openSocket(HOST,PORT, 10);
                                                                                                                 
if ($result != "ok") {
  echo "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 ";
  if (DEBUG > 0) {
    echo $result."<BR>\n";
    flush();
  }
} else {
                                                                                                                 
  socketWrite($socket, "REPORT GAINS\r\n");

  if (DEBUG > 0) {
    echo "Wrote \"REPORT GAINS\"<BR>\n";
    flush();
    echo "Reading Reponse...\n";
    flush();
  }
                                                                                                                 
  $response = rtrim(socketRead($socket));

  if (DEBUG > 0) {
    echo "Read\"".$response."\"<BR>\n";
    flush();
    echo "Closing socket<BR>\n";
    flush();
  }
                                                                                                             
  socket_close($socket);
    
  if (DEBUG > 0) {
    echo "Closed socket<BR>\n";
     flush();
  }

  echo $response;
}
