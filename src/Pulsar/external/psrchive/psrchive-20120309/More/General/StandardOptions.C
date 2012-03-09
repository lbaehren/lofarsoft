/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/StandardOptions.h"
#include "Pulsar/Interpreter.h"

#include "strutil.h"
#include "separate.h"

Pulsar::StandardOptions::StandardOptions ()
{
}

void Pulsar::StandardOptions::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add ("\n" "Preprocessing options:");

  arg = menu.add (this, &StandardOptions::add_job, 'j', "commands");
  arg->set_help ("execute pulsar shell preprocessing commands");

  arg = menu.add (this, &StandardOptions::add_script, 'J', "script");
  arg->set_help ("execute pulsar shell preprocessing script");
}

void Pulsar::StandardOptions::add_job (const std::string& arg)
{
  separate (arg, jobs, ",");
}

void Pulsar::StandardOptions::add_script (const std::string& arg)
{
  loadlines (arg, jobs);
}

void Pulsar::StandardOptions::add_default_job (const std::string& job)
{
  default_jobs.push_back (job);
}

//! Preprocessing tasks implemented by partially derived classes
void Pulsar::StandardOptions::process (Archive* archive)
{
  if (jobs.size() == 0)
    jobs = default_jobs;

  if (jobs.size() == 0)
    return;

  if (!interpreter)
    interpreter = standard_shell();

  if (application->get_verbose())
    cerr << application->get_name() << ": interpreter processing "
	 << archive->get_filename() << endl;

  interpreter->set (archive);
  interpreter->script (jobs);
}

//! Provide access to the interpreter
Pulsar::Interpreter* Pulsar::StandardOptions::get_interpreter ()
{
  if (!interpreter)
    interpreter = standard_shell();

  return interpreter;
}
