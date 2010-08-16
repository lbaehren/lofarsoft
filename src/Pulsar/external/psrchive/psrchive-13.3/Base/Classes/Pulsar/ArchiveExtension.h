//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ArchiveExtension.h,v $
   $Revision: 1.3 $
   $Date: 2008/11/28 05:08:11 $
   $Author: straten $ */

#ifndef __Pulsar_Archive_Extension_h
#define __Pulsar_Archive_Extension_h

#include "Pulsar/Archive.h"
#include "extutil.h"

namespace Pulsar
{
  /*!
    Archive-derived classes may provide access to additional informaton
    through Extension-derived objects.
  */
  class Archive::Extension : public Reference::Able
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
  const T* Archive::get () const
  {
    return get_ext<T> (this, "Pulsar::Archive::get<Ext>", verbose > 2);
  }

  template<class T>
  T* Archive::get ()
  {
    return const_cast<T*>
      ( get_ext<T> (this, "Pulsar::Archive::get<Ext>", verbose > 2) );
  }

  /*! If the specified extension type T does not exist, an atempt is made to
      add it using add_extension.  If this fails, NULL is returned. */
  template<class T>
  T* Archive::getadd ()
  {
    T* ext = get<T>();
    if (ext)
      return ext;

    try
    {
      Reference::To<T> add_ext = new T;
      add_extension (add_ext);
      return add_ext;
    }
    catch (Error& error)
    {
      return 0;
    }
  }

}

#endif
