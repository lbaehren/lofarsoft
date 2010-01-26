/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fitsio_tempo.h"
#include "FITSError.h"

using namespace std;

void unload_polyco (fitsfile* fptr, const polyco* poly,
		    double pred_phs, bool verbose)
{
  if (verbose)
    cerr << "unload_polyco entered" << endl;

  // Unload the polyco to the FITS file
  unload (fptr, poly);

  if (verbose)
    cerr << "unload_polyco unloaded" << endl;

  // ask for the number of rows in the binary table
  int status = 0;
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "load_polyco", "fits_get_num_rows");
  
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PRED_PHS", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, nrows, firstelem, onelement,
		  &pred_phs, &status);
}
