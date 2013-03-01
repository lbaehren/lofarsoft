/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/Passband.h"
#include "psrfitsio.h"
#include "templates.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const Passband* bandpass)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_passband entered" << endl;
  
  // Move to the BANDPASS HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "BANDPASS", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_movnam_hdu BANDPASS");
  
  psrfits_clean_rows (fptr);

  // Initialise a new row
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_insert_rows BANDPASS");

  char* comment = 0;
  
  // Write NCH_ORIG
  
  int nch_orig = bandpass->get_nchan();
  fits_update_key (fptr, TINT, "NCH_ORIG", &nch_orig, comment, &status);

  int npol = bandpass->get_npol();
  fits_update_key (fptr, TINT, "BP_NPOL", &npol, comment, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_update_key NCH_ORIG BP_NPOL");

  float min = 0;
  float max = 0;
  float max_int = pow(2.0,15.0)-1.0;
  
  float* data_offsets = new float [npol];
  float* data_scales = new float [npol];

  // Calculate the quantities
  
  for (int i = 0; i < npol; i++) {
    minmax(bandpass->get_passband(i), min, max);
    data_offsets[i] = 0.5 * (max + min);
    data_scales[i] = ((max - min) / max_int) / 2.0;
  }
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband scales & offsets calculated" << endl;
  
  // Write the data offsets
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, npol, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, npol,
		  data_offsets, &status);
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband offsets written" << endl;

  // Write the data scale factors
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, npol, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, npol, 
		  data_scales, &status);
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband scales written" << endl;
  
  // Calculate the scaled numbers
  
  int* data = new int [nch_orig*npol];
  int count = 0;
  vector<float> temp;
  
  for (int i = 0; i < npol; i++) {
    temp = bandpass->get_passband(i);
    for (int j = 0; j < nch_orig; j++) {
      data[count] = (int)((temp[j]-data_offsets[i])/data_scales[i]);
      count ++;
    }
  }

  if (verbose == 3)
    cerr << "FITSArchive::unload_passband points calculated" << endl;

  // Write the data itself
  
  long dimension = nch_orig * npol;

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  psrfits_update_tdim (fptr, colnum, nch_orig, npol);
  fits_write_col (fptr, TINT, colnum, 1, 1, dimension, data, &status);

  delete [] data;
  delete [] data_scales;
  delete [] data_offsets;

  if (verbose == 3)
    cerr << "FITSArchive::unload_passband exiting" << endl;
}

