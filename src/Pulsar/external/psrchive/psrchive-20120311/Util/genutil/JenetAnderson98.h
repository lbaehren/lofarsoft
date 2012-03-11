//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98.h,v $
   $Revision: 1.12 $
   $Date: 2008/05/11 08:15:08 $
   $Author: straten $ */

#ifndef __Jenet_Anderson_98
#define __Jenet_Anderson_98

#include "Reference.h"
#include <vector>

//! Statistics of two-bit quantization and artifact correction parameters
/*!  Given either the fraction of digitized samples in the low voltage
  state, \f$\Phi\f$, or the instantaneous power normalized by the mean
  power, \f$\sigma_n=\sigma/\langle\sigma\rangle\f$, this class
  computes
  <ol>
  <li> the output levels that minimize signal distortion 
       (dynamic level setting), and
  <li> the fractional quantization noise (scattered power correction).

  \param Phi fraction of low voltage state samples, or
  \param sigma_n normalized power

  \retval lo the optimal low voltage output level
  \retval hi the optimal hi voltage output level
  \retval A the fractional quantization noise

  Furthermore, given the sampling threshold, \f$t\f$, this class
  computes the mean, variance, and probability distribution of
  \f$\Phi\f$.
*/
class JenetAnderson98 : public Reference::Able {

 public:

  //! Table 3 - Optimum Input Threshold Spacing for a Uniform Digitizer
  static double get_optimal_spacing (unsigned bits);

  //! Default constructor
  JenetAnderson98 ();

  //! Set the fraction of samples in the low voltage state
  void set_Phi (double Phi);

  //! Set the normalized, undigitized power
  void set_sigma_n (double sigma_n);

  //! Get the optimal high voltage output levels, JA98 Eq.40
  double get_hi () const { return hi; }

  //! Get the optimal low voltage output levels, JA98 Eq.41
  double get_lo () const { return lo; }

  //! Get the slope of digitized vs undigitized correlation, JA98 Eq.43
  double get_A () const { return A; }

  //! Set the sampling threshold
  void set_threshold ( double t = get_optimal_spacing(2) );
  //! Get the sampling threshold
  double get_threshold () const { return threshold; }

  //! Get the expectation value of Phi, JA98 Eq.A2
  double get_mean_Phi () const { return mean_Phi; }
  //! Set the expectation value of Phi
  void set_mean_Phi (double mean_Phi);

  //! Get the variance of Phi
  double get_var_Phi () const { return var_Phi; }

  //! Get the theoretical probability distribution of Phi, JA98 Eq.A6
  void get_prob_Phi (unsigned L, std::vector<float>& prob_Phi);

  //! Given Phi, return the digitized power
  double A4 (double Phi);

  //! Given the digitized power, return Phi
  double invert_A4 (double sigma_hat);

  //! Given <Phi>, return the digitized power
  double A14 (double mean_Phi);

  //! Given the digitized power, return <Phi>
  double invert_A14 (double sigma_hat);

  //! Interface to a measured probability distribution of Phi
  class Probability;

  //! Plots measured and theoretical probability distributions
  class Plot;

  //! Efficiently computes Equation A5 and its derivative wrt <Phi>
  class EquationA5;

  //! Set the measured probability distribution of Phi
  void set_measured_prob_Phi (const Probability* data);

  //! Set the measured probability distribution of Phi
  const Probability* get_measured_prob_Phi () const;


 protected:

  //! Set the inverse width of the gaussian, sort of
  void set_alpha (double alpha, double Phi);

  //! The sampling threshold
  double threshold;

  //! The interface to a measured probability distribution of Phi
  Reference::To<const Probability> measured;

  double lo;
  double hi;
  double A;
  double mean_Phi;
  double var_Phi;

};

//! Interface to the measured distributions of Phi
class JenetAnderson98::Probability : public Reference::Able  {
  
public:
  
  //! Get the number of samples in each histogram
  virtual unsigned get_nsample() const = 0;
  
  //! Get the sampling threshold as a fraction of the noise power
  virtual float get_threshold () const = 0;
  
  //! Get the cut off power for impulsive interference excision
  virtual float get_cutoff_sigma () const = 0;
  
  //! Get the number of digitizers
  virtual unsigned get_ndig () const = 0;

  //! Get the specified histogram
  virtual void get_histogram (std::vector<float>& h, unsigned dig) const = 0;

  //! Evaluate the probability of Phi for the specified digitizer
  double evaluate (unsigned idig, double Phi, double* dprob_dPhi = 0);

private:
  unsigned last_idig;
  std::vector<float> last_hist;

};

#endif
