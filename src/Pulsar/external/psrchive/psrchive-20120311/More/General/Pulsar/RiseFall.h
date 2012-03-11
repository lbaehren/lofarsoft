//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/RiseFall.h,v $
   $Revision: 1.3 $
   $Date: 2007/11/02 04:25:39 $
   $Author: straten $ */

#ifndef __Pulsar_RiseFall_h
#define __Pulsar_RiseFall_h

#include "Pulsar/OnPulseEstimator.h"

namespace Pulsar {

  //! Estimates the on-pulse region between a rise and a fall
  class RiseFall : public OnPulseEstimator {

  public:

    //! Return the rise and fall as a pair
    virtual std::pair<int, int> get_rise_fall (const Profile*);

    //! Return the pulse phase bins in which the pulse rises and falls
    virtual void get_indeces (int& rise, int& fall) const = 0;

  protected:

    //! Set the weights between rise and fall
    void calculate (PhaseWeight* weight);

  };

}


#endif // !defined __Pulsar_RiseFall_h
