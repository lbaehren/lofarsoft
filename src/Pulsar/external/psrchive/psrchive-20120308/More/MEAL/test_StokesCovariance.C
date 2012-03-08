#include "MEAL/StokesCovariance.h"

#include "MEAL/Rotation.h"
#include "MEAL/Polar.h"
#include "MEAL/RandomPolar.h"

#include "Pauli.h"

#include <iostream>

using namespace std;

class Test {

public:

  Test ();

  void testgrad (unsigned i);

  Matrix<4,4,double> evaluate (unsigned i, Matrix<4,4,double>* grad);

  MEAL::Polar polar;

protected:

  vector< Jones<double> > Jgrad;
  Jones<double> J;

  Vector< 4, complex<double> > input;
  Vector< 4, double > variance;

  unsigned tests;
  unsigned errors;

  bool verbose;
};

int test (string name, Matrix<4,4,double>& covar, Stokes<double>& input)
{
  double tolerance = 1e-15;

  for (unsigned i=0; i<4; i++) {
    for (unsigned j=0; j<4; j++) {
      if (i==j) {
	double diff = fabs(covar[i][i] - input[i]);
	if (diff > tolerance) {
	  cerr << "FAIL! " << name << " diff[" << i << "]=" << diff << "\n"
	    "input = " << input << "\n"
	    "covar =\n" << covar << endl;
	  return -1;
	}
      }
      else if (fabs(covar[i][j]) > tolerance) {
	cerr << "FAIL! " << name << " covar[" << i << "," << j << "]="
	     << covar[i][j] << endl;
	return -1;
      }
    }
  }
  return 0;
}

int main ()
{
  MEAL::StokesCovariance test_covar;

  // test the identity
  Jones<double> J = 1;
  test_covar.set_transformation (J);

  Stokes<double> input (1,2,.5,.6);
  test_covar.set_variance (input);

  Matrix<4,4,double> covar = test_covar.get_covariance();

  if (test ("identity", covar, input) < 0)
    return -1;

  cerr << "simple identity test passed" << endl;

  MEAL::Rotation rotation;
  rotation.set_param (0, M_PI/4);

  test_covar.set_transformation (rotation.evaluate());
  covar = test_covar.get_covariance();

  Stokes<double> expect (1,2,.6,.5);

  if (test ("simple rotation", covar, expect) < 0)
    return -1;

  cerr << "simple rotation test passed" << endl;

  Test t;

  return 0;
}



Test::Test ()
{
  cerr << "Testing partial derivatives using small delta parameters" << endl;

  MEAL::RandomPolar random;

  tests = 0;
  errors = 0;
  verbose = false;

  for (unsigned i=0; i < 1000; i++) {

    random.get (&polar);
    J = polar.evaluate( &Jgrad );

    random_vector (input, 10.0);

    random_vector (variance, 0.6);
    for (unsigned i=0; i<4; i++)
      variance[i] *= variance[i];

    for (unsigned ig=0; ig < Jgrad.size(); ig++)
      testgrad (ig);

  }

  double failure_rate = double(errors)/double(tests)*100.0;

  cerr << "Failed " << errors << " out of " << tests << " times ("
       << failure_rate << "%)" << endl;

  if (failure_rate > 12)
    exit (-1);
}

void Test::testgrad (unsigned ig)
{
  Matrix<4,4,double> grad;
  Matrix<4,4,double> val = evaluate (ig, &grad);

  double param = polar.get_param(ig);

  double step = 1e-14 * param;

  polar.set_param (ig, param - 0.5*step);
  J = polar.evaluate (&Jgrad);
  Matrix<4,4,double> grad1;
  Matrix<4,4,double> val1 = evaluate (ig, &grad1);

  polar.set_param (ig, param + 0.5*step);
  J = polar.evaluate (&Jgrad);
  Matrix<4,4,double> grad2;
  Matrix<4,4,double> val2 = evaluate (ig, &grad2);

  polar.set_param (ig, param);
  J = polar.evaluate (&Jgrad);

  Matrix<4,4,double> curv = grad2 - grad1;

  Matrix<4,4,double> expect_diff = grad;
  expect_diff *= step;

  Matrix<4,4,double> expect2 = curv;

  expect2 *= 0.5 * step * step;
  expect_diff += expect2;

  Matrix<4,4,double> diff = val2 - val1;

  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++) {

      double Diff = diff[i][j];
      double Expect = expect_diff[i][j];

      if ( fabs((Diff - Expect)/Diff) > 1e-2 &&
	   fabs((Diff - Expect)/curv[i][j]) > 1 )
	errors ++;

      tests ++;

    }

}

Matrix<4,4,double> Test::evaluate (unsigned ig, Matrix<4,4,double>* grad)
{
  MEAL::StokesCovariance test;
  test.set_variance (variance);
  test.set_transformation (J);
  test.set_transformation_gradient (Jgrad[ig]);

  *grad = test.get_covariance_gradient();
  return test.get_covariance();
}

