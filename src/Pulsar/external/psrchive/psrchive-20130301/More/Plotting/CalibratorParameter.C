/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorParameter.h"
#include "Pulsar/PlotFrame.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/Archive.h"

#include <cpgplot.h>
#include <assert.h>

using namespace std;

Pulsar::Calibrator::Info* 
Pulsar::CalibratorParameter::get_Info (const Archive* data)
{
  Reference::To<Calibrator> calibrator;

  if (data->get<Pulsar::FluxCalibratorExtension>())
  {
    if (verbose)
      cerr << "Pulsar::CalibratorParameter::prepare flux calibrator" << endl;
    calibrator = new Pulsar::FluxCalibrator (data);
  }

  if (data->get<Pulsar::PolnCalibratorExtension>())
  {
    if (verbose)
      cerr << "Pulsar::CalibratorParameter::prepare poln calibrator" << endl;
    calibrator = new Pulsar::PolnCalibrator (data);
  }

  if (!calibrator)
  {
    if (verbose)
      cerr << "Pulsar::CalibratorParameter::prepare create calibrator" << endl;
    calibrator = new Pulsar::SingleAxisCalibrator (data);
  }

  return calibrator->get_Info();
}

Pulsar::CalibratorParameter::CalibratorParameter ()
{
  get_frame()->get_y_scale()->set_buf_norm(0.05);
  get_frame()->get_x_scale()->set_buf_norm(0.05);

  // remove the default label (pulsar name and frequency)
  get_frame()->get_label_below()->set_all ("");

  plotter.set_control_viewport (false);
  plotter.set_minimum_error (0.0);

  iclass = 0;
  managed = false;
}


void Pulsar::CalibratorParameter::prepare (const Archive* data)
{
  if (!managed)
    prepare ( get_Info(data), data );
}

void Pulsar::CalibratorParameter::prepare (const Calibrator::Info* _info,
					   const Archive* data)
{
  info = _info;

  double cfreq = data->get_centre_frequency();
  double bw = data->get_bandwidth();

  unsigned nchan = info->get_nchan();

  plotter.clear ();

  double freq0 = cfreq-0.5*bw;
  double freq1 = cfreq+0.5*bw;

  /*
    compute the minimum and maximum ordinate values using only the
    data that will be plotted (as defined by the current x_scale)
  */

  unsigned i_min, i_max;
  get_frame()->get_x_scale()->set_minmax (freq0, freq1);
  get_frame()->get_x_scale()->get_indeces (nchan, i_min, i_max);

  plotter.set_xrange (freq0, freq1);
  plotter.set_minmax_range (i_min, i_max);

  /*   */

  unsigned nparam = info->get_nparam (iclass);

  for (unsigned iparam=0; iparam<nparam; iparam++)
  {
    vector< Estimate<double> > y (nchan);

    for (unsigned ichan=0; ichan < nchan; ichan++)
      y[ichan] = info->get_param (ichan, iclass, iparam);

    plotter.add_plot (y);
  }

  get_frame()->get_x_scale()->set_minmax (plotter.get_x_min(), 
					  plotter.get_x_max());

  get_frame()->get_y_scale()->set_minmax (plotter.get_y_min(), 
					  plotter.get_y_max());
}


//! Derived classes must draw in the current viewport
void Pulsar::CalibratorParameter::draw (const Archive* data)
{
  unsigned nparam = info->get_nparam (iclass);

  for (unsigned iparam=0; iparam<nparam; iparam++)
  {
    cpgsci ( info->get_colour_index (iclass, iparam) );
    plotter.plot (iparam);
  }
}


//! Return the label for the y-axis
std::string Pulsar::CalibratorParameter::get_ylabel (const Archive* data)
{
  return info->get_name( iclass );
}

// Text interface to the CalibratorParameter class
class Pulsar::CalibratorParameter::Interface 
  : public TextInterface::To<CalibratorParameter>
{
public:
  Interface (CalibratorParameter* instance)
  {
    if (instance)
      set_instance (instance);

    add( &CalibratorParameter::get_class,
	 &CalibratorParameter::set_class,
	 "class", "Class of model parameters to plot" );

  }
};

//! Return the text interface
TextInterface::Parser* Pulsar::CalibratorParameter::get_interface ()
{
  return new Interface(this);
}


