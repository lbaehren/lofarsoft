/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/CalSource.h"

#include "templates.h"
#include "strutil.h"

using namespace std;

static string get_default (string which)
{
  string result = Pulsar::Config::get_runtime() + "/fluxcal." + which;

  DEBUG("Pulsar::CalSource::get_default=" << result);

  return result;
}

Pulsar::Option<string> Pulsar::CalSource::default_on
( 
 "fluxcal::on", get_default("on"),

 "Standard candle on-source aliases filename",
 
 "The name of the file from which commonly used source name aliases for\n"
 "on-source flux calibrator observation are read."
);

Pulsar::Option<string> Pulsar::CalSource::default_off
(
 "fluxcal::off", get_default("off"),
 
 "Standard candle off-source aliases filename",
 
 "The name of the file from which commonly used source name aliases for\n"
 "off-source flux calibrator observation are read."
);


Pulsar::CalSource::CalSource () 
{
  loaded = false;
}

void Pulsar::CalSource::load () const
{
  if (loaded)
    return;

  loaded = true;

  on_filename = default_on;
  DEBUG("Pulsar::CalSource on_filename=" << on_filename);
  stringfload (&fluxcal_on, on_filename);

  off_filename = default_off;
  DEBUG("Pulsar::CalSource off_filename=" << off_filename);
  stringfload (&fluxcal_off, off_filename);
}

void Pulsar::CalSource::apply (Archive* archive)
{
  if (!loaded)
    load ();

  if (Archive::verbose == 3)
    cerr << "Pulsar::CalSource::apply" << endl;

  if (archive->get_type() != Signal::PolnCal )
    return;

  if (found(archive->get_source(), fluxcal_on))
  {
    archive->set_type ( Signal::FluxCalOn );
    if (Archive::verbose == 3)
      cerr << "Pulsar::CalSource::apply source " << archive->get_source()
	   << " is Signal::FluxCalOn" << endl;
    return;
  }

  if (found(archive->get_source(), fluxcal_off))
  {
    archive->set_type ( Signal::FluxCalOff );
    if (Archive::verbose == 3)
      cerr << "Pulsar::CalSource::apply source " << archive->get_source()
	   << " is Signal::FluxCalOff" << endl;
    return;
  }

  if (Archive::verbose == 3)
    cerr << "Pulsar::CalSource::apply source " << archive->get_source()
      << " not found" << endl;
}


//! Get the number of FluxCalOn aliases
unsigned Pulsar::CalSource::get_on_size () const
{
  if (!loaded)
    load ();
  return fluxcal_on.size();
}

//! Get the number of FluxCalOff aliases
unsigned Pulsar::CalSource::get_off_size () const
{
  if (!loaded)
    load ();
  return fluxcal_off.size();
}

//! Get the filename from which FluxCalOn aliases were read
std::string Pulsar::CalSource::get_on_filename () const
{
  if (!loaded)
    load ();
  return on_filename;
}

//! Get the filename from which FluxCalOff aliases were read
std::string Pulsar::CalSource::get_off_filename () const
{
  if (!loaded)
    load ();
  return off_filename;
}
