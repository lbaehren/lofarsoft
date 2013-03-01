/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "fitsutil.h"
#include "Error.h"

#include <fitsio.h>
#include <math.h>

using namespace std;

void fits_version_check (bool verbose)
{
  float version = 0.0;
  fits_get_version (&version);

  if (verbose)
    cerr << "fits_version_check:"
      " library=" << version <<
      " header=" << CFITSIO_VERSION << endl;

  if ( fabs(version - CFITSIO_VERSION) > 1e-4)
    throw Error (InvalidState, "fits_version_check",
		 "CFITSIO version mismatch: library=%f header=%f",
		 version, CFITSIO_VERSION);
}
