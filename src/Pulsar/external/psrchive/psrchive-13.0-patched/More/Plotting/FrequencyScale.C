/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

Pulsar::FrequencyScale::FrequencyScale ()
{
  reverse = false;
}

void Pulsar::FrequencyScale::init (const Archive* data)
{
  double freq = data->get_centre_frequency();
  double bw = data->get_bandwidth();

  if (reverse)
    set_world (std::pair<float,float>(freq+0.5*bw, freq-0.5*bw));

  set_minmax (freq - 0.5*bw, freq + 0.5*bw);
}

void Pulsar::FrequencyScale::get_indeces (const Archive* data, 
					  unsigned& min, unsigned& max) const
{
  PlotScale::get_indeces (data->get_nchan(), min, max);
}

std::string Pulsar::FrequencyScale::get_label ()
{
  return "Frequency (MHz)";
}

void Pulsar::FrequencyScale::get_ordinates (const Archive* data,
					    std::vector<float>& x_axis) const
{
  x_axis.resize (data->get_nchan());

  double freq = data->get_centre_frequency();
  double bw = data->get_bandwidth();
  double min_freq = freq - 0.5 * bw;
  for (unsigned ibin = 0; ibin < x_axis.size(); ibin++)
    x_axis[ibin] = min_freq + bw * (double(ibin) + 0.5) / x_axis.size();
}

Pulsar::FrequencyScale::Interface::Interface (FrequencyScale* instance)

{
  if (instance)
    set_instance (instance);

  import( PlotScale::Interface() );

  add( &FrequencyScale::get_reverse,
       &FrequencyScale::set_reverse,
       "reverse", "Reverse order" );  
}
