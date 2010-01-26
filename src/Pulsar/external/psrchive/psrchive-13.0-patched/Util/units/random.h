//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/random.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __random_H
#define __random_H

#include <complex>
#include <stdlib.h>

template <class T, class U>
void random_value (T& value, U scale)
{
  value = ( double(rand()) - 0.5*double(RAND_MAX) ) * 2.0 * scale / RAND_MAX;
}

template <class T, class U>
void random_value (std::complex<T>& value, U scale)
{
  T real=0, imag=0;

  random_value (real, scale);
  random_value (imag, scale);

  value = std::complex<T> (real, imag);
}

template <class T, class U>
void random_vector (T& array, U scale)
{
  for (unsigned i=0; i<array.size(); i++)
    random_value (array[i], scale);
}

template <class T, class U>
void random_matrix (T& array, U scale)
{
  for (unsigned i=0; i<array.size(); i++)
    random_vector (array[i], scale);
}

#endif
