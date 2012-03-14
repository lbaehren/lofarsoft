/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/orbital.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __ORBITAL_H
#define __ORBITAL_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Solves Kepler's equation using the Newton-Raphson method */
  double eccentric_anomaly (double mean_anomaly, double eccentricity);

  /* Solves mass function for the companion mass */
  double companion_mass (double mass_function, double sini, double m1);

#ifdef __cplusplus
	   }
#endif

#endif

