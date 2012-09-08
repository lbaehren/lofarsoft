//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Math.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_Complex2Math_H
#define __MEAL_Complex2Math_H

#include "MEAL/Complex2.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<MEAL::Complex2>
operator + (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b);

//! Return a reference to a new SumRule instance representing a-b
Reference::To<MEAL::Complex2>
operator - (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<MEAL::Complex2>
operator * (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<MEAL::Complex2>
operator / (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b);

#endif

