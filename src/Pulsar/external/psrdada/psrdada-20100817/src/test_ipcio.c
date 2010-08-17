#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <values.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

//#define _DEBUG 1

#include "ipcio.h"

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
  
  // uint64_t* buf   = 0;
  uint64_t  count = 0;

  uint64_t* smbuf = 0;
  uint64_t  smbufsz = 0;

  uint64_t offset = 0;

  uint64_t max_offset = 0;
  uint64_t min_offset = MAXINT;

  uint64_t raise_sod = 0;
  uint64_t raise_eod = 0;

  char must_set_sod = 0;
  uint64_t sodbyte = 0;

  ipcio_t ringbuf = IPCIO_INIT;

  uint64_t bytesio = 0;

  char verbose = 0;


  int debug = 0;
  while ((arg = getopt(argc, argv, "b:dhn:k:v")) != -1) {

    switch (arg)  {

    case 'h':
      fprintf (stderr, 
	       "test_ipcio [options]\n"
	       " -b block_size  Set the size of each block in ring buffer \n"
	       " -n nblock      Set the number of blocks in ring buffer \n"
	       " -d             Debug mode \n"
	       " -k key         Key to shared memory \n"
	       " -v             Verbose mode\n"
	       );
      return 0;

    case 'b':
      if (sscanf (optarg, "%"PRIu64"", &bufsz) < 1) {
	fprintf (stderr, "test_ipcio could not parse -b %s", optarg);
	return -1;
      }
      break;

    case 'd':
      debug = 1;
      break;

    case 'n':
      if (sscanf (optarg, "%"PRIu64"", &nbufs) < 1) {
	fprintf (stderr, "test_ipcio could not parse -n %s", optarg);
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

  smbufsz = n64/3 + 7;
  smbuf = malloc (smbufsz * sizeof(uint64_t));
  assert (smbuf != 0);

  fprintf (stderr, "Running %u tests ...\n", ntest);

  if (fork()) {
    
    /* this process is writing to and creates the shared memory */
    fprintf (stderr, "Creating shared memory ring buffer."
	     " nbufs=%"PRIu64" bufsz=%"PRIu64"\n", nbufs, bufsz);

    if (ipcio_create (&ringbuf, key, nbufs, bufsz) < 0) {
      fprintf (stderr, "Error creating shared memory ring buffer\n");
      return -1;
    }

    ipcio_open (&ringbuf, 'w');

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
      sodbyte = raise_sod * sizeof(uint64_t);

      while (count < raise_eod) {

	for (i64=0; i64<smbufsz; i64++) {
	  smbuf[i64] = count;
	  count ++;
	}

	bytesio = smbufsz * sizeof(uint64_t);

	if (count >= raise_eod) {
	  bytesio -= (count - raise_eod) * sizeof(uint64_t);
	  count = raise_eod;
	}

	bytesio = ipcio_write (&ringbuf, (char*)smbuf, bytesio);

	if (count == raise_eod) {
 	  if (ipcio_stop (&ringbuf) < 0) {
	    fprintf (stderr, "Failure to raise eod count=%"PRIu64
		     " eod=%"PRIu64" sod=%"PRIu64" offset=%"PRIu64"\n",
		     count, raise_eod, raise_sod, offset);
	    return -1;
	  }
	}

	if (count >= offset && must_set_sod) {

	  if (verbose)
	    fprintf (stderr, "Enabling start-of-data"
		     " byte=%"PRIu64"\n", sodbyte);
	  
	  if (ipcio_start (&ringbuf, sodbyte) < 0)
	    return -1;

	  must_set_sod = 0;

	}

      }

      fprintf (stderr, "Finished %d %%\r", 
	       (int)(100*(float)itest/(float)ntest));

    }

    fprintf (stderr, "Flagging eod\n");
    ipcio_close (&ringbuf);

    fprintf (stderr, "Scheduling IPC resources for destruction\n");
    ipcio_destroy (&ringbuf);

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
    if (ipcio_connect (&ringbuf, key) < 0) {
      fprintf (stderr, "Error connecting to shared memory ring buffer\n");
      return -1;
    }

    for (itest=0; itest < ntest; itest++)  {

      raise_sod = raise_eod + rand()%(4 * nbufs * n64);
      offset = raise_sod + rand()%((nbufs-2) * n64);
      raise_eod = offset + n64 + rand()%(8 * nbufs * n64);

      if (verbose)
	fprintf (stderr, "test:%u sod=%"PRIu64" off=%"PRIu64" eod=%"PRIu64"\n",
		 itest, raise_sod, offset, raise_eod);

      count = raise_sod;

      ipcio_open (&ringbuf, 'R');

      while ( (int) (bytesio = ipcio_read (&ringbuf, (char*)smbuf, 
					   smbufsz * sizeof(uint64_t))) > 0) {

	check64 = bytesio / sizeof(uint64_t);

	for (i64=0; i64<check64; i64++) {
	  if (smbuf[i64] != count) {
	    fprintf (stderr, "\nERROR buf:%"PRIu64" %"PRIu64" bytes.\n",
		     ipcbuf_get_read_count ((ipcbuf_t*)&ringbuf), bytesio);
	    fprintf (stderr, "buf[%"PRIu64"]=%"PRIu64" != %"PRIu64"\n",
		     i64, smbuf[i64], count);
	    return -1;
	  }
	  count ++;
	}

	if (verbose)
	  fprintf (stderr, ".");

      }

      if (count != raise_eod) {
	fprintf (stderr, "Premature end of data!\n");
	return -1;
      }

      if (ipcio_close (&ringbuf) < 0) {
	fprintf (stderr, "error ipcio_close\n");
	return -1;
      }
      
    }

  }

  return 0;
}
