//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicSpectrum_h
#define __Pulsar_DynamicSpectrum_h

#include "Pulsar/Algorithm.h"
#include "Estimate.h"
#include "MJD.h"

namespace Pulsar
{
  class Archive;
  class Flux;

  //! Computes dynamic spectrum (flux vs time/freq) of an Archive
  class DynamicSpectrum : public Algorithm
  {   
  public:

    //! Default constructor
    DynamicSpectrum();

    //! Destructor
    ~DynamicSpectrum();

    //! Set the Archive to use
    void set_Archive(const Archive* _arch);

    //! Set the flux computation method
    void set_flux_method(Flux *flux);

    //! Compute dynamic spectrum
    void compute();

    //! Reset memory allocation
    void reset();

    //! Get value w/ uncertainty
    Estimate<double> get_ds(unsigned ichan, unsigned isub);

    //! Get pointer to values
    const double *get_ds();

    //! Get pointer to uncertainties
    const double *get_ds_err();

    //! Get number of channels in ds
    const unsigned get_nchan() const { return nchan; }

    //! Get number of subints in ds
    const unsigned get_nsub() const { return nsub; }

    //! Get time of subint relative to start of file
    const MJD get_rel_epoch(unsigned isub);

    //! Get freq of a given channel
    const double get_freq(unsigned ichan, unsigned isub=0);

    //! Unload the results to a file.
    void unload(const std::string& filename);

    //! Load results from a file.
    static DynamicSpectrum *load(const std::string& filename);

  protected:

    //! The input archive
    Reference::To<const Archive> arch;

    //! The flux method
    Reference::To<Flux> flux;

    //! Has DS been computed
    bool computed;

    //! Num chans in use
    unsigned nchan;

    //! Num subints in use
    unsigned nsub;

    //! Dynamic spectrum array (nchan by nsubint)
    double *ds_data;

    //! Dynamic spectrum uncertainty array
    double *ds_data_err;

    //! The max DS value
    double ds_max;

    //! The min DS value
    double ds_min;

  };

}


#endif



