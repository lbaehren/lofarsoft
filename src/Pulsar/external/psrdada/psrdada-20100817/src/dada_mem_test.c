/*
 * dada_mem_test
 *
 * Advanced memory test program
 *
 */

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
	   "dada_mem_test\n"
     " -t secs  length of test [default %d s]\n"
     " -r       do a read test instead of write\n",
     DEFAULT_WRITE_TIME);
}

int main (int argc, char **argv)
{

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* write time [s] */
  unsigned write_time = DEFAULT_WRITE_TIME;

  unsigned read_test = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"rvt:")) != -1)
    switch (arg) {

    case 't':
      if (sscanf (optarg, "%u", &write_time) != 1) {
        fprintf (stderr,"Error: could not parse write time from %s\n",optarg);
        usage();
        return -1;
      }
      fprintf(stderr, "writing for %d seconds\n", write_time);
      break;

    case 'r':
      read_test = 1;

    case 'v':
      verbose=1;
      break;
      
    default:
      usage ();
      return 0;
      
    }

  char * array1 = 0;
  char * array2 = 0;
  unsigned i = 0;

  // do reads/writes of chunk size
  int chunk = 4096;

  // total size to read/write
  int size = 1024*1024*1024;

  array1 = (char *) malloc (sizeof(char) * size);
  array2 = (char *) malloc (sizeof(char) * size);

  if (!array1) {
    fprintf(stderr, "failed to create memory array1\n");
    exit(1);
  }

  if (!array1) { 
    fprintf(stderr, "failed to create memory array2\n");
    exit(1);
  }

  fprintf(stderr, "filling array1\n");
  for (i=0; i<chunk; i++) {
    array1[i] = i % 255;
  }

  for (i=0; i<size; i+=chunk) {
    memcpy (array1 + i, array1, chunk);
  }

  time_t start = time(0);
  time_t end = start + write_time;
  time_t now = start;
  time_t prev = start;

  int ptr = 0;
  long int bytes = 0;

  fprintf(stderr, "starting: chunk=%d, size=%d\n", chunk, size);

  while (now < end) {

    memcpy (array2+ptr, array1+ptr, chunk);

    ptr += chunk;
    bytes += chunk;

    if (ptr >= size)
      ptr = 0;

    now = time(0);
    if (now > prev) {
      fprintf(stderr, "rate = %f BPS\n", (float) bytes / (1024*1024));
      bytes = 0;
    }
    prev = now;

  }

  free (array1);
  free (array2);

  return EXIT_SUCCESS;
}

