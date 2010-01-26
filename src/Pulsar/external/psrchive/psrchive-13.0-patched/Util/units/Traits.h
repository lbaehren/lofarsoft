//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Traits.h,v $
   $Revision: 1.5 $
   $Date: 2007/05/22 08:05:59 $
   $Author: straten $ */

/*
 * For a useful discussion on traits, please see 
 *
 * "Traits: a new and useful template technique" by Nathan C. Meyers
 * C++ Report, June 1995 http://www.cantrip.org/traits.html
 *
 * These traits were implemented in order that data types such as Jones
 * matrices and Quaternions can be passed to algorithms designed to work
 * with scalar values like float and double.
 *
 * Willem van Straten - 26 October 2004
 *
 */

#ifndef __Traits_h
#define __Traits_h

#include <complex>

//! Traits of an element type
template< class E > struct ElementTraits
{
  //! How to cast a complex type to the element type
  template< class T >
  static inline E from_complex (const std::complex<T>& value)
  { return value.real(); }

  //! How to cast an element type to real
  static inline double to_real (const E& element)
  { return element; }

  //! The complex conjugate
  static inline E conjugate (const E& element)
  { return element; }
};

//! Partial specialization for complex elements
template< class E > struct ElementTraits< std::complex<E> >
{
  //! How to cast a complex type to the complex element type
  template< class T >
  static inline std::complex<E> from_complex (const std::complex<T>& value)
  { return value; }

  //! How to cast an element type to real
  static inline double to_real (const std::complex<E>& element)
  { return element.real(); }

  //! The complex conjugate
  template< class T >
  static inline std::complex<E> conjugate (const std::complex<T>& value)
  { return std::conj(value); }
};

//! Traits of the data type
template< class T, class E = ElementTraits<T> > struct DatumTraits
{
  //! Traits of the elements of the data type
  E element_traits;

  static inline unsigned ndim () { return 1; }
  static inline T& element (T& t, unsigned idim) { return t; }
  static inline const T& element (const T& t, unsigned idim) { return t; }
};

//! Quick multiplication of a complex number by i
template<typename T>
std::complex<T> ci (const std::complex<T>& c)
{
  return std::complex<T> (-c.imag(), c.real());
}

//! Quick multiplication of a real number by i
template<typename T>
std::complex<T> ci (const T& real)
{
  return std::complex<T> (0.0, real);
}

//! Return the conjugate of a real number
template<typename T>
T myconj (const T& x) { return ElementTraits<T>::conjugate(x); };

#endif

