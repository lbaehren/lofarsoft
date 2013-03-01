/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Editor.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Check.h"
#include "Pulsar/Profile.h"

#include "TextInterface.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>

using namespace std;
using Pulsar::Editor;

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

  void set_quiet ()
  {
    Application::set_quiet();
    editor.output_identifier = false;
  }

  void detailed_help ();

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! The editor
  Pulsar::Editor editor;
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
  update_history = true;

  version = "$Id: psredit.C,v 1.34 2010/10/06 10:41:49 straten Exp $";

  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);

  // disable sanity checks that try to load Integration data
  Pulsar::Archive::Check::disable ("Dedispersed");
  Pulsar::Archive::Check::disable ("DeFaradayed");

  add( new Pulsar::UnloadOptions );
}

void psredit::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (&editor, &Editor::add_commands, 'c', "command[s]");
  arg->set_help ("one or more commands, separated by commas");

  arg = menu.add (&editor, &Editor::add_extensions, 'a', "extension[s]");
  arg->set_help ("one or more extensions to be added, separated by commas");

  arg = menu.add (this, &psredit::detailed_help, 'H');
  arg->set_help ("more detailed help");

  arg = menu.add (editor.prefix_name, 'Q');
  arg->set_help ("do not prefix output with 'keyword='");

  arg = menu.add (editor.delimiter, 'd', "delim");
  arg->set_help ("separate elements of a container using delimiter");

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
  if (editor.will_modify())
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
  cout << editor.process (archive) << endl;
}
