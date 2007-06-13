/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <iostream>
#include <string>

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/PagedArray.h>
#include <lattices/Lattices/Lattice.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/TiledShape.h>
#include <lattices/Lattices/LatticeRegion.h>
#include <lattices/Lattices/LCBox.h>
#include <lattices/Lattices/LCEllipsoid.h>

#include <templates.h>

/*!
  \file tUseLattices.cc

  \ingroup CR-Pipeline

  \brief A simple test for external build against liblattices

  \author Lars B&auml;hren

  \date 2007/02/05
*/

using casa::IPosition;
using casa::Slicer;

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  
  std::cout << "[1] Test constructor for ArrayLattice..." << std::endl;
  try {
    const IPosition shape (4,4,4,2,3);

    // create from shape
    casa::ArrayLattice<casa::Int>    latticeInt (shape);
    casa::ArrayLattice<casa::Float>  latticeFloat (shape);
    casa::ArrayLattice<casa::Double> latticeDouble (shape);

    std::cout << " - shape(ArrayLattice<Int>)    = " << latticeInt.shape()    << std::endl;
    std::cout << " - shape(ArrayLattice<Float>)  = " << latticeFloat.shape()  << std::endl;
    std::cout << " - shape(ArrayLattice<Double>) = " << latticeDouble.shape() << std::endl;

    // create from other
    casa::ArrayLattice<casa::Int>    latticeInt2 (latticeInt);
    casa::ArrayLattice<casa::Float>  latticeFloat2 (latticeFloat);
    casa::ArrayLattice<casa::Double> latticeDouble2 (latticeDouble);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Test deletion of rows, array of Strings, and some more..." << std::endl;
  try {
    const IPosition shape (3,32,16,5);

    casa::TiledShape t (shape);
    
    if (t.shape() != shape) {
      std::cerr << "Mismatching shapes" << std::endl;
    }

    if (t.tileShape() != shape) {
      std::cerr << "Mismatching tile shapes" << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Test for LatticeRegion class..." << std::endl;
  try {
    IPosition shape (2,11,20);
    IPosition start (2,3,4);
    IPosition end (2,7,8);
    IPosition center (2,5,10);
    int radius (5);

    uint ndim = start.nelements();
    // Show output of simple circle.
    casa::LCEllipsoid cir (center, radius, shape);
    casa::LatticeRegion reg1(cir);

    reg1.hasMask();
    casa::LatticeRegion reg2 (Slicer(start,end,IPosition(2,2),Slicer::endIsLast),
			      shape);
    
    casa::LatticeRegion reg2a ((Slicer(IPosition(ndim,0), reg2.shape()-1,
				       IPosition(2,1,2), Slicer::endIsLast)),
			       reg2.shape());
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
