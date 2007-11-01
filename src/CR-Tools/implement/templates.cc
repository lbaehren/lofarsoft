/*-------------------------------------------------------------------------*
 | $Id:: templates.h 575 2007-08-17 14:04:03Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

// Implementation files

#ifndef HAVE_CASACORE
#include <casa/BasicMath/Functional.cc> 
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <scimath/Functionals/Function.cc>
#include <scimath/Functionals/FunctionParam.cc>
#include <scimath/Functionals/Interpolate1D.cc>
#include <scimath/Mathematics/FFTServer.cc>
#include <scimath/Mathematics/InterpolateArray1D.cc>
#include <images/Images/ImageInterface.cc>
#include <images/Images/PagedImage.cc>
#endif

// #include <casa/Arrays/ArrayIO.tcc>
// #include <casa/Arrays/ArrayLogical.tcc>
// #include <casa/BasicMath/Functional.tcc> 
// #include <casa/Utilities/BinarySearch.tcc>
// #include <casa/Utilities/PtrHolder.tcc>
// #include <scimath/Functionals/Function.tcc>
// #include <scimath/Functionals/FunctionParam.tcc>
// #include <scimath/Functionals/Interpolate1D.tcc>
// #include <scimath/Mathematics/FFTServer.tcc>
// #include <scimath/Mathematics/InterpolateArray1D.tcc>
// #include <images/Images/ImageInterface.tcc>
// #include <images/Images/PagedImage.tcc>
// #include <casa/Utilities/Copy2.cc>

namespace casa {

  // ============================================================================
  //
  //  casa
  //
  // ============================================================================

  // casa/Arrays/Array

  template class Array<Bool>;
  template class Array<Int>;
  template class Array<uInt>;
  template class Array<Short>;
  template class Array<uShort>;
  template class Array<Long>;
  template class Array<uLong>;
  template class Array<Float>;
  template class Array<Double>;
  template class Array<Complex>;
  template class Array<DComplex>;
  template class Array<IPosition>;

  // casa/Arrays/Vector

//   template class Vector<Bool>;
//   template class Vector<Int>;
//   template class Vector<uInt>;
//   template class Vector<Short>;
//   template class Vector<uShort>;
//   template class Vector<Long>;
//   template class Vector<uLong>;
//   template class Vector<Float>;
//   template class Vector<Double>;
//   template class Vector<Complex>;
//   template class Vector<DComplex>;
//   template class Vector<IPosition>;

  template Array<bool> operator< <double>(Array<double> const&, double const&);

  // casa/Arrays/ArrayIO

  template void read_array(Array<Int> &, Char const *);
  template void read_array(Array<Double> &, Char const *);
  template void read_array(Array<Int> &, String const &);
  template void read_array(Array<Double> &, String const &);
  template void readAsciiMatrix(Matrix<Int> &, Char const *);
  template void readAsciiMatrix(Matrix<Double> &, Char const *);
  template void readAsciiVector(Vector<double>&, char const*);
  template void writeAsciiMatrix(Matrix<Int> const &, Char const *);
  template void writeAsciiMatrix(Matrix<Double> const &, Char const *);
  template void writeAsciiVector(Vector<Double> const &, Char const *);

  // casa/Arrays/ArrayMath

  template Array<DComplex> fabs(Array<DComplex> const &);

  // casa/Arrays/ArrayLogical

  template bool allNearAbs (Array<double> const&, double const&, double);
  template Bool allNearAbs (Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs (Array<DComplex> const &, Array<DComplex> const &, Double);

  // ============================================================================
  //
  //  scimath
  //
  // ============================================================================

  // scimath/Mathematics/FFTServer

  template class FFTServer<Float,Complex>;
  template class FFTServer<Double, DComplex>;
 
  // scimath/Mathematics/InterpolateArray1D

  template class InterpolateArray1D<double, double >;
  template class InterpolateArray1D<double, std::complex<double> >;

  // ============================================================================
  //
  //  tables
  //
  // ============================================================================

  

  // ============================================================================
  //
  //  measures
  //
  // ============================================================================



  // ============================================================================
  //
  //  images
  //
  // ============================================================================

  // images/Images/ImageInterface

  template class ImageInterface<Float>;
  template class ImageInterface<Double>;
  template class ImageInterface<Complex>;

  template class PagedImage<Float>;
  template class PagedImage<Double>;
  template class PagedImage<Complex>;

  // ============================================================================
  //
  //  tasking
  //
  // ============================================================================

  
}
