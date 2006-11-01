
/* $Id: tHermite1D.cc,v 1.2 2005/07/15 10:06:05 bahren Exp $ */

/*!
  \file tHermite1D.cc

  \brief A collection of test routines for Hermite1D
 
  \author Lars B&auml;hren
 
  \date 2005/03/06

  <h3>Prerequisites</h3>
  <ul>
    <li>Hermite1D   -- A class to implement Hermite Polynomials.
  </ul>
*/

// C++ Standard library
#include <fstream>

// Custom header files
#include <Functionals/Hermite1D.h>

// =============================================================================

/*!
  \brief Test constructors for a Hermite1D object

  Test default (empty) and argumented constructor(s).
 */
bool test_Hermite1D () 
{
  
  cout << "\n[testHermite1D] Testing default constructor" << endl;
  {
    Hermite1D h;
    //
    cout << "\tConstructed Hermite polynomial of order " << h.order() << endl;
  }
  
  cout << "\n[testHermite1D] Testing argumented constructor" << endl;
  {
    for (int n=0; n<10; n++) {
      Hermite1D h (n);
      //
      cout << "\tConstructed Hermite polynomial of order " << h.order() << endl;
    }
  }
  
  return true;
}

// =============================================================================

/*!
  \brief Test manipulation of the order of the Hermite polynomial
 */
bool test_order () 
{
  cout << "\n[testHermite1D] Testing access to polynomial order." << endl;
  {
    Hermite1D h;
    int order;
    //
    order = h.order();
    //
    cout << "\tConstructed Hermite polynomial of order " << h.order() << endl;
    //
    for (int n=order; n<2*order; n++) {
      h.setOrder(n);
      //
      cout << "\tPolynomial order adjusted to " << h.order() << endl;
    }
  }
  
  return true;
  
}

// =============================================================================

/*!
  \brief Test function evaluation of the Hermite polynomial

  The simplest test for evaluation of the Hermite polynomial implementation
  is to check \f$ H_n(0) \f$, since these values can be compared directly
  to the values found in various tables.

  \param order -- Maximum order \f$ n \f$ of the Hermite polynomial.
  \param x     -- A set of values for which the polynomial will be evaluated.
*/
bool test_eval (const int& order,
		const std::vector<double>& x) 
{
  int nelem;         // Number of elements in the input vector.
  std::ofstream logfile;  // Stream for logfile taking computation output.
  double *results;   // Array for storage of computation results
  
  nelem = x.size();
  results = new double[nelem*order];

  cout << "\n[testHermite1D] Testing evaluation of H[n](0)" << endl;
  {
    Hermite1D h(order);

    for (int n=0; n<=order; n++) {
      cout << "\tn=" << n << "\tH(0)=" << h.fx(n,0.0) << endl;
    }
  }
  
  cout << "\n[testHermite1D] Testing evaluation of polynomial" << endl;
  {
    double y;
    
    for (int n=0; n<order; n++) {
      Hermite1D h(n);
      //
      cout << "\tEvaluating polynomial of order " << h.order() << " ... "
	   << flush;
      //
      for (int i=0; i<nelem; i++) {
	// compute
        y = h.fx(n,x[i]);
	// collect results
	results[n*nelem+i] = y;
      }
      cout << "Done." << endl;
    }

    // Write the computation results to disk for later plotting
    logfile.open ("tHermite1D.data1",ios::out);
    for (int i=0; i<nelem; i++) {
      logfile << x[i];
      for (int n=0; n<order; n++) {
	logfile << "\t" << results[n*nelem+i];
      }
      logfile << endl;
    }
    logfile.close();
    
  }
  
  cout << "\n[testHermite1D] Testing evaluation of polynomial" << endl;
  {
    vector<double> y;   // vector to take up the results
    
    y.resize(nelem);
    
    for (int n=0; n<order; n++) {
      Hermite1D h(n);
      //
      cout << "\tEvaluating polynomial of order " << h.order() << " ... " << flush;
      y = h.fx(n,x);
      cout << "Done." << endl;
    }
  }

  // release allocated memory
  delete [] results;
  
  return true;
}

// =============================================================================
//
//  Main function
//
// =============================================================================

int main (int argc, char *argv[]) 
{
  bool ok;
  int order;
  unsigned int nx;
  unsigned int nofPoints;
  vector<double> x;

  // assign variable values
  order = 10;
  nx = 50;
  nofPoints = 2*nx+1;
  x.resize(nofPoints);

  // Set of values for function evaluation test
  cout << "Values for test of polynomial evaluation:" << endl;
  for (unsigned int i=0; i<x.size(); i++) {
    x[i] = 2.0*(1.0*i-nx)/nx;
  }
  
  // Test the constructors
  {
    ok = test_Hermite1D ();			 
  }
  
  // Test access to polynomial order
  {
    ok = test_order ();			 
  }
  
  // Test evaluation of the Hermite polynomial
  {
    ok = test_eval (order,x);
  }
  
  return 0;
  
}
