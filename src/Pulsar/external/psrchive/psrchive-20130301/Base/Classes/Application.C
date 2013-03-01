/***************************************************************************
 *
 *   Copyright (C) 2008-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/psrchive.h"

#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>

using namespace std;

Pulsar::Application::Application (const string& n, const string& d)
{
  name = n;
  description = d;

  has_manual = false;
  update_history = false;

  stow_script = false;

  verbose = false;
  very_verbose = false;

  sort_filenames = false;
}

//! Add options to the application
void Pulsar::Application::add (Options* f)
{
  f->application = this;
  options.push_back( f );
}

//! Get the application name
std::string Pulsar::Application::get_name () const
{
  return name;
}

//! Get the application description
std::string Pulsar::Application::get_description () const
{
  return description;
}

//! Get the verbosity flag
bool Pulsar::Application::get_verbose () const
{
  return verbose;
}

void Pulsar::Application::set_quiet ()
{
  Archive::set_verbosity (0);
}

void Pulsar::Application::set_verbose ()
{
  verbose = true;
}

void Pulsar::Application::set_very_verbose ()
{
  Archive::set_verbosity (3);
  very_verbose = verbose = true;
}

//! Parse the command line options
void Pulsar::Application::parse (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.set_help_header
    ("\n" + name + " - " + description + "\n"
     "\n"
     "usage: " + name + " [options] filename[s] \n"
     "\n"
     "where options are:");

  if (has_manual) menu.set_help_footer
    ("\n" "See " PSRCHIVE_HTTP "/manuals/" + name + " for more details \n");

  menu.set_version (version);

  arg = menu.add (this, &Application::set_quiet, 'q');
  arg->set_help ("quiet mode");

  arg = menu.add (this, &Application::set_verbose, 'v');
  arg->set_help ("verbose mode");

  arg = menu.add (this, &Application::set_very_verbose, 'V');
  arg->set_help ("very verbose mode");

  arg = menu.add (metafile, 'M', "metafile");
  arg->set_help ("metafile contains list of archive filenames");

  arg = menu.add (Config::get_configuration(), &Config::set_filename,
		  "config", "file");
  arg->set_help ("configuration file");

  for (unsigned i=0; i<options.size(); i++)
    options[i]->add_options (menu);

  add_options (menu);

  menu.parse (argc, argv);

  dirglob_program = name;

  if ( stow_script && optind < argc )
  {
    script = argv[optind];
    optind ++;
  }

  if (!metafile.empty())
    stringfload (&filenames, metafile);
  else
  {
    for (int i=optind; i<argc; i++)
      dirglob (&filenames, argv[i]);

    if (sort_filenames)
      sort (filenames.begin(), filenames.end());
  }

  if (update_history)
  {
    string separator = " ";

    command += name + separator;

    for (int i=1; i<optind; i++)
      command += argv[i] + separator;

    if (command.length () > 80)
    {
      cerr << "WARNING: ProcHistory command string truncated to 80 characters"
	   << endl;
      command = command.substr (0, 80);
    }
  }
}

Pulsar::Archive * Pulsar::Application::load (const string& filename)
{
  Reference::To<Archive> archive;
  archive = Archive::load (filename);

  for (unsigned i=0; i<options.size(); i++)
  {
    if (very_verbose)
      cerr << "Pulsar::Application::main feature "<< i <<" process" << endl;
    options[i]->process (archive);

    if (options[i]->result())
      archive = options[i]->result();
  }
  return archive.release();
}

void Pulsar::Application::run ()
{
  if (filenames.empty())
    throw Error (InvalidParam, name,
		 "please specify filename[s]");

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    Reference::To<Archive> archive = load (filenames[ifile]);

    process (archive);

    if (result())
      archive = result();

    if (update_history)
    {
      ProcHistory * fitsext = archive->get<ProcHistory> ();
      if (fitsext)
	fitsext->set_command_str (command);
    }

    for (unsigned i=0; i<options.size(); i++)
    {
      if (very_verbose)
	cerr << "Pulsar::Application::main feature "<< i <<" finish" << endl;
      options[i]->finish (archive);
    }

  }
  catch (Error& error)
  {
    cerr << name << ": error while processing " << filenames[ifile] << ":";
    if (verbose)
      cerr << error << endl;
    else
      cerr << "\n" << error.get_message() << endl;
  }
}

//! Execute the main loop
int Pulsar::Application::main (int argc, char** argv) try
{
  parse (argc, argv);

  for (unsigned i=0; i<options.size(); i++)
    options[i]->setup ();

  setup ();

  run ();

  finalize ();

  for (unsigned i=0; i<options.size(); i++)
    options[i]->finalize ();

  return 0;
}
catch (Error& error)
{
  cerr << name << ": " << error.get_message() << endl;
  return -1;
}

//! Extra setup, run once before main loop
void Pulsar::Application::setup ()
{
}

//! Return true if application must save data
bool Pulsar::Application::must_save ()
{
  return true;
}

//! Final steps, run once at end of program
void Pulsar::Application::finalize ()
{
}

/*! Optional setup steps, run once at start (before main finalize) */
void Pulsar::Application::Options::setup ()
{
}

/*! Optional processing tasks, run once per Archive (before main process) */
void Pulsar::Application::Options::process (Archive*)
{
}

/*! Optional finishing tasks, run once per Archive (after main process) */
void Pulsar::Application::Options::finish (Archive*)
{
}

/*! Optional final steps, run once at end (after main finalize) */
void Pulsar::Application::Options::finalize ()
{
}

