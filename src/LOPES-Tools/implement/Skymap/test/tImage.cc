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

/* $Id: tImage.cc,v 1.4 2006/07/26 14:25:01 bahren Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <lattices/Lattices/TiledShape.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>

#include <lopes/Skymap/Skymapper.h>

#include <casa/namespace.h>

/*!
  \file tImage.cc

  \ingroup Skymap

  \brief A collection of tests how to work with AIPS++ images
 
  \author Lars B&auml;hren
 
  \date 2006/03/20

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/images/implement/Images.html">Module images</a>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/lattices/implement/Lattices/TiledShape.html">TiledShape class</a>
  </ul>
*/

/*!
  \brief Summarize the characteristics of the created image object 

  \param image -- A paged image
*/
void summary (const PagedImage<Float>& image)
{
  cout << " - Summary of image" << endl;
  cout << " -- image type      : " << image.imageType() << endl;
  cout << " -- is persistent?  : " << image.isPersistent() << endl;
  cout << " -- is paged?       : " << image.isPaged() << endl; 
  cout << " -- is writable?    : " << image.isWritable() << endl; 
  cout << " -- has pixel mask? : " << image.hasPixelMask() << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TiledShape object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TiledShape ()
{
  int nofFailedTests (0);
  
  cout << "\n[test_TiledShape]\n" << endl;

  cout << "[1] Construction from single IPosition ..." << endl;
  try {
    Skymapper skymapper;
    TiledShape shape (skymapper.shape());
    //
    cout << shape.shape() << ' ' << shape.tileShape() << endl;
  } catch (AipsError x) {
    cerr << "[test_TiledShape] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TempImage object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TempImage ()
{
  int nofFailedTests (0);

  Skymapper skymapper;
  CoordinateSystem cs (skymapper.coordinateSystem());
  TiledShape shape (skymapper.shape());
  
  cout << "\n[test_TempImage]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  {
    TempImage<Double> newObject;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  {
    TempImage<Double> newObject (shape,
				 cs);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new PagedImage object

  The following Glish code snippet can be used to inspect the created image tool:
  \code
  include 'image.g';
  img := image ("testimage-01.img");
  img.summary();
  img.statistics();
  if (is_fail(img.view())) {
    print "Unable to display image";
  } else {
    print "Everything done."
  }
  \endcode

  \return nofFailedTests -- The number of failed tests.
*/
int test_PagedImage ()
{
  int nofFailedTests (0);

  Skymapper skymapper;
  CoordinateSystem cs (skymapper.coordinateSystem());
  TiledShape shape (skymapper.shape());
  SetupNewTable setup("testimage.img", TableDesc(), Table::Scratch);
  Table table(setup);
  
  cout << "\n[test_PagedImage]\n" << endl;

  cout << "[1] Construction from table ... " << endl;;
  {
    PagedImage<Float> newObject (shape,
				 cs,
				 table);
    summary (newObject);
  }
  
  cout << "[2] Construction from file name ... " << endl;
  {
    PagedImage<Float> image (shape,
			     cs,
			     String("testimage-01.img"));
    summary (image);
  }
  
  cout << "[3] Access previously created image ... " << endl;
  try {
  } catch (AipsError x) {
    cerr << "[test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
//   cout << "Cleaning up disk" << endl;
//   Table::deleteTable(String("testimage-01.img"));
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_TiledShape ();
  }

//   {
//     nofFailedTests += test_TempImage ();
//   }

  {
    nofFailedTests += test_PagedImage ();
  }

  return nofFailedTests;
}
