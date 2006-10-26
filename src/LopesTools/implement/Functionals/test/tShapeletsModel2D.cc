
/* $Id: tShapeletsModel2D.cc,v 1.1 2005/07/15 10:10:38 bahren Exp $ */

#include <cmath>

#include <Functionals/ShapeletsModel2D.h>

/*!
  \file tShapeletsModel2D.cc
  
  \brief A collection of tests for ShapeletsModel2D.
  
  \author Lars B&auml;hren

  \date 2005/04
*/

// === Display of the model parameters =========================================

/*!
  \brief Show current values of the 2-dim Shapelet model.
*/
void printModel (ShapeletsModel2D& model)
{
  vector<double> center (2);
  int order (0);
  int nx (0);
  int ny (0);

  order = model.order();
  center = model.center();

  cout << endl;

  cout << " - Maximum shapelet order     : " << order << endl;
  cout << " - Shapelet scale parameter   : " << model.beta() << endl;
  cout << " - Center position of profile : " << center[0] << " , "
       << center[1] << endl;
  
  cout << " - Weighting coefficients ... : " << endl;
  for (nx=0; nx<order; nx++) {
    cout << "\t";
    for (ny=0; ny<order; ny++) {
      cout << " " << model.coefficients(nx,ny);
    }
    cout << endl;
  }

//   cout << " - Integration over x-axis    : " << model.integral() << endl;

  cout << endl;
}

// === Misc. tests =============================================================

/*!
  \brief Test handling of 2-dimensional arrays.

  \param a  - 
  \param n1 - 
  \param n2 - 
*/
void test_arrays (float a[], int n1, int n2) {

  for (int j=0; j<n1; j++) {
    for (int i=0; i<n2; i++) {
      cout << i << "\t" << j << "\t" << a[i*n1+j] << endl;
    }
  }

}


// === Test constructors =======================================================

/*!
  \brief Test constructors of ShapeletsModel2D.

  \param order  - Maximum order for which the function coefficients are
                  pre-computed.
  \param beta   - Shapelet scale parameter, \f$ \beta \f$.
  \param center - Center position of the profile, \f$ \mathbf{x}_{0} \f$.

  \return ok - Status.
*/
bool test_ShapeletsModel2D (int order, 
			    double beta,
			    vector<double>& center)
{
  cout << "\n[ShapeletsModel2D] Testing ShapeletsModel2D constructors\n"
       << endl;
  
  // Test 1 : 
  cout << " [1] Empty constructor" << endl;
  {
    ShapeletsModel2D model;
    //
    printModel (model);
  }

  // Test 2 : 
  cout << " [2] Construction providing Shapelet parameters" << endl;
  {
    ShapeletsModel2D model (order,beta);
    //
    printModel (model);
  }

  return true;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  bool ok (true);
  int order (15);
  double beta (2.0);
  vector<double> center (2);

  center[0] = 1.0;
  center[1] = 5.0;

//   {
//     int n1 (5);
//     int n2 (5);
//     float *a;
    
//     a = new float[n1*n2];
    
//     test_arrays (a,n1,n2);
//   }

  // Test constructors of ShapeletsModel2D
  {
    ok = test_ShapeletsModel2D (order, beta, center);
  }

  return 0;

}
