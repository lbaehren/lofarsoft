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


/* $Id: tObservationFrame.cc,v 1.3 2007/04/04 16:16:14 bahren Exp $ */

#include <Observation/ObservationFrame.h>

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
