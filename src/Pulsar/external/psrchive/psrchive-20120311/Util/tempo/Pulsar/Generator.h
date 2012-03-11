//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Generator.h,v $
   $Revision: 1.5 $
   $Date: 2009/12/21 03:02:05 $
   $Author: straten $ */

#ifndef __PulsarGenerator_h
#define __PulsarGenerator_h

#include "Reference.h"
#include "Configuration.h"
#include "MJD.h"

namespace Pulsar {

  class Predictor;
  class Parameters;

  //! Generator of functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictor generators */
  class Generator : public Reference::Able {

  public:

    //! Return a new, copy constructed instance of self
    virtual Generator* clone () const = 0;

    //! Set the parameters used to generate the predictor
    virtual void set_parameters (const Parameters*) = 0;

    //! Set the range of epochs over which to generate
    virtual void set_time_span (const MJD& start, const MJD& finish) = 0;

    //! Set the range of frequencies over which to generate
    virtual void set_frequency_span (long double low, long double high) = 0;

    //! Set the site at which the signal is observed
    virtual void set_site (const std::string&) = 0;

    //! Return a new Predictor instance
    virtual Predictor* generate () const = 0;

    //! Return a pointer to a new Generator of the default derived type
    static Generator* get_default ();

    //! Return a pointer to a new Generator that can best handle the parameters
    static Generator* factory (const Parameters*);

    class Interpreter;

  protected:

    //! Default generator of new predictors
    static Configuration::Parameter<Generator*>& get_default_generator();

  };

}

#endif
