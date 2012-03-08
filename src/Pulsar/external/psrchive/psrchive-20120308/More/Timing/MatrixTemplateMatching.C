/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MatrixTemplateMatching.h"
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/PolnProfileFit.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace std;

Pulsar::MatrixTemplateMatching::MatrixTemplateMatching ()
{
  engine = new PulsarCalibrator;
  engine->set_solve_each ();
}

Pulsar::MatrixTemplateMatching::~MatrixTemplateMatching ()
{
}

//! Set the maximum number of harmonics to include in fit
void Pulsar::MatrixTemplateMatching::set_maximum_harmonic (unsigned max)
{
  engine->set_maximum_harmonic (max);
}

//! Allow software to choose the maximum harmonic
void Pulsar::MatrixTemplateMatching::set_choose_maximum_harmonic (bool flag)
{
  engine->set_choose_maximum_harmonic (flag);
}

void Pulsar::MatrixTemplateMatching::preprocess (Archive* archive)
{
  archive->convert_state (Signal::Stokes);
  engine->preprocess (archive);
}

void Pulsar::MatrixTemplateMatching::set_standard (const Archive* archive)
{
  engine->set_standard (archive);

  cerr << "Pulsar::MatrixTemplateMatching::set_standard: last harmonic=" 
       << engine->get_nharmonic() << endl;

  standard = archive;
}

//! Set the observation from which the arrival times will be derived
void Pulsar::MatrixTemplateMatching::set_observation (const Archive* archive)
{
  engine->prepare (archive);
  observation = archive;
}

//! get the arrival times for the specified sub-integration
void Pulsar::MatrixTemplateMatching::get_toas (unsigned isub,
					       std::vector<Tempo::toa>& toas)
{
  engine->add_pulsar (observation, isub);
  
  const Integration* integration = observation->get_Integration (isub);
  unsigned nchan = integration->get_nchan();

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!engine->get_transformation_valid(ichan))
      continue;

    Estimate<double> shift = engine->get_mtm(ichan) -> get_phase();

    toas.push_back( get_toa (shift, integration, ichan) );
  }

#if 0
  if (verbose > 2)
    cerr << aux << " freq=" << freq << " chisq=" << reduced_chisq[ichan]
	 << " phase=" << phase.val << " +/- " << phase.get_error() << endl;
#endif
}
