#include "dada_client.h"
#include "dada_def.h"

#include "ascii_header.h"
#include "diff_time.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* #define _DEBUG 1 */

/*! Create a new DADA client main loop */
dada_client_t* dada_client_create ()
{
  dada_client_t* client = malloc (sizeof(dada_client_t));
  assert (client != 0);

  client -> log = 0;
  client -> data_block = 0;
  client -> header_block = 0;

  client -> open_function = 0;
  client -> io_function = 0;
  client -> close_function = 0;
  client -> direction = dada_client_undefined;

  client -> context = 0;

  client -> header = 0;
  client -> header_size = 0;
  client -> header_transfer = 1;

  client -> fd = -1;
  client -> transfer_bytes = 0;
  client -> optimal_bytes = 0;
  client -> quit = 0;

  return client;
}

/*! Destroy a DADA client main loop */
void dada_client_destroy (dada_client_t* client)
{
  free (client);
}

/*! Transfer data between the Data Block and transfer target */
int64_t dada_client_io_loop (dada_client_t* client)
{
  /* The buffer used for transfer between Data Block and transfer target */
  static char* buffer = 0;
  static uint64_t buffer_size = 0;

  /* counters */
  uint64_t bytes_to_transfer = 0;
  uint64_t bytes_transfered = 0;
  int64_t bytes = 0;

  multilog_t* log = 0;

  assert (client != 0);
  log = client->log;
  assert (log != 0);

  if (buffer_size != client->optimal_bytes) {
    buffer_size = client->optimal_bytes;
    if (buffer)
      free (buffer);
    int rval = posix_memalign ( (void **) &buffer, 512, client->optimal_bytes);
    assert (buffer != 0);

#ifdef _DEBUG
    fprintf (stderr, "buffer size = %"PRIu64"\n", buffer_size);
#endif

  }

  assert (client->direction==dada_client_reader ||
          client->direction==dada_client_writer);

  while (!client->transfer_bytes || bytes_transfered < client->transfer_bytes)
  {
          
    if (!client->transfer_bytes)
      bytes = buffer_size;
    else
    {
      bytes_to_transfer = client->transfer_bytes - bytes_transfered;
      //fprintf(stderr,"transfer_bytes = %"PRIu64", bytes_transfered = %"PRIu64", bytes_to_transfer = %"PRIu64"\n",client->transfer_bytes,bytes_transfered,bytes_to_transfer);

      if (buffer_size > bytes_to_transfer)
	bytes = bytes_to_transfer;
      else
	bytes = buffer_size;
    }

    if (client->direction == dada_client_reader) {

      if (ipcbuf_eod((ipcbuf_t*)client->data_block))
        break;

#ifdef _DEBUG
      fprintf (stderr, "calling ipcio_read %p %"PRIi64"\n", buffer, bytes);
#endif

      bytes = ipcio_read (client->data_block, buffer, bytes);
      if (bytes < 0) {
        multilog (log, LOG_ERR, "ipcio_read error %s\n", strerror(errno));
        return -1;
      }

    }

#ifdef _DEBUG
    fprintf (stderr, "calling io_function %p %"PRIi64"\n", buffer, bytes);
#endif

    bytes = client->io_function (client, buffer, bytes);

#ifdef _DEBUG
    fprintf (stderr, "io_function returns %"PRIi64"\n", bytes);
#endif

    if (bytes < 0) {
      multilog (log, LOG_ERR, "I/O error %s\n", strerror(errno));
      break;
    }

    if (client->direction == dada_client_writer) {

      if (bytes == 0) {
        multilog (log, LOG_INFO, "end of input\n");
        break;
      }

      bytes = ipcio_write (client->data_block, buffer, bytes);
      if (bytes < 0) {
        multilog (log, LOG_ERR, "ipcio_write error %s\n", strerror(errno));
        return -1;
      }

    }

    bytes_transfered += bytes;

  }

  return bytes_transfered;
}

int64_t dada_client_transfer (dada_client_t* client)
{
  /* pointer to the status and error logging facility */
  multilog_t* log = 0;

  /* Byte count */
  int64_t bytes_transfered = 0;

  /* Time at start and end of transfer loop */
  struct timeval start_loop, end_loop;

  /* Time required to transfer data */
  double transfer_time = 0;

  /* the size of the header buffer */
  uint64_t header_size = 0;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

#ifdef _DEBUG
  fprintf (stderr, "dada_client_transfer call open_function\n");
#endif

  if (client->open_function (client) < 0) {
    multilog (log, LOG_ERR, "Error calling open function\n");
    return -1;
  }

  if (client->quit)
  {
    multilog (log, LOG_INFO, "Client quitting\n");
    return 0;
  }

#ifdef _DEBUG
  fprintf (stderr, "open_function returned\n");
#endif

  assert (client->header != 0);
  assert (client->header_size != 0);

  header_size = client->header_size;

#ifdef _DEBUG
  fprintf (stderr, "before calling io_function:\n");
  fprintf (stderr, "HEADER START\n%sHEADER END\n", client->header);
#endif

  if (client->header_transfer)
  {
    bytes_transfered = client->io_function (client, client->header,
					    header_size);
    if (bytes_transfered < header_size) 
    {
      multilog (log, LOG_ERR, "Error transfering header: %s\n", 
		strerror(errno));
      return -1;
    }
  }

  if (client->direction == dada_client_writer) {

#ifdef _DEBUG
    fprintf (stderr, "dada_client_writer HEADER START\n%sHEADER END\n", client->header);
#endif

    header_size = ipcbuf_get_bufsz (client->header_block);
    if (ipcbuf_mark_filled (client->header_block, header_size) < 0)  {
      multilog (log, LOG_ERR, "Could not mark filled Header Block\n");
      return EXIT_FAILURE;
    }
  }

  if (!client->optimal_bytes)
    client->optimal_bytes = DADA_DEFAULT_BLOCK_SIZE;

  multilog (log, LOG_INFO, "Transfering %"PRIu64" bytes in %"PRIu64
            " byte blocks\n", client->transfer_bytes, client->optimal_bytes);

  gettimeofday (&start_loop, NULL);

  /* Transfer data until the end of the transfer */
  bytes_transfered = dada_client_io_loop (client);
  
  gettimeofday (&end_loop, NULL);
  
  if (client->close_function (client, bytes_transfered) < 0) {
    multilog (log, LOG_ERR, "Error calling close function\n");
    return -1;
  }

  if (bytes_transfered > 0) {
      
    transfer_time = diff_time (start_loop, end_loop);
    multilog (log, LOG_INFO, "%"PRIu64" bytes transfered in %lfs "
              "(%lg MB/s)\n", bytes_transfered, transfer_time,
              bytes_transfered/(1024*1024*transfer_time));
    
  }

  return bytes_transfered;
}


int dada_client_read (dada_client_t* client)
{

  /* pointer to the status and error logging facility */
  multilog_t* log = 0;

  /* The header from the ring buffer */
  char* header = 0;
  uint64_t header_size = 0;

  /* header size, as defined by HDR_SIZE attribute */
  uint64_t hdr_size = 0;
  /* byte offset from start of data, as defined by OBS_OFFSET attribute */
  uint64_t obs_offset = 0;

  /* Byte count */
  int64_t bytes_written = 0;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  while (!header_size) {

    /* Wait for the next valid header sub-block */
    header = ipcbuf_get_next_read (client->header_block, &header_size);

    if (!header) {
      multilog (log, LOG_ERR, "Could not get next header\n");
      return DADA_ERROR_FATAL;
    }

    if (!header_size) {

      ipcbuf_mark_cleared (client->header_block);

      if (ipcbuf_eod (client->header_block)) {
        multilog (log, LOG_INFO, "End of data on header block\n");
        ipcbuf_reset (client->header_block);
      }
      else {
        multilog (log, LOG_ERR, "Empty header block\n");
        return -1;
      }
    }
  }

  header_size = ipcbuf_get_bufsz (client->header_block);

  /* Check that header is of advertised size */
  if (ascii_header_get (header, "HDR_SIZE", "%"PRIu64, &hdr_size) != 1) {
    multilog (log, LOG_ERR, "Header with no HDR_SIZE. Setting to %"PRIu64"\n",
              header_size);
    hdr_size = header_size;
    if (ascii_header_set (header, "HDR_SIZE", "%"PRIu64, hdr_size) < 0) {
      multilog (log, LOG_ERR, "Error setting HDR_SIZE\n");
      return -1;
    }
  }

  if (hdr_size < header_size)
    header_size = hdr_size;

  else if (hdr_size > header_size) {
    multilog (log, LOG_ERR, "HDR_SIZE=%"PRIu64
              " > Header Block size=%"PRIu64"\n", hdr_size, header_size);
    multilog (log, LOG_DEBUG, "ASCII header dump\n%s", header);
    return -1;
  }

  /* Duplicate the header */
  if (header_size > client->header_size) {
    
    if (client->header)
      free (client->header);
    int rval = posix_memalign ( (void **) &(client->header), 512, header_size);

    assert (client->header != 0);
    client->header_size = header_size;
  }
  memcpy (client->header, header, header_size);

  ipcbuf_mark_cleared (client->header_block);

  /* Get the header offset */
  if (ascii_header_get (client->header, "OBS_OFFSET",
                        "%"PRIu64, &obs_offset) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }
  
  /* Transfer data until the end of the data stream */
  while (!ipcbuf_eod((ipcbuf_t*)client->data_block)) {

    /* Set the header offset */
    if (ascii_header_set (client->header, "OBS_OFFSET",
                          "%"PRIu64, obs_offset) < 0) {
      multilog (log, LOG_ERR, "Error writing OBS_OFFSET\n");
      return -1;
    }

    bytes_written = dada_client_transfer (client);

    if (bytes_written < 0)
      return -1;

    obs_offset += bytes_written;

    /* fprintf(stderr,"wrote %"PRIu64" bytes obs_offset = %"PRIu64"\n",bytes_written, obs_offset); */

  }

  ipcbuf_reset ((ipcbuf_t*)client->data_block);

  return 0;

}

/*! Run the DADA client write loop */
int dada_client_write (dada_client_t* client)
{
  /* pointer to the status and error logging facility */
  multilog_t* log = 0;

  /* The header from the ring buffer */
  char* header = 0;
  uint64_t header_size = 0;

  /* Byte count */
  int64_t bytes_read = 0;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  header_size = ipcbuf_get_bufsz (client->header_block);
  multilog (log, LOG_INFO, "header block size = %"PRIu64"\n", header_size);


  if (header_size) {

    header = ipcbuf_get_next_write (client->header_block);
    if (!header)  {
      multilog (log, LOG_ERR, "Could not get next header block\n");
      return EXIT_FAILURE;
    }

    client->header = header;
    client->header_size = header_size;

    // the open function of a write client should set the header_size
    // and transfer size attributes
    bytes_read = dada_client_transfer (client);

    if (bytes_read < 0)
      return -1;

  }

  return 0;
}
int dada_client_close (dada_client_t* client)
{
	  multilog_t* log = client->log;

	//	signal end of data on Data Block
	if (ipcio_close (client->data_block) < 0)  {
		multilog (log, LOG_ERR, "Could not close Data Block\n");
		return -1;
	}
	return 0;
}
