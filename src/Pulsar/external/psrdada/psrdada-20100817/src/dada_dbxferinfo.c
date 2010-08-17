#include "dada_hdu.h"
#include "dada_def.h"
#include "ipcbuf.h"

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
     "dada_dbxferinfo [options]\n"
     " -k         hexadecimal shared memory key  [default: %x]\n"
     " -v         be verbose\n"
     " -d         run as daemon\n", DADA_DEFAULT_BLOCK_KEY);
}

const char * state_to_str(int state);

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

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  /* TODO the amount to conduct a busy sleep inbetween clearing each sub
   * block */

  while ((arg=getopt(argc,argv,"k:dv")) != -1)
    switch (arg) {
     
    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"dada_dbxferinfo: could not parse key from %s\n",
                         optarg);
        return -1;
      }
      break;

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

  log = multilog_open ("dada_dbmonitor", daemon);

  if (daemon) 
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, DADA_DEFAULT_DBMONITOR_LOG);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (verbose)
    printf("Connecting to data block\n");
  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  /* get a pointer to the data block */
 
  ipcbuf_t *hb = hdu->header_block;
  ipcbuf_t *db = (ipcbuf_t *) hdu->data_block;
          
  uint64_t bufsz = ipcbuf_get_bufsz (hb);
  uint64_t nhbufs = ipcbuf_get_nbufs (hb);

  uint64_t total_bytes = nhbufs * bufsz;
  if (verbose) {
    fprintf(stderr,"HEADER BLOCK:\n");
    fprintf(stderr,"Number of buffers: %"PRIu64"\n",nhbufs);
    fprintf(stderr,"Buffer size: %"PRIu64"\n",bufsz);
    fprintf(stderr,"Total buffer memory: %5.0f MB\n", ((double) total_bytes) / 
                                                       (1024.0*1024.0));
  }

  bufsz = ipcbuf_get_bufsz (db);
  uint64_t ndbufs = ipcbuf_get_nbufs (db);
  total_bytes = ndbufs * bufsz;

  fprintf(stderr,"DATA BLOCK:\n");
  fprintf(stderr,"Number of buffers: %"PRIu64"\n",ndbufs);
  fprintf(stderr,"Buffer size: %"PRIu64"\n",bufsz);
  fprintf(stderr,"Total buffer memory: %5.0f MB\n", ((double) total_bytes) / 
                                                       (1024.0*1024.0));

  fprintf(stderr, "\n");
  fprintf(stderr, "sync->r_buf:   %"PRIu64"\n", db->sync->r_buf);
  fprintf(stderr, "sync->w_buf:   %"PRIu64"\n", db->sync->w_buf);
  fprintf(stderr, "sync->w_state: %s\n", state_to_str(db->sync->w_state));
  fprintf(stderr, "sync->r_state: %s\n", state_to_str(db->sync->r_state));
  fprintf(stderr, "IPCBUF_SODACK: %"PRIu64"\n", ipcbuf_get_sodack(db));
  fprintf(stderr, "IPCBUF_EODACK: %"PRIu64"\n", ipcbuf_get_eodack(db));
  
  int i=0;

  fprintf(stderr, "\n");
  fprintf(stderr, "           START              END            EOD\n");
  fprintf(stderr, "ID [    buf,   byte]    [    buf,   byte]   FLAG\n");
  fprintf(stderr, "=================================================\n");
  for (i=0;i<IPCBUF_XFERS;i++) {
    fprintf(stderr,"%2d [%7"PRIu64",%7"PRIu64"] => [%7"PRIu64",%7"PRIu64"]   %4d", 
                    i, db->sync->s_buf[i],db->sync->s_byte[i],
                    db->sync->e_buf[i],db->sync->e_byte[i],
                    db->sync->eod[i]);

    if (i == db->sync->w_xfer) 
      fprintf(stderr, " W");
    else
      fprintf(stderr, "  ");
    if (i == db->sync->r_xfer) 
      fprintf(stderr, " R");
    else 
      fprintf(stderr, "  ");

    fprintf(stderr,"\n");

  }

  if (verbose) {
    /* Note there is only 1 XFER in the header block, and it doesn't even have
     * a proper xfer concept in it */
          
    fprintf(stderr,"\nHeader Block Xfers:\n");
    fprintf(stderr,"[%"PRIu64",%"PRIu64"]=>[%"PRIu64",%"PRIu64"] %d\n",
                   hb->sync->s_buf[0],hb->sync->s_byte[0],
                   hb->sync->e_buf[0],hb->sync->e_byte[0],hb->sync->eod[0]);

  }

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}


const char * state_to_str(int state) 
{

  switch (state) 
  {
    case 0:
      return "disconnected";

    case 1:
      return "connected";

    case 2:
      return "one process that writes to the buffer";

    case 3:
      return "start-of-data flag has been raised";

    case 4:
      return "next operation will change writing state";

    case 5:
      return "one process that reads from the buffer";

    case 6:
      return "start-of-data flag has been raised";

    case 7:
      return "end-of-data flag has been raised";

    case 8:
      return "currently viewing";

    case 9:
      return "end-of-data while viewer";

    default:
      return "unknown";

  }

}


