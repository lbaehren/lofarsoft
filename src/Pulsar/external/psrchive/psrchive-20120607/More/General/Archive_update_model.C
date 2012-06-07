/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Generator.h"
#include "Pulsar/Predictor.h"

#include "Error.h"

using namespace std;

// defined in Base/Resources/Tempo_config.C
namespace Tempo { int config (); }

static int ensure_tempo_configuration = Tempo::config ();

// ///////////////////////////////////////////////////////////////////////
//
// Archive::update_model
//
/*!  
  This method should be called when the ephemeris attribute is
  modified.  It may also be called when the tempo support files
  (e.g. leap.sec, ut1.dat) change.
*/
void Pulsar::Archive::update_model() 
{
  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model" << endl;

  update_model (get_nsubint(), true);
}


// ///////////////////////////////////////////////////////////////////////
//
// Archive::update_model
//
/*!
  This method economizes on the number of times that the polyco is
  re-created and the Integrations are re-aligned to the model.

  By setting the Integration::zero_phase_aligned attribute, each
  sub-integration is flagged as no longer in need of alignment.

  If Integration::zero_phase_aligned is false, then the Integration is
  re-aligned to the new model (see Archive::apply_model).

  \param nsubint the number of Integrations to correct
  \param clear if true, clear the current model
*/
void Pulsar::Archive::update_model (unsigned nsubint, bool clear) try
{

  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model nsubint=" << nsubint << endl;

  Reference::To<Predictor> oldmodel;

  if (clear)
    oldmodel = model;

  create_updated_model (clear);

  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model checking first " 
         << nsubint << " Integrations" << endl;

  // correct the old Integrations with the old model
  for (unsigned isub = 0; isub < nsubint; isub++)
  {
    if (!get_Integration(isub)->zero_phase_aligned)
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::update_model phasing isub=" << isub << endl;
      apply_model (get_Integration(isub), oldmodel.ptr());
    }
    else if (verbose > 2)
      cerr << "Pulsar::Archive::update_model isub=" << isub << " phased" << endl;
  }

}
catch (Error& error)
{
  throw error += "Pulsar::Archive::update_model";
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::create_updated_model
//
/*!  The polyco need only describe the phase and period of every
  Integration.  When the Integrations are separated by a large amount
  of time, the creation of a new polyco to completely span this time
  results in a huge polyco.dat and a huge waste of time.

  Therefore, this method attempts to create the minimum set of polyco
  polynomials required to describe the data.  If a match is not found
  in the current model, a single polynomial is created and appended to
  the current model.

  \param clear_model delete the current model after copying its attributes
*/
void Pulsar::Archive::create_updated_model (bool clear_model)
{
  if (get_type() != Signal::Pulsar)
    throw Error (InvalidState, "Pulsar::Archive::create_updated_model",
		 "not a pulsar observation");

  if (verbose > 2)
    cerr << "Pulsar::Archive::create_updated_model clear_model=" 
         << clear_model << endl;

  if (clear_model)
    for (unsigned isub = 0; isub < get_nsubint(); isub++)
      get_Integration(isub)->zero_phase_aligned = false;

  for (unsigned isub = 0; isub < get_nsubint(); isub++)
  {
    MJD time = get_Integration(isub)->get_epoch();

    if (verbose > 2)
      cerr << "Pulsar::Archive::create_updated_model isub=" << isub
           << " epoch=" << time << endl;

    update_model (time, clear_model);

    // clear the model only on the first time around the loop
    clear_model = false;
  }
}

/* This method ensures the the specified time is described by the
   polyco.  If a match is not found in the current model, a single
   polynomial is created and appended to the current model.

   \param clear_model delete the current model after copying its attributes
 */
void Pulsar::Archive::update_model (const MJD& time, bool clear_model)
{
  if (get_type() != Signal::Pulsar)
    throw Error (InvalidState, "Pulsar::Archive::update_model",
		 "not a pulsar observation");

  if (verbose > 2) cerr << "Pulsar::Archive::update_model epoch=" << time 
			 << " clear=" << clear_model << endl;

  if (model && !clear_model) try {

    model->phase (time);

    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model current model spans epoch"
	   << endl;

    return;

  }
  catch (Error& error) {

    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model current model doesn't span epoch"
	"\n\t" << error.get_message() << endl;

  }

  if (!ephemeris)
    throw Error (InvalidState, "Pulsar::Archive::update_model",
		 "no Pulsar::Parameters available");

  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model Predictor::policy=" 
	 << Predictor::get_policy() << endl;

  Reference::To<Generator> generator;
  if (model && Pulsar::Predictor::get_policy() == Pulsar::Predictor::Input)
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model get matching generator "
              "from current predictor" << endl;
    generator = model->generator();
  }
  else if (Pulsar::Predictor::get_policy() == Pulsar::Predictor::Default)
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model using default generator" << endl;
    generator = Generator::get_default();
  }
  else
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model produce generator"
	" that matches parameters" << endl;
    generator = Generator::factory (ephemeris);
  }

  generator->set_parameters (ephemeris);

  double frequency = get_centre_frequency ();
  double bandwidth = get_bandwidth ();

  generator->set_frequency_span( frequency - 0.5 * bandwidth,
				 frequency + 0.5 * bandwidth );

  generator->set_time_span( time, time );

  generator->set_site( get_telescope() );

  Reference::To<Predictor> predictor = generator->generate();

  if (clear_model || !model)
    model = predictor;
  else
    model->insert (predictor);

}


