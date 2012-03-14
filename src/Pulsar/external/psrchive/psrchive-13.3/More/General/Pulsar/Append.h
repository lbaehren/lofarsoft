//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Append.h,v $
   $Revision: 1.8 $
   $Date: 2009/07/27 03:56:13 $
   $Author: straten $ */

#ifndef _Pulsar_Append_H
#define _Pulsar_Append_H

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Algorithms that combine Integration data
  class Append : public Reference::Able {

  public:

    //! Default constructor
    Append ();

    //! Initialize an archive for appending
    virtual void init (Archive* into) { }

    //! Copy the data in 'from' to 'into'
    void append (Archive* into, const Archive* from);

    bool must_match;

  protected:

    //! Return true if there is a benign reason to stop
    virtual bool stop (Archive* into, const Archive* from);

    //! Throw an exception if there is a fatal reason to stop
    virtual void check (Archive* into, const Archive* from);

    //! Return the policy used to verify that data are mixable
    virtual const Archive::Match* get_mixable_policy (const Archive* into);

    //! Add the data in 'from' to 'into'
    /*! 

    The combine method should not make copies of the data in 'from';
    rather, 'into' should be made to point to the data in 'from'. 

    By sharing the data in 'from', it can be corrected through the
    'from' interface without concern for how the data is incorporated
    in 'into'.

    */
    virtual void combine (Archive* into, Archive* from) = 0;

    /* Internal variables that may be set by combine method */
    bool aligned;
    bool equal_ephemerides;
    bool equal_models;
    bool insert_model;
  };
  
}

#endif

