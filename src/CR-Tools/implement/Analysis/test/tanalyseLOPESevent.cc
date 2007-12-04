/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
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
    
    results = newObject.ProcessEvent("/data/LOPES/cr/sel/2004.01.12.00:28:11.577.event", 
				     41.9898208, 64.70544, 1750., -25.44, 8.94, True, 
				     "tanalyseLOPESevent", True, Vector<Int>(), True);
    Vector<Int> flagged(5);
    flagged(0) = 70202; 
    flagged(1) = 80201;
    flagged(2) = 80202;
    flagged(3) = 90101;
    flagged(4) = 90102;
    results = newObject.ProcessEvent("/data/LOPES/cr/LOPES30_sel/2006.02.23.04:02:50.283.event", 
				     276.113703, 53.2910653 , 4750., -32.07,  46.60, True, 
				     "tanalyseLOPESevent2", True, flagged , True);

    results = newObject.ProcessEvent("/data/LOPES/cr/LOPES30_sel/2006.02.23.04:02:50.283.event", 
				     273.46,  53.32, 4000., -32.07,  46.60, True, 
				     "tanalyseLOPESevent3", True, flagged , True, True);

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
