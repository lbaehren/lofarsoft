//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/IntegrationWeight.h,v $
   $Revision: 1.3 $
   $Date: 2008/08/05 13:30:58 $
   $Author: straten $ */

#ifndef _Pulsar_IntegrationWeight_H
#define _Pulsar_IntegrationWeight_H

#include "Pulsar/Weight.h"

namespace Pulsar {
  
  //! Algorithms that set the weights of sub-integrations
  class IntegrationWeight : public Weight
  {

  public:

    //! Default constructor
    IntegrationWeight ();

    //! Set integration weights
    void weight (Integration*);

    //! Set weight equal to value or scale weight by value
    /*! 
      When true: the weight is set equal to the duration (weight = duration)
      When false: the weight is scaled by the duration (weight *= duration)
    */
    void set_weight_absolute (bool f) { weight_absolute = f; }
    bool get_weight_absolute () const { return weight_absolute; }

    virtual double get_weight (const Integration*, unsigned ichan) = 0;

  protected:

    bool weight_absolute;

  };
  
}

#endif

