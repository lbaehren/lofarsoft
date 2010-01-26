/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fitsio_tempo.h"

using namespace std;

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
    polyco::verbose = true;
    Error::verbose = true;
    verbose = true;
    arg++;
  }

  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0

  //
  // open PSRFITS file and read polyco
  //

  cerr << "Loading polyco from PSRFITS file: " << argv[arg] << endl;

  fits_open_file (&fptr, argv[arg], READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  polyco poly;
  load (fptr, &poly);

  cerr << "Polyco loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed polyco\n" << poly;

  //
  // get PSRFITS file template
  //

  char* psrfits = getenv ("PSRFITS");
  if (!psrfits) {
    cerr << "PSRFITS environment variable not defined.  Cannot continue test."
	 << endl;
    return -1;
  }

  string temp = ".test_polyco_fitsio.dat";
  string templated = temp + "(" + psrfits + ")";
  
  //
  // create PSRFITS file and write polyco
  //

  cerr << "Writing polyco to PSRFITS file: " << temp << endl;

  remove (temp.c_str());
  fits_create_file (&fptr, templated.c_str(), &status);
    if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_create_file " << err << endl;
    return -1;
  }

  unload (fptr, &poly);

  cerr << "Polyco written." << endl;
  fits_close_file (fptr, &status);

  //
  // open newly created PSRFITS file and read polyco
  //

  cerr << "Re-loading polyco from PSRFITS file: " << temp << endl;

  fits_open_file (&fptr, temp.c_str(), READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  polyco poly2;
  load (fptr, &poly2);

  cerr << "Polyco re-loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed polyco\n" << poly;

  FILE* cfptr = fopen (".test_fitsio.in", "w");
  if (!cfptr) {
    perror ("test_fitsio: could not open .test_fitsio.in");
    return -1;
  }
  poly.unload (cfptr);
  fclose (cfptr);

  cfptr = fopen (".test_fitsio.out", "w");
  if (!cfptr) {
    perror ("test_fitsio: could not open .test_fitsio.out");
    return -1;
  }
  poly2.unload (cfptr);
  fclose (cfptr);

  string system_call = "diff .test_fitsio.in .test_fitsio.out";
  cerr << system_call << endl;
 
  system (system_call.c_str());

  return 0;
}
