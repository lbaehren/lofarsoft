
/* $Id: tMasking.cc,v 1.5 2007/03/20 15:16:09 bahren Exp $ */

// Custom header files
#include <lopes/Utilities/Masking.h>

/*!
  \file tMasking.cc
  
  \ingroup Utilities
  
  \author Lars B&auml;hren
  
  \date 2005/02/11
*/

// =============================================================================

/*!
  \brief Test counting of valid and/or invalid elements in a boolean array.
  
  \return ok -- Status of the test routine.
*/
Bool test_validElements () {
  
  Bool ok (True);
  uInt nofElements(20);
  Int nofValid(0);
  Int nofInvalid(0);
  Vector<Bool> mask(nofElements,True);
  
  for (uInt i=0; i<nofElements; i++) {
    mask(i) = False;
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
  Array<Bool> mergeMasks (const Array<Bool>& array1,
			  const Array<Bool>& array2,
			  const String logic);
  \endcode

  \return nofFailedTests
 */
int test_mergeMasks () {

  int nofFailedTests (0);
  
  // Test 1
  try {
    IPosition shape (3,2);
    Array<Bool> mask1(shape);
    Array<Bool> mask2(shape);
    String logic;
    //
    mask1 = True;
    mask2 = False;
    //
    {
      logic = "AND";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NAND";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "OR";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NOR";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
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
    Array<Bool> mask1(shape);
    Array<Bool> mask2(shape);
    String logic;
    //
    IPosition blc (3,0);
    IPosition trc (3,2);
    //
    mask1 = True;
    mask1(blc,trc) = False;
    //
    mask2 = False;
    mask2(blc,trc) = True;
    //
    {
      logic = "AND";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NAND";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "OR";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
      cout << " Merging logic : " << logic << endl;
      cout << " Merged mask   : " << mask << endl;
    }
    //
    {
      logic = "NOR";
      Array<Bool> mask = LOPES::mergeMasks (mask1,mask2,logic);
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

  Bool ok (True);
  int nofFailedTests (0);
  
  try {
    ok = test_validElements ();
  }
  catch (AipsError x) {
    cerr << "[test_validElements] " << x.getMesg() << endl;
    return 1;
  } 

  {
    nofFailedTests += test_mergeMasks ();
  }

  return  nofFailedTests; 
}
