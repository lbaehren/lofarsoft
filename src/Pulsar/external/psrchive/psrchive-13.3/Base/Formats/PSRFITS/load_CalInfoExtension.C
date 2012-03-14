/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalInfoExtension.h"
#include "FITSError.h"

using namespace std;

void Pulsar::FITSArchive::load_CalInfoExtension (fitsfile* fptr)
{
  // status returned by FITSIO routines
  int status = 0;

  // do not return comments in fits_read_key
  char* comment = 0;
  
  // Read detailed CAL information

  if (verbose == 3)
    cerr << "FITSArchive::load_CalInfoExtension" << endl;
  
  CalInfoExtension* ext = new CalInfoExtension;

  fits_read_key (fptr, TDOUBLE, "CAL_FREQ", &(ext->cal_frequency),
		 comment, &status);
  fits_read_key (fptr, TDOUBLE, "CAL_DCYC", &(ext->cal_dutycycle),
		 comment, &status);
  fits_read_key (fptr, TDOUBLE, "CAL_PHS", &(ext->cal_phase),
		 comment, &status);
    
  if (status == 0)
    add_extension (ext);
 
  else {

    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_CalInfoExtension",
			 "fits_read_key CAL_FREQ,DCYC,PHS").warning() << endl;

    return;

  }

  // Read the CAL mode
  status = 0;

  char* tempstr = new char [FLEN_VALUE];
  fits_read_key (fptr, TSTRING, "CAL_MODE", tempstr, comment, &status);

  if (status == 0)
    ext->cal_mode = tempstr;

  delete tempstr;

  if (status != 0 && verbose == 3)
    cerr << FITSError (status, "FITSArchive::load_CalInfoExtension",
		       "fits_read_key CAL_MODE").warning() << endl;
}

