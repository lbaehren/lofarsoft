//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/NoiseStatistics.h,v $
   $Revision: 1.4 $
   $Date: 2008/08/20 02:30:57 $
   $Author: straten $ */

#ifndef __Pulsar_NoiseStatistics_h
#define __Pulsar_NoiseStatistics_h

#include "ReferenceAble.h"

namespace Pulsar {

  class Profile;

  //! Computes the noise to Fourier noise ratio
  /*!  

    By Parseval's Theorem, the energy in the noise of the off-pulse
    baseline in the phase domain should be equal to the energy in the
    noise of the post-Nyquist harmonics in the frequency domain.

    However, if the profile baseline is corrupted by some periodic
    structure, then the r.m.s. computed in the phase domain will be
    larger than that in the frequency domain, where the periodic
    corruption is likely isolated in some low harmonic.

    Likewise, if the profile is unresolved, then the r.m.s. computed
    in the frequency domain will be larger than that in the phase
    domain because there are no post-Nyquist harmonics.

    The noise to Fourier noise ratio is a way to detect either of
    these problems.  If it is much greater than unity, then there is
    likely some low frequency structure in the profile baseline.  If
    it is much less than unity, then it is likely that the profile
    contains unresolved structure.  In either case, there is likely
    a good reason to discard the data.

  */

  class NoiseStatistics : public Reference::Able {

  public:

    //! Default constructor
    NoiseStatistics ();

    //! Set the fractional number of high frequencies used to calculate noise
    void set_baseline_fourier (float width);

    //! Set the fractional number of phase bins used to calculate noise
    void set_baseline_time (float width);

    //! Return the noise to Fourier noise ratio
    float get_nfnr (const Profile* profile);

  protected:

    float baseline_fourier;
    float baseline_time;

  };

}

#endif
