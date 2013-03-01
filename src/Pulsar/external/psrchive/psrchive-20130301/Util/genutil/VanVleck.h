//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __VanVleck
#define __VanVleck

#include <vector>
#include "NormalDistribution.h"

//! Correct low-bit sampled power measurements and correlations
/*! This class contains routines to rescale power and correlation 
 * measurements made from N-level sampled data.  The number of levels,
 * and their output values are arbitrary (but should be constant).
 * The only constraint is that both thresholds and output levels must
 * be symmetric about 0.  Quantized (pre-correction) data should be scaled
 * so that it has power levels between \f$L_0^2\f$ and \f$L_{max}^2\f$, where
 * \f$L_0\f$ (\f$L_{max}\f$) is the smallest (largest) level in absolute
 * value.  Input power values are returned in the same units as the 
 * threshold values.
*/
class VanVleck {

 public:

  //! Default constructor
  VanVleck();

  //! Set number of levels
  void set_nlevel(int n);

  //! Get number of levels;
  int get_nlevel() const { return nlevel; }

  //! Set uniform sampling thresholds
  void set_uniform_threshold();

  //! Set arbitrary sampling thresholds
  void set_threshold(float *thr);

  //! Set uniform output levels
  void set_uniform_levels();

  //! Set "canonical" output levels
  void set_canonical_levels();

  //! Set arbitrary output levels
  void set_levels(float *lev);

  //! Return output (quantized) power given input power, dc
  double out_pow(double in_pow, double in_dc=0.0);

  //! Return derivative d(out_pow)/d(in_pow)
  double d_out_pow(double in_pow, double in_dc=0.0);

  //! Return input (analog) power given output (quantized) power
  double in_pow(double out_pow);

  //! Set input powers for correlation correction
  /*! If only one power is needed (ie, only correcting ACFs), set
   * py<0.
   */
  void set_in_power(double px, double py);

  //! Correct a vector of auto-correlations using current pow_x
  void correct_acf(std::vector<float> data);

  //! Correct a vector of cross-correlations using current input powers
  void correct_ccf(std::vector<float> data);
  
 protected:

  //! Number of levels
  int nlevel;

  //! Input powers to use in correcting corrrelations
  double pow_x; 
  double pow_y;

  //! Vector of sampling thresholds
  std::vector<float> thresh;

  //! Vector of output levels
  std::vector<float> levels;

  //! Compute coefficients for correlation correction using pow_x, pow_y
  void compute_coeffs();

  //! Storage for coeffs we may want to reuse
  double c1x, c1y, c3x, c3y;

};

#endif
