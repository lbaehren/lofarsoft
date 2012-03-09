/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsHistPlus.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsHistPlus::PhaseVsHistPlus ()
{
  // put the filename in the upper left
  get_frame()->get_label_above()->set_left ("$file");
  get_frame()->get_label_above()->set_centre ("");

  ///////////////////////////////////////////////////////////////////////

  manage ("hist", &hist);

  // freq plotter shares the x-axis and y-axis
  hist.get_frame()->set_x_scale (&xaxis);

  hist.get_frame()->set_viewport (0,1.0, 0,.7);

  // remove the alternative labeling
  hist.get_frame()->get_y_axis()->add_opt ('C');
  hist.get_frame()->get_y_axis()->set_alternate (false);

  ///////////////////////////////////////////////////////////////////////

  manage ("flux", &flux);

  // flux plotter shares the x axis
  flux.get_frame()->set_x_scale (&xaxis);

  flux.get_frame()->set_viewport (0,1.0, .7,1);

  // Default Stokes settings
  flux.set_plot_values  ("ILV");
  flux.set_plot_colours ("124");
  flux.set_plot_lines   ("111");

  // tell the flux plotter to integrate over all frequencies, subints
  flux.set_chan (Index (0, true));
  flux.set_subint (Index (0, true));
  
  // remove the x enumeration
  flux.get_frame()->get_x_axis()->rem_opt('N');
  // remove the x label
  flux.get_frame()->get_x_axis()->set_label("");
  // remove the y enumeration
  flux.get_frame()->get_y_axis()->rem_opt('N');
  // remove the y label
  flux.get_frame()->get_y_axis()->set_label("");


  frames.set_shared_x_scale (&xaxis);
  //frames.set_shared_y_scale (&yaxis);

}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::PhaseVsHistPlus::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsHistPlus::prepare (const Archive*)
{
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhaseVsHistPlus::get_scale ()
{
  return &xaxis;
}
