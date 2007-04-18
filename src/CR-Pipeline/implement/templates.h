/***************************************************************************
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

/*!
  \brief Template instantiation for CASA objects

  \author Lars B&auml;hren

  \date 2007/02/19
*/

// Header files

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta/MeasValue.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Utilities/Copy.h>
#include <images/Images/ImageConcat.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/MomentCalculator.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <tables/Tables/ExprNodeArray.h>
#include <tables/Tables/ExprRange.h>
#include <tables/Tables/TableExprData.h>

// Implementation files

#include <casa/Arrays/ArrayLogical.cc>
#include <casa/BasicMath/Functional.cc> 
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/Copy2.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <scimath/Functionals/Function.cc>
#include <scimath/Functionals/FunctionParam.cc>
#include <images/Images/PagedImage.cc>

namespace casa {

  // ============================================================================
  //  casa/implement

  Complex pow (Complex const&, Float const&);
  DComplex pow (DComplex const&, Double const&);
  
  // casa/Arrays/ArrayLogical

  template bool allNearAbs (Array<double> const&, double const&, double);
  template Bool allNearAbs (Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs (Array<DComplex> const &, Array<DComplex> const &, Double);

  // casa/Arrays/Vector

/*   template class Array<Bool>; */
/*   template class Array<Int>; */
/*   template class Array<uInt>; */
/*   template class Array<Short>; */
/*   template class Array<uShort>; */
/*   template class Array<Long>; */
/*   template class Array<uLong>; */
/*   template class Array<Float>; */
/*   template class Array<Double>; */
/*   template class Array<Complex>; */
/*   template class Array<DComplex>; */

/*   template class Vector<Bool>; */
/*   template class Vector<Int>; */
/*   template class Vector<uInt>; */
/*   template class Vector<Short>; */
/*   template class Vector<uShort>; */
/*   template class Vector<Long>; */
/*   template class Vector<uLong>; */
/*   template class Vector<Float>; */
/*   template class Vector<Double>; */
/*   template class Vector<Complex>; */
/*   template class Vector<DComplex>; */

  // casa/BasicMath/Functional
  
  template class Functional<int, float>;
  template class Functional<int, double>;
  template class Functional<Int, Complex>;
  template class Functional<float, double>;
  template class Functional<float, Complex>;
  template class Functional<float, Array<float> >;
  template class Functional<double, std::complex<double> >;
  template class Functional<Vector<float>, Array<float> >;
  template class Functional<Vector<double>, std::complex<double> >;

  // casa/Utilities/BinarySearch

  template int binarySearchBrackets<Vector<double>, double>(bool&, Vector<double> const&, double const&, unsigned int, int);
  
  // casa/Utilities/Copy

  // Bool
  template void objcopy (Bool*, Bool const*, uInt);
  template void objcopy (Bool*, Bool const*, uInt, uInt, uInt);
  template void objset  (Bool*, Bool, uInt);
  template void objset  (Bool*, Bool, uInt, uInt);
  template void objmove (Bool*, Bool const*, uInt);
  // int
  template void objcopy (int*, int const*, uInt);
  template void objcopy (int*, int const*, uInt, uInt, uInt);
  template void objset  (int*, int, uInt);
  template void objset  (int*, int, uInt, uInt);
  template void objmove (int*, int const*, uInt);
  // uInt
  template void objcopy (uInt*, uInt const*, uInt);
  template void objcopy (uInt*, uInt const*, uInt, uInt, uInt);
  template void objset  (uInt*, uInt, uInt);
  template void objset  (uInt*, uInt, uInt, uInt);
  template void objmove (uInt*, uInt const*, uInt);
  // Short
  template void objcopy (Short*, Short const*, uInt);
  template void objcopy (Short*, Short const*, uInt, uInt, uInt);
  template void objset  (Short*, Short, uInt);
  template void objset  (Short*, Short, uInt, uInt);
  template void objmove (Short*, Short const*, uInt);
  // uShort
  template void objcopy (uShort*, uShort const*, uInt);
  template void objcopy (uShort*, uShort const*, uInt, uInt, uInt);
  template void objset  (uShort*, uShort, uInt);
  template void objset  (uShort*, uShort, uInt, uInt);
  template void objmove (uShort*, uShort const*, uInt);
  // Long
  template void objcopy (Long*, Long const*, uInt);
  template void objcopy (Long*, Long const*, uInt, uInt, uInt);
  template void objset  (Long*, Long, uInt);
  template void objset  (Long*, Long, uInt, uInt);
  template void objmove (Long*, Long const*, uInt);
  // uLong
  template void objcopy (uLong*, uLong const*, uInt);
  template void objcopy (uLong*, uLong const*, uInt, uInt, uInt);
  template void objset  (uLong*, uLong, uInt);
  template void objset  (uLong*, uLong, uInt, uInt);
  template void objmove (uLong*, uLong const*, uInt);
  // long long
  template void objcopy (long long*, long long const*, uInt);
  template void objcopy (long long*, long long const*, uInt, uInt, uInt);
  template void objset  (long long*, long long, uInt);
  template void objset  (long long*, long long, uInt, uInt);
  template void objmove (long long*, long long const*, uInt);
  // Float
  template void objcopy (Float*, Float const*, uInt);
  template void objcopy (Float*, Float const*, uInt, uInt, uInt);
  template void objset  (Float*, Float, uInt);
  template void objset  (Float*, Float, uInt, uInt);
  template void objmove (Float*, Float const*, uInt);
  // Double
  template void objcopy (Double*, Double const*, uInt);
  template void objcopy (Double*, Double const*, uInt, uInt, uInt);
  template void objset  (Double*, Double, uInt);
  template void objset  (Double*, Double, uInt, uInt);
  template void objmove (Double*, Double const*, uInt);
  // Complex
  template void objcopy (Complex*, Complex const*, uInt);
  template void objcopy (Complex*, Complex const*, uInt, uInt, uInt);
  template void objset  (Complex*, Complex, uInt);
  template void objset  (Complex*, Complex, uInt, uInt);
  template void objmove (Complex*, Complex const*, uInt);
  // DComplex
  template void objcopy (DComplex*, DComplex const*, uInt);
  template void objcopy (DComplex*, DComplex const*, uInt, uInt, uInt);
  template void objset  (DComplex*, DComplex, uInt);
  template void objset  (DComplex*, DComplex, uInt, uInt);
  template void objmove (DComplex*, DComplex const*, uInt);
  // String
  template void objcopy (String*, String const*, uInt);
  template void objcopy (String*, String const*, uInt, uInt, uInt);
  template void objset  (String*, String, uInt);
  template void objset  (String*, String, uInt, uInt);
  template void objmove (String*, String const*, uInt);
  // Char
  template void objcopy (Char*, Char const*, uInt);
  template void objcopy (Char*, Char const*, uInt, uInt, uInt);
  template void objset  (Char*, Char, uInt);
  template void objset  (Char*, Char, uInt, uInt);
  template void objmove (Char*, Char const*, uInt);
  // uChar
  template void objcopy (uChar*, uChar const*, uInt);
  template void objcopy (uChar*, uChar const*, uInt, uInt, uInt);
  template void objset  (uChar*, uChar, uInt);
  template void objset  (uChar*, uChar, uInt, uInt);
  template void objmove (uChar*, uChar const*, uInt);

  // AutoDiff<Float>
  template void objcopy (AutoDiff<Float>*, AutoDiff<Float> const*, uInt);
  template void objcopy (AutoDiff<Float>*, AutoDiff<Float> const*, uInt, uInt, uInt);
  template void objset  (AutoDiff<Float>*, AutoDiff<Float>, uInt);
  template void objset  (AutoDiff<Float>*, AutoDiff<Float>, uInt, uInt);
  template void objmove (AutoDiff<Float>*, AutoDiff<Float> const*, uInt);
  // AutoDiff<Double>
  template void objcopy (AutoDiff<Double>*, AutoDiff<Double> const*, uInt);
  template void objcopy (AutoDiff<Double>*, AutoDiff<Double> const*, uInt, uInt, uInt);
  template void objset  (AutoDiff<Double>*, AutoDiff<Double>, uInt);
  template void objset  (AutoDiff<Double>*, AutoDiff<Double>, uInt, uInt);
  template void objmove (AutoDiff<Double>*, AutoDiff<Double> const*, uInt);
  // IPosition
  template void objcopy (IPosition*, IPosition const*, uInt);
  template void objcopy (IPosition*, IPosition const*, uInt, uInt, uInt);
  template void objset  (IPosition*, IPosition, uInt);
  template void objset  (IPosition*, IPosition, uInt, uInt);
  template void objmove (IPosition*, IPosition const*, uInt);
  // LatticeExprNode
  template void objcopy (LatticeExprNode*, LatticeExprNode const*, uInt);
  template void objcopy (LatticeExprNode*, LatticeExprNode const*, uInt, uInt, uInt);
  template void objset  (LatticeExprNode*, LatticeExprNode, uInt);
  template void objset  (LatticeExprNode*, LatticeExprNode, uInt, uInt);
  template void objmove (LatticeExprNode*, LatticeExprNode const*, uInt);
  // LoggerHolder
  template void objcopy (LoggerHolder*, LoggerHolder const*, uInt);
  template void objcopy (LoggerHolder*, LoggerHolder const*, uInt, uInt, uInt);
  template void objset  (LoggerHolder*, LoggerHolder, uInt);
  template void objset  (LoggerHolder*, LoggerHolder, uInt, uInt);
  template void objmove (LoggerHolder*, LoggerHolder const*, uInt);
  // MeasValue
/*   template void objcopy (MeasValue*, MeasValue const*, uInt); */
/*   template void objcopy (MeasValue*, MeasValue const*, uInt, uInt, uInt); */
/*   template void objset  (MeasValue*, MeasValue, uInt); */
/*   template void objset  (MeasValue*, MeasValue, uInt, uInt); */
/*   template void objmove (MeasValue*, MeasValue const*, uInt); */
  // MDirection
  template void objcopy (MDirection*, MDirection const*, uInt);
  template void objcopy (MDirection*, MDirection const*, uInt, uInt, uInt);
  template void objset  (MDirection*, MDirection, uInt);
  template void objset  (MDirection*, MDirection, uInt, uInt);
  template void objmove (MDirection*, MDirection const*, uInt);
  // MFrequency
  template void objcopy (MFrequency*, MFrequency const*, uInt);
  template void objcopy (MFrequency*, MFrequency const*, uInt, uInt, uInt);
  template void objset  (MFrequency*, MFrequency, uInt);
  template void objset  (MFrequency*, MFrequency, uInt, uInt);
  template void objmove (MFrequency*, MFrequency const*, uInt);
  // MPosition
  template void objcopy (MPosition*, MPosition const*, uInt);
  template void objcopy (MPosition*, MPosition const*, uInt, uInt, uInt);
  template void objset  (MPosition*, MPosition, uInt);
  template void objset  (MPosition*, MPosition, uInt, uInt);
  template void objmove (MPosition*, MPosition const*, uInt);
  // MVTime
  template void objcopy (MVTime*, MVTime const*, uInt);
  template void objcopy (MVTime*, MVTime const*, uInt, uInt, uInt);
  template void objset  (MVTime*, MVTime, uInt);
  template void objset  (MVTime*, MVTime, uInt, uInt);
  template void objmove (MVTime*, MVTime const*, uInt);
  // Quantum<Double>
  template void objcopy (Quantum<Double>*, Quantum<Double> const*, uInt);
  template void objcopy (Quantum<Double>*, Quantum<Double> const*, uInt, uInt, uInt);
  template void objset  (Quantum<Double>*, Quantum<Double>, uInt);
  template void objset  (Quantum<Double>*, Quantum<Double>, uInt, uInt);
  template void objmove (Quantum<Double>*, Quantum<Double> const*, uInt);
  // TableExprNode
/*   template void objcopy (TableExprNode*, TableExprNode const*, uInt); */
/*   template void objcopy (TableExprNode*, TableExprNode const*, uInt, uInt, uInt); */
/*   template void objset  (TableExprNode*, TableExprNode, uInt); */
/*   template void objset  (TableExprNode*, TableExprNode, uInt, uInt); */
/*   template void objmove (TableExprNode*, TableExprNode const*, uInt); */
  // TableRecord
  template void objcopy (TableRecord*, TableRecord const*, uInt);
  template void objcopy (TableRecord*, TableRecord const*, uInt, uInt, uInt);
  template void objset  (TableRecord*, TableRecord, uInt);
  template void objset  (TableRecord*, TableRecord, uInt, uInt);
  template void objmove (TableRecord*, TableRecord const*, uInt);
  // Vector<String>
  template void objcopy (Vector<String>*, Vector<String> const*, uInt);
  template void objcopy (Vector<String>*, Vector<String> const*, uInt, uInt, uInt);
  template void objset  (Vector<String>*, Vector<String>, uInt);
  template void objset  (Vector<String>*, Vector<String>, uInt, uInt);
  template void objmove (Vector<String>*, Vector<String> const*, uInt);

  template void objset<void*>(void**, void*, uInt);
  template void objcopy<void*>(void**, void* const*, uInt);
  template void objcopy<TableExprRange>(TableExprRange*, TableExprRange const*, uInt);
  template void objmove<void*>(void**, void* const*, uInt);
  
  //1000 casa/Utilities/PtrHolder.cc 

  template class PtrHolder<ImageConcat<Float> >;
  template class PtrHolder<ImageInterface<Float> >;
  template class PtrHolder<MomentCalcBase<Float> >;
  template class PtrHolder<PagedImage<Float> >;

  template class PtrHolder<ImageConcat<Double> >;
  template class PtrHolder<ImageInterface<Double> >;
  template class PtrHolder<MomentCalcBase<Double> >;
  template class PtrHolder<PagedImage<Double> >;

  // casa/BasicMath/Functional

  template class Functional<Complex, Complex>;
  template class Functional<DComplex, DComplex>;
  template class Functional<Double, Array<Double> >;
  template class Functional<uInt, Array<Double> >;
  template class Functional<Vector<Complex>, Complex>;
  template class Functional<Vector<DComplex>, DComplex>;
  template class Functional<Vector<Double>, Array<Double> >;
  template class Functional<Vector<Double>, Double>;
  template class Functional<Vector<Double>, Float>;
  template class Functional<Vector<Float>, Float>;
  template class Functional<Double, Double>;
  template class Functional<Double, Float>;
  template class Functional<Float, Float>;
  template class Functional<uInt, Double>;
  template class Functional<uInt, Float>;

  // ============================================================================
  // scimath

  template class FunctionParam<int>;
  template class FunctionParam<float>;
  template class FunctionParam<double>;
  template class FunctionParam<Complex>;
  template class FunctionParam<DComplex>;
  
  template class Function<Float, Array<Float> >;
  template class Function<Float, Double>;
  template class Function<Int, Double>;
  template class Function<Int, Complex>;
  template class Function<Double, DComplex>;
 
  // ============================================================================
  //  images/implement

  // 1000 images/Images/PagedImage.cc

  template class PagedImage<Float>;
  template class PagedImage<Complex>;

}
