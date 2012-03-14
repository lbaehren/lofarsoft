//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileStats.h,v $
   $Revision: 1.11 $
   $Date: 2009/06/24 05:11:32 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileStats_h
#define __Pulsar_PolnProfileStats_h

#include "Pulsar/ProfileStats.h"
#include "Stokes.h"

namespace Pulsar {

  class PolnProfile;

  //! Computes polarimetric pulse profile statistics
  class PolnProfileStats : public Algorithm {
    
  public:

    //! Default constructor
    PolnProfileStats (const PolnProfile* profile = 0);

    //! Destructor
    ~PolnProfileStats();

    //! Avoid on-pulse phase bins with det(rho) close to zero
    void set_avoid_zero_determinant (bool flag=true);

    //! Set the PolnProfile from which statistics will be derived
    void set_profile (const PolnProfile*);

    //! Set the PolnProfile that defines the baseline and on-pulse regions
    void select_profile (const PolnProfile*);

    //! Set the Profile that defines the baseline and on-pulse regions
    void select_profile (const Profile*);

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& pulse, PhaseWeight& base) const;

     //! Get the ProfileStats estimator
    ProfileStats* get_stats () { return stats; }
    const ProfileStats* get_stats () const { return stats; }

    //! Get the Stokes parameters for the specified phase bin
    Stokes< Estimate<double> > get_stokes (unsigned ibin) const;

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total_intensity () const;

    //! Returns the total polarized flux of the on-pulse phase bins
    Estimate<double> get_total_polarized () const;

    //! Returns the total determinant of the on-pulse phase bins
    Estimate<double> get_total_determinant () const;

    //! Returns the total linearly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_linear () const;

    //! Returns the total circularly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_circular () const;

    //! Returns the total absolute value of circularly polarized flux
    Estimate<double> get_total_abs_circular () const;

    //! Returns the variance of the baseline for the specified polarization
    Estimate<double> get_baseline_variance (unsigned ipol) const;

  protected:

    //! The PolnProfile from which statistics will be derived
    Reference::To<const PolnProfile> profile;

    //! Computes the statistics of a single pulse profile
    Reference::To<ProfileStats> stats;

    //! True when the onpulse and baseline regions have been set
    bool regions_set;

    //! True when zero determinant phase bins should be avoided
    bool avoid_zero_determinant;

    //! The variance of the total intensity baseline
    mutable Stokes< Estimate<double> > baseline_variance;

    //! 
    //! Computes the phase bin masks
    void build ();

  };

}


#endif



