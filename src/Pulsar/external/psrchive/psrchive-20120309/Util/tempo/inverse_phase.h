//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/inverse_phase.h,v $
   $Revision: 1.8 $
   $Date: 2007/11/20 03:59:17 $
   $Author: straten $ */

#ifndef __P_inverse_phase_h
#define __P_inverse_phase_h

#include "MJD.h"
#include "Phase.h"

#include <iostream>

namespace Pulsar {

  //! Number of times inverse_phase is called
  extern unsigned inverse_phase_calls;
  //! Total number of iterations
  extern unsigned inverse_phase_iterations;

  //! Compute the inverse phase function using the Newton-Raphson method

  /*! This template function uses the Newton-Raphson method to solve:

  PHASE = p (TIME)

  for TIME, given PHASE.

  This function requires class P to implement the following interface:

  class P {
  public:
    static double precision;

    Phase phase (const MJD&) const;
    long double frequency (const MJD&) const;
  };

  */
  template<typename P>
  MJD inverse_phase (const P& predictor, const Phase& p, MJD guess)
  {

    MJD dt;
    int gi = 0;

    double precision = std::max (P::precision, MJD::precision);
    if (Pulsar::Predictor::verbose)
      std::cerr << "inverse_phase: precision=" << precision*1e6 
	        << " us" << std::endl;

    inverse_phase_calls ++;

    for (gi=0; gi<10000; gi++) {

      inverse_phase_iterations ++;

      dt = (predictor.phase(guess) - p) / predictor.frequency(guess);

      if (!finite(dt.in_seconds()))
        throw Error (InvalidState, "inverse_phase",
                     "dt not finite; freq=%lf", predictor.frequency(guess));
 
      guess -= dt; // * converge_faster;

      if (Pulsar::Predictor::verbose)
        std::cerr << "inverse_phase: guess=" << guess.printdays(20)
		  << " dt=" << dt.in_seconds()*1e6 << " us" << std::endl;

      if (fabs (dt.in_seconds()) < precision)
	return guess;
    }
    
    std::cerr << "inverse_phase maximum iterations exceeded - error="
	      << dt.in_seconds() * 1e6 << "us" << std::endl;
    
    return guess;
  }

  //! Computes the inverse phase function using the Newton-Raphson method

  /*! This template function uses the Newton-Raphson method to solve:

  PHASE = p (TIME)

  for TIME, given PHASE.

  This function requires P to implement the following interface:

  class P {
  public:
    static double precision;

    MJD get_reftime() const;
    Phase get_refphase() const;
    long double get_reffrequency() const;
    Phase phase (const MJD&) const;
    long double frequency (const MJD&) const;
  };

  */

  template<typename P>
  MJD inverse_phase (const P& predictor, const Phase& p)
  {
    MJD guess = predictor.get_reftime()
      + (p - predictor.get_refphase()) / predictor.get_reffrequency();

    if (Pulsar::Predictor::verbose)
      std::cerr << "inverse_phase: reftime=" << predictor.get_reftime() 
		<< " refphase=" << predictor.get_refphase() 
		<< " reffreq=" << predictor.get_reffrequency() 
		<< " first guess = " << guess << std::endl;

    return inverse_phase (predictor, p, guess);
  }


 
};

#endif
