
/* $Id: tObservationFrame.cc,v 1.2 2005/06/29 12:14:26 bahren Exp $ */

#include <lopes/Observation/ObservationFrame.h>

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
