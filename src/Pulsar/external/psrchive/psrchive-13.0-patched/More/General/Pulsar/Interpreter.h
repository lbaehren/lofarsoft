/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __Pulsar_Interpreter_h
#define __Pulsar_Interpreter_h

#include "Pulsar/FixFluxCal.h"
#include "Pulsar/SetReceiver.h"

#include "CommandParser.h"
#include "tostring.h"
#include "Reference.h"

#include <map>
#include <stack>

namespace Pulsar {
  
  class Archive;

  //! Pulsar data processing command language interpreter
  /*!
    The Interpreter class is a command language interpreter that
    provides access to PSRCHIVE data processing algorithms.
    
    The Interpreter maintains a stack and a named list (map) of
    Pulsar::Archive instances.  Archives in the stack provide a
    convenient history mechanism, and Archives in the map are
    referenced by name, a single keyword.

    All operations perform on the top of the stack.  The only
    exceptions to this rule are the load and unload methods, which may
    operate directly on named archives in the map. The contents of
    both the stack and the map can be modified by adding Archives from
    file, copying Archives or subsets of Archives from other Archives
    in memory and finally by deleting Archives from memory. At any
    stage, the top of the stack or any of the Archives in the map can
    be saved to disk.

    Commands that create copies of existing data (push, clone and
    extract) can accept the name of an Archive in the map as the first
    argument.  The append command requires the name of the Archive in
    the map that is to be appended to the top of the stack.  In all
    cases, the result of the operation affects the top of the stack.

  */
  class Interpreter : public CommandParser {
    
  public:

    enum Status { Good, Warn, Fail, Undefined };

    //! default constructor
    Interpreter ();

    //! construct from command line arguments
    Interpreter (int &argc, char** &argv);

    //! destructor
    ~Interpreter ();

    //! Evaluate a boolean expression
    bool evaluate (const std::string& expression);

    //! set the current Archive
    void set (Archive* data);

    //! get the current Archive
    Archive* get ();

    //! set the named Archive
    void setmap (const std::string& name, Archive* data);

    //! get the named Archive
    Archive* getmap (const std::string& name, bool throw_exception = true);

    //! get the status after the last command
    Status get_status () const;

    //! set to false to disable the 'ok' reply
    void set_reply (bool f) { reply = f; }

    //! get plugin information
    std::string get_report (const std::string& args);
    
    //! load an archive from disk to top of stack and optionally the map
    std::string load (const std::string& args);

    //! write an archive from either the top of the stack or the map
    std::string unload (const std::string& args);

    //! push a clone of the current stack top onto the stack
    std::string push (const std::string& args);

    //! pop the top of the stack
    std::string pop (const std::string& args);

    //! set the name of the current archive
    std::string set (const std::string& args);

    //! get the named archive
    std::string get (const std::string& args);

    //! remove the named archive from the map
    std::string remove (const std::string& args);

    //! clone the current archive
    std::string clone (const std::string& args);

    //! convert the current archive or a named archive to a different class
    std::string convert (const std::string& args);

    //! extract part of the current archive
    std::string extract (const std::string& args);

    //! edit the current archive
    std::string edit (const std::string& args);

    //! test a boolean expression
    std::string test (const std::string& args);

    //! edit the configuration parameters
    std::string config (const std::string& args);

    //! append one archive to another
    std::string append (const std::string& args);

    //! frequency scrunch an archive in the stack
    std::string fscrunch (const std::string& args);
    
    //! time scrunch an archive in the stack
    std::string tscrunch (const std::string& args);
    
    //! poln scrunch an archive in the stack
    std::string pscrunch (const std::string& args);
    
    //! form the Stokes invariant interval profile
    std::string invint (const std::string& args);

    //! bin scrunch an archive in the stack
    std::string bscrunch (const std::string& args);
    
    //! fold an archive in the stack
    std::string fold (const std::string& args);
    
    //! centre an archive in the stack
    std::string centre (const std::string& args);
    
    //! dedisperse an archive in the stack
    std::string dedisperse (const std::string& args);

    //! correct for faraday rotation
    std::string defaraday (const std::string& args);

    //! weight each profile according to the named scheme
    std::string weight (const std::string& args);

    //! scale each profile by the specified value
    std::string scale (const std::string& args);

    //! offset each profile by the specified value
    std::string offset (const std::string& args);

    //! rotate each profile by the specified value
    std::string rotate (const std::string& args);

    //! fix archive based on arguments
    std::string fix (const std::string& args);

    //! correct for parallactic angle effects
    std::string correct_instrument (const std::string& args);
    
    //! apply scattered power corrections
    std::string scattered_power_correct (const std::string& args);

    //! dump the raw data points to the screen
    std::string screen_dump (const std::string& args);

    //! toggle the clober flag
    std::string toggle_clobber (const std::string& args);

    //! Extensions to the Interpreter
    class Extension;

    //! Import the Extension
    void import (Extension*);

    //! Import a nested Extension
    void import (Extension*, 
		 const std::string& command, 
		 const std::string& help);

    //! allow dedispersion or Faraday rotation correction to infinite frequency
    bool allow_infinite_frequency;

  protected:
    
    //! the Archive stack
    std::stack< Reference::To<Archive> > theStack;

    //! the Archive map
    std::map< std::string, Reference::To<Archive> > theMap;

    //! get the interface to the current archive
    TextInterface::Parser* get_interface ();

    //! operate on data "in place", otherwise make a copy
    bool inplace;
    
    //! allow the user to overwrite existing named archive
    bool clobber;
    
    //! time various operations
    bool stopwatch;
    
    //! reply to each command
    bool reply;
   
    //! status flag (pass/fail)
    Status status;

    //! the extension with with Archives will be unloaded
    std::string unload_extension;
    
    //! the path to which Archives will be unloaded
    std::string unload_path;

    //! All methods should call this method to parse the arguments
    std::vector<std::string> setup (const std::string& args);

    //! All methods should return via the response methods
    std::string response (Status status, const std::string& text = "");

    //! Parses arguments as a single instance of T
    template<typename T> T setup (const std::string& args);

    //! Parses arguments as an optional single instance of T
    template<typename T> T setup (const std::string& args, T default_value);

    FixFluxCal fix_flux_cal;
    SetReceiver set_receiver;

  private:
    
    void init ();

    //! the last expression evaluated by evaluate method
    std::string evaluate_expression;

  };
  
}

template<typename T>
T Pulsar::Interpreter::setup (const std::string& args)
{
  std::vector<std::string> arguments = setup (args);
  if (arguments.size() != 1)
    throw Error (InvalidParam, "Pulsar::Interpreter::setup",
		 "invalid number of parameters");
  
  return fromstring<T> (arguments[0]);
}

template<typename T>
T Pulsar::Interpreter::setup (const std::string& args, T default_value)
{
  std::vector<std::string> arguments = setup (args);
  if (arguments.size() > 1)
    throw Error (InvalidParam, "Pulsar::Interpreter::setup",
		 "invalid number of parameters");
  if (arguments.size())
    return fromstring<T> (arguments[0]);
  else
    return default_value;
}

// standard shell constructor defined in More/Polarimetry/standard_shell.C
Pulsar::Interpreter* standard_shell ();

#endif
