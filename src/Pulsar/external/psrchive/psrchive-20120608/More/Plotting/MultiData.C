/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiData.h"
#include "Pulsar/MultiDataSimple.h"

Pulsar::Plot* Pulsar::MultiData::factory (Plot* plot)
{
  SimplePlot* simple = dynamic_cast<SimplePlot*>(plot);
  if (simple)
    return new MultiDataSimple (simple);

  return plot;
}

//! Process the Archive as needed before calling plot
void Pulsar::MultiData::preprocess (Archive* archive)
{
  managed_plot->preprocess (archive);
}

//! Provide access to the plot attributes
Pulsar::PlotAttributes* Pulsar::MultiData::get_attributes ()
{
  return managed_plot->get_attributes ();
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::MultiData::get_interface ()
{
  return managed_plot->get_interface ();
}

//! Get the text interface to the frame attributes
TextInterface::Parser* Pulsar::MultiData::get_frame_interface ()
{
  return managed_plot->get_frame_interface ();
}

//! Process a configuration command and store it for later use
void Pulsar::MultiData::configure (const std::string& option)
{
  managed_plot->configure (option);
  options.push_back (option);
}

