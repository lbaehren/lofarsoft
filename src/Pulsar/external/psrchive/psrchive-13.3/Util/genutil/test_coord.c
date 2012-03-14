/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coord.h"

int main ()
{
  char str1 [256];
  char str2 [256];

  double ra1, dec1;
  double ra2, dec2;

  int places = 3;

  strcpy (str1, "0437-4715");
  str2coord (&ra1, &dec1, str1);

  strcpy (str2, "04:37:00.000-47:15:00.000");
  str2coord (&ra2, &dec2, str2);

  if (ra1 != ra2) {
    fprintf (stderr, "ra1=%lf != ra2=%lf\n", ra1, ra2);
    return -1;
  }

  if (dec1 != dec2) {
    fprintf (stderr, "dec1=%lf != dec2=%lf\n", dec1, dec2);
    return -1;
  }

  places = 5;
  strcpy (str1, "16:37:07.89653+07:04:59.99999");
  str2coord (&ra1, &dec1, str1);

  coord2str (str2, 256, ra1, dec1, places);

  if (strcmp (str1, str2) != 0) {
    fprintf (stderr, "str1=%s != str2=%s\n", str1, str2);
    return -1;
  }

  fprintf (stderr, " input string: %s\n", str1);
  fprintf (stderr, "output string: %s\n", str2);

  printf ("\n**********************************************************\n");
  printf ("         string to coord operations test completed ok.");
  printf ("\n**********************************************************\n");

  return 0;
}
