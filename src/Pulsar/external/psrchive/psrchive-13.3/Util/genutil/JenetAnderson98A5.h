//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98A5.h,v $
   $Revision: 1.3 $
   $Date: 2009/02/17 13:26:24 $
   $Author: straten $ */

#ifndef __Jenet_Anderson_98_A5
#define __Jenet_Anderson_98_A5

#include "JenetAnderson98.h"

/*! 
  Given the mean value of \f$\Phi\f$, this method computes the
  digitized power, as predicted by equation A5 of JA98.
*/
class JenetAnderson98::EquationA5 : public Reference::Able {

 public:

  //! Set the number of samples used to estimate the undigitized power
  void set_nsamp (unsigned nsamp);

  //! Set the discrete probability distribution; used instead of A6
  template<typename T>
  void set_A6 (const std::vector<T>& prob)
  {
    double sum = 0.0;
    for (unsigned i=0; i<prob.size(); i++)
      sum += prob[i];

    prob_Phi.resize( prob.size() );
    for (unsigned i=0; i<prob.size(); i++)
      prob_Phi[i] = prob[i] / sum;

    set_nsamp( prob.size() );
  }

  //! Return the digitized power, given <Phi>
  double evaluate (double mean_Phi);

  //! Return the mean value of Phi, given the digitized power
  double invert (double sigma_hat);

 private:

  std::vector<double> losq;
  std::vector<double> hisq;
  std::vector<double> fact;
  double dA5_dmean_Phi;

  std::vector<double> prob_Phi;

};

#endif
