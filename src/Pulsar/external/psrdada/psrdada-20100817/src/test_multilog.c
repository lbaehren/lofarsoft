#include "multilog.h"

int main ()
{
  int occurs = 2;
  multilog_t* log = 0;

  fprintf (stderr, "Opening multilog\n");
  log = multilog_open ("test", 0);

  fprintf (stderr, "Adding stderr to multilog\n");
  multilog_add (log, stderr);

  fprintf (stderr, "Adding stdout to multilog\n");
  multilog_add (log, stdout);

  fprintf (stderr, "Writing message to multilog\n");
  multilog (log, LOG_INFO, "This message should come out %d times\n", occurs);

  fprintf (stderr, "Closing log\n");
  multilog_close (log);

  return 0;
}
