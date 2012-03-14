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

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::unload (fitsfile* fptr, const Pointing* ext, int row)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_Pointing entered" << endl;
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "LST_SUB", &colnum, &status);
  
  double tempdouble = ext->get_local_sidereal_time ();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col LST_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  tempdouble = ext->get_right_ascension().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col RA_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  tempdouble = ext->get_declination().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col DEC_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  tempdouble = ext->get_galactic_longitude().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLON_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  tempdouble = ext->get_galactic_latitude().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLAT_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float tempfloat = ext->get_feed_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col FD_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  tempfloat = ext->get_position_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col POS_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  tempfloat = ext->get_parallactic_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col PAR_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  tempfloat = ext->get_telescope_azimuth().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_AZ");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  tempfloat = ext->get_telescope_zenith().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_ZEN");

  if (verbose == 3)
    cerr << "FITSArchive::unload_Pointing exiting" << endl;
}
