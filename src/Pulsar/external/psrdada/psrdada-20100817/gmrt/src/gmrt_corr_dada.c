

#include "ascii_header.h"
#include "gmrt_corr_dada.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

int gmrt_corr_get_header(dada_hdu_t * hdu)
{

	uint64_t header_size = 0;
	uint64_t hdr_size = 0;
	char * header;

	while (!header_size) {
  
    /* Wait for the next valid header sub-block */
    header = ipcbuf_get_next_read (hdu->header_block, &header_size);
    fprintf(stderr, "gmrt_corr_get_header: ipcbuf_get_next_read()\n");
    
    if (!header) {
      fprintf(stderr, "ERROR: Could not get next header\n");
      return -1;
    } 
    
    if (!header_size) {

      fprintf(stderr, "gmrt_corr_get_header: ipcbuf_mark_cleared 1\n");
      ipcbuf_mark_cleared (hdu->header_block);

      if (ipcbuf_eod (hdu->header_block)) {
				fprintf(stderr, "ERROR: End of data on header block");
        ipcbuf_reset (hdu->header_block);
      }
      else {
        fprintf(stderr, "ERROR: Empty header block\n");
        return -1;
      }
    }
  }

  header_size = ipcbuf_get_bufsz (hdu->header_block);
	
	/* Check that header is of advertised size */
  if (ascii_header_get (header, "HDR_SIZE", "%"PRIu64, &hdr_size) != 1) {
		fprintf(stderr, "ERROR: Header with no HDR_SIZE. Setting to %"PRIu64"\n", header_size);
    hdr_size = header_size;
    if (ascii_header_set (header, "HDR_SIZE", "%"PRIu64, hdr_size) < 0) {
			fprintf(stderr, "ERROR: error setting HDR_SIZE\n");
      return -1;
    }
  }
  
  if (hdr_size < header_size)
    header_size = hdr_size;
  
  else if (hdr_size > header_size) {
    fprintf(stderr, "HDR_SIZE=%"PRIu64" Headre Block size=%"PRIu64"\n", hdr_size, header_size);
    fprintf(stderr, "ASCII header dump\n%s", header);
    return -1;
  }
  
  /* Duplicate the header */
  if (header_size > hdu->header_size) {
    
    if (hdu->header)
      free (hdu->header);
    int rval = posix_memalign ( (void **) &(hdu->header), 512, header_size);
    
    assert (hdu->header != 0);
    hdu->header_size = header_size;
  }
	memcpy (hdu->header, header, header_size);
  
  fprintf(stderr, "gmrt_corr_get_header: ipcbuf_mark_cleared 2\n");
  ipcbuf_mark_cleared (hdu->header_block);

	return 0;

}

/* read size bytes from the datablock stream into address, ignoring ant */
int64_t gmrt_corr_dada_ipcio_read(dada_hdu_t * hdu, char * address, uint64_t size)
{

	uint64_t buffer_size = 0;
	int64_t bytes = 0;

	if (ipcbuf_eod((ipcbuf_t*)hdu->data_block))
		return 0;

#ifdef _DEBUG
  fprintf (stderr, "calling ipcio_read %p %"PRIi64"\n", buffer, bytes);
#endif

  bytes = ipcio_read (hdu->data_block, address, size);
  if (bytes < 0) {
    fprintf(stderr, "ipcio_read error %s\n", strerror(errno));
    return -1;
  }

	return bytes;

} 
