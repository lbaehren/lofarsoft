/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utc.h"

int main ()
{
  char tempstr1 [256];
  char tempstr2 [256];
  utc_t utc;
  struct tm cal;
  struct tm* date;
  time_t present;

  strcpy (tempstr1,  "1997251043045");
  str2utc (&utc, tempstr1);
  printf ("TIME %s  ->  %s\n\n", tempstr1,
	   utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1997-251-04:30:45"))  {
    fprintf (stderr, "\tdoes not match expected answer '1997-251-04:30:45'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "970309-094500");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1997-068-09:45:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '1997-068-09:45:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "880309-094500");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1988-069-09:45:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '1988-069-09:45:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "000309-000000");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "2000-069-00:00:00"))  {
    fprintf (stderr, 
	     "WARNING: tm2utc() or str2tm() is sensitive to Y2000 kludges!\n");
    fprintf (stderr, "\texpected answer '2000-069-00:00:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "20000309-000000");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "2000-069-00:00:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '2000-069-00:00:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "960201_205958");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1996-032-20:59:58"))  {
    fprintf (stderr, "\tdoes not match expected answer '1996-032-20:59:58'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "960229_160535");
  str2tm (&cal, tempstr1);
  tm2utc (&utc, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1996-060-16:05:35"))  {
    fprintf (stderr, "\tdoes not match expected answer '1996-060-16:05:35'\n");
    return -1;
  }
  printf ("\n");


  present = time (NULL);
  date = gmtime (&present);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }
  strftime (tempstr1, 25, "%Y/%m/%d-%H:%M:%S", date);
  tm2utc (&utc, *date);
  printf ("TM %s -> UTC %s -> ", tempstr1, 
	  utc2str (tempstr2, utc, "yyyy-ddd-hh:mm:ss"));

  utc2tm (&cal, utc);
  strftime (tempstr2, 25, "%Y/%m/%d-%H:%M:%S", &cal);

  printf ("TM %s\n", tempstr2);

  if (strcmp (tempstr1, tempstr2))  {
    fprintf (stderr, "\nERROR!! TM1: %s does not match TM2: %s\n",
	     tempstr1, tempstr2);
    return -1;
  }
  printf ("\n**********************************************************\n");
  printf ("              UTC to TM operations test completed ok.");
  printf ("\n**********************************************************\n");

  return 0;
}
