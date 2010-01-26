//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Probability.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/26 11:43:20 $
   $Author: straten $ */

#ifndef __Probability_h
#define __Probability_h

//! Base class of probability density functions
class Probability {

 public:

  virtual ~Probability () {}

  //! Get the value of the probability density at x
  virtual double density (double x) = 0;

  //! Get the value of the distribution function at x
  /*! The distribution, \f$ D(x) = \int_{-\inf}^x P(t) dt \f$ */
  virtual double cumulative_distribution (double x) = 0;

  //! Get the first moment of the distribution averaged from -inf to x
  virtual double cumulative_mean (double x) = 0;

  //! Get the second moment of the distribution averaged from -inf to x
  virtual double cumulative_moment2 (double x) = 0;

  //! Get the first moment of the distribution averaged from x1 to x2
  virtual double mean (double x1, double x2);

  //! Get the second moment of the distribution averaged from x1 to x2
  virtual double moment2 (double x1, double x2);


};

#endif

