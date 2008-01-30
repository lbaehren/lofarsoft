/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <Analysis/analyseLOPESevent.h>

using CR::analyseLOPESevent;
using CR::LopesEventIn;

/*!
  \file tanalyseLOPESevent.cc

  \ingroup Analysis

  \brief A collection of test routines for the analyseLOPESevent class
 
  \author Andreas Horneffer
 
  \date 2007/06/19
*/

/*!
  \brief Test constructors for a new analyseLOPESevent object

  \return nofFailedTests -- The number of failed tests.
*/
int test_analyseLOPESevent ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_analyseLOPESevent]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    analyseLOPESevent newObject;

    Record obsrec,results;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");

    newObject.initPipeline(obsrec);

    std::cout << "  1) Testing on big LOPES10 event" << std::endl;
    results = newObject.ProcessEvent("/data/LOPES/cr/sel/2004.01.12.00:28:11.577.event", //evname
				     41.9898208,           // Az
				     64.70544,             // El
				     1750.,                // distance
				     -25.44,               // XC
				     8.94,                 // YC
				     True,                 // RotatePos
				     "tanalyseLOPESevent", // PlotPrefix
				     True,                 // generatePlots
				     Vector<Int>(),        // FlaggedAntIDs
				     True);                // verbose
    Vector<Int> flagged(5);
    flagged(0) = 70202; 
    flagged(1) = 80201;
    flagged(2) = 80202;
    flagged(3) = 90101;
    flagged(4) = 90102;
    std::cout << "  2) Testing on big LOPES30 event w/o direction-fit & w/o TV-Calib" << std::endl;
    results = newObject.ProcessEvent("/data/LOPES/cr/LOPES30_sel/2006.02.23.04:02:50.283.event", //evname
				     276.113703,            // Az
				     53.2910653,            // El
				     4750.,                 // distance
				     -32.07,                // XC
				     46.60,                 // YC
				     True,                  // RotatePos
				     "tanalyseLOPESevent2", // PlotPrefix
				     True,                  // generatePlots
				     flagged ,              // FlaggedAntIDs
				     True,                  // verbose
				     False,                 // simplexFit
				     0.,                    // ExtraDelay
				     False);                // doTVcal

    std::cout << "  3) Testing on big LOPES30 event with simplex-fit" << std::endl;
    results = newObject.ProcessEvent("/data/LOPES/cr/LOPES30_sel/2006.02.23.04:02:50.283.event", //evname
				     273.46,                // Az
				     53.32,                 // El
				     4000.,                 // distance
				     -32.07,                // XC
				     46.60,                 // YC
				     True,                  // RotatePos
				     "tanalyseLOPESevent3", // PlotPrefix
				     True,                  // generatePlots
				     flagged ,              // FlaggedAntIDs
				     True,                  // verbose
				     True);                 //simplexFit

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    double remoteStart (1./4.);
    double remoteStop (3./4.);
    double fitStart (-2e-06);
    double fitStop (-1.5e-06);
    double plotStart;
    double plotStop;

    analyseLOPESevent event (remoteStart,
			     remoteStop,
			     fitStart,
			     fitStop,
			     plotStart,
			     plotStop);
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
    nofFailedTests += test_analyseLOPESevent ();
  }

  return nofFailedTests;
}
