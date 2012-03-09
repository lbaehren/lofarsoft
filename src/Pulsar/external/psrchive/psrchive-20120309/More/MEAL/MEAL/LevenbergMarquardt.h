//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/LevenbergMarquardt.h,v $
   $Revision: 1.17 $
   $Date: 2009/06/12 09:10:20 $
   $Author: straten $ */

#ifndef __Levenberg_Marquardt_h
#define __Levenberg_Marquardt_h

#include "MEAL/GaussJordan.h"
#include "MEAL/Axis.h"
#include "Estimate.h"
#include "Error.h"

#include <iostream>

#ifdef sun
#include <ieeefp.h>
#endif

namespace MEAL
{

  //! Levenberg-Marquardt algorithm for non-linear least squares minimization
  /*! This template class implements the nonlinear least squares
    fitting algorithm suggested by Levenberg, developed by Marquardt,
    and described in Numerical Recipes Chapter 15.5.  Use of the
    template methods of this class requires the definition of a model class
    that satisfies the following template:

    <pre>
    class Mt {

      //! Return the number of parameters in the model
      unsigned get_nparam () const;

      //! Return the value of the specified parameter
      double get_param (unsigned iparam) const;

      //! Set the value of the specified parameter
      void set_param (unsigned iparam, ) const;

      //! Return true if parameter at index is to be fitted
      bool get_infit (unsigned index) const;

      //! Return the value and gradient (wrt model parameters) of model at x
      Yt evaluate (std::vector<Gt>* gradient);

    };
    </pre>

    The type of the gradient, Gt, is explicity specified in the
    declaration of this template class.  The types of At and Yt are
    implicitly specified by the template instantiation of the methods
    of this class.  If Yt or Gt is not of type float or double, there
    must also be defined:

    <pre>
    //! a subtraction operator:
    const Yt operator - (const Yt &, const Yt &);

    //! a multiplication operator:
    const Gt operator * (const Yt &, const Gt &);

    <\pre>

    The LevenbergMarquardt class is used in three stages:
    <UL>
    <LI> call to ::init() with data and model
    <LI> repeated calls to ::iter() with data and model, comparing the chisq
    returned in order to determine convergence (or not) of fit
    <LI>call to ::result() to get curvature and covariance matrices
    </UL>
  */
  template <class Grad>
  class LevenbergMarquardt
  {
    
  public:
    static unsigned verbose;
    
    LevenbergMarquardt ()
    { 
      lamda_increase_factor = 10.0;
      lamda_decrease_factor = 0.1;
      singular_threshold = 1e-8;
    }
    
    //! returns initial chi-squared
    /*!
      At = abscissa type
      Et = estimate type
      Mt = model type
    */
    template <class At, class Et, class Mt>
    float init (const std::vector< At >& x,
		const std::vector< Et >& y,
		Mt& model);
    
    //! returns next chi-squared (better or worse)
    template <class At, class Et, class Mt>
    float iter (const std::vector< At >& x,
		const std::vector< Et >& y,
		Mt& model);

    //! returns the best-fit answers
    template <class Mt>
    void result (Mt& model,
		 std::vector<std::vector<double> >& covariance = null_arg,
		 std::vector<std::vector<double> >& curvature = null_arg);

    //! lamda determines the dominance of the steepest descent method
    float lamda;

    float lamda_increase_factor;
    float lamda_decrease_factor;

    //! Singular Matrix threshold
    /*! Passed to MEAL::GaussJordan, this attribute is used to
      decide when the curvature matrix is close to singular. */
    float singular_threshold;

  protected:

    //! Inverts H*d=b, where: H=modified Hessian, d=delta, b=gradient
    template <class Mt> void solve_delta (const Mt& model);
    
    //! returns chi-squared and calculates the Hessian matrix and gradient
    template <class At, class Et, class Mt>
    float calculate_chisq (const std::vector< At >& x,
			   const std::vector< Et >& y,
			   Mt& model);

  private:

    //! gradient of model: partial derivatives wrt its parameters
    std::vector<Grad> gradient;

    //! gradient of chi-squared wrt model parameters
    std::vector<double> beta;

    //! curvature matrix
    std::vector<std::vector<double> > alpha;

    //! next change to model
    std::vector<std::vector<double> > delta;

    //! chi-squared of best fit
    float best_chisq;                     

    //! curvature matrix (one half of Hessian matrix) of best fit
    std::vector<std::vector<double> > best_alpha;
    //! gradient of chi-squared of best fit
    std::vector<double> best_beta;

    //! The parameters of the current model
    std::vector<double> backup;

    static std::vector<std::vector<double> > null_arg;

  };

  template<class At>
  class AbscissaTraits
  {
  public:
    template<class Mt>
    static void apply (Mt& model, const At& abscissa)
    { abscissa.apply(); }
  };

  template<>
  class AbscissaTraits<double>
  {
  public:
    template<class Mt>
    static void apply (Mt& model, double abscissa)
    { model.set_abscissa(abscissa); }
  };


  //! Defines the weighted inner product and norm
  template<class Et>
  class WeightingScheme
  {
  public:

    typedef typename Et::val_type val_type;
    typedef typename Et::var_type var_type;
    
    //! Default constructor
    WeightingScheme (const Et& estimate)
    {
      set_variance (estimate.get_variance());
    }

    //! Set the variance
    void set_variance (const var_type& variance)
    {
      inverse_variance = 1.0 / variance;
    }

    //! Return the difference between data and model
    val_type difference (const Et& estimate, const val_type& model)
    {
      return estimate.get_value() - model;
    }

    //! Return the norm of the value
    val_type norm (const val_type& x) const
    {
      return x*x; 
    }

    val_type get_weighted_conjugate (const val_type& data) const
    {
      return data * inverse_variance;
    }

    float get_weighted_norm (const val_type& data) const
    { 
      return norm(data) * inverse_variance;
    }

    var_type inverse_variance;

  };



  //! Specialization for std::complex<Estimate<T>>
  template<class Et>
  class WeightingScheme< std::complex<Et> >
  {

  public:

    typedef std::complex<Et> type;
    typedef std::complex<typename Et::val_type> val_type;
    typedef typename Et::var_type var_type;
    
    //! Default constructor
    WeightingScheme (const type& estimate)
    {
      set_variance (estimate);
    }

    //! Set the variance
    void set_variance (const type& estimate)
    {
      inv_var_real = 1.0 / estimate.real().get_variance();
      inv_var_imag = 1.0 / estimate.imag().get_variance();
    }

    //! Return the difference between data and model
    val_type difference (const type& estimate, const val_type& model)
    {
      val_type val (estimate.real().get_value(), estimate.imag().get_value());
      return val - model;
    }

    //! Return the norm of the value
    val_type norm (const val_type& x) const
    {
      return std::norm(x); 
    }

    val_type get_weighted_conjugate (const val_type& data) const
    {
      return val_type (data.real()*inv_var_real, -data.imag()*inv_var_imag);
    }

    float get_weighted_norm (const val_type& data) const
    { 
      return data.real()*data.real()*inv_var_real 
	+ data.imag()*data.imag()*inv_var_imag;
    }

    var_type inv_var_real;
    var_type inv_var_imag;

  };



  //! Calculates alpha and beta
  template <class Mt, class At, class Et, class Grad>
  float lmcoff (// input
		Mt& model,
		const At& abscissa,
		const Et& data,
		// storage
		std::vector<Grad>& gradient,
		// output
		std::vector<std::vector<double> >& alpha,
		std::vector<double>& beta);
  
  //! Calculates alpha and beta
  /*! Wt must be a weighting scheme */
  template <class Mt, class Yt, class Wt, class Grad>
  float lmcoff1 (// input
		 Mt& model,
		 const Yt& delta_data,
		 const Wt& weighting_scheme,
		 const std::vector<Grad>& gradient,
		 // output
		 std::vector<std::vector<double> >& alpha,
		 std::vector<double>& beta);

  template<class Mt>
  std::string get_name (const Mt& model, unsigned iparam);

}

template <class Grad>
std::vector<std::vector<double> > MEAL::LevenbergMarquardt<Grad>::null_arg;

template <class Grad>
unsigned MEAL::LevenbergMarquardt<Grad>::verbose = 0;

template <class Grad>
template <class At, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::init
(const std::vector< At >& x,
 const std::vector< Et >& y,
 Mt& model)
{
  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::init" << std::endl;

  // size all of the working space arrays
  alpha.resize  (model.get_nparam());
  beta.resize   (model.get_nparam());
  delta.resize  (model.get_nparam());
  backup.resize (model.get_nparam());
  for (unsigned j=0; j<model.get_nparam(); j++) {
    alpha[j].resize (model.get_nparam());
    delta[j].resize (1);
  }

  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::init calculate chisq"
              << std::endl;

  best_chisq = calculate_chisq (x, y, model);
  best_alpha = alpha;
  best_beta = beta;
  lamda = 0.001;

  if (verbose > 0)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::init chisq=" 
	 << best_chisq << std::endl;

  return best_chisq;
}

template<class T>
void verify_orthogonal (const std::vector<std::vector<double > >& alpha,
			const T& model)
{
  unsigned nrow = alpha.size();

  if (!nrow)
    return;

  unsigned nparam = model.get_nparam ();

  unsigned nfree = 0;
  for (unsigned iparam=0; iparam < nparam; iparam++)
    if (model.get_infit(iparam))
      nfree ++;

  /*
    Convert row numbers to parameter names
  */
  std::vector<std::string> names (nfree);
  std::vector<unsigned> indeces (nfree);

  unsigned kparam = 0;
  for (unsigned krow=0; krow<nfree; krow++)
  {
    while (!model.get_infit(kparam))
      kparam ++;

    names[krow] = model.get_param_name(kparam);
    indeces[krow] = kparam;

    kparam ++;
  }

  std::vector<double> row_mod (nfree, 0.0);

  /*
    calculate the norm of each row vector
  */

  for (unsigned irow=0; irow<nfree; irow++)
  {
    for (unsigned jcol=0; jcol<nfree; jcol++) 
      row_mod[irow] += alpha[irow][jcol] * alpha[irow][jcol];
    row_mod[irow] = sqrt(row_mod[irow]);

    if (row_mod[irow] == 0)
      std::cerr << irow << "=" << names[irow] << " gradient = 0" << std::endl;
  }

  for (unsigned krow=0; krow<nfree; krow++)
  {
    if (row_mod[krow] == 0)
      continue;

    for (unsigned irow=krow+1; irow<nfree; irow++)
    {
      if (row_mod[irow] == 0)
	continue;

      double degen = 0.0;
      for (unsigned jcol=0; jcol<nfree; jcol++)
        degen += alpha[krow][jcol] * alpha[irow][jcol];
      degen /= row_mod[krow] * row_mod[irow];

      if (degen > 0.99999)
      {
        double ival = model.get_param(indeces[irow]);
	double kval = model.get_param(indeces[krow]);

	std::cerr << "degen(" << names[krow] << "," << names[irow] << ") = "
		  << degen << std::endl 
		  << "\t" << names[krow] << " = " << kval << std::endl
		  << "\t" << names[irow] << " = " << ival << std::endl;
      }

      if (!finite(degen))
      {
        std::cerr << "NaN or Inf in curvature matrix" << std::endl;
        return;
      }
    }
  }
}

template<typename Mt>
std::string MEAL::get_name (const Mt& model, unsigned iparam)
{
  unsigned ifree = 0;
  for (unsigned i=0; i < model.get_nparam(); i++)
    if (model.get_infit(i))
    {
      if (ifree == iparam)
	return model.get_param_name(i);
      ifree ++;
    }
  return "unknown";
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::solve_delta
// /////////////////////////////////////////////////////////////////////////

/*! Using the curvature matrix and gradient stored in the best_alpha
  and best_beta attributes and the current value of lamda, solve
  Equation 15.5.14 for the change in model parameters, delta.

  \retval delta attribute

  This method also uses the alpha attribute to temporarily hold alpha'
*/
template <class Grad>
template <class Mt>
void MEAL::LevenbergMarquardt<Grad>::solve_delta (const Mt& model)
{
  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta" << std::endl;

  if (alpha.size() != model.get_nparam())
    throw Error (InvalidState, 
		 "MEAL::LevenbergMarquardt<Grad>::solve_delta",
		  "alpha.size=%d != model.nparam=%d", 
		 alpha.size(), model.get_nparam());

#ifndef _DEBUG
  if (verbose > 2)
#endif
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta lamda="
	 << lamda << " nparam=" << model.get_nparam() << std::endl;

  unsigned iinfit = 0;
  for (unsigned ifit=0; ifit<model.get_nparam(); ifit++)
    if (model.get_infit(ifit))
    {
      unsigned jinfit = 0;
      for (unsigned jfit=0; jfit<model.get_nparam(); jfit++)
	if (model.get_infit(jfit)) {
	  alpha[iinfit][jinfit]=best_alpha[ifit][jfit];
	  jinfit ++;
	}

      alpha[iinfit][iinfit] *= (1.0 + lamda);
      delta[iinfit][0]=best_beta[ifit];
      iinfit ++;
    }


  if (iinfit == 0)
    throw Error (InvalidState,
		 "MEAL::LevenbergMarquardt<Grad>::solve_delta"
		  "no parameters in fit");

  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta for " << iinfit
	 << " parameters" << std::endl;

  //! curvature matrix
  std::vector<std::vector<double> > temp_copy (alpha);

  try
  {
    // invert Equation 15.5.14
    MEAL::GaussJordan (alpha, delta, iinfit, singular_threshold);
  }
  catch (Error& error)
  {
    if (verbose > 2)
      verify_orthogonal (temp_copy, model);
    throw error += "MEAL::LevenbergMarquardt<Grad>::solve_delta";
  }

  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta exit" 
	      << std::endl;
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::iter
// /////////////////////////////////////////////////////////////////////////

/*! This method calls solve_delta to determine the change in model
  parameters based on the current value of lamda, the curvature matrix
  and gradient, as stored in the best_alpha and best_beta attributes.

  The model is updataed using the delta attribute, and the new
  chi-squared, curvature and gradient are calculated.  If the new
  chi-squared is better than best_chisq, then the model is kept, the
  best_alpha, best_beta, and best_chisq attributes are updated, and
  lamda is decreased for the next trial.  If the new chi-squared is
  worse, the model is restored to its previous state, and lamda is
  increased for the next trial.

  \return chi-squared of model
  \retval model best model 

  This method also uses the beta attribute to unpack delta.
*/
template <class Grad>
template <class At, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::iter
( const std::vector< At >& x,
  const std::vector< Et >& y,
  Mt& model )
{
  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::iter" << std::endl;

  solve_delta (model);

  // After call to solve_delta, delta contains required change in model
  // parameters.  Update the model.

  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::iter update model"
	      << std::endl;

  unsigned iinfit = 0;
  for (unsigned ifit=0; ifit<model.get_nparam(); ifit++) {

    double change = 0.0;

    if (model.get_infit(ifit)) {
      change = delta[iinfit][0];
      iinfit ++;
    }

    backup[ifit] = model.get_param (ifit);

    if (verbose > 2)
      std::cerr << "   delta[" << ifit << "]=" << change << std::endl;

    model.set_param (ifit, backup[ifit] + change);
  }

  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::iter"
      " calculate new chisq" << std::endl;
  float new_chisq = calculate_chisq (x, y, model);

  if (new_chisq < best_chisq) {

    lamda *= lamda_decrease_factor;

    if (verbose)
      std::cerr << "MEAL::LevenbergMarquardt<Grad>::iter new chisq="
           << new_chisq << "\n  better fit; lamda=" << lamda << std::endl;

    best_chisq = new_chisq;
    best_alpha = alpha;
    best_beta  = beta;

  }
  else {

    lamda *= lamda_increase_factor;

    if (verbose)
      std::cerr << "MEAL::LevenbergMarquardt<Grad>::iter new chisq="
           << new_chisq << "\n  worse fit; lamda=" << lamda << std::endl;

    // restore the old model
    for (unsigned iparm=0; iparm<model.get_nparam(); iparm++)
      model.set_param (iparm, backup[iparm]);

  }

  return new_chisq;
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::result
// /////////////////////////////////////////////////////////////////////////

/*! After a call to init or iter, best_alpha contains the last
   curvature matrix computed.  A call is made to solve_delta with
   lamda=0; member "alpha" will then contain the covariance matrix.

   \retval covar the covariance matrix
   \retval curve the curvature matrix
*/
template <class Grad>
template <class Mt>
void MEAL::LevenbergMarquardt<Grad>::result
( Mt& model,
  std::vector<std::vector<double> >& covar,
  std::vector<std::vector<double> >& curve )
{
  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::result" << std::endl;

  if (&curve != &null_arg)
    curve = best_alpha;

  lamda = 0.0;
  solve_delta (model);

  if (&covar == &null_arg)
    return;

  covar.resize (model.get_nparam());

  unsigned iindim = 0;
  for (unsigned idim=0; idim < model.get_nparam(); idim++) {
    covar[idim].resize (model.get_nparam());
 
    if (!model.get_infit(idim))
      for (unsigned jdim=0; jdim < model.get_nparam(); jdim++)
	covar[idim][jdim] = 0;
    else {
      unsigned jindim = 0;
      for (unsigned jdim=0; jdim < model.get_nparam(); jdim++)
	if (model.get_infit(jdim)) {
	  covar[idim][jdim] = alpha [iindim][jindim];
	  jindim ++;
	}
	else
	  covar[idim][jdim] = 0;

      iindim ++;
    }
  }  
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::chisq
// /////////////////////////////////////////////////////////////////////////

/*! Given a set of abscissa, ordinate, ordinate error, and a model,
   compute the: - normalized squared difference "chi-squared" -
   chi-squared gradient "beta" - Hessian matrix "alpha"

   \return chi-squared

   \retval alpha attribute
   \retval beta attribute

   This method uses the gradient attribute to store the model gradient
*/ 
template <class Grad>
template <class At, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::calculate_chisq
(const std::vector< At >& x,
 const std::vector< Et >& y,
 Mt& model)
{
  if (verbose > 2)
    std::cerr << "MEAL::LevenbergMarquardt<Grad>::chisq" << std::endl;

  if (alpha.size() != model.get_nparam())
    throw Error (InvalidState, "MEAL::LevenbergMarquardt<Grad>::chisq",
		 "alpha.size=%d != model.nparam=%d",
		 alpha.size(), model.get_nparam());

  if (y.size() < x.size())
    throw Error (InvalidParam, "MEAL::LevenbergMarquardt<Grad>::chisq",
		 "y.size=%d < x.size=%d", y.size(), x.size());

  // initialize sums
  double Chisq = 0.0;
  for (unsigned j=0; j<alpha.size(); j++) {
    for (unsigned k=0; k<=j; k++)
      alpha[j][k] = 0.0;
    beta[j] = 0.0;
  }

  for (unsigned ipt=0; ipt < x.size(); ipt++) {

    if (verbose > 2)
      std::cerr << "MEAL::LevenbergMarquardt<Grad>::chisq lmcoff[" << ipt
	   << "/" << x.size() << "]" << std::endl;

    Chisq += lmcoff (model, x[ipt], y[ipt],
		     gradient, alpha, beta);
  }

  // populate the symmetric half of the curvature matrix
  for (unsigned ifit=1; ifit<model.get_nparam(); ifit++)
    for (unsigned jfit=0; jfit<ifit; jfit++)
      alpha[jfit][ifit]=alpha[ifit][jfit];

  return Chisq;
}

template <class Mt, class At, class Et, class Grad>
float MEAL::lmcoff (
		    // input
		    Mt& model,
		    const At& abscissa,
		    const Et& data,
		    // storage
		    std::vector<Grad>& gradient,
		    // output
		    std::vector<std::vector<double> >& alpha,
		    std::vector<double>& beta
		    )
{
  if (LevenbergMarquardt<Grad>::verbose > 2)
    std::cerr << "MEAL::lmcoff data=" << data << std::endl;

  AbscissaTraits<At>::apply (model, abscissa);

  WeightingScheme<Et> weight (data);

  return lmcoff1 (model, weight.difference (data, model.evaluate (&gradient)),
		  weight, gradient, alpha, beta);
}

template <class Mt, class Yt, class Wt, class Grad>
float MEAL::lmcoff1 (
		     // input
		     Mt& model,
		     const Yt& delta_y,
		     const Wt& weight,
		     const std::vector<Grad>& gradient,
		     // output
		     std::vector<std::vector<double> >& alpha,
		     std::vector<double>& beta
		     )
{
  //! The traits of the gradient element
  ElementTraits<Grad> traits;

  if (LevenbergMarquardt<Grad>::verbose > 2)
    std::cerr << "MEAL::lmcoff1 delta_y=" << delta_y << std::endl;

  Yt w_delta_y = weight.get_weighted_conjugate (delta_y);

  for (unsigned ifit=0; ifit < model.get_nparam(); ifit++)
  {
    if (model.get_infit(ifit))
    {
      // Equation 15.5.6 (with 15.5.8)
      beta[ifit] += traits.to_real (w_delta_y * gradient[ifit]);

      if (LevenbergMarquardt<Grad>::verbose > 2)
        std::cerr << "MEAL::lmcoff1 compute weighted conjugate of gradient"
                     "[" << ifit << "]" << std::endl;

      Grad w_gradient = weight.get_weighted_conjugate (gradient[ifit]);

      if (LevenbergMarquardt<Grad>::verbose > 2)
        std::cerr << "MEAL::lmcoff1 add to curvature matrix" << std::endl;

      // Equation 15.5.11
      for (unsigned jfit=0; jfit <= ifit; jfit++)
	if (model.get_infit(jfit))
	  alpha[ifit][jfit] += traits.to_real (w_gradient * gradient[jfit]);
    }
  }

  // Equation 15.5.5
  float chisq = weight.get_weighted_norm (delta_y);

  if (LevenbergMarquardt<Grad>::verbose > 2)
    std::cerr << "MEAL::lmcoff1 chisq=" << chisq << std::endl;

  return chisq;
}



#endif
