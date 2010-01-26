/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/ProfileStats.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "StraightLine.h"
#include "EstimatePlotter.h"

#include <cpgplot.h>

class psrsp : public Pulsar::Application
{
public:

  // 
  psrsp ();

  //
  void process (Pulsar::Archive*);

  // compute the total flux spectrum
  void total_flux (Pulsar::Integration* subint);

  // compute the bin-by-bin spectrum
  void bin_flux (Pulsar::Integration* subint);
    
  // used to compute profile statistics
  Pulsar::ProfileStats stats;

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&) {}

};



void psrsp::process (Pulsar::Archive* archive)
{
  if (archive->get_scale() != Signal::Jansky)
    cerr << "psrsp::process WARNING archive is not flux calibrated" << endl;

  archive -> tscrunch();
  archive -> pscrunch();
  archive -> dedisperse();

  cpgpage();
  total_flux (archive->get_Integration(0));

  cpgpage();
  bin_flux (archive->get_Integration(0));
}

void psrsp::total_flux (Pulsar::Integration* subint)
{
  StraightLine<double> line;

  vector< float > frequencies;
  vector< Estimate<float> > fluxes;

  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++)
  {
    if (subint->get_weight(ichan) == 0)
      continue;

    stats.set_profile( subint->get_Profile(0,ichan) );

    double frequency = subint->get_centre_frequency(ichan);
    Estimate<float> flux = stats.get_total();

    line.add_coordinate (frequency, flux);

    frequencies.push_back( frequency );
    fluxes.push_back( flux );

    cout << frequency << " "
	 << flux.get_value() << " " << flux.get_error() << endl;
  }

  cerr << "slope=" << line.get_slope() << endl;

  EstimatePlotter plotter;
  plotter.add_plot( frequencies, fluxes );
  plotter.plot ();
  cpgbox("bcnst",0,0,"bcnst",0,0);
}

void psrsp::bin_flux (Pulsar::Integration* subint)
{
  vector< float > phases;
  vector< Estimate<float> > slopes;

  unsigned nbin = subint->get_nbin();
  unsigned nchan = subint->get_nchan();

  vector< float > baseline_variance (nchan, 0.0);

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (subint->get_weight(ichan) == 0)
      continue;
    stats.set_profile( subint->get_Profile(0,ichan) );
    baseline_variance[ichan] = stats.get_baseline_variance().get_value();
  }

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    StraightLine<double> line;

    for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++)
    {
      if (subint->get_weight(ichan) == 0)
	continue;

      double frequency = subint->get_centre_frequency (ichan);

      Pulsar::Profile* profile = subint->get_Profile (0, ichan);
      float* amps = profile->get_amps();

      Estimate<float> flux (amps[ibin], baseline_variance[ichan]);

      line.add_coordinate (frequency, flux);
    }

    Estimate<float> slope = line.get_slope();
    float phase = float(ibin) / float(nbin);

    phases.push_back( phase );
    slopes.push_back( slope );

  }

  EstimatePlotter plotter;
  plotter.add_plot( phases, slopes );
  plotter.plot ();
  cpgbox("bcnst",0,0,"bcnst",0,0);

}


psrsp::psrsp () : Application ("psrsp", "pulsar spectral power")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::PlotOptions );
}

int main (int argc, char** argv)
{
  psrsp program;

  return program.main (argc, argv);
}

