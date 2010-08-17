#include "dada_hdu.h"
#include "multilog.h"
#include "ascii_header.h"
#include "futils.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void usage()
{
  fprintf (stdout,
	   "dada_write_test [options]\n"
	   " -H filename    ascii header information in file\n"
	   " -g gigabytes   number of gigabytes to write\n");
}

static char* default_header =
"OBS_ID       dada_write_test.0001\n"
"OBS_OFFSET   100\n"
"FILE_SIZE    1073741824\n"
"FILE_NUMBER  3\n";

int main (int argc, char **argv)
{
  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log;

  /* number of bytes to write */
  uint64_t bytes_to_write = (uint64_t)(1024 * 1024) * (uint64_t)(534 * 5);

  /* the header to be written to the header block */
  char* header = default_header;

  /* the header sub-block */
  char* header_buf = 0;

  /* the size of the header string */
  unsigned header_strlen = 0;

  /* the size of the header buffer */
  uint64_t header_size = 0;

  /* the filename from which the header will be read */
  char* header_file = 0;

  /* the size of the data buffer */
  uint64_t data_size = 1024 * 1024;

  /* the data to copy */
  char* data = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  float gigabytes = 0.0;
  float one_gigabyte = 1024.0 * 1024.0 * 1024.0;

  int arg = 0;
  while ((arg=getopt(argc,argv,"g:hH:v")) != -1) {
    switch (arg) {

    case 'g':
      gigabytes = atof (optarg);
      bytes_to_write = (uint64_t) (gigabytes * one_gigabyte);
      break;

    case 'H':
      header_file = optarg;
      break;
      
    case 'v':
      verbose=1;
      break;

    case 'h':
    default:
      usage ();
      return 0;
      
    }
  }
  
  log = multilog_open ("dada_write_test", 0);
  multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_write (hdu) < 0)
    return EXIT_FAILURE;

  data = (char*) malloc (data_size);
  assert (data != 0);

  header_strlen = strlen(header);

  header_size = ipcbuf_get_bufsz (hdu->header_block);
  multilog (log, LOG_INFO, "header block size = %"PRIu64"\n", header_size);

  header_buf = ipcbuf_get_next_write (hdu->header_block);

  if (!header_buf)  {
    multilog (log, LOG_ERR, "Could not get next header block\n");
    return EXIT_FAILURE;
  }

  if (header_file)  {
    if (fileread (header_file, header_buf, header_size) < 0)  {
      multilog (log, LOG_ERR, "Could not read header from %s\n", header_file);
      return EXIT_FAILURE;
    }
  }
  else { 
    header_strlen = strlen(header);
    memcpy (header_buf, header, header_strlen);
    memset (header_buf + header_strlen, '\0', header_size - header_strlen);
  }

  /* Set the header size attribute */ 
  if (ascii_header_set (header_buf, "HDR_SIZE", "%"PRIu64"", header_size) < 0) {
    multilog (log, LOG_ERR, "Could not write HDR_SIZE to header\n");
    return -1;
  }

  if (ipcbuf_mark_filled (hdu->header_block, header_size) < 0)  {
    multilog (log, LOG_ERR, "Could not mark filled header block\n");
    return EXIT_FAILURE;
  }

  fprintf (stderr, "Writing %"PRIu64" bytes to data block\n", bytes_to_write);

  while (bytes_to_write)  {

    if (data_size > bytes_to_write)
      data_size = bytes_to_write;

    //fprintf (stderr, "Writing %"PRIu64" bytes to data block\n", data_size);
    if (ipcio_write (hdu->data_block, data, data_size) < 0)  {
      multilog (log, LOG_ERR, "Could not write %"PRIu64" bytes to data block\n",
                data_size);
      return EXIT_FAILURE;
    } 
    bytes_to_write -= data_size;
  }

  if (dada_hdu_unlock_write (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  dada_hdu_destroy (hdu);

  return EXIT_SUCCESS;
}

