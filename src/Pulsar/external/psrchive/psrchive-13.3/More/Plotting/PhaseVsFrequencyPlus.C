/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsFrequencyPlus.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequencyPlus::PhaseVsFrequencyPlus ()
{
  // put the filename in the upper left
  get_frame()->get_label_above()->set_left ("$file");
  get_frame()->get_label_above()->set_centre ("");

  ///////////////////////////////////////////////////////////////////////

  manage ("freq", &freq);

  // freq plotter shares the x-axis and y-axis
  freq.get_frame()->set_x_scale (&xaxis);
  freq.get_frame()->set_y_scale (&yaxis);

  freq.get_frame()->set_viewport (0,.7, 0,.7);

  // remove the alternative labeling
  freq.get_frame()->get_y_axis()->add_opt ('C');
  freq.get_frame()->get_y_axis()->set_alternate (false);

  ///////////////////////////////////////////////////////////////////////

  manage ("flux", &flux);

  // flux plotter shares the x axis
  flux.get_frame()->set_x_scale (&xaxis);

  flux.get_frame()->set_viewport (0,.7, .7,1);

  // tell the flux plotter to integrate over all frequencies
  flux.set_chan (Index (0, true));
  
  // remove the x enumeration
  flux.get_frame()->get_x_axis()->rem_opt('N');
  // remove the x label
  flux.get_frame()->get_x_axis()->set_label("");
  // remove the y enumeration
  flux.get_frame()->get_y_axis()->rem_opt('N');
  // remove the y label
  flux.get_frame()->get_y_axis()->set_label("");

  ///////////////////////////////////////////////////////////////////////

  manage ("psd", &psd);
  plot_psd = true;

  // spectrum plotter shares the y axis on its x axis
  psd.get_frame()->set_x_scale (&yaxis);

  // set only the y viewport
  psd.get_frame()->get_y_scale()->set_viewport (std::pair<float,float>(.7,1));

  // remove the x enumeration
  psd.get_frame()->get_x_axis()->rem_opt('N');
  // remove the x label
  psd.get_frame()->get_x_axis()->set_label("");
  // remove the y enumeration
  //psd.get_frame()->get_y_axis()->rem_opt('N');
  // shorten the y label
  psd.get_frame()->get_y_axis()->set_label("Power");
  // remove the below frame labels
  psd.get_frame()->get_label_below()->set_all ("");

  // transpose the frame
  psd.get_frame()->set_transpose ();

  frames.set_shared_x_scale (&xaxis);
  frames.set_shared_y_scale (&yaxis);

}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::PhaseVsFrequencyPlus::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsFrequencyPlus::prepare (const Archive*)
{
  prepare (&freq);
  prepare (&flux);
  prepare (&psd);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhaseVsFrequencyPlus::get_scale ()
{
  return &xaxis;
}

//! Plot the power spectral density
void Pulsar::PhaseVsFrequencyPlus::set_plot_psd (bool _plot)
{
  if (plot_psd == _plot)
    return;

  plot_psd = _plot;
  if (plot_psd)
    manage ("psd", &psd);
  else
    unmanage (&psd);
}

//! Plot the power spectral density
bool Pulsar::PhaseVsFrequencyPlus::get_plot_psd () const
{
  return plot_psd;
}
