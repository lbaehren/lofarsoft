/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/CommonOptions.h"
#include "Pulsar/Archive.h"

Pulsar::CommonOptions::CommonOptions ()
{
  tscrunch = false;
  fscrunch = false;
  pscrunch = false;
}

//! Extra usage information implemented by derived classes
std::string Pulsar::CommonOptions::get_usage ()
{
  return 
    " -T               integrate all sub-integrations \n"
    " -F               integrate all frequency channels \n"
    " -p               integrate both polarizations \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::CommonOptions::get_options ()
{
  return "FTp";
}

//! Parse a non-standard command
bool Pulsar::CommonOptions::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'T':
    tscrunch = true;
    break;

  case 'F':
    fscrunch = true;
    break;
    
  case 'p':
    pscrunch = true;
    break;

  default:
    return false;
  }

  return true;
}

//! Preprocessing tasks implemented by partially derived classes
void Pulsar::CommonOptions::process (Archive* archive)
{
  if (tscrunch)
  {
    if (application->get_verbose())
      cerr << application->get_name() << ": tscrunch" << endl;
    archive->tscrunch ();
  }

  if (fscrunch)
  {
    if (application->get_verbose())
      cerr << application->get_name() << ": fscrunch" << endl;
    archive->fscrunch ();
  }

  if (pscrunch)
  {
    if (application->get_verbose())
      cerr << application->get_name() << ": pscrunch" << endl;
    archive->pscrunch ();
  }
}
