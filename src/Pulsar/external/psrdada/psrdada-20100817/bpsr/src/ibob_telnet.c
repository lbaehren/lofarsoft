
#include "sock.h"
#include "ibob.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

void usage()
{
  printf ("ibob_telnet [host port|-n N] \n"
	  " host        on which the ibob interface is accessible \n"
	  " port        on which the ibob interace is accessible \n"
	  " -n N        connect to %sN port %d \n"
	  " -v          verbose\n", IBOB_VLAN_BASE, IBOB_PORT );
}

void filter (const char* out)
{
  if (!out)
    return;

  for (; *out != '\0'; out++)
    if (!isprint(*out) && *out != '\n' && *out != '\r')
      printf ("telnet code: %d received\n", (unsigned)(*out));
    else
      putchar (*out);
}

int main (int argc, char** argv)
{
  int ibob_number = 0;

  int verbose = 0;
  int arg = 0;

  while ((arg=getopt(argc,argv,"n:i:vh")) != -1)
  {
    switch (arg) {

    case 'n':
      ibob_number = atoi (optarg);
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

  ibob_t* ibob = ibob_construct ();

  if (ibob_number > 0)
    ibob_set_number (ibob, ibob_number);
  else
  {
    if ((argc - optind) != 2)
    {
      fprintf(stderr, "Error: host and port must be specified\n");
      usage();
      return EXIT_FAILURE;
    }
    ibob_set_host (ibob, argv[optind], atoi(argv[optind+1]));
  }

  printf ("ibob_telnet: opening %s %d\n", ibob->host, ibob->port);
  printf ("ibob_telnet: type 'quit' to exit gracefully\n");

  if ( ibob_open (ibob) < 0 )
  {
    fprintf (stderr, "could not open %s %d: %s\n",
	     ibob->host, ibob->port, strerror(errno));
    return -1;
  }

#define BUFFER 1024
  char buffer [BUFFER];

  while (1)
  {
    fprintf (stderr, "MYBOB %% ");
    fgets (buffer, BUFFER, stdin);

    if (strstr (buffer, "quit"))
      break;

    char* newline = strchr (buffer, '\n');
    if (newline)
      *newline = '\0';

    ibob_send (ibob, buffer);

    ssize_t got = 0;

    do 
    {
      got = ibob_recv (ibob, buffer, BUFFER);
      fwrite (buffer, 1, got, stderr);
    }
    while (got == BUFFER);

  }

  fprintf (stderr, "ibob_telnet: closing connection\n");
  ibob_close (ibob);
  return 0;
}

