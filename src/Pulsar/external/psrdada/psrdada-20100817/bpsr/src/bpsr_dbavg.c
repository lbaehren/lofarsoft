#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "bpsr_def.h"

#include "disk_array.h"
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

#define DRATE_DOWNSCALE_FACTOR 8;

void usage()
{
  fprintf (stdout,
           "bpsr_dbavg [options]\n"
           " -k         hexadecimal shared memory key  [default: %x]\n"
           " -D <path>  add a disk to which data will be written\n"
           " -W         over-write exisiting files\n"
           " -s         single transfer only\n"
           " -d         run as daemon\n", DADA_DEFAULT_BLOCK_KEY);
}

typedef struct {

  /* the set of disks to which data may be written */
  disk_array_t* array;

  /* current utc start, as defined by UTC_START attribute */
  char utc_start[64];

  uint64_t obs_offset;

  int header_written;

  int pol1_fd;

  int pol2_fd;

  /* current filename */
  char pol1_file_name [FILENAME_MAX];

  /* current filename */
  char pol2_file_name [FILENAME_MAX];

  /* Bytes per second */
  int bytes_per_second;

  /* file offset from start of data, as defined by FILE_NUMBER attribute */
  unsigned file_number;

  /* internal byte counter for output */
  uint64_t counter;

  /* Sum of the spectra + 1 (counter) */
  int64_t spectra[2049];


} bpsr_dbavg_t;

#define BPSR_DBAVG_INIT { 0, "", 0, 0, 0, 0, "", "", 0, 0 }

/*! Function that opens the data transfer target */
int file_open_function (dada_client_t* client)
{
  /* the bpsr_dbavg specific data */
  bpsr_dbavg_t* dbavg = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* utc start, as defined by UTC_START attribute */
  char utc_start[64] = "";

  /* size of each file to be written in bytes, as determined by FILE_SIZE */
  uint64_t file_size = 0;

  /* the optimal buffer size for writing to file */
  uint64_t optimal_bytes = 0;

  /* the open file descriptor */
  int fd = -1;

  /* observation offset from the UTC_START */
  uint64_t obs_offset = 0;

  /* observation offset from the UTC_START */
  int bytes_per_second = 0;

  assert (client != 0);

  dbavg = (bpsr_dbavg_t*) client->context;
  assert (dbavg != 0);
  assert (dbavg->array != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  /* Get the UTC_START */
  if (ascii_header_get (client->header, "UTC_START", "%s", utc_start) != 1) {
    multilog (log, LOG_WARNING, "Header with no UTC_START\n");
    strcpy (utc_start, "UNKNOWN");
  }

  /* Get the UTC_START */
  if (ascii_header_get (client->header, "BYTES_PER_SECOND", "%d", &bytes_per_second) != 1) {
     multilog (log, LOG_WARNING, "Header with no BYTES_PER_SECOND\n");
     bytes_per_second = 32000000;
  }
  dbavg->bytes_per_second = bytes_per_second;

  /* Get the OBS_OFFSET */
  if (ascii_header_get (client->header, "OBS_OFFSET", "%"PRIu64, &obs_offset) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }

  /* check to see if we are still working with the same observation */
  if ((strcmp (utc_start, dbavg->utc_start) != 0) || 
      (obs_offset != dbavg->obs_offset)) {
    dbavg->file_number = 0;
    multilog (log, LOG_INFO, "New UTC_START=%s, OBS_OFFSET=%"PRIu64" -> "
              "file number=0\n", utc_start, obs_offset);
  }
  else {
    dbavg->file_number++;
    multilog (log, LOG_INFO, "Continue UTC_START=%s, OBS_OFFSET=%"PRIu64" -> "
              "file number=%lu\n", utc_start, obs_offset, dbavg->file_number);
  }

  /* Set the file number to be written to the header */
  if (ascii_header_set (header, "FILE_NUMBER", "%u", dbavg->file_number)<0) {
    multilog (log, LOG_ERR, "Error writing FILE_NUMBER\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "dbavg: copy the utc_start and obs_offset\n");
#endif

  /* set the current observation id */
  strcpy (dbavg->utc_start, utc_start);
  dbavg->obs_offset = obs_offset;

  dbavg->header_written = 0;

#ifdef _DEBUG
  fprintf (stderr, "dbavg: create the file name\n");
#endif

  /* create the current file name */
  snprintf (dbavg->pol1_file_name, FILENAME_MAX, "pol1_%s_%"PRIu64".%06u.csv", 
            utc_start, obs_offset, dbavg->file_number);

  snprintf (dbavg->pol2_file_name, FILENAME_MAX, "pol2_%s_%"PRIu64".%06u.csv", 
            utc_start, obs_offset, dbavg->file_number);

  /* Get the file size */
  if (ascii_header_get (header, "FILE_SIZE", "%"PRIu64, &file_size) != 1) {
    multilog (log, LOG_WARNING, "Header with no FILE_SIZE\n");
    file_size = DADA_DEFAULT_FILESIZE;
  }

#ifdef _DEBUG
  fprintf (stderr, "dbavg: open the file\n");
#endif

  /* Open the pol1 file */
  fd = disk_array_open (dbavg->array, dbavg->pol1_file_name,
        		file_size, &optimal_bytes, 0);

  if (fd < 0) {
    multilog (log, LOG_ERR, "Error opening %s: %s\n", dbavg->pol1_file_name,
              strerror (errno));
    return -1;
  }

  multilog (log, LOG_INFO, "%s opened for writing %"PRIu64" bytes\n",
            dbavg->pol2_file_name, file_size);

  dbavg->pol1_fd = fd;

  /* Open the pol2 file */
  fd = disk_array_open (dbavg->array, dbavg->pol2_file_name,
            file_size, &optimal_bytes, 0);
                                                                                                                                                                         
  if (fd < 0) {
    multilog (log, LOG_ERR, "Error opening %s: %s\n", dbavg->pol2_file_name,
              strerror (errno));
    return -1;
  }
                                                                                                                                                                         
  multilog (log, LOG_INFO, "%s opened for writing %"PRIu64" bytes\n",
            dbavg->pol2_file_name, file_size);
                                                                                                                                                                         
  dbavg->pol2_fd = fd;

  client->transfer_bytes = file_size;
  client->optimal_bytes = 512 * optimal_bytes;

  return 0;
}

/*! Function that closes the data file */
int file_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the bpsr_dbavg specific data */
  bpsr_dbavg_t* dbavg = 0;

  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  dbavg = (bpsr_dbavg_t*) client->context;
  assert (dbavg != 0);

  log = client->log;
  assert (log != 0);

  if (close (dbavg->pol1_fd) < 0)
    multilog (log, LOG_ERR, "Error closing %s: %s\n", 
              dbavg->pol1_file_name, strerror(errno));

  if (close (dbavg->pol2_fd) < 0)
    multilog (log, LOG_ERR, "Error closing %s: %s\n",
              dbavg->pol2_file_name, strerror(errno));


  if (!bytes_written)  {

    multilog (log, LOG_ERR, "Removing empty file: %s\n", dbavg->pol1_file_name);
    multilog (log, LOG_ERR, "Removing empty file: %s\n", dbavg->pol2_file_name);

    if (chmod (dbavg->pol1_file_name, S_IRWXU) < 0)
      multilog (log, LOG_ERR, "Error chmod (%s, rwx): %s\n", 
        	dbavg->pol1_file_name, strerror(errno));
    
    if (chmod (dbavg->pol2_file_name, S_IRWXU) < 0)
      multilog (log, LOG_ERR, "Error chmod (%s, rwx): %s\n", 
        	dbavg->pol2_file_name, strerror(errno));

    if (remove (dbavg->pol1_file_name) < 0)
      multilog (log, LOG_ERR, "Error remove (%s): %s\n", 
        	dbavg->pol1_file_name, strerror(errno));
    
    if (remove (dbavg->pol2_file_name) < 0)
      multilog (log, LOG_ERR, "Error remove (%s): %s\n", 
        	dbavg->pol2_file_name, strerror(errno));
    
  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t file_write_function (dada_client_t* client, 
        		     void* data, uint64_t data_size)
{
  bpsr_dbavg_t* dbavg = (bpsr_dbavg_t *) client->context;

  /* Dont write the header to files, just pretend we did */
  if (!dbavg->header_written) {
    dbavg->header_written = 1;
    return (int64_t) data_size;

  } else {

    int i=0;
    int j=0;
  
    /* should be 15625 for acclen=25, tsamp = 64 us */
    int output_n_spec = dbavg->bytes_per_second / 2048;

    int index = (int) (dbavg->counter % 2048);

    char * ptr = (char *) data;
    unsigned int val;

    /* assume acclen=25, 15625 Spectra/sec, 32,000,000 bytes/sec */

    for (i=0; i<data_size; i++) {

      /* Do a data dump */
      if (dbavg->spectra[2048] == output_n_spec) {

        dprintf(dbavg->pol1_fd, "%"PRIu64, dbavg->counter);
        dprintf(dbavg->pol2_fd, "%"PRIu64, dbavg->counter);

        for (j=0; j<1024; j++) {
          dprintf(dbavg->pol1_fd, ",%u", dbavg->spectra[2*j]);
          dprintf(dbavg->pol2_fd, ",%u", dbavg->spectra[(2*j)+1]);

          /* Reset the values */
          dbavg->spectra[2*j] = 0;
          dbavg->spectra[(2*j)+1] = 0;
        }

        dprintf(dbavg->pol1_fd, "\n");
        dprintf(dbavg->pol2_fd, "\n");

        dbavg->spectra[2048] = 0;

      } 

      val = (unsigned int) ptr[i];
      val &= 0x000000ff;

      //fprintf(stderr, "spectra[%d] <= ptr[%d] : %u\n", index, i, val);

      dbavg->spectra[index] += (unsigned int) val;

      index++;

      /* Once a time slice has been assigned */
      if (index == 2048) {
        dbavg->spectra[2048]++;
        index = 0;
      }

      dbavg->counter++;

    }  

    return (int64_t) data_size;

  }
}


int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  bpsr_dbavg_t dbavg = BPSR_DBAVG_INIT;

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

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  dbavg.array = disk_array_create ();

  while ((arg=getopt(argc,argv,"k:dD:vWs")) != -1)
    switch (arg) {

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"bpsr_dbavg: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    case 'd':
      daemon=1;
      break;
      
    case 'D':
      if (disk_array_add (dbavg.array, optarg) < 0) {
        fprintf (stderr, "Could not add '%s' to disk array\n", optarg);
        return EXIT_FAILURE;
      }
      break;
      
    case 'v':
      verbose=1;
      break;
      
    case 'W':
      disk_array_set_overwrite (dbavg.array, 1);
      break;

    case 's':
      quit = 1;
      break;

    default:
      usage ();
      return 0;
      
    }

  log = multilog_open ("bpsr_dbavg", daemon);

  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, BPSR_DEFAULT_DBAVG_LOG);
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

  client->context = &dbavg;

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
