/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pulsar.h"

#include <string.h>

#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::load_Receiver (fitsfile* fptr)
{
  // status returned by FITSIO routines
  int status = 0;
  // no comment
  char* comment = 0;

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver" << endl;
  
  Reference::To<Receiver> ext = new Receiver;

  string tempstr;
  string unknown = "unknown";

  // Receiver name

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading receiver" << endl;

  psrfits_read_key (fptr, "FRONTEND", &tempstr, unknown, verbose > 2);
  ext->set_name (tempstr);
  
  // Number of receptors

  int nrcvr = 0;
  psrfits_read_key ( fptr, "NRCVR", &nrcvr, 0, verbose > 2);
  ext->set_nrcvr( nrcvr );
  
  // Receptor polarizations

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading feed polarization" << endl;

  unknown = "";
  psrfits_read_key (fptr, "FD_POLN", &tempstr, unknown, verbose > 2);

  if (tempstr == unknown)
  {
    warning << "FITSArchive::load_Receiver FD_POLN unknown; assuming linear"
	    << endl;
    ext->set_basis (Signal::Linear);
  }
  else
  {
    if (strncasecmp(tempstr.c_str(),"LIN",3) == 0)
      ext->set_basis (Signal::Linear);
    else if (strncasecmp(tempstr.c_str(),"CIRC",4) == 0)
      ext->set_basis (Signal::Circular);
    else
      throw Error (InvalidParam, "FITSArchive::load_Receiver",
	           "unknown FD_POLN='" + tempstr + "'");
  }

  float temp = 0.0;
  Angle angle;

  if (ext->get_basis () == Signal::Linear)  {

    // Read angle of X-probe wrt platform zero

    if (verbose == 3)
      cerr << "FITSArchive::load_Receiver reading XPOL_ANG" << endl;

    float dephalt = 0.0;
    psrfits_read_key (fptr, "XPOL_ANG", &temp, dephalt, verbose > 2);
    angle.setDegrees( temp );
    ext->set_X_offset (angle);

    // Read angle of Y-probe wrt platform zero
  
    if (verbose == 3)
      cerr << "FITSArchive::load_Receiver reading YPOL_ANG" << endl;
  
    dephalt = 90.0;
    psrfits_read_key (fptr, "YPOL_ANG", &temp, dephalt, verbose > 2);
    angle.setDegrees( temp - 90.0 );
    ext->set_Y_offset (angle);
  
    // Read angle of linear noise diode wrt platform zero

    if (verbose == 3)
      cerr << "FITSArchive::load_Receiver reading CAL_ANG" << endl;
  
    dephalt = 45.0;
    psrfits_read_key (fptr, "CAL_ANG", &temp, dephalt, verbose > 2);
    angle.setDegrees( temp - 45.0 );
    ext->set_calibrator_offset (angle);

  }

  // Read angle of X-probe wrt platform zero

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading FD_SANG" << endl;
  
  fits_read_key (fptr, TFLOAT, "FD_SANG", &temp, comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_SANG").warning() << endl;
    status = 0;
  }
  else {
    angle.setDegrees( temp );
    ext->set_field_orientation( angle );
  }

  // Read angle of Y-probe wrt platform zero

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading FD_HAND" << endl;
  
  int hand;

  fits_read_key (fptr, TINT, "FD_HAND", &hand, comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_HAND").warning() << endl;
    status = 0;
  }
  else switch (hand) {
  case 1:
    ext->set_hand ( Signal::Right ); break;
  case -1:
    ext->set_hand ( Signal::Left ); break;
  default:
    throw Error (InvalidParam, "FITSArchive::load_Receiver",
		 "FD_HAND=%d", hand);
  }

  // Read angle of linear noise diode wrt platform zero

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading FD_XYPH" << endl;
  
  fits_read_key (fptr, TFLOAT, "FD_XYPH", &temp, comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_XYPH").warning() << endl;
    status = 0;
  }
  else {
    angle.setDegrees( temp );
    ext->set_reference_source_phase (angle);
  }

  // Feed track mode

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading feed track mode" << endl;

  ext->set_tracking_mode (Receiver::Feed);

  psrfits_read_key (fptr, "FD_MODE", &tempstr, unknown, verbose > 2);
  if (tempstr != unknown)
  {
    if (strncasecmp(tempstr.c_str(),"FA",2) == 0)
      ext->set_tracking_mode (Receiver::Feed);
    else if (strncasecmp(tempstr.c_str(),"CPA",3) == 0)
      ext->set_tracking_mode (Receiver::Celestial);
    else if (strncasecmp(tempstr.c_str(),"GPA",3) == 0)
      ext->set_tracking_mode (Receiver::Galactic);
    else if (verbose == 3)
      cerr << "FITSArchive::load_Receiver unknown FD_MODE=" << tempstr << endl;
  }

  // Read requested feed angle
  
  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading requested feed angle" << endl;
  
  fits_read_key (fptr, TFLOAT, "FA_REQ", &temp, comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FA_REQ").warning() << endl;
    status = 0;
    angle.setDegrees( 0 );
  }
  else
    angle.setDegrees( temp );

  ext->set_tracking_angle (angle);

  // Read attenuator settings

  if (verbose == 3)
    cerr << "FITSArchive::load_Receiver reading attenuator settings" << endl;

  fits_read_key (fptr, TFLOAT, "ATTEN_A", &temp, comment, &status);
  ext->set_atten_a (temp);

  fits_read_key (fptr, TFLOAT, "ATTEN_B", &temp, comment, &status);
  ext->set_atten_b (temp);

  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key ATTEN_A,B").warning() << endl;

    ext->set_atten_a (0);
    ext->set_atten_b (0);

    status = 0;
  }

  if (status == 0) {
    add_extension (ext);
    return;
  }


}

