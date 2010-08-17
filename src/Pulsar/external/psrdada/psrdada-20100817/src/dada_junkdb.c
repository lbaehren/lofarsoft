#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_generator.h"
#include "ascii_header.h"

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

#define DEFAULT_DATA_RATE 64.000
#define DEFAULT_WRITE_TIME 10 

static void fsleep (double seconds)
{
  struct timeval t ;

  t.tv_sec = seconds;
  seconds -= t.tv_sec;
  t.tv_usec = seconds * 1e6;
  select (0, 0, 0, 0, &t) ;
}

void usage()
{
  fprintf (stdout,
	   "dada_junkdb [options] header_file\n"
     " -k       hexadecimal shared memory key  [default: %x]\n"
     " -z rate  data rate to write [default %f MB/s]\n"
     " -t secs  length of time to write [default %d s]\n"
     " -g       write gaussian distributed data\n"
     " -d       run as daemon\n", DADA_DEFAULT_BLOCK_KEY, DEFAULT_DATA_RATE,
     DEFAULT_WRITE_TIME);
}

typedef struct {

  /* header file to use */
  char * header_file;

  /* data rate to write at */
  float rate;

  /* generate gaussian data ? */
  unsigned write_gaussian;

  /* length of time to write for */
  uint64_t write_time;

  /* pre generated data to write */
  char * data;

  /* length of each array */ 
  uint64_t data_size;

  uint64_t bytes_to_copy;

  time_t curr_time;

  time_t prev_time;

  unsigned header_read;

  unsigned verbose;

} dada_junkdb_t;

#define DADA_JUNKDB_INIT { "", 0, 0, 0, "", 0, 0, 0, 0, 0 }


/*! Pointer to the function that transfers data to/from the target */
int64_t transfer_data (dada_client_t* client, void* data, uint64_t data_size)
{

#ifdef _DEBUG
  multilog (client->log, LOG_INFO, "transfer_data %p %"PRIu64"\n", data, data_size);
#endif

  /* the dada_junkdb specific data */
  dada_junkdb_t* junkdb = (dada_junkdb_t*) client->context;

  if (!junkdb->header_read) {
    junkdb->header_read = 1;
#ifdef _DEBUG
    multilog (client->log, LOG_INFO, "transfer_data: read header\n");
#endif
    return data_size;
  }

  uint64_t bytes_copied = 0;
  uint64_t bytes = 0;

  while (bytes_copied < data_size) {

    if (junkdb->bytes_to_copy == 0) {
      fsleep(0.1);
    } 

    junkdb->prev_time = junkdb->curr_time;
    junkdb->curr_time = time(0);
    if (junkdb->curr_time > junkdb->prev_time) {
      junkdb->bytes_to_copy += floor(junkdb->rate * 1024 * 1024);
    }

    if (junkdb->bytes_to_copy) {

      if (data_size-bytes_copied < junkdb->bytes_to_copy)
        bytes = data_size-bytes_copied;
      else 
        bytes = junkdb->bytes_to_copy-bytes_copied;

      if (bytes > junkdb->data_size)
        bytes = junkdb->data_size;

#ifdef _DEBUG
      multilog (client->log, LOG_INFO, "[%"PRIu64" / %"PRIu64"] bytes=%"PRIu64", btc=%"PRIu64"\n", bytes_copied, data_size,bytes, junkdb->bytes_to_copy);
#endif
      memcpy (data+bytes_copied, junkdb->data, bytes);
      bytes_copied += bytes;
      junkdb->bytes_to_copy -= bytes;
    }

  }
#ifdef _DEBUG 
  multilog (client->log, LOG_INFO, "transfer_data: copied %"PRIu64" bytes\n", bytes_copied);
#endif

  return (int64_t) bytes_copied;
} 



/*! Function that closes the data file */
int release_data (dada_client_t* client, uint64_t bytes_written)
{
  /* the dada_junkdb specific data */
  dada_junkdb_t* junkdb = 0;

  assert (client != 0);
  junkdb = (dada_junkdb_t*) client->context;
  assert (junkdb != 0);

  free (junkdb->data);
 
  return 0;
}

/*! Function that opens the data transfer target */
int generate_data(dada_client_t* client)
{

  /* the dada_junkdb specific data */
  dada_junkdb_t* junkdb = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* the size of the header, as determined by HDR_SIZE */
  uint64_t hdr_size = 0;

  assert (client != 0);

  junkdb = (dada_junkdb_t*) client->context;

  assert (junkdb != 0);
  assert (client->header != 0);

  log = client->log;

  time_t current_time = 0;
  time_t prev_time = 0;
  
  /* read the header */
  if (fileread (junkdb->header_file, client->header, client->header_size) < 0)  {
    multilog (log, LOG_ERR, "Could not read header from %s\n", junkdb->header_file);
  }

  if (junkdb->verbose)
  fprintf (stderr, "===========HEADER START=============\n"
                   "%s"
                   "============HEADER END==============\n", client->header);
  junkdb->header_read = 0;

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

  client->header_size = hdr_size;
  client->optimal_bytes = 32 * 1024 * 1024;
  client->transfer_bytes = floor(junkdb->rate * junkdb->write_time * 1024 * 1024);
  junkdb->bytes_to_copy = 0;

  /* seed the RNG */
//#ifdef _DEBUG
  multilog (client->log, LOG_INFO, "generating gaussian data %"PRIu64"\n", junkdb->data_size);
//#endif

  srand ( time(NULL) );
  junkdb->data = (char *) malloc (sizeof(char) * junkdb->data_size);

  if (junkdb->write_gaussian) 
    fill_gaussian_chars(junkdb->data, junkdb->data_size, 8, 500);

  multilog (client->log, LOG_INFO, "data generated\n");

#ifdef _DEBUG
  multilog (client->log, LOG_INFO, "generate_data returned\n");
#endif

  return 0;
}

int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  dada_junkdb_t junkdb = DADA_JUNKDB_INIT;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Secondary Read Client main loop */
  dada_client_t* client = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* header to use for the data block */
  char * header_file = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  unsigned write_gaussian = 0;

  /* data rate [MB/s] */
  float rate = DEFAULT_DATA_RATE;

  /* write time [s] */
  unsigned write_time = DEFAULT_WRITE_TIME;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  while ((arg=getopt(argc,argv,"gk:t:vz:")) != -1)
    switch (arg) {

    case 'g':
      write_gaussian = 1;
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"dada_dbmonitor: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    case 't':
      if (sscanf (optarg, "%u", &write_time) != 1) {
        fprintf (stderr,"Error: could not parse write time from %s\n",optarg);
        usage();
        return -1;
      }
      fprintf(stderr, "writing for %d seconds\n", write_time);
      break;

    case 'v':
      verbose=1;
      break;
      
    case 'z':
      if (sscanf (optarg, "%f", &rate) != 1) {
        fprintf (stderr,"Error: could not parse data rate from %s\n",optarg);
        usage();
        return -1;
      } else {
        fprintf (stderr, "optarg = %s, setting data rate = %f MB/s\n", optarg, rate); 
      }
      break;

    default:
      usage ();
      return 0;
      
    }

  if ((argc - optind) != 1) {
    fprintf (stderr, "Error: a header file must be specified\n");
    usage();
    exit(EXIT_FAILURE);
  } 

  header_file = strdup(argv[optind]);

  /* test that the header file can be read */
  FILE* fptr = fopen (header_file, "r");
  if (!fptr) {
    fprintf (stderr, "Error: could not open '%s' for reading: %s\n", header_file, strerror(errno));
    return(EXIT_FAILURE);
  }
  fclose(fptr);

  if (verbose)
    fprintf (stdout, "Using header file: %s\n", header_file);

  log = multilog_open ("dada_junkdb", 0);

  multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_write (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;
  junkdb.rate = rate;
  junkdb.write_time = write_time;
  junkdb.write_gaussian = write_gaussian;
  junkdb.data_size = floor(rate * 1024 * 1024);
  junkdb.header_file = strdup(header_file);
  junkdb.verbose = verbose;

  client->data_block = hdu->data_block;
  client->header_block = hdu->header_block;

  client->open_function  = generate_data;
  client->io_function    = transfer_data;
  client->close_function = release_data;
  client->direction      = dada_client_writer;

  client->context = &junkdb;

  if (dada_client_write (client) < 0) {
    multilog (log, LOG_ERR, "Error during transfer\n");
    return -1;
  }

  if (dada_hdu_unlock_write (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

