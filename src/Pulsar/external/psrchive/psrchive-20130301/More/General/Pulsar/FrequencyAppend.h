//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FrequencyAppend.h,v $
   $Revision: 1.8 $
   $Date: 2009/11/29 12:23:25 $
   $Author: straten $ */

#ifndef _Pulsar_FrequencyAppend_H
#define _Pulsar_FrequencyAppend_H

#include "Pulsar/Append.h"
#include "Pulsar/Config.h"
#include "Functor.h"

namespace Pulsar {

  class Integration;

  //! Algorithms that combine Integration data
  class FrequencyAppend : public Append
  {

  public:

    //! Initialize an archive for appending
    void init (Archive* into);

    typedef Functor< void (Integration*) > Weight;

    //! The policy used to weight each profile during combine
    static Option< Weight > weight_strategy;

  protected:

    //! Return true if there is a benign reason to stop
    virtual bool stop (Archive* into, const Archive* from);

    //! Throw an exception if there is a fatal reason to stop
    virtual void check (Archive* into, const Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Archive* into, Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Integration* into, Integration* from);

    /* internal */
    bool check_phase;

    //! Return the policy used to verify that data are mixable
    virtual const Archive::Match* get_mixable_policy (const Archive* into);

  };
  
}

#endif

