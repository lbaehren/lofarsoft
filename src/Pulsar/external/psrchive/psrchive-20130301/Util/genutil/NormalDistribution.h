//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/NormalDistribution.h,v $
   $Revision: 1.3 $
   $Date: 2008/01/18 21:43:46 $
   $Author: demorest $ */

#ifndef __NormalDistribution_h
#define __NormalDistribution_h

#include "Probability.h"

//! Base class of probability density functions
class NormalDistribution : public Probability {

 public:

  //! Get the value of the probability density at x
  double density (double x);

  //! Get the value of the distribution function at x
  double cumulative_distribution (double x);

  //! Get the first moment of the distribution averaged from 0 to x
  double cumulative_mean (double x);

  //! Get the second moment of the distribution averaged from 0 to x
  double cumulative_moment2 (double x);

  //! Get the raw first moment of the distribution averaged from 0 to x.
  double cumulative_mean_raw(double x);

  //! Get the raw third moment of the distribution averaged from 0 to x.
  double cumulative_moment3_raw(double x);
};

#endif

