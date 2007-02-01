/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <complex>
#include <iostream>
#include <string>

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/MaskedArray.cc>

/*!
  \file tUseCasa.cc

  \brief A simple test for external build against libcasa

  \author Lars B&auml;hren

  \date 2007/01/26

  This is the first part in a series of test to explore building against an
  existing CASA installation using CMake
*/

// Template instantiation for CASA library 

namespace casa {
  template class ArrayIterator<float>;
  template class ArrayIterator<Double>;
  template class ArrayIterator<Complex>;
  template class ArrayIterator<DComplex>;

  template class VectorIterator<float>;
  template class VectorIterator<Double>;
  template class VectorIterator<Complex>;
  template class VectorIterator<DComplex>;

  template Bool allNearAbs(float const &, Array<float> const &, Double);
  template Bool allNearAbs(Double const &, Array<Double> const &, Double);
  template Bool allNearAbs(Complex const &, Array<Complex> const &, Double);
  template Bool allNearAbs(DComplex const &, Array<DComplex> const &, Double);
  
  template Bool allNearAbs(Array<float> const &, float const &, Double);
  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Complex const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, DComplex const &, Double);
  
  template Bool allNearAbs(Array<float> const &, Array<float> const &, Double);
  template Bool allNearAbs(Array<Double> const &, Array<Double> const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);
}

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);
  uint nofElements (100);

  try {
    casa::Vector<float> in (nofElements);
    casa::Vector<casa::Complex> out;

    casa::FFTServer<float,casa::Complex> server(casa::IPosition(1,nofElements),
						casa::FFTEnums::REALTOCOMPLEX);
    server.fft(out,in);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
