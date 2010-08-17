#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"

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
#define MAX_FILES 1024

void usage()
{
  fprintf (stdout,
	   "dada_diskdb [options]\n"
     " -k   hexadecimal shared memory key  [default: %x]\n"
     " -f   file to write to the ring buffer \n"
     " -s   single file then exit\n"
     " -d   run as daemon\n", DADA_DEFAULT_BLOCK_KEY);
}

typedef struct {

  /* the set of disks from which data files will be read */
  disk_array_t* array;

  /* current utc start, as defined by UTC_START attribute */
  char utc_start[64];

  /* current observation offset, as defined by OBS_OFFSET attribute */
  uint64_t obs_offset;

  /* current filename */
  char filename [MAX_FILES][FILENAME_MAX];

  /* file offset from start of data, as defined by FILE_NUMBER attribute */
  unsigned file_number;

  /* remove files after they have been transferred to the Data Block */
  char remove_files;

} dada_diskdb_t;

#define DADA_DISKDB_INIT { 0, "", 0, "", 0, 0 }

/* Number of files to load */
static int n_files = 0;
/* Current file loading */
static int cur_file = 0;


/*! Pointer to the function that transfers data to/from the target */
int64_t file_read_function (dada_client_t* client, 
			    void* data, uint64_t data_size)
{
#ifdef _DEBUG
  fprintf (stderr, "file_read_function %p %"PRIu64"\n", data, data_size);
#endif

  return read (client->fd, data, data_size);
}

/*! Function that closes the data file */
int file_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the dada_diskdb specific data */
  dada_diskdb_t* diskdb = 0;

  assert (client != 0);
  diskdb = (dada_diskdb_t*) client->context;
  assert (diskdb != 0);

  if (close (client->fd) < 0) {
    multilog (client->log, LOG_ERR, "Could not close file: %s\n",
              strerror(errno));
    return -1;
  }

  cur_file++;
  if(cur_file >= n_files){
	  diskdb->filename[cur_file][0] = '\0';
	  client->fd = -1;
	  client->quit=1;
  }
  return 0;
}

/*! Function that opens the data transfer target */
int file_open_function (dada_client_t* client)
{
  /* the dada_diskdb specific data */
  dada_diskdb_t* diskdb = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* utc start, as defined by UTC_START attribute */
  char utc_start [64] = "";

  /* observation offset, as defined by OBS_OFFSET attribute */
  uint64_t obs_offset = 0;

  /* size of each file to be written in bytes, as determined by FILE_SIZE */
  uint64_t file_size = 0;

  /* the size of the header, as determined by HDR_SIZE */
  uint64_t hdr_size = 0;

  int64_t ret = 0;

  assert (client != 0);

  diskdb = (dada_diskdb_t*) client->context;
  assert (diskdb != 0);
  assert (diskdb->array != 0);

  assert (client->header != 0);

  log = client->log;

  while (diskdb->filename[cur_file][0] == '\0') {

    if (n_files > 0)
    {
      client->quit = 1;
      return 0;
    }

    /* look for a new file in the disk array */
    fprintf (stderr, "WRITE FILE SEARCH\n");
    return -1;

  }

  client->fd = open (diskdb->filename[cur_file], O_RDONLY);

  if (client->fd < 0)  {
    multilog (client->log, LOG_ERR, "Error opening %s: %s\n",
              diskdb->filename[cur_file], strerror(errno));
    return -1;
  } 

  ret = read (client->fd, client->header, client->header_size);

  if (ret < client->header_size) {
    multilog (client->log, LOG_ERR, 
	      "read %d out of %d bytes from Header: %s\n", 
	      ret, client->header_size, strerror(errno));
    file_close_function (client, 0);
    return -1;
  }

#ifdef _DEBUG
fprintf (stderr, "read HEADER START\n%sHEADER END\n", client->header);
#endif

  /* Get the header size */
  if (ascii_header_get (client->header, "HDR_SIZE", "%"PRIu64, &hdr_size) != 1)
  {
    multilog (log, LOG_WARNING, "Header with no HDR_SIZE\n");
    hdr_size = DADA_DEFAULT_HEADER_SIZE;
  }

  /* Ensure that the incoming header fits in the client header buffer */
  if (hdr_size > client->header_size) {
    multilog (client->log, LOG_ERR, "HDR_SIZE=%u > Block size=%"PRIu64"\n",
	      hdr_size, client->header_size);
    return -1;
  }

  /* Get the file size */
  if (ascii_header_get (client->header, "FILE_SIZE", "%"PRIu64, &file_size)!=1)
  {
    multilog (log, LOG_WARNING, "Header with no FILE_SIZE\n");

    struct stat buf;
    if (fstat (client->fd, &buf) < 0)  {
      multilog (log, LOG_ERR, "Error fstat %s\n", strerror(errno));
      return -1;
    }
    file_size = buf.st_size - hdr_size;

  }

  client->header_size = hdr_size;
  client->optimal_bytes = 1024 * 1024;
  client->transfer_bytes = file_size;

  /* Get the observation ID */
  if (ascii_header_get (client->header, "UTC_START", "%s", utc_start) != 1) {
    multilog (log, LOG_WARNING, "Header with no UTC_START\n");
    strcpy (utc_start, "UNKNOWN");
  }

  /* Get the observation offset */
  if (ascii_header_get (client->header, "OBS_OFFSET", "%"PRIu64, &obs_offset) 
      != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }

  /* set the current observation id */
  strcpy (diskdb->utc_start, utc_start);
  diskdb->obs_offset = obs_offset;

#ifdef _DEBUG
  fprintf (stderr, "file_open_function returns\n");
#endif

  lseek (client->fd, 0, SEEK_SET);

  return 0;
}

int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  dada_diskdb_t diskdb = DADA_DISKDB_INIT;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Secondary Read Client main loop */
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

  diskdb.array = disk_array_create ();

  while ((arg=getopt(argc,argv,"k:df:vs")) != -1)
    switch (arg) {

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"dada_dbmonitor: could not parse key from %s\n",optarg);
        return -1;
      }
      break;
      
    case 'd':
      daemon=1;
      break;

    case 'f':
      strcpy (diskdb.filename[n_files], optarg);
      n_files++;
      break;

    case 'v':
      verbose=1;
      break;
      
    case 's':
      quit=1;
      break;

    default:
      usage ();
      return 0;
      
    }

  log = multilog_open ("dada_diskdb", daemon);

  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, DADA_DEFAULT_DISKDB_LOG);
  }
  else
    multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_write (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;

  client->data_block = hdu->data_block;
  client->header_block = hdu->header_block;

  client->open_function  = file_open_function;
  client->io_function    = file_read_function;
  client->close_function = file_close_function;
  client->direction      = dada_client_writer;

  client->context = &diskdb;

  while (!client->quit) {
#ifdef _DEBUG
	  fprintf (stderr, "call dada_client_write\n");
#endif
    if (dada_client_write (client) < 0) {
      multilog (log, LOG_ERR, "Error during transfer\n");
      return -1;
    }

    if (quit)
      client->quit = 1;

  }
  if (dada_client_close (client) < 0) {
	  multilog (log, LOG_ERR, "Error closing data block\n");
	  return -1;
  }


  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

