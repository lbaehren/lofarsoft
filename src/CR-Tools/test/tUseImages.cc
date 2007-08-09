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
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>
#include <lattices/Lattices/TiledShape.h>

#include <templates.h>

/*!
  \file tUseImages.cc

  \ingroup CR-Pipeline

  \brief A simple test for external build against libimages

  \author Lars B&auml;hren

  \date 2007/02/15
*/

using casa::Double;
using casa::Float;
using casa::Matrix;
using casa::String;
using casa::Vector;

// ------------------------------------------------------------------------------

/*!
  \brief Create the shape information for the created image
  
  \return tiledShape -- A TiledShape object, used to discribe the internal shape
                        of the image
*/
casa::IPosition image_shape ()
{
  casa::IPosition shape (5,25,25,10,50,128);

  return shape;
}

// ------------------------------------------------------------------------------

/*!
  \brief Create the coordinate system attached to the image
  
  \return cs -- Coordinate system tool
*/
casa::CoordinateSystem image_csys ()
{
  const double pi (3.14159265);
  casa::CoordinateSystem cs;

  // [1] Direction axis
  {
    casa::IPosition shape;
    Matrix<Double> xform(2,2);
    
    shape = image_shape();
    xform = 0.0;
    xform.diagonal() = 1.0;
    
    casa::DirectionCoordinate coord (casa::MDirection::AZEL,
				     casa::Projection(casa::Projection::STG),
				     0.0*pi/180.0,
				     90.0*pi/180.0,
				     -2.0*pi/180.0,
				     2.0*pi/180.0,
				     xform,
				     0.5*shape(0),
				     0.5*shape(1));
    
    coord.setReferencePixel(Vector<Double>(2,0.0));
    
    cs.addCoordinate(coord);
  }
  
  {
    Vector<String> names (1,"Distance");
    Vector<String> units (1,"m");
    Vector<Double> refVal (1,-1.0);
    Vector<Double> inc (1,0.0);
    Matrix<Double> pc (1,1,1.0);
    Vector<Double> refPix (1,0.0);
    
    casa::LinearCoordinate coord (names,
				  units,
				  refVal,
				  inc,
				  pc,
				  refPix);
    
    cs.addCoordinate(coord);
  }
  
  {
    Vector<String> names  (1,"Time");
    Vector<String> units  (1,"s");
    Vector<Double> refVal (1,0.0);
    Vector<Double> inc    (1,1.0);
    Matrix<Double> pc     (1,1,1.0);
    Vector<Double> refPix (1,0.0);
    
    casa::LinearCoordinate axis (names,
				  units,
				  refVal,
				  inc,
				  pc,
				  refPix);
    
    cs.addCoordinate(axis);
  }

  {
    Vector<String> names  (1,"Frequency");
    casa::SpectralCoordinate axis;
    
    axis.setWorldAxisNames(names);
    
    cs.addCoordinate(axis);
  }
  
  return cs;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test the creation of a paged image
  
  \return nofFailedTests -- The number of failed tests
*/
int test_PagedImage ()
{
  int nofFailedTests (0);
  
  try {
    casa::IPosition shape (image_shape());
    casa::String filename ("testimage.img");
    casa::TiledShape tshape (shape);

    std::cout << " - Creating PagedImage of shape "
	      << shape
	      << " on disk..."
	      << std::endl;
    casa::PagedImage<Float> image (tshape,
			      image_csys(),
			      filename);
    std::cout << " - Finished writing image." << std::endl;

//     std::cout << " - Creating pixel array..." << std::endl;
//     casa::Array<Float> pixels (shape);
//     pixels = 1.0;

//     std::cout << " - Write pixel values to image..." << std::endl;
//     casa::IPosition start  (shape.nelements(),0);
//     casa::IPosition stride (shape.nelements(),1);
//     image.putSlice (pixels,start,stride);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_PagedImage ();
  }

//   try {
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }

  return nofFailedTests;
}
