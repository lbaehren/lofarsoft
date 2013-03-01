//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/TimeAppend.h,v $
   $Revision: 1.2 $
   $Date: 2007/07/14 22:05:12 $
   $Author: straten $ */

#ifndef _Pulsar_TimeAppend_H
#define _Pulsar_TimeAppend_H

#include "Pulsar/Append.h"

namespace Pulsar {

  //! Algorithms that combine Integration data
  class TimeAppend : public Append {

  public:

    //! Default constructor
    TimeAppend ();

    bool chronological;
    float max_overlap;

  protected:

    //! Return true if there is a benign reason to stop
    virtual bool stop (Archive* into, const Archive* from);

    //! Throw an exception if there is a fatal reason to stop
    virtual void check (Archive* into, const Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Archive* into, Archive* from);

  };
  
}

#endif

