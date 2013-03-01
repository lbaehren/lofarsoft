//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardData.h,v $
   $Revision: 1.5 $
   $Date: 2009/06/24 05:11:32 $
   $Author: straten $ */

#ifndef __Calibration_StandardData_H
#define __Calibration_StandardData_H

#include "Pulsar/PolnProfileStats.h"
#include "MEAL/NormalizeStokes.h"

namespace Calibration {

  //! Computes the normalized Stokes parameters in each phase bin

  /*!

    The apparent pulsar flux varies with time, and it is necessary to
    normalize the observed Stokes parameters when modelling.  Because
    the total intensity is distorted by polarimetric tranformations,
    it is necessary to estimate the apparent gain variations using the
    polarimetic invariant interval.

    Before this class was written, the NormalizeStokes class was used
    to normalize each set of Stokes parameters by their invariant.
    However, when the signal-to-noise ratio is low, the invariant can
    be less than or equal to zero and as a result many points had to
    be discarded.

    This problem is overcome by normalizing each set of Stokes
    parameters by the mean of the invariant interval over all on-pulse
    phase bins.  This approach has the following benefits:

    1) even when the Stokes parameters in a single phase bin are highly
    polarized, the other less polarized on-pulse phase bins will produce
    a mean invariant greater than zero

    2) the mean invariant interval has sqrt(onpulse_nbin) greater S/N

    3) the mean invariant is chi distributed with onpulse_nbin * 4
    degrees of freedom (much more like normally distributed than 4 d.o.f)

  */

  class StandardData : public Reference::Able {

  public:

    //! Default constructor
    /*! If specified, baseline and on-pulse regions are defined by select */
    StandardData (const Pulsar::PolnProfile* select = 0);

    //! Select the baseline and on-pulse regions from profile
    void select_profile (const Pulsar::PolnProfile*);

    //! Set the profile from which estimates will be derived
    void set_profile (const Pulsar::PolnProfile*);

    //! Normalize estimates by the average determinant
    void set_normalize (bool);

    //! Get the Stokes parameters of the specified phase bin
    Stokes< Estimate<double> > get_stokes (unsigned ibin);

    //! Get the algorithm used to compute the profile statistics
    Pulsar::PolnProfileStats* get_poln_stats ();

 protected:

    Reference::To< Pulsar::PolnProfileStats > stats;
    Reference::To< MEAL::NormalizeStokes > normalize;

    Estimate<double> total_determinant;

  };

}

#endif
