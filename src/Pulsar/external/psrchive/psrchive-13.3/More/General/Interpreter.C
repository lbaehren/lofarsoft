/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Interpreter.h"
#include "Pulsar/InterpreterExtension.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Config.h"
#include "Pulsar/Statistics.h"

#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/Dispersion.h"
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/DurationWeight.h"
#include "Pulsar/SNRWeight.h"
#include "Pulsar/Transposer.h"

#include "strutil.h"
#include "substitute.h"
#include "tostring.h"
#include "Error.h"

#include "evaluate.h"

using namespace std;

// #define _DEBUG 1

#ifdef _DEBUG
#define VERBOSE true
#else
#define VERBOSE Archive::verbose > 2
#endif

void Pulsar::Interpreter::init()
{
  clobber = true;  // names in map can be reassigned
  inplace = true;  // operations affect current top of stack

  stopwatch = false;
  reply = false;
  
  allow_infinite_frequency = false;

  prompt = "psrsh> ";
  
  add_command
    (&Interpreter::get_report,
     "report", "display plugin information",
     "no arguments required\n");

  add_command 
    ( &Interpreter::toggle_clobber,
      "clobber", "toggle overwrite permission",
      "usage: clobber \n");

  add_command 
    ( &Interpreter::load,
      "load", "load archive from file",
      "usage: load [name] <filename>\n"
      "  string filename   name of the file to be read \n"
      "  string name       load the archive to name \n" );
  
  add_command 
    ( &Interpreter::unload,
      "unload", "unload archive or set unload options",
      "usage: unload [name] [filename] \n"
      "  string filename   name of the file to be written \n"
      "  string name       unload the named archive \n" );
  
  add_command
    ( &Interpreter::push,
      "push", "push archive onto stack",
      "usage: push [name] \n"
      "  string name       name of archive to push \n" );
  
  add_command
    ( &Interpreter::pop,
      "pop", "pop current archive off top of stack",
      "usage: pop \n" );
  
  add_command
    ( &Interpreter::set,
      "set", "set name of current archive",
      "usage: set <name> \n"
      "  string name       name to give to current archive \n");
  
  add_command
    ( &Interpreter::get,
      "get", "get named archive",
      "usage: get <name> \n"
      "  string name       name of archive to become current \n");
  
  add_command
    ( &Interpreter::remove,
      "remove", "remove named archive",
      "usage: remove <name> \n"
      "  string name       name of archive to be removed \n" );
  
  add_command
    ( &Interpreter::clone,
      "clone", "duplicate an archive",
      "usage: clone [name] \n"
      "  string name       name of archive to be cloned \n" );

  add_command
    ( &Interpreter::convert,
      "convert", "convert to another archive class",
      "usage: convert <class> [name] \n"
      "  string class      name of the class to which to convert \n"
      "  string name       name of archive to be converted \n" );

  add_command
    ( &Interpreter::extract,
      "extract", "duplicate part of an archive",
      "usage: extract [name] <subints> \n"
      "  string name       name of archive from which to extract \n"
      "  unsigned subints  range[s] of subints to be extracted \n" );

  add_command
    ( &Interpreter::append,
      "append", "append data from one archive to another",
      "usage: append <name> \n"
      "  string name       name of archive to be appended \n" );
  
  add_command
    ( &Interpreter::edit, 'e',
      "edit", "edit archive parameters",
      "usage: edit <command> ...\n"
      "  string command    any edit command as understood by psredit \n" );

  add_command
    ( &Interpreter::test,
      "test", "test a boolean expression",
      "usage: test <expr> \n"
      "  string expr       a boolean expression \n"
      "The boolean expression may contain any of the recognized variable \n"
      "names preceded by a $ sign; e.g. \n"
      "\n"
      "  test $snr > 10 \n"
      "\n"
      "For a full list of variable names, type \"test help\" \n" );

  add_command
    ( &Interpreter::config,
      "config", "set a configuration parameter",
      "usage: config name = value \n"
      "\n"
      "For a full list of configuration parameter names,"
      " type \"config help\" \n" );

  add_command 
    ( &Interpreter::fscrunch, 'F',
      "fscrunch", "integrate archive in frequency",
      "usage: fscrunch [chans] \n"
      "  unsigned chans    number of desired frequency channels \n"
      "                    if not specified, fscrunch all (chans=1)\n" );
  
  add_command 
    ( &Interpreter::tscrunch, 'T',
      "tscrunch", "integrate archive in time",
      "usage: tscrunch [subints] \n"
      "  unsigned subints  number of desired sub-integrations \n"
      "                    if not specified, tscrunch all (subints=1)\n" );
  
  add_command 
    ( &Interpreter::pscrunch, 'p',
      "pscrunch", "integrate archive to produce total intensity",
      "usage: pscrunch \n" );
  
  add_command
    ( &Interpreter::bscrunch, 'B',
      "bscrunch", "integrate archive in phase bins",
      "usage: bscrunch bins \n"
      "  unsigned bins     number of desired phase bins\n" );

  add_command
    ( &Interpreter::fold,
      "fold", "fold archive profiles",
      "usage: fold factor \n"
      "  unsigned factor   folding factor \n" );

  add_command 
    ( &Interpreter::invint, 'I',
      "invint", "form the Stokes invariant interval profile",
      "usage: invint \n" );
  
  add_command 
    ( &Interpreter::centre, 'C',
      "centre", "centre profiles using polyco or max",
      "usage: centre <max> \n"
      "  max               centre on the peak in total intensity \n" );

  add_command 
    ( &Interpreter::dedisperse, 'D',
      "dedisperse", "dedisperse all profiles in an archive",
      "usage: dedisperse \n" );

  add_command 
    ( &Interpreter::defaraday, 'R',
      "defaraday", "apply faraday rotation correction",
      "usage: defaraday \n" );

  add_command 
    ( &Interpreter::weight, 'w',
      "weight", "weight each profile using the specified scheme",
      "usage: weight <time|snr> \n"
      "  time              weight each sub-integration by its duration \n"
      "  snr               weight each profile by its S/N squared\n");

  add_command 
    ( &Interpreter::scale, 's',
      "scale", "scale each profile by the specified value",
      "usage: scale <factor> \n"
      "  float factor      value by which all data will be scaled \n" );

  add_command 
    ( &Interpreter::offset, 'o',
      "offset", "offset each profile by the specified value",
      "usage: offset <summand> \n"
      "  float summand     value by which all data will be offset \n" );

  add_command 
    ( &Interpreter::rotate, 'r',
      "rotate", "rotate each profile by the specified value",
      "usage: rotate <turns> \n"
      "  double turns      phase turns by which all data will be rotated \n" );

  add_command 
    ( &Interpreter::fix, 'f',
      "fix", "fix something in the archive",
      "usage: fix <something> \n"
      "  string something  thing to be fixed \n"
      "things that can be fixed: \n"
      "  'fluxcal'         fix the Archive::Type (FluxCalOn or Off) \n"
      "  'receiver'        fix the receiver information \n" );

  add_command 
    ( &Interpreter::scattered_power_correct,
      "spc", "apply scattered power correction",
      "usage: spc \n");

}

Pulsar::Interpreter::Interpreter()
{
  init ();
}

//! construct from command line arguments
Pulsar::Interpreter::Interpreter (int &argc, char** &argv)
{
  init ();
}

//! destructor
Pulsar::Interpreter::~Interpreter ()
{

}

void Pulsar::Interpreter::import (Extension* ext)
{
  if (ext->interpreter)
    throw Error (InvalidState, "Pulsar::Interpreter::import",
		 "Extension already owned by another Interpreter");

  ext->interpreter = this;

  CommandParser::import (ext);
}

//! Import a nested Extension
void Pulsar::Interpreter::import (Extension* ext, 
				  const std::string& command, 
				  const std::string& help)
{
  if (ext->interpreter)
    throw Error (InvalidState, "Pulsar::Interpreter::import",
		 "Extension already owned by another Interpreter");

  ext->interpreter = this;

  CommandParser::import (ext, command, help);
}

string Pulsar::Interpreter::get_report (const string& args)
{
  Pulsar::Archive::agent_report ();
  return response (Good);
}

/*!
  Eventually, the Interpreter class might filter special arguments
  out of the list before passing the remainder along to the method.
*/
vector<string> Pulsar::Interpreter::setup (const string& text)
{
  status = Undefined;

  vector<string> arguments;
  separate (text, arguments);

  return arguments;
}

/*!
  Eventually, the Interpreter class might filter special arguments
  out of the list before passing the remainder along to the method.
*/
string Pulsar::Interpreter::response (Status s, const string& text)
{
  status = s;

  if (status == Fail)
    fault = true;

  if (!reply)
    return text;

  string out;

  switch (status) {
  case Good:
    out = "ok";      break;
  case Warn:
    out = "warning"; break;
  case Fail:
    out = "fail";    break;
  default:
    out = "?";       break;
  }

  if (!text.length())
    return out;

  return out + " " + get_command() + ": " + text;
}

void Pulsar::Interpreter::set (Archive* data)
{
  if (theStack.empty() || !inplace) 
  {
    if (VERBOSE)
      cerr << "Pulsar::Interpreter::set push " << data << endl;
    theStack.push (data);
  }
  else
  {
    if (VERBOSE)
      cerr << "Pulsar::Interpreter::set top " << data << endl;
    theStack.top() = data;
  }

  if (VERBOSE)
    cerr << "Pulsar::Interpreter::set stack size=" << theStack.size() << endl;
}

Pulsar::Archive* Pulsar::Interpreter::get ()
{
  if (VERBOSE)
    cerr << "Pulsar::Interpreter::get stack size=" << theStack.size() << endl;

  if (theStack.empty() || !theStack.top())
    throw Error (InvalidState, "Pulsar::Interpreter::get",
		 "no archive in stack");

  return theStack.top();
}

void Pulsar::Interpreter::setmap (const string& name, Archive* data)
{
  if (!clobber)
  {
    map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);
    if (entry != theMap.end())
      throw Error (InvalidState, "Pulsar::Interpreter::set",
		   "archive already exists with name '" + name + "'");
  }
  theMap[name] = data;
}

Pulsar::Archive* Pulsar::Interpreter::getmap (const string& name, bool ex)
{
  map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);

  if (entry == theMap.end())
  {
    // not found
    if (!ex) 
      return 0;
    throw Error (InvalidState, "Pulsar::Interpreter::get",
		 "no archive named '" + name + "'");
  }
  return entry->second;
}

string Pulsar::Interpreter::load (const string& args) try
{
  vector<string> arguments = setup (args);
  
  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, help("load"));

  if (arguments.size() == 2)
    setmap( arguments[0], Archive::load(arguments[1]) );
  else
    set( Archive::load(arguments[0]) );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::Interpreter::unload (const string& args) try
{
  vector<string> arguments = setup (args);
  
  if (arguments.size() > 2)
    return response (Fail, "invalid number of arguments");

  string mapname;
  string filename;

  if (arguments.size() == 2) {
    mapname = arguments[0];
    filename = arguments[1];
  }
  else if (arguments.size() == 1)
    filename = arguments[0];
  else
    filename = get()->get_filename();

  if ( filename.substr(0,4) == "ext=" )
    filename = replace_extension( get()->get_filename(), filename.substr(4) );
  
  if ( mapname.length() )
    getmap( mapname )->unload( filename );
  else
    get()->unload( filename );

  return response (Good, "data written to " + filename);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

//! push a clone of the current stack top onto the stack
string Pulsar::Interpreter::push (const string& args) try
{
  vector<string> arguments = setup (args);

  if (arguments.size() > 1)
    return response (Fail, "please specify only one name");

  if (arguments.size())
    theStack.push ( getmap(arguments[0]) );
  else
    theStack.push( get()->clone() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


//! pop the top of the stack
string Pulsar::Interpreter::pop (const string& args)
{
  if (theStack.empty())
    return response (Warn, "currently at bottom");

  theStack.pop();
  return response (Good);
}

string Pulsar::Interpreter::set (const string& args) try
{
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "please specify one name");

  setmap( arguments[0], get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::Interpreter::get (const string& args) try
{
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "please specify one name");

  set( getmap(arguments[0]) );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


string Pulsar::Interpreter::remove (const string& args)
{
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "please specify one name");

  string name = arguments[0];
  map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);

  if (entry == theMap.end())
    return response (Fail, "no archive named " + name);

  theMap.erase (entry);

  return response (Good);
}


string Pulsar::Interpreter::clone (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (arguments.size() > 1)
    return response (Fail, "please specify only one name");

  if (arguments.size() == 1)
    set( getmap(arguments[0])->clone() );
  else
    set( get()->clone() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// convert the archive to a new type
string Pulsar::Interpreter::convert (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify the target archive class name");

  Reference::To<Archive> input;

  if (arguments.size() > 1)
    input = getmap(arguments[1]);
  else
    input = get();

  Reference::To<Archive> output = Archive::new_Archive (arguments[0]);
  output-> copy (*input);

  set( output );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


string Pulsar::Interpreter::extract (const string& args) try
{
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, help("extract"));

  /*
    if the first argument is a valid map name, then start with this archive;
    otherwise, start with the top of the stack 
  */

  Reference::To<Archive> archive = getmap (arguments[0], false);

  unsigned range = 0;

  if (archive)
    range ++;
  else
    archive = get();

  vector<unsigned> indeces;

  /*
    parse the (remaining) options as though they were indeces
  */
  for (unsigned i = range; i < arguments.size(); i++)
    TextInterface::parse_indeces (indeces, arguments[i], 
				  archive->get_nsubint());

  /*
    extract the sub-integrations
  */
  set( archive -> extract(indeces) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

TextInterface::Parser* Pulsar::Interpreter::get_interface ()
{
  if (VERBOSE)
    cerr << "Pulsar::Interpreter::get_interface" << endl;

  return standard_interface( get() );
}

string Pulsar::Interpreter::edit (const string& args) try
{ 

  // replace variable names with values
  if (args == "help")
  {
    return get_interface()->help (true);
  }

  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify at least one editor command");

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++) {
    if (icmd)
      retval += " ";
    retval += get_interface()->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

bool Pulsar::Interpreter::evaluate (const std::string& expression)
{
  evaluate_expression = substitute (expression, get_interface());

  if (VERBOSE)
    cerr << "Pulsar::Interpreter::evaluate expression="
            "'" << evaluate_expression << "'" << endl;

  double value = compute (evaluate_expression);

  if (value == 1)
    return true;

  if (value != 0)
    throw Error (InvalidParam, "evaluateBoolean",
		 "non-boolean result=" + tostring(value));

  return false;
}

string Pulsar::Interpreter::test (const string& args) try
{ 
  // replace variable names with values
  if (args == "help")
    return get_interface()->help (true);

  if (evaluate (args))
    return response (Good);
  else
    return response (Fail,
		     "assertion '"+args+"'\n"
		     "        = '"+evaluate_expression+"' failed");
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


string Pulsar::Interpreter::config (const string& args) try
{ 

  // replace variable names with values
  if (args == "help")
    return Config::get_interface()->help (true);

  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify at least one parameter name");

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++) {
    if (icmd)
      retval += " ";
    retval += Config::get_interface()->process (arguments[icmd]);
  }

  return retval;

}
catch (Error& error) {
  return response (Fail, error.get_message());
}


string Pulsar::Interpreter::append (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "please specify one name");

  get()->append( getmap(arguments[0]) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


// //////////////////////////////////////////////////////////////////////
//
// fscrunch <string> <int>
//
string Pulsar::Interpreter::fscrunch (const string& args) try
{
  bool scrunch_by = false;
  string temp = args;

  if (args[0] == 'x')
  {
    scrunch_by = true;
    temp.erase (0,1);
  }

  unsigned scrunch = setup<unsigned> (temp, 0);
  
  if (!scrunch)
    get() -> fscrunch();
  else if (scrunch_by)
    get() -> fscrunch (scrunch);
  else
    get() -> fscrunch_to_nchan (scrunch);
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}



// //////////////////////////////////////////////////////////////////////
//
// tscrunch <string> <int>
//
string Pulsar::Interpreter::tscrunch (const string& args) try
{
  bool scrunch_by = false;
  string temp = args;

  if (args[0] == 'x')
  {
    scrunch_by = true;
    temp.erase (0,1);
  }

  unsigned scrunch = setup<unsigned> (temp, 0);
  
  if (!scrunch)
    get() -> tscrunch();
  else if (scrunch_by)
    get() -> tscrunch (scrunch);
  else
    get() -> tscrunch_to_nsub (scrunch);
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


// //////////////////////////////////////////////////////////////////////
//
// pscrunch
//
string Pulsar::Interpreter::pscrunch (const string& args) try
{
  if (args.length())
    return response (Fail, "accepts no arguments");

  get() -> pscrunch();

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::Interpreter::bscrunch (const string& args) try 
{
  bool scrunch_by = false;
  string temp = args;

  if (args[0] == 'x')
  {
    scrunch_by = true;
    temp.erase (0,1);
  }

  unsigned scrunch = setup<unsigned> (temp, 0);
  
  if (!scrunch)
    return response (Fail, "invalid bscrunch argument");

  if (scrunch_by)
    get() -> bscrunch (scrunch);
  else
    get() -> bscrunch_to_nbin (scrunch);

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


string Pulsar::Interpreter::fold (const string& args) try
{
  unsigned factor = setup<unsigned> (args);
  
  if (!factor)
    return response (Fail, "invalid fold factor");

  get() -> fold (factor);
  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// invint
//
string Pulsar::Interpreter::invint (const string& args) try
{
  if (args.length())
    return response (Fail, "accepts no arguments");

  get() -> invint();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::Interpreter::centre (const string& args) try
{
  vector<string> arguments = setup (args);

  if (arguments.size() == 1 && arguments[0] == "max")
    get()->centre_max_bin();
  else if (arguments.size() == 0)
    get()->centre();
  else
    return response (Fail, "unrecognized argument '" + args + "'");

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// dedisp <string>
//

string Pulsar::Interpreter::dedisperse (const string& args) try
{
  if (!args.length())
  {
    get()->dedisperse();
    return response (Good);
  }

  double frequency = setup<double> (args);

  Pulsar::Dispersion xform;

  if (frequency)
    xform.set_reference_frequency( get()->get_centre_frequency() );
  else
  {
    if (!allow_infinite_frequency)
      return response (Fail, "sorry, infinite frequency is not allowed");
    xform.set_reference_wavelength( 0 );
  }

  xform.set_measure( get()->get_dispersion_measure() );
  xform.execute( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::defaraday (const string& args) try
{
  if (!args.length())
  {
    get()->defaraday();
    return response (Good);
  }

  double frequency = setup<double> (args);

  FaradayRotation xform;

  if (frequency)
    xform.set_reference_frequency( get()->get_centre_frequency() );
  else
  {
    if (!allow_infinite_frequency)
      return response (Fail, "sorry, infinite frequency is not allowed");
    xform.set_reference_wavelength( 0 );
  }

  xform.set_measure( get()->get_rotation_measure() );
  xform.execute( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::scattered_power_correct (const string& args) try
{
  if (args.length())
    return response (Fail, "accepts no arguments");

  Archive* arch = get();
  if (arch->get_state() == Signal::Stokes)
    arch->convert_state(Signal::Coherence);
  
  Pulsar::ScatteredPowerCorrection spc;
  spc.correct (arch);

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::weight (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify weighting scheme");

  Reference::To<Weight> weight;
  if (arguments.size() == 1 && arguments[0] == "time")
    weight = new DurationWeight;

  else if (arguments.size() == 1 && arguments[0] == "snr")
    weight = new SNRWeight;

  else
    return response (Fail, "unrecognized weighting scheme '" + args + "'");

  (*weight)( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::scale (const string& args) try
{
  float factor = setup<float> (args);
  
  Archive* archive = get();

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();

  for (unsigned isub=0; isub < nsub; isub++)
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
	archive->get_Profile (isub, ipol, ichan)->scale(factor);

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::offset (const string& args) try
{
  float summand = setup<float> (args);
  
  Archive* archive = get();

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();

  for (unsigned isub=0; isub < nsub; isub++)
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
	archive->get_Profile (isub, ipol, ichan)->offset(summand);

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::rotate (const string& args) try
{
  get()->rotate_phase( setup<double>(args) );
  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::Interpreter::fix (const string& args) try
{
  string what = setup<string>(args);

  if (what == "fluxcal")
  {
    fix_flux_cal.apply (get());
    return response (Good, fix_flux_cal.get_changes());
  }

  if (what == "receiver" || what == "rcvr")
  {
    set_receiver.apply (get());
    return response (Good);
  }

  return response (Fail, "unrecognized fix '"+args+"'");
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}


// //////////////////////////////////////////////////////////////////////
//
// clobber
//

string Pulsar::Interpreter::toggle_clobber (const string& args)
{
  clobber = !clobber;
  
  if (!reply)
    return "";

  if (clobber)
    return "will clobber named archives";
  else
    return "will not clobber named archives";
}

// //////////////////////////////////////////////////////////////////////
//
// screendump <string>
//
string Pulsar::Interpreter::screen_dump (const string& args) try
{
  if (args.length())
    set( getmap (args) );

  Reference::To<Pulsar::Archive> copy = get() -> clone();
  copy -> tscrunch();
  copy -> fscrunch();

  Transposer transposer (copy);
  transposer.set_dim (0, Signal::Polarization);
  transposer.set_dim (1, Signal::Phase);
  transposer.set_dim (2, Signal::Frequency);

  vector<float> data;

  transposer.get_amps (data);
	
  for (unsigned i = 0; i < (copy->get_nchan())*(copy->get_nbin()); i+=4) {
	  
    if (copy -> get_npol() == 4) {
      cerr << data[i] << "\t" << data[i+1] << "\t" 
	   << data[i+2]  << "\t" << data[i+3] << endl;
    }
    
    if (copy -> get_npol() == 2) {
      cerr << data[i] << "\t" << data[i+1] << endl;
    }
    
    if (copy -> get_npol() == 1) {
      cerr << data[i] << endl;
    }
    
  }

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, "screenDump: " + error.get_message());
}

