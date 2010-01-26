/***************************************************************************
 *
 *   Copyright (C) 2002-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/psrchive.h"
#include "Pulsar/Interpreter.h"

#include "Pulsar/Application.h"
#include "Pulsar/UnloadOptions.h"

#include "strutil.h"

using namespace std;

//
//! An application to interactively trash files
//
class psrsh : public Pulsar::Application
{
public:

  //! Default constructor
  psrsh ();

  //! Saving is options
  bool must_save () { return false; }

  //! The main loop
  void run ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  Reference::To<Pulsar::Interpreter> interpreter;
  string script;

  //! Load data from filenames provided as arguments
  bool load_files;

  void interpreter_help (const string& cmd);
};

int main (int argc, char** argv)
{
  psrsh program;
  return program.main (argc, argv);
}

psrsh::psrsh ()
  : Application ("psrsh", "command language interpreter")
{
  has_manual = true;
  version = "$Id: psrsh.C,v 1.19 2009/10/18 01:32:57 straten Exp $";

  load_files = true;

  add( new Pulsar::UnloadOptions );

  interpreter = standard_shell();
}

void psrsh::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (this, &psrsh::interpreter_help, 'H');
  arg->set_long_name ("cmd");
  arg->set_help ("list available commands");
  arg->set_has_arg (optional_argument);

  arg = menu.add (load_files, 'n');
  arg->set_long_name ("nofiles");
  arg->set_help ("no filenames required");

  menu.set_help_footer
    ("\n"
     "If no files are specified, psrsh enters the interactive shell mode \n"
     "\n"
     "Otherwise, psrsh enters the shell script command processor mode: \n"
     "the first file must be the script, and all subseqent archive files \n"
     "will be processed using this script \n"
     + menu.get_help_footer ());
}

void psrsh::run ()
{
  if (!filenames.empty())
  {
    script = filenames[0];
    filenames.erase (filenames.begin());
    name = basename (script);

    if (load_files)
      Application::run ();
    else
      interpreter->script( script );
  }
  else
  {
    // no arguments: interactive mode
    interpreter->initialize_readline ("psrsh");
    interpreter->set_reply( true );
    
    while (!interpreter->quit)
      cout << interpreter->parse( interpreter->readline() );
  }
}

void psrsh::process (Pulsar::Archive* archive)
{
  interpreter->set( archive );
  interpreter->script( script );
}

void psrsh::interpreter_help (const string& cmd)
{
  // cerr << "psrsh::interpreter_help cmd='" << cmd << "'" << endl;

  cout << endl << interpreter->help (cmd);

  exit (0);
}
