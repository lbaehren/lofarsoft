
/* $Id: tShapeletsModel1D.cc,v 1.1 2005/07/15 10:10:38 bahren Exp $ */

// C++ Standard library
#include <fstream>
using namespace std;

#include <Functionals/ShapeletsModel1D.h>

/*!
  \file tShapeletsModel1D.cc
  
  \brief A collection of tests for ShapeletsModel1D.
  
  \author Lars B&auml;hren

  \date 2005/04
 */

// =============================================================================

/*!
  \brief Show current values of the 1-dim Shapelet model.
*/
void printModel (ShapeletsModel1D& model)
{
  int nofCoeff (model.coefficients().size());
  vector<double> coeff (model.coefficients());

  cout << endl;

  cout << " - Maximum shapelet order     : " << model.order() << endl;
  cout << " - Shapelet scale parameter   : " << model.beta() << endl;
  cout << " - Center position of profile : " << model.center() << endl;
  
  cout << " - Weighting coefficients ... : [";
  for (int n=0; n<nofCoeff; n++) {
    cout << " " << coeff[n];
  }
  cout << " ]" << endl;

  cout << " - Integration over x-axis    : " << model.integral() << endl;

  cout << endl;
}


// =============================================================================

/*!
  \brief Test construction of a ShapeletsModel1D object.
*/
void test_ShapeletsModel1D () {
  
  cout << "\n[ShapeletsModel1D] Testing ShapeletsModel1D constructors\n"
       << endl;

  int order (10);
  double beta (1.0);
  double center (5.0);
  
  // Test 1: Empty constructor 
  cout << " [1] Empty constructor" << endl;
  {
    ShapeletsModel1D model;
    //
    printModel (model);
  }
  
  // Test 2: Construction providing Shapelet parameters
  cout << " [2] Construction providing Shapelet parameters" << endl;
  {
    ShapeletsModel1D model (order,beta);
    //
    printModel (model);
  }
  
  // Test 3: Construction providing model parameters
  cout << " [3] Construction providing model parameters" << endl;
  {
    vector<double> coeff;
    coeff.resize(order+1,1.0);
    //
    ShapeletsModel1D model (center,coeff);
    //
    printModel (model);
    //
    coeff.clear();
  }
  
  // Test 4: Construction providing maximum set of parameters
  cout << " [4] Construction providing maximum set of parameters" << endl;
  {
    vector<double> coeff;
    coeff.resize(order+1,1.5);
    //
    ShapeletsModel1D model (order,beta,center,coeff);
    //
    printModel (model);
    //
    coeff.clear();
  }
  
}

// =============================================================================

/*!
  \brief Test for access to model parameters
  
  Provides test for ShapeletsModel1D::setBeta, ShapeletsModel1D::setCenter,
  ShapeletsModel1D::setCoefficients.

  \param order - Maximum order for which the function coefficients are
                 pre-computed.
  \param beta  - Shapelet scale parameter, \f$ \beta \f$.

  \return ok - Status.
*/
bool test_parameters (int order, 
		      double beta) {
  
  cout << "\n[ShapeletsModel1D] Testing access to model parameters\n" << endl;

  double center (5.0);
  vector<double> coeff;
  
  coeff.resize(order+1,1.0);
  
  ShapeletsModel1D model (order,beta,center,coeff);
  
  // Test 1: adjust scale parameter.
  cout << " [1] Adjusting the scale parameter" << endl;
  {
    printModel (model);
    //
    beta *= 2.0;
    model.setBeta(beta);
    //
    printModel(model);
  }
  
  // Test 2: adjust the profile center position
  cout << " [2] Adjusting the profile center" << endl;
  {
    printModel(model);
    //
    center = 0.0;
    model.setCenter(center);
    //
    printModel(model);
  }
  
  // Test 3: adjust the weighting coefficients
  cout << " [3] Adjusting coefficients" << endl;
  {
    printModel(model);
    //
    coeff.assign(order+1,2.0);
    model.setCoefficients(coeff);
    //
    printModel(model);
    //
    coeff.assign(order+1,4.0);
    model.setCoefficients(coeff);
    //
    printModel(model);
  }

  return true;
}

// =============================================================================

/*!
  \brief Test evaluation of the model function.

  \param order - Maximum order for which the function coefficients are
                 pre-computed.
  \param beta  - Shapelet scale parameter, \f$ \beta \f$.

  \return ok - Status.
*/
bool test_eval (int order, double beta) {
  
  cout << "\n[ShapeletsModel1D] Testing evaluation of model function\n" << endl;

  double x (0.0);
  double center (0.0);
  
  // Test 1 : 
  {
    Shapelets1D phi (order,beta);
    ShapeletsModel1D model (order,beta);
    vector<double> coeff (order+1,0.0);

    for (int n=0; n<=order; n++) {
      coeff.assign(order+1,0.0);
      coeff[n] = 1.0;
      model.setCoefficients (coeff);
      //
      cout << "\tn=" << n << "\tphi=" << phi.fx(n,x) << "\tmodel="
	   << model.fx(x)<< endl;
    }
  }

  // Test 2 :
  {
    // Set center of the profile
    center = 0.0;
    // Set up the model coefficients
    vector<double> coeff1 (order+1,0.0);
    vector<double> coeff2 (order+1,0.0);
    //
    coeff1[0] = coeff2[0] = 1.0;
    for (int n=1; n<=order; n++) {
      coeff1[n] = 0.8*coeff1[n-1];
      coeff2[n] = -0.8*coeff1[n-1];
    }
    // New model
    ShapeletsModel1D model1 (order,beta,center,coeff1);
    ShapeletsModel1D model2 (order,beta,center,coeff2);
    double rmsRadius (model1.rmsRadius(order));
    double dx (model1.resolution(order)/2);
    double minRadius (2*rmsRadius);
    int nofPoints (int(2*minRadius/dx+1));
    //
    ofstream logfile;
    logfile.open("tShapeletsModel1D.data1",ios::out);
    //
    for (int n=0; n<nofPoints; n++) {
      x = n*dx - minRadius;
      logfile << "\t" << n << "\t" << x << "\t" << model1.fx(x)
	      << "\t" << model2.fx(x) << endl;
    }
    logfile.close();
  }
  
  return true;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {
  
  bool ok;
  int order (10);
  double beta (1.0);

  // Test for the constructors
  test_ShapeletsModel1D();
  
  // Test access to the model parameters
  {
    ok = test_parameters(order,beta);
  }
  
  // Test the evaluation of the model
  {
    ok = test_eval (order,beta);
  }

  return 0;
  
}
