
#include "sock.h"

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

void usage() {
  fprintf(stdout,
    "ibob_bramdump [options] host port\n"
    " host        on which the ibob interface is accessible\n"
    " port        on which the ibob interace is accessible\n"
    "\n"
    " -v          verbose\n"
    " -h          print help text\n");
}

char fake_telnet[] = { 255, 251, 37,
		       255, 253, 38,
		       255, 251, 38,
		       255, 253, 3,
		       255, 251, 24,
		       255, 251, 31,
		       255, 251, 32,
		       255, 251, 33,
		       255, 251, 34,
		       255, 251, 39,
		       255, 253, 5,
		       255, 251, 35,
		       0 };

int main (int argc, char** argv)
{
  char * hostname;
  int port;
  int verbose = 0;
  int arg = 0;
  
  //strcpy(hostname, "srv0.apsr.edu.au");
  //int port;

  while ((arg=getopt(argc,argv,"n:i:vh")) != -1) {
    switch (arg) {

    case 'v':
      verbose = 1;
      break;

    case 'h':
    default:
      usage();
      return 0;
    }
  }

  if ((argc - optind) != 2) {
    fprintf(stderr, "Error: host and port must be specified\n");
    usage();
    return EXIT_FAILURE;
  } else {
    hostname = argv[optind];
    port = atoi(argv[(optind+1)]);
  }

  unsigned retries = 33;

  while (retries)
  {

  int fd = sock_open(hostname, port);
  if (fd < 0)  {
    fprintf(stderr, "Error creating socket\n");
    return EXIT_FAILURE;
  } else {
    fprintf(stderr, "opened socket to %s on port %d [fd %d]\n",hostname, port, fd);
  }

  FILE* sockin = 0;
  FILE* sockout = 0;

  sockin = fdopen (fd, "r");
  sockout = fdopen (fd, "w");

  // set the socket output to be line-buffered
  setvbuf (sockout, 0, _IOLBF, 0);
  // setvbuf (sockin, 0, _IOLBF, 0);

  fprintf (sockout, fake_telnet);

  const char* cmd = "bramdump scope_output1/bram";

  fprintf (stderr, "sending bramdump request\n");
  fprintf (sockout, "%s\r\n", cmd);

  char buffer [128];
  char* rgot = 0;
  do 
  {
    rgot = fgets (buffer, 128, sockin);
  }
  while (rgot && !strstr(rgot, "IBOB"));

  fprintf (stderr, "reading response\n");

  unsigned i=0;
  for (i=0; i<2048; i++)
  {
    char buffer [128];
    char* rgot = fgets (buffer, 128, sockin);

    if (rgot && !feof(sockin))
      fprintf (stdout, "%s", buffer);
    else
    {
      if (feof (sockin))
        fprintf (stderr, "Socket connection terminated.\n");
      if (ferror (sockin))
        perror ("ferror after fgets");
      else
        perror ("fgets");
      break;
    }
  }

  sock_close (fd);
  if (i == 2048)
    break;

    retries --;
    fprintf (stderr, "Failed to read 2048 lines; retries=%d\n", retries);
    sleep (1);
  }
 
  return 0;
}

