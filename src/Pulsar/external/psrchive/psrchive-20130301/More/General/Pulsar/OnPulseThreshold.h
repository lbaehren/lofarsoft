//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/OnPulseThreshold.h,v $
   $Revision: 1.7 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_OnPulseThreshold_h
#define __Pulsar_OnPulseThreshold_h

#include "Pulsar/OnPulseEstimator.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Uses the baseline statistics to find on-pulse phase bins
  /*! The on-pulse phase bins are those with a value greater than the
    threshold times the rms of the off-pulse phase bins.  Therefore,
    this class makes use of a BaselineEstimator to find the off-pulse
    baseline.  By default, this is the BaselineWindow class. */

  class OnPulseThreshold : public OnPulseEstimator {

  public:

    //! Default constructor
    OnPulseThreshold ();

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Allow negative on-pulse phase bins
    void set_allow_negative (bool);

    //! Set the BaselineEstimator used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Get the BaselineEstimator used to find the off-pulse phase bins
    const BaselineEstimator* get_baseline_estimator () const;
    BaselineEstimator* get_baseline_estimator ();

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return new copy of self
    OnPulseThreshold* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    //! The threshold
    float threshold;

    //! Take the absolute value
    bool allow_negative;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

    //! The BaselineEstimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

  };

}


#endif // !defined __Pulsar_OnPulseThreshold_h
