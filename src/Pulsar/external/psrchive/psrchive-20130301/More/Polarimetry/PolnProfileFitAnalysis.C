/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileFitAnalysis.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/TotalCovariance.h"

#include "MEAL/ProductRule.h"
#include "MEAL/PhaseGradients.h"

#include "Pauli.h"
#include "Jacobi.h"

#include <fstream>
#include <assert.h>

// #define _DEBUG 1

using namespace std;

bool Pulsar::PolnProfileFit::Analysis::verbose = false;

Pulsar::PolnProfileFit::Analysis::Analysis ()
{
  built = false;
  compute_error = true;
  max_harmonic = 0;

  matrix_identity (Xi);
}

//! When set, estimate the uncertainty in each attribute
void Pulsar::PolnProfileFit::Analysis::set_compute_error (bool flag)
{
  compute_error = flag;
}

void Pulsar::PolnProfileFit::Analysis::set_harmonic (unsigned index)
{
  double phase_shift = -2.0 * M_PI * double(index+1);

  fit->phase_axis.set_value (phase_shift);
  fit->equation->set_input_index (index);
}

void Pulsar::PolnProfileFit::Analysis::evaluate (unsigned index)
{
  set_harmonic (index);

  static vector< Jones<double> > temp_gradient;
  model_result = fit->equation->evaluate (&temp_gradient);

  phase_result = fit->phases->evaluate (&phase_gradient);

  // assert( temp_gradient.size() == 10 );

  model_gradient.resize(8);

  unsigned J_start = 0;

  if (fit->equation->get_param_name(0) == "phi_0")
  {
    model_gradient[0] = temp_gradient[0];
    J_start = 1;
  }
  else if (fit->equation->get_param_name(7) == "phi_0")
  {
    model_gradient[0] = temp_gradient[7];
    J_start = 0;
  }
  else
  {
    for (unsigned i=0; i<fit->equation->get_nparam(); i++)
      cerr << i << " " << fit->equation->get_param_name(i) << endl;

    throw Error (InvalidState, "Pulsar::PolnProfileFit::Analysis::evaluate",
		 "model parameters in unknown order");
  }

  for (unsigned j=0; j < 7; j++)
    model_gradient[j+1] = temp_gradient[j+J_start];
}


/*!
  Computes Equation 14 of van Straten (2006)
*/
void
Pulsar::PolnProfileFit::Analysis::get_curvature (Matrix<8,8,double>& alpha) try
{
  if (verbose)
    cerr << "Pulsar::PolnProfileFit::Analysis::get_curvature" << endl;

  alpha = Matrix<8,8,double>();

  initialize();

  unsigned nharmonic = fit->equation->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++)
  {
    evaluate (ih);
    add_curvature (alpha);
  }

  //cerr << "ALPHA=\n" << alpha << endl;
}
 catch (Error& error)
   {
     throw error += "Pulsar::PolnProfileFit::Analysis::get_curvature";
   }

void 
Pulsar::PolnProfileFit::Analysis::add_curvature (Matrix<8,8,double>& alpha) try
{
  for (unsigned ir=0; ir < 8; ir++)
  {
    Stokes< complex<double> > dr = complex_coherency(model_gradient[ir]);

    for (unsigned is=0; is <= ir; is ++)
    {
      Stokes< complex<double> > ds = complex_coherency(model_gradient[is]);

      alpha[ir][is] += (dr * Xi * conj(ds)).real();

      if (ir != is)
        alpha[is][ir] = alpha[ir][is];
    }
  }
}
 catch (Error& error)
   {
     throw error += "Pulsar::PolnProfileFit::Analysis::add_curvature";
   }

Jones<double> 
Pulsar::PolnProfileFit::Analysis::del_deleta (unsigned i,
					    const Jones<double>& K) const
{
  if (i == 0)
    return xform_result * K * herm(xform_result) * phase_gradient[0];

  i --;

  return ( xform_gradient[i] * K * herm(xform_result) +
	   xform_result * K * herm(xform_gradient[i]) ) * phase_result;
}

Jones<double> Pulsar::PolnProfileFit::Analysis::delrho_delS (unsigned k) const
{
  Stokes<double> q;
  q[k] = 1.0;
  return convert(q);
}

void Pulsar::PolnProfileFit::Analysis::delC_delS
( Matrix<8,8,double>& delC_delSre,
  Matrix<8,8,double>& delC_delSim, unsigned k ) const
{
  Matrix<8,8,double> delalpha_delSre;
  Matrix<8,8,double> delalpha_delSim;

  Jones<double> delR_delS = delrho_delS(k);

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {

    Stokes<complex<double> > dr=complex_coherency(model_gradient[ir]);
    Stokes<complex<double> > d2r=complex_coherency(del_deleta(ir,delR_delS));

    // over all columns up to and including the diagonal
    for (unsigned is=0; is <= ir; is ++) {
	  
      Stokes<complex<double> > ds=complex_coherency(model_gradient[is]);
      Stokes<complex<double> > d2s=complex_coherency(del_deleta(is,delR_delS));

      delalpha_delSre[ir][is] = ( d2r*Xi*conj(ds) + dr*Xi*conj(d2s) ).real();

      complex<double> i (0,1);
      d2r *= i;
      d2s *= i;

      delalpha_delSim[ir][is] = ( d2r*Xi*conj(ds) + dr*Xi*conj(d2s) ).real();

      if (is != ir) {
	delalpha_delSre[is][ir] = delalpha_delSre[ir][is];
	delalpha_delSim[is][ir] = delalpha_delSim[ir][is];
      }

    }
    
  }

  delC_delSre = -covariance * delalpha_delSre * covariance;
  delC_delSim = -covariance * delalpha_delSim * covariance;
}

/*! Return the partial derivative of the multiple correlation squared with
  respect to the real or imaginary component of a Stokes parameter */

double 
Pulsar::PolnProfileFit::Analysis::delR2_varphiJ_delS 
(Matrix<8,8,double>& delC_delS) 
{
  // partial derivatives of conformal partition of covariance matrix
  Matrix <7,7,double> delC_JJ_delS;
  Vector <7,double> delC_varphiJ_delS;
  double delc_varphi_delS = 0;
      
  // partition the partial derivative of the covariance matrix
  partition (delC_delS, delc_varphi_delS,
	     delC_varphiJ_delS, delC_JJ_delS);

  return ( delC_varphiJ_delS * (inv_C_JJ * C_varphiJ)
	   - C_varphiJ * (inv_C_JJ * delC_JJ_delS * inv_C_JJ * C_varphiJ) 
	   + C_varphiJ * (inv_C_JJ * delC_varphiJ_delS)
	   - R2_varphiJ * delc_varphi_delS ) / c_varphi;
}

void Pulsar::PolnProfileFit::Analysis::initialize () try
{
  if (verbose)
    cerr << "Pulsar::PolnProfileFit::Analysis::initialize" << endl;

  xform_result = xform->evaluate (&xform_gradient);

  if (verbose)
    cerr << "Pulsar::MTMAnalysis::initialize xform=\n" << xform_result << endl;

  stokes_covariance.set_transformation ( xform_result );

  Matrix<4,4,double> covar = stokes_covariance.get_covariance();

  if (verbose)
    cerr << "Pulsar::MTMAnalysis::initialize covar=\n" << covar << endl;

  Xi = inv(covar);

  if (verbose)
    cerr << "Pulsar::MTMAnalysis::initialize Xi=\n" << Xi << endl;
}
 catch (Error& error)
   {
     throw error += "Pulsar::PolnProfileFit::Analysis::initialize";
   }

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFit::Analysis::set_fit (PolnProfileFit* f)
{
  fit = f;
  built = false;

  if (!fit)
    return;

  // by default, the PolnProfileFit is empty and has no phase gradients added
  fit->phases->resize(1);

  Stokes<double> avg_var;
  for (unsigned i=0; i<4; i++)
    avg_var[i] = 0.5 *
      ( fit->standard_variance[i].real() + fit->standard_variance[i].imag() );

  if (verbose)
    cerr << "Pulsar::PolnProfileFit::Analysis::set_fit"
      " stokes var=" << avg_var << endl;

  stokes_covariance.set_variance (avg_var);
  xform = fit->get_transformation();
}


template<typename T> T sqr (T x) { return x*x; }

//! Computes the above three numbers
void Pulsar::PolnProfileFit::Analysis::build (bool only_relative_error) try
{
  Matrix<8,8,double> curvature;

#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFit::Analysis::set_fit get_curvature" << endl;
#endif

  // calculate the curvature matrix
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  //cerr << "curv=\n" << curvature << endl;
  //cerr << "cov=\n" << covariance << endl;

  if (!only_relative_error) {

    // partition the covariance matrix
    partition (covariance, c_varphi, C_varphiJ, C_JJ);

    // calculate the inverse of the Jones parameter covariance matrix
    inv_C_JJ = inv(C_JJ);
    
    // the multiple correlation squared
    R2_varphiJ = C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi;
    
  }

  // /////////////////////////////////////////////////////////////////////
  //
  // now do the same for scalar template matching
  //
  // /////////////////////////////////////////////////////////////////////

  ScalarProfileFitAnalysis scalar;
  scalar.set_fit (fit);

  if (max_harmonic && max_harmonic < fit->equation->get_num_input())
    scalar.set_max_harmonic( max_harmonic );

  Matrix<2,2,double> I_curvature;
  scalar.get_curvature (I_curvature);

  Matrix<2,2,double> I_covariance = inv(I_curvature);

  //cerr << "I_curv=\n" << I_curvature << endl;
  //cerr << "I_cov=\n" << I_covariance << endl;
  
  // the relative unconditional phase shift variance
  double hatvar_varphi = covariance[0][0] / I_covariance[0][0];

  // the relative conditional phase shift variance
  double hatvar_varphiJ = hatvar_varphi * (1-R2_varphiJ);

  if (!compute_error) {
    multiple_correlation = sqrt(R2_varphiJ);
    relative_error = sqrt(hatvar_varphi);
    relative_conditional_error = sqrt(hatvar_varphiJ);
    return;
  }

  // the variance of the relative unconditional phase shift variance
  double var_hatvar_varphi = 0.0;

  // the variance of the multiple correlation squared
  double var_R2_varphiJ = 0.0;

  // the variance of the relative conditional phase shift variance
  double var_hatvar_varphiJ = 0.0;

  // the variance of the phase shift variance
  double var_c_varphi = 0.0;

  unsigned nharmonic = fit->equation->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    evaluate (ih);

    // over all four stokes parameters
    for (unsigned ip=0; ip < 4; ip++) {

      // calculate the partial derivative of the covariance matrix wrt S_ip
      Matrix<8,8,double> delC_delSre;
      Matrix<8,8,double> delC_delSim;
      delC_delS (delC_delSre, delC_delSim, ip);

      // the variance of the real and imaginary parts of the Stokes parameters
      std::complex<double> var_S = fit->standard_variance[ip];

      var_c_varphi += sqr(delC_delSre[0][0]) * var_S.real();
      var_c_varphi += sqr(delC_delSim[0][0]) * var_S.imag();

      double delR2_varphiJ_delSre = 0;
      double delR2_varphiJ_delSim = 0;

      if (!only_relative_error) {

	delR2_varphiJ_delSre = delR2_varphiJ_delS (delC_delSre);
	delR2_varphiJ_delSim = delR2_varphiJ_delS (delC_delSim);

	var_R2_varphiJ += sqr(delR2_varphiJ_delSre) * var_S.real();
	var_R2_varphiJ += sqr(delR2_varphiJ_delSim) * var_S.imag();

#ifdef _DEBUG
      if (ip==3) {
	Estimate<double> R2 (R2_varphiJ, var_R2_varphiJ);
	multiple_correlation = sqrt(R2);

	cerr << ih << " R2 " << multiple_correlation.get_error() << endl;
      }
#endif

      }

      double delA_delSre = hatvar_varphi * delC_delSre[0][0]/covariance[0][0];
      double delA_delSim = hatvar_varphi * delC_delSim[0][0]/covariance[0][0];

      if (ip == 0) {

	Matrix<2,2,double> delC_delS0re;
	Matrix<2,2,double> delC_delS0im;

	scalar.delC_delS (delC_delS0re, delC_delS0im, ih);

	delA_delSre -= hatvar_varphi * delC_delS0re[0][0]/I_covariance[0][0];
	delA_delSim -= hatvar_varphi * delC_delS0im[0][0]/I_covariance[0][0];

      }

      var_hatvar_varphi += sqr( delA_delSre ) * var_S.real();
      var_hatvar_varphi += sqr( delA_delSim ) * var_S.imag();

      if (!only_relative_error) {

	// ... and the conditional variance
	double delB_delSre = 
	  delA_delSre * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSre;
	double delB_delSim = 
	  delA_delSim * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSim;
	
	var_hatvar_varphiJ += sqr( delB_delSre ) * var_S.real();
	var_hatvar_varphiJ += sqr( delB_delSim ) * var_S.imag();

      }

    }

  }

  Estimate<double> R2 (R2_varphiJ, var_R2_varphiJ);
  multiple_correlation = sqrt(R2);

  Estimate<double> hatvar (hatvar_varphi, var_hatvar_varphi);
  relative_error = sqrt(hatvar);

  Estimate<double> hatvarJ (hatvar_varphiJ, var_hatvar_varphiJ);
  relative_conditional_error = sqrt(hatvarJ);

  if (!only_relative_error)
    built = true;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileFit::Analysis::build";
}

//! Get the multiple correlation
double Pulsar::PolnProfileFit::Analysis::get_Rmult ()
{
  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  // partition the covariance matrix
  partition (covariance, c_varphi, C_varphiJ, C_JJ);

  // calculate the inverse of the Jones parameter covariance matrix
  inv_C_JJ = inv(C_JJ);

  // the multiple correlation squared
  R2_varphiJ = C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi;

  cerr << "Rmult = " << R2_varphiJ << endl;

  return R2_varphiJ;
}


//! Get the variance of varphi
double 
Pulsar::PolnProfileFit::Analysis::get_c_varphi ()
{
  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  try {

    // calculate the covariance matrix
    covariance = inv(curvature);

  }
  catch (Error& e) {
    cerr << "Pulsar::PolnProfileFit::Analysis::get_c_varphi error inv(curv)\n"
	 << e << "\ncurv=\n" << curvature << endl;
  }

  double c_varphi = covariance[0][0];

  return c_varphi;
}



void Pulsar::PolnProfileFit::Analysis::compute_weights (unsigned nharmonic) 
{

  ScalarProfileFitAnalysis scalar;
  scalar.set_fit (fit);

  weights.resize( nharmonic );
  store_covariance.resize( nharmonic );

  for (unsigned i=0; i<nharmonic; i++) try {

    scalar.set_max_harmonic( i + 1 );

    Matrix<2,2,double> I_curvature;
    scalar.get_curvature (I_curvature);

    Matrix<2,2,double> I_covariance = inv(I_curvature);

    weights[i] = 1.0 / I_covariance[0][0];

    if (I_covariance[0][0] <= 0)
      weights[i] = 0.0;

  }
  catch (Error& error) {
    weights[i] = 0;
  }
  

}

double Pulsar::PolnProfileFit::Analysis::get_expected_relative_error 
(std::vector<unsigned>& histogram)
{
  double M_tot = 0.0;
  double I_tot = 0.0;

  unsigned npts = histogram.size() -1;
  if (npts > weights.size())
    npts = weights.size();

  for (unsigned i=0; i < npts; i++) {
    if (weights[i] == 0)
      continue;
    unsigned count = histogram[i+1];
    double I_var = 1.0/weights[i];
    I_tot += count * I_var;
    double M_var = store_covariance[i];
    M_tot += count * M_var;
    cerr << i+1 << " " << count << " " << sqrt(M_var/I_var) << endl;
  }

  return sqrt (M_tot/I_tot);
}

//! Get the variance of varphi
double Pulsar::PolnProfileFit::Analysis::get_C_varphi ()
{
  initialize();

  unsigned nharmonic = fit->equation->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  if (weights.size() != nharmonic)
    compute_weights (nharmonic);

  double C_varphi = 0;

  unsigned min_harmonic = 1;

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;

  for (unsigned ih=0; ih < nharmonic; ih++)
  {
    evaluate (ih);

    add_curvature (curvature);

    store_covariance[ih] = 0;

    if (ih < min_harmonic)
      continue;

    // calculate the covariance matrix
    covariance = inv(curvature);

    if ( covariance[0][0] <= 0 )  {
      cerr << "minimum harmonic = " << ih + 1 << endl;
      min_harmonic = ih + 1;
    }

    store_covariance[ih] = covariance[0][0];

    C_varphi += weights[ih] * covariance[0][0];
  }

  if (C_varphi == 0)
    throw Error (InvalidState, "get_C", "Insufficient signal-to-noise ratio");

  return C_varphi;
}


//! Get the correlation coefficients
Matrix<8,8,double> Pulsar::PolnProfileFit::Analysis::get_correlation () const
{
  Matrix<8,8,double> C;
  for (unsigned i=0; i<8; i++)
    for (unsigned j=0; j<=i; j++)
      C[i][j] = C[j][i] = covariance[i][j] /
	sqrt (covariance[i][i]*covariance[j][j]);

  return C;
}

Estimate<double>
Pulsar::PolnProfileFit::Analysis::get_relative_error () const try
{
  if (!built)
    const_cast<PolnProfileFit::Analysis*>(this)->build();
  return relative_error;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfileFit::Analysis::get_relative_error";
}

Estimate<double>
Pulsar::PolnProfileFit::Analysis::get_multiple_correlation () const
{
  if (!built)
    const_cast<PolnProfileFit::Analysis*>(this)->build();
  return multiple_correlation;
}

Estimate<double>
Pulsar::PolnProfileFit::Analysis::get_relative_conditional_error () const
{
  if (!built)
    const_cast<PolnProfileFit::Analysis*>(this)->build();
  return relative_conditional_error;
}

double Pulsar::PolnProfileFit::Analysis::get_cond_var ()
{
  if (verbose)
    for (unsigned ir=0; ir < xform_gradient.size(); ir++)
      cerr << "basis[" << ir << "]=" 
	   << xform->get_param(ir) << endl;

  xform_result = xform->evaluate (&xform_gradient);

  error.set_transformation (xform_result);
  Stokes<double> variance = error.get_variance();

  double var = 0.0;

  for (unsigned ih=0; ih < fit->equation->get_num_input(); ih++)
  {
    unsigned bih = fit->equation->get_num_input() - ih -1;

    evaluate (bih);

    Stokes< complex<double> > S = complex_coherency(model_result);

    double weight = bih + 1;

    double this_var = 0;
    for (unsigned ipol=0; ipol<4; ipol++)
      this_var += weight * norm(S[ipol]) / variance[ipol];

    var += this_var;
  }
  
  var = 1/var;

  static double scale = 0;

  if (scale == 0) {
    scale = 1.0 / pow (10.0, floor(log(var)/log(10.0)));
    cerr << "scale=" << scale << endl;
  }

  var *= scale;

  return var;
}














// /////////////////////////////////////////////////////////////////////
//
// the same for scalar template matching
//
// /////////////////////////////////////////////////////////////////////

void Pulsar::ScalarProfileFitAnalysis::set_fit (const PolnProfileFit* fit)
{
  set_spectrum( fit->standard_fourier->get_Profile(0) );
  set_max_harmonic( fit->equation->get_num_input() );

  variance = fit->standard_variance[0];
}

void Pulsar::ScalarProfileFitAnalysis::set_spectrum (const Profile* p,
						     double rescale_variance)
{
  spectrum = p;
  amps = reinterpret_cast<const complex<float>*> (p->get_amps() + 2);
  variance *= rescale_variance;
}

void Pulsar::ScalarProfileFitAnalysis::set_max_harmonic (unsigned n)
{
  max_harmonic = n;
}

void Pulsar::ScalarProfileFitAnalysis::set_variance (double v)
{
  variance = v;
}


void
Pulsar::ScalarProfileFitAnalysis::get_curvature (Matrix<2,2,double>& curvature)
{
  curvature = Matrix<2,2,double>();

  complex<double> gradient[2];

  for (unsigned ih=0; ih < max_harmonic; ih++)
  {
    complex<double> delexp_delvarphi (0.0, -2.0 * M_PI * double(ih+1));
    complex<double> Stokes_I (amps[ih]);

    // partial derivative with respect to phase
    gradient[0] = Stokes_I * delexp_delvarphi;
    // partial derivative with respect to gain
    gradient[1] = Stokes_I;

    for (unsigned ir=0; ir < 2; ir++)
      for (unsigned is=0; is < 2; is ++)
	curvature[ir][is] += (conj(gradient[ir]) * gradient[is]).real();
  }

  curvature /= get_mean_variance();

  covariance = inv(curvature);
}

void Pulsar::ScalarProfileFitAnalysis::delC_delS
(
 Matrix<2,2,double>& delC_delSre,
 Matrix<2,2,double>& delC_delSim,
 unsigned index
 ) const
{
  complex<double> delexp_delvarphi (0.0, -2.0 * M_PI * double(index+1));
  complex<double> Stokes_I (amps[index]);

  Matrix<2,2,double> delalpha_delSre;
  Matrix<2,2,double> delalpha_delSim;

  complex<double> delgradient_delS[2];
  complex<double> gradient[2];

  // partial derivative with respect to phase
  gradient[0] = Stokes_I * delexp_delvarphi;
  // partial derivative with respect to gain
  gradient[1] = Stokes_I;
  
  // partial derivative with respect to phase
  delgradient_delS[0] = delexp_delvarphi;
  // partial derivative with respect to gain
  delgradient_delS[1] = 1.0;
  
  for (unsigned ir=0; ir < 2; ir++)
  {
    for (unsigned is=0; is < 2; is ++)
    {
      delalpha_delSre[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
      
      delalpha_delSim[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
    }
  }

  delalpha_delSre /= variance.real();
  delalpha_delSim /= variance.imag();

  delC_delSre = -covariance*delalpha_delSre*covariance;
  delC_delSim = -covariance*delalpha_delSim*covariance;

}

Estimate<double> Pulsar::ScalarProfileFitAnalysis::get_error () const
{
  Matrix<2,2,double> curvature;
  const_cast<ScalarProfileFitAnalysis*>(this)->get_curvature (curvature);

  if (Pulsar::PolnProfileFit::Analysis::verbose)
    cerr << "Pulsar::ScalarProfileFitAnalysis::get_error curvature="
	 << curvature << endl;

  // the variance of the phase shift variance
  double var_c_varphi = 0.0;

  if (Pulsar::PolnProfileFit::Analysis::verbose)
    cerr << "Pulsar::ScalarProfileFitAnalysis::get_error variance="
	 << variance << endl;

  for (unsigned ih=0; ih < max_harmonic; ih++)
  {
    Matrix<2,2,double> delC_delSre;
    Matrix<2,2,double> delC_delSim;

    delC_delS (delC_delSre, delC_delSim, ih);

    var_c_varphi += sqr(delC_delSre[0][0]) * variance.real();
    var_c_varphi += sqr(delC_delSim[0][0]) * variance.imag();
  }

  return sqrt( Estimate<double> (covariance[0][0], var_c_varphi) );
}
