/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalibratorStokes.h"

#include "psrfitsio.h"
#include "FITSError.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

void Pulsar::FITSArchive::load_CalibratorStokes (fitsfile* fptr) try
{
  int status = 0;
 
  if (verbose > 2)
    cerr << "FITSArchive::load_CalibratorStokes entered" << endl;
  
  // Move to the CAL_POLN HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "CAL_POLN", 0, &status);
  
  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2) cerr << "Pulsar::FITSArchive::load_CalibratorStokes"
		   " no CAL_POLN HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_CalibratorStokes", 
		     "fits_movnam_hdu CAL_POLN");

  Reference::To<CalibratorStokes> stokes = new CalibratorStokes;

  // Get NCHAN
  int nchan = 0;
  psrfits_read_key (fptr, "NCHAN", &nchan, 0, verbose > 2);
  
  if (!nchan)
    // Try the old NCH_POLN
    psrfits_read_key (fptr, "NCH_POLN", &nchan, 0, verbose > 2);

  if (!nchan)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CalibratorStokes CAL_POLN HDU"
	   << " contains no data. CalibratorStokes not loaded" << endl;
    return;
  }

  stokes->set_nchan( nchan );
  
  vector<float> data( nchan );

  // Read the data weights
  psrfits_read_col (fptr, "DAT_WTS", data);

  if (verbose > 2)
    cerr << "FITSArchive::load_CalibratorStokes weights read" << endl;
  
  for (int ichan=0; ichan < nchan; ichan++)
  {
    float weight = data[ichan];
    stokes->set_valid (ichan, finite(weight) && weight != 0);
  }

  // Read the data itself

  unsigned npol_QUV = 3;
  data.resize( nchan * npol_QUV );

  // Read the data
  psrfits_read_col (fptr, "DATA", data);

  if (verbose > 2)
    cerr << "FITSArchive::load_CalibratorStokes data read" << endl;
  
  unsigned count = 0;
  for (int ichan = 0; ichan < nchan; ichan++)
  {
    if (!stokes->get_valid(ichan))
    {
      count += npol_QUV;
      continue;
    }

    Stokes< Estimate<float> > s;

    for (unsigned ipol = 1; ipol < 4; ipol++)
    {
      s[ipol] = data[count];
      count++;
    }

    stokes->set_stokes (ichan, s);
  }

  assert (count == data.size());

  // Read the data error estimates
  psrfits_read_col (fptr, "DATAERR", data);

  if (verbose > 2)
    cerr << "FITSArchive::load_CalibratorStokes data error read" << endl;
  
  count = 0;
  for (int ichan = 0; ichan < nchan; ichan++)
  {
    if (!stokes->get_valid(ichan))
    {
      count += npol_QUV;
      continue;
    }

    Stokes< Estimate<float> > s = stokes->get_stokes (ichan);

    for (unsigned ipol = 1; ipol < 4; ipol++)
    {
      float err = data[count];
      s[ipol].var = err * err;
      count++;
    }

    stokes->set_stokes (ichan, s);
  }

  assert (count == data.size());

  add_extension (stokes);
  
  if (verbose > 2)
    cerr << "FITSArchive::load_CalibratorStokes exiting" << endl;
}
 catch (Error& error)
   {
     throw error += "FITSArchive::load_CalibratorStokes";
   }
