/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "mini++.h"
#include "convert_endian.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

std::string Mini::reason;

void Mini::init (struct mini& hdr)
{
  // ensure that the entire struct is set to zero
  memset (&hdr, 0, sizeof(struct mini));

  hdr.junk = hdr.junk2 = hdr.junk3 = -1;
}

int Mini::fload (const char* fname, struct mini* hdr, bool big_endian)
{
  FILE* fptr = fopen (fname, "r");
  if (fptr == NULL) {
    fprintf (stderr, "Mini::load Cannot open '%s'", fname);
    perror ("");
    return -1;
  }
  int ret = load (fptr, hdr, big_endian);
  fclose (fptr);
  return ret;
}

int Mini::load (FILE* fptr, struct mini* hdr, bool big_endian)
{
  if (fread (hdr, sizeof(struct mini), 1, fptr) < 1)  {
    fprintf (stderr, "Mini::load Cannot read mini struct from FILE*");
    perror ("");
    return -1;
  }
  if (big_endian)
    mini_fromBigEndian (hdr);
  else
    mini_fromLittleEndian (hdr);
  return 0;
}

// unloads a mini struct to a file (always big endian)
int Mini::unload (FILE* fptr, struct mini& hdr)
{
  mini_toBigEndian (&hdr);
  if (fwrite (&hdr, sizeof(struct mini), 1, fptr) < 1)  {
    fprintf(stderr,"Mini::unload Cannot write mini struct to FILE*");
    perror ("");
    return -1;
  }
  mini_fromBigEndian(&hdr);
  return 0;
}

MJD Mini::get_MJD (const struct mini& hdr)
{ 
  return MJD(hdr.mjd, hdr.fracmjd);
}

void Mini::set_MJD (struct mini& hdr, const MJD& mjd)
{
  hdr.mjd = mjd.intday();
  hdr.fracmjd = mjd.fracday();
}
