/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int main ()
{
  mode_t dirmode = S_ISUID | S_ISGID   /* set user and group id */
                 | S_IRWXU | S_IRWXG | S_IRWXO; /* read write exec all */

  /* get the user's home directory */
  char* home = getenv ("HOME");

  /* get the user's mask */
  mode_t old_mask = umask (0);

  /* put it back */
  umask (old_mask);

  /* mask out the bits */
  dirmode &= ~old_mask;

  if (!home)
  {
    fprintf (stderr, "$HOME undefined\n");
    return -1;
  }

  if (mkdir (home, dirmode) != 0)
  {
    fprintf (stderr, "mkdir (%s) error: %s\n", home, strerror(errno));
    if (errno != EEXIST)
      return -1;
  }

  fprintf (stderr, "mkdir ($HOME) works as expected\n");

  if (mkdir ("/tmp", dirmode) != 0)
  {
    fprintf (stderr, "mkdir (/tmp) error: %s\n", strerror(errno));
    if (errno != EEXIST)
      return -1;
  }

  fprintf (stderr, "mkdir (/tmp) works as expected\n");

  return 0;
}
