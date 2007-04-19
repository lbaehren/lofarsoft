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

/* $Id: tMSFiller.cc,v 1.2 2006/07/27 10:54:05 bahren Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Data/MSFiller.h>

/*!
  \file tMSFiller.cc

  \ingroup Data

  \brief A collection of test routines for MSFiller
 
  \author Lars B&auml;hren
 
  \date 2006/07/10
*/

/*!
  \brief Test usage of the MSSimulator and newMSSimulator classes
*/
int test_MSSimulator ()
{
  int nofFailedTests (0);
  string newMSName ("tMSFiller.ms");
  
  MSSimulator sim;

  // Elevation limit 
  try {
    Quantity elevationLimit (10.0, "deg");
    //
    sim.setElevationLimit(elevationLimit);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Weighting of the auto-correlation values
  try {    
    Float autoCorrWeight (1.0);
    sim.setAutoCorrelationWt(autoCorrWeight);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Time parameters
  try {
    Quantity integrationTime (60.0, "s");
    Quantity gapTime (10.0, "s");
    Bool useHourAngle (False);
    Quantity startTime (0.0, "h");
    Quantity stopTime  (1.0, "h");
    MEpoch refTime (MVEpoch(51483.18946969),MEpoch::UTC);
    //
    sim.setTimes(integrationTime,
		 gapTime,
		 useHourAngle,
		 startTime,
		 stopTime,
		 refTime);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  std::cout << " - Sky field information ..." << endl;
  try {
    uInt nSources (1);
    Vector<String> sourceName (nSources);
    sourceName(0) = "Zenith";
    Vector<MDirection> sourceDirection(1);
    sourceDirection(0) = MDirection(Quantity(0.0, "deg"), 
				    Quantity(90.0, "deg"),   
				    MDirection::Ref(MDirection::AZEL)) ;    
    Vector<Int> intsPerPointing(nSources);
    intsPerPointing(0) = 1;
    Vector<Int> mosPointingsX(nSources);
    Vector<Int> mosPointingsY(nSources);
    mosPointingsX(0) = 3;
    mosPointingsY(0) = 3;
    Vector<Float> mosSpacing(nSources);
    mosSpacing(0) = 0.0;
    
    sim.initFields(nSources,
		   sourceName,
		   sourceDirection,
		   intsPerPointing,
		   mosPointingsX,
		   mosPointingsY,
		   mosSpacing);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Antenna configuration
  try {
    String telescope ("ATCA");
    Vector<Double> x(6);
    Vector<Double> y(6);
    Vector<Double> z(6);
    y.set(0.0);
    z.set(0.0);
    x(0) = 0;
    x(1) = 40;
    x(2) = 100;
    x(3) = 180;
    x(4) = 280;
    x(5) = 400;
    Vector<Float> diam(6);
    diam.set(12.0);
    Vector<String> mount(6);
    mount.set("AZ/EL");
    Vector<String> name(6);
    name.set("ANT1");
    for (uInt i=0; i<6;i++) {
      ostringstream cbuf; cbuf << flush << "ANT" << i;
      name(i)=cbuf.str();
    }
    String coordsys = "local";	
    MPosition mRef( Quantity( 6372960.26, "m"),
		    Quantity( 2.61014232, "rad"),
		    Quantity( -0.52613792, "rad"),
		    MPosition::Ref(MPosition::ITRF));
    //
    sim.initAnt(telescope,
		x,
		y,
		z,
		diam,
		mount,
		name,
		coordsys,
		mRef);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  std::cout << " - Spectral windows ..." << endl;
  try {
    uInt nSpWindows = 2;
    Vector<String> spWindowName(2);
    Vector<Int> nChan(2);
    Vector<Quantity> startFreq(2);
    Vector<Quantity> freqInc(2);
    Vector<Quantity> freqRes(2);
    Vector<String> stokesString(2);
    
    spWindowName(0) = "XBAND";
    spWindowName(1) = "QBAND";
    nChan(0) = 1;
    nChan(1) = 16;
    startFreq(0) = Quantity(8.0, "GHz");
    startFreq(1) = Quantity(43.0, "GHz");
    freqInc(0) = Quantity(50, "MHz");
    freqInc(0) = Quantity(0.5, "MHz");
    freqRes(0) = Quantity(50, "MHz");
    freqRes(0) = Quantity(0.5, "MHz");
    stokesString(0) = "RR RL LR LL";
    stokesString(1) = "RR LL";
    //
    sim.initSpWindows(nSpWindows,
		      spWindowName,
		      nChan,
		      startFreq,
		      freqInc, 
		      freqRes,
		      stokesString);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  std::cout << " - Antenna feeds ..." << endl;
  try {
    String feedMode ("perfect R L");
    //
    sim.initFeeds(feedMode);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Write the MeasurementSet
  try {
    sim.writeMS(newMSName);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new MSFiller object

  \return nofFailedTests -- The number of failed tests.
*/
int test_MSFiller ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_MSFiller]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    MSFiller newObject;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_MSSimulator ();
  }

  // Test for the constructor(s)
//   {
//     nofFailedTests += test_MSFiller ();
//   }

  return nofFailedTests;
}
