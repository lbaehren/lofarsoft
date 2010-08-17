#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <values.h>

#include <sys/types.h>
#include <sys/wait.h>

//#define _DEBUG 1

#include "ipcbuf.h"

int main (int argc, char** argv)
{
  key_t key = 0x69;
  int   arg;

  uint64_t nbufs = 16;
#ifdef _SC_PAGE_SIZE
  uint64_t bufsz = sysconf (_SC_PAGE_SIZE);
#else
  uint64_t bufsz = 1000000;
#endif

  uint64_t n64 = bufsz/sizeof(uint64_t);
  uint64_t i64 = 0;
  uint64_t check64 = 0;

  uint64_t length = 0;
  uint64_t max_length = 0;
  uint64_t min_length = MAXINT;

  unsigned ntest = 1024 * 16;
  unsigned itest = 0;
  unsigned twice = 0;
  
  uint64_t* buf   = 0;
  uint64_t  count = 0;

  uint64_t offset = 0;

  uint64_t max_offset = 0;
  uint64_t min_offset = MAXINT;

  uint64_t raise_sod = 0;
  uint64_t raise_eod = 0;

  char must_set_sod = 0;
  uint64_t sodbuf = 0;
  uint64_t sodbyte = 0;

  ipcbuf_t ringbuf = IPCBUF_INIT;

  uint64_t bytesio = 0;

  char verbose = 0;

  int debug = 0;
  while ((arg = getopt(argc, argv, "b:dhn:k:v")) != -1) {

    switch (arg)  {

    case 'h':
      fprintf (stderr, 
	       "test_ipcbuf [options]\n"
	       " -b block_size  Set the size of each block in ring buffer \n"
	       " -n nblock      Set the number of blocks in ring buffer \n"
	       " -d             Debug mode \n"
	       " -k key         Key to shared memory \n"
	       " -v             Verbose mode\n"
	       );
      return 0;

    case 'b':
      if (sscanf (optarg, "%"PRIu64"", &bufsz) < 1) {
	fprintf (stderr, "test_ipcbuf could not parse -b %s", optarg);
	return -1;
      }
      break;

    case 'd':
      debug = 1;
      break;

    case 'n':
      if (sscanf (optarg, "%"PRIu64"", &nbufs) < 1) {
	fprintf (stderr, "test_ipcbuf could not parse -n %s", optarg);
	return -1;
      }
      break;

    case 'k':
      key = atoi (optarg);
      break;

    }
  }

  srand (13);
  count = 0;

  fprintf (stderr, "Running %u tests ...\n", ntest);

  if (fork()) {
    
    /* this process is writing to and creates the shared memory */
    fprintf (stderr, "Creating shared memory ring buffer."
	     " nbufs=%"PRIu64" bufsz=%"PRIu64"\n", nbufs, bufsz);

    if (ipcbuf_create (&ringbuf, key, nbufs, bufsz) < 0) {
      fprintf (stderr, "Error creating shared memory ring buffer\n");
      return -1;
    }

    /* lock write to shm */
    if (ipcbuf_lock_write (&ringbuf) < 0) {
      fprintf (stderr, "Error ipcbuf_lock_write\n");
      return -1;
    }

    /* going to test the start/stop features */
    ipcbuf_disable_sod (&ringbuf);

    for (itest=0; itest < ntest; itest++)  {

      raise_sod = raise_eod + rand()%(4 * nbufs * n64);

      if (raise_sod%n64 == raise_eod%n64)
	twice ++;

      offset = rand()%((nbufs-2) * n64);

      if (offset < min_offset)
	min_offset = offset;

      if (offset > max_offset)
	max_offset = offset;

      offset += raise_sod;

      raise_eod = offset + n64 + rand()%(8 * nbufs * n64);

      length = raise_eod - raise_sod;

      if (length < min_length)
	min_length = length;

      if (length > max_length)
	max_length = length;

      if (verbose)
	fprintf (stderr, "test:%u sod=%"PRIu64" off=%"PRIu64" eod=%"PRIu64"\n",
		 itest, raise_sod, offset, raise_eod);

      must_set_sod = 1;
      sodbuf = raise_sod * sizeof(uint64_t) / bufsz;
      sodbyte = raise_sod * sizeof(uint64_t) % bufsz;

      while (count < raise_eod) {

	/* get the next buffer to be filled */
	buf = (uint64_t*) ipcbuf_get_next_write (&ringbuf);
	if (!buf) {
	  fprintf (stderr, "error ipcbuf_get_next_write\n");
	  return -1;
	}

	for (i64=0; i64<n64; i64++) {
	  buf[i64] = count;
	  count ++;
	}

#ifdef _DEBUG
	fprintf (stderr, "buffer:%"PRIu64" buf[0]=%"PRIu64"\n",
		 ipcbuf_get_write_count(&ringbuf), buf[0]);
#endif

	bytesio = bufsz;
	if (count >= raise_eod) {

	  bytesio -= (count - raise_eod) * sizeof(uint64_t);

	  if (verbose)
	    fprintf (stderr, "Enabling end-of-data buf=%"PRIu64
		     " byte=%"PRIu64"\n",
		     ipcbuf_get_write_count(&ringbuf), bytesio);
	  
 	  if (ipcbuf_enable_eod (&ringbuf) < 0) {
	    fprintf (stderr, "Failure to raise eod count=%"PRIu64
		     " eod=%"PRIu64" sod=%"PRIu64" offset=%"PRIu64"\n",
		     count, raise_eod, raise_sod, offset);
	    return -1;
	  }

	}

	if (ipcbuf_mark_filled (&ringbuf, bytesio) < 0) {
	  fprintf (stderr, "error ipcbuf_mark_filled\n");
	  return -1;
	}

	if (count < raise_eod && ipcbuf_eod (&ringbuf)) {
	  fprintf (stderr, "premature EOD bytesio=%"PRIu64" bufsz=%"PRIu64"\n",
		   bytesio, bufsz);
	  return -1;
	}

	if (count >= offset && must_set_sod) {

	  if (verbose)
	    fprintf (stderr, "Enabling start-of-data buf=%"PRIu64
		     " byte=%"PRIu64"\n", sodbuf, sodbyte);
	  
	  if (ipcbuf_enable_sod (&ringbuf, sodbuf, sodbyte) < 0)
	    return -1;

	  must_set_sod = 0;

	}

#ifdef _DEBUG
	fprintf (stderr, "W: End of data: %d %d\n",
		 ringbuf.sync->eod[ringbuf.xfer], ipcbuf_eod (&ringbuf));
#endif

	if (debug) {
	  if (ipcbuf_get_write_count(&ringbuf) == ipcbuf_get_nbufs(&ringbuf)) {
	    fprintf (stderr, "Pause\n");
	    getchar();
	  }
	}
	
      }

      fprintf (stderr, "Finished %d %%\r", 
	       (int)(100*(float)itest/(float)ntest));

    }

    if (debug) {
      fprintf (stderr, "Pause before reset\n");
      getchar();
    }

    fprintf (stderr, "Waiting for read to finish\n");
    if (ipcbuf_reset (&ringbuf) < 0)
      fprintf (stderr, "Error ipcbuf_reset\n");

    if (debug) {
      fprintf (stderr, "Pause before destroy\n");
      getchar();
    }

    fprintf (stderr, "Scheduling IPC resources for destruction\n");
    ipcbuf_destroy (&ringbuf);

    fprintf (stderr,
	     "Succesful completion!\n"
	     "In %u random transfers:\n"
	     "%u started in the same buffer as the previous transfer ended\n"
	     "\n"
	     "Maximum transfer length: %"PRIu64"\n"
	     "Minimum transfer length: %"PRIu64"\n"
	     "Maximum offset from sod: %"PRIu64"\n"
	     "Minimum offset from sod: %"PRIu64"\n",
	     ntest, twice, max_length, min_length, max_offset, min_offset);

    wait (0);

  }

  else {

    sleep (1);

    fprintf (stderr, "Connecting to shared memory ring buffer\n");
    if (ipcbuf_connect (&ringbuf, key) < 0) {
      fprintf (stderr, "Error connecting to shared memory ring buffer\n");
      return -1;
    }

    /* read from  shm */
    if (ipcbuf_lock_read (&ringbuf) < 0) {
      fprintf (stderr, "Error ipcbuf_lock_read\n");
      return -1;
    }

    nbufs = ipcbuf_get_nbufs(&ringbuf);
    bufsz = ipcbuf_get_bufsz(&ringbuf);

    fprintf (stderr, " nbufs=%"PRIu64" bufsz=%"PRIu64"\n", nbufs, bufsz);

    for (itest=0; itest < ntest; itest++)  {

      raise_sod = raise_eod + rand()%(4 * nbufs * n64);
      offset = raise_sod + rand()%((nbufs-2) * n64);
      raise_eod = offset + n64 + rand()%(8 * nbufs * n64);

      if (verbose)
	fprintf (stderr, "test:%u sod=%"PRIu64" off=%"PRIu64" eod=%"PRIu64"\n",
		 itest, raise_sod, offset, raise_eod);

      count = raise_sod;

      while (count < raise_eod) {

	if ( ipcbuf_eod (&ringbuf) )
	  fprintf (stderr, "END OF DATA!\n");

	buf = (uint64_t*) ipcbuf_get_next_read (&ringbuf, &bytesio);

	if (!buf) {
	  fprintf (stderr, "Error ipcbuf_get_next_read\n");
	  return -1;
	}

#ifdef _DEBUG
	fprintf (stderr, "buf:%"PRIu64" %"PRIu64" bytes. buf[0]=%"PRIu64" \n",
		 ipcbuf_get_read_count (&ringbuf), bufsz, buf[0]);
#endif

	check64 = bytesio / sizeof(uint64_t);

	for (i64=0; i64<check64; i64++) {
	  if (buf[i64] != count) {
	    fprintf (stderr, "\nERROR buf:%"PRIu64" %"PRIu64" bytes.\n",
		     ipcbuf_get_read_count (&ringbuf), bytesio);
	    fprintf (stderr, "buf[%"PRIu64"]=%"PRIu64" != %"PRIu64"\n",
		     i64, buf[i64], count);
	    return -1;
	  }
	  count ++;
	}

	if (verbose)
	  fprintf (stderr, ".");
	
	if (ipcbuf_mark_cleared (&ringbuf) < 0) {
	  fprintf (stderr, "error ipcbuf_mark_cleared\n");
	  return -1;
	}

#ifdef _DEBUG	
	fprintf (stderr, "End of data: %d %d\n",
		 ringbuf.sync->eod[ringbuf.xfer], ipcbuf_eod (&ringbuf));
#endif

      }

      if (ipcbuf_reset (&ringbuf) < 0) {
	fprintf (stderr, "error ipcbuf_reset\n");
	return -1;
      }
      
    }

  }

  return 0;
}
