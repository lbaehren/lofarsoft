//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/CalSource.h,v $
   $Revision: 1.3 $
   $Date: 2009/11/29 12:13:49 $
   $Author: straten $ */

#ifndef __Pulsar_CalSource_h
#define __Pulsar_CalSource_h

#include "Pulsar/Correction.h"
#include "Pulsar/Config.h"

namespace Pulsar {
  
  //! Corrects the Signal::Source type attribute of the Archive
  /*! If the Archive::type attribute is set to Signal::PolnCal and the
    Archive::source attribute is in any of the recognized lists, then
    the Archive::type attribute may be corrected. */
  class CalSource : public Correction
  {

  public:
    
    //! The default on filename
    static Option<std::string> default_on;

    //! The default off filename
    static Option<std::string> default_off;

    //! Default constructor
    CalSource ();
      
    // Return the name of the check
    std::string get_name () { return "CalSource"; }

    //! Correct the Signal::Source type attribute of the Archive
    void apply (Archive* archive);

    //! Get the number of FluxCalOn aliases
    unsigned get_on_size () const;

    //! Get the number of FluxCalOff aliases
    unsigned get_off_size () const;

    //! Get the filename from which FluxCalOn aliases were read
    std::string get_on_filename () const;

    //! Get the filename from which FluxCalOff aliases were read
    std::string get_off_filename () const;

  protected:

    //! Source names matching FluxCalOn
    mutable std::vector<std::string> fluxcal_on;

    //! Source names matching FluxCalOff
    mutable std::vector<std::string> fluxcal_off;

    //! Name of file from which FluxCalOn aliases were read
    mutable std::string on_filename;

    //! Name of file from which FluxCalOff aliases were read
    mutable std::string off_filename;

    void load () const;
    mutable bool loaded;
  };
 

}

#endif
