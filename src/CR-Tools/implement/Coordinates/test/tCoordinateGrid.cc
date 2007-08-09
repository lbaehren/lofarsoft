
/* $Id: tCoordinateGrid.cc,v 1.3 2006/01/16 18:17:33 bahren Exp $ */

#include <Coordinates/CoordinateGrid.h>

/*!
  \file tCoordinateGrid.cc
  
  \brief A collection of tests for CoordinateGrid
  
  \author Lars B&auml;hren
  
  \date 2005/03/21
*/

using std::cout;
using std::cerr;
using std::endl;

using casa::AipsError;
using casa::Double;
using casa::Int;
using casa::Matrix;
using casa::String;
using casa::Vector;

using CR::CoordinateGrid;

// =============================================================================

/*!
  \brief Set the parameter values required for creating a CoordinateGrid object.
  
  \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                      along each coordinate axis.
  \param center    - Coordinates of the center of the grid.
  \param increment - Coordinate increment between two subsequent grid nodes
                     along a coordinate axis.
  \param units     - Physical units associated with each coordinate axis.
*/
void setParameters (Vector<Int>& shape,
		    Vector<Double>& center,
		    Vector<Double>& increment,
		    Vector<String>& units)
{
  Int naxes (3);

  center.resize (naxes);
  increment.resize (naxes);
  shape.resize (naxes);
  units.resize (naxes);
  
  center(0) = 0.0;
  center(1) = 90;
  center(2) = 1000;
  
  increment(0) = 1.0;
  increment(1) = 1.0;
  increment(2) = 10.0;
  
  shape = 10;
  
  units(0) = "deg";
  units(1) = "deg";
  units(2) = "m";
}

// =============================================================================

/*!
  \brief Check the coordinates of the generated grid
  
  \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                      along each coordinate axis.
  \param center    - Coordinates of the center of the grid.
  \param increment - Coordinate increment between two subsequent grid nodes
                     along a coordinate axis.
  \param units     - Physical units associated with each coordinate axis.
*/
int test_gridding (Vector<Int>& shape,
		   Vector<Double>& center,
		   Vector<Double>& increment,
		   Vector<String>& units)
{
  cout << "\n[tCoordinateGrid::test_gridding]\n" << endl;

  int nofFailedTests (0);
  Vector<Int> pos (shape.nelements(),0);
  Int naxes (shape.nelements());
  Int nelem (1);
  
  shape = 3;

  try {
    for (int i=0; i<naxes; i++) {
      nelem *= shape(i);
    }
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  try {
    pos(0) = -1;    
    for (int m=0; m<nelem; m++) {
      // increment the last element in the position vector
      pos(0) += 1;
      // go through the axes and check if we are within the axis shape
      for (int n=0; n<naxes-1; n++) {
	// check if we're within the range of this axis
	if (pos(n) == shape(n)) {
	  pos(n) = 0;
	  pos(n+1) += 1;
	}
      }
      cout << "\t" << m << "\t" << pos << endl;
    }
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Show the parameters of the coordinate grid

  \param cg - CoordinateGrid object
 */
void show_variables (CoordinateGrid& cg)
{
  cout << " - Center .... : " << cg.center() << endl;
  cout << " - Increment . : " << cg.increment() << endl;
  cout << " - Shape ..... : " << cg.shape() << endl;
  cout << " - Units ..... : " << cg.units() << endl;
  cout << " - Use limits? : " << cg.useLimits() << endl;
}

// =============================================================================

/*!
  \brief Test constructors
  
  \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                      along each coordinate axis.
  \param center    - Coordinates of the center of the grid.
  \param increment - Coordinate increment between two subsequent grid nodes
                     along a coordinate axis.
  \param units     - Physical units associated with each coordinate axis.
*/
int test_CoordinateGrid (Vector<Int>& shape,
			 Vector<Double>& center,
			 Vector<Double>& increment,
			 Vector<String>& units)
{
  cout << "\n[tCoordinateGrid::test_CoordinateGrid]" << endl;

  int nofFailedTests (0);
  
  // Test 1
  cout << "\n[1] Testing empty constructor." << endl;
  try {
    CoordinateGrid cg;
    //
    show_variables (cg);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Test 2
  cout << "\n[2] Testing argumented constructor." << endl;
  try {
    CoordinateGrid cg (shape, center, increment, units);
    //
    show_variables (cg);
    //
    std::ofstream logfile;
    logfile.open("tCoordinateGrid-grid1.data",std::ios::out);
    logfile << "# Coordinate grid nodes for 3-dim regular grid." << endl;
    logfile << "# gnuplot > splot 'tCoordinateGrid-grid1.data'" << endl;
    cg.grid(logfile);
    logfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test 3
  cout << "\n[3] Testing argumented constructor." << endl;
  try {
    Int naxes (2);
    //
    shape.resize(naxes);
    shape = 10;
    //
    center.resize(naxes);
    center(0) = 0.0;
    center(1) = 90.0;
    //
    increment.resize(naxes);
    increment(0) = 1;
    increment(1) = 1;
    //
    units.resize(naxes);
    units(0) = "deg";
    units(1) = "deg";
    //
    CoordinateGrid cg (shape, center, increment, units);
    //
    show_variables (cg);
    //
    std::ofstream logfile;
    logfile.open("tCoordinateGrid-grid2.data",std::ios::out);
    logfile << "# Coordinate grid nodes for 2-dim regular grid." << endl;
    logfile << "# gnuplot > plot 'tCoordinateGrid-grid2.data'" << endl;
    cg.grid(logfile);
    logfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test 4
  cout << "\n[4] Testing argumented constructor." << endl;
  try {
    Int naxes (5);
    //
    shape.resize(naxes);
    shape(0) = 10;
    shape(1) = 10;
    shape(2) = 10;
    shape(3) = 5;
    shape(4) = 20;
    //
    center.resize(naxes);
    center(0) = 0.0;
    center(1) = 90.0;
    center(2) = 1000.0;
    center(3) = 10.0;
    center(4) = 20e06;
    //
    increment.resize(naxes);
    increment(0) = 1;
    increment(1) = 1;
    increment(2) = 100;
    increment(3) = 0.1;
    increment(4) = 1e06;
    //
    units.resize(naxes);
    units(0) = "deg";
    units(1) = "deg";
    units(2) = "m";
    units(3) = "sec";
    units(4) = "Hz";
    //
    CoordinateGrid cg (shape, center, increment, units);
    //
    show_variables (cg);
    //
    std::ofstream logfile;
    logfile.open("tCoordinateGrid-grid3.data",std::ios::out);
    logfile << "# Coordinate grid nodes for 5-dim regular grid." << endl;
    logfile << "# gnuplot > splot 'tCoordinateGrid-grid3.data' u 1:2:3" << endl;
    logfile << "# gnuplot > splot 'tCoordinateGrid-grid3.data' u 1:2:4" << endl;
    logfile << "# gnuplot > splot 'tCoordinateGrid-grid3.data' u 1:2:5" << endl;
    cg.grid(logfile);
    logfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test limiting the range of valid values
  
  \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                      along each coordinate axis.
  \param center    - Coordinates of the center of the grid.
  \param increment - Coordinate increment between two subsequent grid nodes
                     along a coordinate axis.
  \param units     - Physical units associated with each coordinate axis.
*/
int test_limits (Vector<Int>& shape,
		 Vector<Double>& center,
		 Vector<Double>& increment,
		 Vector<String>& units)
{
  cout << "\n[tCoordinateGrid::test_limits]" << endl;

  int nofFailedTests (0);
  Int naxes (shape.nelements());
  Matrix<Double> limits (naxes,2);
  //
  limits(0,0) = -180.0;
  limits(0,1) = 180.0;
  limits(1,0) = 0.0;
  limits(1,1) = 90.0;
  limits(2,0) = 500.0;
  limits(2,1) = 5000.0;
  
  // Test 1 : Set limits on all coordinate axes
  try {
    CoordinateGrid cg (shape, center, increment, units);
    //
    cout << "\n[1] Setting limits for the range of values." << endl;
    cg.setLimits(limits);
    //
    show_variables (cg);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test 2 : Set limits for the coordinate axes individually
  try {
    Vector<Double> tmpLimits (2);
    //
    CoordinateGrid cg (shape, center, increment, units);
    //
    cout << "\n[2] Setting range limits axis by axis ..." << endl;
    for (int axis=0; axis<naxes; axis++) {
      cout << "\t axis=" << axis << "\t limits=" << tmpLimits << " ... " << std::flush;
      //
      tmpLimits(0) = limits(axis,0);
      tmpLimits(1) = limits(axis,1);
      //
      cg.setLimits (tmpLimits,axis);
      //
      cout << "Set." << endl;
    }
    //
    cout << "[3] Accessing axis outside valid range; will throw error message"
	 << endl;
    cg.setLimits (tmpLimits,naxes);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test retrival of the coordinate grid via orderd array.

  \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                      along each coordinate axis.
  \param center    - Coordinates of the center of the grid.
  \param increment - Coordinate increment between two subsequent grid nodes
                     along a coordinate axis.
  \param units     - Physical units associated with each coordinate axis.
*/
int test_grid (Vector<Int>& shape,
	       Vector<Double>& center,
	       Vector<Double>& increment,
	       Vector<String>& units)
{
  cout << "\n[tCoordinateGrid::test_grid]" << endl;

  int nofFailedTests (0);
  Array<Double> grid;
  
  try {
    CoordinateGrid cg (shape, center, increment, units);
    //
    cg.grid(grid);
    //
    show_variables (cg);
    //
    cout << " - Shape of retrived array : " << grid.shape() << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
  
// =============================================================================
//
//  Main routine
//
// =============================================================================

int main ()
{
  int nofFailedTests (0);
  bool ok;
  Vector<Double> center;
  Vector<Double> increment;
  Vector<Int> shape;
  Vector<String> units;


  // Test generation of grid coordinates
  {
    setParameters (shape, center, increment, units);
    //
    nofFailedTests += test_gridding (shape, center, increment, units);
  }

  // Testing constructors
  {
    setParameters (shape, center, increment, units);
    //
    nofFailedTests += test_CoordinateGrid (shape, center, increment, units);
  }

  // Test retrival of the grid data
  {
    setParameters (shape, center, increment, units);
    //
    ok = test_grid (shape, center, increment, units);
  }
  
  // Test limiting the range of valid values
  {
    setParameters (shape, center, increment, units);
    //
    nofFailedTests += test_limits (shape, center, increment, units);
  }
  
  // Test export of grid node values
  {
    setParameters (shape, center, increment, units);
    //
    nofFailedTests += test_grid (shape, center, increment, units);
  }
  
  return nofFailedTests;
}
