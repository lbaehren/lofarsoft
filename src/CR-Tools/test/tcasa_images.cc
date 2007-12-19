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
    Matrix<double> xform(2,2);
    
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
    
    coord.setReferencePixel(Vector<double>(2,0.0));
    
    cs.addCoordinate(coord);
  }
  
  {
    Vector<String> names (1,"Distance");
    Vector<String> units (1,"m");
    Vector<double> refVal (1,-1.0);
    Vector<double> inc (1,0.0);
    Matrix<double> pc (1,1,1.0);
    Vector<double> refPix (1,0.0);
    
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
    Vector<double> refVal (1,0.0);
    Vector<double> inc    (1,1.0);
    Matrix<double> pc     (1,1,1.0);
    Vector<double> refPix (1,0.0);
    
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

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);

  /* check the input parameters first */

  if (argc < 2) {
    std::cout << "[tcasa_image] No path to image provided." << std::endl;
  } else {
    std::string filename = argv[1];
    std::cout << "[tcasa_image] Path to test image = " << filename << std::endl;
  }

  /* Everything from here on does not rely on the existance of an already
     pre-existing image. */

  return nofFailedTests;
}
