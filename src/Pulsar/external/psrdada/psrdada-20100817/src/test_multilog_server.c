#include "multilog.h"

#include <unistd.h>
#include <time.h>

int main ()
{
  multilog_t* log = 0;
  time_t seconds;
  int port = 2030;

  fprintf (stderr, "Opening multilog\n");
  log = multilog_open ("time", 0);

  fprintf (stderr, "Serving on %d\n", port);
  multilog_serve (log, port);

  while (1) {
    time(&seconds);
    multilog (log, LOG_INFO, ctime(&seconds));
    sleep (1);
  }

  fprintf (stderr, "Closing log\n");
  multilog_close (log);

  return 0;
}

