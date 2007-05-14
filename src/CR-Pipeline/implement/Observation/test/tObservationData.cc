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

/* $Id: tObservationData.cc,v 1.6 2007/04/04 16:16:14 bahren Exp $ */

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>

#include <templates.h>
#include <Observation/ObservationData.h>

#include <casa/namespace.h>

using CR::ObservationData;

/*!
  \file tObservationData.cc

  \brief A collection of tests for ObservationData.

  \author Lars B&auml;hren

  \date 2005/04/18
*/

/*!
  \brief Create array with some dummy antenna positions

  \param nofAntennas    -- Number of antenna elements the telecope has
  \param coordAxisFirst -- Return the coordinates per antenna as the first
                           axis of the array, i.e. [nCoord,nAntenna]. If
			   <tt>coordAxisFirst=False</tt> the antenna
			   positions will be return as [nAntenna,nCoord].

  \return antennaPositions -- Array with the 3D antenna positions,
 */
Matrix<Double> antennaPositions (uint const &nofAntennas,
				 bool const &coordAxisFirst)
{
  IPosition shape (2,3,nofAntennas);
  Matrix<Double> pos (shape);

  // Assign values
  for (uint antenna (0); antenna<nofAntennas; antenna++) {
    pos(0,antenna) = 1.0*antenna;
    pos(1,antenna) = 2.0*antenna;
    pos(2,antenna) = (pos(0,antenna)+pos(1,antenna))/2.0;
  }

  // If required, swap the orientation of the axes
  if (coordAxisFirst) {
    return pos;
  } else {
    Matrix<Double> swapped (shape(1),shape(0));
    for (int antenna (0); antenna<shape(1); antenna++) {
      for (int coord (0); coord<shape(0); coord++) {
	swapped(antenna,coord) = pos(coord,antenna);
      }
    }
    return swapped;
  }
  
}

/*!
  \brief Generate AIPS++ ObsInfo object capsulating obersvation information
 */
ObsInfo getObsInfo ()
{
  Quantity epoch (52940.4624,"d");
  MEpoch obsDate (epoch);
  String telescope ("LOPES");
  String observer ("Lars Baehren");

  ObsInfo obsInfo;
  obsInfo.setTelescope (telescope);
  obsInfo.setObsDate (obsDate);

  return obsInfo;
}

// =============================================================================

/*!
  \brief Test constructors for an ObservationData object.

  \return ok -- Status of the test routine; return <i>False</i> in case of 
          errors.
*/
int test_ObservationData ()
{
  cout << "\n[tObservationData::test_ObservationData]" << endl;

  int nofFailedTests (0);

  ObsInfo obsInfo = getObsInfo();
  uint nofAntennas (10);
  
  cout << "\n [1] Default (empty) constructor\n" << endl;
  try {
    ObservationData obsData;
    //
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "\n [2] Test constructor with observatory name\n" << endl;
  try {
    //
    ObservationData obsData (obsInfo.telescope());
    //
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "\n [3] Test constructor with epoch and observatory name\n" << endl;
  try {
    //
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope());
    //
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n [4] Test (almost) fully argumented constructor\n" << endl;
  try {
    //
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope(),
			     antennaPositions(nofAntennas,true),
			     obsInfo.observer());
    //
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n [5] Test fully argumented constructor\n" << endl;
  try {
    //
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope(),
			     ObservationData::observatoryPosition(obsInfo.telescope()),
			     antennaPositions(nofAntennas,true),
			     obsInfo.observer());
    //
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n [6] Test copy constructor\n" << endl;
  try {
    //
    ObservationData obs1 (obsInfo.obsDate(),
			  obsInfo.telescope());
    cout << "original object" << endl;
    obs1.summary();
    //
    cout << "copied object" << endl;
    ObservationData obs2 (obs1);
    obs2.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_ObservationData] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test working with the antenna position information

  \return nofFailedTests -- Number of failed tests

  Tests for ObservationData::setAntennaPositions and
  ObservationData::antennaPositions.
 */
int test_antennas ()
{
  cout << "\n[tObservationData::test_antennas]" << endl;

  int nofFailedTests (0);
  ObsInfo obsInfo = getObsInfo();
  uint nofAntennas (10);
  Matrix<Double> antPos (antennaPositions(nofAntennas,true));

  cout << "\n[1] Set antenna positions after object creation" << endl;
  try {
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope());
    cout << "object after construction" << endl;
    obsData.summary();
    //
    obsData.setAntennaPositions (antPos);
    //
    cout << "object after assignment of antenna positions" << endl;
    obsData.summary();
  } catch (AipsError x) {
    cerr << "[tObservationData::test_antennas] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "\n[2] Set antenna positions from inverted shape" << endl;
  try {
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope());

    Matrix<Double> positions = antennaPositions(nofAntennas,false);
    obsData.setAntennaPositions (positions,false);

    cout << "coordinate axis first : " << obsData.antennaPositions (True)  << endl;
    cout << "antenna number first  : " << obsData.antennaPositions (False) << endl;
  } catch (AipsError x) {
    cerr << "[tObservationData::test_antennas] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "\n[3] Retrieve antenna positions in different ordering" << endl;
  try {
    ObservationData obsData (obsInfo.obsDate(),
			     obsInfo.telescope());
    obsData.setAntennaPositions (antPos);

    cout << "coordinate axis first : " << obsData.antennaPositions (True)  << endl;
    cout << "antenna number first  : " << obsData.antennaPositions (False) << endl;
  } catch (AipsError x) {
    cerr << "[tObservationData::test_antennas] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test creation and operation of the coordinate conversion engine.

  \return ok -- Status of the test routine; return <i>False</i> in case of 
                errors.
*/
int test_conversionEngine ()
{
  cout << "\n[tObservationData::test_conversionEngine]" << endl;

  int nofFailedTests (0);
  Quantity epoch (52940.4624,"d");
  String telescope ("LOPES");
  Vector<Double> sunAZEL (2);
  Vector<Double> sunJ2000 (2);

  sunAZEL(0) = 178.822463;   // Position of the Sun in AZEL coordinates
  sunAZEL(1) = 28.0218712;   // at LOPES telescope on 52940.4624
  //
  sunJ2000(0) = -147.700631;    // Position of the Sun in J2000 coordinates
  sunJ2000(1) = -13.0431452;    // 
  
  // Test 1: Conversion between local and celestial reference frame
  cout << "\n [1] Conversion between local and celestial reference frame\n" << endl;
  try {
    Vector<Double> sun (2);
    Vector<String> refcode(3);
    //
    refcode(0) = "J2000";
    refcode(1) = "GALACTIC";
    refcode(2) = "B1950";
    //
    ObservationData obsData (epoch,telescope);
    //
    cout << " Observatory : " << obsData.observatory() << endl;
    cout << " Location    : " << obsData.observatoryPosition() << endl;
    cout << " Epoch ..... : " << epoch << endl;
    cout << "             : " << obsData.epoch() << endl;
    cout << " Sun (AZEL)  : " << sunAZEL << endl;
    cout << " Sun (J2000) : " << sunJ2000 << endl;
    //
    for (uInt n=0; n<refcode.nelements(); n++) {
      MDirection::Convert conv = obsData.conversionEngine (refcode(n),False);
      //
      MVDirection MVDirectionFROM;
      Vector<Quantity> QDirectionTO(2);
      //
      MVDirectionFROM = MVDirection (Quantity(sunAZEL(0),"deg"),
				     Quantity(sunAZEL(1),"deg"));
      //
      QDirectionTO = conv(MVDirectionFROM).getValue().getRecordValue();
      //
      sun(0) = QDirectionTO(0).getValue("deg");
      sun(1) = QDirectionTO(1).getValue("deg"); 
      //
      cout << " Sun ....... : " << sun << " (" << refcode(n) << ")" << endl;
    }
  } catch (AipsError x) {
    cerr << "[tObservationData::test_conversionEngine] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test 2: Conversion between two celestial reference frames
  cout << "\n [2] Conversion between two celestial reference frames\n" << endl;
  try {
    ObservationData obsData (epoch,telescope);
    Vector<Double> sun (2);
    Vector<String> refcode(3);
    MVDirection MVDirectionFROM;
    Vector<Quantity> QDirectionTO(2);
    //
    refcode(0) = "J2000";
    refcode(1) = "GALACTIC";
    refcode(2) = "B1950";
    //
    for (uInt n=1; n<refcode.nelements(); n++) {
      //
      MDirection::Convert conv = obsData.conversionEngine (refcode(n),refcode(0));
      //
      MVDirectionFROM = MVDirection (Quantity(sunJ2000(0),"deg"),
				     Quantity(sunJ2000(1),"deg"));
      //
      QDirectionTO = conv(MVDirectionFROM).getValue().getRecordValue();
      //
      sun(0) = QDirectionTO(0).getValue("deg");
      sun(1) = QDirectionTO(1).getValue("deg"); 
      //
      cout << " "
	   << sunJ2000 << " (" << refcode(0) << ")"
	   << "  ->  "
	   << sun << " (" << refcode(n) << ")"<< endl;
    }
  } catch (AipsError x) {
    cerr << "[tObservationData::test_conversionEngine] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  int nofFailedTests (0);

  {
    nofFailedTests += test_ObservationData ();
  }

  {
    nofFailedTests += test_antennas ();
  }

  {
    nofFailedTests += test_conversionEngine ();
  }

  return nofFailedTests;
}
