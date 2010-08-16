//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Index.h,v $
   $Revision: 1.1 $
   $Date: 2008/11/12 07:45:10 $
   $Author: straten $ */

#ifndef __Pulsar_Index_h
#define __Pulsar_Index_h

#include <iostream>

namespace Pulsar {

  //! Combines an index value and integrate flag
  class Index {

  public:

    //! Default constructor
    Index (unsigned value = 0, bool integrate = false);

    //! Get the value of the index
    unsigned get_value () const { return index; }
    //! Set the value of the index
    void set_value (unsigned value) { index = value; }

    //! Get the integrate flag
    bool get_integrate () const { return integrate; }
    //! Set the integrate flag
    void set_integrate (bool flag) { integrate = flag; }

  protected:

    unsigned index;
    bool integrate;

  };

  std::ostream& operator << (std::ostream& os, const Index&);
  std::istream& operator >> (std::istream& is, Index&);

  class Profile;
  class Archive;

  //! Return the requested profile, cloning and integrating when needed
  const Profile*
  get_Profile (const Archive* data,
	       Index subint, Index pol, Index chan);

  class Integration;

  //! Return the requested profile, cloning and integrating when needed
  const Integration*
  get_Integration (const Archive* data, Index subint);

  class PolnProfile;

  //! Return a new PolnProfile with state == Signal::Stokes
  /*! This function clones, integrates, & converts only when necessary */
  const PolnProfile* 
  get_Stokes (const Archive* data, Index subint, Index chan);

}

#endif
