/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesSpherical.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::StokesSpherical::StokesSpherical ()
{
  manage ("flux", &flux);
  flux.get_frame()->set_viewport (0,1, 0,.5);

  flux.set_plot_values  ("Ip");
  flux.set_plot_colours ("12");
  flux.set_plot_lines   ("11");

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  manage ("pa", &orientation);
  orientation.get_frame()->set_viewport (0,1, .5,.75);

  // shorten the y label
  orientation.get_frame()->get_y_axis()->set_label("\\gh (deg.)");
  // remove the x label
  orientation.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  orientation.get_frame()->get_x_axis()->rem_opt('N');
  // remove the below frame labels
  orientation.get_frame()->get_label_below()->set_all ("");
  orientation.get_frame()->get_label_above()->set_all ("");

  manage ("ell", &ellipticity);
  ellipticity.get_frame()->set_viewport (0,1, .75,1);

  // shorten the y label
  ellipticity.get_frame()->get_y_axis()->set_label("\\ge (deg.)");
  // remove the x label
  ellipticity.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  ellipticity.get_frame()->get_x_axis()->rem_opt('N');
  // remove the below frame labels
  ellipticity.get_frame()->get_label_below()->set_all ("");

}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::StokesSpherical::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesSpherical::prepare (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::StokesSpherical::prepare" << endl;

  prepare( &flux, data );
  prepare( &orientation );
  prepare( &ellipticity );
}


