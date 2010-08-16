//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationExpert.h,v $
   $Revision: 1.13 $
   $Date: 2008/02/05 05:25:30 $
   $Author: straten $ */

#ifndef __Pulsar_IntegrationExpert_h
#define __Pulsar_IntegrationExpert_h

#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Provides access to private and protected members of Integration
  /*! Some methods of the Integration class are protected or private only
    so that they may not be naively called.  This class provides 
    access to developers without the need to:
    <UL>
    <LI> unprotect methods, or
    <LI> make friends with every class or function that requires access
    </UL>
  */
  class Integration::Expert : public Reference::Able {

  public:

    Expert (Integration* inst)
    { instance = inst; }

    //! Set the number of pulsar phase bins
    void set_nbin (unsigned numbins)
    { instance->set_nbin (numbins); }

    //! Set the number of frequency channels
    void set_nchan (unsigned numchan)
    { instance->set_nchan (numchan); }

    //! Set the number of polarization measurements
    void set_npol (unsigned numpol)
    { instance->set_npol (numpol); }

    //! Set the dimensions of the data container
    void resize (unsigned npol=0, unsigned nchan=0, unsigned nbin=0)
    { instance->resize (npol, nchan, nbin); }

    //! Inserts Profiles from Integration into this
    void insert (Integration* from)
    { instance->insert (from); }

    //! Rotate each profile by time (in seconds); updates the epoch attribute
    void rotate (double time)
    { instance->rotate (time); }

    //! Rotate each profile by phase; does not update the epoch attribute
    void rotate_phase (double phase)
    { instance->rotate_phase (phase); }

    //! Integrate profiles from neighbouring chans
    void fscrunch (unsigned nscrunch = 0)
    { instance->fscrunch (nscrunch); }

    //! Integrate profiles from single polarizations into one total intensity
    void pscrunch ()
    { instance->pscrunch (); }

    //! Dedispersion worker function
    void dedisperse (unsigned ichan, unsigned kchan, double dm, double f0)
    { instance->dedisperse (ichan, kchan, dm, f0); }

    //! Defaraday worker function
    void defaraday (unsigned ichan, unsigned kchan, double rm, double f0)
    { instance->defaraday (ichan, kchan, rm, f0); }

    //! Swap the two specified Profiles
    void swap_profiles (unsigned ipol, unsigned ichan,
			unsigned jpol, unsigned jchan)
    { instance->swap_profiles (ipol, ichan, jpol, jchan); }

    //! Copy the data from 'from' into 'this'
    void copy (const Integration* subint, bool management = true)
    { instance->copy (subint, management); }

    //! Combine the data from 'from' into 'this'
    void combine (const Integration* from)
    { instance->combine (from); }

    //! Perform the congruence transformation on each polarimetric profile
    void transform (const Jones<float>& response)
    { instance->transform (response); }

    //! Perform frequency response on each polarimetric profile
    void transform (const std::vector< Jones<float> >& response)
    { instance->transform (response); }

    //! Convert polarimetric data to the specified state
    void convert_state (Signal::State state)
    { instance->convert_state (state); }

    //! Leading edge of phase bin zero = polyco predicted phase zero
    void set_zero_phase_aligned (bool flag)
    { instance->zero_phase_aligned = flag; }

    //! Use with care
    std::vector< std::vector< Reference::To<Profile> > >& profiles ()
    { return instance->profiles; }

    //! Return true if the Integration has a parent Archive
    bool has_parent ()
    { return instance->parent; }

    //! Provide access to the parent Archive
    const Archive* get_parent ()
    { return instance->parent; }

  private:

    //! instance
    Reference::To<Integration, false> instance;

  };

}

#endif
