//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/ExponentialDistribution.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/26 12:02:32 $
   $Author: straten $ */

#ifndef __ExponentialDistribution_h
#define __ExponentialDistribution_h

#include "Probability.h"

//! Base class of probability density functions
class ExponentialDistribution : public Probability {

 public:

  //! Get the value of the probability density at x
  double density (double x);

  //! Get the value of the distribution function at x
  double cumulative_distribution (double x);

  //! Get the first moment of the distribution averaged from 0 to x
  double cumulative_mean (double x);

  //! Get the second moment of the distribution averaged from 0 to x
  double cumulative_moment2 (double x);


};

#endif

