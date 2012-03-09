#include "Pulsar/TotalCovariance.h"
#include "MEAL/StokesCovariance.h"

#include "MEAL/Polar.h"
#include "MEAL/RandomPolar.h"
#include "MEAL/Rotation.h"

#include "Pauli.h"

#include <iostream>

using namespace std;

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
  MEAL::Polar polar;

  Calibration::TotalCovariance total_covar;
  total_covar.set_transformation (&polar);

  MEAL::RandomPolar random;

  Vector< 4, complex<double> > input1;
  Vector< 4, complex<double> > input2;
  Vector< 4, double > variance;

  unsigned trials = 1000;
  for (unsigned i=0; i < trials; i++) {

    // simulate a Lorentz transformation
    random.get (&polar);
    Jones<double> J = polar.evaluate();

    // simulate a depolarizer
    Matrix<4,4,double> M;

    random_matrix (M, 10.0);
    for (unsigned i=1; i<4; i++)
      M[0][i] = 0.0;
    M[0][0] = 1.0;

    total_covar.set_optimizing_transformation (M);

    // simulate some complex Stokes parameters to enter the scalar product
    random_vector (input1, 10.0);
    random_vector (input2, 10.0);

    // simulate the un-transformed variances
    random_vector (variance, 0.6);
    for (unsigned i=0; i<4; i++)
      variance[i] *= variance[i];

    total_covar.set_template_variance ( Stokes<double>(variance) );

    // compute the covariance matrix
    MEAL::StokesCovariance compute;
    compute.set_variance (variance);
    compute.set_transformation (M * Mueller(J));

    Matrix<4,4,double> inv_covar = inv(compute.get_covariance());

    // compute the scalar product using the formula in the paper
    double scalar0 = (input1 * (inv_covar * conj(input2))).real();

    // compute the scalar product as done in PolnProfileFitAnalysis
    Stokes<double> r1 = real(input1);
    Stokes<double> i1 = imag(input1);

    Stokes<double> r2 = real(input2);
    Stokes<double> i2 = imag(input2);

    double scalar1 = r1 * (inv_covar * r2) + i1 * (inv_covar * i2);

    double diff = fabs( (scalar0 - scalar1)/scalar0 );
    if (diff > 1e-10) {
      cerr << "method 0 and 1 differ" << endl
	   << "scalar0=" << scalar0 << endl
	   << "scalar1=" << scalar1 << endl
	   << "diff=" << diff << endl
	   << "inv_covar=\n" << inv_covar << endl
	   << "input1=" << input1 << endl
	   << "real=" << r1 << endl
	   << "imag=" << i1 << endl
	   << "input2=" << input2 << endl
	   << "real=" << r2 << endl
	   << "imag=" << i2 << endl;
      return -1;
    }

    // compute the scalar product as done in LevenbergMarquardt

    //! The traits of the gradient element
    ElementTraits< Jones<double> > traits;

    Jones<double> J1 = convert( Stokes< complex<double> > (input1) );
    Jones<double> J2 = convert( Stokes< complex<double> > (input2) );

    Jones<double> wJ1 = total_covar.get_weighted_conjugate (J1);

    double scalar2 = 2.0 * traits.to_real (wJ1 * J2);

    diff = fabs( (scalar0 - scalar2)/scalar0 );
    if (diff > 1e-10) {
      cerr << "method 0 and 2 differ" << endl
	   << "scalar0=" << scalar0 << endl
	   << "scalar2=" << scalar2 << endl
	   << "diff=" << diff << endl
	   << "inv_covar=\n" << inv_covar << endl
	   << "input1=" << input1 << endl
	   << "Jones1=" << J1 << endl
	   << "weighted=" << wJ1 << endl
	   << "input2=" << input2 << endl
	   << "Jones2=" << J2 << endl;
      return -1;
    }
  }

  cerr << "three inner product formulas tested on " << trials 
       << " random vector pairs" << endl;

  return 0;
}

