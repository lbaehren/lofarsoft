//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fitsutil/fitsutil.h,v $
   $Revision: 1.5 $
   $Date: 2009/11/30 11:39:01 $
   $Author: straten $ */

#ifndef __FITS_UTIL_H
#define __FITS_UTIL_H

//! Infinity
extern float fits_nullfloat;

//! Convert FITS datatype to string
const char* fits_datatype_str (int datatype);

//! Checks that library and header version numbers match
void fits_version_check (bool verbose = false);

#endif

