//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Parameters.h,v $
   $Revision: 1.7 $
   $Date: 2009/06/05 09:01:54 $
   $Author: straten $ */

#ifndef __PulsarParameters_h
#define __PulsarParameters_h

#include "Reference.h"

#include <stdio.h>

namespace Pulsar {

  //! Storage of pulsar parameters used to create a Predictor
  /*! This pure virtual base class defines the interface to pulsar
    parameters */
  class Parameters : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    class Lookup;

    //! Factory helper creates a vector of pointers to derived class instances
    static void children (std::vector< Reference::To<Parameters> >&);

    //! Return a new, copy constructed instance of self
    virtual Parameters* clone () const = 0;

    //! Return true if *this == *that
    virtual bool equals (const Parameters* that) const = 0;

    //! Load from an open stream
    virtual void load (FILE*) = 0;

    //! Unload to an open stream
    virtual void unload (FILE*) const = 0;

    //! Load from the named file
    void load (const std::string& filename);

    //! Unload to the named file
    void unload (const std::string& filename) const;

    //! Return the pulsar name
    virtual std::string get_name () const;

  };

}

#endif
