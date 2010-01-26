//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SystemCalibratorUnloader.h,v $
   $Revision: 1.1 $
   $Date: 2008/07/25 23:30:01 $
   $Author: straten $ */

#ifndef __SystemCalibratorUnloader_H
#define __SystemCalibratorUnloader_H

#include "Pulsar/SystemCalibrator.h"

namespace Pulsar
{
  //! Unloads Pulsar::SystemCalibrator solution to file
  class SystemCalibrator::Unloader : public Reference::Able
  {
  public:

    //! Default constructor
    Unloader ();

    //! Copy constructor
    // Unloader (const Unloader&);

    //! Set the name of the archive class used to store the result
    virtual void set_archive_class (const std::string&);

    //! Set the filename of the result
    virtual void set_filename (const std::string&);

    //! Create a filename from the archive and sub-integration
    virtual void set_filename (const Archive*, unsigned subint);

    //! Set the extension used when creating a filename
    virtual void set_extension (const std::string&);

    //! Set the name of the program; enables verbosity
    virtual void set_program (const std::string&);

    //! Unload the SystemCalibrator solution using the current settings
    virtual void unload (SystemCalibrator*);

  protected:

    std::string archive_class;
    std::string filename;
    std::string extension;
    std::string program;

  };

}

#endif
