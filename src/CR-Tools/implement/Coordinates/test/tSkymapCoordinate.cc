/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1964 2008-09-06 17:52:38Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#include <Coordinates/SkymapCoordinate.h>
#include <Observation/ObservationData.h>

// Namespace usage
using std::cout;
using std::endl;
using CR::SkymapCoordinate;

/*!
  \file tSkymapCoordinate.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the SkymapCoordinate class
 
  \author Lars B&auml;hren
 
  \date 2009/01/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SkymapCoordinate object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[tSkymapCoordinate::test_constructors]\n" << endl;

  int nofFailedTests     = 0;
  std::string telescope  = "LOFAR";
  std::string observer   = "Lars Baehren";
  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint blocksize         = 1024;
  casa::Quantity sampleFreq (200,"MHz");
  uint nyquistZone (1);
  uint blocksPerFrame (1);
  uint nofFrames (10);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    SkymapCoordinate coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing construction from observation data and time-freq ..." << endl;
  try {
    CR::ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    //
    SkymapCoordinate coord (obsData,timeFreq);
    //
    std::cout << "-- blocksize        = " << blocksize      << std::endl;
    std::cout << "-- sample frequency = " << sampleFreq     << std::endl;
    std::cout << "-- Nyquist zone     = " << nyquistZone    << std::endl;
    std::cout << "-- blocks per frame = " << blocksPerFrame << std::endl;
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing construction for (Direction,Radius) coordinate ..." << endl;
  try {
    CR::ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    //
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Testing construction for cartesian coordinate ..." << endl;
  try {
    CR::ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::Cartesian,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    //
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Testing copy constructor ..." << endl;
  try {
    CR::ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    coord.summary();
    //
    SkymapCoordinate coordCopy (coord);
    coordCopy.summary();
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
  nofFailedTests += test_constructors ();

  return nofFailedTests;
}
