/***************************************************************************
 *
 *   Copyright (C) 2009 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "bibob.h"

void usage() {
  fprintf(stdout,
    "bibob_terminal [options] host port\n"
    " host        of the ibob telnet interface\n"
    " port        of the ibob telnet interace\n"
    "\n"
    " -v          verbose\n"
    " -h          print help text\n");

}

int main (int argc, char** argv)
{

  char * hostname;
  int port;
  int verbose = 0;
  int n_attempts = 3;
  int arg = 0;

  while ((arg=getopt(argc,argv,"vh")) != -1) {

    switch (arg) {

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

  if ((argc - optind) != 2) {
    fprintf(stderr, "Error: host and port must be specified\n");
    usage();
    return EXIT_FAILURE;
  } else {
    hostname = argv[optind];
    port = atoi(argv[(optind+1)]);
  }

  bibob_t * bibob = bibob_construct();
  int r = 0;


  r = bibob_set_host(bibob, hostname, port);

   r= bibob_open(bibob);

  unsigned done = 0;
  char command[100];
  int len = 0;
  size_t bytes = 0;

  while (!done) {

    printf("Enter a command: ");
    fgets (command, 100, stdin);
    len = strlen(command);
    command[len-1] = '\0';

    if (strcmp(command, "quit") == 0) {
      done = 1;
      printf("exiting\n");
    } else {
      bytes = bibob_send(bibob, command);
      printf("Sent %s in %d bytes\n", command, bytes);

      bytes = bibob_recv(bibob);
      printf("Received response in %d bytes:\n", bytes);
      printf("%s\n", bibob->buffer);
    }
  }

  bibob_close(bibob);
  bibob_destroy(bibob);

  return 0;
}


