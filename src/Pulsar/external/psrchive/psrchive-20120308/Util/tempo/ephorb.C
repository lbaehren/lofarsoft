/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar ephemeris and MJD, computes binary phase
//
// interface to routines in psrephem_orbital.C by Aidan Hotan
//
// /////////////////////////////////////////////////////////////////////

#include "psrephem_orbital.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using Legacy::psrephem;

void usage()
{
  cout << "\n"
    "ephorb: compute binary phase\n"
    "\n"
    "usage: ephorb [options] <pulsar.eph>\n"
    "\n"
    "where options are:\n"
    "\n"
    "  -m MJD   MJD [default: now] \n"
    "  -d hours duration of observation in hours \n"
    "\n"
    "  -s char  site code from $TEMPO/obsys.dat \n"
    "  -f freq  observing frequency in MHz \n"
    "\n"
    "  -p       phase wrt periastron (mean anomaly) \n"
    "  -P       longitude wrt periastron (true anomaly)\n"
    "  -A       longitude wrt ascending node \n"
    "           (true anomaly + longitude of periastron) \n"
    "\n"
    "The longitude of periastron is computed using OM and OMDOT \n"
       << endl;
}

int main (int argc, char ** argv) try
{
  bool phase_periastron = false;
  bool longitude_periastron = false;
  bool longitude_ascending = false;

  MJD mjd;
  double duration = 0.0;

  char site = '7';
  double freq = 1400.0;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hd:f:m:s:pPaA")) != -1)
  {
    switch (gotc)
    {
    case 'h':
      usage ();
      return 0;

    case 'd':
      duration = atof (optarg);
      break;

    case 'f':
      freq = atof (optarg);
      break;

    case 'm':
      mjd = MJD(optarg);
      break;

    case 's':
      site = optarg[0];
      break;

    case 'p':
      phase_periastron = true;
      break;

    case 'P':
      longitude_periastron = true;
      break;

    case 'A':
      longitude_ascending = true;
      break;

    }
  }

  if (optind >= argc)
  {
    cerr << "Please provide tempo parameter file" << endl;
    return -1;
  }

  if (mjd == 0.0)
  {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    fprintf (stderr, "\nUsing current date/time: %s\n", asctime(&date));
    mjd = MJD (date);
  }

  psrephem eph (argv[optind]);

  double epoch = mjd.in_days();

  if (duration)
  {
    unsigned nsteps = 100;
    for (unsigned i=0; i<nsteps; i++)
    {
      double hours = (duration * i) / nsteps;
      double seconds = hours * 3600.0;

      MJD t = mjd + seconds;

      cout << hours << " " << t.datestr("%H:%M:%S") << " "
	   << get_binlng_asc (t.in_days(), eph, freq, site) << endl;
    }
    return 0;
  }

  cout << "================================================" << endl;

  if ( phase_periastron )
    cout << "Binary phase (wrt periastron) = " 
	 << get_binphs_peri (epoch, eph, freq, site)*360.0 << " deg" << endl;
  
  if ( longitude_periastron )
    cout << "Longitude wrt periastron = " 
	 << get_binlng_peri (epoch, eph, freq, site) << " deg" << endl;
  
  if ( longitude_ascending )
    cout << "Longitude wrt ascending node = "
	 << get_binlng_asc (epoch, eph, freq, site) << " deg" << endl;
  
  cout << "================================================" << endl;

  if ( longitude_ascending )
    cout <<
      "\n"
      "Superior conjunction occurs when longitude wrt ascending node = 90 deg"
      "\n"
	 << endl;

  return 0;
}
catch (Error& error)
{
  cerr << "ephorb: error" << error << endl;
  return -1;
}
