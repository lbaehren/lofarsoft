#include "dada_hdu.h"
#include "dada_def.h"

#include "node_array.h"
#include "string_array.h"
#include "ascii_header.h"
#include "daemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void usage()
{
  fprintf (stdout,
     "dada_header [options]\n"
     " -k         hexadecimal shared memory key  [default: %x]\n"
     " -p         read header block as a passive viewer \n"
     " -v         be verbose\n",DADA_DEFAULT_BLOCK_KEY);
}

int main (int argc, char **argv)
{

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Open as a passive viewer */
  char passive = 0;

 /* dada key for SHM */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  /* TODO the amount to conduct a busy sleep inbetween clearing each sub
   * block */

  while ((arg=getopt(argc,argv,"pvk:")) != -1)
    switch (arg)
    {
    case 'p':
      passive = 1;
      break;

    case 'v':
      verbose=1;
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1)
      {
        fprintf (stderr, "dada_header: could not parse key from %s\n", optarg);
        return -1;
      }
      break;

    default:
      usage ();
      return 0;
    }

  log = multilog_open ("dada_header", 0);
  multilog_add (log, stderr);
  hdu = dada_hdu_create (log);

  /* Set the particular dada key */
  dada_hdu_set_key(hdu, dada_key);

  if (verbose) 
    fprintf(stderr,"Connecting to header block\n");

  if (dada_hdu_connect (hdu) < 0)
  {
    fprintf(stderr, "Could not connect to the header and data blocks\n");
    return EXIT_FAILURE;
  }

  if (!passive && dada_hdu_lock_read(hdu) < 0)
  {
    fprintf(stderr, "Could not lock Header Block for reading\n");
    return EXIT_FAILURE;
  }

  if (passive && dada_hdu_open_view(hdu) < 0)
  {
    fprintf(stderr, "Could not lock Header Block for viewing\n");
    return EXIT_FAILURE;
  }


  uint64_t header_size = 0;
  char* header = 0;

  if (verbose) 
    fprintf(stderr,"Waiting for next filled header\n");

  /* Wait for the next valid header sub-block */
  header = ipcbuf_get_next_readable (hdu->header_block, &header_size);
  if (!header)
  {
    fprintf(stderr,"Could not get next header\n");
    return DADA_ERROR_FATAL;
  }

  header_size = ipcbuf_get_bufsz (hdu->header_block);

  if (verbose)
  {
    fprintf(stderr,"HEADER BEGIN\n");
    fprintf(stderr,"======================================================\n");
  }

  fprintf(stdout,"%s",header);

  if (verbose)
  {
    fprintf(stderr,"======================================================\n");
    fprintf(stderr,"HEADER END\n");
  }

  if (!passive && dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;
  
  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

