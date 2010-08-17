#include "sock.h"
#include "dada_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#define MULTIBOB_VLAN_BASE "169.254.128."
#define MULTIBOB_PORT 23
#define MULTIBOB_COUNT 13

void usage()
{
  fprintf(stdout,
	  "ibob_rearm_trigger host port \n"
	  " host        on which the ibob interface is accessible \n"
	  " port        on which the ibob interace is accessible \n"
	  " -M          arm iBob on %s1 to %d port %d \n"
	  " -v          verbose\n",

	  MULTIBOB_VLAN_BASE, MULTIBOB_COUNT, MULTIBOB_PORT );
}

static char buffer[64];
static int buffer_size = 64; 

void log_bad_open (FILE* fptr, const char* hostname, int port)
{
  time_t current = time(0);
  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));

  fprintf (fptr, "[%s] Error opening %s %d: %s\n",
	   buffer, hostname, port, strerror(errno));
}


int main (int argc, char** argv)
{
  char * hostname;
  int port;

  char multibob = 0;
  int verbose = 0;
  int arg = 0;

  FILE *fptr = 0;

  char buffer2[64];

  while ((arg=getopt(argc,argv,"Mn:i:vh")) != -1) {
    switch (arg) {

    case 'M':
      multibob = 1;
      break;

    case 'v':
      verbose = 1;
      break;

    case 'h':
      usage();
      return 0;

    default:
      usage();
      return 0;
    }
  }

  if (multibob)
  {
    hostname = strdup (MULTIBOB_VLAN_BASE"XXXXXXX");
    port = MULTIBOB_PORT;
  }
  else
  {
    if ((argc - optind) != 2)
    {
      fprintf(stderr, "Error: host and port must be specified\n");
      usage();
      return EXIT_FAILURE;
    }
    hostname = argv[optind];
    port = atoi(argv[(optind+1)]);
  }

  fptr = fopen("/lfs/data0/bpsr/logs/rearm.log","a");

  char command[100];
  int rval = 0;

  int* fds = 0;
  unsigned nfd = 1;
  unsigned ifd = 0;

  if (multibob)
  {
    fds = (int*) malloc (sizeof(int) * MULTIBOB_COUNT);
    nfd = MULTIBOB_COUNT;
    for (ifd=0; ifd<MULTIBOB_COUNT; ifd++)
    {
      sprintf (hostname, MULTIBOB_VLAN_BASE"%d", ifd+1);
      fds[ifd] = sock_open (hostname, port);
      if ( fds[ifd] < 0 )
      {
	log_bad_open (stderr, hostname, port);
	if (fptr)
	  log_bad_open (fptr, hostname, port);
      }
      
      if (verbose) 
	printf ("opened socket to %s on port %d [fd %d]\n",
		hostname, port, fds[ifd]);

    }
  }
  else
  {
    fds = (int*) malloc (sizeof(int));
    nfd = 1;
    fds[0] = sock_open (hostname, port);
    if ( fds[0] < 0 )
    {
      log_bad_open (stderr, hostname, port);
      if (fptr)
	log_bad_open (fptr, hostname, port);
      return -1;
    }

    if (verbose) 
      printf ("opened socket to %s on port %d [fd %d]\n",
	      hostname, port, fds[0]);

  }

  struct timeval timeout;
  timeout.tv_sec=0;
  timeout.tv_usec=500000;

  time_t current = time(0);
  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));

  if (fptr)
    fprintf (fptr, "[%s] Waiting for next second\n", buffer);

  /* Busy sleep until a second has ticked over */
  time_t prev = current;
  while (current == prev)
    current = time(0);

  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));

  if (fptr)
    fprintf (fptr, "[%s] 1 second tick, waiting for 500,000 usecs\n", buffer);

  /* Now sleep for 0.5 seconds */
  select(0,NULL,NULL,NULL,&timeout);

  current = time(0);
  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));

  if (fptr)
    fprintf (fptr, "[%s] Wait over, rearming\n", buffer);

  sprintf(command, "regwrite reg_arm 0\r\n");

  current = time(0);
  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));

  if (fptr)
    fprintf (fptr, "[%s] regwrite reg_arm 0\n", buffer);

  if (verbose) printf("<- %s",command);

  for (ifd=0; ifd < nfd; ifd++)
  {
    if (fds[ifd] < 0)
      continue;

    rval = sock_write (fds[ifd], command, strlen(command)-1);
    if (rval < 0)
    {
      fprintf (stderr, "could not write command %s\n",command);
      sock_close(fds[ifd]);

      if (nfd == 1)
	return 1;

      fds[ifd] = -1;
    }
  }

  timeout.tv_sec=0;
  timeout.tv_usec=10000;
  select(0,NULL,NULL,NULL,&timeout);

  current = time(0);
  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));
  
  if (fptr)
    fprintf (fptr, "[%s] regwrite reg_arm 1\n", buffer);

  sprintf(command, "regwrite reg_arm 1\r\n");

  if (verbose) printf("<- %s",command);

  for (ifd=0; ifd < nfd; ifd++)
  {
    if (fds[ifd] < 0)
      continue;

    rval = sock_write(fds[ifd], command, strlen(command)-1);

    sock_close (fds[ifd]);

    if (rval < 0 )
    {
      fprintf(stderr, "could not write command %s\n",command);
      if (nfd == 1)
	return 1;
    }
  }

  current = time(0);
  printf("%d\n",(current+1));

  strftime (buffer, buffer_size, DADA_TIMESTR, localtime(&current));
  current++;
  strftime (buffer2, buffer_size, DADA_TIMESTR, localtime(&current));

  if (fptr)
    fprintf (fptr, "[%s] returing time %d [%s]\n", buffer, (current), buffer2);

  if (fptr)
    fclose (fptr);

  return 0;
}

