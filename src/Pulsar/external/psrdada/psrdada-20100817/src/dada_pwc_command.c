#include "dada_pwc_nexus.h"
#include "daemon.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* #define _DEBUG 1 */

void usage()
{
  fprintf (stdout,
	   "dada_pwc_command [options] config_file\n"
     " -h         print help text\n"
	   " -d         run as daemon\n"
	   " -v         verbose messages\n");
}

int main (int argc, char **argv)
{
  /* the DADA nexus command distribution interface */
  dada_pwc_nexus_t* nexus = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  int arg = 0;

  /* configuration file name */
  char* dada_config;

  multilog_fprintf (stderr, LOG_INFO, "Creating DADA PWC nexus\n");
  nexus = dada_pwc_nexus_create ();

  while ((arg=getopt(argc,argv,"d:vh")) != -1)
    switch (arg) {
      
    case 'd':
      daemon=1;
      break;

    case 'v':
      verbose=1;
      break;
      
    case 'h':
      usage();
      return 0;

    default:
      usage ();
      return 0;
      
    }

  if ((argc - optind) != 1) {
    fprintf(stderr, "Error: config_file must be specified\n\n");
    usage();
    return EXIT_FAILURE;
  } else {
    dada_config = argv[optind];
  }

  log = multilog_open ("dada_nexus", daemon);

  if (daemon)
  {
    be_a_daemon ();
    multilog_serve (log, 123);
  }
  else
    multilog_add (log, stderr);

  multilog_fprintf (stderr, LOG_INFO, "Configuring DADA PWC nexus\n");
  if (dada_pwc_nexus_configure (nexus, dada_config) < 0)
  {
    multilog_fprintf (stderr, LOG_ERR, 
		      "Error while configuring the DADA nexus\n");
    return -1;
  }

  multilog_fprintf (stderr, LOG_INFO, "Running the DADA PWC nexus server\n");
  if (dada_pwc_nexus_serve (nexus) < 0)
  {
    multilog_fprintf (stderr, LOG_ERR,
		     "Error while running the DADA PWC nexus server\n");
    return -1;
  }

  multilog_fprintf (stderr, LOG_INFO, "Destroying nexus\n");
  dada_pwc_nexus_destroy (nexus);

  return EXIT_SUCCESS;
}

