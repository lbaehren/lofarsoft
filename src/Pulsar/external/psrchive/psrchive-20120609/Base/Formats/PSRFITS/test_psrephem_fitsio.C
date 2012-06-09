/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

#include "fitsio_tempo.h"
#include "ephio.h"
#include "Error.h"

using namespace std;
using Legacy::psrephem;

int main (int argc, char** argv)
{
  bool verbose = false;
  int arg=1;
  string first;
  if ( argc > 1 )
    first = argv[arg];

  if ( argc < 2 || first == "-h" ) {
    cerr << "USAGE: test_fitsio inputfilename" << endl;
    return 0;
  }
  if ( first == "-v" ) {
    psrephem::verbose = true;
    Error::verbose = true;
    verbose = true;
    arg++;
  }

  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0

  //
  // open PSRFITS file and read ephemeris
  //

  cerr << "Loading ephemeris from PSRFITS file: " << argv[arg] << endl;

  fits_open_file (&fptr, argv[arg], READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  psrephem eph;
  load (fptr, &eph);

  cerr << "Ephemeris loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed ephemeris\n" << eph;

  //
  // get PSRFITS file template
  //

  char* psrfits = getenv ("PSRFITS");
  if (!psrfits) {
    cerr << "PSRFITS environment variable not defined.  Cannot continue test."
	 << endl;
    return -1;
  }

  string temp = ".test_psrephem_fitsio.dat";
  string templated = temp + "(" + psrfits + ")";
  
  //
  // create PSRFITS file and write ephemeris
  //

  cerr << "Writing ephemeris to PSRFITS file: " << temp << endl;

  remove (temp.c_str());
  fits_create_file (&fptr, templated.c_str(), &status);
    if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_create_file " << err << endl;
    return -1;
  }

  unload (fptr, &eph);

  cerr << "Ephemeris written." << endl;
  fits_close_file (fptr, &status);

  //
  // open newly created PSRFITS file and read ephemeris
  //

  cerr << "Re-loading ephemeris from PSRFITS file: " << temp << endl;

  fits_open_file (&fptr, temp.c_str(), READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  psrephem eph2;
  load (fptr, &eph2);

  cerr << "Ephemeris re-loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed ephemeris\n" << eph;

  FILE* cfptr = fopen (".test_fitsio.in", "w");
  if (!cfptr) {
    perror ("test_fitsio: could not open .test_fitsio.in");
    return -1;
  }
  eph.unload (cfptr);
  fclose (cfptr);

  cfptr = fopen (".test_fitsio.out", "w");
  if (!cfptr) {
    perror ("test_fitsio: could not open .test_fitsio.out");
    return -1;
  }
  eph2.unload (cfptr);
  fclose (cfptr);

  string system_call = "diff .test_fitsio.in .test_fitsio.out";
  cerr << system_call << endl;

  system (system_call.c_str());

  return 0;
}
