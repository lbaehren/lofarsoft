/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalInfoExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const CalInfoExtension* ext)
{
  // status returned by FITSIO routines
  int status = 0;
  // do not return comments in fits_read_key
  char* comment = 0;

  float tempfloat = ext->cal_frequency;
  fits_update_key (fptr, TFLOAT, "CAL_FREQ", &tempfloat, comment, &status);

  tempfloat = ext->cal_dutycycle;
  fits_update_key (fptr, TFLOAT, "CAL_DCYC", &tempfloat, comment, &status);

  tempfloat = ext->cal_phase;
  fits_update_key (fptr, TFLOAT, "CAL_PHS", &tempfloat, comment, &status);

  char* tempstr = const_cast<char*>(ext->cal_mode.c_str());
  fits_update_key (fptr, TSTRING, "CAL_MODE", tempstr, comment, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload CalInfoExtension");
}

