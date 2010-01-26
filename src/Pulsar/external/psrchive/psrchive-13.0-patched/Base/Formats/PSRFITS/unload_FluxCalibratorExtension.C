/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "CalibratorExtensionIO.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const FluxCalibratorExtension* fce)
{
  int status = 0;

  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::unload FluxCalibratorExtension" << endl;
  
  // Move to the FLUX_CAL Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FLUX_CAL", 0, &status);
  
  if (status != 0)
    throw FITSError (status, 
                     "Pulsar::FITSArchive::unload FluxCalibratorExtension", 
		     "fits_movnam_hdu FLUX_CAL");
  
  psrfits_clean_rows (fptr);

  // Initialise a new row
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, 
                     "Pulsar::FITSArchive::unload FluxCalibratorExtension", 
		     "fits_insert_rows FLUX_CAL");

  // Write the number of receptors (receiver channels)
  psrfits_update_key (fptr, "NRCVR", (int)fce->get_nreceptor());

  // Write the standard lot (NCHAN, EPOCH, DAT_FREQ, DAT_WTS)
  Pulsar::unload (fptr, fce);

  unsigned nchan = fce->get_nchan();
  unsigned nreceptor = fce->get_nreceptor();

  unsigned dimension = nchan * nreceptor;

  vector< Estimate<double> > temp (dimension);

  try {

    unsigned ichan = 0;
    unsigned ireceptor = 0;

    vector<unsigned> dimensions (2);
    dimensions[0] = nchan;
    dimensions[1] = nreceptor;

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_S_sys (ichan, ireceptor);

    unload_Estimates (fptr, temp, "S_SYS", &dimensions);

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_S_cal (ichan, ireceptor);

    unload_Estimates(fptr, temp, "S_CAL", &dimensions);

  }
  catch (Error& error) {
    throw error += "Pulsar::FITSArchive::unload FluxCalibratorExtension";
  }

}
