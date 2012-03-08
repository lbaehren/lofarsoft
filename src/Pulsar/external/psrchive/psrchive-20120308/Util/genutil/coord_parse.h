/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/coord_parse.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __COORD_PARSE_H
#define __COORD_PARSE_H

#ifdef __cplusplus
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

  /* defined in angleconv.c */
  char* crad2dmse (double pos, double err, int ra, char * name);

#ifdef __cplusplus
	   }
#endif

#endif
