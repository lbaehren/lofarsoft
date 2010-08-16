//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/TimerIntegration.h,v $
   $Revision: 1.12 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Timer_Integration_h
#define __Timer_Integration_h

#include <stdio.h>
#include <vector>

#include "Pulsar/Integration.h"
#include "mini.h"

namespace Pulsar {
  
  class TimerIntegration : public Integration {

  public:

    friend class TimerArchive;

    //! Null constructor simply intializes defaults
    TimerIntegration () { init(); }

    //! Copy constructor
    TimerIntegration (const TimerIntegration& t_subint);

    //! General copy constructor
    TimerIntegration (const Integration& subint);

    //! Destructor deletes data area
    virtual ~TimerIntegration ();

    //! Copy an Integration
    virtual void copy (const Integration* subint, bool management = true);

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
    virtual MJD get_epoch () const;
    //! Set the epoch of the rising edge of bin zero
    virtual void set_epoch (const MJD& mjd);

    //! Get the total time integrated (in seconds)
    virtual double get_duration () const;
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds);
        
    //! Get the folding period (in seconds)
    virtual double get_folding_period () const;
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds);

    //! load the subint from file
    void load (FILE * fptr, int wts_and_bpass, bool big_endian=true);

    //! unload the subint to file
    void unload (FILE * fptr) const;

    //! Resizes the dimensions of the data area
    virtual void resize (unsigned npol, unsigned nchan, unsigned nbin);

    //! Inserts Profiles from Integration into this
    virtual void insert (Integration*);

  protected:

    struct mini mini;

    //! number of polarization measurments
    unsigned npol;

    //! number of sub-chans
    unsigned nchan;
    
    //! number of bins
    unsigned nbin;

    //! weight of each channel/poln
    std::vector<float> wts;

    //! median of average bandpass
    std::vector< std::vector<float> > med;

    //! bandpass averaged over bins
    std::vector< std::vector<float> > bpass;

    //! Set the number of pulsar phase bins
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_nbin (unsigned numbins) { nbin = numbins; }

    //! Set the number of frequency channels
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_nchan (unsigned numchan) { nchan = numchan; }

    //! Set the number of polarization measurements
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_npol (unsigned numpol) { npol = numpol; }

    //! Set all values to null
    void init ();

    //! Load in the "new-style"
    void load_extra (FILE* fptr, bool big_endian);

    //! Load in the "old-style"
    void load_old (FILE* fptr, bool big_endian);

    //! Unpack the Pointing extension attributes after loading mini header
    void unpack_Pointing ();

    //! Pack the Pointing extension attributes before unloading mini header
    void pack_Pointing () const;

  };

  //! load a Profile from a timer archive
  void TimerProfile_load (FILE* fptr, Profile* profile, bool big_endian=true);

  //! unload a Profile to a timer archive
  void TimerProfile_unload (FILE* fptr, const Profile* profile, int poln);

}

#endif
