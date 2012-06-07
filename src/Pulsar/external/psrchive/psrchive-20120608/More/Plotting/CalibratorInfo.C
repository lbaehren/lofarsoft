/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorInfo.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::CalibratorInfo::CalibratorInfo ()
{
  // reserve 5% of the viewport height for space between panels
  between_panels = 0.05;
}

void Pulsar::CalibratorInfo::prepare (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::CalibratorInfo::prepare" << endl;

  Calibrator::Info* info = CalibratorParameter::get_Info (data);

  get_frame()->get_label_above()->set_centre("$file\n"+info->get_title());

  unsigned nclass = info->get_nclass();

  // fraction of viewport reserved for total space between panels
  float yspace = 0;
  // fraction of viewport for space between each panel
  float ybetween = 0;

  if (nclass > 1)
  {
    yspace = between_panels;
    ybetween = yspace / float(nclass -1);
  }

  // fraction of viewport for each panel
  float yheight = (1.0 - yspace) / float(nclass);
  float ybottom = 0.0;

  for (unsigned iclass=0; iclass < nclass; iclass++)
  {
    Reference::To<CalibratorParameter> plot = new CalibratorParameter;

    string name = "panel" + tostring(iclass);
    manage (name, plot);

    plot->set_class (iclass);
    plot->prepare (info, data);

    plot->get_frame()->set_viewport (0,1, ybottom, ybottom+yheight);
    ybottom += yheight + ybetween;

    // horizontal y-axis enumeration
    plot->get_frame()->get_y_axis()->add_opt('V');

    // increase the displacement between the label and the axis
    plot->get_frame()->get_y_axis()->set_displacement( 3.5 );

    if (iclass > 0)
    {
      // remove x-axis label
      plot->get_frame()->get_x_axis()->set_label("");

      // remove x-axis enumeration
      plot->get_frame()->get_x_axis()->rem_opt('N');
    }
  }
}


//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::CalibratorInfo::get_interface ()
{
  return new Interface (this);
}

Pulsar::CalibratorInfo::Interface::Interface (CalibratorInfo* instance)
{
  if (instance)
    set_instance (instance);

  add( &CalibratorInfo::get_between_panels,
       &CalibratorInfo::set_between_panels,
       "space", "Fraction of viewport for space between panels" );
}
