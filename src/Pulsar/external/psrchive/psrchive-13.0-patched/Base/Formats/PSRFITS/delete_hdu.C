/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "FITSError.h"

using namespace std;

/*!
  \param ffptr reference to the PSRFITS file to be modified
  \param hdu_name name of the HDU to remove from ffptr
*/
void Pulsar::FITSArchive::delete_hdu (fitsfile* ffptr, char* hdu_name) const
{
  int status = 0;

  fits_movnam_hdu (ffptr, BINARY_TBL, hdu_name, 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::delete_hdu", 
		     "fits_movnam_hdu %s", hdu_name);

  fits_delete_hdu (ffptr, 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::delete_hdu", 
		     "fits_delete_hdu %s", hdu_name);

  if (verbose == 3)
    cerr << "FITSArchive::delete_hdu " << hdu_name << " HDU deleted" << endl;
}
