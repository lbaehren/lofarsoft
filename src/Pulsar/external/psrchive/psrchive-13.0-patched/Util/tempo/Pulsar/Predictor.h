//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Predictor.h,v $
   $Revision: 1.8 $
   $Date: 2009/11/29 12:13:50 $
   $Author: straten $ */

#ifndef __PulsarPredictor_h
#define __PulsarPredictor_h

#include "Reference.h"
#include "Phase.h"
#include "MJD.h"
#include "Configuration.h"

#include <stdio.h>

namespace Pulsar {

  class Generator;

  //! Functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictors */
  class Predictor : public Reference::Able {

  public:

    //! Policy for creating new predictors
    enum Policy 
    {
      //! Clone the input predictor
      Input,
      //! Generate a new predictor using the default generator
      Default
    };

    //! Policy for creating new predictors
    static Configuration::Parameter<Policy>& get_policy ();

    //! Verbosity flag
    static bool verbose;

    //! Return a new, copy constructed instance of self
    virtual Predictor* clone () const = 0;

    //! Return a new Generator set up to produce a new Predictor like self
    virtual Generator* generator () const = 0;

    //! Add the information from the supplied predictor to self
    /*! This method should check to avoid inconsistency and duplication */
    virtual void insert (const Predictor*) = 0;

    //! Keep only the components required to span the given epochs
    virtual void keep (const std::vector<MJD>& epochs) = 0;

    //! Return true if the Predictor configuration matches this
    virtual bool matches (const Predictor*) const = 0;

    //! Set the observing frequency at which phase and time are related
    virtual void set_observing_frequency (long double MHz) = 0;

    //! Get the observing frequency at which phase and epoch are related
    virtual long double get_observing_frequency () const = 0;

    //! Return the phase, given the epoch
    virtual Phase phase (const MJD& t) const = 0;

    //! Return the epoch, given the phase and, optionally, a first guess
    virtual MJD iphase (const Phase& phase, const MJD* guess = 0) const = 0;

    //! Return the spin frequency, given the epoch
    virtual long double frequency (const MJD& t) const = 0;

    //! Return the phase correction for dispersion delay
    virtual Phase dispersion (const MJD &t, long double MHz) const = 0;

    //! Load from an open stream
    virtual void load (FILE*) = 0;

    //! Unload to an open stream
    virtual void unload (FILE*) const = 0;

    //! Factory helper creates a vector of pointers to derived class instances
    static void children (std::vector< Reference::To<Predictor> >&);

  };

}

std::ostream& operator<< (std::ostream& ostr, Pulsar::Predictor::Policy p);

std::istream& operator>> (std::istream& istr, Pulsar::Predictor::Policy& p);

#endif
