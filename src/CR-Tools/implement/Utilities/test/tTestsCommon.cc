/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Utilities/TestsCommon.h>
#include <Imaging/Beamformer.h>

/*!
  \file tTestsCommon.cc

  \ingroup CR_Utilities

  \brief A collection of test routines for the TestsCommon class
 
  \author Lars B&auml;hren
 
  \date 2009/04/15
*/

//_______________________________________________________________________________
//                                                                   test_general

/*!
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_general ()
{
  std::cout << "\n[tTestsCommon::test_general]\n" << std::endl;

  int nofFailedTests (0);

  std::cout << "[1] Testing number_sequence() ..." << std::endl;
  try {
    std::vector<int> numbers;
    numbers = CR::number_sequence();
    uint nelem = numbers.size();
    std::cout << "-- nof. elements = " << nelem << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   test_Imaging

/*!
  \brief Test methods for the module Imaging

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_Imaging ()
{
  std::cout << "\n[tTestsCommon::test_Imaging]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing logging of Beamformer settings ..." << std::endl;
  try {
    CR::Beamformer bf;
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

  nofFailedTests += test_general ();
  nofFailedTests += test_Imaging ();

  return nofFailedTests;
}
