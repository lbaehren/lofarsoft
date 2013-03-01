/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* A simple program to report the frequency derivative at a given epoch */

#include "Predict.h"
#include "psrephem.h"

#include <unistd.h>

using namespace std;
using Legacy::psrephem;

void usage ();

int main (int argc, char** argv) try
{
  char c;
  char* input_ephemeris = 0;
  char* input_epoch = 0;

  while ((c = getopt(argc, argv, "hp:t:")) != -1) {

    switch (c) {

    case 'h':
      usage ();
      return 0;

    case 'p':
      input_ephemeris = optarg;
      break;

    case 't':
      input_epoch = optarg;
      break;

    default:
      cerr << "fdot: unrecognized parameter '" << c << "'" << endl;
      break;

    }

  }

  if (!input_epoch) {
    cerr << "fdot: please specify the epoch with -t MJD" << endl;
    return -1;
  }

  if (!input_ephemeris) {
    cerr << "fdot: please specify the ephemeris with -p filename" << endl;
    return -1;
  }

  psrephem ephemeris (input_ephemeris);
  MJD epoch (input_epoch);

  Tempo::Predict predict;

  predict.set_parameters (&ephemeris);
  predict.set_ncoef (15);
  predict.set_nspan (120);

  polyco model = predict.get_polyco (epoch, epoch);

  cerr << "acceleration\tperiod\t\tDM" << endl;

  cout.precision(10);

  cout << model.accel(epoch) << "\t" << 1/model.frequency(epoch) << "\t" << ephemeris.get_dm() << endl;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

void usage ()
{
  cout <<
    "fdot - print the first derivative of the pulse frequency \n"
    "USAGE: fdot -p ephemeris -t epoch \n"
       << endl;
}
