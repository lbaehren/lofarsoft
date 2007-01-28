
/* $Id: tShapelets2D.cc,v 1.2 2005/07/15 10:06:05 bahren Exp $ */

/*!
  \file tShapelets2D.cc

  \brief A collection of test routines for Shapelets2D
  
  \author Lars B&auml;hren

  \date 2005/03

  <h3>Prerequisites</h3>
  <ul>
    <li>Hermite1D   -- A class to implement Hermite Polynomials.
    <li>Shapelets1D -- A class to implement 1-dimensional Shapelet functions.
    <li>Shapelets2D -- A class to implement 2-dimensional Shapelet functions.
  </ul>
*/

// C++ Standard library
#include <fstream>

// Custom header files
#include <Functionals/Shapelets2D.h>

using LOPES::Shapelets2D;

// =============================================================================

/*!
  \brief Display the current parameters values of a Shapelets2D object

  \param phi -- A Shapelets2D object
 */
void printShapelets2D (Shapelets2D& phi) {

  cout << endl;
  cout << " - Shapelet order  : " << phi.order() << endl;
  cout << " - Scale parameter : " << phi.beta() << endl;
  
}

// =============================================================================

/*!
  \brief Test constructors of Shapelets2D

  Constructors for a 2-dimensional Shapelet are just the same as for an
  1-dimensional.

  \param order -- Maximum order \f$ n \f$ of the Shapelet basis function
                  \f$ \phi_{n}(x) \f$ or \f$ B_n(x;\beta) \f$.
  \param x     -- A set of values for which the basis functions will be
                  evaluated.
 */
bool test_Shapelets2D (const int& order,
		       const vector<double>& x)
{
  cout << "\n[test_Shapelets2D] Testing default constructor" << endl;
  {
    Shapelets2D phi;
    //
    printShapelets2D (phi);
  }
  
  cout << "\n[test_Shapelets2D] Testing argumented constructor" << endl;
  {
    Shapelets2D phi (order);    
    //
    printShapelets2D (phi);
  }  
  
  cout << "\n[test_Shapelets2D] Testing argumented constructor" << endl;
  {
    double beta (2.0);
    //
    Shapelets2D phi (order,beta);    
    //
    printShapelets2D(phi);
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
  cout << "\n[test_Shapelets2D] Testing adjustment of parameters" << endl;

  int newOrder (15);
  double newBeta (0.12345);
  Shapelets2D phi;
  
  {
    // display initial shapelet parameters
    printShapelets2D (phi);
    
    cout << "Adjusting Shapelet order : " << order << " -> " << newOrder << endl;
    phi.setOrder (newOrder);
    printShapelets2D (phi);
    
    cout << "Adjusting scale parameter : " << phi.beta() << " -> " << newBeta 
	 << endl;
    phi.setBeta (newBeta);
    printShapelets2D (phi);
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
  cout << "\n[testShapelets2D] Testing evaluation of function" << endl;

  int nelem (x.size());  // Number of elements in the input vector
  ofstream logfile;      // Stream for logfile taking computation output.
  Shapelets2D phi (order);

  logfile.open("tShapelets2D.data1",ios::out);

  cout << " - Computing and exporting results to disk ... " << flush;
  {
    double y;
    
    // loop over the test values
    for (int nx=0; nx<nelem; nx++) {
      for (int ny=0; ny<nelem; ny++) {
	//
	logfile << nx << "\t" << ny << "\t" << x[nx] << "\t" << x[ny];
	// loop over the basis function combinations (l,m)
	for (int l=0; l<order; l++) {
	  for (int m=0; m<order; m++) {
	    // function evaluation
	    y = phi.fx (l,x[nx],m,x[ny]);
	    // store the computed value
	    logfile << "\t" << y;
	  }
	}
	// end loop (l,m)
	logfile << endl;
      }
      logfile << endl;
    }
    //
  }
  cout << "Done." << endl;

  logfile.close();
  
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
  cout << "Values for test of polynomial evaluation:" << endl;
  for (unsigned int i=0; i<x.size(); i++) {
    x[i] = 5.0*(1.0*i-nx)/nx;
  }
  
  // Test: Object construction
  {
    ok = test_Shapelets2D (order,x);
  }

  // Test: Access to the basis function parameters
  {
    ok = test_parameters (order,x);
  }

  // Test: Evaluation of the Shapelet basis function
  {
    ok = test_eval (order,x);
  }

  return 0;

}

