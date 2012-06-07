/***************************************************************************
 *
 *   Copyright (C) 2010-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Editor.h"
#include "Pulsar/ArchiveExtension.h"

using namespace std;

void Pulsar::Editor::add_extension (Archive* archive, const std::string& ext)
{
  if (Archive::verbose > 1)
    cerr << "Pulsar::Editor::add_extension " << ext << endl;

  archive->add_extension( Archive::Extension::factory( ext ) );
}

//! Get the identifier for this instance
string Pulsar::Editor::get_identifier (const Archive* archive)
{
  return archive->get_filename();
}


