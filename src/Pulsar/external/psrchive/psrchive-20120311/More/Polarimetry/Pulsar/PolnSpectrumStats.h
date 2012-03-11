//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnSpectrumStats.h,v $
   $Revision: 1.5 $
   $Date: 2009/06/24 05:11:32 $
   $Author: straten $ */

#ifndef __Pulsar_PolnSpectrumStats_h
#define __Pulsar_PolnSpectrumStats_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/PhaseWeight.h"

#include "FTransformAgent.h"
#include "Stokes.h"

namespace Pulsar {

  class PolnProfile;
  class PolnProfileStats;

  //! Computes polarimetric pulse profile statistics
  class PolnSpectrumStats : public Algorithm
  {   
  public:

    //! Default constructor
    PolnSpectrumStats (const PolnProfile* profile = 0);

    //! Destructor
    ~PolnSpectrumStats();

    //! Set the PolnProfile from which statistics will be derived
    void set_profile (const PolnProfile*);

    //! Set the PolnProfile that defines the last harmonic and baseline
    void select_profile (const PolnProfile*);

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& pulse, PhaseWeight& base) const;

    //! Return the last harmonic chosen in the on-pulse signal
    unsigned get_last_harmonic () const { return last_harmonic; }

    //! Get the fourier transform of the last set profile
    const PolnProfile* get_fourier () const;

    //! Get the Stokes parameters for the specified harmonic
    Stokes< std::complex< Estimate<double> > > get_stokes (unsigned) const;

    //! Get the real component statistics
    const PolnProfileStats* get_real () const;

    //! Get the imaginary component statistics
    const PolnProfileStats*  get_imag () const;

    //! Returns the total determinant of the on-pulse phase bins
    Estimate<double> get_total_determinant () const;

    //! Returns the variance of the baseline for the specified polarization
    std::complex< Estimate<double> > get_baseline_variance (unsigned) const;

    //! Set the fourier transform plan
    void set_plan (FTransform::Plan*);

  protected:

    //! The PolnProfile from which statistics will be derived
    Reference::To<const PolnProfile> profile;

    //! The Fourier transform of the profile
    Reference::To<PolnProfile> fourier;

    //! Computes the statistics of the real component
    Reference::To<PolnProfileStats> real;

    //! Computes the statistics of the imaginary component
    Reference::To<PolnProfileStats> imag;

    //! When, true the onpulse and baseline estimators have been selected
    bool regions_set;

    PhaseWeight onpulse;
    PhaseWeight baseline;

    unsigned last_harmonic;

    //! Computes the phase bin masks
    void build ();

    //! The fourier transform plan (useful in multi-threaded applications)
    FTransform::Plan* plan;

  };

}


#endif



