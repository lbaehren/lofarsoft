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

/* $Id$*/

#include <templates.h>
#include <Imaging/SkymapCoordinates.h>
#include <images/Images/PagedImage.h>

using std::cout;
using std::endl;

using casa::DirectionCoordinate;
using casa::LinearCoordinate;
using casa::MDirection;
using casa::SpectralCoordinate;

using CR::ObservationData;
using CR::SkymapCoordinates;

/*!
  \file tSkymapCoordinates.cc

  \ingroup Imaging

  \brief A collection of test routines for CR::SkymapCoordinates
 
  \author Lars B&auml;hren
 
  \date 2007/03/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Wrapper for creation of a SkymapCoordinates object 

  \return coord -- A new SkymapCoordinates object
*/
SkymapCoordinates create_SkymapCoordinates ()
{
  TimeFreq timeFreq (1024,40e6,1);        // Time and Frequency data
  ObservationData obsData ("LOPES");      // Observation data
  uint nofBlocks (1);                     // Number of processed blocks
  
  // create SkymapCoordinates object
  SkymapCoordinates coord (timeFreq,
			   obsData,
			   nofBlocks,
			   SkymapCoordinates::NORTH_WEST,
			   SkymapCoordinates::TIME_CC);

  return coord;
}

// -----------------------------------------------------------------------------

/*!
  \brief Provide a summary of a CoordinateSystem object

  \verbatim
  -- Number of coordinates   = 4
  -- World axis names        = [Longitude, Latitude, Distance, Time, Frequency]
  -- World axis units        = [rad, rad, m, s, Hz]
  -- Reference pixel (CRPIX) = [60, 60, 0, 0, 1]
  -- Reference value (CRVAL) = [0, 1.5708, -1, 0, 0]
  -- Increment       (CDELT) = [-0.0349066, 0.0349066, 0, 2.5e-08, 39062.5]
  \endverbatim
*/
void show_CoordinateSystem (casa::CoordinateSystem &csys)
{
  casa::ObsInfo obsinfo = csys.obsInfo();

  // Oberservation information
  cout << "-- Telescope name          = " << obsinfo.telescope()   << endl;
  cout << "-- Observer name           = " << obsinfo.observer()    << endl;
  cout << "-- Observation date        = " << obsinfo.obsDate()     << endl;
  // Information on the coordinate axes
  cout << "-- Number of coordinates   = " << csys.nCoordinates()   << endl;
  cout << "-- World axis names        = " << csys.worldAxisNames() << endl;
  cout << "-- World axis units        = " << csys.worldAxisUnits() << endl;
  cout << "-- Reference pixel (CRPIX) = " << csys.referencePixel() << endl;
  cout << "-- Reference value (CRVAL) = " << csys.referenceValue() << endl;
  cout << "-- Increment       (CDELT) = " << csys.increment()      << endl;
  
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with the <tt>toWorld</tt> method of a casa::Coordinate

  \param axis -- The casa::Coordinate, on which to test and perform the
                 conversion from pixel to world values.
*/
void test_pixel2world (casa::Coordinate &axis) 
{
  uint nofPixels (10);
  Vector<Double> pixelValues (nofPixels);
  Vector<Double> worldValues (nofPixels);
  Vector<Double> crpix = axis.referencePixel();
  Vector<Double> crval = axis.referenceValue();
  Vector<Double> cdelt = axis.increment();
  Matrix<Double> pc    = axis.linearTransform();
  
  if (crpix.nelements() == 1) {
    Vector<Double> valPixel (1);
    Vector<Double> valWorld (1);
    for (uint pixel(0); pixel<nofPixels; pixel++) {
      valPixel(0) = crpix(0)+pixel;
      axis.toWorld (valWorld,valPixel);
      pixelValues(pixel) = valPixel(0);
      worldValues(pixel) = valWorld(0);
    }
  }
  
  cout << "-- Reference pixel  = " << crpix << endl;
  cout << "-- Reference value  = " << crval << endl;
  cout << "-- Increment        = " << cdelt << endl;
  cout << "-- Linear transform = " << pc.row(0) << endl;
  cout << "-- Pixel values     = " << pixelValues << endl;
  cout << "-- World values     = " << worldValues << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SkymapCoordinates object

  Test the various construction methods for a SkymapCoordinates object; we need
  to make sure, that such an object, as used inside a Skymapper object, will
  contain the basic information required to create a default skymap.

  \return nofFailedTests -- The number of failed tests.
*/
int test_SkymapCoordinates ()
{
  cout << "\n[test_SkymapCoordinates]\n" << endl;

  int nofFailedTests (0);                 // nof. failed tests
  TimeFreq timeFreq (1024,40e6,1);        // Time and Frequency data
  ObservationData obsData ("LOFAR-ITS");  // Observation data
  uint nofBlocks (10);                    // Number of processed blocks

  obsData.setObserver ("Lars Baehren");
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    SkymapCoordinates coord;
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    // create SkymapCoordinates object
    SkymapCoordinates coord (timeFreq,
			     obsData,
			     nofBlocks);
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing argumented constructor ..." << endl;
  try {
    // create SkymapCoordinates object
    SkymapCoordinates coord (timeFreq,
			     obsData,
			     nofBlocks,
			     SkymapCoordinates::NORTH_WEST,
			     SkymapCoordinates::TIME_CC);
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various conversion routines

  As it turns out not all of the possible spherical map projections can be
  constructed, properly; currently the following projection types cause fail
  to return a projection

  \return nofFailedTests -- The number of failed tests.
*/
int test_conversions ()
{
  cout << "\n[test_conversions]\n" << endl;

  int nofFailedTests (0);
  SkymapCoordinates coord;
  
  cout << "[1] Projections and their parameters" << endl;
  try {
    std::vector<std::string> projectionRefcode;

//     projectionRefcode.push_back("AZP");
    projectionRefcode.push_back("TAN");
    projectionRefcode.push_back("SIN");
    projectionRefcode.push_back("STG");
    projectionRefcode.push_back("ARC");
//     projectionRefcode.push_back("ZPN");
    projectionRefcode.push_back("ZEA");
//      projectionRefcode.push_back("AIR");
//      projectionRefcode.push_back("CYP");
    projectionRefcode.push_back("CAR");
    projectionRefcode.push_back("MER");
//     projectionRefcode.push_back("CEA");
//      projectionRefcode.push_back("COP");
    projectionRefcode.push_back("AIT");
    projectionRefcode.push_back("MOL");
    projectionRefcode.push_back("CSC");
    projectionRefcode.push_back("QSC");
    projectionRefcode.push_back("TSC");

    cout << "\tRefcode\tType\tzenithal\tparameters" << endl;
    
    for (uint n(0); n<projectionRefcode.size(); n++) {
      Projection projection (coord.ProjectionType (projectionRefcode[n]));
      cout << "\t" << projection.name()
		<< "\t" << projection.type()
		<< "\t" << projection.isZenithal(projection.type())
 		<< "\t\t" << projection.parameters()
		<< endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Direction frames and their type" << endl;
  try {
    uint refType (0);
    std::vector<std::string> directionRefcode;
    std::vector<casa::MDirection::Types> directionTypes;

    directionRefcode.push_back("J2000");   directionTypes.push_back(MDirection::J2000);
    directionRefcode.push_back("JNAT");    directionTypes.push_back(MDirection::JNAT);
    directionRefcode.push_back("JMEAN");   directionTypes.push_back(MDirection::JMEAN);
    directionRefcode.push_back("JTRUE");   directionTypes.push_back(MDirection::JTRUE);
    directionRefcode.push_back("APP");     directionTypes.push_back(MDirection::APP);
    directionRefcode.push_back("B1950");   directionTypes.push_back(MDirection::B1950);
    directionRefcode.push_back("HADEC");   directionTypes.push_back(MDirection::HADEC);
    directionRefcode.push_back("AZEL");    directionTypes.push_back(MDirection::AZEL);
    directionRefcode.push_back("AZELSW");  directionTypes.push_back(MDirection::AZELSW);
    directionRefcode.push_back("AZELNE");  directionTypes.push_back(MDirection::AZELNE);
    directionRefcode.push_back("ITRF");    directionTypes.push_back(MDirection::ITRF);
    directionRefcode.push_back("TOPO");    directionTypes.push_back(MDirection::TOPO);
    directionRefcode.push_back("SUN");     directionTypes.push_back(MDirection::SUN);
    directionRefcode.push_back("MOON");    directionTypes.push_back(MDirection::MOON);

    cout << "\tRefcode\tType\t\tRefString\ttype\tRefType\tshowType" << endl;
    
    for (uint n(0); n<directionRefcode.size(); n++) {
      MDirection direction (coord.MDirectionType (directionRefcode[n]));
      refType = direction.getRef().getType();
      cout << "\t" << directionRefcode[n]
		<< "\t" << directionTypes[n] << "\t->"
		<< "\t" << direction.getRefString()
		<< "\t\t" << direction.type()
		<< "\t" << refType
		<< "\t" << direction.showType (refType)
		<< "\t" << direction.showType (directionTypes[n])
		<< endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the CoordinateSystem object created from the input parameters

  This function will run a number of tests related to working with the
  coordinates and the coordinate system created from the input parameters:
  <ol>
    <li>Simple retrival of the constructed casa::CoordinateSystem object and check
        of its contents.
    <li>Retrival of the individual coordinate axes (direction, distance, time,
        frequency) -- either from the CoordinateSystem object or directory using
	the functions in LOPES::SkymapCoordinates.
    <li>
  </ol>
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_coordinateSystem ()
{
  cout << "\n[test_coordinateSystem]\n" << endl;

  int nofFailedTests (0);                 // nof. failed tests
  bool status (true);
  TimeFreq timeFreq (1024,40e6,1);        // Time and Frequency data
  ObservationData obsData ("WSRT");       // Observation data
  uint nofBlocks (10);                    // Number of processed blocks
  
  // create SkymapCoordinates object
  SkymapCoordinates coord (timeFreq,
			   obsData,
			   nofBlocks,
			   SkymapCoordinates::NORTH_WEST,
			   SkymapCoordinates::TIME_CC);
  
  cout << "[0] Check the numbering of the image axes..." << endl;
  try {
    cout << "-- Coordinate for direction axes : "
	 << SkymapCoordinates::Direction << endl;
    cout << "-- Coordinate for distance axis  : "
	 << SkymapCoordinates::Distance  << endl;
    cout << "-- Coordinate for time     axis  : "
	 << SkymapCoordinates::Time      << endl;
    cout << "-- Coordinate for frequency axis : "
	 << SkymapCoordinates::Frequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[1] Retrieve the coordinate system object and inspect it" << endl;
  try {
    casa::CoordinateSystem csys = coord.coordinateSystem();

    show_CoordinateSystem (csys);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Retrieve the various coordinate axes" << endl;
  try {
    casa::CoordinateSystem csys = coord.coordinateSystem();

    cout << "-- Direction coordinates axis..." << endl;
    {
      DirectionCoordinate axis1 = csys.directionCoordinate (SkymapCoordinates::Direction);
      DirectionCoordinate axis2 = coord.directionAxis();
      
      cout << " -- Reference pixel (CRPIX) = "
	   << axis1.referencePixel() << "  <=>  " << axis2.referencePixel()
	   << endl;
      cout << " -- Reference value (CRVAL) = "
	   << axis1.referenceValue() << "  <=>  " << axis2.referenceValue()
	   << endl;
      cout << " -- Increment       (CDELT) = "
	   << axis1.increment() << "  <=>  " << axis2.increment()
	   << endl;
    }

    cout << "-- Distance coordinates axis..." << endl;
    {
      LinearCoordinate axis1 = csys.linearCoordinate (SkymapCoordinates::Distance);
      LinearCoordinate axis2 = coord.distanceAxis();
      
      cout << " -- Reference pixel (CRPIX) = "
	   << axis1.referencePixel() << "  <=>  " << axis2.referencePixel()
	   << endl;
      cout << " -- Reference value (CRVAL) = "
	   << axis1.referenceValue() << "  <=>  " << axis2.referenceValue()
	   << endl;
      cout << " -- Increment       (CDELT) = "
	   << axis1.increment() << "  <=>  " << axis2.increment()
	   << endl;
    }

    cout << "-- Time coordinates axis..." << endl;
    {
      LinearCoordinate axis1 = csys.linearCoordinate (SkymapCoordinates::Time);
      LinearCoordinate axis2 = coord.timeAxis();
      
      cout << " -- Reference pixel (CRPIX) = "
	   << axis1.referencePixel() << "  <=>  " << axis2.referencePixel()
	   << endl;
      cout << " -- Reference value (CRVAL) = "
	   << axis1.referenceValue() << "  <=>  " << axis2.referenceValue()
	   << endl;
      cout << " -- Increment       (CDELT) = "
	   << axis1.increment() << "  <=>  " << axis2.increment()
	   << endl;
    }

    cout << "-- Frequency coordinates axis..." << endl;
    {
      SpectralCoordinate axis1 = csys.spectralCoordinate (SkymapCoordinates::Frequency);
      SpectralCoordinate axis2 = coord.frequencyAxis();
      
      cout << " -- Reference pixel (CRPIX) = "
	   << axis1.referencePixel() << "  <=>  " << axis2.referencePixel()
	   << endl;
      cout << " -- Reference value (CRVAL) = "
	   << axis1.referenceValue() << "  <=>  " << axis2.referenceValue()
	   << endl;
      cout << " -- Increment       (CDELT) = "
	   << axis1.increment() << "  <=>  " << axis2.increment()
	   << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Update the coordinate axes ..." << endl;
  try {
    Double refPixel;
    Double refValue;
    Double increment;
    casa::CoordinateSystem csys = coord.coordinateSystem();

    // show the coordinate system before any manipulations
    cout << " - coordinate system before any manipulations" << endl;
    show_CoordinateSystem (csys);

    // (a) manipulate distance axis
    refPixel = 0.0;
    refValue = 100;
    increment = 50;

    status = coord.setDistanceAxis (refPixel,refValue,increment);

    if (status) {
      cout << " -  coordinate system after update of distance axis" << endl;
      csys = coord.coordinateSystem();
      show_CoordinateSystem (csys);
    } else {
      cout << " - Error updating the distance axis!" << endl;
    }

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Conversion from pixel to world coordinates..." << endl;
  try {
    uint nofPixels (10);
    Vector<Double> pixel (nofPixels);
    Vector<Double> world (nofPixels);

    // populate the vector with the pixel coordinates
    for (uint n(0); n<nofPixels; n++) {
      pixel(n) = 1.0*n;
    }

    // -- get values along the distance axis
    world = coord.distanceAxisValues (pixel);

    cout << "-- Pixel values             = " << pixel << endl;
    cout << "-- World values (Distance)  = " << world << endl;

    // the same result we should obtain after adjustment of the distance axis
    coord.setDistanceShape(nofPixels);

    world = coord.distanceAxisValues();
    cout << "-- World values (Distance)  = " << world << endl;

    // -- get the vaues along the time axis
    world = coord.timeAxisValues (pixel);
    cout << "-- World values (Time)      = " << world << endl;

    // -- get the vaues along the frequency axis
    world = coord.frequencyAxisValues (pixel);
    cout << "-- World values (Frequency) = " << world << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Retrival of coordinate values..." << endl;
  try {
    casa::Slice slice (0,5,1);
    Matrix<bool>   maskDirection;
    Matrix<Double> valDirection;
    Matrix<Double> valITRF;
    Vector<Double> valDistance;
    Vector<Double> valTime;
    Vector<Double> valFreq;

    coord.directionAxisValues("AZEL",valDirection,maskDirection,true);
    coord.directionAxisValues(MDirection::ITRF,valITRF,maskDirection,true);
    valDistance  = coord.distanceAxisValues();
    valTime      = coord.timeAxisValues();
    valFreq      = coord.frequencyAxisValues();

    cout << "-- World values (Direction) = " << valDirection.row(0) << endl;
    cout << "-- World values (ITRF)      = " << valITRF.row(0)     << endl;
    cout << "-- World values (Distance)  = " << valDistance(slice)  << endl;
    cout << "-- World values (Time)      = " << valTime(slice)      << endl;
    cout << "-- World values (Frequency) = " << valFreq(slice)      << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_mapProperties ()
{
  cout << "\n[test_mapProperties]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Adjust orientation of the map..." << endl;
  try {
    std::string top;
    std::string right;
    SkymapCoordinates coord = create_SkymapCoordinates ();

    // (North,East)
    coord.setMapOrientation (SkymapCoordinates::NORTH_EAST);
    coord.mapOrientation (top,right);
    cout << "-- New map orientation = (" << top << "," << right << ")" << endl;

    // (North,West)
    coord.setMapOrientation (SkymapCoordinates::NORTH_WEST);
    coord.mapOrientation (top,right);
    cout << "-- New map orientation = (" << top << "," << right << ")" << endl;

    // (South,East)
    coord.setMapOrientation (SkymapCoordinates::SOUTH_EAST);
    coord.mapOrientation (top,right);
    cout << "-- New map orientation = (" << top << "," << right << ")" << endl;

    // (South,West)
    coord.setMapOrientation (SkymapCoordinates::SOUTH_WEST);
    coord.mapOrientation (top,right);
    cout << "-- New map orientation = (" << top << "," << right << ")" << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Adjust the quantity stored/displayed in the map..." << endl;
  try {
    std::string domain;
    std::string quantity;
    SkymapCoordinates coord = create_SkymapCoordinates ();

    coord.setMapQuantity (SkymapCoordinates::TIME_FIELD);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setMapQuantity (SkymapCoordinates::TIME_POWER);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setMapQuantity (SkymapCoordinates::TIME_CC);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setMapQuantity (SkymapCoordinates::TIME_X);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setMapQuantity (SkymapCoordinates::FREQ_FIELD);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setMapQuantity (SkymapCoordinates::FREQ_POWER);
    coord.mapQuantity (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_imageCreation ()
{
  cout << "\n[test_imageCreation]\n" << endl;

  int nofFailedTests (0);

  cout << "-- creating SkymapCoordinates object..." << endl;
  SkymapCoordinates coord = create_SkymapCoordinates ();

  cout << "-- retrieve CoordinateSystem object..." << endl;
  CoordinateSystem csys = coord.coordinateSystem();

  cout << "-- get the shape of the image pixel array... " << std::flush;
  IPosition shape = coord.shape();
  TiledShape tile (shape);
  cout << shape << endl;
  
  cout << "-- creating the image file ... " << std::flush;
  PagedImage<Float> image (tile,
			   csys,
			   String("testimage.img"));
  cout << endl;
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_SkymapCoordinates ();
  }
  
  // Only continue if we actually can construct an object
  if (nofFailedTests == 0) {
    nofFailedTests += test_conversions ();
    nofFailedTests += test_coordinateSystem ();
    nofFailedTests += test_mapProperties ();
    nofFailedTests += test_imageCreation ();
  }
  
  return nofFailedTests;
}
