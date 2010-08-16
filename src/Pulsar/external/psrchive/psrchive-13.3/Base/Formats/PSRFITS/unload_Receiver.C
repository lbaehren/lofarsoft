/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "FITSError.h"

#include <memory>
#include <string.h>

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const Receiver* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // no comment
  char* comment = 0;

  // temporary string
  char* tempstr = new char[FLEN_VALUE];

  strncpy (tempstr, ext->get_name().c_str(), FLEN_VALUE);
  fits_update_key (fptr, TSTRING, "FRONTEND", tempstr, comment, &status);
    
  if (ext->get_basis() == Signal::Linear)
    strcpy (tempstr, "LIN");
  
  else if (ext->get_basis() == Signal::Circular)
    strcpy (tempstr, "CIRC");
  
  else
    strcpy (tempstr, "    ");
  
  fits_update_key (fptr, TSTRING, "FD_POLN", tempstr, comment, &status);
  
  float temp;

  temp = ext->get_field_orientation().getDegrees();
  fits_update_key (fptr, TFLOAT, "FD_SANG", &temp, comment, &status);

  int hand = (int) ext->get_hand();
  fits_update_key (fptr, TINT, "FD_HAND", &hand, comment, &status);

  temp = ext->get_reference_source_phase().getDegrees();
  fits_update_key (fptr, TFLOAT, "FD_XYPH", &temp, comment, &status);

  switch (ext->get_tracking_mode()) {
  case Receiver::Feed:
    strcpy (tempstr, "FA"); break;
  case Receiver::Celestial:
    strcpy (tempstr, "CPA"); break;
  case Receiver::Galactic:
    strcpy (tempstr, "GPA"); break;
  default:
    strcpy (tempstr, "   "); break;
  }

  fits_update_key (fptr, TSTRING, "FD_MODE", tempstr, comment, &status);
  
  temp = ext->get_tracking_angle().getDegrees();
  fits_update_key (fptr, TFLOAT, "FA_REQ", &temp, comment, &status);

  temp = ext->get_atten_a();
  fits_update_key (fptr, TFLOAT, "ATTEN_A", &temp, comment, &status);

  temp = ext->get_atten_b();
  fits_update_key (fptr, TFLOAT, "ATTEN_B", &temp, comment, &status);

  delete [] tempstr;

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload Receiver");
}
