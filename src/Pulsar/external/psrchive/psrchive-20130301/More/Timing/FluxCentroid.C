/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCentroid.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

Pulsar::FluxCentroid::FluxCentroid ()
{
  duty_cycle = 0.25;
}

Estimate<double> Pulsar::FluxCentroid::get_shift () const
{
  Reference::To<PhaseWeight> baseline = observation->baseline();
  float mean = baseline->get_avg();
  float rms = baseline->get_rms();

  unsigned max_bin = observation->find_max_bin();

  const float* amps = observation->get_amps();
  unsigned nbin = observation->get_nbin();
  unsigned stop_bin = unsigned (nbin * duty_cycle * 0.5);

  double first_moment = 0;
  double total_flux = 0;
  double variance = 0.0;

  for (unsigned ibin=1; ibin<stop_bin; ibin++)
  {
    // positive direction
    float flux = amps[(max_bin+ibin)%nbin] - mean;
    first_moment += ibin * flux;
    total_flux += flux;

    // negative direction
    flux = amps[(max_bin+nbin-ibin)%nbin] - mean;
    first_moment -= ibin * flux;
    total_flux += flux;

    // one for each flux added
    variance += 2 * ibin*ibin * rms*rms;
  }

  double phase = (max_bin + first_moment/total_flux) / nbin;
  double norm = nbin * total_flux;
  variance /= norm*norm;

  // cerr << "phase=" << phase << " error=" << sqrt(variance) << endl;

  return Estimate<double> (phase, variance);
}
