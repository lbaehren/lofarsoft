#include "MEAL/StokesError.h"

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

  double evaluate (unsigned i, double* grad);

protected:

  vector< Jones<double> > Jgrad;
  Jones<double> J;

  Vector< 4, complex<double> > input;
  Vector< 4, double > variance;

  MEAL::Polar polar;

  unsigned tests;
  unsigned errors;
};

int main ()
{
  // MEAL::Function::very_verbose = true;

  MEAL::StokesError test;

  // test the identity
  Jones<double> J = 1;
  test.set_transformation (J);

  Stokes<double> input (1,2,.5,.6);
  test.set_variance (input);

  Stokes<double> output = test.get_variance ();

  if (input != output) {
    cerr << "FAIL! identity\n"
      "input = " << input << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple identity test passed" << endl;

  MEAL::Rotation rotation;

  cerr << "set rotation angle" << endl;

  rotation.set_param (0, M_PI/4);

  cerr << "set transformation" << endl;

  test.set_transformation (rotation.evaluate());

  cerr << "get variance" << endl;

  output = test.get_variance ();

  Stokes<double> expect (1,2,.6,.5);

  if (expect != output)
  {
    cerr << "FAIL! simple rotation\n"
      "expect = " << expect << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple rotation test passed" << endl;

  J = 0;
  test.set_transformation_gradient (J);
  output = test.get_variance_gradient ();

  expect = 0;
  if (expect != output) {
    cerr << "FAIL! null gradient\n"
      "expect = " << expect << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple null gradient test passed" << endl;

  vector< Jones<double> > grad;

  Stokes<double> input2 (3.63323e-08,3.41734e-08,3.57376e-08,3.55304e-08);

  test.set_variance (input2);
  test.set_transformation( rotation.evaluate(&grad) );

  test.set_transformation_gradient (grad[0]);
  output = test.get_variance_gradient ();

  if (output[2] == 0 || output[3] == 0) {
    cerr << "FAIL! non-zero gradient\n"
      "gradient = " << grad[0] << "\n"
      "output= " << output << endl;
    return -1;
  }

  cerr << "simple non-zero gradient test passed" << endl;

  double delta_phi = 1e-3;

  test.set_transformation( rotation.evaluate() );
  output = test.get_variance ();

  rotation.set_param(0, rotation.get_param(0) + delta_phi);
  test.set_transformation( rotation.evaluate() );

  Stokes<double> output2 = test.get_variance ();
  Stokes<double> diff = output2 - output;

  rotation.set_param(0, rotation.get_param(0) - 0.5*delta_phi);
  test.set_transformation( rotation.evaluate(&grad) );
  test.set_transformation_gradient (grad[0]);

  expect = delta_phi * test.get_variance_gradient ();

  double ndiff = norm(diff - expect) / norm(diff);

  if (ndiff > 1e-6) {
    cerr << "FAIL! differential gradient test\n"
      "expect = " << expect << "\n"
      "diff = " << diff << endl;
    return -1;
  }

  cerr << "differential gradient test passed" << endl;

  Test t;

  cerr << "all tests passed" << endl;
  return 0;
}

Test::Test ()
{
  cerr << "Testing del against small delta" << endl;

  MEAL::RandomPolar random;

  tests = 0;
  errors = 0;

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

  if (failure_rate > 15)
    exit (-1);
}

void Test::testgrad (unsigned ig)
{
  double grad = 0;
  double val = evaluate (ig, &grad);

  // cerr << "grad[" << ig << "]=" << grad << endl;

  double param = polar.get_param(ig);

  double step = 1e-14 * param;
  double expect_diff = step * grad;

  polar.set_param (ig, param - 0.5*step);
  J = polar.evaluate (&Jgrad);
  double grad1;
  double val1 = evaluate (ig, &grad1);

  polar.set_param (ig, param + 0.5*step);
  J = polar.evaluate (&Jgrad);
  double grad2;
  double val2 = evaluate (ig, &grad2);

  polar.set_param (ig, param);
  J = polar.evaluate (&Jgrad);

  double curv = grad2-grad1;

  expect_diff += 0.5 * curv * step * step;

  double diff = val2 - val1;

  if ( fabs((diff - expect_diff)/diff) > 1e-2 &&
       fabs((diff - expect_diff)/curv) > 1 ) {

#if 0
    cerr << "val=" << val << " grad=" << grad << endl;
    cerr << "1: val=" << val1 << " grad=" << grad1 << endl;
    cerr << "2: val=" << val2 << " grad=" << grad2 << endl;
    
    cerr << "diff=" << val2-val1 << " expected=" << expect_diff << endl;
    cerr << "curv=" << curv << endl;
#endif

    errors ++;
  }

  tests ++;
}

double Test::evaluate (unsigned ig, double* grad)
{
  MEAL::StokesError test;
  test.set_variance (variance);
  test.set_transformation (J);
  test.set_transformation_gradient (Jgrad[ig]);

  Stokes<double> var = test.get_variance();
  Stokes<double> vargrad = test.get_variance_gradient();


  Stokes< complex<double> > S (input);
  Jones<double> rho = convert (S);

  Jones<double> rhop = J * rho * herm(J);
  Jones<double> delrhop = Jgrad[ig]* rho * herm(J) + J * rho * herm(Jgrad[ig]);

  S = complex_coherency (rhop);
  Stokes< complex<double> > Sgrad = complex_coherency (delrhop);

  double result = 0;
  double result_grad = 0;

  for (unsigned i=0; i < 4; i++) {

#if 1

    result += norm(S[i]) / var[i];
    result_grad += 
      (2*(conj(S[i])*Sgrad[i]).real() - norm(S[i])*vargrad[i]/var[i]) / var[i];

#else

    // a simpler test; does not involve the variance
    result += norm(S[i]);
    result_grad += 2*(conj(S[i])*Sgrad[i]).real();

#endif

  }

  *grad = result_grad;
  return result;

}
