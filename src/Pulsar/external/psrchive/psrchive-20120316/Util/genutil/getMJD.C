/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "MJD.h"

int main (int argc, char* argv[]) 
{
  MJD  mjd;
  char printhere[40];
  bool verbose = false;
  bool mjdgiven = false;

  int c;
  while ((c = getopt(argc, argv, "hm:v")) != -1) {
    switch (c)  {
    case 'h':
      fprintf (stderr, "getMJD [date]\n");
      fprintf (stderr, "WHERE: date is a UTC of the form yyyy-ddd-hh:mm:ss\n");
      fprintf (stderr, "       date is optional. [default: now]\n");
      fprintf (stderr, "ALSO:  getMJD -m mjd\n");
      fprintf (stderr, "       prints the date of given MJD\n");
      return 0;

    case 'm':
      if (verbose)
	fprintf (stderr, "getMJD: parse MJD from '%s'\n", optarg);
      mjd.Construct (optarg);
      mjdgiven = true;
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (mjdgiven) {
    struct tm date;
    double fracsec;
    mjd.gregorian (&date, &fracsec);
    fprintf (stderr, "Date for MJD given: %s\n", asctime(&date));
    return 0;
  }

  if (optind < argc) {
    if (verbose)
      fprintf (stderr, "Converting '%s' to UTC and using as date",
	       argv[optind]);

    utc_t utcdate;
    if (str2utc (&utcdate, argv[optind]) < 0) {
      fprintf (stderr, "Error converting '%s' to UTC.\n", argv[optind]);
      return -1;
    }
    fprintf (stderr, "Using UTC parsed: %s\n",
	       utc2str (printhere, utcdate, "yyyy-ddd-hh:mm:ss"));
    mjd = MJD (utcdate);
  }
  else {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    fprintf (stderr, "Using today's date: %s\n", asctime(&date));
    mjd = MJD (date);
  }

  printf ("%11.5f\n", mjd.in_days());

  if (verbose) {
    utc_t  stuffback;
    mjd.UTC (&stuffback);
    printf ("utc from MJD: %s\n", 
	    utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss"));
  }

  // cerr << mjd.printdays(15);

  return 0;
}


