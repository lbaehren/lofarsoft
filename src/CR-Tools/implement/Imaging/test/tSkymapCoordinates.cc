/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <crtools.h>
#include <Imaging/SkymapCoordinates.h>

#include <images/Images/PagedImage.h>
#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

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

  \ingroup CR_Imaging

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
  unsigned int blocksize (1024);
  double sampleFrequency (40e6);
  unsigned int nyquistZone (1);
  TimeFreq timeFreq (blocksize,sampleFrequency,nyquistZone);
  ObservationData obsData ("WSRT");       // Observation data
  uint nofBlocks (1);                     // Number of processed blocks
  
  // create SkymapCoordinates object
  SkymapCoordinates coord (timeFreq,
			   obsData,
			   nofBlocks,
			   SkymapCoordinates::NORTH_WEST,
			   CR::TIME_CC);

  return coord;
}

// -----------------------------------------------------------------------------

/*!
  \brief Provide a summary of a Coordinate object 

  Example output:
  \verbatim
  -- World axis names        = [Longitude, Latitude]
  -- World axis units        = [rad, rad]
  -- Reference pixel (CRPIX) = [60, 60]
  -- Reference value (CRVAL) = [0, 0.0274156]
  -- Increment       (CDELT) = [-0.0349066, 0.0349066]
  \endverbatim
*/
void show_Coordinate (casa::Coordinate &coord)
{
  cout << "-- World axis names        = " << coord.worldAxisNames() << endl;
  cout << "-- World axis units        = " << coord.worldAxisUnits() << endl;
  cout << "-- Reference pixel (CRPIX) = " << coord.referencePixel() << endl;
  cout << "-- Reference value (CRVAL) = " << coord.referenceValue() << endl;
  cout << "-- Increment       (CDELT) = " << coord.increment()      << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Provide a summary of a CoordinateSystem object

  Example output:
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
  \brief Export the direction axis values to an ASCII table

  For plotting with Gnuplot use e.g.:
  \code
  set grid
  set xlabel 'Longitude'
  set ylabel 'Latitude'
  plot 'directions-azel.data' u 3:4 w d t 'AZEL - CAR'
  \endcode
  To get postscript output:
  \code
  set terminal post color solid
  set output 'directions-azel-car.ps'
  replot
  \endcode

  \param filename   -- Name of the file, to which the data will be written
  \param directions -- [nofDirections,2]
  \param mask       -- [nofDirections,2]
*/
void export_DirectionAxisValues (std::string const &filename,
				 casa::Matrix<double> const &directions,
				 casa::Matrix<bool> const &mask)
{
  int n(0);
  int nx(0);
  int ny(0);
  casa::IPosition shapeDirections (directions.shape());
  casa::IPosition shapeMask (mask.shape());
  std::ofstream outfile;

  if (shapeDirections(0) == shapeMask(0)*shapeMask(1)) {
    outfile.open(filename.c_str());
    
    cout << "--> exporting direction axis values ... " << std::flush;
    for (nx=0; nx<shapeMask(0); nx++) {
      for (ny=0; ny<shapeMask(1); ny++) {
	outfile << "\t" << nx << "\t" << ny
		<< "\t" << directions(n,0) << "\t" << directions(n,1)
		<< "\t" << mask(nx,ny)
		<< endl;
	n++;
      }
    }
    cout << "done" << endl;
    
    outfile.close();
  } else {
    cerr << "[export_DirectionAxisValues] Mismatching array shapes!" << endl;
    cerr << "-- Direction values = " << shapeDirections << endl;
    cerr << "-- Direction mask   = " << shapeMask << endl;
  }
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
			     nofBlocks,
			     SkymapCoordinates::NORTH_WEST,
			     CR::TIME_CC);
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing copy constructor ..." << endl;
  try {
    // create SkymapCoordinates object
    SkymapCoordinates coord1 (timeFreq,
			      obsData,
			      nofBlocks,
			      SkymapCoordinates::NORTH_WEST,
			      CR::FREQ_POWER);
    // create new object from the previouse one
    SkymapCoordinates coord2 (coord1);
    // compare parameters
    std::cout << "-- pixel array : "
	      << coord1.shape()
	      << " -> "
	      << coord2.shape()
	      << std::endl;
    std::cout << "-- beam type   : "
	      << coord1.beamType() 
	      << " -> "
	      << coord2.beamType()
	      << std::endl;
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
			   CR::TIME_CC);
  
  cout << "[0] Check the numbering of the image axes..." << endl;
  try {
    cout << "-- Coordinate for direction axes : "
	 << SkymapCoordinates::Direction << endl;
    cout << "-- Coordinate for distance axis  : "
	 << SkymapCoordinates::Distance  << endl;
    cout << "-- Coordinate for time axis      : "
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
    valDistance = coord.distanceAxisValues();
    valTime     = coord.timeAxisValues();
    valFreq     = coord.frequencyAxisValues();

    cout << "-- World values (Direction) = " << valDirection.row(0) << endl;
    cout << "-- World values (ITRF)      = " << valITRF.row(0)      << endl;
    cout << "-- World values (Distance)  = " << valDistance(slice)  << endl;
    cout << "-- World values (Time)      = " << valTime(slice)      << endl;
    cout << "-- World values (Frequency) = " << valFreq(0)          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with and manipulation of the direction axis of the image

  In order to validate the internally constructed conversion engine, we use known
  positions of a source (here: the Sun).
  <ul>
    <li>J2000    : (97.5777684,23.4197344)
    <li>B1950    : (86.8167851,23.4061253)
    <li>GALACTIC : (-174.732126,-1.94879617)
  </ul>
  The exported direction values can be displayed using Gnuplot via:
  \code
  set grid
  set xlabel 'Longitude'
  set ylabel 'Latitude'
  plot 'directions-azel.data' u 3:4 w d t 'AZEL', 'directions-j2000.data' u 3:4 w d t 'J2000', 'directions-galactic.data' u 3:4 w d t 'GALACTIC'
  \endcode
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_directionAxis ()
{
  cout << "\n[test_directionAxis]\n" << endl;

  int nofFailedTests (0);
  bool status (true);
  std::string refcode ("AZEL");
  std::string projection ("CAR");
  unsigned int nofAxes (2);
  bool anglesInDegrees (true);
  SkymapCoordinates coord (create_SkymapCoordinates ());
  
  cout << "[1] Retrival of the DirectionCoordinate..." << endl;
  try {
    casa::DirectionCoordinate dc = coord.directionAxis();
    show_Coordinate (dc);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Setting direction coordinate from parameters..." << endl;
  try {
    Vector<double> refValue (nofAxes);
    Vector<double> increment (nofAxes);
    IPosition shape (nofAxes);
    bool anglesInDegree (true);

    refValue(0) = 179.0;
    refValue(1) = 028.0;

    increment(0) = -0.5;
    increment(1) = +0.5;

    shape(0) = 100;
    shape(1) = 150;

    // assign new direction coordinate ...
    status = coord.setDirectionAxis (refcode,
				     projection,
				     refValue,
				     increment,
				     shape,
				     anglesInDegree);
    // ... and show its properties
    if (status) {
      coord.summary();
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Retrieval of the coordinate values..." << endl;
  try {
    casa::Matrix<double> directions;
    casa::Matrix<bool> mask;
    anglesInDegrees = true;

    cout << "-- retrieving directions in AZEL coordinates ..." << endl;

    status = coord.directionAxisValues ("AZEL",
					directions,
					mask,
					anglesInDegrees);

    if (status) {
      export_DirectionAxisValues ("directions-azel.data",
				  directions,
				  mask);
    }

    cout << "-- retrieving directions in J2000 coordinates ..." << endl;

    status = coord.directionAxisValues ("J2000",
					directions,
					mask,
					anglesInDegrees);

    if (status) {
      export_DirectionAxisValues ("directions-j2000.data",
				  directions,
				  mask);
    }

    cout << "-- retrieving directions in GALACTIC coordinates ..." << endl;

    status = coord.directionAxisValues ("GALACTIC",
					directions,
					mask,
					anglesInDegrees);

    if (status) {
      export_DirectionAxisValues ("directions-galactic.data",
				  directions,
				  mask);
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with settings handling the properties of the created map
  
  \return nofFailedTests -- The number of failed tests.
*/
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

    coord.setBeamType (CR::TIME_FIELD);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setBeamType (CR::TIME_POWER);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setBeamType (CR::TIME_CC);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setBeamType (CR::TIME_X);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setBeamType (CR::FREQ_FIELD);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

    coord.setBeamType (CR::FREQ_POWER);
    coord.beamType (domain,quantity);
    cout << "-- New map quantity = (" << domain << "," << quantity << ")" << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test creation of an image using the generated coordinate information

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_imageCreation ()
{
  cout << "\n[test_imageCreation]\n" << endl;

  int nofFailedTests (0);
  SkymapCoordinates coord = create_SkymapCoordinates ();
  
  cout << "[1] Creating image from default settings ..." << endl;
  try {
    
    cout << "-- retrieve CoordinateSystem object..." << endl;
    CoordinateSystem csys = coord.coordinateSystem();
    
    cout << "-- get the shape of the image pixel array... " << std::flush;
    IPosition shape = coord.shape();
    TiledShape tile (shape);
    cout << shape << endl;
    
    cout << "-- creating the image file ... " << std::flush;
#ifdef HAVE_HDF5
    casa::HDF5Image<Float> image (tile,
				  csys,
				  String("testimage1.h5"));
#else
    PagedImage<Float> image (tile,
			     csys,
			     String("testimage1.img"));
#endif
    cout << "done" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Creating image from user defined settings ..." << endl;
  try {
    bool status (true);
    unsigned int nofAxes (2);
    Vector<double> refValue (nofAxes);
    Vector<double> increment (nofAxes);
    IPosition shape (nofAxes);
    bool anglesInDegree (true);
    
    refValue(0) = 179.0;
    refValue(1) = 028.0;
    
    increment(0) = -0.25;
    increment(1) = +0.25;
    
    shape(0) = 200;
    shape(1) = 200;
    
    // assign new direction coordinate ...
    status = coord.setDirectionAxis ("AZEL",
				     "STG",
				     refValue,
				     increment,
				     shape,
				     anglesInDegree);
    
    if (status) {
      CoordinateSystem csys = coord.coordinateSystem();
      
      cout << "-- get the shape of the image pixel array... " << std::flush;
      IPosition shape = coord.shape();
      TiledShape tile (shape);
      cout << shape << endl;
      
      cout << "-- creating the image file ... " << std::flush;
      PagedImage<Float> image (tile,
			       csys,
			       String("testimage2.img"));
      cout << "done" << endl;
    }
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
    nofFailedTests += test_mapProperties ();
    nofFailedTests += test_coordinateSystem ();
    nofFailedTests += test_directionAxis ();
    nofFailedTests += test_imageCreation ();
  }
  
  return nofFailedTests;
}
