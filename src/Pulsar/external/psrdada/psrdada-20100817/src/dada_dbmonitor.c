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

#define IPCBUF_EODACK 3   /* acknowledgement of end of data */

void usage()
{
fprintf (stdout,
     "dada_dbmonitor [options]\n"
     " -k         hexadecimal shared memory key  [default: %x]\n"
     " -v         be verbose\n"
     " -d         run as daemon\n", DADA_DEFAULT_BLOCK_KEY);
}


int main (int argc, char **argv)
{

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;


  int arg = 0;

  /* TODO the amount to conduct a busy sleep inbetween clearing each sub
   * block */

  while ((arg=getopt(argc,argv,"dvk:")) != -1)
    switch (arg) {

    case 'd':
      daemon=1;
      break;

    case 'v':
      verbose=1;
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"dada_dbmonitor: could not parse key from %s\n",optarg);
        return -1;
      }
      break;
      
    default:
      usage ();
      return 0;

    }

  log = multilog_open ("dada_dbmonitor", daemon);

  if (daemon)
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, DADA_DEFAULT_DBMONITOR_LOG);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);
  
  printf("connecting\n");
  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;


  /* get a pointer to the data block */

  uint64_t full_bufs = 0;
  uint64_t clear_bufs = 0;
  uint64_t bufs_read = 0;
  uint64_t bufs_written = 0;
  int64_t available_bufs = 0;

  ipcbuf_t *hb = hdu->header_block;
  ipcbuf_t *db = (ipcbuf_t *) hdu->data_block;
          
  uint64_t bufsz = ipcbuf_get_bufsz (hb);
  uint64_t nhbufs = ipcbuf_get_nbufs (hb);
  uint64_t total_bytes = nhbufs * bufsz;

  fprintf(stderr,"HEADER BLOCK:\n");
  fprintf(stderr,"Number of buffers: %"PRIu64"\n",nhbufs);
  fprintf(stderr,"Buffer size: %"PRIu64"\n",bufsz);
  fprintf(stderr,"Total buffer memory: %"PRIu64" KB\n",(total_bytes/(1024)));

  bufsz = ipcbuf_get_bufsz (db);
  uint64_t ndbufs = ipcbuf_get_nbufs (db);
  total_bytes = ndbufs * bufsz;

  fprintf(stderr,"DATA BLOCK:\n");
  fprintf(stderr,"Number of buffers: %"PRIu64"\n",ndbufs);
  fprintf(stderr,"Buffer size: %"PRIu64"\n",bufsz);
  fprintf(stderr,"Total buffer memory: %"PRIu64" MB\n",(total_bytes/(1024*1024)));

  fprintf(stderr,"\nFREE\tFULL\tCLEAR\tW_BUF\tR_BUF\tFREE\tFULL\tCLEAR\t"
                 "W_BUF\tR_BUF\n");

  while (!quit) {

    bufs_written = ipcbuf_get_write_count (db);
    bufs_read = ipcbuf_get_read_count (db);
    full_bufs = ipcbuf_get_nfull (db);
    clear_bufs = ipcbuf_get_nclear (db);
    available_bufs = (ndbufs - full_bufs);
    
    fprintf(stderr,"%"PRIi64"\t%"PRIu64"\t%"PRIu64"\t%"PRIu64"\t%"PRIu64"\t",
                    available_bufs, full_bufs, clear_bufs, bufs_written, bufs_read);

    bufs_written = ipcbuf_get_write_count (hb);
    bufs_read = ipcbuf_get_read_count (hb);
    full_bufs = ipcbuf_get_nfull (hb);
    clear_bufs = ipcbuf_get_nclear (hb);
    available_bufs = (nhbufs - full_bufs);
    
    fprintf(stderr,"%"PRIi64"\t%"PRIu64"\t%"PRIu64"\t%"PRIu64"\t%"PRIu64"\n",
                    available_bufs, full_bufs, clear_bufs, bufs_written, bufs_read);

    sleep(1);
  }

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
