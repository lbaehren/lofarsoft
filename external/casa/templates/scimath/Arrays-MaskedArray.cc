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

#include <casa/Arrays/MaskedArray.cc>

namespace casa {

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

  // ============================================================================
  //
  //  Additional templates for test programs
  //
  // ============================================================================

  // 1000 casa/Arrays/MaskedArray.cc scimath/Mathematics/AutoDiff.h 
  template class MaskedArray<AutoDiff<AutoDiff<Double> > >;

}
