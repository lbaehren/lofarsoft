//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/UnloadOptions.h,v $
   $Revision: 1.4 $
   $Date: 2010/05/02 16:08:46 $
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

    //! The application produces an output for each input
    void set_output_each (bool);

    //! Return the output filename for the input
    std::string get_output_filename (const Archive*);

    //! Set the extension added to output filenames
    void set_extension (const std::string&);
    std::string get_extension () const { return extension; }

    //! Set the directory to which output files will be written
    void set_directory (const std::string&);
    std::string get_directory () const { return directory; }

    //! Return true if one or more unload options have been set
    bool get_unload_setup () const;

    //! Add options to the menu
    virtual void add_options (CommandLine::Menu&);

    //! Verify setup
    virtual void setup ();

    //! Unload the archive
    virtual void finish (Archive*);

  private:

    //! The application produces and output for each input
    bool output_each;

    //! Overwrite files
    bool overwrite;

    //! Write files with new extension
    std::string extension;

    //! Write files to new directory
    std::string directory;

  };

}

#endif
