//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/WeightedFrequency.h,v $
   $Revision: 1.1 $
   $Date: 2008/07/08 02:55:30 $
   $Author: straten $ */

#ifndef __Pulsar_WeightedFrequency_h
#define __Pulsar_WeightedFrequency_h

#include "Pulsar/Config.h"

namespace Pulsar {

  class Archive;

  //! Compute the weighted mean frequency
  class WeightedFrequency
  {

  public:

    //! Compute the weighted mean frequency of a single channel over time
    class OverEpoch;

    //! Compute the weighted mean frequency of a range of frequency channels
    class OverFrequency;

    //! Destructor
    virtual ~WeightedFrequency () { }

    //! Return the weighted mean frequency
    double operator () (unsigned istart, unsigned iend) const;

    //! Get the number of frequencies available
    virtual unsigned get_nindex () const = 0;

    //! Get the frequency for the specified index
    virtual double get_frequency (unsigned index) const = 0;

    //! Get the weight for the specified index
    virtual double get_weight (unsigned index) const = 0;

    //! Round the weighted frequency to the nearest kHz
    static Option<bool> round_to_kHz;

  };

  class WeightedFrequency::OverEpoch : public WeightedFrequency
  {
  public:

    //! Default constructor
    OverEpoch (const Archive*);

    //! Set the archive from which to get weights and frequencies
    void set_Archive (const Archive*);

    //! Set the channel for which to compute the weighted mean
    void set_ichan (unsigned ichan);

    //! Get the number of frequencies available
    unsigned get_nindex () const;

    //! Get the frequency for the specified index
    double get_frequency (unsigned index) const;

    //! Get the weight for the specified index
    double get_weight (unsigned index) const;

  protected:

    Reference::To<const Archive> archive;
    unsigned ichan;
    unsigned ipol;
  };

  class Integration;

  class WeightedFrequency::OverFrequency : public WeightedFrequency
  {
  public:

    //! Default constructor
    OverFrequency (const Integration*);

    //! Set the sub-integration from which to get weights and frequencies
    void set_Integration (const Integration*);

    //! Get the number of frequencies available
    unsigned get_nindex () const;

    //! Get the frequency for the specified index
    double get_frequency (unsigned index) const;

    //! Get the weight for the specified index
    double get_weight (unsigned index) const;

  protected:

    Reference::To<const Integration> subint;
    unsigned ipol;
  };

}

#endif
