//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileFitAnalysis.h,v $
   $Revision: 1.29 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileFitAnalysis_h
#define __Pulsar_PolnProfileFitAnalysis_h

#include "Pulsar/PolnProfileFit.h"
#include "MEAL/StokesError.h"
#include "MEAL/StokesCovariance.h"

#include "MEAL/MuellerTransformation.h"
#include "MEAL/Complex2Value.h"
#include "MEAL/Real4Value.h"

#include <vector>

namespace Pulsar {

  //! Analysis of the matrix template matching algorithm
  class PolnProfileFitAnalysis {

  public:

    //! Verbosity flag
    bool verbose;

    //! Default constructor
    PolnProfileFitAnalysis ();

    //! When set, estimate the uncertainty in each attribute
    void set_compute_error (bool flag = true);

    //! Set the PolnProfileFit algorithm to be analysed
    void set_fit (PolnProfileFit*);

    //! Set the maximum boost parameter allowed in the optimal basis
    void set_optimal_boost_max (double max);

    //! Set the maximum harmonic used when optimizing
    void set_optimal_harmonic_max (unsigned max);

    //! Optimize the template for timing
    void optimize ();

    //! Set the transformation to be used to find the optimal basis
    void set_basis (MEAL::Complex2*);

    //! Get the transformation into the optimal basis
    MEAL::Complex2* get_Jbasis ();

    //! Return true when using Jones basis
    bool has_Jbasis() const { return Jbasis; }

    //! Set the transformation to be used to find the optimal basis
    void set_basis (MEAL::Real4*);

    //! Get the transformation into the optimal basis
    MEAL::Real4* get_Mbasis ();

    //! Return true when using Mueller basis
    bool has_Mbasis() const { return Mbasis; }

    //! Use or don't use the optimal transformation
    void use_basis (bool);

    //! Return the number of free parameters in the basis
    unsigned get_basis_nparam () const;

    //! Get the relative arrival time error
    Estimate<double> get_relative_error () const;

    //! Get the multiple correlation between phase shift and Jones parameters
    Estimate<double> get_multiple_correlation () const;

    //! Get the relative conditional arrival time error
    Estimate<double> get_relative_conditional_error () const;

    //! Get the variance of varphi and its gradient with respect to basis
    double get_c_varphi (std::vector<double>* c_varphi_grad = 0);

    //! Get the variance of varphi and its gradient with respect to basis
    double get_C_varphi (std::vector<double>* c_varphi_grad = 0);

    //! Get the expected relative conditional error given a histogram
    double get_expected_relative_error (std::vector<unsigned>& histogram);

    //! Get the uncertainty in the variance of varphi
    double get_c_varphi_error () const;

    //! Get the multiple correlation and its gradient with respect to basis
    double get_Rmult (std::vector<double>& grad);

    //! Get the conditional variance of varphi and its gradient wrt transform
    double get_cond_var (std::vector<double>& grad);

    //! Get the correlation coefficients
    Matrix<8,8,double> get_correlation () const;

    //! Write the terms in C_varhpi out to file
    void output_C_varphi (const char* filename);

  protected:

    //! The relative arrival time error
    Estimate<double> relative_error;

    //! The relative conditional arrival time error
    Estimate<double> relative_conditional_error;

    //! The multiple correlation between phase shift and Jones parameters
    Estimate<double> multiple_correlation;

    //! Flag set true when all three of the above values have been computed
    bool built;

    //! Computes the above three numbers
    void build (bool only_relative_error = false);

    //! The PolnProfileFit algorithm to be analysed
    Reference::To<PolnProfileFit> fit;

    //! The transformation built into the MTM algorithm
    Reference::To<MEAL::Complex2> xform;

    //! The maximum allowed boost in the optimal basis
    double max_boost;

    //! The maximum harmonic used when computing the optimal basis
    unsigned max_harmonic;

    //! The transformation to be used to find the optimal basis
    Reference::To<MEAL::Complex2> Jbasis;

    //! The means of inserting the basis transformation into the MTM model
    Reference::To<MEAL::Complex2Value> Jbasis_insertion;

    //! Insert the basis transformation into the MTM model
    void insert_basis ();

    //! The partial derivative of K with respect to free parameter, eta
    Jones<double> del_deleta (unsigned i, const Jones<double>& K) const;

    //! The partial derivative of the multiple correlation squared wrt S_k
    double delR2_varphiJ_delS (Matrix<8,8,double>& delC_delS);

    //! The partial derivative of the covariance matrix wrt Re[S_k] and Im[S_k]
    void delC_delS( Matrix<8,8,double>& delC_delSre,
		    Matrix<8,8,double>& delC_delSim, unsigned k ) const;

    //! The partial derivative of rho wrt Stokes parameter
    Jones<double> delrho_delS (unsigned k) const;

    //! The partial derivative of rho wrt basis parameter
    Jones<double> delrho_delB (unsigned b) const;

    //! The partial derivative of covariance wrt basis parameter
    Matrix<4,4,double> get_delXi_delB (unsigned b);

    //! The partial derivative of the curvature matrix wrt basis parameter
    Matrix<8,8,double> delalpha_delB (unsigned ib);

    //! The partial derivative of the curvature matrix wrt basis parameters
    void get_delalpha_delB (std::vector< Matrix<8,8,double> >&);
    void add_delalpha_delB (std::vector< Matrix<8,8,double> >&);

    //! Get the curvature matrix
    void get_curvature (Matrix<8,8,double>& curvature);
    void add_curvature (Matrix<8,8,double>& curvature);

    //! Set the model up to evaluate the specified harmonic
    void set_harmonic (unsigned index);

    //! Evaluate the model and phase (and gradients) of the specified harmonic
    void evaluate (unsigned index);

    Jones<double> model_result;
    std::vector< Jones<double> > model_gradient;

    Jones<double> xform_result;
    std::vector< Jones<double> > xform_gradient;

    Jones<double> phase_result;
    std::vector< Jones<double> > phase_gradient;

    Jones<double> Jbasis_result;
    std::vector< Jones<double> > Jbasis_gradient;

    //! Propagation of uncertainty through the congruence tranformation
    MEAL::StokesError error;

    //! Propagation of Stokes parameter covariances through congruence xform
    MEAL::StokesCovariance stokes_covariance;

    //! the covariance matrix
    Matrix<8,8,double> covariance;

    //! the covariances between the Jones matrix parameters
    Matrix <7,7,double> C_JJ;

    //! the inverse of the covariances between the Jones matrix parameters
    Matrix <7,7,double> inv_C_JJ;

    //! the covariances between the phase shift and each Jones matrix parameter
    Vector <7,double> C_varphiJ;
    
    //! the variance of the unconditional phase shift
    double c_varphi;

    //! the multiple correlation squared
    double R2_varphiJ;

    double efac;

    //! Compute the uncertainty of results
    bool compute_error;

    //! The transformation to be used to find the optimal basis
    Reference::To<MEAL::Real4> Mbasis;

    //! The means of inserting the basis transformation into the MTM model
    Reference::To<MEAL::Real4Value> Mbasis_insertion;

    Matrix<4,4,double> Mbasis_result;
    std::vector< Matrix<4,4,double> > Mbasis_gradient;

    //! The original inputs
    std::vector< Reference::To<MEAL::Complex2> > inputs;
    
  private:

    void initialize();

    Matrix<4,4,double> Xi;
    std::vector< Matrix<4,4,double> > delXi_delB;

    std::vector< double > weights;
    std::vector< double > store_covariance;

    void compute_weights (unsigned nharmonic);

  };


  //! Analysis of the scalar template matching algorithm
  class ScalarProfileFitAnalysis {

  public:

    //! Set the PolnProfileFit algorithm to be analysed
    void set_fit (const PolnProfileFit*);

    //! Set the fluctuation spectrum of the Profile to be analyzed
    void set_spectrum (const Profile*);

    //! Set the maximum harmonic to be used from the fluctuation spectrum
    void set_max_harmonic (unsigned max_harmonic);

    //! Set the variance of the fluctuation spectrum
    void set_variance (double v);

    //! Get the variance of the fluctuation spectrum
    double get_variance () const { return variance; }

    //! Get the estimated phase shift error for the fluctuation spectrum
    Estimate<double> get_error () const;

    //! Get the curvature matrix
    void get_curvature (Matrix<2,2,double>& curvature);

    //! The partial derivative of the covariance matrix wrt Re[S_0] and Im[S_0]
    void delC_delS ( Matrix<2,2,double>& delC_delSre,
		     Matrix<2,2,double>& delC_delSim,
		     unsigned index ) const;

  protected:

    //! The PolnProfileFit algorithm to be analysed
    Reference::To<const Profile> spectrum;

    //! The amps in the fluctation spectrum
    const std::complex<float>* amps;

    //! The maximum harmonic
    unsigned max_harmonic;

    Matrix<2,2,double> covariance;
    double variance;
  };

}

#endif
