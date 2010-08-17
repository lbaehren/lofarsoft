#include "ipcio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

static char quick = 0;

unsigned long job_size (unsigned ijob)
{
  if (ijob % 9 == 8)
    return ijob*1024;
  else if (quick)
    return random () / 1024;
  else
    return random ();
}


int main (int argc, char** argv)
{
  ipcio_t ringbuf = IPCIO_INIT;

  key_t key = 0x69;

  uint64_t nbufs = 32;
#ifdef _SC_PAGE_SIZE
  uint64_t bufsz = sysconf (_SC_PAGE_SIZE);
#else
  uint64_t bufsz = 4*1024;
#endif

  unsigned long* smbuf = 0;
  unsigned smbufsz = 0;
  unsigned ismbuf = 0;

  unsigned njob = 1000;
  unsigned ijob = 0;

  unsigned long nlong;
  unsigned long ilong = 0;
  uint64_t jlong = 0;

  ssize_t bytesio = 0;

  char read = 0;
  char write = 0;
  char open = 0;

  int debug = 0;
  int arg;

  while ((arg = getopt(argc, argv, "db:n:k:qrw")) != -1) {

    switch (arg)  {
    case 'h':
      fprintf (stderr, "test_ipcio -[r|w] filename\n");
      return 0;

    case 'd':
      debug = 1;
      break;

    case 'r':
      read = 1;
      break;

    case 'w':
      write = 1;
      break;

    case 'k':
      key = atoi (optarg);
      break;

    case 'q':
      quick = 1;
      break;

    }
  }

  if ((read && write) || !(read || write)) {
    fprintf (stderr, "Please specify one of -r or -w\n");
    return -1;
  }

  smbufsz = bufsz/3 + 7;
  smbuf = malloc (smbufsz * sizeof(unsigned long));
  assert (smbuf != 0);

  fprintf (stderr, "small buf size = %d\n", smbufsz);

  srandom (73);

  if (write) {

    /* this process is reading from the file and creates the shared memory */
    fprintf (stderr, "Creating shared memory ring buffer."
	     " nbufs=%"PRIu64" bufsz=%"PRIu64"\n", nbufs, bufsz);

    if (ipcio_create (&ringbuf, key, nbufs, bufsz) < 0) {
      fprintf (stderr, "Error creating shared memory ring buffer\n");
      return -1;
    }

    for (ijob=0; ijob < njob; ijob++) {

      if (!open) {
	fprintf (stderr, "Opening for writing\n");
	if (ipcio_open (&ringbuf, 'W') < 0) {
          fprintf (stderr, "Error while opening for writing\n");
          return -1;
        }
	open = 1;
      }
      else {
	fprintf (stderr, "Starting at %"PRIu64"\n", jlong);
	if (ipcio_start (&ringbuf, jlong*sizeof(unsigned long)) < 0) {
          fprintf (stderr, "Error while starting\n");
          return -1;
        }
      }

      nlong = job_size (ijob);
      ilong = 0;

      fprintf (stderr, "Writing %ld longs\n", nlong);

      while (ilong < nlong) {

	// fill the next buffer out with data
	for (ismbuf=0; ismbuf < smbufsz && ilong < nlong; (ismbuf++, ilong++))
	  smbuf[ismbuf]=ilong;

	bytesio = ipcio_write (&ringbuf, (char*)smbuf,
			       ismbuf*sizeof(unsigned long));

	if (bytesio < 0) {
	  perror ("ipcio_write error");
	  return -1;
	}

      }

      if (ijob % 10 == 3) {
	fprintf (stderr, "Closing\n");
	ipcio_close (&ringbuf);
        jlong = 0;
	open = 0;
      }
      else {
	fprintf (stderr, "Stopping\n");
	ipcio_stop (&ringbuf);
        jlong += ilong;
      }

    }

    if (open)  {
      fprintf (stderr, "Closing\n");
      ipcio_close (&ringbuf);
    }

    sleep (1);
    fprintf (stderr, "Scheduling IPC resources for destruction\n");
    ipcio_destroy (&ringbuf);

  }

  else {

    fprintf (stderr, "Connecting to shared memory ring buffer\n");

    if (ipcio_connect (&ringbuf, key) < 0) {
      fprintf (stderr, "Error connecting to shared memory ring buffer\n");
      return -1;
    }

    for (ijob=0; ijob < njob; ijob++) {

      if (!open) {
	fprintf (stderr, "Opening for reading\n");
	ipcio_open (&ringbuf, 'R');
	open = 1;
      }

      nlong = job_size (ijob);
      ilong = 0;

      fprintf (stderr, "Reading %ld longs\n", nlong);

      while (!ipcbuf_eod((ipcbuf_t*)&ringbuf)) {

	bytesio = ipcio_read (&ringbuf, (char*)smbuf, 
			      smbufsz*sizeof(unsigned long));

	if (bytesio < 0) {
	  perror ("ipcio_read error");
	  return -1;
	}

	bytesio /= sizeof(unsigned long);

	// fill the next buffer out with data
	for (ismbuf=0; ismbuf < bytesio; (ismbuf++, ilong++))
	  if (smbuf[ismbuf] != ilong)  {
	    fprintf (stderr, "Error on long[%d]=%ld != %ld\n",
		     ismbuf, smbuf[ismbuf], ilong);
	    return -1;
	  }

      }

      fprintf (stderr, "Closing\n");
      ipcio_close (&ringbuf);
      open = 0;

    }

  }

  return 0;
}
