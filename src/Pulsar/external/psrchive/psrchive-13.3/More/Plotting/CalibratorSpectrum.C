/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorSpectrum.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ReferenceCalibrator.h"

#include <cpgplot.h>
#include <assert.h>

using namespace std;

Pulsar::CalibratorSpectrum::CalibratorSpectrum ()
{
  isubint = 0;
  isubint.set_integrate(true); // Default to summing to match previous
  plot_total = false;
  plot_low = false;
  plot_Ip = false;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
  get_frame()->get_x_scale()->set_buf_norm(0.05);

  plotter.set_control_viewport (false);
  plotter.set_minimum_error (0.0);
}
 
TextInterface::Parser* Pulsar::CalibratorSpectrum::get_interface ()
{
  return new Interface (this);
}

/*
  Because it assumes that the errors of its two arguments are uncorrelated,
  the variance is underestimated by Estimate<double>::operator*(x,y)
*/
Estimate<double> sqr (const Estimate<double>& x)
{
  Estimate<double> result = x*x;
  result.var *= 2;
  return result;
}

void Pulsar::CalibratorSpectrum::prepare (const Archive* data)
{
  Reference::To<Archive> clone;

  if (plot_Ip && data->get_state() != Signal::Stokes)
  {
    clone = data -> clone();
    clone->convert_state(Signal::Stokes);
    data = clone;
  }

  vector< vector< Estimate<double> > > hi;
  vector< vector< Estimate<double> > > lo;

  unsigned nchan = data->get_nchan();
  unsigned npol = data->get_npol();

  Reference::To<const Integration> subint = get_Integration(data, isubint);
  ReferenceCalibrator::get_levels (subint, nchan, hi, lo);

  assert (hi.size() == npol);

  unsigned ipol, ipt, npt = hi[0].size();

  if (!plot_total)
    for (ipol=0; ipol<npol; ipol++)
      for (ipt=0; ipt<npt; ipt++)
	hi[ipol][ipt] -= lo[ipol][ipt];

  if (plot_low)
    for (ipol=0; ipol<npol; ipol++)
      for (ipt=0; ipt<npt; ipt++)
	hi[ipol][ipt] = lo[ipol][ipt];

  if (plot_Ip)
  {
    for (ipt=0; ipt<npt; ipt++)
      if (hi[0][ipt].get_variance() != 0)
	hi[1][ipt] = sqrt( sqr(hi[1][ipt]) +
			   sqr(hi[2][ipt]) +
			   sqr(hi[3][ipt]) );
    npol = 2;
  }

  double cfreq = data->get_centre_frequency();
  double bw = data->get_bandwidth();

  plotter.clear ();
  plotter.set_xrange (cfreq-0.5*bw, cfreq+0.5*bw);

  for (ipol=0; ipol<npol; ipol++)
    plotter.add_plot (hi[ipol]);

  get_frame()->get_y_scale()->set_minmax (plotter.get_y_min(), 
					  plotter.get_y_max());
}


//! Derived classes must draw in the current viewport
void Pulsar::CalibratorSpectrum::draw (const Archive* data)
{
  unsigned npol = data->get_npol();

  if (plot_Ip)
    npol = 2;

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    cpgsci (ipol+1);
    plotter.plot (ipol);
  }
}


//! Return the label for the y-axis
std::string Pulsar::CalibratorSpectrum::get_ylabel (const Archive* data)
{
  string type = tostring( data->get_state() );

  if (plot_Ip)
    type = "Total and Polarized Flux";

  if (plot_low)
    return "Off-Pulse " + type;
  else if (plot_total)
    return "On-Pulse " + type;
  else 
    return "Calibrator " + type;
}

