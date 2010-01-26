//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PeakConsecutive.h,v $
   $Revision: 1.4 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_PeakConsecutive_h
#define __Pulsar_PeakConsecutive_h

#include "Pulsar/RiseFall.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Finds pulse defined by number of consecutive points above threshold
  /*!
    Finds the edges at which the power remains above a threshold
    for a while.    
  */

  class PeakConsecutive : public RiseFall {

  public:

    //! Default constructor
    PeakConsecutive ();

    //! Set the profile from which the rise and fall will be computed
    void set_Profile (const Profile*);

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Set the consecutive
    void set_consecutive (unsigned consecutive);

    //! Get the consecutive
    unsigned get_consecutive () const;

    //! Set the BaselineEstimator used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Get the BaselineEstimator used to find the off-pulse phase bins
    const BaselineEstimator* get_baseline_estimator () const;
    BaselineEstimator* get_baseline_estimator ();

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Get the pulse phase bin indeces of the rise and fall
    void get_indeces (int& rise, int& fall) const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return new copy of self
    PeakConsecutive* clone () const;

  protected:

    //! Set the weights between rise and fall, or each rise/fall pair
    void calculate (PhaseWeight* weight);

    //! Compute the rise and fall of the pulse
    void build ();

    //! Merge all on pulse regions into one
    bool merge_regions;

    //! Flag set when the rise and fall are computed
    bool built;

    //! The threshold
    float threshold;

    //! The number of bins that must remain above threshold
    unsigned consecutive;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

    int bin_rise;
    int bin_fall;

    std::vector<unsigned> on_transitions;
    std::vector<unsigned> off_transitions;

    //! The BaselineEstimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

  };

}

void regions( unsigned ndat, const float* data, 
	      unsigned istart, unsigned istop,
	      unsigned region_size, float cutoff,
	      std::vector<unsigned>& on_transitions,
	      std::vector<unsigned>& off_transitions );

#endif // !defined __Pulsar_PeakConsecutive_h
