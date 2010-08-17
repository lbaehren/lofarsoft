#include "dada_hdu.h"
#include "daemon.h"

#include <unistd.h>
#include <stdlib.h>

void usage()
{
  fprintf (stdout,
	   "dada_reader [options]\n"
	   " -d run as daemon\n");
}

int main (int argc, char **argv)
{
  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log;

  /* DADA Logger port */
  int log_port = 0xdada;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  int arg = 0;
  while ((arg=getopt(argc,argv,"dv")) != -1) {
    switch (arg) {

      case 'd':
	daemon=1;
	break;

      case 'v':
        verbose=1;
        break;

      default:
	usage ();
	return 0;

    }
  }

  log = multilog_open ("dada_writer", daemon);

  /* set up for daemon usage */	  
  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, log_port);
  }

  hdu = dada_hdu_create (log);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_read (hdu) < 0)
    return EXIT_FAILURE;

  /* main loop */

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  dada_hdu_destroy (hdu);

  return EXIT_SUCCESS;
}

