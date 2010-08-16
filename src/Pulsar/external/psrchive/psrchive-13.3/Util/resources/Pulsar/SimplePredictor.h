//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/resources/Pulsar/SimplePredictor.h,v $
   $Revision: 1.3 $
   $Date: 2008/09/15 06:26:52 $
   $Author: straten $ */

#ifndef __SimplePredictor_h
#define __SimplePredictor_h

#include "Pulsar/Predictor.h"
#include "sky_coord.h"

#include <string>
#include <vector>

namespace Pulsar {

  //! Simple pulse phase model
  class SimplePredictor : public Predictor {

  public:

    //! The precision of the prediction in seconds
    static double precision;

    //! Default constructor
    SimplePredictor (const char* filename = 0);

    //! Copy constructor
    SimplePredictor (const SimplePredictor&);

    //! Destructor
    ~SimplePredictor ();

    //! Return a new, copy constructed instance of self
    Predictor* clone () const;

    //! Set up Generator to produce a new Predictor like self
    Generator* generator () const;

    //! Add the information from the supplied predictor to self
    void insert (const Pulsar::Predictor*);

    //! Keep only the components required to span the given epochs
    void keep (const std::vector<MJD>& epochs);

    //! Return true if reference attributes are equal
    bool matches (const Pulsar::Predictor*) const;

    //! Set the observing frequency at which predictions will be made
    void set_observing_frequency (long double MHz);

    //! Get the observing frequency at which phase and epoch are related
    long double get_observing_frequency () const;

    //! Return the phase, given the epoch
    Phase phase (const MJD& t) const;

    //! Return the epoch, given the phase
    MJD iphase (const Phase& phase, const MJD* guess) const;

    //! Return the spin frequency, given the epoch
    long double frequency (const MJD& t) const;

    //! Return the phase correction for dispersion delay
    Phase dispersion (const MJD &t, long double MHz) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Source name
    std::string get_name () const { return name; }

    //! Coordinates
    sky_coord get_coordinates () const { return coordinates; }

    //! Dispersion measure
    double get_dispersion_measure () const { return dispersion_measure; }

  private:

    //! Polynomial coefficients
    std::vector<long double> coefs;

    //! Reference epoch
    MJD reference_epoch;

    //! Source name
    std::string name;

    //! Coordinates
    sky_coord coordinates;

    //! Dispersion measure
    double dispersion_measure;

  };

}

#endif
