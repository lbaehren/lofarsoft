//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PeakCumulative.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_PeakCumulative_h
#define __Pulsar_PeakCumulative_h

#include "Pulsar/RiseFall.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Find the edges of a pulse
  /*!

    Finds the edges at which the cumulative power falls below a
    threshold for the last time (rise), and where it remains above
    another threshold for the last time (fall).  The function is
    best-suited to detect the edges of a pulsar main peak.
  
    The threshold attribute defines: 

    - the rising edge of the pulse: threshold * Profile::sum()
    - the falling edge of the pulse: (1-threshold) * Profile::sum()

    The profile baseline is first subtracted from each amplitude;
    therefore, this class makes use of a BaselineEstimator to find the
    off-pulse baseline.  By default, this is the baseline estimator
    defined by Profile::baseline; an alternate estimator can be 
    specified by calling set_baseline_estimator.
    
    If the choose flag is set, then the search for the peak edges will
    be performed twice and the narrowest peak will be returned.  On the
    second try, the search starts half way into the profile. If no edges
    are found, Profile::find_max_phase is called with a duty cycle of
    0.4 in order to find the "peak".  In this case, rise and fall are
    set to reflect the phase of peak-0.2 and peak+0.2, respectively.
    
    If the choose flag is false, then the first peaks found will be returned.
    
  */

  class PeakCumulative : public RiseFall {

  public:

    //! Default constructor
    PeakCumulative ();

    //! Set the profile from which the rise and fall will be computed
    void set_Profile (const Profile*);

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Set the choose
    void set_choose (bool choose);

    //! Get the choose
    bool get_choose () const;

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
    PeakCumulative* clone () const;

  protected:

    //! Compute the rise and fall of the pulse
    void build ();

    //! Flag set when the rise and fall are computed
    bool built;

    //! The threshold
    float threshold;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

    int bin_rise;
    int bin_fall;
    bool choose;

    //! The BaselineEstimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

  };

}


#endif // !defined __Pulsar_PeakCumulative_h
