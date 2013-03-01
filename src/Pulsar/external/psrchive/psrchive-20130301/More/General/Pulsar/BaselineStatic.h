//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineStatic.h,v $
   $Revision: 1.1 $
   $Date: 2011/03/03 20:47:26 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineStatic_h
#define __Pulsar_BaselineStatic_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  //! Finds a Profile baseline using an interative bounding method
  class BaselineStatic : public BaselineEstimator {

  public:

    //! Default constructor
    BaselineStatic ();

    //! Destructor
    ~BaselineStatic ();

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    BaselineStatic* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

  };

}

#endif
