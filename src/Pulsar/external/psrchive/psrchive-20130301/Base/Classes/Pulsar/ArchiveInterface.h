//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ArchiveInterface.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/24 05:01:39 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTI_h
#define __Pulsar_ArchiveTI_h

#include "Pulsar/Archive.h"
#include "TextInterface.h"

namespace Pulsar
{

  //! Provides a text interface to get and set Archive attributes
  class Archive::Interface : public TextInterface::To<Archive>
  {

  public:

    //! Default constructor that takes an optional instance
    Interface ( Archive* = 0 );

    //! Set the instance to which this interface interfaces
    void set_instance (Archive*) ;

    //! clone this text interface
    TextInterface::Parser *clone();
    
    //! Get the interface name
    std::string get_interface_name() const { return "Archive::Interface"; }

  };

}


#endif
