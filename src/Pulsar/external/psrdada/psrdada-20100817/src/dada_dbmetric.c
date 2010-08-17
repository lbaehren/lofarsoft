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
     "dada_dbmetric [options]\n"
     " -k         hexadecimal shared memory key  [default: %x]\n"
     " -v         be verbose\n", DADA_DEFAULT_BLOCK_KEY);
}


int main (int argc, char **argv)
{

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* dada key for SHM */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  while ((arg=getopt(argc,argv,"vk:")) != -1)
    switch (arg) {
                                                                                
    case 'v':
      verbose=1;
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr, "dada_dbmetric: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    default:
      usage ();
      return 0;
                                                                                
  }
  
  log = multilog_open ("dada_dbmetric", 0);
  multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  /* get a pointer to the data block */

  uint64_t full_bufs = 0;
  uint64_t clear_bufs = 0;
  uint64_t bufs_read = 0;
  uint64_t bufs_written = 0;

  ipcbuf_t *hb = hdu->header_block;
  ipcbuf_t *db = (ipcbuf_t *) hdu->data_block;
          
  uint64_t bufsz = ipcbuf_get_bufsz (hb);
  uint64_t nhbufs = ipcbuf_get_nbufs (hb);

  if (verbose) 
    fprintf(stderr,"TOTAL,FULL,CLEAR,W_BUF,R_BUF,TOTAL,FULL,CLEAR,W_BUF,"
                   "R_BUF\n");

  bufs_written = ipcbuf_get_write_count (db);
  bufs_read = ipcbuf_get_read_count (db);
  full_bufs = ipcbuf_get_nfull (db);
  clear_bufs = ipcbuf_get_nclear (db);

  bufsz = ipcbuf_get_bufsz (db);
  uint64_t ndbufs = ipcbuf_get_nbufs (db);
    
  fprintf(stderr,"%"PRIu64",%"PRIu64",%"PRIu64",%"PRIu64",%"PRIu64",",
                 ndbufs, full_bufs, clear_bufs, bufs_written, bufs_read);

  bufs_written = ipcbuf_get_write_count (hb);
  bufs_read = ipcbuf_get_read_count (hb);
  full_bufs = ipcbuf_get_nfull (hb);
  clear_bufs = ipcbuf_get_nclear (hb);
    

  fprintf(stderr,"%"PRIu64",%"PRIu64",%"PRIu64",%"PRIu64",%"PRIu64"\n",
                 nhbufs, full_bufs, clear_bufs, bufs_written, bufs_read);


  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
