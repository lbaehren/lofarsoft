//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/DeltaPA.h,v $
   $Revision: 1.4 $
   $Date: 2009/01/27 07:42:35 $
   $Author: straten $ */

#ifndef __Pulsar_DeltaPA_h
#define __Pulsar_DeltaPA_h

#include "Estimate.h"
#include <vector>

namespace Pulsar {

  class PolnProfile;

  //! Computes mean position angle difference from weighted cross-correlation
  class DeltaPA  {

  public:

    //! Default constructor
    DeltaPA () { threshold = 3.0; }

    //! Destructor
    ~DeltaPA () { }

    //! Get the position angle difference mean
    Estimate<double> get (const PolnProfile* p0, const PolnProfile* p1) const;

    //! Get the number of phase bins used in the last call to get
    unsigned get_used_bins () const { return used_bins; }

    //! Set the cutoff threshold in units of the baseline noise
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! Set the phase bins to be included in the mean
    void set_include (const std::vector<unsigned>& bins);

    //! Set the phase bins to be excluded from the mean
    void set_exclude (const std::vector<unsigned>& bins);

  protected:

    float threshold;
    mutable unsigned used_bins;

    //! Include only the specified phase bins
    std::vector<unsigned> include_bins;

    //! Exclude the specified phase bins
    std::vector<unsigned> exclude_bins;
  };

}

#endif
