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

// include of header files 

#include <casa/BasicSL/Complex.h>
#include <casa/Containers/PoolStack.h>
#include <casa/Utilities/Fallible.h>
#include <scimath/Mathematics/AutoDiffIO.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <scimath/Mathematics/AutoDiffRep.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/ObjectPool.cc>
#include <casa/Containers/OrderedPair.cc>
#include <casa/Containers/PoolStack.cc>
#include <casa/Containers/SimOrdMap.cc>
#include <scimath/Mathematics/AutoDiff.cc>
#include <scimath/Mathematics/AutoDiffIO.cc>
#include <scimath/Mathematics/AutoDiffMath.cc>
#include <scimath/Mathematics/AutoDiffRep.cc>
#include <scimath/Mathematics/Convolver.cc>
#include <scimath/Mathematics/FFTServer.cc>
#include <scimath/Mathematics/HistAcc.cc>
#include <scimath/Mathematics/MathFunc.cc>
#include <scimath/Mathematics/MatrixMathLA.cc>
#include <scimath/Mathematics/RigidVector.cc>
#include <scimath/Mathematics/SquareMatrix.cc>
#include <scimath/Mathematics/StatAcc.cc>

namespace casa {

  // ============================================================================
  //
  //  Templates for library
  //
  // ============================================================================

  template class Array<Double>;

  template class AutoDiff<Int>;
  template class AutoDiff<Float>;
  template class AutoDiff<Double>;
  template class AutoDiff<Complex>;
  template class AutoDiff<DComplex>;

  template ostream & operator<<(ostream &, Array<AutoDiff<Float> > const &);
  template ostream & operator<<(ostream &, Array<AutoDiff<Double> > const &);
  template ostream & operator<<(ostream &, Array<AutoDiff<Complex> > const &);
  template ostream & operator<<(ostream &, Array<AutoDiff<DComplex> > const &);

  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);

  template void operator+=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator-=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator*=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator/=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);

  template void operator+=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator-=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator*=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator/=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);

  template bool operator< (AutoDiff<Float> const&, AutoDiff<Float> const&);
  template bool operator< (AutoDiff<Double> const&, AutoDiff<Double> const&);
  template bool operator< (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template bool operator< (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template bool operator> (AutoDiff<Float> const&, AutoDiff<Float> const&);
  template bool operator> (AutoDiff<Double> const&, AutoDiff<Double> const&);
  template bool operator> (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template bool operator> (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template AutoDiff<float> operator* (AutoDiff<float> const&, AutoDiff<float> const&);
  template AutoDiff<double> operator* (AutoDiff<double> const&, AutoDiff<double> const&);
  template AutoDiff<Complex> operator* (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> operator* (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template AutoDiff<float> operator/ (AutoDiff<float> const&, AutoDiff<float> const&);
  template AutoDiff<Double> operator/ (AutoDiff<Double> const&, AutoDiff<Double> const&);
  template AutoDiff<Complex> operator/ (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> operator/ (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template AutoDiff<float> operator- (float const&, AutoDiff<float> const&);
  template AutoDiff<double> operator- (double const&, AutoDiff<double> const&);
  template AutoDiff<Complex> operator- (Complex const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> operator- (DComplex const&, AutoDiff<DComplex> const&);

  template AutoDiff<float> operator- (AutoDiff<float> const&, float const&);
  template AutoDiff<double> operator- (AutoDiff<double> const&, double const&);
  template AutoDiff<Complex> operator- (AutoDiff<Complex> const&, Complex const&);
  template AutoDiff<DComplex> operator- (AutoDiff<DComplex> const&, DComplex const&);

  template AutoDiff<float> operator- (AutoDiff<float> const&, AutoDiff<float> const&);
  template AutoDiff<double> operator- (AutoDiff<double> const&, AutoDiff<double> const&);
  template AutoDiff<Complex> operator- (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> operator- (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template Array<AutoDiff<Float> > operator-(Array<AutoDiff<Float> > const &);
  template Array<AutoDiff<Double> > operator-(Array<AutoDiff<Double> > const &);
  template Array<AutoDiff<Complex> > operator-(Array<AutoDiff<Complex> > const &);
  template Array<AutoDiff<DComplex> > operator-(Array<AutoDiff<DComplex> > const &);

  template AutoDiff<float> operator+ (AutoDiff<float> const&, AutoDiff<float> const&);
  template AutoDiff<double> operator+ (AutoDiff<double> const&, AutoDiff<double> const&);
  template AutoDiff<Complex> operator+ (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> operator+ (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template class MaskedArray<AutoDiff<AutoDiff<Double> > >;

  template class Vector<AutoDiff<AutoDiff<Double> > >;

  template class ObjectPool<AutoDiffRep<AutoDiff<Double> >, uInt>;

  template class OrderedPair<uInt, PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt> *>;

  template class PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt>;

  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt>*>;

  template class Fallible<Float>;

  template class AutoDiffRep<DComplex>;

  template class StatAcc<Double>;

  // ============================================================================
  //
  //  Additional templates for test programs
  //
  // ============================================================================

  // 1000 scimath/Mathematics/AutoDiff.cc
  template class AutoDiff<AutoDiff<Double> >;

  // 1000 scimath/Mathematics/AutoDiffIO.cc scimath/Mathematics/AutoDiff.h 
  template ostream & operator<<(ostream &, AutoDiff<AutoDiff<Double> > const &);

  template bool operator== (AutoDiff<float> const&, AutoDiff<float> const&);
  template bool operator== (AutoDiff<double> const&, AutoDiff<double> const&);
  template bool operator== (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template bool operator== (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  // 1000 scimath/Mathematics/AutoDiffMath.cc 
  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<AutoDiff<Double> > const &, AutoDiff<AutoDiff<Double> > const &);
  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<Double> const &, AutoDiff<AutoDiff<Double> > const &);
  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<AutoDiff<Double> > const &, AutoDiff<Double> const &);

  // 1000 scimath/Mathematics/AutoDiffRep.cc scimath/Mathematics/AutoDiff.h 
  template class AutoDiffRep<AutoDiff<Double> >;

  // 1000 scimath/Mathematics/Convolver.cc 
  template class Convolver<Double>;

  // 1000 scimath/Mathematics/FFTServer.cc casa/BasicSL/Complex.h 
  template class FFTServer<Double, DComplex> ;

  // 1000 scimath/Mathematics/HistAcc.cc 
  template class HistAcc<Float>;
  template class HistAcc<Int>;

  // 1000 scimath/Mathematics/MathFunc.cc 
  template class ExpSincConv<Double>;
  template class GaussianConv<Double>;
  template class KB_Conv<Double>;
//   template class MathFunc<Double>;
  template class Mod_KB_Conv<Double>;
  template class Sinc_Conv<Double>;
//   template class Sph_Conv<Double>;
  template class Unary<Double>;

  // 1010 scimath/Mathematics/MathFunc.cc 
  template class ExpSincConv<Float>;
  template class GaussianConv<Float>;
  template class KB_Conv<Float>;
  template class MathFunc<Float>;
  template class Mod_KB_Conv<Float>;
  template class Sinc_Conv<Float>;
  template class Sph_Conv<Float>;
  template class Unary<Float>;

  // 1000 scimath/Mathematics/MatrixMathLA.cc 
  template Matrix<DComplex> invert(Matrix<DComplex> const &);
  template void invert(Matrix<DComplex> &, DComplex &, Matrix<DComplex> const &);

  // 1000 scimath/Mathematics/StatAcc.cc 
  template class StatAcc<Float>;
  template class StatAcc<Int>;

  template AutoDiff<Float> abs (AutoDiff<Float> const&);
  template AutoDiff<Double> abs (AutoDiff<Double> const&);
  template AutoDiff<Complex> abs (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> abs (AutoDiff<DComplex> const&);

  template AutoDiff<float> asin (AutoDiff<float> const&);
  template AutoDiff<double> asin (AutoDiff<double> const&);
  template AutoDiff<Complex> asin (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> asin (AutoDiff<DComplex> const&);

  template AutoDiff<float> cos (AutoDiff<float> const&);
  template AutoDiff<double> cos (AutoDiff<double> const&);
  template AutoDiff<Complex> cos (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> cos (AutoDiff<DComplex> const&);

  template AutoDiff<float> exp (AutoDiff<float> const&);
  template AutoDiff<double> exp (AutoDiff<double> const&);
  template AutoDiff<Complex> exp (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> exp (AutoDiff<DComplex> const&);

  template AutoDiff<Float> log (AutoDiff<Float> const&);
  template AutoDiff<Double> log (AutoDiff<Double> const&);
  template AutoDiff<Complex> log (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> log (AutoDiff<DComplex> const&);

  template AutoDiff<float> pow (AutoDiff<float> const&, AutoDiff<float> const&);
  template AutoDiff<double> pow (AutoDiff<double> const&, AutoDiff<double> const&);
  template AutoDiff<Complex> pow (AutoDiff<Complex> const&, AutoDiff<Complex> const&);
  template AutoDiff<DComplex> pow (AutoDiff<DComplex> const&, AutoDiff<DComplex> const&);

  template AutoDiff<float> sin (AutoDiff<float> const&);
  template AutoDiff<double> sin (AutoDiff<double> const&);
  template AutoDiff<Complex> sin (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> sin (AutoDiff<DComplex> const&);

  template AutoDiff<float> sinh (AutoDiff<float> const&);
  template AutoDiff<double> sinh (AutoDiff<double> const&);
  template AutoDiff<Complex> sinh (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> sinh (AutoDiff<DComplex> const&);

  template AutoDiff<float> sqrt (AutoDiff<float> const&);
  template AutoDiff<double> sqrt (AutoDiff<double> const&);
  template AutoDiff<Complex> sqrt (AutoDiff<Complex> const&);
  template AutoDiff<DComplex> sqrt (AutoDiff<DComplex> const&);

}
