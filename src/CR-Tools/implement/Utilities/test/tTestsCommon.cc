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

using std::cout;
using std::endl;

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
  cout << "\n[tTestsCommon::test_general]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Testing number_sequence() ..." << endl;
  try {
    std::vector<int> numbers;
    numbers = CR::number_sequence();
    uint nelem = numbers.size();
    cout << "-- nof. elements = " << nelem << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Testing test_getFrequencies() ..." << endl;
  try {
    casa::Vector<double> freq1 = CR::test_frequencyValues();
    casa::Vector<double> freq2 = CR::test_frequencyValues(100);
    casa::Vector<double> freq3 = CR::test_frequencyValues(100,100e06);
    casa::Vector<double> freq4 = CR::test_frequencyValues(100,100e06,2);
    //
    cout << "-- (--/--/--)      = " << freq1 << endl;
    cout << "-- (100/--/--)     = " << freq2 << endl;
    cout << "-- (100/100e06/--) = " << freq3 << endl;
    cout << "-- (100/100e06/2)  = " << freq4 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Testing test_antPositions() ..." << endl;
  try {
    casa::Matrix<double> antPositions;
    //
    cout << "-- Cartesian coordinates ..." << endl;
    CR::test_antPositions(antPositions,CoordinateType::Cartesian);
    cout << antPositions << endl;
    //
    cout << "-- Spherical coordinates ..." << endl;
    CR::test_antPositions(antPositions,CoordinateType::Spherical);
    cout << antPositions << endl;
    //
    cout << "-- Cylindrical coordinates ..." << endl;
    CR::test_antPositions(antPositions,CoordinateType::Cylindrical);
    cout << antPositions << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
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
  cout << "\n[tTestsCommon::test_Imaging]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Testing logging of Beamformer settings ..." << endl;
  try {
    CR::Beamformer bf;
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

  nofFailedTests += test_general ();
  nofFailedTests += test_Imaging ();

  return nofFailedTests;
}
