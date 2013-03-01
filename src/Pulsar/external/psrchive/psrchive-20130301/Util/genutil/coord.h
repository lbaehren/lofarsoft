/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/coord.h,v $
   $Revision: 1.12 $
   $Date: 2008/05/24 14:13:51 $
   $Author: straten $ */

#ifndef __COORD_H
#define __COORD_H

#ifdef __cplusplus

#include "MJD.h"
 
/* ********************************************************************
   bary_earth -
        given an MJD, latitude and longitude in degrees, and elevation
        in metres, returns the barycentric velocity (fraction of c)
        and position (fraction of AU) of telescope.
   ******************************************************************** */
int bary_earth (const MJD& mjd_date, double longitude, double latitude,
                double elevation, double* baryvel, double* barypos);
 
/* ********************************************************************
   doppler_factor -
        given an MJD, then latitude, longitude and elevation (degrees
        and metres), and the ra and dec of an astronomical source
        (considered at rest), returns the doppler shift due to the
        telescope's barycentric velocity
   ******************************************************************** */
int doppler_factor (const MJD& date, double ra, double dec,
                    double longitude, double latitude, double elevation,
                    double* dfactor);
 
extern "C" {

#endif

/* ********************************************************************
   str2coord -
	converts a string with RA and DEC in their human notations:
	RA in time-measure (1h = 15 deg)
	DEC in sexagesimal angular measure
	into an ra and dec in radians
   ******************************************************************** */
  int str2coord (double *ra, double *dec, const char* coordstr);
  int str2ra  (double *ra,  const char* rastr);
  int str2dec2 (double *dec, const char* decstr);
  
  int coord2str (char* coordstring, unsigned strlen,
		 double ra, double dec, unsigned places);
  int ra2str (char* rastr, unsigned rastrlen, double ra, unsigned places);
  int dec2str2 (char* decstr, unsigned decstrlen, double dec, unsigned places);
  

  int coord2name (char* pulsar, double ra, double dec);

#ifdef __cplusplus
	   }
#endif

#endif
