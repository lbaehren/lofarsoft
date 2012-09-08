//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ProfileExtension.h,v $
   $Revision: 1.4 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_Extension_h
#define __Pulsar_Profile_Extension_h

#include "Pulsar/Profile.h"
#include "extutil.h"

namespace Pulsar
{
  /*!
    Profile-derived classes may provide access to additional informaton
    through Extension-derived objects.
  */
  class Profile::Extension : public Reference::Able
  {
  public:

    //! Construct with a name
    Extension (const char* name);

    //! Destructor
    virtual ~Extension ();
    
    //! Return a new copy-constructed instance identical to this instance
    virtual Extension* clone () const = 0;
    
    //! Return a text interface that can be used to access this instance
    virtual TextInterface::Parser* get_interface () { return 0; }
    
    //! Integrate information from another Profile
    virtual void integrate (const Profile* subint) { }

    //! Return the name of the Extension
    std::string get_extension_name () const;
    
    //! Return an abbreviated name that can be typed relatively quickly
    virtual std::string get_short_name () const;
    
  protected:
    
    //! Extension name - useful when debugging
    std::string extension_name;
    
  };

  /*! e.g. MyExtension* ext = archive->get<MyExtension>(); */
  template<class T>
  const T* Profile::get () const
  {
    return get_ext<T> (this, "Pulsar::Profile::get<Ext>", verbose);
  }

  template<class T>
  T* Profile::get ()
  {
    return const_cast<T*>
      ( get_ext<T> (this, "Pulsar::Profile::get<Ext>", verbose) );
  }

}

#endif
