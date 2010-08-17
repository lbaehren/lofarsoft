/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "mulbibob.h"

#include <unistd.h>
#include <ctype.h>

void usage()
{
  printf ("mulbibob_server -n N \n"
	  " -n N        number of iBoBs to be controlled [default: 1] \n"
	  " -p port     port on which to listen for control connections \n"
	  " -v          verbose \n" );
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
  int bibob_number = 1;
  int port = 0;

  int verbose = 0;
  int arg = 0;

  while ((arg=getopt(argc,argv,"n:p:vh")) != -1)
  {
    switch (arg) {

    case 'n':
      bibob_number = atoi (optarg);
      break;

    case 'p':
      port = atoi (optarg);
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

  mulbibob_t* mulbibob = mulbibob_construct (bibob_number);
  mulbibob->port = port;

  mulbibob_serve (mulbibob);
  mulbibob_destroy (mulbibob);

  return 0;
}

