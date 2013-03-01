//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Predictor.h,v $
   $Revision: 1.15 $
   $Date: 2008/09/15 06:26:52 $
   $Author: straten $ */

#ifndef __Tempo2Predictor_h
#define __Tempo2Predictor_h

#include "Pulsar/Predictor.h"

// From the TEMPO2 distribution
#include "tempo2pred.h"

namespace Tempo2 {

  class Generator;

  //! Functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictors */
  class Predictor : public Pulsar::Predictor {

  public:

    //! Default constructor
    Predictor ();

    //! Copy constructor
    Predictor (const Predictor&);

    //! Destructor
    ~Predictor ();

    //! Return a new, copy constructed instance of self
    Pulsar::Predictor* clone () const;

    //! Return a new Generator set up to produce a new Predictor like self
    Pulsar::Generator* generator () const;

    //! Get the number of time coefficients
    unsigned get_time_ncoeff () const;

    //! Get the number of frequency coefficients
    unsigned get_frequency_ncoeff () const;

    //! Get the length of each polynomial segment in days
    long double get_segment_length () const;

    //! Get the name of the pulsar
    std::string get_psrname () const;

    //! Get the name of the observatory
    std::string get_sitename () const;

    //! Get the start frequency in MHz
    long double get_freq_start () const;

    //! Get the end frequency in MHz
    long double get_freq_end () const;

    //! Get the disersion constant
    long double get_dispersion_constant () const;

    //! Add the information from the supplied predictor to self
    void insert (const Pulsar::Predictor*);

    //! Keep only the components required to span the given epochs
    void keep (const std::vector<MJD>& epochs);

    //! Return true if the Predictor configuration matches this
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

  private:

    friend class Generator;

    //! The class is implemented by the T2Predictor library
    T2Predictor predictor;

    //! The observing frequency to be passed to GetPhase and GetFrequency
    long double observing_frequency;

  };

}

//! convert an MJD to long double
long double from_MJD (const MJD& t);

//! convert a long double to an MJD
MJD to_MJD (long double t);

#endif
