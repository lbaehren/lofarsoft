//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Parameters.h,v $
   $Revision: 1.6 $
   $Date: 2009/03/15 06:55:53 $
   $Author: straten $ */

#ifndef __Tempo2Parameters_h
#define __Tempo2Parameters_h

#include "Pulsar/Parameters.h"

#include "tempo2.h"

namespace Tempo2 {

  class Generator;

  //! Storage of pulsar parameters used to create a Predictor
  /*! This pure virtual base class defines the interface to pulsar
    parameters */
  class Parameters : public Pulsar::Parameters {

  public:

    //! Default constructor
    Parameters ();

    //! Destructor
    ~Parameters ();

    //! Copy constructor
    Parameters (const Parameters&);

    //! Return a new, copy constructed instance of self
    Pulsar::Parameters* clone () const;

    //! Return true if *this == *that
    bool equals (const Pulsar::Parameters* that) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

  protected:

    friend class Generator;

    // the tempo2 pulsar structure defined in tempo2.h
    pulsar* psr;

  };

}

#endif
