/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include "psrfitsio.h"

using namespace std;
using namespace Pulsar;

void unload (fitsfile* fptr, const Pulsar::DigitiserStatistics::row& drow,
	     const vector<unsigned>& dimensions)
{
  
  int row = drow.index;

  if (row <= 0)
    throw Error (InvalidParam, "unload (Pulsar::DigitiserStatistics::row*)",
		 "digistat_row invalid row number=%d", row);

  psrfits_write_col( fptr, "ATTEN", row, drow.atten, vector<unsigned>() );
  psrfits_write_col( fptr, "DATA", row, drow.data, dimensions );

}

void 
Pulsar::FITSArchive::unload (fitsfile* fptr, const DigitiserStatistics* dstats)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_digistat entered" << endl;

  // Move to the DIG_STAT HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat", 
		     "fits_movnam_hdu DIG_STAT");
  
  
  char* comment = 0;
  int idata = 0;
  
  // Write DIG_MODE
  char* tempstr = const_cast<char*>(dstats->get_dig_mode().c_str());
  fits_update_key (fptr, TSTRING, "DIG_MODE", tempstr, comment, &status);
  
  // Write NDIGR
  idata = dstats->get_ndigr();
  fits_update_key (fptr, TINT, "NDIGR", &(idata), comment, &status);
  
  // Write NPAR
  idata = dstats->get_npar();  
  fits_update_key (fptr, TINT, "NPAR", &(idata), comment, &status);
  
  // Write NCYCSUB
  idata = dstats->get_ncycsub();
  fits_update_key (fptr, TINT, "NCYCSUB", &(idata), comment, &status);
  
  // Write DIGLEV
  tempstr = const_cast<char*>(dstats->get_diglev().c_str());
  fits_update_key (fptr, TSTRING, "DIGLEV", tempstr, comment, &status);


  psrfits_clean_rows (fptr);

  // Insert some new rows

  fits_insert_rows (fptr, 0, (dstats->rows).size(), &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat",
                     "fits_insert_rows DIG_STAT");

  vector<unsigned> dimensions (3);
  dimensions[0] = dstats->get_npar();
  dimensions[1] = dstats->get_ndigr();
  dimensions[2] = dstats->get_ncycsub();

  try {
    
    for (unsigned i = 0; i < (dstats->rows).size(); i++)
      ::unload( fptr, dstats->rows[i], dimensions );

  }
  catch (Error& error) {
    throw error += "FITSArchive::unload_digistat";
  }

  if (verbose == 3)
    cerr << "FITSArchive::unload_digistat exiting" << endl;
}
