/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
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

/* $Id: tMasking.cc,v 1.5 2007/03/20 15:16:09 bahren Exp $ */

// Custom header files
#include <Utilities/Masking.h>

/*!
  \file tMasking.cc
  
  \ingroup CR_Utilities
  
  \brief A collection of tests for the routines implemented in Masking.cc

  \author Lars B&auml;hren
  
  \date 2005/02/11
*/

// =============================================================================

/*!
  \brief Test counting of valid and/or invalid elements in a boolean array.
  
  \return ok -- Status of the test routine.
*/
bool test_validElements () {
  
  bool ok (true);
  unsigned int nofElements(20);
  int nofValid(0);
  int nofInvalid(0);
  Vector<bool> mask(nofElements,true);
  
  for (unsigned int i=0; i<nofElements; i++) {
    mask(i) = false;
    nofValid = ntrue (mask);
    nofInvalid = mask.nelements() - nofValid;
    //
    cout << "\n[tMasking::test_validElements]" << endl;
    cout << " - Mask ........... : " << mask << endl;
    cout << " - Valid elements . : " << nofValid <<endl;
    cout << " - Invalid elements : " << nofInvalid << endl;
  }

  return ok;
}

// =============================================================================

/*!
  \brief Test merging of two existing masks

  \code 
  Array<bool> mergeMasks (const Array<bool>& array1,
			  const Array<bool>& array2,
			  const String logic);
  \endcode

  \return nofFailedTests
 */
int test_mergeMasks () {

  int nofFailedTests (0);
  
  // Test 1
  try {
    IPosition shape (3,2);
    Array<bool> mask1(shape);
    Array<bool> mask2(shape);
    String logic;
    //
    mask1 = true;
    mask2 = false;
    //
    {
      logic = "AND";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NAND";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "OR";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NOR";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
  } catch (std::string err) {
    std::cerr << "[test_mergeMasks] " << err << std::endl;
    nofFailedTests++;
  }

  // Test 2
  try {
    IPosition shape (3,4);
    Array<bool> mask1(shape);
    Array<bool> mask2(shape);
    String logic;
    //
    IPosition blc (3,0);
    IPosition trc (3,2);
    //
    mask1 = true;
    mask1(blc,trc) = false;
    //
    mask2 = false;
    mask2(blc,trc) = true;
    //
    {
      logic = "AND";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NAND";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "OR";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NOR";
      Array<bool> mask = CR::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
  } catch (std::string err) {
    std::cerr << "[test_mergeMasks] " << err << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test creationg of a submask

  \return nofFailedTests -- The number of failed tests
*/
int test_subMask ()
{
  int nofFailedTests (0);
  return nofFailedTests;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main (int argc, char* argv[]) {

  bool ok (true);
  int nofFailedTests (0);
  
  try {
    ok = test_validElements ();
  }
  catch (std::string message) {
    cerr << "[test_validElements] " << message << endl;
    return 1;
  } 

  {
    nofFailedTests += test_mergeMasks ();
  }

  return  nofFailedTests; 
}
