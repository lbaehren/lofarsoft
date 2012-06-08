/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/iri2001/iri2rm.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __IRI2RM_H
#define __IRI2RM_H

#ifdef __cplusplus
extern "C" {
#endif

#define iri2rm F77_FUNC(iri2rm,IRI2RM)

  /*
    C-----------------------------------------------------------------
    C
    C INPUT:  Glong         Geographic Longitude East in degrees
    C         Glati         Geographic Latitude North in degrees
    C         year          Year, e.g. 1985
    C         mmdd (-ddd)   Date (or Day of Year as a negative number)
    C         UT            Universal Time in decimal hours
    C         az            Azimuth in degrees
    C         el            Elevation in degrees
  */

  void iri2rm (double* Glong, double* Glati, 
	       float* year, int* mmdd, double* ut, 
	       float* az, float* el, float* RM);

#ifdef __cplusplus
           }
#endif

#endif
