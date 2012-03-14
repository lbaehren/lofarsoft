/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Passband.h"
#include "FITSError.h"

using namespace std;

void Pulsar::FITSArchive::load_Passband (fitsfile* fptr) try
{
  int status = 0;
  char* comment = 0;

  if (verbose == 3)
    cerr << "FITSArchive::load_Passband entered" << endl;

  // Move to the BANDPASS HDU
  fits_movnam_hdu (fptr, BINARY_TBL, "BANDPASS", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_Passband no BANDPASS HDU" << endl;

    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Passband", 
		     "fits_movnam_hdu BANDPASS");

  if (verbose == 3)
    cerr << "FITSArchive::load_Passband BANDPASS HDU found" << endl;

  float nullfloat = 0.0;
  
  // Get the dimensions  
  int nch_orig = 0;
  fits_read_key (fptr, TINT, "NCH_ORIG", &nch_orig, comment, &status);
  
  if (status != 0)
  {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Passband", 
			 "fits_read_key NCH_ORIG").warning() << endl;
    return;
  }

  int npol = 0;
  fits_read_key (fptr, TINT, "BP_NPOL", &npol, comment, &status);
  if (status != 0)
  {
    if(verbose == 3)
    {
      cerr << FITSError (status, "FITSArchive::load_Passband", 
			 "fits_read_key BP_NPOL").warning() << endl;
      cerr << "FITSArchive::load_Passband assuming BP_NPOL = 2" << endl;
    }
    npol = 2;
    status = 0;
  }

  if (verbose == 3)
    cerr << "FITSArchive::load_Passband BANDPASS NCH_ORIG=" << nch_orig
         << " BP_NPOL=" << npol << endl;
  
  if (npol <= 0 || nch_orig <= 0)
  {
    if (verbose == 3)
      cerr << "FITSArchive::load_Passband Extension invalid" << endl; 
    return;
  }
  
  Reference::To<Passband> bandpass = new Passband;
  bandpass->resize (nch_orig, npol);
  
  // Read the data offsets
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  
  float* data_offsets = new float [npol];
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, npol, &nullfloat,
		 data_offsets, &initflag, &status);

  if (status != 0)
  {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Passband", 
			 "fits_read_col DAT_OFFS").warning() << endl;
    return;
  }

  if (verbose == 3)
    cerr << "FITSArchive::load_Passband offsets read" << endl;
  
  // Read the data scale factors
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  
  float* data_scales = new float [npol];
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, npol, &nullfloat, 
		 data_scales, &initflag, &status);

  if (status != 0)
  {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Passband", 
			 "fits_read_col DAT_SCL").warning() << endl;
    return;
  }
  
  if (verbose == 3)
    cerr << "FITSArchive::load_Passband scale factors read" << endl;
  
  // Read the data itself
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  
  long dimension = nch_orig * npol;

  int* data = new int [dimension];
  
  fits_read_col (fptr, TINT, colnum, 1, 1, dimension, &nullfloat, 
		 data, &initflag, &status);

  if (status != 0)
  {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Passband", 
			 "fits_read_col DATA").warning() << endl;
    return;
  }
  
  if (verbose == 3)
    cerr << "FITSArchive::load_Passband data read" << endl;
  
  vector<vector<float> > bandpasses;
  bandpasses.resize(npol);

  int counter = 0;
  
  for (int i = 0; i < npol; i++)
  {
    bandpasses[i].resize(nch_orig);
    for (int j = 0; j < nch_orig; j++)
    {
      bandpasses[i][j] = data[counter]*data_scales[i] + data_offsets[i];
      counter++;
    }
    bandpass->set_passband(bandpasses[i], i);
  }

  delete [] data;
  delete [] data_offsets;
  delete [] data_scales;

  if (status == 0)
    add_extension (bandpass);

  if (verbose == 3)
    cerr << "FITSArchive::load_Passband exiting" << endl;
}
 catch (Error& error)
   {
     throw error += "FITSArchive::load_Passband";
   }
