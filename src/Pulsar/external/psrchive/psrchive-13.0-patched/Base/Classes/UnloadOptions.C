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
  unload = false;
  overwrite = false;
}

void Pulsar::UnloadOptions::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add ("\n" "Output options:");

  arg = menu.add (overwrite, 'm');
  arg->set_help ("modify (overwrite) the original file");

  arg = menu.add (extension, 'e', "ext");
  arg->set_help ("write files with a new extension");

  arg = menu.add (directory, 'O', "path");
  arg->set_help ("write files to a new directory");
}

void Pulsar::UnloadOptions::setup ()
{
#ifdef _DEBUG
  cerr << "Pulsar::UnloadOptions::setup" << endl;
#endif

  if (overwrite && (!extension.empty() || !directory.empty()))
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "cannot use -m option with -e and/or -O option");

  if (overwrite || !extension.empty() || !directory.empty())
    unload = true;

  else if (application->must_save())
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "please specify either the -m option"
		 " or the -e and/or -O option");
}

//! Unload the archive
void Pulsar::UnloadOptions::finish (Archive* archive)
{
  if (!unload)
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

  string newname = archive->get_filename();

  if (!extension.empty())
    newname = replace_extension( newname, extension );

  if (!directory.empty())
    newname = directory + "/" + basename (newname);

#ifdef _DEBUG
  cerr << "Pulsar::UnloadOptions::finish writing " << newname << endl;
#endif

  cout << "Unloading " << newname << " ... ";
  archive->unload (newname);
  cout << "done" << endl;
}
