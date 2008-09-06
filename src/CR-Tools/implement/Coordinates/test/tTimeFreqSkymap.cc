/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Coordinates/TimeFreqSkymap.h>

// Namespace usage
using CR::TimeFreqSkymap;

/*!
  \file tTimeFreqSkymap.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CR::TimeFreqSkymap class
 
  \author Lars B&auml;hren
 
  \date 2008/09/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CR::TimeFreqSkymap object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_constructors ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[tTimeFreqSkymap::test_constructors]\n" << std::endl;

  std::cout << "[1] Default constructor ..." << std::endl;
  try {
    TimeFreqSkymap newTimeFreqSkymap;
    //
    newTimeFreqSkymap.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Simple argumented constructor ..." << std::endl;
  try {
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap newTimeFreqSkymap (blocksPerFrame,
				      nofFrames);
    //
    newTimeFreqSkymap.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Fully argumented constructor ..." << std::endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap newTimeFreqSkymap (blocksize,
				      sampleFreq,
				      nyquistZone,
				      blocksPerFrame,
				      nofFrames);
    //
    newTimeFreqSkymap.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Copy constructor ..." << std::endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap coord1 (blocksize,
			   sampleFreq,
			   nyquistZone,
			   blocksPerFrame,
			   nofFrames);
    //
    coord1.summary();
    /* create second object from the first one */
    TimeFreqSkymap coord2 (coord1);
    /* Summary of object properties */
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
    nofFailedTests += test_constructors ();
  }

  return nofFailedTests;
}
