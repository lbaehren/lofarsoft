/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar ephemeris, update its parameters to a new epoch
//
// /////////////////////////////////////////////////////////////////////

#include "psrephem.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>

using namespace std;
using Legacy::psrephem;

void usage()
{
  cout << "\n"
    "ephepo: update the epoch in a TEMPO ephemeris\n"
    "\n"
    "  -b      change the binary epoch, T0, to reflect MJD\n"
    "  -m MJD  set PEPOCH to MJD and update period and position accordingly\n"
    "  -v      verbose\n"
       << endl;
}

int main (int argc, char ** argv)
{
  bool binary = false;
  bool verbose = false;
  int gotc = 0;

  MJD mjd;

  while ((gotc = getopt(argc, argv, "bhvm:")) != -1) {
    switch (gotc) {

    case 'b':
      binary = true;
      break;

    case 'm':
      mjd = MJD(optarg);
      break;

    case 'h':
      usage ();
      return 0;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (optind >= argc) {
    cerr << "Please provide pulsar .par/eph file as the last argument" << endl;
    return -1;
  }

  if (mjd == 0.0) {
    cerr << "Please specify the MJD with -m" << endl;
    return -1;
  }

  try {

    psrephem eph (argv[optind]);

    eph.set_epoch (mjd, binary);

    cout << eph << endl;

  }
  catch (Error& error) {
    cerr << "ephepo: error" << error << endl;
    return -1;
  }

  return 0;

}
