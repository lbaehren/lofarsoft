/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "ScintPowerEstimator.h"
#include "Pulsar/ScintArchive.h"

//! Default constructor
Pulsar::ScintPowerEstimator::ScintPowerEstimator (const ScintArchive* arch)
  : PowerEstimator ("ScintPowerEstimator")
{
  archive = arch;
}

//! Copy constructor
Pulsar::ScintPowerEstimator::ScintPowerEstimator (const ScintPowerEstimator& e)
  : PowerEstimator ("ScintPowerEstimator")
{
  archive = e.archive;
}

//! Operator =
const Pulsar::ScintPowerEstimator&
Pulsar::ScintPowerEstimator::operator= (const ScintPowerEstimator& extension)
{
  archive = extension.archive;
  return *this;
}

//! Destructor
Pulsar::ScintPowerEstimator::~ScintPowerEstimator ()
{
}

//! Implement PowerEstimator::set_archive
void Pulsar::ScintPowerEstimator::set_archive (const ScintArchive* arch)
{
  archive = arch;
}

void Pulsar::ScintPowerEstimator::get_power (unsigned isub, unsigned ipol,
					     std::vector<float>& on_power,
					     std::vector<float>& off_power)
  const
{
  unsigned nchan = archive->get_nchan();                                      

  on_power.resize (nchan);
  off_power.resize (nchan);

  float amps[4];
  float sums[4];

  float max;
  float min;

  for (unsigned ichan=0; ichan < nchan; ichan++) {

    amps[0] = archive->get_passband(0)[ichan];
    amps[1] = archive->get_passband(1)[ichan];
    amps[2] = archive->get_passband(2)[ichan];
    amps[3] = archive->get_passband(3)[ichan];

    sums[0] = amps[0] + amps[1];
    sums[1] = amps[1] + amps[2];
    sums[2] = amps[2] + amps[3];
    sums[3] = amps[3] + amps[0];

    if (sums[0] > sums[1]) {
      max = sums[0];
      min = sums[2];
    }
    else {
      max = sums[1];
      min = sums[3];
    }

    if (sums[2] > max) {
      max = sums[2];
      min = sums[0];
    }

    if (sums[3] > max) {
      max = sums[3];
      min = sums[1];
    }

    on_power[ichan] = max;
    off_power[ichan] = min;

    // cerr << "max=" << max << " min=" << min << endl;
  }

}

