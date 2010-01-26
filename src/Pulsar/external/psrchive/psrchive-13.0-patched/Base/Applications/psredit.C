/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Check.h"
#include "Pulsar/Profile.h"

#include "TextInterface.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>

using namespace std;

//! Pulsar Archive Editing application
class psredit: public Pulsar::Application
{
public:

  //! Default constructor
  psredit ();

  //! Return true if an edit command is used
  bool must_save ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  void add_commands (const std::string& str) { separate (str, commands, ","); }

  void set_quiet () { Application::set_quiet(); output_filename = false; }

  void detailed_help ();

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! commands to be executed
  vector<string> commands;

  //! prefix parameter value queries with parameter name=
  bool prefix_name;

  // print the name of each file processed
  bool output_filename;
};

int main (int argc, char** argv)
{
  psredit program;
  return program.main (argc, argv);
}

psredit::psredit ()
  : Pulsar::Application ("psredit", "query and/or set archive attributes")
{
  has_manual = true;
  version = "$Id: psredit.C,v 1.33 2009/09/02 02:54:31 straten Exp $";

  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);

  // disable sanity checks that try to load Integration data
  Pulsar::Archive::Check::disable ("Dedispersed");
  Pulsar::Archive::Check::disable ("DeFaradayed");

  prefix_name = true;
  output_filename = true;

  add( new Pulsar::UnloadOptions );
}

void psredit::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psredit::add_commands, 'c', "command[s]");
  arg->set_help ("one or more commands, separated by commas");

  arg = menu.add (this, &psredit::detailed_help, 'H');
  arg->set_help ("more detailed help");

  arg = menu.add (prefix_name, 'Q');
  arg->set_help ("do not prefix output with 'keyword='");
}

const char* long_help = 
  "\n"
  "Detailed Help: \n"
  "\n"
  "    A command can either get or a set a parameter value.\n"
  "    A get command is simply the name of the parameter.\n"
  "    A set command is a parameter assignment statement, e.g\n"
  "\n"
  "    psredit -c freq,name=\"Hydra A\"\n"
  "\n"
  "    will print the centre frequency and set the source name.\n"
  "    Note that parameter names are case sensitive.\n"
  "\n"
  "    Multiple commands may also be specified by using -c multiple times. \n"
  "\n"
  "    For the complete list of all available parameters in an archive, \n"
  "    run \"psredit <filename>\" without any command line options. \n"
  "\n"
  "    Parameter names followed by a '*' represent vectors; these may be \n"
  "    further specified using range notation.  For example:\n"
  "\n"
  "    psredit -c int[0,8-15]:mjd\n"
  "\n"
  "    will print the epoch from integrations 0 and 8 through 15 inclusive.\n"
  "    Output values will be separated by commas.  If a range is not \n"
  "    specified, all values will be output. \n";

void psredit::detailed_help ()
{
  cerr << long_help << endl;
  exit (0);
}

//
//
//
bool psredit::must_save ()
{
  for (unsigned j = 0; j < commands.size(); j++)
    if (commands[j].find('=') != string::npos)
      return true;

  // load files quickly (no data)
  Pulsar::Profile::no_amps = true;

  return false;
}

//
//
//
void psredit::process (Pulsar::Archive* archive)
{
  Reference::To<TextInterface::Parser> interface = archive->get_interface();

  if (commands.size() == 0)
  {
    cout << interface->help (true) << endl;;
    return;
  }

  // so that a space precedes each parameter processed
  interface->set_indentation (" ");
  interface->set_prefix_name (prefix_name);

  if (output_filename)
    cout << archive->get_filename();

  for (unsigned j = 0; j < commands.size(); j++)
  {
    if (verbose)
      cerr << "psredit: processing command '" << commands[j] << "'" << endl;

    cout << interface->process (commands[j]);
  }

  cout << endl;
}
