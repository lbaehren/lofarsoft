/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ProductRule.h"
#include "MEAL/Polynomial.h"

using namespace std;
using namespace MEAL;

int main () try
{
  Function::verbose = Function::very_verbose = true;

  ProductRule<Scalar> product1;

  Polynomial poly1 (3);
  Polynomial poly2 (5);

  product1 *= &poly1;
  product1 *= &poly2;

  if (product1.get_nparam() != poly1.get_nparam() + poly2.get_nparam()) {
    cerr << " product1 nparam=" << product1.get_nparam() 
	 << " p1 & p2 nparam=" << poly1.get_nparam() + poly2.get_nparam()
	 << endl;
    return -1;
  }

  // after adding both polynomials to the product, poly1 should be found
  vector<unsigned> imap;
  MEAL::get_imap (&product1, &poly1, imap);

  if (imap.size() != poly1.get_nparam()) {
    cerr << "unequal sizes 1" << endl;
    return -1;
  }

  ProductRule<Scalar> product2;

  product2 *= &poly2;
  product2 *= &poly1;

  product1 *= &product2;

  /* 
     after adding the products of the same two polynomials, product1
     should have the same number of free parameters
  */
  if (product1.get_nparam() != poly1.get_nparam() + poly2.get_nparam()) {
    cerr << " second check " << endl;
    cerr << " product1 nparam=" << product1.get_nparam() 
	 << " p1 & p2 nparam=" << poly1.get_nparam() + poly2.get_nparam()
	 << endl;
    return -1;
  }

  MEAL::get_imap (&product1, &product2, imap);

  if (imap.size() != product2.get_nparam()) {
    cerr << "unequal sizes 2" << endl;
    return -1;
  }

  Polynomial poly3 (4);
  product2 *= &poly3;

  /*
    the real test: after adding a third, new polynomial to the second
    product, will the mapping of the third polynomial be found in
    the first product?
  */

  MEAL::get_imap (&product1, &poly3, imap);

  if (imap.size() != poly3.get_nparam()) {
    cerr << "unequal sizes 3" << endl;
    return -1;
  }

  cerr << "Successful completion" << endl;
  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

