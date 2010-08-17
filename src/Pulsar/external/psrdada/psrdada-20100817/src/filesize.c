#include "futils.h"

#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

long filesize (const char* filename)
{
  struct stat statistics;
  if (stat (filename, &statistics) < 0) {
    fprintf (stderr, "filesize() error stat (%s)", filename);
    perror ("");
    return -1;
  }

  return (long) statistics.st_size;
}

