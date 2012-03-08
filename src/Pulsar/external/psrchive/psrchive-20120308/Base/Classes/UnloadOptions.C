/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/UnloadOptions.h"
#include "Pulsar/Archive.h"

#include "strutil.h"

using namespace std;

Pulsar::UnloadOptions::UnloadOptions ()
{
  overwrite = false;
  output_each = true;
}

//! The application produces an output for each input
void Pulsar::UnloadOptions::set_output_each (bool flag)
{
  output_each = flag;
}

//! Return the output filename for the input
string Pulsar::UnloadOptions::get_output_filename (const Archive* archive)
{
  string newname = archive->get_filename();

  if (!extension.empty())
    newname = replace_extension( newname, extension );

  if (!directory.empty())
    newname = directory + "/" + basename (newname);

  return newname;
}

void Pulsar::UnloadOptions::set_extension (const string& ext)
{
  extension = ext;
}

void Pulsar::UnloadOptions::set_directory (const string& dir)
{
  directory = dir;
}

void Pulsar::UnloadOptions::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add ("\n" "Output options:");

  if (output_each)
  {
    arg = menu.add (overwrite, 'm');
    arg->set_help ("modify (overwrite) the original file");
  }

  arg = menu.add (extension, 'e', "ext");
  arg->set_help ("write files with a new extension");

  arg = menu.add (directory, 'O', "path");
  arg->set_help ("write files to a new directory");
}

bool Pulsar::UnloadOptions::get_unload_setup () const
{
  return overwrite || !extension.empty() || !directory.empty();
}

void Pulsar::UnloadOptions::setup ()
{
#ifdef _DEBUG
  cerr << "Pulsar::UnloadOptions::setup" << endl;
#endif

  if (!output_each)
    return;

  if (overwrite && (!extension.empty() || !directory.empty()))
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "cannot use -m option with -e and/or -O option");

  if (!get_unload_setup() && application->must_save())
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "please specify either the -m option"
		 " or the -e and/or -O option");
}

//! Unload the archive
void Pulsar::UnloadOptions::finish (Archive* archive)
{
  if (!(output_each && get_unload_setup()))
    return;

  if (overwrite)
  {
#ifdef _DEBUG
    cerr << "Pulsar::UnloadOptions::finish overwriting "
	 << archive->get_filename() << endl;
#endif

    cout << "Updating " << archive->get_filename () << " ... ";
    archive->unload ();
    cout << "done" << endl;
    return;
  }

  string newname = get_output_filename (archive);

#ifdef _DEBUG
  cerr << "Pulsar::UnloadOptions::finish writing " << newname << endl;
#endif

  cout << "Unloading " << newname << " ... ";
  archive->unload (newname);
  cout << "done" << endl;
}
