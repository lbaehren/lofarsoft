#include "futils.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

long fileread (const char* filename, char* buffer, unsigned bufsz)
{
  FILE* fptr = 0;
  long fsize = filesize (filename);

  if (fsize < 0) {
    fprintf (stderr, "fileread: filesize(%s) %s\n", filename, strerror(errno));
    return -1;
  }

  if (fsize > bufsz) {
    fprintf (stderr, "fileread: filesize=%ld > bufsize=%u\n", fsize, bufsz);
    return -1;
  }

  fptr = fopen (filename, "r");
  if (!fptr) {
    fprintf (stderr, "fileread: fopen(%s) %s\n", filename, strerror(errno));
    return -1;
  }

  if (fread (buffer, fsize, 1, fptr) != 1) {
    perror ("fileread: fread");
    fclose (fptr);
    return -1;
  }

  fclose (fptr);

  memset (buffer + fsize, '\0', bufsz - fsize);

  return 0;
}

