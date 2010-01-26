/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxPlot.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/InfoLabel.h"
#include "Physical.h"
#include <cpgplot.h>

using namespace std;

Pulsar::FluxPlot::FluxPlot ()
{
  isubint = ichan = ipol = 0;
  logarithmic = false;

  peak_zoom = 0;
  peak_centre_origin = true;

  baseline_zoom = 0;
  plot_ebox = 0;

  original_nchan = 0;

  crop = 0.0;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}

Pulsar::FluxPlot::~FluxPlot ()
{
}

TextInterface::Parser* Pulsar::FluxPlot::get_interface ()
{
  return new Interface (this);
}

/*! The ProfileVectorPlotter class computes the minimum and maximum values
  to be plotted */
void Pulsar::FluxPlot::prepare (const Archive* data)
{
  plotter.profiles.clear();

  // derived classes fill the plotter.profiles attribute
  get_profiles (data);

  if (peak_zoom)
    auto_scale_phase (plotter.profiles[0], peak_zoom);

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

    Reference::To<PhaseWeight> weight = plotter.profiles[0]->baseline();

    // the standard deviation of an exponential distribution equals its mean
    double rms = weight->get_rms ();
    float log_noise = log(rms) / log(10.0);

    for (unsigned iprof=0; iprof < plotter.profiles.size(); iprof++)
    {
      Reference::To<Profile> temp = plotter.profiles[iprof]->clone();
      temp->logarithm (10.0, rms);
      temp->offset( -log_noise );
      plotter.profiles[iprof] = temp;
    }
  }

  if (baseline_zoom)
    selection = plotter.profiles[0]->baseline();

  if (selection && baseline_zoom)
  {
    if (verbose)
      cerr << "Pulsar::FluxPlot::prepare using selected bins" << endl;

    double min = selection->get_min();
    double max = selection->get_max();

    Estimate<double> mean = selection->get_mean();
    Estimate<double> rms = sqrt(selection->get_variance());
    min = std::min( min, mean.get_value() - baseline_zoom*rms.get_value() );
    max = std::max( max, mean.get_value() + baseline_zoom*rms.get_value() );

    frame->get_y_scale()->set_minmax (min, max);
  }
  else
  {
    if (verbose)
      cerr << "Pulsar::FluxPlot::prepare using all bins" << endl;
    plotter.minmax (get_frame());

    /* Added by DS, for pav we want the peak to represent the
     percentage of max above 0 and if the graph has a negative
     component, and the magnitude of the negative component is more
     than the cropped positive component, then crop the nevative
     as well.
    */

    if( crop != 0.0 )
    {
      float min, max;
      get_frame()->get_y_scale()->get_minmax( min, max );

      max *= crop;
      if( min < 0 && max + min < 0 )
        min = -max;

      pair<float,float> coords;
      coords.first = min;
      coords.second = max;
      frame->get_y_scale()->set_world( coords );
    }
  }
}



/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluxPlot::draw (const Archive* data)
{
  // PhaseScale::get_ordinates fills the x-axis with turns/deg/milliseconds ...
  get_scale()->get_ordinates (data, plotter.ordinates);

  pair<float,float> range = get_frame()->get_x_scale()->get_range_norm();

  if (verbose)
    cerr << "Pulsar::FluxPlot::draw ProfileVectorPlotter::draw ("
	 << range.first << ", " << range.second << ")" << endl;

  plotter.draw ( range.first, range.second );

  cpgsci (1);
  if (plot_ebox)
    plot_error_box (data);

  if (selection)
    plot_selection ();
  
}

void Pulsar::FluxPlot::plot_selection ()
{
#ifdef _DEBUG
  cerr << "Pulsar::FluxPlot::plot_selection" << endl;
#endif

  vector<float> x = plotter.ordinates;
  float scale = ((x.back() - x.front()) * x.size()) / (x.size()-1);

  const float* amps = plotter.profiles[0]->get_amps();

  pair<float,float> range = get_scale()->get_range_norm();

  int range_start = int(floor(range.first));
  int range_end = int(ceil(range.second));

  cpgsci (5);

  for( int range = range_start; range < range_end; range ++ )
  {
    float xoff = float(range) * scale;

    for (unsigned i=0; i<x.size(); i++)
      if ((*selection)[i])
	cpgpt1 (x[i] + xoff, amps[i], 17);
  }

  if (!baseline_zoom)
    return;

  float x_min = 0;
  float x_max = 0;
  get_scale()->PlotScale::get_range (x_min, x_max);

  Estimate<double> mean = selection->get_mean ();
  Estimate<double> var = selection->get_variance ();
  double error = sqrt (mean.get_variance() + var.get_value());

  cpgsci (4);
  cpgmove (x_min, mean.get_value());
  cpgdraw (x_max, mean.get_value());

  cpgsls (2);

  for (double offset=-2*error; offset <= 2*error; offset+=error) {
    cpgmove (x_min, mean.get_value() + offset);
    cpgdraw (x_max, mean.get_value() + offset);
  }

}

void Pulsar::FluxPlot::set_selection (const PhaseWeight* w)
{
  selection = w; 
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluxPlot::plot_profile (const Profile* data)
{
  plotter.profiles.clear();
  plotter.profiles.push_back (data);
  plotter.minmax (get_frame());

  get_frame()->focus (0);

  get_scale()->get_ordinates (0, plotter.ordinates);

  pair<float,float> range = get_frame()->get_x_scale()->get_range_norm();
  plotter.draw ( data, range.first, range.second );

  if (selection)
    plot_selection ();  
}

//! Scale in on the on-pulse region
void Pulsar::FluxPlot::auto_scale_phase (const Profile* profile, float buf)
{
  int rise, fall;
  profile->find_peak_edges (rise, fall);

  float nbin = profile->get_nbin ();
  float start = rise / nbin;
  float stop = fall / nbin;

  if (start >= stop)
    stop += 1.0;

  if (verbose)
    cerr << "AUTO ZOOM rise=" << rise << " fall=" << fall 
	<< " nbin=" << nbin << endl
	<< "AUTO ZOOM phase rise=" << start << " fall=" << stop << endl;

  double mean = (stop + start) * 0.5;
  double diff = (stop - start) * 0.5;

  double span = buf * diff;

  if (span > 0.5)
    span = 0.5;

  start = mean - span;
  stop  = mean + span;

  if (verbose)
    cerr << "AUTO ZOOM scaled rise=" << start << " fall=" << stop << endl;

  get_frame()->get_x_scale()->set_range_norm (start, stop);

  if (peak_centre_origin)
    get_scale()->set_origin_norm( -0.5 * (start+stop) );

}

template<typename T> T sqr (T x) { return x*x; }

//! Return the label for the y-axis
std::string Pulsar::FluxPlot::get_ylabel (const Archive* data)
{
  if (logarithmic)
    return "Signal-to-Noise Ratio";
  else if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else
    return "Flux";
}

float Pulsar::FluxPlot::get_phase_error (const Archive* data)
{
  double dm        = data->get_dispersion_measure();
  double freq      = data->get_centre_frequency();
  double chan_bw   = data->get_bandwidth() / data->get_nchan();

  if (original_nchan)
    chan_bw = data->get_bandwidth() / original_nchan;

  const ProcHistory* history = data->get<ProcHistory>();
  if (history && history->rows.size())
    chan_bw = history->rows[0].chan_bw;

  chan_bw = fabs(chan_bw);

  double period = data->get_Integration(0)->get_folding_period();

  if (verbose)
  {
    cerr << "Frequency = " << freq << endl;
    cerr << "Channel bandwidth = " << chan_bw << endl;
    cerr << "DM = " << dm << endl;
    cerr << "period = " << period*1e3 << " ms" << endl;
  }

  // DM smearing in seconds
  double dm_smearing = dispersion_smear (dm, freq, chan_bw);
  if (verbose)
    cerr << "Dispersion smearing = " << dm_smearing*1e3 << " ms" << endl;

  // Scattering in seconds
  double scattering = pow(dm/1000,3.5) * pow(400/freq,4);
  if (verbose)
    cerr << "Predicted scattering = " << scattering*1e3 << " ms" << endl;

  // Time resolution in seconds
  double time_res = period / data->get_nbin();
  if (verbose)
    cerr << "Time resolution = " << time_res*1e3 << " ms" << endl;

  time_res = sqrt( sqr(dm_smearing) + sqr(scattering) + sqr(time_res) );
  if (verbose)
    cerr << "TOTAL resolution = " << time_res*1e3 << " ms" << endl;
  
  float x_error = time_res / period;
  if (verbose)
    cerr << "Phase error = " << x_error << " turns" << endl;

  return x_error;
}

float Pulsar::FluxPlot::get_flux_error (const Profile* profile)
{
  // sigma error box
  double var = profile->baseline()->get_variance().get_value();
  float y_error = sqrt((double)var);

  if (verbose)
    cerr << "Flux error = " << y_error << endl;

  return y_error;
}


void Pulsar::FluxPlot::plot_error_box (const Archive* data)
{
  float y_error = get_flux_error (plotter.profiles[0]);
  float x_error = get_phase_error (data);

  float x_min = 0;
  float x_max = 0;
  get_frame()->get_x_scale()->get_range (x_min, x_max);

  float y_min = 0;
  float y_max = 0;
  get_frame()->get_y_scale()->get_range (y_min, y_max);

  PlotLabel* label = get_frame()->get_label_below();

  // left side of error box defined by label margin
  float x1 = x_min + label->get_margin (pgplot::World);
  float x2 = x1 + x_error * get_scale()->get_scale (data);

  float y1 = 0;
  float y2 = 0;

  switch (plot_ebox)
    {
    case 1:
      // bottom of error box is ten sigma away from baseline
      y1 = 10 * y_error;
      y2 = y1 + 4.0 * y_error;
      break;

    case 2:
      // middle of error box is half way between baseline and top of plot
      y1 = 0.5 * y_max - 2.0 * y_error;
      y2 = 0.5 * y_max + 2.0 * y_error;
      break;

    case 3:
      {
        // top edge of error box is one row below last row in top left
        unsigned nrows = label->get_nrows( label->get_left() );
        y2 = y_max + label->get_displacement( nrows, pgplot::World );
        y1 = y2 - 4.0 * y_error;
        break;
      }

    default:
      throw Error (InvalidState, "Pulsar::FluxPlot::plot_error_box",
		   "unknown error box code = %d", plot_ebox);
    }

  if (verbose)
    cerr << "x1=" << x1 << " x2=" << x2 
	 << " y1=" << y1 << " y2=" << y2 << endl;

  cpgmove (x1,y1);
  cpgdraw (x2,y1);
  cpgdraw (x2,y2);
  cpgdraw (x1,y2);
  cpgdraw (x1,y1);
}



void Pulsar::FluxPlot::set_info_label (bool flag)
{
  if (flag) {
    info_label = new InfoLabel (this);
    get_frame()->set_label_above( info_label );
    get_frame()->get_label_below()->set_all(PlotLabel::unset);
  }
  else
    get_frame()->set_label_above( new PlotLabel );
}

bool Pulsar::FluxPlot::get_info_label () const
{
  return info_label;
}

