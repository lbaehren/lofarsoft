
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "daemon.h"


void usage ()
{
  fprintf (stdout,
	   "daemon_test      Test the daemon code\n"
	   "\n");
}

int main (int argc, char** argv)
{
  int arg;

  char * log_file = 0;

  while ((arg = getopt(argc, argv, "l:")) != -1) {

    switch (arg)  {
    case 'l':
      if (optarg) 
        log_file = optarg;
      else {
        fprintf(stderr, "must specify a logfile\n");
        usage();
        return (1);
      }
      break;
    }
  }

  if (log_file) 
    be_a_daemon_with_log (log_file);
  else
    be_a_daemon();

  printf("line1\n");
  sleep(1);
  printf("line2\n");

  return 0;
}
