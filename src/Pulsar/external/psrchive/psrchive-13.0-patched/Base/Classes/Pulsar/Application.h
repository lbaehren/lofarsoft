//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Application.h,v $
   $Revision: 1.12 $
   $Date: 2009/09/22 11:43:54 $
   $Author: straten $ */

#ifndef __Pulsar_Application_h
#define __Pulsar_Application_h

#include "CommandLine.h"
#include "Functor.h"

namespace Pulsar {

  class Archive;

  //! Application with basic command line options
  class Application : public Reference::Able
  {
  public:

    //! Construct with the application name and a short description
    Application (const std::string& name,
		 const std::string& description);

    //! Execute the main loop
    virtual int main (int argc, char** argv);

    //! Application options
    class Options;

    //! Add options to the application
    void add (Options*);

    //! Get the application name
    std::string get_name () const;

    //! Get the application description
    std::string get_description () const;

    //! Get the verbosity flag
    bool get_verbose () const;

    //! Return true if the application needs to save data
    virtual bool must_save ();

  protected:

    //! Available options
    std::vector< Reference::To<Options> > options;

    //! Add command line options
    virtual void add_options (CommandLine::Menu&) = 0;

    //! Parse the command line options
    virtual void parse (int argc, char** argv);

    //! Operate in quiet mode
    virtual void set_quiet ();

    //! Operate in verbose mode
    virtual void set_verbose ();

    //! Operate in very verbose mode
    virtual void set_very_verbose ();

    //! Any extra setup before running main loop
    virtual void setup ();

    //! The main loop
    virtual void run ();

    //! Data analysis tasks implemented by most derived classes
    virtual void process (Archive*) = 0;

    //! Any final work after main loop finishes
    virtual void finalize ();

    //! set to true if this application has an online manual
    bool has_manual;

    //! set to true if this application should update the processing history
    bool update_history;

    // name of the application
    std::string name;

    // short description of the application
    std::string description;

    // revision information
    std::string version;

    // command line used to execute this program
    std::string command;

    // list of file names on which to operate
    std::vector <std::string> filenames;

    // name of file containing list of Archive filenames
    std::string metafile;

    // verbosity flags
    bool verbose;
    bool very_verbose;

  };

  //! Describes application command line options.
  class Application::Options : public Reference::Able
  {
    public:

    //! Additional getopt options
    virtual void add_options (CommandLine::Menu&) = 0;

    //! Additional one-time setup tasks
    virtual void setup ();

    //! Additional per-Archive processing tasks
    virtual void process (Archive*);

    //! Additional per-Archive finishing tasks (e.g., unload)
    virtual void finish (Archive*);

    //! Additional one-time final tasks
    virtual void finalize();

  protected:

    friend class Application;

    //! The application to which these options belong
    Reference::To<Application,false> application;

  };

}

#endif
