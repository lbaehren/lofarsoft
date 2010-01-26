/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dirutil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

uint64_t filesize (const char* filename)
{
  struct stat statistics;
  if (stat (filename, &statistics) < 0)
  {
    fprintf (stderr, "filesize error stat (%s): ", filename);
    perror ("");
    return 0;
  }
  return (uint64_t) statistics.st_size;
}
