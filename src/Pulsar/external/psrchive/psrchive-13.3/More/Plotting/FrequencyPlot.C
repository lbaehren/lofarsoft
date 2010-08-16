/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FrequencyPlot.h"

Pulsar::FrequencyPlot::FrequencyPlot ()
{
  get_frame()->set_x_scale( new FrequencyScale );
}

//! Get the default label for the x axis
std::string Pulsar::FrequencyPlot::get_xlabel (const Archive*)
{
  return get_scale()->get_label();
}

//! Get the default label for the y axis
std::string Pulsar::FrequencyPlot::get_ylabel (const Archive*)
{
  return "";
}

void Pulsar::FrequencyPlot::set_yrange (float min, float max)
{
  get_frame()->get_y_scale()->set_minmax (min, max);
}

//! Get the scale
Pulsar::FrequencyScale* Pulsar::FrequencyPlot::get_scale ()
{
  FrequencyScale* scale = 0;
  scale = dynamic_cast<FrequencyScale*>( get_frame()->get_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::FrequencyPlot::get_scale",
		 "x scale is not a FrequencyScale");
  return scale;
}
