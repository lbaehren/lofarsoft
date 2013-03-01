//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/LastHarmonic.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_LastHarmonic_h
#define __Pulsar_LastHarmonic_h

#include "Pulsar/RiseFall.h"
#include "Pulsar/LastSignificant.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Finds the last significant harmonic in a power spectral density
  /*!
    The last significant harmonic is defined by number of
    consecutive points above threshold.
  */

  class LastHarmonic : public RiseFall
  {

  public:

    //! Default constructor
    LastHarmonic ();

    //! Set the profile from which the last harmonic will be computed
    void set_Profile (const Profile*);

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Set the number of consecutive points that must remain above threshold
    void set_consecutive (unsigned consecutive);

    //! Get the number of consecutive points that must remain above threshold
    unsigned get_consecutive () const;

    //! Get the index of the last significant harmonic
    unsigned get_last_harmonic () const;

    //! Set the BaselineEstimator used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Get the BaselineEstimator used to find the off-pulse phase bins
    const BaselineEstimator* get_baseline_estimator () const;
    BaselineEstimator* get_baseline_estimator ();

    const PhaseWeight* get_baseline () const;

    //! Get the harmonic indeces of the rise and fall (rise always equals 1)
    void get_indeces (int& rise, int& fall) const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return new copy of self
    LastHarmonic* clone () const;

  protected:

    //! Compute the rise and fall of the pulse
    void build ();

    //! Flag set when the rise and fall are computed
    bool built;

    int bin_rise;
    int bin_fall;

    //! The estimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! The basline returned by the estimator
    Reference::To<PhaseWeight> baseline;

    //! The algorithm used to find the last signficant harmonic
    LastSignificant significant;

  };

}

#endif // !defined __Pulsar_LastHarmonic_h
