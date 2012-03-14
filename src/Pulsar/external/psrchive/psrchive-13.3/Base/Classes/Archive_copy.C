/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"

#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"

using namespace std;

void Pulsar::Archive::copy (const Archive* archive)
{
  copy (*archive);
}

void Pulsar::Archive::copy (const Archive& archive)
{
  const unsigned nsub = archive.copy_nsubint();
  
  if (verbose == 3)
    cerr << "Pulsar::Archive::copy " << nsub << " Integrations" << endl;

  if (this == &archive)
    return;
  
  // copy only the selected subints
  resize (nsub, archive.get_npol(), archive.get_nchan(), archive.get_nbin());
  
  for (unsigned isub=0; isub<nsub; isub++)
  {
    unsigned copy_isub = archive.copy_isubint( isub );
    const Integration* subint = archive.get_Integration( copy_isub );
    get_Integration(isub) -> copy (subint, false);
  }
  
  if (archive.ephemeris)
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::copy ephemeris" << endl;
    ephemeris = archive.ephemeris->clone();
  } 
  else
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::copy no ephemeris" << endl;
    ephemeris = 0;
  }

  if (archive.model)
    model = archive.model->clone();
  else
    model = 0;

  // set virtual attributes

  set_nbin( archive.get_nbin() );
  set_nchan( archive.get_nchan() );
  set_npol( archive.get_npol() );

  set_telescope( archive.get_telescope() );

  set_state( archive.get_state() );
  set_scale( archive.get_scale() );
  set_type( archive.get_type() );

  set_source( archive.get_source() );
  set_coordinates( archive.get_coordinates() );

  set_centre_frequency( archive.get_centre_frequency() );
  set_bandwidth( archive.get_bandwidth() );

  set_dispersion_measure( archive.get_dispersion_measure() );
  set_dedispersed( archive.get_dedispersed() );

  set_rotation_measure( archive.get_rotation_measure() );
  set_faraday_corrected( archive.get_faraday_corrected() );

  set_poln_calibrated( archive.get_poln_calibrated() );

  set_filename( archive.get_filename() );

  // Using a Reference::To<Extension> ensures that the cloned
  // Extension will be deleted if the derived class chooses not to
  // manage it.

  if (verbose == 3)
    cerr << "Pulsar::Archive::copy " << archive.get_nextension()
	 << " Extensions" << endl;

  extension.resize (0);

  for (unsigned iext=0; iext < archive.get_nextension(); iext++)
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::copy clone " 
	   << archive.get_extension(iext)->get_extension_name() << endl;

    Reference::To<Extension> ext = archive.get_extension(iext)->clone();
    add_extension (ext);
  }

  mixable_policy = archive.get_mixable()->clone();
  standard_match_policy = archive.get_standard_match()->clone();
  processing_match_policy = archive.get_processing_match()->clone();
  calibrator_match_policy = archive.get_calibrator_match()->clone();

  // Resize the IntegrationOrder Extension (if there is one)
  Pulsar::IntegrationOrder* tempio = get<Pulsar::IntegrationOrder>();
  if (!tempio)
    return;

  vector<double> tempvals(tempio->size(), 0.0);
  for (unsigned i = 0; i < tempio->size(); i++)
    tempvals[i] = tempio->get_Index(i);

  tempio->resize(nsub);
  if (tempvals.size() > 0)
  {
    for (unsigned i = 0; i < nsub; i++)
     tempio->set_Index(i, tempvals[ archive.copy_isubint(i) ]);
  }
}


