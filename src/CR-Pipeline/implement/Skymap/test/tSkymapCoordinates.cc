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

/* $Id: tSkymapCoordinates.cc,v 1.2 2007/04/05 16:42:26 bahren Exp $*/

#include <lopes/Skymap/SkymapCoordinates.h>

using std::cout;
using std::endl;

using casa::DirectionCoordinate;
using casa::LinearCoordinate;
using casa::SpectralCoordinate;

using LOPES::SkymapCoordinates;

/*!
  \file tSkymapCoordinates.cc

  \ingroup Skymap

  \brief A collection of test routines for LOPES::SkymapCoordinates
 
  \author Lars B&auml;hren
 
  \date 2007/03/13
*/

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
  ObservationData obsData ("LOFAR-ITS");  // Observation data
  uint nofBlocks (10);                    // Number of processed blocks
  
  // create SkymapCoordinates object
  SkymapCoordinates coord (timeFreq,
			   obsData,
			   nofBlocks,
			   SkymapCoordinates::NORTH_WEST,
			   SkymapCoordinates::TIME_CC);
  
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

    // get values along the distance axis
    world = coord.distanceAxisValues (pixel);

    cout << "-- Pixel values     = " << pixel << endl;
    cout << "-- World values     = " << world << endl;

    // the same result we should obtain after adjustment of the distance axis
    coord.setDistanceShape(nofPixels);

    world = coord.distanceAxisValues();

    cout << "-- World values     = " << world << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
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
  }
  
  return nofFailedTests;
}
