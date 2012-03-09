/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Plot.h"
#include "Pulsar/Archive.h"

bool Pulsar::Plot::verbose = false;

//! Get the constructor
Pulsar::Plot::Constructor* Pulsar::Plot::get_constructor ()
{
  if (!constructor)
    throw Error (InvalidState, "Pulsar::Plot::get_constructor",
		 "dynamic Plot constructor not set");

  return constructor;
}

void Pulsar::Plot::configure (const std::string& option)
{
  try {
    tui = get_interface();
    tui->process (option);
  }
  catch (Error& error) {
    try {
      fui = get_frame_interface();
      fui->process (option);
    }
    catch (Error& error) {
      throw error += "Pulsar::Plot::configure";
    }
  }
}

/* By default, most plots will need the profile baseline to be removed */
void Pulsar::Plot::preprocess (Archive* archive)
{
  archive->remove_baseline();
}
