/***************************************************************************
 *   Copyright (C) 2006                                                    *
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
  \file Arrays.cc

  \ingroup scimath

  \brief Template instantiation for the <i>scimath</i> CASA module

  \author Lars B&auml;hren

  \date 2007/01/19
*/

// include of header files 

#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Utilities/CountedPtr.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffIO.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <scimath/Mathematics/RigidVector.h>
#include <scimath/Mathematics/SquareMatrix.h>
#include <scimath/Functionals/Function1D.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/Cube.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Matrix.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/BasicMath/Functional.cc>

namespace casa {

  // Array

  template class Array<AutoDiff<Float> >;
  template class Array<AutoDiff<Double> >;
  template class Array<AutoDiff<Complex> >;
  template class Array<AutoDiff<DComplex> >;
  template class Array<AutoDiffA<Float> >;
  template class Array<AutoDiffA<Double> >;
  template class Array<AutoDiffA<Complex> >;
  template class Array<AutoDiffA<DComplex> >;
  template class Array<Vector<SquareMatrix<Complex, 2> > >;
  template class Array<Vector<SquareMatrix<Complex, 4> > >;
  template class Array<Vector<SquareMatrix<Float, 2> > >;
  template class Array<OrderedMap<Double, SquareMatrix<Complex, 2>*>*>;
  template class Array<CountedPtr<Function1D<Float> > >;
  template class Array<CountedPtr<SquareMatrix<Complex, 4> > >;
  template class Array<CountedPtr<SquareMatrix<Complex, 2> > >;
  template class Array<RigidVector<Double, 3> >;
  template class Array<RigidVector<Double, 2> >;
  template class Array<SquareMatrix<Complex, 2> >;
  template class Array<SquareMatrix<Complex, 4> >;
  template class Array<SquareMatrix<Float, 2> >;
  template class Array<SquareMatrix<Float, 4> >;

#ifdef AIPS_SUN_NATIVE 
  template class Array<AutoDiff<Complex> >::ConstIteratorSTL;
  template class Array<AutoDiff<DComplex> >::ConstIteratorSTL;
  template class Array<AutoDiff<Double> >::ConstIteratorSTL;
  template class Array<AutoDiff<Float> >::ConstIteratorSTL;
  template class Array<AutoDiffA<Complex> >::ConstIteratorSTL;
  template class Array<AutoDiffA<DComplex> >::ConstIteratorSTL;
  template class Array<AutoDiffA<Double> >::ConstIteratorSTL;
  template class Array<AutoDiffA<Float> >::ConstIteratorSTL;
#endif 

  // Cube

  template class Cube<Vector<SquareMatrix<Complex, 2> > >;
  template class Cube<Vector<SquareMatrix<Complex, 4> > >;
  template class Cube<SquareMatrix<Complex, 2> >;
  template class Cube<SquareMatrix<Complex, 4> >;
  template class Cube<OrderedMap<Double, SquareMatrix<Complex, 2>*>*>;
  template class Cube<RigidVector<Double, 2> >;
  template class Cube<SquareMatrix<Float, 4> >;

  // MaskedArray

  template class MaskedArray<AutoDiffA<Complex> >;
  template class MaskedArray<AutoDiffA<DComplex> >;
  template class MaskedArray<Vector<SquareMatrix<Complex, 2> > >;
  template class MaskedArray<Vector<SquareMatrix<Complex, 4> > >;
  template class MaskedArray<Vector<SquareMatrix<Float, 2> > >;
  template class MaskedArray<SquareMatrix<Complex, 2> >;
  template class MaskedArray<SquareMatrix<Complex, 4> >;
  template class MaskedArray<OrderedMap<Double, SquareMatrix<Complex, 2>*>*>;
  template class MaskedArray<CountedPtr<Function1D<Float> > >;
  template class MaskedArray<CountedPtr<SquareMatrix<Complex, 4> > >;
  template class MaskedArray<CountedPtr<SquareMatrix<Complex, 2> > >;
  template class MaskedArray<AutoDiff<Complex> >;
  template class MaskedArray<AutoDiff<DComplex> >;
  template class MaskedArray<AutoDiff<Double> >;
  template class MaskedArray<AutoDiff<Float> >;
  template class MaskedArray<AutoDiffA<Double> >;
  template class MaskedArray<AutoDiffA<Float> >;
  template class MaskedArray<RigidVector<Double, 3> >;
  template class MaskedArray<RigidVector<Double, 2> >;
  template class MaskedArray<SquareMatrix<Float, 2> >;
  template class MaskedArray<SquareMatrix<Float, 4> >;

  // Matrix

  template class Matrix<Vector<SquareMatrix<Complex, 2> > >;
  template class Matrix<Vector<SquareMatrix<Complex, 4> > >;
  template class Matrix<Vector<SquareMatrix<Float, 2> > >;
  template class Matrix<SquareMatrix<Complex, 2> >;
  template class Matrix<SquareMatrix<Complex, 4> >;
  template class Matrix<OrderedMap<Double, SquareMatrix<Complex, 2>*>*>;
  template class Matrix<AutoDiff<Complex> >;
  template class Matrix<AutoDiff<DComplex> >;
  template class Matrix<AutoDiff<Double> >;
  template class Matrix<AutoDiff<Float> >;
  template class Matrix<AutoDiffA<Double> >;
  template class Matrix<AutoDiffA<Float> >;
  template class Matrix<RigidVector<Double, 2> >;
  template class Matrix<SquareMatrix<Float, 2> >;
  template class Matrix<SquareMatrix<Float, 4> >;

  // Vector

  template class Vector<AutoDiffA<Complex> >;
  template class Vector<AutoDiffA<DComplex> >;
  template class Vector<Vector<SquareMatrix<Complex, 2> > >;
  template class Vector<Vector<SquareMatrix<Complex, 4> > >;
  template class Vector<Vector<SquareMatrix<Float, 2> > >;
  template class Vector<SquareMatrix<Complex, 2> >;
  template class Vector<SquareMatrix<Complex, 4> >;
  template class Vector<OrderedMap<Double, SquareMatrix<Complex, 2>*>*>;
  template class Vector<AutoDiff<Complex> >;
  template class Vector<AutoDiff<DComplex> >;
  template class Vector<AutoDiff<Double> >;
  template class Vector<AutoDiff<Float> >;
  template class Vector<AutoDiffA<Double> >;
  template class Vector<AutoDiffA<Float> >;
  template class Vector<RigidVector<Double, 3> >;
  template class Vector<RigidVector<Double, 2> >;
  template class Vector<SquareMatrix<Float, 2> >;
  template class Vector<SquareMatrix<Float, 4> >;

  // ============================================================================
  //
  //  Additional templates for test programs
  //
  // ============================================================================

  // - Fitting/test

  // 1000 casa/Arrays/ArrayMath.cc 
  template void indgen(Array<Complex> &, Complex);
  template void indgen(Array<Complex> &, Complex, Complex);

  // 1000 casa/BasicMath/Functional.cc casa/BasicSL/Complex.h 
  template class Functional<Vector<Complex>, Vector<Complex> >;
  template class Functional<Vector<DComplex>, Vector<DComplex> >;

  // - Mathematics/test

  // 1000 casa/Arrays/Array.cc scimath/Mathematics/AutoDiff.h 
  template class Array<AutoDiff<AutoDiff<Double> > >;

  // 1000 casa/Arrays/ArrayIO.cc 
  template ostream & operator<<(ostream &, Array<AutoDiff<Double> > const &);

  // 1000 casa/Arrays/ArrayLogical.cc 
  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);

  // 1000 casa/Arrays/ArrayMath.cc 
  template void operator+=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator-=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator*=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator/=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator+=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator-=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator*=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator/=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template Array<AutoDiff<Double> > operator-(Array<AutoDiff<Double> > const &);

  // 1000 casa/Arrays/MaskedArray.cc scimath/Mathematics/AutoDiff.h 
  template class MaskedArray<AutoDiff<AutoDiff<Double> > >;

  // 1000 casa/Arrays/Vector.cc scimath/Mathematics/AutoDiff.h 
  template class Vector<AutoDiff<AutoDiff<Double> > >;

}
