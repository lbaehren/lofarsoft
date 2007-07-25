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

using std::cout;
using std::endl;

using casa::IPosition;
using casa::Lattice;
using casa::PagedArray;
using casa::Slicer;

// ------------------------------------------------------------------------------

/*!
  \brief Test the construction of various ArrayLattice objects

  \return nofFailedTest -- The number of failed tests
*/
int test_ArrayLattice ()
{
  cout << "\n[test_ArrayLattice]\n" << endl;

  int nofFailedTests (0);

  try {
    const IPosition shape (4,4,4,2,3);

    // create from shape
    casa::ArrayLattice<casa::Int>    latticeInt (shape);
    casa::ArrayLattice<casa::Float>  latticeFloat (shape);
    casa::ArrayLattice<casa::Double> latticeDouble (shape);

    cout << " - shape(ArrayLattice<Int>)    = " << latticeInt.shape()    << endl;
    cout << " - shape(ArrayLattice<Float>)  = " << latticeFloat.shape()  << endl;
    cout << " - shape(ArrayLattice<Double>) = " << latticeDouble.shape() << endl;

    // create from other
    casa::ArrayLattice<casa::Int>    latticeInt2 (latticeInt);
    casa::ArrayLattice<casa::Float>  latticeFloat2 (latticeFloat);
    casa::ArrayLattice<casa::Double> latticeDouble2 (latticeDouble);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test the construction of and working with a LatticeRegion object

  \return nofFailedTest -- The number of failed tests
*/
int test_LatticeRegion ()
{
  cout << "\n[test_LatticeRegion]\n" << endl;

  int nofFailedTests (0);
  
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

    cout << " -- shape  = " << shape  << endl;
    cout << " -- start  = " << start  << endl;
    cout << " -- end    = " << end    << endl;
    cout << " -- center = " << center << endl;

    reg1.hasMask();
    casa::LatticeRegion reg2 (Slicer(start,end,IPosition(2,2),Slicer::endIsLast),
			      shape);
    
    casa::LatticeRegion reg2a ((Slicer(IPosition(ndim,0), reg2.shape()-1,
				       IPosition(2,1,2), Slicer::endIsLast)),
			       reg2.shape());
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test slicing in subarray into a larger lattice
  
  To access the data in a PagedArray you can either:
  <ol>
    <li>Use a LatticeIterator
    <li>Use the getSlice and putSlice member functions
    <li>Use the parenthesis operator or getAt and putAt functions
  </ol>
  These access methods are given in order of preference.

  \return nofFailedTests -- The number of failed tests
*/
int test_putSlice ()
{
  cout << "\n[test_putSlice]\n" << endl;

  int nofFailedTests (0);

  try {
    int nofAxes (5);
    int nofChannels (128);
    IPosition shape (nofAxes,40,40,20,10,nofChannels);
    casa::String filename ("PagedArray.tmp");

    PagedArray<double> arr (shape,filename);

    int nofPixels (shape(0)*shape(1)*shape(2));
    IPosition pixelsShape (2,nofPixels,nofChannels);
    casa::Matrix<float> pixels (pixelsShape);
    // variables required for the array slicing operation
    IPosition start (nofAxes,0);
    IPosition stride (nofAxes,1);
    IPosition beamSliceStart (2,0);
    IPosition beamSliceEnd (2,shape(4)-1);
    int slice (0);

    cout << " -- array shape ..... = " << shape    << endl;
    cout << " -- file name ....... = " << filename << endl;
    cout << " -- pixel array shape = " << pixelsShape << endl;

    for (start(3)=0; start(3)<shape(3); start(3)++) {
      
      // From onwards this position we perform the actual slicing operation

      pixels = start(3);
      slice  = 0;
      
      for (start(0)=0; start(0)<shape(0); start(0)++) {      // Longitude
	for (start(1)=0; start(1)<shape(1); start(1)++) {    // Latitude
	  // news slicer positions
	  beamSliceStart(0) = slice*shape(2);
	  beamSliceEnd(0)   = beamSliceStart(0)+shape(2)-1;
	  slice++;
	  // feedback
	  cout << "\t" << beamSliceStart << " -> " << beamSliceEnd
	       << "  =>  " << start << endl;
	  // insert the pixel values
// 	  arr.doPutSlice (pixels(beamSliceStart,beamSliceEnd),start,stride);
	}
      }
    }
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  nofFailedTests += test_ArrayLattice();
  nofFailedTests += test_LatticeRegion();

  nofFailedTests += test_putSlice();
  
  return nofFailedTests;
}
