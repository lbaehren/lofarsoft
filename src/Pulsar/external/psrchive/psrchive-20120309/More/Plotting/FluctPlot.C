/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluctPlot.h"
#include "Pulsar/Profile.h"

Pulsar::FluctPlot::FluctPlot ()
{
  isubint = ichan = ipol = 0;
  get_frame()->get_y_scale()->set_buf_norm(0.05);
}

TextInterface::Parser* Pulsar::FluctPlot::get_interface ()
{
  return new Interface (this);
}

/*! The ProfileVectorPlotter class computes the minimum and maximum values
  to be plotted */
void Pulsar::FluctPlot::prepare (const Archive* data)
{
  plotter.profiles.clear();

  // derived classes fill the plotter.profiles attribute
  get_profiles (data);

  if (!plotter.profiles.size())
    return;

  unsigned nbin = plotter.profiles[0]->get_nbin();

  // set the x-axis to run from 0 to nbin
  get_frame()->get_x_scale()->set_minmax (0, nbin);

  plotter.ordinates.resize (nbin);
  for (unsigned i=0; i<nbin; i++)
    plotter.ordinates[i] = i;

  plotter.minmax (get_frame());
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluctPlot::draw (const Archive* data)
{
  std::pair<float,float> range = get_frame()->get_x_scale()->get_range_norm();
  plotter.draw ( range.first, range.second );
}

//! Return the label for the y-axis
std::string Pulsar::FluctPlot::get_ylabel (const Archive* data)
{
  return "Fluctuation Power";
}

//! Return the label for the x-axis
std::string Pulsar::FluctPlot::get_xlabel (const Archive* data)
{
  return "Pulse Frequency";
}

