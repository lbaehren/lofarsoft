
/* $Id: tObservationFrame.cc,v 1.3 2007/04/04 16:16:14 bahren Exp $ */

#include <lopes/Observation/ObservationFrame.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::AipsError;

/*!
  \file tObservationFrame.cc

  \brief A collection of tests for ObservationFrame.
 */

// =============================================================================

void testConstructor () {

  // Test: Empty constructor

  try {
    cout << "[tObservationFrame::testConstructor] Testing empty constructor"
	 << endl;
    ObservationFrame obs = ObservationFrame ();
  } catch (AipsError x) {
    cerr << "[tObservationFrame::testConstructor]" << x.getMesg() << endl;
  }

  // Test: Argumented constructor

  try {
    cout << "[tObservationFrame::testConstructor] Testing argumented constructor"
	 << endl;
    //
    Quantity epoch;
    String obsName;
    Time currentTime;
    //
    epoch = Quantity (currentTime.modifiedJulianDay(),"d");
    obsName = "WSRT";
    //
    ObservationFrame obs = ObservationFrame (epoch,obsName);
  } catch (AipsError x) {
    cerr << "[tObservationFrame::testConstructor]" << x.getMesg() << endl;
  }
  
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  testConstructor ();

  return 0;

}
