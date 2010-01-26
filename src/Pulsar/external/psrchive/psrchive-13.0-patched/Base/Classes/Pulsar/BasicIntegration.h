//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicIntegration.h,v $
   $Revision: 1.12 $
   $Date: 2007/12/06 20:03:35 $
   $Author: demorest $ */

#ifndef __BasicIntegration_h
#define __BasicIntegration_h

#include "Pulsar/Integration.h"
  
namespace Pulsar {

  //! BasicIntegration class
  class BasicIntegration : public Integration {

  public:

    //! null constructor
    BasicIntegration () { init(); }

    //! copy constructor
    BasicIntegration (const BasicIntegration& subint);

    //! copy constructor
    BasicIntegration (const Integration* subint);

    //! destructor
    virtual ~BasicIntegration () { }

    //! Return the pointer to a new copy of self
    virtual Integration* clone () const;

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nchan () const { return nchan; }

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_npol () const { return npol; }

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nbin () const { return nbin; }
 
    //! Get the epoch of the rising edge of bin zero
    virtual MJD get_epoch() const { return epoch; }
    //! Set the epoch of the rising edge of bin zero
    virtual void set_epoch (const MJD& mjd) { epoch = mjd; }

    //! Get the total time integrated (in seconds)
    virtual double get_duration() const { return duration; }
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) { duration = seconds; }

    //! Get the folding period (in seconds)
    virtual double get_folding_period() const { return pfold; }
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds) { pfold = seconds; }

  protected:

    //! Set the number of pulsar phase bins
    /*! Called by Integration methods to update child attribute */
    virtual void set_nbin (unsigned numbins) { nbin = numbins; }

    //! Set the number of frequency channels
    /*! Called by Integration methods to update child attribute */
    virtual void set_nchan (unsigned numchan) { nchan = numchan; }

    //! Set the number of polarization measurements
    /*! Called by Integration methods to update child attribute */
    virtual void set_npol (unsigned numpol) { npol = numpol; }

    //! number of polarization measurments
    unsigned npol;

    //! number of sub-chans
    unsigned nchan;
    
    //! number of bins
    unsigned nbin;

    //! Epoch of the rising edge of bin zero
    MJD epoch;

    //! duration of integration
    double duration;

    //! folding period (in seconds)
    double pfold;

    //! initialize all attributes to null
    void init();

  };

}

#endif
