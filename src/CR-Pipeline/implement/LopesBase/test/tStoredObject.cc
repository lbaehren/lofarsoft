/***************************************************************************
 *   Copyright (C) 2005                                                    *
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
/* $Id: tStoredObject.cc,v 1.4 2007/04/17 13:52:48 bahren Exp $*/

#include <lopes/LopesBase/StoredObject.h>

/*!
  \file tStoredObject.cc

  \brief A collection of test routines for StoredObject
 
  \author Andreas Horneffer
 
  \date 2005/07/20
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StoredObject object

  return nofFailedTests -- The number of failed test within this function
*/
int test_StoredObject ()
{
  int nofFailedTests (0);

  std::cout << "[0] Constructor for base object..." << std::endl;
  try {
    BasicObject base;
    base.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };  
  
  std::cout << "[1] Testing default constructor..." << std::endl;
  try {
    StoredObject<Double> object_double;
    StoredObject<DComplex> object_dcomplex;
    object_double.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };  
  
  std::cout << "[2] Testing argumented constructor..." << std::endl;
  try {
    uInt nofParents (2);
    StoredObject<Double> object_double (nofParents);
    StoredObject<DComplex> object_dcomplex (nofParents);
    object_double.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };  
  
//   try {
//   bool ok (true);
//     StoredObject<Double> so;
//     Vector<Double> vec;
//     ok = so.get(&vec);
//   } catch (AipsError x) {
//     cerr << x.getMesg() << endl;
//     nofFailedTests++;
//   };
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_StoredObject ();
  }

  return nofFailedTests;
}
