/***************************************************************************
 *
 *   Copyright (C) 2006 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PowerSpectra.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"
#include "median_smooth.h"

#include <cpgplot.h>

#include <float.h>

using namespace std;

Pulsar::PowerSpectra::PowerSpectra ()
{
  isubint = ipol = 0;
  draw_lines = true;
  median_window = 0;
  logarithmic = false;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}
 
TextInterface::Parser* Pulsar::PowerSpectra::get_interface ()
{
  return new Interface (this);
}

//! Derived classes must compute the minimum and maximum values (y-axis)
void Pulsar::PowerSpectra::prepare (const Archive* data)
{
  spectra.clear();
  get_spectra (data);

  if (!spectra.size())
    throw Error (InvalidState, "Pulsar::PowerSpectra::prepare",
		 "Spectra array empty after call to get_spectra");

  if (logarithmic)
  {
    // logarithmic axis
    frame->get_y_axis()->add_opt ('L');
    // vertical labels
    frame->get_y_axis()->add_opt ('V');
    // exponential notation
    frame->get_y_axis()->add_opt ('2');
    // increase the space between the label and the axis
    frame->get_y_axis()->set_displacement (3.0);
  }

  unsigned i_min, i_max;
  get_scale()->get_indeces (data, i_min, i_max);

  if (i_min >= i_max)
    throw Error (InvalidState, "Pulsar::PowerSpectra::prepare",
		 "frequency scale i_min=%d >= i_max=%d", i_min, i_max);

  float min = FLT_MAX;
  float max = -FLT_MAX;

  for (unsigned iprof=0; iprof < spectra.size(); iprof++)
  {
    if (median_window)
      fft::median_smooth (spectra[iprof], median_window);

    if (logarithmic)
    {
      double log_base = 1.0/log(10.0);

      unsigned nchan = data->get_nchan();
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        if (spectra[iprof][ichan] <= 0)
          spectra[iprof][ichan] = -FLT_MAX;
        else
          spectra[iprof][ichan] = log_base * log( spectra[iprof][ichan] );
      }
    }

    for (unsigned ichan=i_min; ichan < i_max; ichan++)
      if (spectra[iprof][ichan] != 0 && spectra[iprof][ichan] != -FLT_MAX)
      {
        min = std::min( min, spectra[iprof][ichan] );
        max = std::max( max, spectra[iprof][ichan] );
      }

    for (unsigned ichan=i_min; ichan < i_max; ichan++)
      if (spectra[iprof][ichan] == -FLT_MAX)
        spectra[iprof][ichan] = min;

  }

  if (verbose)
    cerr << "Pulsar::PowerSpectra::prepare"
      " min=" << min << " max=" << max << endl;

  get_frame()->get_y_scale()->set_minmax (min, max);
}


//! Derived classes must draw in the current viewport
void Pulsar::PowerSpectra::draw (const Archive* data)
{
  get_scale()->get_ordinates (data, frequencies);

  for (unsigned iprof=0; iprof < spectra.size(); iprof++)
  {
    if (plot_sci.size() == spectra.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof+1);

    if (plot_sls.size() == spectra.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof+1);

    draw (spectra[iprof]);
  }
}

//! draw the profile in the current viewport and window
void Pulsar::PowerSpectra::draw (const vector<float>& data) const
{
  if (verbose)
  {
    float x_min, x_max;
    float y_min, y_max;
    cpgqwin (&x_min, &x_max, &y_min, &y_max);
    
    cerr << "Pulsar::PowerSpectra::draw xmin=" << x_min << " xmax=" << x_max
	 << " ymin=" << y_min << " ymax=" << y_max << endl;
  }

  const float* x = &frequencies[0];
  const float* y = &data[0];

  if (get_frame()->get_transpose())
    swap (x,y);

  if (draw_lines)
    cpgline (data.size(), x, y);
  else
    cpgpt (data.size(), x, y, -1);
}



//! Return the label for the y-axis
std::string Pulsar::PowerSpectra::get_ylabel (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else if (logarithmic)
    return "Log (Flux)";
  else
    return "Relative Flux Units";
}

