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

#include "Pauli.h"
#include "Jacobi.h"

#include <fstream>
#include <assert.h>

// #define _DEBUG 1

using namespace std;

Pulsar::PolnProfileFitAnalysis::PolnProfileFitAnalysis ()
{
  built = false;
  compute_error = true;
  max_boost = 0.0;
  max_harmonic = 0;
  verbose = false;
}

//! When set, estimate the uncertainty in each attribute
void Pulsar::PolnProfileFitAnalysis::set_compute_error (bool flag)
{
  compute_error = flag;
}

void Pulsar::PolnProfileFitAnalysis::set_harmonic (unsigned index)
{
  double phase_shift = -2.0 * M_PI * double(index+1);

  fit->phase_axis.set_value (phase_shift);
  fit->model->set_input_index (index);
}

void Pulsar::PolnProfileFitAnalysis::evaluate (unsigned index)
{
  set_harmonic (index);

  static vector< Jones<double> > temp_gradient;
  model_result = fit->model->evaluate (&temp_gradient);
  phase_result = fit->phase_xform->evaluate (&phase_gradient);

  // assert( temp_gradient.size() == 10 );

  model_gradient.resize(8);

  unsigned J_start = 0;

  if (fit->model->get_param_name(0) == "phase") {
    model_gradient[0] = temp_gradient[2];
    J_start = 3;
  }
  else if (fit->model->get_param_name(7) == "phase") {
    model_gradient[0] = temp_gradient[9];
    J_start = 0;
  }
  else
    throw Error (InvalidState, "Pulsar::PolnProfileFitAnalysis::evaluate",
		 "model parameters in unknown order");

  for (unsigned j=0; j < 7; j++)
    model_gradient[j+1] = temp_gradient[j+J_start];
}


/*!
  Computes Equation 14 of van Straten (2006)
*/
void Pulsar::PolnProfileFitAnalysis::get_curvature (Matrix<8,8,double>& alpha)
{
#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFitAnalysis::get_curvature" << endl;
#endif

  alpha = Matrix<8,8,double>();

  initialize();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    evaluate (ih);
    add_curvature (alpha);

  }

  //cerr << "ALPHA=\n" << alpha << endl;
}

void Pulsar::PolnProfileFitAnalysis::add_curvature (Matrix<8,8,double>& alpha)
{
  for (unsigned ir=0; ir < 8; ir++) {

    Stokes< complex<double> > dr = complex_coherency(model_gradient[ir]);

    for (unsigned is=0; is <= ir; is ++) {
      
      Stokes< complex<double> > ds = complex_coherency(model_gradient[is]);

      alpha[ir][is] += (dr * Xi * conj(ds)).real();

      if (ir != is)
        alpha[is][ir] = alpha[ir][is];

    }
    
  }
  
}

void conformal_partition (const Matrix<8,8,double>& covariance,
			  double& c_varphi,
			  Vector <7,double>& C_varphiJ,
			  Matrix <7,7,double>& C_JJ)
{
  c_varphi = covariance[0][0];

#define OUTPUT_COVARIANCE 0

#if OUTPUT_COVARIANCE
  fprintf (stderr, "%12s%12.3g\n", "phase", covariance[0][0]);
#endif

  for (unsigned i=0; i < 7; i++) {

    C_varphiJ[i] = covariance[i+1][0];

#if OUTPUT_COVARIANCE
    unsigned m = i + 3;
    fprintf (stderr, "%12s", "unknown");
    fprintf (stderr, "%12.3g", C_varphiJ[i]);
#endif

    for (unsigned j=0; j < 7; j++) {

      C_JJ[i][j] = covariance[i+1][j+1];

      if (j > i)
	continue;

#if OUTPUT_COVARIANCE
      fprintf (stderr, "%12.3g", C_JJ[i][j]);
#endif

    }

#if OUTPUT_COVARIANCE
    fprintf (stderr, "\n");
#endif
  }

}



Jones<double> 
Pulsar::PolnProfileFitAnalysis::del_deleta (unsigned i,
					    const Jones<double>& K) const
{
  if (i == 0)
    return xform_result * K * herm(xform_result) * phase_gradient[2];

  i --;

  return ( xform_gradient[i] * K * herm(xform_result) +
	   xform_result * K * herm(xform_gradient[i]) ) * phase_result;
}

Jones<double> Pulsar::PolnProfileFitAnalysis::delrho_delS (unsigned k) const
{
  Stokes<double> q;
  q[k] = 1.0;
  return convert(q);
}

void Pulsar::PolnProfileFitAnalysis::delC_delS
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
Pulsar::PolnProfileFitAnalysis::delR2_varphiJ_delS 
(Matrix<8,8,double>& delC_delS) 
{
  // partial derivatives of conformal partition of covariance matrix
  Matrix <7,7,double> delC_JJ_delS;
  Vector <7,double> delC_varphiJ_delS;
  double delc_varphi_delS = 0;
      
  // partition the partial derivative of the covariance matrix
  conformal_partition (delC_delS, delc_varphi_delS,
		       delC_varphiJ_delS, delC_JJ_delS);

  return ( delC_varphiJ_delS * (inv_C_JJ * C_varphiJ)
	   - C_varphiJ * (inv_C_JJ * delC_JJ_delS * inv_C_JJ * C_varphiJ) 
	   + C_varphiJ * (inv_C_JJ * delC_varphiJ_delS)
	   - R2_varphiJ * delc_varphi_delS ) / c_varphi;
}


Jones<double>
Pulsar::PolnProfileFitAnalysis::delrho_delB (unsigned b) const
{
  if (Mbasis) {

    Jones<double> rho = inputs[fit->model->get_input_index()]->evaluate();
    return transform (Mbasis_gradient[b], rho);

  }
  else if (Jbasis) {

    Jones<double> rho = fit->model->get_input()->evaluate();
    return Jbasis_result * rho * herm(Jbasis_gradient[b])
      + Jbasis_gradient[b] * rho * herm(Jbasis_result);

  }
  else
    return 0;
}

//! Return weighted partial derivatives of variances wrt basis parameter
Matrix<4,4,double> Pulsar::PolnProfileFitAnalysis::get_delXi_delB (unsigned b)
{
#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFitAnalysis::get_delXi_delB " << b << endl;
#endif

  if (Mbasis)
    stokes_covariance.set_transformation_gradient (Mbasis_gradient[b]);
  else
    stokes_covariance.set_transformation_gradient (Jbasis_gradient[b]);

  Matrix<4,4,double> covar_grad = stokes_covariance.get_covariance_gradient ();

  return - Xi * covar_grad * Xi;
}

Matrix<8,8,double>
Pulsar::PolnProfileFitAnalysis::delalpha_delB (unsigned ib)
{
  Matrix<8,8,double> delalpha_delB;

  Jones<double> delR_delB = delrho_delB(ib);

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {

    Stokes<complex<double> > dr=complex_coherency(model_gradient[ir]);
    Stokes<complex<double> > d2r=complex_coherency(del_deleta(ir,delR_delB));

    // over all columns up to and including the diagonal
    for (unsigned is=0; is <= ir; is ++) {
	  
      Stokes<complex<double> > ds=complex_coherency(model_gradient[is]);
      Stokes<complex<double> > d2s=complex_coherency(del_deleta(is,delR_delB));

      delalpha_delB[ir][is] = (  d2r*Xi*conj(ds)
				 + dr*delXi_delB[ib]*conj(ds)
				 + dr*Xi*conj(d2s)  ).real();

      if (ir != is)
	delalpha_delB[is][ir] = delalpha_delB[ir][is];

    }
    
  }

  // cerr << "del[" << ib << "]=" << delalpha_delB << endl;

  return delalpha_delB;
}

void Pulsar::PolnProfileFitAnalysis::initialize ()
{
#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFitAnalysis::initialize" << endl;
#endif

  xform_result = xform->evaluate (&xform_gradient);

  if (Mbasis) {

    Mbasis_result = Mbasis->evaluate( &Mbasis_gradient );
    Mbasis_insertion->set_value( Mbasis_result );  
    stokes_covariance.set_transformation( Mbasis_result );

  }
  else if (Jbasis) {

#ifdef _DEBUG
    cerr << "Pulsar::PolnProfileFitAnalysis::initialize basis" << endl;
#endif

    Jbasis_result = Jbasis->evaluate( &Jbasis_gradient );
    Jbasis_insertion->set_value( Jbasis_result );  
    stokes_covariance.set_transformation( Jbasis_result );

  }
  else {

    Jbasis_result = 1;
    stokes_covariance.set_transformation (Jbasis_result);

  }

  Matrix<4,4,double> covar = stokes_covariance.get_covariance();

  Xi = inv(covar);

#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFitAnalysis::initialize\n"
    "  Xi=\n" << Xi << endl;
#endif

  delXi_delB.resize (get_basis_nparam());

  for (unsigned i=0; i<delXi_delB.size(); i++) {
    delXi_delB[i] = get_delXi_delB (i);
#ifdef _DEBUG
    cerr << "  delXi_delB[" << i << "]=\n" << delXi_delB[i] << endl;
#endif
  }

}

// compute the partial derivatives of the curvature matrix wrt basis parameters
void Pulsar::PolnProfileFitAnalysis::get_delalpha_delB
(vector< Matrix<8,8,double> >& delalpha_delbasis)
{
  unsigned nparam = get_basis_nparam ();

  delalpha_delbasis.resize (nparam);
  for (unsigned ib=0; ib < nparam; ib++)
    delalpha_delbasis[ib] = Matrix<8,8,double>();

  initialize ();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    unsigned bih = nharmonic - ih - 1;
    evaluate (bih);

    // over all basis parameters
    add_delalpha_delB (delalpha_delbasis);

  }


#if 0
  for (unsigned ib=0; ib < basis->get_nparam(); ib++)
    cerr << "del[" << ib << "]=" << endl << delalpha_delbasis[ib] << endl;
#endif

}

void Pulsar::PolnProfileFitAnalysis::add_delalpha_delB
(vector< Matrix<8,8,double> >& delalpha_delbasis)
{
  unsigned nparam = get_basis_nparam ();

  assert(delalpha_delbasis.size() == nparam);

  // over all basis parameters
  for (unsigned ib=0; ib < nparam; ib++)
    delalpha_delbasis[ib] += delalpha_delB (ib);
}

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFitAnalysis::set_fit (PolnProfileFit* f)
{
  fit = f;
  built = false;

  if (!fit)
    return;
    
  error.set_variance (fit->standard_variance);
  stokes_covariance.set_variance (fit->standard_variance);

  xform = fit->get_transformation();

  if (Jbasis)
    insert_basis ();
}

template<unsigned M, typename T>
Matrix<M,M,T> align (const Vector<M,T>& v)
{
  Matrix<M,M,T> result;
  matrix_identity (result);

  Vector<M,T> copy = v;

  for (unsigned m=1; m<M; m++) {

    unsigned j = M-m;
    unsigned i = j-1;

    T x = copy[i];
    T y = copy[j];
    
    T theta = atan2 (y, x);

    Matrix<M,M,T> temp;
    matrix_identity (temp);

    temp[i][i] = temp[j][j] = cos(theta);
    temp[j][i] = - sin(theta);
    temp[i][j] = - temp[j][i];

    cerr << "\nTEMP=\n" << temp << endl;

    copy = temp * copy;

    cerr << "\nCOPY=\n" << copy << endl;

    result = temp * result;

    cerr << "\nRESULT=\n" << result << endl;
  }

  return result;
}

//! Computes the above three numbers
void Pulsar::PolnProfileFitAnalysis::build (bool only_relative_error)
{
  Matrix<8,8,double> curvature;

#ifdef _DEBUG
  cerr << "Pulsar::PolnProfileFitAnalysis::set_fit get_curvature" << endl;
#endif

  // calculate the curvature matrix
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  //cerr << "curv=\n" << curvature << endl;
  //cerr << "cov=\n" << covariance << endl;

  if (!only_relative_error) {

    // partition the covariance matrix
    conformal_partition (covariance, c_varphi, C_varphiJ, C_JJ);

    Matrix<7,7,double> copy = C_JJ;
    Matrix<7,7,double> evec;
    Vector<7,double> eval;

    Jacobi (copy, evec, eval);

    cerr << "evec=\n" << evec << "\neval=\n" << eval << endl << endl;

    cerr << "C_varphiJ=\n" << C_varphiJ << endl << endl;

    cerr << "evec*C_varphiJ=\n" << evec * C_varphiJ << endl << endl;

    cerr << "evec*C_JJ*evec=\n" << evec * C_JJ * herm(evec) << endl << endl;

    Matrix<7,7,double> A = align(C_varphiJ);

    cerr << "A=\n" << A << endl << endl;

    cerr << "A*inv(A)=\n" << A*herm(A) << endl << endl;

    cerr << "A*C_varphiJ=\n" << A * C_varphiJ << endl << endl;

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

  if (max_harmonic && max_harmonic < fit->model->get_num_input())
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

  unsigned nharmonic = fit->model->get_num_input();
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
      double var_S = 0.5*fit->standard_variance[ip];

      var_c_varphi += ( delC_delSre[0][0]*delC_delSre[0][0] +
			delC_delSim[0][0]*delC_delSim[0][0] ) * var_S;

      double delR2_varphiJ_delSre = 0;
      double delR2_varphiJ_delSim = 0;

      if (!only_relative_error) {

	delR2_varphiJ_delSre = delR2_varphiJ_delS (delC_delSre);
	delR2_varphiJ_delSim = delR2_varphiJ_delS (delC_delSim);

	var_R2_varphiJ += delR2_varphiJ_delSre * delR2_varphiJ_delSre * var_S;
	var_R2_varphiJ += delR2_varphiJ_delSim * delR2_varphiJ_delSim * var_S;

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

      var_hatvar_varphi += delA_delSre * delA_delSre * var_S;
      var_hatvar_varphi += delA_delSim * delA_delSim * var_S;

      if (!only_relative_error) {

	// ... and the conditional variance
	double delB_delSre = 
	  delA_delSre * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSre;
	double delB_delSim = 
	  delA_delSim * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSim;
	
	var_hatvar_varphiJ += delB_delSre * delB_delSre * var_S;
	var_hatvar_varphiJ += delB_delSim * delB_delSim * var_S;

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

//! Get the multiple correlation and its gradient with respect to basis
double Pulsar::PolnProfileFitAnalysis::get_Rmult (std::vector<double>& grad)
{
  for (unsigned i=0; i<Jbasis->get_nparam(); i++)
    cerr << "basis[" << i << "]=" << Jbasis->get_param(i) << endl;

  Jbasis_insertion->set_value( Jbasis->evaluate() );

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  get_delalpha_delB (delalpha_delbasis);

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  // partition the covariance matrix
  conformal_partition (covariance, c_varphi, C_varphiJ, C_JJ);

  // calculate the inverse of the Jones parameter covariance matrix
  inv_C_JJ = inv(C_JJ);

  // the multiple correlation squared
  R2_varphiJ = C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi;

  grad.resize( Jbasis->get_nparam() );

  for (unsigned ib=0; ib < Jbasis->get_nparam(); ib++) {

    // calculate the partial derivative of the covariance matrix wrt basis
    Matrix<8,8,double> delC_delB;
    delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

    // partition the partial derivative of the covariance matrix
    double delc_varphi_delB;
    Matrix <7,7,double> delC_JJ_delB;
    Vector <7,double> delC_varphiJ_delB;

    conformal_partition (delC_delB, delc_varphi_delB,
			 delC_varphiJ_delB, delC_JJ_delB);

    grad[ib] = 
      ( delC_varphiJ_delB * (inv_C_JJ * C_varphiJ)
	- C_varphiJ * ((inv_C_JJ*delC_JJ_delB*inv_C_JJ) * C_varphiJ)
	+ C_varphiJ * (inv_C_JJ * delC_varphiJ_delB)
	- C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi * delc_varphi_delB )
      / c_varphi;

    cerr << "delRmult_delB[" << ib << "]= " << grad[ib] << endl;
      
  }

  cerr << "Rmult = " << R2_varphiJ << endl;

  return R2_varphiJ;
}


//! Get the variance of varphi and its gradient with respect to basis
double 
Pulsar::PolnProfileFitAnalysis::get_c_varphi (std::vector<double>* grad)
{
  if (Jbasis) {
    //if (verbose)
    cerr << "boost:";
    for (unsigned i=1; i<4; i++)
      cerr << " " << Jbasis->get_param(i);
    cerr << endl;
  }

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  try {

    // calculate the covariance matrix
    covariance = inv(curvature);

  }
  catch (Error& e) {
    cerr << "Pulsar::PolnProfileFitAnalysis::get_c_varphi error inv(curv)\n"
	 << e << "\ncurv=\n" << curvature << endl;
  }

  double c_varphi = covariance[0][0];

  if (!grad)
    return c_varphi;

  unsigned nparam = get_basis_nparam ();

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  get_delalpha_delB (delalpha_delbasis);

  grad->resize( nparam );

  assert( delalpha_delbasis.size() == nparam );

  double size = 0;

  for (unsigned ib=0; ib < nparam; ib++) {

    // calculate the partial derivative of the covariance matrix wrt basis
    Matrix<8,8,double> delC_delB;
    delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

    (*grad)[ib] = delC_delB[0][0];
    
    if (verbose)
      cerr << "delvar_delB[" << ib << "]= " << (*grad)[ib] << endl;

    if (ib)
      size += (*grad)[ib] * (*grad)[ib];
  }

  cerr << "c_varphi = " << c_varphi << " grad=" << sqrt(size) 
       << " grad[0]=" << (*grad)[0] << endl;

  return c_varphi;
}



void Pulsar::PolnProfileFitAnalysis::compute_weights (unsigned nharmonic) 
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

double Pulsar::PolnProfileFitAnalysis::get_expected_relative_error 
(std::vector<unsigned>& histogram)
{
  double M_tot = 0.0;
  double I_tot = 0.0;
  unsigned total = 0;

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

//! Get the variance of varphi and its gradient with respect to basis
double 
Pulsar::PolnProfileFitAnalysis::get_C_varphi (std::vector<double>* grad)
{
  unsigned nparam = get_basis_nparam ();

  if (Jbasis) {
    //if (verbose)
    cerr << "boost:";
    for (unsigned i=1; i<4; i++)
      cerr << " " << Jbasis->get_param(i);
    cerr << endl;
  }
  else if (Mbasis)
    cerr << "BASIS=\n" << Mbasis->evaluate() << endl;

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;

  // the partial derivatives of the curvature matrix wrt basis parameters
  vector< Matrix<8,8,double> > delalpha_delbasis (nparam);

  initialize();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  if (weights.size() != nharmonic)
    compute_weights (nharmonic);

  double C_varphi = 0;

  if (grad)  {
    *grad = vector<double> (nparam, 0.0);
    assert(grad->size() == nparam);
  }

  unsigned min_harmonic = 1;

  for (unsigned ih=0; ih < nharmonic; ih++) {

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

    if (!grad)
      continue;

    add_delalpha_delB (delalpha_delbasis);

    for (unsigned ib=0; ib < nparam; ib++) {

      // calculate the partial derivative of the covariance matrix wrt basis
      Matrix<8,8,double> delC_delB;
      delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

      (*grad)[ib] += weights[ih] * delC_delB[0][0];

    }

  }

  if (C_varphi == 0)
    throw Error (InvalidState, "get_C", "Insufficient signal-to-noise ratio");

  if (!grad)
    return C_varphi;

  double size=0;
  for (unsigned ib=0; ib < nparam; ib++)
    size += (*grad)[ib] * (*grad)[ib];

  cerr << "C_varphi = " << C_varphi << " grad=" << sqrt(size) 
       << " grad[0]=" << (*grad)[0] << endl;

  return C_varphi;
}

#if 0

void Pulsar::PolnProfileFitAnalysis::output_C_varphi (const char* filename)
{
  ofstream output (filename);
  if (!output)
    throw Error (FailedSys, "Pulsar::PolnProfileFitAnalysis::output_C_varphi",
		 "could not open " + string(filename));

  get_C_varphi ();

  unsigned nharmonic = weights.size();

  for (unsigned ih=0; ih < nharmonic; ih++) {
    cerr << "ih=" << ih << endl;
    set_optimal_harmonic_max (ih);
    build (false);
    output << ih
	   << "\t" << relative_error.get_value() 
	   << "\t" << relative_error.get_error() << endl;
  }
}

#endif

//! Get the correlation coefficients
Matrix<8,8,double> Pulsar::PolnProfileFitAnalysis::get_correlation () const
{
  Matrix<8,8,double> C;
  for (unsigned i=0; i<8; i++)
    for (unsigned j=0; j<=i; j++)
      C[i][j] = C[j][i] = covariance[i][j] /
	sqrt (covariance[i][i]*covariance[j][j]);

  return C;
}

Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_relative_error () const
{
  if (!built)
    const_cast<PolnProfileFitAnalysis*>(this)->build();
  return relative_error;
}


Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_multiple_correlation () const
{
  if (!built)
    const_cast<PolnProfileFitAnalysis*>(this)->build();
  return multiple_correlation;
}

Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_relative_conditional_error () const
{
  if (!built)
    const_cast<PolnProfileFitAnalysis*>(this)->build();
  return relative_conditional_error;
}

//! Set the transformation to be used to find the optimal basis
void Pulsar::PolnProfileFitAnalysis::set_basis (MEAL::Complex2* _basis)
{
  built = false;
  Jbasis = _basis;

  if (fit)
    insert_basis();
}

void Pulsar::PolnProfileFitAnalysis::insert_basis ()
{
  if (Jbasis_insertion)
    return;

  if (!fit || !Jbasis)
    return;

  Jbasis_insertion = new MEAL::Complex2Value;
  Jbasis_insertion -> set_value( Jbasis->evaluate() );

  MEAL::ProductRule<MEAL::Complex2>* p = new MEAL::ProductRule<MEAL::Complex2>;
  *p *= xform;
  *p *= Jbasis_insertion;

  fit->set_transformation ( p );
}

//! Get the transformation into the optimal basis
MEAL::Complex2* Pulsar::PolnProfileFitAnalysis::get_Jbasis ()
{
  return Jbasis;
}

//! Use or don't use the optimal transformation
void Pulsar::PolnProfileFitAnalysis::use_basis (bool use)
{
  if (Jbasis) {
    if (use)
      Jbasis_insertion -> set_value( Jbasis->evaluate() );
    else
      Jbasis_insertion -> set_value( Jones<double>::identity() );
  }
  else if (Mbasis) {
    if (use) {
      Mbasis_insertion -> set_value( Mbasis->evaluate() );
#if 1
      Calibration::TotalCovariance* covar = new Calibration::TotalCovariance;
      covar->set_optimizing_transformation( Mbasis->evaluate() );
      fit->set_uncertainty( covar );
#endif
    }
    else {
      Matrix<4,4,double> I;
      matrix_identity(I);
      Mbasis_insertion -> set_value( I );
      fit->set_uncertainty( new Calibration::TemplateUncertainty );
    }
  }
}


//! Set the transformation to be used to find the optimal basis
void Pulsar::PolnProfileFitAnalysis::set_basis (MEAL::Real4* M)
{
  built = false;
  Mbasis = M;

  if (inputs.size() != fit->model->get_num_input()) {

    Mbasis_insertion = new MEAL::Real4Value;

    inputs.resize( fit->model->get_num_input() );
    
    for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {
      fit->model->set_input_index (ih);
      inputs[ih] = fit->model->get_input();

      MEAL::MuellerTransformation* m_xform = new MEAL::MuellerTransformation;
      m_xform->set_transformation( Mbasis_insertion );
      m_xform->set_input( inputs[ih] );

      fit->model->set_input( m_xform );
    }
  }
}

//! Get the transformation into the optimal basis
MEAL::Real4* Pulsar::PolnProfileFitAnalysis::get_Mbasis ()
{
  return Mbasis;
}


unsigned Pulsar::PolnProfileFitAnalysis::get_basis_nparam () const
{
  if (Mbasis)
    return Mbasis->get_nparam();
  else if (Jbasis)
    return Jbasis->get_nparam();
  else
    return 0;
}


double Pulsar::PolnProfileFitAnalysis::get_cond_var (vector<double>& grad)
{
  if (verbose)
    for (unsigned ir=0; ir < xform_gradient.size(); ir++)
      cerr << "basis[" << ir << "]=" 
	   << xform->get_param(ir) << endl;

  xform_result = xform->evaluate (&xform_gradient);

  error.set_transformation (xform_result);
  Stokes<double> variance = error.get_variance();

  double var = 0.0;
  grad = vector<double> (xform_gradient.size(), 0.0);

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    unsigned bih = fit->model->get_num_input() - ih -1;

    evaluate (bih);

    Stokes< complex<double> > S = complex_coherency(model_result);

    double weight = bih + 1;

    double this_var = 0;
    for (unsigned ipol=0; ipol<4; ipol++)
      this_var += weight * norm(S[ipol]) / variance[ipol];

    var += this_var;

    // cerr << ih << " " << this_var << " " << var << endl;

    for (unsigned ir=0; ir < xform_gradient.size(); ir++) {

      error.set_transformation_gradient (xform_gradient[ir]);
      Stokes<double> variance_gradient = error.get_variance_gradient();

      Stokes< complex<double> > Sg = complex_coherency(model_gradient[ir+1]);

      // cerr << "Sg[" << ir << "]=" << Sg << endl;

      for (unsigned ipol=0; ipol<4; ipol++) {

	double n1 = 2 * (conj(S[ipol])*Sg[ipol]).real();
	double n2 = norm(S[ipol]) * variance_gradient[ipol] / variance[ipol];

#if 0
	cerr << ir << " " << ipol << ":\n"
	  "n1=" << n1 << "\n"
	  "n2=" << n2 << endl;
#endif

	grad[ir] += weight * (n1 - n2) / variance[ipol];

      }

    }

  }
  
  var = 1/var;

  static double scale = 0;

  if (scale == 0) {
    scale = 1.0 / pow (10.0, floor(log(var)/log(10.0)));
    cerr << "scale=" << scale << endl;
  }

  double size = 0;

  for (unsigned ir=0; ir < xform_gradient.size(); ir++) {
    grad[ir] = - scale * var * var * grad[ir];
    if (verbose)
      cerr << "grad[" << ir << "]=" << grad[ir] << endl;
    size += grad[ir] * grad[ir];
  }

  size = sqrt(size);

  var *= scale;

  // if (verbose)
    cerr << "cond_var=" << var << " grad=" << size 
	 << " grad[0]=" << grad[0] << endl;

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
  set_max_harmonic( fit->model->get_num_input() );

  variance = fit->standard_variance[0];
}

void Pulsar::ScalarProfileFitAnalysis::set_spectrum (const Profile* p)
{
  spectrum = p;
  amps = reinterpret_cast<const complex<float>*> (p->get_amps() + 2);
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

  for (unsigned ih=0; ih < max_harmonic; ih++) {

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

  curvature /= variance;

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
  
  for (unsigned ir=0; ir < 2; ir++) {
    
    for (unsigned is=0; is < 2; is ++) {
      
      delalpha_delSre[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
      
      complex<double> i (0,1);
      
      delalpha_delSim[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
      
    }

  }

  delalpha_delSre /= variance;
  delalpha_delSim /= variance;

  delC_delSre = -covariance*delalpha_delSre*covariance;
  delC_delSim = -covariance*delalpha_delSim*covariance;

}

Estimate<double> Pulsar::ScalarProfileFitAnalysis::get_error () const
{
  Matrix<2,2,double> curvature;
  const_cast<ScalarProfileFitAnalysis*>(this)->get_curvature (curvature);

  // the variance of the real and imaginary parts of the Stokes parameter
  double var_S = 0.5*variance;

  // the variance of the phase shift variance
  double var_c_varphi = 0.0;

  for (unsigned ih=0; ih < max_harmonic; ih++) {

    Matrix<2,2,double> delC_delSre;
    Matrix<2,2,double> delC_delSim;

    delC_delS (delC_delSre, delC_delSim, ih);

    var_c_varphi += delC_delSre[0][0]*delC_delSre[0][0] * var_S;
    var_c_varphi += delC_delSim[0][0]*delC_delSim[0][0] * var_S;

  }

  return sqrt( Estimate<double> (covariance[0][0], var_c_varphi) );
}
