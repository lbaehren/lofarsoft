//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Predict.h,v $
   $Revision: 1.12 $
   $Date: 2009/11/29 12:13:50 $
   $Author: straten $ */

#ifndef __Tempo_Predict_h
#define __Tempo_Predict_h

#include "Pulsar/Generator.h"
#include "Configuration.h"
#include "polyco.h"

namespace Tempo {

  //! Uses tempo orediction mode to produce polynomial ephemerides (polyco)
  class Predict : public Pulsar::Generator {

  public:

    //! The minimum value of nspan allowed
    static Configuration::Parameter<unsigned>& get_minimum_nspan();

    //! The maximum rms allowed, in turns
    static Configuration::Parameter<double>& get_maximum_rms();

    //! Default constructor
    Predict (const Pulsar::Parameters* parameters = 0);

    //
    // Pulsar::Generator interface
    //

    //! Return a new, copy constructed instance of self
    Predict* clone () const;

    //! Set the parameters used to generate the predictor
    void set_parameters (const Pulsar::Parameters*);

    //! Set the range of epochs over which to generate
    void set_time_span (const MJD& start, const MJD& finish);

    //! Set the range of frequencies over which to generate
    void set_frequency_span (long double low, long double high);

    //! Set the site at which the signal is observed
    void set_site (const std::string&);

    //! Return a new Predictor instance
    Pulsar::Predictor* generate () const;

    //
    // Original interface
    //

    //! Set the observatory site code
    /*! Correponds to ASITE in tz.in */
    void set_asite (char code);

    //! Set the observing frequency
    /*! Corresponds to FREQ in tz.in */
    void set_frequency (double MHz);

    //! Set the maximum hour angle of observations
    /*! Corresponds to MAXHA in tz.in */
    void set_maxha (unsigned hours);

    //! Set the time spanned by each polynomial ephemeris
    /*! Corresponds to NSPAN in tz.in */
    void set_nspan (unsigned minutes);

    //! Set the number of coefficients per polynomial ephemeris
    /*! Corresponds to NCOEFF in tz.in */
    void set_ncoef (unsigned ncoef);

    //! Set flag to verify time spanned by polynomial ephemerides
    void set_verify (bool verify);

    //! Returns a polyco valid over the range in MJD specified by m1 and m2
    polyco get_polyco (const MJD& m1, const MJD& m2);

    //! Write the tz.in file according to the tempo definition
    void write_tzin () const;

  protected:

    //! The observatory site code
    char asite;

    //! The observing frequency in MHz
    double frequency;

    //! The maximum hour angle of observations in hours
    unsigned maxha;

    //! The time spanned by each polynomial ephemeris in minutes
    unsigned nspan;

    //! The number of coefficients per polynomial ephemeris
    unsigned ncoef;

    //! The pulsar parameters
    Reference::To<const Pulsar::Parameters> parameters;

    //! The pulsar name
    std::string psrname;

    //! Verify the time spanned by polynomial ephemerides
    bool verify;

    MJD m1, m2;

    //! Does the work for the two different interfaces
    polyco generate_work () const;

  private:

    //! Cached polynomial
    Reference::To<polyco> cached;

  };

}

#endif
