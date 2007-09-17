
/* $Id$ */

#include <iostream>
#include <fstream>

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>

#include <coordinates/Coordinates/ObsInfo.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

#include <Imaging/SkymapGrid.h>

using std::cerr;
using std::cout;
using std::endl;
using std::flush;

using casa::uInt;
using casa::Matrix;
using casa::String;
using casa::Vector;

using CR::ObservationData;
using CR::SkymapGrid;

/*!
  \file tSkymapGrid.cc

  \ingroup Imaging

  \brief A collection of test for SkymapGrid

  \author Lars B&auml;hren

  \date 2005/04/08

  <h3>Usage</h3>

  Running the test program will create a number of data files, containing the
  position values of the SkymapGrid nodes.

  \verbatim
  set grid
  # coordinate grid in celestial reference frame
  plot 'tSkymapGrid-grid-1.log' u 2:3
  # coordinate grid in local reference frame
  plot 'tSkymapGrid-grid-2.log' u 4:5
  \endverbatim
*/

// ------------------------------------------------------------------------------

/*!
  \brief Add timestamp information the the name of the logfile

  \verbatim
  <basename>-<yyyy.mm.dd>-<hh.mm>.log
  \endverbatim

  \param basename -- Basename to which to which the timestamp information is
         appended.

  \return filename -- The filename including the timestamp information
 */
String time2filename (String basename) {

  Time sysTime;
  std::ostringstream filename;
  uInt month;
  uInt dayOfMonth;
  uInt hours;
  uInt minutes;

  // Set the base for the filename
  if (basename == "") {
    basename = "tSkymapGrid-";
  } 

  month = sysTime.month();
  dayOfMonth = sysTime.dayOfMonth();
  hours = sysTime.hours();
  minutes = sysTime.minutes();

  filename << basename << "-" << sysTime.year() << ".";
  //
  if (month < 10) filename << "0";
  filename << month << ".";
  //
  if (dayOfMonth < 10) filename << "0";
  filename << dayOfMonth << "_";
  //
  if (hours < 10) filename << "0";
  filename << sysTime.hours() << ":";
  //
  if (minutes < 10) filename << "0";
  filename << minutes << ".log";
  
  return filename.str();
}

// ------------------------------------------------------------------------------

/*!
  \brief Test working with a CASA DirectionCoordinate object

  \return nofFailedTests -- The number of failed tests
*/
int test_DirectionCoordinate ()
{
  int nofFailedTests (0);
  bool ok (true);
  Matrix<double> xform(2,2);
  xform = 0.0; xform.diagonal() = 1.0;

  DirectionCoordinate radec (MDirection::AZEL,
			     Projection(Projection::STG),
			     0*casa::C::pi/180.0,
			     90*casa::C::pi/180.0,
			     -1*casa::C::pi/180.0,
			     1*casa::C::pi/180,
			     xform,
			     128,
			     128);

  Vector<double> pixel (radec.referencePixel());
  Vector<double> world (radec.referenceValue());

  ok = radec.toWorld (world,pixel);

  if (ok) {
    cout << pixel << " -> " << world/casa::C::pi*180.0 << endl;
  } else {
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ---------------------------------------------------- test_CoordinateConversion

/*!
  \brief Test the coordinate conversion provided by the direction coordinate

  The conversion between world and pixel coordinates can be carried out without
  the construction of a coordinate system tool, since all required transformation
  operators are already contained with a DirectionCoordinate object.

  \param dir -- A DirectionCoordinate object
*/
void test_CoordinateConversion (const DirectionCoordinate& dir)
{
  Vector<double> referencePixel(dir.referencePixel());
  Vector<double> referenceValue(dir.referenceValue());
  Vector<double> increment(dir.increment());

  cout << "\n[test_CoordinateConversion]\n" << endl;
  cout << " - Reference pixel      = " << referencePixel << endl;
  cout << " - Reference value      = " << referenceValue << endl;
  cout << " - Coordinate increment = " << increment << endl;

  // coordinate conversion test
  {
    bool ok (true);
    Vector<double> pixel (referencePixel);
    Vector<double> world (referenceValue);

    ok = dir.toWorld (world,pixel);
    cout << " - Coordinate conversion : " << endl;
    cout << "\t" << pixel << " -> " << world;
    ok = dir.toPixel (pixel,world);
    cout << " -> " << pixel << endl;
    //
    world = 0.0;
    ok = dir.toPixel (pixel,world);
    cout << "\t" << world << " -> " << pixel;
    ok = dir.toWorld (world,pixel);
    cout << " -> " << world << endl;
    //
    world(0) = 178.82;
    world(1) = 28.02;
    world *= casa::C::pi/180.0;
    ok = dir.toPixel (pixel,world);
    cout << "\t" << world << " -> " << pixel;
    ok = dir.toWorld (world,pixel);
    cout << " -> " << world << endl;
  }
  
}

// =============================================================================

/*!
  \brief Test manipulation of a CoordinateSystem object.

  Since SkymapGrid internally uses the functionality of the AIPS++
  CoordinateSystem class, we perform a number of tests here, to make sure
  we understand how it works.

  \return ok -- Status of the test routine.
 */
bool test_CoordinateSystem () 
{
  cout << "\n[tSkymapGrid::test_CoordinateSystem]" << endl;
  
  Vector<String> refcode (3);
  Vector<String> projection (4);
  IPosition shape(2);
  Matrix<double> xform(2,2);

  // Direction reference codes to be tested.
  refcode(0) = "J2000";
  refcode(1) = "GALACTIC";
  refcode(2) = "B1950";

  // Map projections to be tested.
  projection(0) = "STG";
  projection(1) = "TAN";
  projection(2) = "CAR";
  projection(3) = "AIT";
  
  shape = 100;
  
  xform = 0.0;
  xform.diagonal() = 1.0;
  
  // Test 1 : Add direction coordinate to empty coordinate system
  cout << "\n [1] Coordinate system with direct asignment of MDirection ..."
       << endl;
  {
    CoordinateSystem cs;
    //
    DirectionCoordinate dir (MDirection::AZEL,            // Direction
			     Projection(Projection::STG), // Projection
			     0*casa::C::pi/180.0,               // CRVAL(0)
			     90*casa::C::pi/180.0,              // CRVAL(1)
			     1*casa::C::pi/180.0,               // CDELT(0)
			     1*casa::C::pi/180,                 // CDELT(1)
			     xform,                       // PC(i,j)
			     shape(0)/2+1,                // CRPIX(0)
			     shape(1)/2+1);               // CRPIX(1)
    //
    test_CoordinateConversion (dir);
    
    cout << " -- Number of coordinates : " << cs.nCoordinates();
    cs.addCoordinate (dir);
    cout << " -> " << cs.nCoordinates() << endl;
    //
    cout << " -- Axis names .......... : "
	 << dir.axisNames(dir.directionType(),false) << endl;
  }

  // Test 2 : Change the direction type 
  cout << "\n [2] Coordinate system with asignment of MDirection via refcode ..."
       << endl;
  for (uInt numRefcode=0; numRefcode<refcode.nelements(); numRefcode++) {
    bool ok;
    MDirection md;
    MDirection::Types tp;
    CoordinateSystem cs;
    
    ok = md.getType(tp,refcode(numRefcode));
    ok = md.setRefString (refcode(numRefcode));
    MDirection md1 (tp);
    //
    md = md1;
    //
    cout << " -- MDirection type : " << tp << " (" << md.getRefString()
	 << ")" << endl;
    //
    cout << " -- Creating direction coordinate ... " << flush;
    DirectionCoordinate dir (tp,                          // Direction
			     Projection(Projection::STG), // Projection
			     0*casa::C::pi/180.0,               // CRVAL(0)
			     90*casa::C::pi/180.0,              // CRVAL(1)
			     -1*casa::C::pi/180.0,              // CDELT(0)
			     1*casa::C::pi/180,                 // CDELT(1)
			     xform,                       // PC(i,j)
			     shape(0)/2+1,                // CRPIX(0)
			     shape(1)/2+1);               // CRPIX(1)  
    cout << "ok" << endl;

    cs.addCoordinate (dir);
  }
  
  // Test 3 : Change the projection
  cout << "\n [3] Coordinate system with asignment of Projection refcode ..."
       << endl;
  for (uInt n=0; n<projection.nelements(); n++) {
    {
      Projection proj;
      Projection::Type tp;
      bool isZenithal (false);
      
      tp = proj.type(projection(n));
      isZenithal = proj.isZenithal(tp);
      cout << " -- Map projection ....... : " << proj.name(tp)
	   << " (" << tp << ")" << endl;
      cout << " -- Projection is zenithal : " << isZenithal << endl;
      
      cout << " -- Creating direction coordinate ... " << flush;
      DirectionCoordinate dir (MDirection::AZEL,            // Direction
			       Projection(tp),              // Projection
			       0*casa::C::pi/180.0,         // CRVAL(0)
			       90*casa::C::pi/180.0,        // CRVAL(1)
			       -1*casa::C::pi/180.0,        // CDELT(0)
			       1*casa::C::pi/180,           // CDELT(1)
			       xform,                       // PC(i,j)
			       shape(0)/2+1,                // CRPIX(0)
			       shape(1)/2+1);               // CRPIX(1)  
      cout << "ok" << endl;
    }
  }

  return true;
}

// =============================================================================

/*!
  \brief Test SkymapGrid object constructors.

  This routine will construct a SkymapGrid object using each of the available
  methods.

  \return ok -- Status of the test routine.
 */
bool test_SkymapGrid ()
{
  cout << "\n[tSkymapGrid::test_SkymapGrid]" << endl;

  bool ok (true);

  // Test 1 : Default constructor
  cout << "\n [1] Testing default constructor ..." << endl;
  {
    SkymapGrid sg;
    //
    sg.show (std::cout);
  }

  // Test 2 : Argumented constructor
  cout << "\n [2] Testing argumented constructor ... " << endl;
  {
    ObsInfo obsInfo;

    obsInfo.setObserver ("LOPES-Tools programmer");
    obsInfo.setTelescope ("LOFAR-ITS");

    SkymapGrid sg (obsInfo);
    //
    sg.show (std::cout);
  }

  // Test 3 : Argumented constructor
  cout << "\n [3] Testing argumented constructor ... " << endl;
  {
    String refcode ("J2000");
    String projection ("TAN");
    IPosition shape (2);
    Vector<double> center (2);
    Vector<double> increment (2);

    shape = 100;
    //
    center(0) = 178;
    center(1) = 28;
    //
    increment = 1.0;

    SkymapGrid sg (refcode, projection, shape, center, increment);
    //
    sg.show (std::cout);
  }

  // Test 4 : Argumented constructor
  cout << "\n [4] Testing argumented constructor ... " << endl;
  {
    String refcode ("GALACTIC");
    String projection ("AIT");
    IPosition shape (2);
    Vector<double> center (2);
    Vector<double> increment (2);
    ObsInfo obsInfo;

    obsInfo.setObserver ("LOPES-Tools programmer");
    obsInfo.setTelescope ("LOFAR-ITS");
    //
    shape(0) = 360;
    shape(1) = 180;
    //
    center(0) = 178;
    center(1) = 28;
    //
    increment = 1.0;

    SkymapGrid sg (obsInfo, refcode, projection, shape, center, increment);
    //
    sg.show (std::cout);
  }

  cout << "\n [#] Tests completed" << endl;

  return ok;
}

// =============================================================================

/*!
  \brief Test operators defined for a SkymapGrid object

  \return ok -- Status of the test routine.
 */
bool test_operators ()
{
  cout << "\n[tSkymapGrid::test_operators]" << endl;

  bool ok (true);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<double> center (2);
  Vector<double> increment (2);
  
  shape = 20;
  //
  center(0) = 178;
  center(1) = 28;
  //
  increment = 2.0;

  // Test 1: Copy operator
  cout << "\n [1] Copy operator for SkymapGrid" << endl;
  {
    SkymapGrid sg1 (refcode, projection, shape, center, increment);
    //
    refcode = "GALACTIC";
    projection = "AIT";
    SkymapGrid sg2 (refcode, projection, shape, center, increment);
    //
    sg1.show (std::cout);
    sg2.show (std::cout);
    //
    try {
      sg1 = sg2;
    } catch (AipsError x) {
      cerr << "[tSkymapGrid::test_operators] " << x.getMesg() << endl;
      ok = false;
    }
    //
    cout << " - SkymapGrid after copy operation :" << endl;
    sg1.show (std::cout);
  }

  cout << "\n [#] Tests completed" << endl;

  return ok;
}

// =============================================================================

/*!
  \brief Test the generation and extraction of the coordinate grid

  Get the coordinates of the grid nodes.

  \return ok -- Status of the test routine.
 */
bool test_grid ()
{
  cout << "\n[tSkymapGrid::test_grid]" << endl;

  bool ok (true);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<double> center (2);
  Vector<double> increment (2);
  
  shape = 20;
  //
  center(0) = 178;
  center(1) = 28;
  //
  increment = 2.0;
  
  // Test 1 : Retrieve celestial grid via standard interface
  cout << "\n [1] Retrieve default celestial grid via standard interface" << endl;
  {
    SkymapGrid sg;
    //
    sg.show (std::cout);
    //
    Matrix<double> grid = sg.grid();
    IPosition shape (grid.shape());
    //
    String filename (time2filename("tSkymapGrid-grid1"));
    std::ofstream logfile (filename.c_str(),std::ios::out);
    //
    for (int pix=0; pix<shape(0); pix++) {
      logfile << pix;
      for (int coord=0; coord<shape(1); coord++) {
	logfile << "\t" << grid(pix,coord);
      }
      logfile << endl;
    }
    logfile.close();
  }
  
  // Test 2 : Retrieve unmasked part of the default grid
  cout << "\n [2] Retrieve unmasked part of the default grid" << endl;
  {
    SkymapGrid sg;
    //
    sg.show (std::cout);
    //
    Matrix<double> grid = sg.grid(true);
    Matrix<double> azel = sg.azel(true);
    IPosition shape (grid.shape());
    //
    String filename (time2filename("tSkymapGrid-grid2"));
    std::ofstream logfile (filename.c_str(),std::ios::out);
    //
    for (int pix=0; pix<shape(0); pix++) {
      logfile << pix;
      for (int coord=0; coord<shape(1); coord++) {
	logfile << "\t" << grid(pix,coord);
      }
      for (int coord=0; coord<shape(1); coord++) {
	logfile << "\t" << azel(pix,coord);
      }
      logfile << endl;
    }
    logfile.close();
  }
  
  // Test 3 : Retrieve coordinate grid selectively
  cout << "\n [3] Retrieve coordinate grid selectively" << endl;
  try {
    bool which (true);
    //
    SkymapGrid sg (refcode, projection, shape, center, increment);
    //
    Matrix<double> grid = sg.grid(which);
    Matrix<double> azel = sg.azel(which);
    IPosition shape (grid.shape());
    //
    sg.show (std::cout);
    //
    String filename (time2filename("tSkymapGrid-grid3"));
    std::ofstream logfile (filename.c_str(),std::ios::out);
    //
    for (int pix=0; pix<shape(0); pix++) {
      logfile << pix;
      for (int coord=0; coord<shape(1); coord++) {
	logfile << "\t" << grid(pix,coord);
      }
      for (int coord=0; coord<shape(1); coord++) {
	logfile << "\t" << azel(pix,coord);
      }
      logfile << endl;
    }
    logfile.close();
  } catch (AipsError x) {
    cerr << "[tSkymapGrid::test_grid] " << x.getMesg() << endl;
    ok = false;
  }

  // Test 4 : Retrieve grid nodes within a given elevation range
  cout << "\n [4] Retrieve grid nodes within a given elevation range" << endl;
  try {
    bool which (true);
    Vector<double> elevation(2);
    // 
    elevation(0) = 20.0;
    elevation(1) = 50.0;
    //
    refcode = "GALACTIC";
    //
    SkymapGrid sg (refcode,
		   projection,
		   shape,
		   center,
		   increment);
    //
    sg.setElevationRange (elevation);
    //
    Matrix<double> grid = sg.grid(which);
    Matrix<double> azel = sg.azel(which);
    IPosition shape (grid.shape());
    //
    sg.show (std::cout);
    //
    String filename (time2filename("tSkymapGrid-grid4"));
    std::ofstream logfile (filename.c_str(),std::ios::out);
    //
    int pix (0);
    int coord (0);
    //
    for (pix=0; pix<shape(0); pix++) {
      logfile << pix;
      for (coord=0; coord<shape(1); coord++) {
	logfile << "\t" << grid(pix,coord);
      }
      for (coord=0; coord<shape(1); coord++) {
	logfile << "\t" << azel(pix,coord);
      }
      logfile << endl;
    }
    //
    logfile.close();
  } catch (AipsError x) {
    cerr << "[tSkymapGrid::test_grid] " << x.getMesg() << endl;
    ok = false;
  }

  // Test 5 : Check for dependency of the local coordinate grid on the choice
  //          of the celestial reference frame.
  cout << "\n [5] Dependency of local grid on celestial reference frame" << endl;
  try {
    bool which (true);
    Quantity epoch (52940.4624,"d");
    String telescope ("LOPES");
    ObservationData obsData (epoch,telescope);
    ObsInfo obsInfo (obsData.obsInfo());
    Vector<double> sunJ2000 (2);
    Vector<double> sunGALACTIC (2);
    //
    sunJ2000(0) = -147.700631;
    sunJ2000(1) = -13.0431452;
    //
    sunGALACTIC(0) = -29.4510056;
    sunGALACTIC(1) = 45.6177395;
    //
    increment = 0.5;
    //
    refcode = "J2000";
    SkymapGrid sgJ2000 (obsInfo,refcode, projection, shape, sunJ2000, increment);
    Matrix<double> gridJ2000 = sgJ2000.grid(which);
    Matrix<double> azelJ2000 = sgJ2000.azel(which);
    IPosition shapeJ2000 (gridJ2000.shape());
    //
    refcode = "GALACTIC";
    SkymapGrid sgGALACTIC (obsInfo,refcode, projection, shape, sunGALACTIC, increment);
    Matrix<double> gridGALACTIC = sgGALACTIC.grid(which);
    Matrix<double> azelGALACTIC = sgGALACTIC.azel(which);
    IPosition shapeGALACTIC (gridGALACTIC.shape());
    //
    cout << " - Grid shape J2000 .. : " << shapeJ2000 << endl;
    cout << " - Grid shape GALACTIC : " << shapeGALACTIC << endl;
    //
    if (shapeJ2000 == shapeGALACTIC) {
      String filename (time2filename("tSkymapGrid-grid5"));
      std::ofstream logfile (filename.c_str(),std::ios::out);
      //
      for (int pix=0; pix<shapeJ2000(0); pix++) {
	logfile << pix;
	// J2000 based grid
	for (int coord=0; coord<shapeJ2000(1); coord++) {
	  logfile << "\t" << gridJ2000(pix,coord);
	}
	for (int coord=0; coord<shapeJ2000(1); coord++) {
	  logfile << "\t" << azelJ2000(pix,coord);
	}
	// GALACTIC based grid
	for (int coord=0; coord<shapeJ2000(1); coord++) {
	  logfile << "\t" << gridGALACTIC(pix,coord);
	}
	for (int coord=0; coord<shapeJ2000(1); coord++) {
	  logfile << "\t" << azelGALACTIC(pix,coord);
	}
	logfile << endl;
      }
      //
      logfile.close();
    }
  } catch (AipsError x) {
    cerr << "[tSkymapGrid::test_grid] " << x.getMesg() << endl;
    ok = false;
  }

  cout << "\n [#] Tests completed" << endl;

  return ok;
}

// =============================================================================

/*!
  \brief Test the manipulation of the map orientation

  \return ok -- Status of the test routine.
 */
bool test_orientation ()
{
  bool ok (true);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<double> center (2);
  Vector<double> increment (2);
  Vector<String> orientation(2);
  
  shape = 50;
  //
  center(0) = 178;
  center(1) = 28;
  //
  increment = 0.5;
  
  SkymapGrid sg (refcode, projection, shape, center, increment);
  
  // Test 1: [East,North]
  {
    orientation(0) = "East";
    orientation(1) = "North";
    //
    sg.setOrientation (orientation);
    //
    Matrix<double> grid = sg.grid(true);
  }

  // Test 2: [East,South]
  {
    orientation(0) = "East";
    orientation(1) = "South";
    //
    sg.setOrientation (orientation);
    //
    Matrix<double> grid = sg.grid(true);
  }

  // Test 3: [West,North]
  {
    orientation(0) = "West";
    orientation(1) = "North";
    //
    sg.setOrientation (orientation);
    //
    Matrix<double> grid = sg.grid(true);
  }

  // Test 4: [West,South]
  {
    orientation(0) = "West";
    orientation(1) = "South";
    //
    sg.setOrientation (orientation);
    //
    Matrix<double> grid = sg.grid(true);
  }

  return ok;
}

// =============================================================================

int main () {

  int nofFailedTests (0);

  {
    nofFailedTests += test_DirectionCoordinate ();
  }

//   {
//     ok = test_CoordinateSystem ();
//   }

//   {
//     ok = test_SkymapGrid ();
//   }

//   {
//     ok = test_operators ();
//   }

//   {
//     ok = test_grid ();
//   }

  return nofFailedTests;

}
