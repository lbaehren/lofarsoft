/***************************************************************************
 *
 *   Copyright (C) 2010 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCylindricalPlus.h"
#include "Pulsar/Profile.h"

Pulsar::StokesCylindricalPlus::StokesCylindricalPlus(float crop) : crop(crop)
{
  // bottom (of 3 plots) 50% of viewport
  manage ("flux", &flux);
  flux.get_frame()->set_viewport (0,1, 0,.5);

  flux.set_plot_values  ("ILV");
  flux.set_plot_colours ("124");
  flux.set_plot_lines   ("111");

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  // middle (of 3 plots) 25% of viewport
  manage ("flux_cropped", &flux_cropped);
  flux_cropped.get_frame()->set_viewport (0,1, .5,.75);

  flux_cropped.set_plot_values  ("ILV");
  flux_cropped.set_plot_colours ("124");
  flux_cropped.set_plot_lines   ("111");

  flux_cropped.get_frame()->get_label_above()->set_all ("");
  flux_cropped.get_frame()->get_label_below()->set_all ("");
  flux_cropped.get_frame()->get_x_axis()->set_label("");

  // remove x scale from the middle (cropped) stokes plot
  flux_cropped.get_frame()->get_x_axis()->rem_opt('N');

  // set a shortened y-axis label so that it fits
  flux_cropped.get_frame()->get_y_axis()->set_label("Flux Den.");

  // top (of 3 plots) 25% of viewport
  manage ("pa", &orientation);
  orientation.get_frame()->set_viewport (0,1, .75,1);

  // shorten the y label
  orientation.get_frame()->get_y_axis()->set_label("P.A. (deg.)");
  // remove the x label
  orientation.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  orientation.get_frame()->get_x_axis()->rem_opt('N');
  // remove the below frame labels
  orientation.get_frame()->get_label_below()->set_all ("");
}

Pulsar::StokesCylindricalPlus::~StokesCylindricalPlus () {}

void Pulsar::StokesCylindricalPlus::set_flux_crop (const float _crop)
{
  crop = _crop;
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::StokesCylindricalPlus::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCylindricalPlus::prepare (const Archive*)
{
  flux_cropped.get_interface()->process("crop=" + tostring<float>(crop));

  prepare( &flux );
  prepare( &flux_cropped );
  prepare( &orientation );
}
