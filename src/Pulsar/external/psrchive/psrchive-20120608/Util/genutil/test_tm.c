/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

extern long timezone;

int main ()
{
  time_t calculated, backup;
  struct tm* date;
  char datestr1 [25];
  char datestr2 [25];
  int out;

  calculated = time (NULL);
  backup = calculated;

  date = localtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling localtime");
    return -1;
  }

  if (backup != calculated)  {
    printf ("WARNING: localtime() modifies its time_t argument.!!\n");
    calculated = backup;
  }

  out = strftime (datestr1, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("local: %s\n", datestr1);

  if (backup != calculated)  {
    printf ("WARNING: strftime() modifies its time_t argument.!!\n");
    calculated = backup;
  }

  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }
  out = strftime (datestr1, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("UTC: %s\n", datestr1);

  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }

  calculated = mktime (date);
  calculated -= timezone;

  if (backup != calculated)  {
    printf ("ERROR: mktime() calculation provides unexpected result.!!\n");
  }
  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }

  out = strftime (datestr2, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("UTC: %s\n", datestr2);

  if (strcmp (datestr1, datestr2)) {
    fprintf (stderr, "Inconsistent UTC returned by mktime()!\n");
    return -1;
  }

  date = localtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling localtime");
    return -1;
  }

  out = strftime (datestr2, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("local: %s\n", datestr2);

  printf ("difference between UTC and local time: %ld seconds.\n", timezone);

  if (backup != calculated)
    return -1;

  printf ("\n**********************************************************\n");
  printf ("                  TM operations test completed ok.");
  printf ("\n**********************************************************\n");

  return 0;
}

