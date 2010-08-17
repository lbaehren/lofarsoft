#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "bpsr_def.h"
#include "bpsr_udp.h"

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

#include <cpgplot.h>

#define BPSR_DBPLOT_INIT { 0, 0, 0, 0, 0, "", 0 }

typedef struct {

  int header_written;
  int acclen;
  uint64_t bytes_per_second;
  uint64_t spectra_per_second;
  uint64_t counter;
  char * device;
  int64_t spectra[(BPSR_UDP_DATASIZE_BYTES+1)];        // Sum of the spectra + 1 (counter)
  float ymin;
  float ymax;

}dbplot_t;


void usage()
{
  fprintf (stdout,
     "bpsr_dbplot [options]\n"
     " -a acclen  iBob accumulation length [default %d]\n"
     " -k key     connect to key data block\n"
     " -s         single transfer only\n"
     " -D device  pgplot device name [default ?]\n"
     " -v         be verbose\n"
     " -d         run as daemon\n", BPSR_DEFAULT_ACC_LEN);
}


/*! Function that opens the data transfer target */
int dbplot_open_function (dada_client_t* client)
{

  dbplot_t* dbplot = (dbplot_t *) client->context;

  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* size of each transfer in bytes, as defined by TRANSFER_SIZE attribute */
  uint64_t xfer_size = 64*1024*1024;

  /* the optimal buffer size for writing to file */
  uint64_t optimal_bytes = 8*1024*1024;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  dbplot->header_written = 0;

  //cpgpap(3.0, 0.8);
  /* multilog (log, LOG_INFO, "Ready for writing %"PRIu64" bytes\n", xfer_size); */

  client->transfer_bytes = xfer_size;
  client->optimal_bytes = optimal_bytes;
  client->fd = 1;

  return 0;
}

/*! Function that closes the data file */
int dbplot_close_function (dada_client_t* client, uint64_t bytes_written)
{

  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  if (bytes_written < client->transfer_bytes) {
    multilog (log, LOG_INFO, "Transfer stopped early at %"PRIu64" bytes\n",
	      bytes_written);
  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t dbplot_buffer_function (dada_client_t* client, 
			    void* data, uint64_t data_size)
{
  dbplot_t* dbplot = (dbplot_t *) client->context;

  /* Dont plot the header silly */
  if (!dbplot->header_written) {

    dbplot->header_written = 1;
    return (int64_t) data_size;

  } else {

    int i=0;
    int j=0;
    int k=0;

    float x_points[1024];
    float pol0_points[1024];
    float pol1_points[1024];

    int index = (int) (dbplot->counter % 2048);

    char * ptr = (char *) data;
    unsigned int val;

    /* assume acclen=25, 15625 Spectra/sec, 32,000,000 bytes/sec */
    //dbplot->ymin = 0;
    //dbplot->ymax = 0;

    for (i=0; i<data_size; i++) {

      /* Do a data dump, when 1 seconds data has been collected */
      if (dbplot->spectra[BPSR_UDP_DATASIZE_BYTES] == dbplot->spectra_per_second) {

        /* Plotting stuff here */
        //cpgpage ();
        
        /* Get the min/max */
        for (j=0; j<512; j++) {

          pol0_points[(2*j)] = (float) dbplot->spectra[j*4];
          pol0_points[(2*j)+1] = (float) dbplot->spectra[(j*4)+1];
          pol1_points[(2*j)] = (float) dbplot->spectra[(j*4)+2];
          pol1_points[(2*j)+1] = (float) dbplot->spectra[(j*4)+3];

        }

        for (j=0; j<1024; j++) {

          x_points[j] = (float) j;
          if (pol0_points[j] < dbplot->ymin) dbplot->ymin = pol0_points[j];
          if (pol0_points[j] > dbplot->ymax) dbplot->ymax = pol0_points[j];
          if (pol1_points[j] < dbplot->ymin) dbplot->ymin = pol1_points[j];
          if (pol1_points[j] > dbplot->ymax) dbplot->ymax = pol1_points[j];

        }

        cpgbbuf();
        cpgenv(0, 1024, 0, (1.1*dbplot->ymax), 0, 0);
        cpglab("Frequency Channel", "Intensity", "Intensity vs Frequency Channel"); 
 
        cpgsci(1);

        float x = 0;
        float y = 0;

        /* Red for Pol 0 */
        cpgsci(2);
        cpgmtxt("T", 1.5, 0.0, 0.0, "Pol 0");
        cpgline(1024, x_points, pol0_points);

        /* Green for Pol 1*/
        cpgsci(3);
        cpgmtxt("T", 0.5, 0.0, 0.0, "Pol 1");
        cpgline(1024, x_points, pol1_points);

        cpgsci(1);
        cpgebuf();

        /* Reset the values */
        for (j=0; j<1024; j++) {
          dbplot->spectra[2*j] = 0;
          dbplot->spectra[(2*j)+1] = 0;
        }
        dbplot->spectra[2048] = 0;

      }

      /* add the "val" to the appropriate channel counter */
      val = (unsigned int) ptr[i];
      val &= 0x000000ff;
      dbplot->spectra[index] += (unsigned int) val;
      index++;

      /* Once a time slice has been assigned */
      if (index == 2048) {
        dbplot->spectra[2048]++;
        index = 0;
      }

      dbplot->counter++;

    }

    return (int64_t) data_size;

  }
}


int main (int argc, char **argv)
{

  /* context struct */
  dbplot_t dbplot= BPSR_DBPLOT_INIT;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Primary Read Client main loop */
  dada_client_t* client = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Accumulation length */
  int acclen = BPSR_DEFAULT_ACC_LEN;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* PGPLOT device name */
  char * device = "?";

  /* dada key for SHM */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int single_xfer_only = 0;

  int arg = 0;

  /* TODO the amount to conduct a busy sleep inbetween clearing each sub
   * block */
  int busy_sleep = 0;

  while ((arg=getopt(argc,argv,"dD:a:svk:")) != -1)
    switch (arg) {
      
    case 'd':
      daemon=1;
      break;

    case 'a':
      acclen = atoi(optarg);
      break;

    case 's':
      single_xfer_only = 1;
      break;

    case 'D':
      device = optarg;
      break;

    case 'v':
      verbose=1;
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr, "dada_db: could not parse key from %s\n", optarg);
        return -1;
      }
      break;
      
    default:
      usage ();
      return 0;
      
  }

  dbplot.device = strdup(device);
  dbplot.acclen = acclen;
  dbplot.header_written = 0;
  dbplot.spectra_per_second = (BPSR_IBOB_CLOCK * 1000000 / dbplot.acclen ) / BPSR_IBOB_NCHANNELS;
  dbplot.bytes_per_second = dbplot.spectra_per_second * BPSR_UDP_DATASIZE_BYTES;
  dbplot.counter = 0;
                                                                                                       
  /* Reset the spectra counters */
  int i=0;
  for (i=0; i<BPSR_UDP_DATASIZE_BYTES; i++) {
    dbplot.spectra[i] = 0;
  }
  dbplot.spectra[(BPSR_UDP_DATASIZE_BYTES+1)];
                                                                                                       
  /* Open pgplot device window */
  if (cpgopen(dbplot.device) != 1) {
    multilog(log, LOG_INFO, "bpsr_dbplot: error opening plot device\n");
    exit(1);
  }
  cpgask(0);

  log = multilog_open ("bpsr_dbplot", daemon);

  multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_read (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;

  client->data_block = hdu->data_block;
  client->header_block = hdu->header_block;

  client->open_function  = dbplot_open_function;
  client->io_function    = dbplot_buffer_function;
  client->close_function = dbplot_close_function;
  client->direction      = dada_client_reader;

  client->context = &dbplot;

  while (!quit) {
    
    if (dada_client_read (client) < 0)
      multilog (log, LOG_ERR, "Error during transfer\n");

    if (single_xfer_only)
      quit = 1;

    if (client->quit)
      quit = 1;

  }

  cpgclos();

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
