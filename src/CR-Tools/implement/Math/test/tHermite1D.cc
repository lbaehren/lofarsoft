/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <fstream>

#include <Math/Hermite1D.h>

using CR::Hermite1D;

/*!
  \file tHermite1D.cc
  
  \ingroup CR_Math

  \brief A collection of test routines for Hermite1D
 
  \author Lars B&auml;hren
 
  \date 2005/03/06

  <h3>Prerequisites</h3>
  <ul>
    <li>Hermite1D   -- A class to implement Hermite Polynomials.
  </ul>
*/

// =============================================================================

/*!
  \brief Test constructors for a Hermite1D object
  
  \return nofFailedTests -- The number of failed tests in this function
*/
int test_Hermite1D () 
{
  int nofFailedTests (0);
  
  cout << "\n[testHermite1D] Testing default constructor" << endl;
  try {
    Hermite1D h;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "\n[testHermite1D] Testing argumented constructor" << endl;
  try {
    for (int n=0; n<10; n++) {
      Hermite1D h (n);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test manipulation of the order of the Hermite polynomial
 */
int test_order () 
{
  cout << "\n[testHermite1D] Testing access to polynomial order." << endl;

  int nofFailedTests (0);

  try {
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
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test function evaluation of the Hermite polynomial

  The simplest test for evaluation of the Hermite polynomial implementation
  is to check \f$ H_n(0) \f$, since these values can be compared directly
  to the values found in various tables.

  \param order -- Maximum order \f$ n \f$ of the Hermite polynomial.
  \param x     -- A set of values for which the polynomial will be evaluated.

  \return nofFailedTests -- The number of failed tests in this function
*/
int test_eval (const int& order,
	       const vector<double>& x) 
{
  std::cout << "\n[tHermite1D::test_eval]\n" << std::endl;
  
  int nofFailedTests (0);
  int nelem;         // Number of elements in the input vector.
  ofstream logfile;  // Stream for logfile taking computation output.
  double *results;   // Array for storage of computation results
  
  nelem = x.size();
  results = new double[nelem*order];

  cout << "[1] Testing evaluation of H[n](0)" << endl;
  {
    Hermite1D h(order);

    for (int n=0; n<=order; n++) {
      cout << "\tn=" << n << "\tH(0)=" << h.eval(n,0.0) << endl;
    }
  }
  
  cout << "[2] Testing Hermite1D::eval(int,double) ..." << endl;
  try {
    double y;
    
    for (int n=0; n<order; n++) {
      Hermite1D h(n);
      //
      cout << "\tEvaluating polynomial of order " << h.order() << " ... "
	   << flush;
      //
      for (int i=0; i<nelem; i++) {
	// compute
        y = h.eval(n,x[i]);
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
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
 
  cout << "[3] Testing Hermite1D::eval(int,vector<double>) ..." << endl;
  try {
    vector<double> y (x.size());

    cout << " -- order     = " << order    << endl;
    cout << " -- length(x) = " << x.size() << endl;
    cout << " -- length(y) = " << y.size() << endl;
    
    for (int n=0; n<order; n++) {
      Hermite1D h(n);
      //
      cout << " --> Evaluating polynomial of order " << h.order() << " ... " << flush;
      y = h.eval(n,x);
      cout << "Done." << endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  // release allocated memory
  delete [] results;
  
  return nofFailedTests;
}

// =============================================================================
//
//  Main function
//
// =============================================================================

int main (int argc, char *argv[]) 
{
  int nofFailedTests (0);
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
  try {
    for (unsigned int i=0; i<x.size(); i++) {
      x[i] = 2.0*(1.0*i-nx)/nx;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  if (nofFailedTests == 0) {
    // Test the constructors
    nofFailedTests += test_Hermite1D ();
    // Test access to polynomial order
    nofFailedTests += test_order ();
    // Test evaluation of the Hermite polynomial
    nofFailedTests += test_eval (order,x);
  }
  
  return nofFailedTests;
}
