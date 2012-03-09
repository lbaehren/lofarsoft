//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/GaussianBaseline.h,v $
   $Revision: 1.10 $
   $Date: 2009/08/25 05:45:21 $
   $Author: straten $ */

#ifndef __Pulsar_GaussianBaseline_h
#define __Pulsar_GaussianBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Adaptively computes the baseline, assuming normally distributed noise
  /*!  

  The GaussianBaseline class uses an iterative method to better estimate
  of the off-pulse baseline.  The BaselineWindow algorithm
  systematically under-estimates the mean of the off-pulse baseline
  and, for pulsars with small duty cycles, does not make full use of
  all off-pulse samples (more samples reduce the uncertainty in the
  estimate of the off-pulse mean).

  GaussianBaseline addresses these issues by iteratively computing the
  mean and r.m.s. and excluding all points that lie greater than
  threshold (default threshold equals one sigma) away from the current
  mean.  The one-sigma cutoff neatly omits the on-pulse points, but also
  necessitates correction of the estimate of the variance, which is
  given by the integral

  \f$ \int_{-t}^{+t} x^2 P(x) \f$

  where \f$t\f$ is the threshold (with no cutoff, \f$t \rightarrow \infty\f$)

  Computation of the correction factor requires an assumption about
  the probability distribution, \f$P(x)\f$; GaussianBaseline assumes
  normally distributed measurement noise.

  After creating the baseline mask, GaussianBaseline performs two
  clean-up tasks:

  1) Any isolated masked samples are converted to not-masked (not
  necessarily on-pulse), and any isolated not-masked samples are
  converted to masked.  "Isolated" is defined as a single state
  surrounded by opposite states.  This step fills in the few random
  holes left by samples greater than threshold from the mean.

  2) A list is made of all of the transitions from masked to
  not-masked regions (and vice versa), a box-car smoothed version of
  the profile is made (default smoothing factor equals 4) and,
  beginning at each transition from not-masked to masked, masked points 
  are converted to not-masked until the smoothed profile falls below
  the off-pulse mean.  This step peels the baseline mask away from the
  on-pulse regions (where the pulse is weak, the iterative
  baseline can creep up the wings of the pulse).

  When any of the above steps fails, GaussianBaseline reverts to
  the dependable BaselineWindow method.

  */
  class GaussianBaseline : public IterativeBaseline {

  public:

    //! Default constructor
    GaussianBaseline ();

    //! Set the threshold below which samples are included in the baseline
    void set_threshold (float sigma);
    float get_threshold () const { return threshold; }

    //! Set the smoothing factor used during post processing
    void set_smoothing (unsigned);

    //! Get the variance correction factor
    float get_variance_correction () const { return moment_correction; }

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    GaussianBaseline* clone () const;

  protected:

    void get_bounds (PhaseWeight* weight, float& lower, float& upper);

    void postprocess (PhaseWeight* weight, const Profile* profile);

    //! Adjustment when computing variance of samples below threshold
    float moment_correction;

    //! Value of lower last returned by get_bounds
    float last_lower;

    //! Value of upper last returned by get_bounds
    float last_upper;

    //! Last mean computed during get_bounds
    float last_mean;

    //! Smoothing factor used in postprocessing
    unsigned smooth_bins;

  };

}

#endif
