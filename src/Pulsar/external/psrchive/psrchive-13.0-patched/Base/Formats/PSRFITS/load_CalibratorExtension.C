/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "CalibratorExtensionIO.h"

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;

void Pulsar::load (fitsfile* fptr, CalibratorExtension* ext)
{
  int status = 0;
 
  if (Archive::verbose > 2)
    cerr << "Pulsar::load CalibratorExtension" << endl;
  
  char* comment = 0;

  // Get NCHAN
  {
    int nchan = 0;
    fits_read_key (fptr, TINT, "NCHAN", &nchan, comment, &status);
    
    if (status == 0)
    {
      if (Archive::verbose > 2)
	cerr << "Pulsar::load CalibratorExtension NCHAN=" << nchan << endl;
      if (nchan >= 0)
	ext->set_nchan(nchan);
    }
  }

  status = 0;

  // Not a fatal error if extension is empty
  if (ext->get_nchan() == 0)
  {
    if (Archive::verbose > 2) 
      cerr << "Pulsar::load CalibratorExtension HDU"
	   << " contains no data." << endl;
    return;
  }

  // Get EPOCH
  char* epoch = new char[80];
  fits_read_key (fptr, TSTRING, "EPOCH", epoch, comment, &status);

  // Also not a fatal error if not present
  if (status == 0)
  {
    MJD mjd (epoch);
    ext->set_epoch (mjd);
  } 

  delete [] epoch;
  status = 0;

  long dimension = ext->get_nchan();

  vector<float> data (dimension, 0.0);
  
  // Read the centre frequencies
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, ext->get_nchan(),
		 &fits_nullfloat, &(data[0]), &initflag, &status);

  if (status)
    throw FITSError (status, "Pulsar::load CalibratorExtension", 
		     "fits_read_col DAT_FREQ");

  if (Archive::verbose > 2) 
    cerr << "Pulsar::load CalibratorExtension " << ext->get_nchan() 
	 << " frequencies read" << endl;

  unsigned ichan = 0;

  for (ichan=0; ichan < ext->get_nchan(); ichan++)
    ext->set_centre_frequency (ichan, data[ichan]);

  // Read the data scale weights
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, ext->get_nchan(),
		 &fits_nullfloat, &(data[0]), &initflag, &status);

  if (status)
    throw FITSError (status, "Pulsar::load CalibratorExtension", 
		     "fits_read_col DAT_WTS");

  if (Archive::verbose > 2)
    cerr << "Pulsar::load CalibratorExtension " << ext->get_nchan() 
	 << " weights read" << endl;

  for (ichan=0; ichan < ext->get_nchan(); ichan++)
    if ( !finite(data[ichan]) )
      data[ichan] = 0;

  for (ichan=0; ichan < ext->get_nchan(); ichan++)
  {
    ext->set_weight (ichan, data[ichan]);
    if (Archive::verbose > 2 && data[ichan] == 0)
      cerr << "Pulsar::load CalibratorExtension ichan=" << ichan 
	   << " flagged invalid" << endl;
  }

  if (Archive::verbose > 2)
    cerr << "Pulsar::load CalibratorExtension exit" << endl;
}
