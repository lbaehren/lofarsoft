/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhasePlot.h"

Pulsar::PhasePlot::PhasePlot ()
{
  get_frame()->set_x_scale( new PhaseScale );
}

TextInterface::Parser* Pulsar::PhasePlot::get_interface ()
{
  return new Interface (this);
}

//! Get the default label for the x axis
std::string Pulsar::PhasePlot::get_xlabel (const Archive*)
{
  return get_scale()->get_label();
}

//! Get the default label for the y axis
std::string Pulsar::PhasePlot::get_ylabel (const Archive*)
{
  return "";
}

void Pulsar::PhasePlot::set_yrange (float min, float max)
{
  get_frame()->get_y_scale()->set_minmax (min, max);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhasePlot::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( get_frame()->get_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::PhasePlot::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
