#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "bpsr_def.h"
#include "bpsr_udp.h"

#include "ascii_header.h"
#include "daemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

/* #define _DEBUG 1 */

void usage()
{
  fprintf (stdout,
           "bpsr_dbmon [options]\n"
           " -k         hexadecimal shared memory key  [default: %x]\n"
           " -D <path>  add a disk to which data will be written\n"
           " -s         single transfer only\n"
           " -d         run as daemon\n", DADA_DEFAULT_BLOCK_KEY);
}

typedef struct {

  /* current utc start, as defined by UTC_START attribute */
  char utc_start[64];

  uint64_t obs_offset;

  int header_written;

  int pol0_fd;

  int pol1_fd;

  int verbose;

  /* path to output files */
  char * path;

  /* current filename */
  char pol0_file_name [FILENAME_MAX];

  /* current filename */
  char pol1_file_name [FILENAME_MAX];

  /* Bytes per second */
  int bytes_per_second;

  /* internal byte counter for output */
  uint64_t counter;

  /* Sum of the pol 0 spectra */
  unsigned int * pol0_spectra;

  /* Sum of the pol 1 spectra */
  unsigned int * pol1_spectra;

} bpsr_dbmon_t;

#define BPSR_DBMON_INIT { "", 0, 0, 0, 0, 0, "", "", "", 0, 0, 0, 0 }

/*! Function that opens the data transfer target */
int file_open_function (dada_client_t* client)
{
  /* the bpsr_dbmon specific data */
  bpsr_dbmon_t* dbmon = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* utc start, as defined by UTC_START attribute */
  char utc_start[64] = "";

  /* the optimal buffer size for writing to file */
  uint64_t optimal_bytes = 0;

  /* observation offset from the UTC_START */
  uint64_t obs_offset = 0;

  /* observation offset from the UTC_START */
  int bytes_per_second = 0;

  assert (client != 0);

  dbmon = (bpsr_dbmon_t*) client->context;
  assert (dbmon != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  /* Get the UTC_START */
  if (ascii_header_get (client->header, "UTC_START", "%s", utc_start) != 1) {
    multilog (log, LOG_WARNING, "Header with no UTC_START\n");
    strcpy (utc_start, "UNKNOWN");
  }

  /* Get the BYTES_PER_SECOND */
  if (ascii_header_get (client->header, "BYTES_PER_SECOND", "%d", &bytes_per_second) != 1) {
    multilog (log, LOG_WARNING, "Header with no BYTES_PER_SECOND\n");
    bytes_per_second = 32000000;
  }
  dbmon->bytes_per_second = bytes_per_second;

  /* Get the OBS_OFFSET */
  if (ascii_header_get (client->header, "OBS_OFFSET", "%"PRIu64, &obs_offset) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }

#ifdef _DEBUG
  fprintf (stderr, "dbmon: copy the utc_start and obs_offset\n");
#endif

  /* set the current observation id */
  strcpy (dbmon->utc_start, utc_start);
  dbmon->obs_offset = obs_offset;

  dbmon->header_written = 0;

#ifdef _DEBUG
  fprintf (stderr, "dbmon: create the file name\n");
#endif

  /* Create file names*/
  snprintf (dbmon->pol0_file_name, FILENAME_MAX, "%s/%s_%"PRIu64".pol0.tmp", 
            dbmon->path, utc_start, obs_offset);

  snprintf (dbmon->pol1_file_name, FILENAME_MAX, "%s/%s_%"PRIu64".pol1.tmp", 
            dbmon->path, utc_start, obs_offset);

  int flags = O_WRONLY | O_CREAT;
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  /* Open the pol0 file */
  dbmon->pol0_fd = open(dbmon->pol0_file_name, flags, mode);
  if (dbmon->pol0_fd < 0) {
    multilog (log, LOG_ERR, "Error opening %s: %s\n", dbmon->pol0_file_name,
              strerror (errno));
    return -1;
  }
  if (dbmon->verbose) 
    multilog (log, LOG_INFO, "%s_%"PRIu64".pol0.tmp opened for writing\n",
              utc_start, obs_offset);


  /* Open the pol1 file */
  dbmon->pol1_fd = open(dbmon->pol1_file_name, flags, mode);
  if (dbmon->pol1_fd < 0) {
    multilog (log, LOG_ERR, "Error opening %s: %s\n", dbmon->pol1_file_name,
              strerror (errno));
    return -1;
  }
  if (dbmon->verbose)
    multilog (log, LOG_INFO, "%s_%"PRIu64".pol1.tmp opened for writing\n",
              utc_start, obs_offset);

  /* Initialise the spectra */
  int i=0;
  for (i=0; i<BPSR_IBOB_NCHANNELS; i++) {
    dbmon->pol0_spectra[i] = 0;
    dbmon->pol1_spectra[i] = 0;
  }

  client->transfer_bytes = dbmon->bytes_per_second * 5;
  client->optimal_bytes = 512 * BPSR_IBOB_NCHANNELS;

  return 0;
}

/*! Function that closes the data file */
int file_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the bpsr_dbmon specific data */
  bpsr_dbmon_t* dbmon = 0;

  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  dbmon = (bpsr_dbmon_t*) client->context;
  assert (dbmon != 0);

  log = client->log;
  assert (log != 0);

  /* Write the 2 files */

  write(dbmon->pol0_fd, dbmon->pol0_spectra, BPSR_IBOB_NCHANNELS*sizeof(unsigned int));
  write(dbmon->pol1_fd, dbmon->pol1_spectra, BPSR_IBOB_NCHANNELS*sizeof(unsigned int));

  if (close (dbmon->pol0_fd) < 0)
    multilog (log, LOG_ERR, "Error closing %s: %s\n", 
              dbmon->pol0_file_name, strerror(errno));

  if (close (dbmon->pol1_fd) < 0)
    multilog (log, LOG_ERR, "Error closing %s: %s\n",
              dbmon->pol1_file_name, strerror(errno));


  if (!bytes_written)  {

    multilog (log, LOG_ERR, "Removing empty file: %s\n", dbmon->pol0_file_name);
    multilog (log, LOG_ERR, "Removing empty file: %s\n", dbmon->pol1_file_name);

    if (chmod (dbmon->pol0_file_name, S_IRWXU) < 0)
      multilog (log, LOG_ERR, "Error chmod (%s, rwx): %s\n", 
        	dbmon->pol0_file_name, strerror(errno));
    
    if (chmod (dbmon->pol1_file_name, S_IRWXU) < 0)
      multilog (log, LOG_ERR, "Error chmod (%s, rwx): %s\n", 
        	dbmon->pol1_file_name, strerror(errno));

    if (remove (dbmon->pol0_file_name) < 0)
      multilog (log, LOG_ERR, "Error remove (%s): %s\n", 
        	dbmon->pol0_file_name, strerror(errno));
    
    if (remove (dbmon->pol1_file_name) < 0)
      multilog (log, LOG_ERR, "Error remove (%s): %s\n", 
        	dbmon->pol1_file_name, strerror(errno));
    
  } else {

    char cmd[1024];
    sprintf(cmd, "mv %s %s", dbmon->pol0_file_name, dbmon->pol0_file_name);
    cmd[strlen(cmd)-4] = '\0';
    system(cmd);

    sprintf(cmd, "mv %s %s", dbmon->pol1_file_name, dbmon->pol1_file_name);
    cmd[strlen(cmd)-4] = '\0';
    system(cmd);



  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t file_write_function (dada_client_t* client, 
        		     void* data, uint64_t data_size)
{

  bpsr_dbmon_t* dbmon = (bpsr_dbmon_t *) client->context;

  /* Dont write the header to files, just pretend we did */
  if (!dbmon->header_written) {
    dbmon->header_written = 1;
    return (int64_t) data_size;

  } else {

    char * char_ptr = (char *) data;

    /* We should be processing 5 seconds worth of data, irresepective
     * of the data rate */ 
    if (data_size % BPSR_IBOB_NCHANNELS != 0)
      multilog(client->log, LOG_ERR, "Data size %"PRIu64" not a multiple of 2048\n", data_size);

    int i=0;
    int j=0;

    /* Split the data up into the 2 respective unsigned in arrays */
    for (i=0; i<data_size; i+=4) {

      j = (i % 2048) / 2;

      dbmon->pol0_spectra[j]   += (0x000000ff & (unsigned int) char_ptr[i]);
      dbmon->pol0_spectra[j+1] += (0x000000ff & (unsigned int) char_ptr[i+1]);
      dbmon->pol1_spectra[j]   += (0x000000ff & (unsigned int) char_ptr[i+2]);
      dbmon->pol1_spectra[j+1] += (0x000000ff & (unsigned int) char_ptr[i+3]);
      //fprintf(stderr, "%d: %u %u\n", j, dbmon->pol0_spectra[j], dbmon->pol1_spectra[j]);
      //fprintf(stderr, "%d: %u %u\n", j+1, dbmon->pol0_spectra[j+1], dbmon->pol1_spectra[j+1]);


    }

    return (int64_t) data_size;

  }
}


int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  bpsr_dbmon_t dbmon = BPSR_DBMON_INIT;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Primary Read Client main loop */
  dada_client_t* client = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* Path to write the data files */
  char * path;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  while ((arg=getopt(argc,argv,"k:dD:vWs")) != -1)
    switch (arg) {

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"bpsr_dbmon: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    case 'd':
      daemon=1;
      break;
      
    case 'D':
      if (optarg) 
        dbmon.path = strdup(optarg);
      break;
      
    case 'v':
      verbose=1;
      break;
      
    case 's':
      quit = 1;
      break;

    default:
      usage ();
      return 0;
      
    }

  log = multilog_open ("bpsr_dbmon", daemon);

  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, BPSR_DEFAULT_DBMON_LOG);
  }
  else
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

  client->open_function  = file_open_function;
  client->io_function    = file_write_function;
  client->close_function = file_close_function;
  client->direction      = dada_client_reader;

  dbmon.pol0_spectra = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
  dbmon.pol1_spectra = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
  dbmon.verbose = verbose;

  client->context = &dbmon;

  while (!client->quit) {

    if (dada_client_read (client) < 0)
      multilog (log, LOG_ERR, "Error during transfer\n");

    if (quit) {
      client->quit = 1;
    }

  }

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
