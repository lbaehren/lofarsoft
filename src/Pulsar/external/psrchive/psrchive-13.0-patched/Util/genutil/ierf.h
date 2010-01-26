/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/ierf.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __IERF_H
#define __IERF_H

#define MAX_ITERATIONS 40  /* maximum iterations toward answer */

#ifdef __cplusplus
extern "C" {
#endif

  /* Computes the inverse error function using the Newton-Raphson method */
  double ierf (double erfx);


#ifdef __cplusplus
	   }
#endif

#endif

