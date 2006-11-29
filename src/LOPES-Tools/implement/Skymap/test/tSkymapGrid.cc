
/* $Id: tSkymapGrid.cc,v 1.7 2006/08/10 15:27:55 bahren Exp $ */

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

#include <lopes/Skymap/SkymapGrid.h>

/*!
  \file tSkymapGrid.cc

  \ingroup Skymap

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

// =============================================================================

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
  ostringstream filename;
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

Bool test_DirectionCoordinate ()
{
  Bool ok (True);
  Matrix<Double> xform(2,2);
  xform = 0.0; xform.diagonal() = 1.0;

  DirectionCoordinate radec (MDirection::AZEL,
			     Projection(Projection::STG),
			     0*C::pi/180.0,
			     90*C::pi/180.0,
			     -1*C::pi/180.0,
			     1*C::pi/180,
			     xform,
			     128,
			     128);

  Vector<Double> pixel (radec.referencePixel());
  Vector<Double> world (radec.referenceValue());

  ok = radec.toWorld (world,pixel);
  
  cout << pixel << " -> " << world/C::pi*180.0 << endl;

  return ok;
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
  Vector<Double> referencePixel(dir.referencePixel());
  Vector<Double> referenceValue(dir.referenceValue());
  Vector<Double> increment(dir.increment());

  cout << "\n[test_CoordinateConversion]\n" << endl;
  cout << " - Reference pixel      = " << referencePixel << endl;
  cout << " - Reference value      = " << referenceValue << endl;
  cout << " - Coordinate increment = " << increment << endl;

  // coordinate conversion test
  {
    Bool ok (True);
    Vector<Double> pixel (referencePixel);
    Vector<Double> world (referenceValue);

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
    world *= C::pi/180.0;
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
Bool test_CoordinateSystem () 
{
  cout << "\n[tSkymapGrid::test_CoordinateSystem]" << endl;
  
  Vector<String> refcode (3);
  Vector<String> projection (4);
  IPosition shape(2);
  Matrix<Double> xform(2,2);

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
			     0*C::pi/180.0,               // CRVAL(0)
			     90*C::pi/180.0,              // CRVAL(1)
			     1*C::pi/180.0,               // CDELT(0)
			     1*C::pi/180,                 // CDELT(1)
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
	 << dir.axisNames(dir.directionType(),False) << endl;
  }

  // Test 2 : Change the direction type 
  cout << "\n [2] Coordinate system with asignment of MDirection via refcode ..."
       << endl;
  for (uInt numRefcode=0; numRefcode<refcode.nelements(); numRefcode++) {
    Bool ok;
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
			     0*C::pi/180.0,               // CRVAL(0)
			     90*C::pi/180.0,              // CRVAL(1)
			     -1*C::pi/180.0,              // CDELT(0)
			     1*C::pi/180,                 // CDELT(1)
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
      Bool isZenithal (False);
      
      tp = proj.type(projection(n));
      isZenithal = proj.isZenithal(tp);
      cout << " -- Map projection ....... : " << proj.name(tp)
	   << " (" << tp << ")" << endl;
      cout << " -- Projection is zenithal : " << isZenithal << endl;
      
      cout << " -- Creating direction coordinate ... " << flush;
      DirectionCoordinate dir (MDirection::AZEL,            // Direction
			       Projection(tp),              // Projection
			       0*C::pi/180.0,               // CRVAL(0)
			       90*C::pi/180.0,              // CRVAL(1)
			       -1*C::pi/180.0,              // CDELT(0)
			       1*C::pi/180,                 // CDELT(1)
			       xform,                       // PC(i,j)
			       shape(0)/2+1,                // CRPIX(0)
			       shape(1)/2+1);               // CRPIX(1)  
      cout << "ok" << endl;
    }
  }

  return True;
}

// =============================================================================

/*!
  \brief Test SkymapGrid object constructors.

  This routine will construct a SkymapGrid object using each of the available
  methods.

  \return ok -- Status of the test routine.
 */
Bool test_SkymapGrid ()
{
  cout << "\n[tSkymapGrid::test_SkymapGrid]" << endl;

  Bool ok (True);

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
    Vector<Double> center (2);
    Vector<Double> increment (2);

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
    Vector<Double> center (2);
    Vector<Double> increment (2);
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
Bool test_operators ()
{
  cout << "\n[tSkymapGrid::test_operators]" << endl;

  Bool ok (True);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<Double> center (2);
  Vector<Double> increment (2);
  
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
      ok = False;
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
Bool test_grid ()
{
  cout << "\n[tSkymapGrid::test_grid]" << endl;

  Bool ok (True);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<Double> center (2);
  Vector<Double> increment (2);
  
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
    Matrix<Double> grid = sg.grid();
    IPosition shape (grid.shape());
    //
    String filename (time2filename("tSkymapGrid-grid1"));
    std::ofstream logfile (filename.c_str(),ios::out);
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
    Matrix<Double> grid = sg.grid(True);
    Matrix<Double> azel = sg.azel(True);
    IPosition shape (grid.shape());
    //
    String filename (time2filename("tSkymapGrid-grid2"));
    ofstream logfile (filename.c_str(),ios::out);
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
    Bool which (True);
    //
    SkymapGrid sg (refcode, projection, shape, center, increment);
    //
    Matrix<Double> grid = sg.grid(which);
    Matrix<Double> azel = sg.azel(which);
    IPosition shape (grid.shape());
    //
    sg.show (std::cout);
    //
    String filename (time2filename("tSkymapGrid-grid3"));
    ofstream logfile (filename.c_str(),ios::out);
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
    ok = False;
  }

  // Test 4 : Retrieve grid nodes within a given elevation range
  cout << "\n [4] Retrieve grid nodes within a given elevation range" << endl;
  try {
    Bool which (True);
    Vector<Double> elevation(2);
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
    Matrix<Double> grid = sg.grid(which);
    Matrix<Double> azel = sg.azel(which);
    IPosition shape (grid.shape());
    //
    sg.show (std::cout);
    //
    String filename (time2filename("tSkymapGrid-grid4"));
    ofstream logfile (filename.c_str(),ios::out);
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
    //
    logfile.close();
  } catch (AipsError x) {
    cerr << "[tSkymapGrid::test_grid] " << x.getMesg() << endl;
    ok = False;
  }

  // Test 5 : Check for dependency of the local coordinate grid on the choice
  //          of the celestial reference frame.
  cout << "\n [5] Dependency of local grid on celestial reference frame" << endl;
  try {
    Bool which (True);
    Quantity epoch (52940.4624,"d");
    String telescope ("LOPES");
    ObservationData obsData (epoch,telescope);
    ObsInfo obsInfo (obsData.obsInfo());
    Vector<Double> sunJ2000 (2);
    Vector<Double> sunGALACTIC (2);
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
    Matrix<Double> gridJ2000 = sgJ2000.grid(which);
    Matrix<Double> azelJ2000 = sgJ2000.azel(which);
    IPosition shapeJ2000 (gridJ2000.shape());
    //
    refcode = "GALACTIC";
    SkymapGrid sgGALACTIC (obsInfo,refcode, projection, shape, sunGALACTIC, increment);
    Matrix<Double> gridGALACTIC = sgGALACTIC.grid(which);
    Matrix<Double> azelGALACTIC = sgGALACTIC.azel(which);
    IPosition shapeGALACTIC (gridGALACTIC.shape());
    //
    cout << " - Grid shape J2000 .. : " << shapeJ2000 << endl;
    cout << " - Grid shape GALACTIC : " << shapeGALACTIC << endl;
    //
    if (shapeJ2000 == shapeGALACTIC) {
      String filename (time2filename("tSkymapGrid-grid5"));
      ofstream logfile (filename.c_str(),ios::out);
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
    ok = False;
  }

  cout << "\n [#] Tests completed" << endl;

  return ok;
}

// =============================================================================

/*!
  \brief Test the manipulation of the map orientation

  \return ok -- Status of the test routine.
 */
Bool test_orientation ()
{
  Bool ok (True);
  String refcode ("J2000");
  String projection ("STG");
  IPosition shape (2);
  Vector<Double> center (2);
  Vector<Double> increment (2);
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
    Matrix<Double> grid = sg.grid(True);
  }

  // Test 2: [East,South]
  {
    orientation(0) = "East";
    orientation(1) = "South";
    //
    sg.setOrientation (orientation);
    //
    Matrix<Double> grid = sg.grid(True);
  }

  // Test 3: [West,North]
  {
    orientation(0) = "West";
    orientation(1) = "North";
    //
    sg.setOrientation (orientation);
    //
    Matrix<Double> grid = sg.grid(True);
  }

  // Test 4: [West,South]
  {
    orientation(0) = "West";
    orientation(1) = "South";
    //
    sg.setOrientation (orientation);
    //
    Matrix<Double> grid = sg.grid(True);
  }

  return ok;
}

// =============================================================================

int main () {

  Bool ok (True);

  {
    ok = test_DirectionCoordinate ();
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

  return 0;

}
