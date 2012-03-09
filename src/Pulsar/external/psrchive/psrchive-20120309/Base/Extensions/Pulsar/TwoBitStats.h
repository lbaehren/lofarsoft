//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/TwoBitStats.h,v $
   $Revision: 1.7 $
   $Date: 2008/04/21 06:20:00 $
   $Author: straten $ */

#ifndef __TwoBitStats_h
#define __TwoBitStats_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Stores histograms of two-bit low-voltage states
  /*! The TwoBitStats class contains histograms of the number of
    low-voltage states encountered in samples of a given length.
    Refer to the TwoBitCorrection class in baseband/dsp for further
    details.
  */

  class TwoBitStats : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    TwoBitStats ();

    //! Copy constructor
    TwoBitStats (const TwoBitStats& extension);

    //! Operator =
    const TwoBitStats& operator= (const TwoBitStats& extension);

    //! Clone method
    TwoBitStats* clone () const { return new TwoBitStats( *this ); }

    //! Set the number of samples per estimate and number of digitizers
    void resize (unsigned nsample, unsigned ndig);

    //! Get the number of digitizers
    unsigned get_ndig () const;

    //! Get the number of time samples used to estimate undigitized power
    unsigned get_nsample () const;

    //! Set the sampling threshold as a fraction of the noise power
    void set_threshold (float threshold);
    
    //! Get the sampling threshold as a fraction of the noise power
    float get_threshold () const { return threshold; }

    //! Set the cut off power used for impulsive interference excision
    void set_cutoff_sigma (float cutoff_sigma);
    
    //! Get the cut off power for impulsive interference excision
    float get_cutoff_sigma () const { return cutoff_sigma; }

    //! Get the specified histogram
    const std::vector<float>& get_histogram (unsigned idig) const;

    //! Set the specified histogram
    void set_histogram (const std::vector<float>&, unsigned idig);

    //! Set all histogram data to zero
    void zero ();

    //! Get the mean fraction of low voltage states
    double get_mean_Phi (unsigned idig) const;

    //! Get the difference between the measured and theoretical distributions
    double get_distortion (unsigned idig) const;

  protected:

    //! Number of digitizers
    unsigned ndig;

    //! Number of samples used to estimate undigitized power
    unsigned nsample;

    //! Sampling threshold as a fraction of the noise power
    float threshold;

    //! Cut off power used for impulsive interference excision
    float cutoff_sigma;

    //! Low-voltage state count histograms
    std::vector< std::vector<float> > histogram;

    //! Throw an exception if idig out of range
    void range_check (unsigned idig, const char* method) const;

  };
 
}

#endif
