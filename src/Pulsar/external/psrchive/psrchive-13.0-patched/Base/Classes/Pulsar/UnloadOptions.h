//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/UnloadOptions.h,v $
   $Revision: 1.3 $
   $Date: 2009/09/02 02:54:31 $
   $Author: straten $ */

#ifndef __Pulsar_UnloadOptions_h
#define __Pulsar_UnloadOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Unload interpreter command line options
  class UnloadOptions : public Application::Options
  {
  public:

    //! Default constructor
    UnloadOptions ();

    //! Add options to the menu
    virtual void add_options (CommandLine::Menu&);

    //! Verify setup
    virtual void setup ();

    //! Unload the archive
    virtual void finish (Archive*);

  private:

    //! Unload
    bool unload;

    //! Overwrite files
    bool overwrite;

    //! Write files with new extension
    std::string extension;

    //! Write files to new directory
    std::string directory;

  };

}

#endif
