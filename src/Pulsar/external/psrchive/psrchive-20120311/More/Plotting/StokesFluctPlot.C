/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesFluctPlot.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/Fourier.h"

#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/LastSignificant.h"

#include <cpgplot.h>

using namespace std;

Pulsar::StokesFluctPlot::StokesFluctPlot ()
{
  plot_last_harmonic = false;

  plot_values  = "Ip";
  plot_colours = "123";
  plot_lines   = "111";

  PlotFrame* frame = get_frame();

  // logarithmic axis
  frame->get_y_axis()->add_opt ('L');
  // vertical labels
  frame->get_y_axis()->add_opt ('V');
  // exponential notation
  frame->get_y_axis()->add_opt ('2');
  // increase the space between the label and the axis
  frame->get_y_axis()->set_displacement (3.0);

  // increase the space between the y-axis and the data
  frame->get_x_scale()->set_buf_norm (0.04);

  PlotLabel* below = frame->get_label_below();

  // swap the label from the left to the right
  below->set_right( below->get_left() );
  below->set_left( PlotLabel::unset );

  signal_to_noise = true;
}

TextInterface::Parser* Pulsar::StokesFluctPlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesFluctPlot::prepare (const Archive* data)
{
  FluctPlot::prepare (data);

  // enforce that the minimum always start at zero (s/n = 1)
  float min, max;
  get_frame()->get_y_scale()->get_minmax (min, max);
  get_frame()->get_y_scale()->set_minmax (0.0, max);
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::StokesFluctPlot::draw (const Archive* data)
{
  FluctPlot::draw (data);
  if (plot_last_harmonic)
  {
    float min, max;
    get_frame()->get_y_scale()->get_minmax (min, max);
    cpgmove (last_harmonic, min);
    cpgdraw (last_harmonic, max*.5);
  }
}


//! Return the label for the y-axis
std::string Pulsar::StokesFluctPlot::get_ylabel (const Archive* data)
{
  if (signal_to_noise)
    return "Signal-to-Noise Ratio";
  else
    return "Fluctuation Power";
}

namespace Pulsar
{
  void moddet (Profile* moddet, const PolnProfile* data);
}

#include "Pauli.h"

void Pulsar::moddet (Profile* moddet, const PolnProfile* data)
{
  unsigned nbin = data->get_nbin() / 2;
  Signal::State state = data->get_state();

  moddet->resize (nbin);
  float* amps = moddet->get_amps();

  Jones<double> coherence;
  complex<double> Ci (0,1);

  for (unsigned ibin = 0; ibin < nbin; ibin++)
  {
    if (state == Signal::Stokes)
    {
      Stokes< complex<double> > real = data->get_Stokes(ibin*2);
      Stokes< complex<double> > imag = data->get_Stokes(ibin*2+1);

      Stokes< complex<double> > stokes = real + Ci*imag;

      coherence = convert (stokes);
    }
    else if (state == Signal::Coherence)
    {
      Jones<double> real = data->get_coherence(ibin*2);
      Jones<double> imag = data->get_coherence(ibin*2+1);

      coherence = real + Ci*imag;
    }

    amps[ibin] = sqrt(norm(det(coherence)));
  }
}


Pulsar::Profile* new_Fluct (const Pulsar::PolnProfile* data, char code)
{
  Reference::To<Pulsar::Profile> profile;

  switch (code) {
  case 'I':
    return data->get_Profile(0)->clone();
  case 'Q':
    return data->get_Profile(1)->clone();
  case 'U':
    return data->get_Profile(2)->clone();
  case 'V':
    return data->get_Profile(3)->clone();

  case 'L':
  {
    // total linearly polarized flux
    profile = data->get_Profile(1)->clone();
    profile->sum (data->get_Profile(2));
    return profile.release();
  }

  case 'p':
  {
    // total polarized flux
    profile = data->get_Profile(1)->clone();
    profile->sum (data->get_Profile(2));
    profile->sum (data->get_Profile(3));
    return profile.release();
  }

  case 'S':
  {
    // polarimetric invariant flux
    profile = new Pulsar::Profile;
    data->invint(profile);
    return profile.release();
  }

  case 'd':
  {
    // modulus of undetected determinant
    profile = new Pulsar::Profile;
    moddet (profile, data);
    return profile.release();
  }

  default:
    throw Error (InvalidParam, "Pulsar::get_Profile",
		 "Unknown polarization code '%c'", code);
  }

}

void Pulsar::StokesFluctPlot::get_profiles (const Archive* data)
{
  plotter.profiles.resize( plot_values.size() );
  plotter.plot_sci.resize( plot_values.size() );
  plotter.plot_sls.resize( plot_values.size() );
 
  if (plot_values.size() > plot_colours.size())
    throw Error (InvalidState, "Pulsar::StokesPlot::get_profiles",
                 "Mismatch: %u plots and %u colours",
                 plot_values.size(), plot_colours.size());
 
  if (plot_values.size() > plot_lines.size())
    throw Error (InvalidState, "Pulsar::StokesPlot::get_profiles",
                 "Mismatch: %u plots and %u lines",
                 plot_values.size(), plot_lines.size());

  if (verbose)
    cerr << "Pulsar::StokesFluctPlot::get_profiles calling get_Stokes" << endl;

  Reference::To<const PolnProfile> profile;

  if (plot_values != "I")
    profile = get_Stokes (data, isubint, ichan);

  if (verbose)
    cerr << "Pulsar::StokesFluctPlot::get_profiles filling vector" << endl;

  Pulsar::LastSignificant last_significant;

  for (unsigned ipol=0; ipol < plotter.profiles.size(); ipol++)
  {
    Reference::To<Profile> prof;

    if (plot_values == "I")
    {
      Index ipol;
      ipol.set_integrate (true);
      
      Reference::To<const Profile> I;
      I = get_Profile (data, isubint, ipol, ichan);

      prof = fourier_transform (I);
      detect (prof);
    }

    // special case for invariant interval: form before FFT
    else if (plot_values[ipol] == 'S')
    {
      prof = new_Fluct (profile, plot_values[ipol]);
      prof = fourier_transform (prof);
      detect (prof);
    }

    // special case for fluctuation determinant: no detection
    else if (plot_values[ipol] == 'd')
    {
      Reference::To<PolnProfile> fft = fourier_transform (profile);
      prof = new_Fluct (fft, plot_values[ipol]);
    }

    // all other cases: FFT before formation
    else
    {
      Reference::To<PolnProfile> fft = fourier_transform (profile);
      detect (fft);
      prof = new_Fluct (fft, plot_values[ipol]);
    }

    ExponentialBaseline baseline;
    Reference::To<PhaseWeight> weight = baseline.baseline( prof );

    // the standard deviation of an exponential distribution equals its mean
    double rms = weight->get_mean().get_value();

    if (plot_last_harmonic)
    {
      last_significant.find (prof, rms);
      cerr << ipol << ": last harmonic = " << last_significant.get() << endl;
    }
 
    float log_noise = log(rms) / log(10.0);

    prof->logarithm (10.0, rms/10.0);
    prof->offset( -log_noise );

    plotter.profiles[ipol] = prof;
    plotter.plot_sci[ipol] = plot_colours[ipol] - '0';
    plotter.plot_sls[ipol] = plot_lines[ipol] - '0';
  }

  last_harmonic = last_significant.get();

  if (verbose)
    cerr << "Pulsar::StokesFluctPlot::get_profiles last harmonic ibin="
	 << last_harmonic << endl;

}

