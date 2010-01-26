//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardOptions.h,v $
   $Revision: 1.5 $
   $Date: 2009/09/02 02:54:31 $
   $Author: straten $ */

#ifndef __Pulsar_StandardOptions_h
#define __Pulsar_StandardOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Standard interpreter command line options
  class StandardOptions : public Application::Options
  {
  public:

    //! Default constructor
    StandardOptions ();

    //! Preprocessing tasks implemented by partially derived classes
    virtual void process (Archive*);

    //! Add to the jobs
    void add_job (const std::string& job);

    //! Load a script into the jobs
    void add_script (const std::string& job);

    //! Add to the default preprocessing jobs (ignored if jobs are set)
    void add_default_job (const std::string& job);

    //! Provide access to the interpreter
    Interpreter* get_interpreter ();

  private:

    //! Add options to the menu
    void add_options (CommandLine::Menu&);

    // Preprocessing jobs
    std::vector<std::string> jobs;

    // Default preprocessing jobs
    std::vector<std::string> default_jobs;

    // standard interpreter
    Reference::To<Interpreter> interpreter;


  };

}

#endif
