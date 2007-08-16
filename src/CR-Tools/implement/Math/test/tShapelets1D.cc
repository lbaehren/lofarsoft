/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

/*!
  \file tShapelets1D.cc
  
  \ingroup Math

  \brief A collection of test routines for the Shapelets1D class
  
  \author Lars B&auml;hren

  \date 2005/03

  <h3>Prerequisites</h3>
  <ul>
    <li>Hermite1D   -- A class to implement Hermite Polynomials.
    <li>Shapelets1D -- A class to implement 1-dimensional Shapelet functions.
  </ul>
*/

// C++ Standard library
#include <fstream>

// Custom header files
#include <Math/Shapelets1D.h>

using CR::Shapelets1D;

// =============================================================================

/*!
  \brief Display the current parameters values of a Shapelets1D object

  \param phi -- A Shapelets1D object
 */
void printShapelets1D (Shapelets1D& phi) {

  int order = phi.order();
  
  cout << endl;
  cout << " - Shapelet order  : " << order << endl;
  cout << " - Scale parameter : " << phi.beta() << endl;
  cout << " - Integral        : " << phi.integral(order) << endl;
  
}

// =============================================================================

/*!
  \brief Test constructors of Shapelets1D

  \param order -- Maximum order \f$ n \f$ of the Shapelet basis function
                  \f$ \phi_{n}(x) \f$ or \f$ B_n(x;\beta) \f$.
  \param x     -- A set of values for which the basis functions will be
                  evaluated.
 */
bool test_Shapelets1D (const int& order,
		       const vector<double>& x)
{
  cout << "\n[test_Shapelets1D] Testing default constructor" << endl;
  {
    Shapelets1D phi;
    //
    printShapelets1D (phi);
  }
  
  cout << "\n[test_Shapelets1D] Testing argumented constructor" << endl;
  {
    Shapelets1D phi (order);    
    //
    printShapelets1D (phi);
  }  
  
  cout << "\n[test_Shapelets1D] Testing argumented constructor" << endl;
  {
    double beta = 2.0;
    //
    Shapelets1D phi (order,beta);    
    //
    printShapelets1D(phi);
  }  
  
  return true;
}

// =============================================================================

/*!
  \brief Test the methods for accessing the Shapelet function parameters.

  \param order -- Maximum order \f$ n \f$ of the Shapelet basis function
                  \f$ \phi_{n}(x) \f$ or \f$ B_n(x;\beta) \f$.
  \param x     -- A set of values for which the basis functions will be
                  evaluated.
 */
bool test_parameters (const int& order,
		      const vector<double>& x)
{
  cout << "\n[test_Shapelets1D] Testing adjustment of parameters" << endl;

  int newOrder (15);
  double newBeta (0.12345);
  Shapelets1D phi;
  
  {
    // display initial shapelet parameters
    printShapelets1D (phi);
    
    cout << "\n Adjusting Shapelet order : " << order << " -> " << newOrder
	 << endl;
    phi.setOrder (newOrder);
    printShapelets1D (phi);
    
    cout << "\n Adjusting scale parameter : " << phi.beta() << " -> " << newBeta
	 << endl;
    phi.setBeta (newBeta);
    printShapelets1D (phi);
  }
  
  return true;
}

// =============================================================================

/*!
  \brief Test the scales representable with a Shapelet of order n.

  \param order -- Maximum order \f$ n \f$ of the Shapelet basis function
                  \f$ \phi_{n}(x) \f$ or \f$ B_n(x;\beta) \f$.
 */
bool test_scales (const int& order)
{
  cout << "\n[test_scales] Testing representable scales." << endl;

  Shapelets1D phi (order);
  
  for (int n=0; n<=order; n++) {
    cout << "\tn=" << n
	 << "\tR[r.m.s.]=" << phi.rmsRadius(n)
	 << "\tR[min]=" << phi.resolution(n) << endl;
  }
  
  return true;
}

// =============================================================================

/*!
  \brief Test the evaluation of the 1-dim Shapelet basis function

  \param order -- Maximum order \f$ n \f$ of the Shapelet basis function
                  \f$ \phi_{n}(x) \f$ or \f$ B_n(x;\beta) \f$.
  \param x     -- A set of values for which the basis functions will be
                  evaluated.
*/
bool test_eval (const int& order,
		const vector<double>& x) 
{
  int nelem;         // Number of elements in the input vector
  ofstream logfile;  // Stream for logfile taking computation output.
  double *results;   // Array for storage of computation results
  
  nelem = x.size();  
  results = new double[nelem*order];
  
  cout << "\n[testShapelets1D] Testing evaluation of Shapelet" << endl;
  {
    double y;
    
    for (int n=0; n<order; n++) {
      Shapelets1D phi(n);
      //
      cout << "\tEvaluating Shapelet of order " << phi.order() << " ... "
	   << flush;
      //
      for (int i=0; i<nelem; i++) {
	// compute ...
        y = phi.eval(n,x[i]);
	// ... and collect results
	results[n*nelem+i] = y;
      }
      cout << "Done." << endl;
    }

    // Write the computation results to disk for later plotting
    logfile.open ("tShapelets1D.data1",ios::out);
    for (int i=0; i<nelem; i++) {
      logfile << x[i];
      for (int n=0; n<order; n++) {
	logfile << "\t" << results[n*nelem+i];
      }
      logfile << endl;
    }
    logfile.close();
    
  }
  
  cout << "\n[testShapelets1D] Testing evaluation of Shapelet" << endl;
  {
    vector<double> y;   // vector to take up the results
    
    y.resize(nelem);
    
    for (int n=0; n<order; n++) {
      Shapelets1D h(n);
      //
      cout << "\tEvaluating Shapelet of order " << h.order() << " ... " << flush;
      y = h.eval(n,x);
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

int main () {

  bool ok;                 // Control variable
  int order;               // Maximum order of the Shapelets
  unsigned int nx;
  unsigned int nofPoints;  // Number of test values for which to evaluate
  vector<double> x;        // Array of values for which to evaluate.
  
  // assign variable values
  order = 10;
  nx = 50;
  nofPoints = 2*nx+1;
  x.resize(nofPoints);

  // Set of values for function evaluation test
  cout << "Values for test of Shapelet evaluation:" << endl;
  for (unsigned int i=0; i<x.size(); i++) {
    x[i] = 5.0*(1.0*i-nx)/nx;
  }
  
  // Test: Object construction
  {
    ok = test_Shapelets1D (order,x);
  }

  // Test: Access to the basis function parameters
  {
    ok = test_parameters (order,x);
  }

  // Test: Representable scales
  {
    ok = test_scales (order);
  }

  // Test: Evaluation of the Shapelet basis function
  {
    ok = test_eval (order,x);
  }

  return 0;

}

