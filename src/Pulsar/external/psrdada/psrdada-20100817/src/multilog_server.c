#include "multilog.h"
#include "sock.h"

#include <signal.h>

static void* multilog_server (void * arg)
{
  multilog_t* log = (multilog_t*) arg;

  int listen_fd = 0;
  int comm_fd = 0;
  FILE* fptr = 0;

  listen_fd = sock_create (&(log->port));
  if (listen_fd < 0)  {
    perror ("multilog_server: Error creating socket");
    return 0;
  }

  while (log->port) {

    comm_fd = sock_accept (listen_fd);

    if (comm_fd < 0)  {
      perror ("multilog_server: Error accepting connection");
      return 0;
    }

    fptr = fdopen (comm_fd, "w");
    if (!fptr)  {
      perror ("multilog_server: Error creating I/O stream");
      return 0;
    }

    /* line buffer the output */
    setvbuf (fptr, 0, _IOLBF, 0);

    if (multilog_add (log, fptr) < 0) {
      perror ("multilog_server: Error adding stream");
      return 0;
    }

  }

  return 0;
}

int multilog_serve (multilog_t* log, int port)
{
#if 0
  sighandler_t handler = 
#endif
  signal (SIGPIPE, SIG_IGN);
#if 0
  if (handler != SIG_DFL)
    signal (SIGPIPE, handler);
#endif

  log->port = port;
  if (pthread_create (&(log->thread), 0, multilog_server, log) < 0) {
    perror ("multilog_serve: Error creating new thread");
    return -1;
  }
  return 0;
}

