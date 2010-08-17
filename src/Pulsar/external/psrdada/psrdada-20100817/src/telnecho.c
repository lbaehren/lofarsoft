#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

int main (int argc, char** argv)
{
  char bigquit = 0;
  char quit = 0;

  int sfd;
  int cfd;

  FILE* sockin = 0;
  FILE* sockout = 0;

  char* rgot = NULL;

  char hostname [100];
  int port;

  unsigned bufsize = 1024;

  char* inmsg = (char*) malloc ( bufsize );
  assert (inmsg != 0);

  port = 20013;
  if (argc > 1)
    port = atoi (argv[1]);

  if (sock_getname (hostname, 100, 1) < 0) {
    perror ("Error getting hostname.\n");
    return -1;
  }


  sfd = sock_create (&port);
  if (sfd < 0)  {
    perror ("Error creating socket\n");
    return -1;
  }

  do  {

    fprintf (stderr, "%s available on %d\n",hostname, port);
    cfd = sock_accept (sfd);
    fprintf (stderr, "Connection accepted.\n");
    
    sockin = fdopen (cfd, "r");
    sockout = fdopen (cfd, "w");

    // set the socket output to be line-buffered
    setvbuf (sockout, 0, _IOLBF, 0);

    fprintf (sockout,
	     "Welcome to telnecho.\n"
	     "Enter a line of text for me to echo.\n");

    quit = 0;

    while (sockin) {

      rgot = fgets (inmsg, bufsize, sockin);

      if (rgot && !feof(sockin)) {
	fprintf (stderr, "Sending: '%s'\n", inmsg);
	fprintf (sockout, inmsg);
      }

      else {

	if (feof (sockin))
	  fprintf (stderr, "Socket connection terminated.\n");

	else if (!rgot) {
	  if (ferror (sockin))
	    perror ("ferror after fgets");
	  else
	    perror ("fgets");
	}

	fclose (sockin); sockin = NULL;

      }

    }

    if (! (feof (sockout) || ferror (sockout))) {
      fprintf (sockout, "Closing connection...\n");
      fclose (sockout);
    }
  }

  while (!bigquit);

  fprintf (stderr, "Server exiting\n");
  sock_close (sfd);
  return 0;
}
