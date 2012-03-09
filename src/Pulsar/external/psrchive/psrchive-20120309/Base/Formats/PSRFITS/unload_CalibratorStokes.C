/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalibratorStokes.h"
#include "psrfitsio.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const CalibratorStokes* stokes)
{
  if (verbose > 2)
    cerr << "FITSArchive::unload CalibratorStokes entered" << endl;
  
  // Move to the CAL_POLN Binary Table and delete any existing rows
  psrfits_move_hdu (fptr, "CAL_POLN");
  psrfits_clean_rows (fptr);

  // Insert a new row
  int status = 0;
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_insert_rows CAL_POLN");

  int nchan = stokes->get_nchan();

  if (verbose > 2) cerr << "FITSArchive::unload CalibratorStokes nchan=" 
		    << nchan << endl;

  // Write NCHAN  
  psrfits_update_key (fptr, "NCHAN", nchan);

  vector<float> data ( nchan );
  for (int i = 0; i < nchan; i++)
    data[i] = stokes->get_valid(i);

  vector<unsigned> dimensions;

  // Write the weights
  psrfits_write_col (fptr, "DAT_WTS", 1, data, dimensions);

  if (verbose > 2) cerr << "FITSArchive::unload CalibratorStokes"
		 " weights written" << endl;
  
  // Write the model parameters

  unsigned npol_QUV = 3;
  data.resize( nchan * npol_QUV );

  unsigned count = 0;
  for (count = 0; count < data.size(); count++)
    data[count] = 0;

  count = 0;
  for (int i = 0; i < nchan; i++)
  {
    if (stokes->get_valid(i))
      for (int ipol = 1; ipol < 4; ipol++)
      {
	data[count] = stokes->get_stokes(i)[ipol].val;
	count++;
      }
    else
      count += npol_QUV;
  }

  assert (count == data.size());

  dimensions.resize (2);
  dimensions[0] = npol_QUV;
  dimensions[1] = nchan;

  psrfits_write_col (fptr, "DATA", 1, data, dimensions);

  // Write the variance of the model parameters
    
  count = 0;
  for (int i = 0; i < nchan; i++)
  {
    if (stokes->get_valid(i))
      for (int ipol = 1; ipol < 4; ipol++)
      {
	data[count] = sqrt( stokes->get_stokes(i)[ipol].var );
	count++;
      }
    else
      count += npol_QUV;
  }

  assert (count == data.size());

  psrfits_write_col (fptr, "DATAERR", 1, data, dimensions);

  if (verbose > 2)
    cerr << "FITSArchive::unload CalibratorStokes exiting" << endl; 
}
