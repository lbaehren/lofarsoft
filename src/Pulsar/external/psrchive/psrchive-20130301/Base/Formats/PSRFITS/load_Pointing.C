/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"
#include "FITSError.h"

using namespace std;

Angle angle_units (fitsfile* fptr, int colnum, double angle, const char* name)
{
  char keyword [FLEN_KEYWORD+1];
  char value   [FLEN_VALUE+1];
 
  char* comment = 0;
  int status = 0;

  fits_make_keyn ("TUNIT", colnum, keyword, &status);
  fits_read_key (fptr, TSTRING, keyword, value, comment, &status);

  Angle retval;

  if (status == 0 && string(value) == "deg") {
    if (name)
      cerr << "Pulsar::FITSArchive::load_Pointing " << name << " in degrees"
	   << endl;
    retval.setDegrees (angle);
  }
  else {
    if (name)
      cerr << "Pulsar::FITSArchive::load_Pointing " << name << " in turns"
	   << endl;
    retval.setTurns (angle);  
  }

  return retval;
}

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::load_Pointing (fitsfile* fptr, int row,
				         Pulsar::Integration* integ)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_Pointing" << endl;
  
  Reference::To<Pointing> ext = new Pointing;

  // status returned by FITSIO routines
  int status = 0;

  int initflag = 0;
  int colnum = 0;
  float nullfloat = 0.0;
  double nulldouble = 0.0;

  fits_get_colnum (fptr, CASEINSEN, "LST_SUB", &colnum, &status);

  double lst_in_seconds;

  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &lst_in_seconds, &initflag, &status);

  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no LST_SUB" << endl;
    lst_in_seconds = 0;
    status = 0;
  }

  ext->set_local_sidereal_time (lst_in_seconds);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  double double_angle = 0.0;
  Angle angle;

  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no RA_SUB" << endl;
    double_angle = 0;
    status = 0;
  }

  double RA_angle = double_angle;
  int RA_colnum = colnum;

  initflag = 0;
  colnum = 0;

  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no DEC_SUB" << endl;
    double_angle = 0;
    status = 0;
  }
  
  double DEC_angle = double_angle;
  int DEC_colnum = colnum;

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no GLON_SUB" << endl;
    double_angle = 0;
    status = 0;
  }
  
  angle.setDegrees (double_angle);
  ext->set_galactic_longitude (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no GLAT_SUB" << endl;
    double_angle = 0;
    status = 0;
  }
  
  angle.setDegrees (double_angle);
  ext->set_galactic_latitude (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no FD_ANG" << endl;
    float_angle = 0;
    status = 0;
  }
  
  angle.setDegrees (float_angle);
  ext->set_feed_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status || initflag)
  {
    /* 
      Rationale for aborting: POS_ANG is currently the only attribute
      of the Pointing class that is actually used by anything.

      Furthermore, the existence of a Pointing extension will cause
      the value of FA_REQ to be overridden by POS_ANG, which is not
      desireable if FA_REQ is set to something sensible.
    */

    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no POS_ANG (aborting)" 
           << endl;

    return;
  }
  
  angle.setDegrees (float_angle);
  ext->set_position_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no PAR_ANG" << endl;
    float_angle = 0;
    status = 0;
  }
  
  angle.setDegrees (float_angle);
  ext->set_parallactic_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no TEL_AZ" << endl;
    float_angle = 0;
    status = 0;
  }
  
  angle.setDegrees (float_angle);
  ext->set_telescope_azimuth (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing WARNING no TEL_ZEN" << endl;
    float_angle = 0;
    status = 0;
  }

  angle.setDegrees (float_angle);
  ext->set_telescope_zenith (angle);

  ext->set_right_ascension( angle_units (fptr, RA_colnum, RA_angle,
					 (verbose > 2) ? "RA_SUB" : 0) );

  ext->set_declination( angle_units (fptr, DEC_colnum, DEC_angle,
				     (verbose > 2) ? "DEC_SUB" : 0) );

  integ->add_extension (ext);

}


