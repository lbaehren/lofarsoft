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

void usage()
{
  fprintf (stdout,
           "dada_dbNdb [options]\n"
           " -k <key>   add a DADA HDU to which data will be written\n"
           " -d         run as daemon\n");
}

typedef struct {

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu;

  /* DADA HDU Identifier */
  key_t key;

} dada_hduq_t;

typedef struct {

  /* the set of DADA HDUs to which data may be written */
  dada_hduq_t* hdu;

  /* the number of DADA HDUs in the above array */
  unsigned nhdu;

  unsigned ihdu;

  /* current observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN];

  /* minimum data size to be written to any hdu */
  uint64_t chunk_size;

  /* current number of bytes written to all hdus (reset each xfer) */
  uint64_t current_bytes;

  /* verbose output */
  int verbose;

  /* Length of a "packet" */
  int resolution;

  /* All transfers must be a multiple of this many bytes */
  int byte_multiple;

  /* Obs offset from header */
  int64_t obs_offset;
  
} dada_dbNdb_t;

#define DADA_DBNDB_INIT { 0, 0, 0, "", 0, 0, 0, 0, 0}

int dbNdb_add_hdu (dada_dbNdb_t* queue, key_t key, multilog_t* log)
{
  dada_hdu_t* hdu = dada_hdu_create (log);
  dada_hdu_set_key (hdu, key);

  if (queue->verbose) 
    fprintf(stderr, "dbNdb_add_hdu: connecting with key=%x\n", key);

  if (dada_hdu_connect (hdu) < 0)
  {
    fprintf(stderr, "cannot connect to DADA HDU key=%x\n", key);
    return -1;
  }

  if (queue->verbose) 
    fprintf(stderr, "dbNdb_add_hdu: adding to queue\n");

  queue->hdu = realloc( queue->hdu, (queue->nhdu+1) * sizeof(dada_hduq_t) );
  queue->hdu[queue->nhdu].hdu = hdu;
  queue->hdu[queue->nhdu].key = key;
  queue->nhdu ++;

  return 0;
}


/*! Function that opens the data transfer target */
int hdu_open_function (dada_client_t* client)
{

  /* the dada_dbNdb specific data */
  dada_dbNdb_t* queue = 0;
  
  /* the queue element to which data will be sent */
  dada_hduq_t* hdu = 0;

  /*! The Header Block to which a new header will be written */
  ipcbuf_t* header_block;

  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* number of bytes in the header */
  uint64_t header_size = 0;

  /* Offset of the first byte of valid data */
  uint64_t obs_offset = 0;

  /* observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN] = "";

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  queue = (dada_dbNdb_t*) client->context;

  assert (queue != 0);
  assert (queue->hdu != 0);
  assert (queue->ihdu < queue->nhdu);

  /* Select a hdu */
  int ihdu = 0;

  queue->ihdu = 0;

  /* cycle through the available hdus */
  for (ihdu=0; ihdu < queue->nhdu; ihdu++) {

    int64_t space_left = ipcio_space_left(queue->hdu[ihdu].hdu->data_block);
    float percent_full  = ipcio_percent_full(queue->hdu[ihdu].hdu->data_block);

    if (queue->verbose) 
      multilog(log, LOG_INFO, "dada_dbNdb: hdu_open_function() HDU %x, %"PRIi64" bytes, %5.2f percent full\n",queue->hdu[ihdu].key, space_left, percent_full);

    /* test if enough space is available on the data block question */
    if ((space_left >= queue->chunk_size) && (percent_full < 0.5)) {
      queue->ihdu = ihdu;
      break;
    }
  }

  hdu = queue->hdu + queue->ihdu;
  header_block = hdu->hdu->header_block;

  multilog(log, LOG_INFO, "Writing data to DADA HDU %x\n",hdu->key);

  //fprintf (stderr, "Opening HDU %d\n", queue->ihdu);

  if (dada_hdu_lock_write (hdu->hdu) < 0)
  {
    multilog (log, LOG_ERR, "cannot lock write DADA HDU (key=%x)\n", hdu->key);
    return -1;
  }

  /* Get the observation ID */
  if (ascii_header_get (client->header, "OBS_ID", "%s", obs_id) != 1) {
    /* multilog (log, LOG_WARNING, "Header with no OBS_ID\n"); */
    strcpy (obs_id, "UNKNOWN");
  }

  /* Get the header offset */
  if (ascii_header_get (client->header, "OBS_OFFSET",
                        "%"PRIu64, &obs_offset) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }

  /* Get the RESOLUTION */
  if (ascii_header_get (client->header, "RESOLUTION", "%d", &(queue->resolution)) != 1) {
    multilog (log, LOG_WARNING, "Header with no RESOLUTION\n");
    queue->resolution = 0;  // Set to random value to ensure no NaN's later on
    queue->byte_multiple = 1;
  } else {
    queue->byte_multiple = queue->resolution;
  }

  /* ensure that the switching byte wont break O_DIRECT */
  queue->byte_multiple *= 512;


#ifdef _DEBUG
  fprintf (stderr, "queue: copy the obs id\n");
#endif

  /* set the current observation id */
  strcpy (queue->obs_id, obs_id);

  header_size = ipcbuf_get_bufsz (client->header_block);

  assert( header_size == ipcbuf_get_bufsz (header_block) );

  header = ipcbuf_get_next_write (header_block);
  if (!header)  {
    multilog (log, LOG_ERR, "Could not get next header block\n");
    return EXIT_FAILURE;
  }

  memcpy ( header, client->header, header_size );

  if (ipcbuf_mark_filled (header_block, header_size) < 0)  {
    multilog (log, LOG_ERR, "Could not mark filled Header Block\n");
    return EXIT_FAILURE;
  }

  if (queue->verbose) 
    multilog (log, LOG_INFO, "HDU (key=%x) opened for writing\n", hdu->key);

  client->transfer_bytes = 0;

  /* Only set the obs_offset upon the first call to this function */
  if (queue->obs_offset == -1) 
    queue->obs_offset = obs_offset;

  if (queue->verbose)
    multilog (log, LOG_INFO, "current_bytes=%"PRIu64", obs_offset=%"PRIu64"\n",
                    queue->current_bytes,  queue->obs_offset);
  
  /* Get the observation ID */
  int bytes_per_second = 0;
  if (ascii_header_get (client->header, "BYTES_PER_SECOND", "%d", 
                        &bytes_per_second) != 1) {
    multilog (log, LOG_WARNING, "Header with no BYTES_PER_SECOND\n");
    bytes_per_second=64000000;
  }
  
  queue->chunk_size = (bytes_per_second * 10);

  client->header_transfer = 0;
  return 0;
}

/*! Function that closes the data file */
int hdu_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the dada_dbNdb specific data */
  dada_dbNdb_t* queue = 0;

  /* the queue element to which data will be sent */
  dada_hduq_t* hdu = 0;

  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  queue = (dada_dbNdb_t*) client->context;

  assert (queue != 0);
  assert (queue->hdu != 0);
  assert (queue->ihdu < queue->nhdu);

  hdu = queue->hdu + queue->ihdu;

  log = client->log;
  assert (log != 0);

  if (dada_hdu_unlock_write (hdu->hdu) < 0)
  {
    multilog (log, LOG_ERR, "cannot unlock DADA HDU (key=%x)\n", hdu->key);
    return -1;
  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t hdu_write_function (dada_client_t* client, 
                            void* data, uint64_t data_size)
{
  /* the dada_dbNdb specific data */
  dada_dbNdb_t* queue = 0;
  
  /* the queue element to which data will be sent */
  dada_hduq_t* hdu = 0;

  assert (client != 0);

  queue = (dada_dbNdb_t*) client->context;

  assert (queue != 0);
  assert (queue->hdu != 0);
  assert (queue->ihdu < queue->nhdu);

  //hdu = queue->hdu[queue->ihdu];
  hdu = queue->hdu + queue->ihdu;

  /* Write data to the current queue */
  int64_t bytes_written = ipcio_write (hdu->hdu->data_block, data, data_size);

  queue->current_bytes += data_size;
 
  if (!client->transfer_bytes) {

    /* If the next write would cross a chunk boundary, test if we should
     * change hdus */
    uint64_t current_chunk = queue->current_bytes / queue->chunk_size;
    uint64_t next_chunk_byte = (current_chunk + 1) *  queue->chunk_size;

    /* If we will cross chunks */
    if (next_chunk_byte < (queue->current_bytes + data_size)) {

      int ihdu = 0;
      dada_hduq_t* newhdu = 0;

      for (ihdu=0; ihdu < queue->nhdu; ihdu++) {
        newhdu = queue->hdu + ihdu;

        /* If the hdu can fit at LEAST a chunk */
        if ( (ipcio_space_left(newhdu->hdu->data_block) >= queue->chunk_size) &&
              (ipcio_percent_full(newhdu->hdu->data_block) < 0.5) ) {

          /* and its a different hdu to the current hdu, set stopping byte */
          if (ihdu != queue->ihdu) {

            //if (ihdu == 0) 
              multilog(client->log, LOG_WARNING, "DB full, switching %x -> %x\n", hdu->key, newhdu->key);

            /* Take into account chunk size and resolution */
            uint64_t gap = (queue->obs_offset + next_chunk_byte) % queue->byte_multiple;

            if (queue->verbose) 
              multilog(client->log, LOG_INFO, "hdu_write_function: gap is %"PRIu64" bytes\n", gap);

            /* shrink the xfer so that it is on a byte_multiple */
            client->transfer_bytes = next_chunk_byte - gap;

            /* If the gap is > 0.5 byte_multiples, grow it */
            if (gap > queue->byte_multiple / 2) 
              client->transfer_bytes += queue->byte_multiple;

            if (queue->verbose)
              multilog(client->log, LOG_INFO, "CHUNK=%"PRIu64", "
                       "BYTE_MULTIPLE=%"PRIu64", CHANGE = %"PRIu64"\n",
                       next_chunk_byte, queue->byte_multiple, client->transfer_bytes);

          }

          break;
        }
      }
    }
  }

  return bytes_written;
}


int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  dada_dbNdb_t queue = DADA_DBNDB_INIT;

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

  /* The key */
  key_t dada_key = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"dk:v")) != -1)
    switch (arg) {
      
    case 'd':
      daemon=1;
      break;
      
    case 'k':

      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr, "dada_dbNdb: could not parse key from %s\n", optarg);
        return -1;
      }

      if (!log)
        log = multilog_open ("dada_dbNdb", daemon);

      if (dbNdb_add_hdu (&queue, dada_key, log) < 0) {
        fprintf (stderr, "dada_dbNdb: could not add HDU (key=%x)\n", dada_key);
        return -1;
      }

      if (verbose)
        fprintf(stderr,"dada_dbNdb: HDU (key=%x) added\n", dada_key);

      break;

    case 'v':
      verbose=1;
      queue.verbose = 1;
      break;
      
    default:
      usage ();
      return 0;
      
    }


  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, DADA_DEFAULT_DBNDB_LOG);
  }
  else
    multilog_add (log, stderr);

  if (verbose) 
     multilog (log, LOG_INFO, "dada_dbNdb: creating structures\n");

  hdu = dada_hdu_create (log);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (verbose)
    multilog (log, LOG_INFO,"dada_dbNdb: lock read key=%x\n", 
                            hdu->data_block_key);

  if (dada_hdu_lock_read (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;

  client->data_block   = hdu->data_block;
  client->header_block = hdu->header_block;

  client->open_function  = hdu_open_function;
  client->io_function    = hdu_write_function;
  client->close_function = hdu_close_function;
  client->direction      = dada_client_reader;

  queue.current_bytes = 0;
  queue.obs_offset = -1;

  client->context = &queue;

  if (dada_client_read (client) < 0)
    multilog (log, LOG_ERR, "Error during transfer\n");

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

