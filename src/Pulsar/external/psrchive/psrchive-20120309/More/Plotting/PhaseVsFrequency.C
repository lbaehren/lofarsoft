/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequency::PhaseVsFrequency ()
{
  get_frame()->set_y_scale( new FrequencyScale );

  isubint = 0;
  ipol = 0;
}

TextInterface::Parser* Pulsar::PhaseVsFrequency::get_interface ()
{
  return new Interface (this);
}

std::string Pulsar::PhaseVsFrequency::get_ylabel (const Archive* data)
{
  return "Frequency (MHz)";
}

unsigned Pulsar::PhaseVsFrequency::get_nrow (const Archive* data)
{
  return data->get_nchan();
}

const Pulsar::Profile*
Pulsar::PhaseVsFrequency::get_Profile (const Archive* data, unsigned ichan)
{
  return Pulsar::get_Profile (data, isubint, ipol, ichan);
}
