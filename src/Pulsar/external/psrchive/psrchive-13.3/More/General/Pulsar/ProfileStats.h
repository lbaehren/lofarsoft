//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileStats.h,v $
   $Revision: 1.12 $
   $Date: 2009/08/20 11:32:40 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileStats_h
#define __Pulsar_ProfileStats_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Algorithm.h"

#include "TextInterface.h"
#include "Estimate.h"

namespace Pulsar
{
  class Profile;
  class BaselineEstimator;
  class OnPulseEstimator;

  //! Computes pulse profile statistics
  class ProfileStats : public Algorithm
  {   
  public:

    //! Default constructor
    ProfileStats (const Profile* profile = 0);

    //! Destructor
    ~ProfileStats();

    //! Set the Profile from which statistics will be derived
    void set_profile (const Profile*);

    //! Set the Profile that defines the baseline and on-pulse regions
    void select_profile (const Profile*);

    //! Deselect onpulse phase bins in profile that fall below threshold
    void deselect_onpulse (const Profile* profile, float threshold);

    //! The algorithm used to find the on-pulse phase bins
    void set_onpulse_estimator (OnPulseEstimator*);
    OnPulseEstimator* get_onpulse_estimator () const;

    //! The algorithm used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);
    BaselineEstimator* get_baseline_estimator () const;

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& pulse, PhaseWeight& base) const;

    //! Get the number of phase bins in the on pulse window
    unsigned get_onpulse_nbin () const;

    //! Return true if the specified phase bin is in the on pulse window
    bool get_onpulse (unsigned ibin) const;

    //! Set if the specified phase bin is in the on pulse window
    void set_onpulse (unsigned ibin, bool);

    //! Get the number of phase bins in the baseline window
    unsigned get_baseline_nbin () const;

    //! Return true if the specified phase bin is in the baseline window
    bool get_baseline (unsigned ibin) const;

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total (bool subtract_baseline = true) const;

    //! Returns the variance of the baseline
    Estimate<double> get_baseline_variance () const;

    //! Return the on-pulse phase bin mask
    PhaseWeight* get_onpulse ();

    //! Return the off-pulse baseline mask
    PhaseWeight* get_baseline ();

    //! Return the all pulse phase bin mask
    PhaseWeight* get_all ();

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Text interface to methods
    class Interface;

  protected:

    //! The Profile from which statistics will be derived
    Reference::To<const Profile> profile;

    //! The algorithm used to find the on-pulse phase bins
    Reference::To<OnPulseEstimator> onpulse_estimator;

    //! The algorithm used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! True when the onpulse and baseline regions have been set
    bool regions_set;

    //! The on-pulse phase bin mask
    mutable PhaseWeight onpulse;

    //! The off-pulse baseline mask
    mutable PhaseWeight baseline;

    //! The variance of the total intensity baseline
    mutable Estimate<double> baseline_variance;

    //! Computes the phase bin masks
    void build () const;

  private:

    //! flag set when built
    mutable bool built;
  };

}


#endif



