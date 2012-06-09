/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits>

#include "MJD.h"

using namespace std;

//! convert an MJD to long double
long double from_MJD (const MJD& t);

//! convert a long double to an MJD
MJD to_MJD (long double t);


int main ()
{
  struct tm* date;
  time_t present;
  char tempstr1 [256];
  char tempstr2 [256];

  present = time (NULL);

  date = gmtime (&present);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }
  strftime (tempstr1, 50, "%Y/%m/%d-%H:%M:%S", date);

  MJD mjd (*date);

  if (mjd.datestr (tempstr2, 50, "%Y/%m/%d-%H:%M:%S") == NULL) {
    perror ("Error calling MJD::datestr");
    return -1;
  }
  printf ("TM %s -> MJD %s -> TM %s\n", 
	  tempstr1, mjd.printdays(5).c_str(), tempstr2);

  if (strcmp (tempstr1, tempstr2))  {
    fprintf (stderr, "\nERROR!! TM1: %s does not match TM2: %s\n",
	     tempstr1, tempstr2);
    return -1;
  }
  printf ("\n**********************************************************\n");
  printf ("              MJD to TM operations test completed ok.");
  printf ("\n**********************************************************\n");


  cerr << "\nlong double digits10 = " 
       << numeric_limits<long double>::digits10 << endl;

  double mjd_start = 53140 + M_PI;

  MJD mjd0 (mjd_start);

  long double seconds_in_day = 24.0 * 60.0 * 60.0;
  long double mjds = mjd_start * seconds_in_day;

  double precision = pow (1.0, -numeric_limits<long double>::digits10);

  for (long double time = 0; time < 6000.0 * seconds_in_day; time += 100*M_PI)
    {
      MJD epoch = mjd0 + time;
      long double epochs = mjds + time;

      {
	long double test = from_MJD (epoch) * seconds_in_day;
	if ( fabs(test - epochs) / test > precision) {
	  cerr << "long double test=" << test << " != " << epochs << endl
	       << " diff=" << test - epochs << " seconds" << endl
	       << " rel=" << (test - epochs) / test 
	       << endl;
	  return -1;
	}
      }

      {
	MJD test = to_MJD (epochs/seconds_in_day);
	if ( fabs((test - epoch).in_days()) / test.in_days() > precision) {
	  cerr << "MJD test=" << test.printdays(20) << " != "
	       << epoch.printdays(20) << endl
	       << " diff=" << (test - epoch).in_seconds() << " seconds" << endl
	       << " rel=" << fabs((test - epoch).in_days()) / test.in_days()
	       << endl;
	  return -1;
	}
      }

    }

  printf ("\n**********************************************************\n");
  printf ("      MJD vs long double precision test completed ok.");
  printf ("\n**********************************************************\n");

  return 0;
}

//! convert an MJD to long double
long double from_MJD (const MJD& t)
{
  const long double secs_in_day = 86400.0L;

  return 
    (long double) (t.intday()) +
    (long double) (t.get_secs()) / secs_in_day +
    (long double) (t.get_fracsec()) / secs_in_day;
}

//! convert a long double to an MJD
MJD to_MJD (long double t)
{
  long double input = t;

  long double ld = floorl(t);
  t -= ld;
  long double lsec = t * 86400.0L;
  long double ls = floorl(lsec);
  lsec -= ls;

  long double result = ld + (lsec + ls) / 86400.0L;
  if (result != input)
    cerr << "result-input=" << result - input << endl;

  return MJD (int(ld), int(ls), double(lsec));
}
