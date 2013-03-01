/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/ITRFExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const ITRFExtension* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // no comment
  char* comment = 0;

  fits_update_key (fptr, TDOUBLE, "ANT_X",
		   const_cast<double*>(&(ext->ant_x)), comment, &status);
  fits_update_key (fptr, TDOUBLE, "ANT_Y",
		   const_cast<double*>(&(ext->ant_y)), comment, &status);
  fits_update_key (fptr, TDOUBLE, "ANT_Z",
		   const_cast<double*>(&(ext->ant_z)), comment, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload ITRFExtension");

}
