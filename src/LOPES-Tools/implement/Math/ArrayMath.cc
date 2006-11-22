/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: ArrayMath.cc,v 1.4 2006/10/31 18:24:08 bahren Exp $*/

#include <Math/ArrayMath.h>

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/ArrayIter.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/VectorIter.h>
#include <casa/BasicSL/Complex.h>

namespace LOPES { // Namespace LOPES -- begin
  
  // --- Element by element arithmetic of two arrays ----------------------------
  
  template <class T, class U> Array<U> operator+ (const Array<T> &left,
						  const Array<U> &right)
  {
    if (left.shape() != right.shape()) {
      throw(ArrayConformanceError("::operator+(const Array<T> &, const "
				  "Array<T> &)"
				  " - arrays do not conform"));
    }
    
    unsigned int ntotal = left.nelements();
    Array<U> tmp (left.shape());
    
    bool leftDelete (false);
    const T *leftStorage = left.getStorage(leftDelete);
    const T *leftPtr = leftStorage;
    
    bool rightDelete;
    const U *rightStorage = right.getStorage(rightDelete);
    const U *rightPtr = rightStorage;
    
    bool tmpDelete;
    U *tmpStorage = tmp.getStorage(tmpDelete);
    U *tmpPtr = tmpStorage;
    
    while (ntotal--) {
      *tmpPtr++ += (U)*leftPtr++ + *rightPtr++;
    }
    
    return tmp;
  }
  
  template <class T, class U>  Array<U> operator- (const Array<T> &left,
						   const Array<U> &right)
  {
    if (left.shape() != right.shape()) {
      throw(ArrayConformanceError("::operator+(const Array<T> &, const "
				  "Array<T> &)"
				  " - arrays do not conform"));
    }
    
    unsigned int ntotal = left.nelements();
    Array<U> tmp (left.shape());
    
    bool leftDelete (false);
    const T *leftStorage = left.getStorage(leftDelete);
    const T *leftPtr = leftStorage;
    
    bool rightDelete;
    const U *rightStorage = right.getStorage(rightDelete);
    const U *rightPtr = rightStorage;
    
    bool tmpDelete;
    U *tmpStorage = tmp.getStorage(tmpDelete);
    U *tmpPtr = tmpStorage;
    
    while (ntotal--) {
      *tmpPtr++ += (U)*leftPtr++ - *rightPtr++;
    }
    
    return tmp;
  }
  
  template <class T, class U>  Array<U> operator* (const Array<T> &left,
						   const Array<U> &right)
  {
    if (left.shape() != right.shape()) {
      throw(ArrayConformanceError("::operator+(const Array<T> &, const "
				  "Array<T> &)"
				  " - arrays do not conform"));
    }
    
    unsigned int ntotal = left.nelements();
    Array<U> tmp (left.shape());
    
    bool leftDelete (false);
    const T *leftStorage = left.getStorage(leftDelete);
    const T *leftPtr = leftStorage;
    
    bool rightDelete;
    const U *rightStorage = right.getStorage(rightDelete);
    const U *rightPtr = rightStorage;
    
    bool tmpDelete;
    U *tmpStorage = tmp.getStorage(tmpDelete);
    U *tmpPtr = tmpStorage;
    
    while (ntotal--) {
      *tmpPtr++ += (U)*leftPtr++ * *rightPtr++;
    }
    
    return tmp;
  }

// --- Element by element arithmetic between an array and a scalar -------------

template <class T, class U> Array<U> operator+ (const Array<T> &left,
						const U &right)
{
  unsigned int ntotal = left.nelements();
  Array<U> tmp (left.shape());

  tmp = right;
  
  bool leftDelete (false);
  const T *leftStorage = left.getStorage(leftDelete);
  const T *leftPtr = leftStorage;
  
  bool tmpDelete;
  U *tmpStorage = tmp.getStorage(tmpDelete);
  U *tmpPtr = tmpStorage;
  
  while (ntotal--) {
    *tmpPtr++ += (U)*leftPtr++;
  }

  return tmp;
}

template <class T, class U> Array<U> operator- (const Array<T> &left,
						const U &right)
{
  unsigned int ntotal = left.nelements();
  Array<U> tmp (left.shape());

  tmp = -right;
  
  bool leftDelete (false);
  const T *leftStorage = left.getStorage(leftDelete);
  const T *leftPtr = leftStorage;
  
  bool tmpDelete;
  U *tmpStorage = tmp.getStorage(tmpDelete);
  U *tmpPtr = tmpStorage;
  
  while (ntotal--) {
    *tmpPtr++ += (U)*leftPtr++;
  }

  return tmp;
}

template <class T, class U> Array<U> operator* (const Array<T> &left,
						const U &right)
{
  unsigned int ntotal = left.nelements();
  Array<U> tmp (left.shape());

  tmp = right;
  
  bool leftDelete (false);
  const T *leftStorage = left.getStorage(leftDelete);
  const T *leftPtr = leftStorage;
  
  bool tmpDelete;
  U *tmpStorage = tmp.getStorage(tmpDelete);
  U *tmpPtr = tmpStorage;
  
  while (ntotal--) {
    *tmpPtr++ *= (U)*leftPtr++;
  }

  return tmp;
}



// --- Template instantiation --------------------------------------------------

template Array<Complex> operator+ (const Array<Float> &left,
				   const Array<Complex> &right);
template Array<DComplex> operator+ (const Array<Double> &left,
				    const Array<DComplex> &right);

template Array<Complex> operator- (const Array<Float> &left,
				   const Array<Complex> &right);
template Array<DComplex> operator- (const Array<Double> &left,
				    const Array<DComplex> &right);

template Array<Complex> operator* (const Array<Float> &left,
				   const Array<Complex> &right);
template Array<DComplex> operator* (const Array<Double> &left,
				    const Array<DComplex> &right);

template Array<Complex> operator+ (const Array<Float> &left,
				   const Complex &right);
template Array<DComplex> operator+ (const Array<Double> &left,
				   const DComplex &right);

template Array<Complex> operator- (const Array<Float> &left,
				   const Complex &right);
template Array<DComplex> operator- (const Array<Double> &left,
				   const DComplex &right);


template Array<Complex> operator* (const Array<Float> &left,
				   const Complex &right);
template Array<DComplex> operator* (const Array<Double> &left,
				    const DComplex &right);

} // Namespace LOPES -- end
