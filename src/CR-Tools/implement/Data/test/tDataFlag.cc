/***************************************************************************
 *   Copyright (C) 2005                                                    *
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
/* $Id: tDataFlag.cc,v 1.4 2006/05/04 11:46:29 bahren Exp $ */

#include <Data/DataFlag.h>

using CR::DataFlag;

/*!
  \file tDataFlag.cc

  \ingroup Data
  
  \brief A collection of test routines for the DataFlag class

  \author Lars B&auml;hren

  \date 2005/06/24
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test DataFlag object constructors.

  \return ok -- Status of the test routine
*/
int test_DataFlag ()
{
  cout << "\n[test_DataFlag] Testing constructors\n" << endl;

  int nofFailedTests (0);
  Int nofFiles (10);
  Float threshold (0.75);
  Vector<Float> limits (2);
  Vector<Bool> bFlags (nofFiles, True);
  Vector<Float> fFlags (nofFiles, 0.9);

  limits(0) = -2048;
  limits(1) =  2047;

  // [1]
  cout << " - Test 1 (Int) ..." << endl;
  try {
    DataFlag<Float> df (nofFiles);
  } catch (std::string message) {
    nofFailedTests++;
  }
    
  // [2]
  cout << " - Test 2 (Int, Float) ..." << endl;
  try {
    DataFlag<Float> df (nofFiles, threshold);
  } catch (std::string message) {
    nofFailedTests++;
  }

  // [3]
  cout << " - Test 3 (Int, Float, Vector<Double>) ..." << endl;
  try {
    DataFlag<Float> df (nofFiles, threshold, limits);
  } catch (std::string message) {
    nofFailedTests++;
  }

  // [4]
  cout << " - Test 4 (Vector<Bool>) ..." << endl;
  try {
    DataFlag<Float> df (bFlags);
  } catch (std::string message) {
    nofFailedTests++;
  }

  // [5]
  cout << " - Test 5 (Vector<Float>) ..." << endl;
  try {
    DataFlag<Float> df (fFlags);
  } catch (std::string message) {
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

Bool test_flags ()
{
  cout << "\n[test_flags] Testing access to flags\n" << endl;

  Bool ok(True);
  Int nofFiles (10);
  Float threshold (0.75);
  Vector<Bool> bFlags (nofFiles);
  Vector<Float> fFlags (nofFiles);

  DataFlag<Float> df (nofFiles, threshold);

  // [1]
  cout << " - Test 1" << endl;
  //
  df.flags(fFlags);
  df.flags(bFlags);
  //
  cout << "\tThreshold : " << df.threshold() << endl;
  cout << "\tFloat     : " << fFlags << endl;
  cout << "\tBool      : " << bFlags << endl;

  // [2]
  cout << " - Test 2" << endl;
  //
  fFlags = 0.5;
  df.setFlags (fFlags);
  //
  df.flags(fFlags);
  df.flags(bFlags);
  //
  cout << "\tThreshold : " << df.threshold() << endl;
  cout << "\tFloat     : " << fFlags << endl;
  cout << "\tBool      : " << bFlags << endl;

  // [2]
  cout << " - Test 2" << endl;
  //
  df.setThreshold (0.4);
  //
  df.flags(fFlags);
  df.flags(bFlags);
  //
  cout << "\tThreshold : " << df.threshold() << endl;
  cout << "\tFloat     : " << fFlags << endl;
  cout << "\tBool      : " << bFlags << endl;

  return ok;
}

// --- Main routine ------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  Bool ok(True);

  nofFailedTests += test_DataFlag ();

  ok = test_flags ();

  return nofFailedTests;
}
