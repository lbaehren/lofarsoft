//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/MoreProfiles.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

#ifndef __Pulsar_MoreProfiles_h
#define __Pulsar_MoreProfiles_h

#include "Pulsar/DataExtension.h"

namespace Pulsar
{
  /*! Extra pulse profiles to represent other dimensions */
  class MoreProfiles : public DataExtension
  {
  public:

    //! Construct with a name
    MoreProfiles (const char* name);

    //! Copy constructor
    MoreProfiles (const MoreProfiles&);

    //! Return the number of phase bins
    unsigned get_nbin () const;
    
    //! Resize the data area
    void resize (unsigned nbin);

    //! set the weight of the profile
    void set_weight (float wt);

    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! rotates the profile by phase (in turns)
    void rotate_phase (double phase);

    //! set all amplitudes to zero
    void zero ();

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring phase bins in profile
    void bscrunch_to_nbin (unsigned nbin);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

    //! integrate information from another Profile
    void integrate (const Profile*);

    //! average information from another MoreProfiles
    virtual void average (const MoreProfiles*);

    //! resize the profile vector
    virtual void resize (unsigned nprofile, unsigned nbin);

    //! get the size of the profile vector
    virtual unsigned get_size () const;

    //! get the ith Profile
    virtual Profile* get_Profile (unsigned i);

    //! get the ith const Profile
    virtual const Profile* get_Profile (unsigned i) const;

  protected:

    //! vector of Profile instances
    std::vector< Reference::To<Profile> > profile;

  };

}

#endif
