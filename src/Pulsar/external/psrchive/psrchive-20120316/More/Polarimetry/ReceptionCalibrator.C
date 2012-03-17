/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/ReceptionModelReport.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/SourceInfo.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfile.h"

#include "MEAL/PhysicalCoherency.h"
#include "MEAL/Complex2Constant.h"
#include "MEAL/Complex2Value.h"
#include "MEAL/ProductRule.h"
#include "MEAL/Gain.h"

#include "MEAL/Tracer.h"

#include "Pauli.h"

#include <algorithm>
#include <assert.h>

using namespace std;

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (Calibrator::Type* _type)
{
  type = _type;

  measure_cal_V = true;
  measure_cal_Q = false;

  normalize_by_invariant = false;
  independent_gains = false;

  check_pointing = false;
  physical_coherency = false;

  output_report = false;

  unique = 0;

  nthread = 1;
}

Pulsar::ReceptionCalibrator::~ReceptionCalibrator()
{
}

void Pulsar::ReceptionCalibrator::set_standard_data (const Archive* data)
{
  Reference::To<Archive> clone = data->clone ();

  clone->fscrunch();
  clone->tscrunch();

  Reference::To<PolnProfile> p = clone->get_Integration(0)->new_PolnProfile(0);

  standard_data = new Calibration::StandardData;

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::set_standard_data"
      " normalize_by_invariant=" << normalize_by_invariant << endl;

  standard_data->set_normalize (normalize_by_invariant);
  standard_data->select_profile( p );

  ensure_consistent_onpulse ();
}

void Pulsar::ReceptionCalibrator::ensure_consistent_onpulse ()
{
  if (!standard_data)
    return;

  ProfileStats* stats = standard_data->get_poln_stats()->get_stats();

  for (unsigned istate=0; istate < phase_bins.size(); istate++)
    stats->set_onpulse (phase_bins[istate], true);
}

const Pulsar::PhaseWeight* Pulsar::ReceptionCalibrator::get_baseline () const
{
  return standard_data->get_poln_stats()->get_stats()->get_baseline();
}

//! Get the on-pulse mask
const Pulsar::PhaseWeight* Pulsar::ReceptionCalibrator::get_onpulse () const
{
  return standard_data->get_poln_stats()->get_stats()->get_onpulse();
}

void Pulsar::ReceptionCalibrator::set_normalize_by_invariant (bool set)
{
  normalize_by_invariant = set;
  if (standard_data)
    standard_data->set_normalize (normalize_by_invariant);
}

/*!
  This method is called on the first call to add_observation.
  It initializes various arrays and internal book-keeping attributes.
*/

void Pulsar::ReceptionCalibrator::initial_observation (const Archive* data)
{
  if (!data)
    throw Error (InvalidState, "ReceptionCalibrator::initial_observation",
		 "no Archive");

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::initial_observation" << endl;

  if (data->get_type() != Signal::Pulsar)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() 
		 + "' not a Pulsar observation");

  if (data->get_state() != Signal::Stokes)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='%s' state=%s != Signal::Stokes",
		 data->get_filename().c_str(),
		 Signal::state_string(data->get_state()));

  // use the FrontendCorrection class to determine applicability
  FrontendCorrection corrections;

  if (! corrections.required (data))
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has been corrected for parallactic angle rotation");
		 
  if (!data->get_dedispersed ())
    cerr << "Pulsar::ReceptionCalibrator WARNING archive not dedispersed\n"
      "  Pulse phase will vary as a function of frequency channel" << endl;

  set_calibrator( data->clone() );

  if (!standard_data)
    set_standard_data (data);

  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular)
  {
    if (measure_cal_Q)
    {
      cerr << "Pulsar::ReceptionCalibrator cannot measure CAL Q"
              " in circular basis" << endl;
      measure_cal_Q = false;
    }
    if (measure_cal_V)
    {
      cerr << "Pulsar::ReceptionCalibrator cannot measure CAL V"
              " in circular basis" << endl;
      measure_cal_V = false;
    }
  }

  create_model ();

  if (calibrator_estimate.size() == 0)
  {
    has_pulsar = true;
    load_calibrators ();
  }

  assert( pulsar.size() == phase_bins.size() );

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimates ( pulsar[istate], phase_bins[istate] );

  add_epoch( data->start_time () );
}

void Pulsar::ReceptionCalibrator::init_model (unsigned ichan)
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::init_model ichan=" << ichan << endl;

  SystemCalibrator::init_model (ichan);

  if (normalize_by_invariant)
    model[ichan] -> set_constant_pulsar_gain ();

  if (measure_cal_Q)
    model[ichan] -> fix_orientation ();
}

void Pulsar::ReceptionCalibrator::load_calibrators ()
{
  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::load_calibrators" << endl;

  SystemCalibrator::load_calibrators ();

  if (!flux_calibrator_estimate.size())
    return;

  const unsigned nchan = get_nchan();
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (flux_calibrator_estimate.at(ichan).is_constrained())
      continue;

    cerr << "ichan=" << ichan << " top flux calibrator is not constrained" << endl;

    if (flux_calibrator_estimate[ichan].multiples.size() == 0)
      model[ichan]->set_valid (false, "no flux cal data");

    unsigned index = flux_calibrator_estimate[ichan].input_index;

    assert (index == model[ichan]->get_equation()->get_num_input() - 1);

    // the last attempt to add more than one flux calibrator constraint failed
    model[ichan]->get_equation()->erase_input( index );
  }
}


//! Add the specified pulse phase bin to the set of state constraints
void Pulsar::ReceptionCalibrator::add_state (unsigned phase_bin)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_state", false);

  if (verbose > 2)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" 
	 << phase_bin << endl;

  for (unsigned istate=0; istate<phase_bins.size(); istate++)
    if (phase_bins[istate] == phase_bin)
    {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin
	   << " already in use" << endl;
      return;
    }

  phase_bins.push_back (phase_bin);

  pulsar.resize( pulsar.size() + 1 );

  if (has_calibrator())
    init_estimates( pulsar.back(), phase_bin );
}


//! Get the number of pulsar phase bin input polarization states
unsigned Pulsar::ReceptionCalibrator::get_nstate_pulsar () const
{
  return pulsar.size();
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_calibrator (const Archive* data)
{
  if (data->get_type() == Signal::Calibrator)
    set_previous (data);
  else
    SystemCalibrator::add_calibrator (data);
}

bool equal_pi (const Angle& a, const Angle& b, float tolerance = 0.01);


void Pulsar::ReceptionCalibrator::set_previous (const Archive* data)
{
  const PolnCalibratorExtension* ext = data->get<PolnCalibratorExtension>();
  if (ext->get_type() == get_type())
  {
    cerr << "Pulsar::ReceptionCalibrator::set_previous solution of same type"
	 << endl;
    previous = new PolnCalibrator (data);
    previous_cal = data->get<CalibratorStokes>();
  }
}


//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::match (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::match", false);

  if (!has_calibrator())
    initial_observation (data);

  SystemCalibrator::match (data);
}


void Pulsar::ReceptionCalibrator::add_pulsar
( Calibration::CoherencyMeasurementSet& measurements,
  const Integration* integration, unsigned ichan )
{
  standard_data->set_profile( integration->new_PolnProfile (ichan) );

  for (unsigned istate=0; istate < pulsar.size(); istate++)
    add_data (measurements, pulsar.at(istate).at(ichan), ichan);

  DEBUG("Pulsar::ReceptionCalibrator::add_pulsar ADD DATA ichan=" << ichan);

  model[ichan]->get_equation()->add_data (measurements);
}

void
Pulsar::ReceptionCalibrator::add_data
( vector<Calibration::CoherencyMeasurement>& bins,
  SourceEstimate& estimate,
  unsigned ichan )
{
  estimate.add_data_attempts ++;
  get_data_call ++;

  unsigned ibin = estimate.phase_bin;

  try {

    Stokes< Estimate<double> > stokes = standard_data->get_stokes( ibin );

    // NOTE: the measured states are not corrected
    Calibration::CoherencyMeasurement state (estimate.input_index);
    state.set_stokes( stokes );
    bins.push_back ( state );

    /* Correct the stokes parameters using the current best estimate of
       the instrument and the parallactic angle rotation before adding
       them to best estimate of the input state */
    
    Jones< Estimate<double> > correct;

    model[ichan]->get_equation()->set_transformation_index
      (model[ichan]->get_pulsar_path());

    correct = inv( model[ichan]->get_pulsar_transformation()->evaluate() );

    stokes = transform( stokes, correct );
    
    estimate.source_guess.integrate( stokes );

  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "Pulsar::ReceptionCalibrator::add_data ichan=" << ichan 
	   << " ibin=" << ibin << " error\n\t" << error.get_message() << endl;
    estimate.add_data_failures ++;
    get_data_fail ++;
  }
}

void add_if_needed (Pulsar::SourceEstimate& estimate,
		    Calibration::SignalPath* model)
{
  if (!model->get_valid())
    return;

  assert ( estimate.add_data_attempts >= estimate.add_data_failures );

  unsigned success = estimate.add_data_attempts - estimate.add_data_failures;

  unsigned to_push = success - estimate.multiples.size();

#if _DEBUG
  cerr << "add_if_needed: attempts=" << estimate.add_data_attempts
       << " failures=" << estimate.add_data_failures 
       << " multiples=" << estimate.multiples.size() 
       << " to push=" << to_push << endl;
#endif

  assert ( to_push < 2 );

  if (!to_push)
    return;

  Pulsar::SourceEstimate::Multiple multiple;
  multiple.source = estimate.source;
  multiple.input_index = estimate.input_index;

  estimate.multiples.push_back( multiple );

  estimate.create_source (model->get_equation());
}

void init_flux (Pulsar::SourceEstimate& estimate)
{
  if (!estimate.source)
    return;

  // set the initial guess
  Stokes<double> flux_cal_state (1,0,0,0);
  
  estimate.source->set_stokes ( flux_cal_state );
  estimate.source->set_param_name_prefix( "flux_" );
}

void Pulsar::ReceptionCalibrator::prepare_calibrator_estimate
(Signal::Source source)
{
  SystemCalibrator::prepare_calibrator_estimate (source);

  if (source != Signal::FluxCalOn)
    return;

  cerr << "Pulsar::ReceptionCalibrator::add_calibrator FluxCalOn" << endl;

  if (flux_calibrator_estimate.size() == 0)
  { 
    // add the flux calibrator states to the equations
    init_estimates (flux_calibrator_estimate);

    for (unsigned ichan=0; ichan<get_nchan(); ichan++)
    {  
      init_flux (flux_calibrator_estimate[ichan]);

      // Flux Calibrator observations are made through a different backend
      model[ichan]->get_fluxcal()->add_backend();
    }
  }
  else
  {
    if (multiple_flux_calibrators)
    {
      for (unsigned ichan=0; ichan<get_nchan(); ichan++)
      {
	add_if_needed (flux_calibrator_estimate[ichan], model[ichan]);
	init_flux (flux_calibrator_estimate[ichan]);
      }
    }

    // each flux calibrator observation is made through a different backend
    for (unsigned ichan=0; ichan<get_nchan(); ichan++)
      model[ichan]->get_fluxcal()->add_backend();
  }
}

void Pulsar::ReceptionCalibrator::setup_calibrators ()
{
  for (unsigned ichan=0; ichan<calibrator_estimate.size(); ichan++)
    setup_poln_calibrator (calibrator_estimate[ichan]);

  for (unsigned ichan=0; ichan<flux_calibrator_estimate.size(); ichan++)
    setup_flux_calibrator (flux_calibrator_estimate[ichan]);
}


/*
  For each of Stokes Q, U & V
  If the calibrator polarization vector component is not a free parameter,
  then it should be fixed to a specified value
*/

void set_fixed_QUV ( Pulsar::SourceEstimate& cal, double value )
{
  for (unsigned ipol=1; ipol < 4; ipol++)
    if (!cal.source->get_infit (ipol))
      cal.source->set_Estimate (ipol, 0.0);
}

void Pulsar::ReceptionCalibrator::setup_poln_calibrator (SourceEstimate& est)
{
  Signal::Basis basis = get_calibrator()->get_basis ();

  for (unsigned istokes=0; istokes<4; istokes++)
    est.source->set_infit (istokes, false);

  // calibrator flux is unity by definition
  est.source->set_Estimate (0, 1.0);
    
  if (basis == Signal::Linear)
  {
    // degree of polarization (Stokes U) may vary
    est.source->set_infit (2, true);
    
    if (measure_cal_Q)
      est.source->set_infit (1, true);
  }
  else
  {
    // degree of polarization (Stokes Q) may vary
    est.source->set_infit (1, true);
  }
  
  if (measure_cal_V && has_fluxcal())
  {
    // Stokes V of the calibrator may vary
    est.source->set_infit (3, true);
  }

  set_fixed_QUV (est, 0.0);
}

void Pulsar::ReceptionCalibrator::setup_flux_calibrator (SourceEstimate& est)
{
  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular || measure_cal_V)
  {
    // Stokes V of Hydra may not vary
    est.source->set_infit (3, false);
  }

  set_fixed_QUV (est, 0.0);
}

bool Pulsar::ReceptionCalibrator::has_fluxcal () const
{
  return flux_calibrator_estimate.size();
}

void Pulsar::ReceptionCalibrator::submit_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 const SourceObservation& data
 )
{
  if (data.source != Signal::FluxCalOn)
  {
    SystemCalibrator::submit_calibrator_data (measurements, data);
    return;
  }

  submit_flux_calibrator_data (measurements, data.ichan, data.baseline);
}

void Pulsar::ReceptionCalibrator::submit_flux_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 unsigned ichan, const Stokes< Estimate<double> >& data
 ) try
{
  // add the flux calibrator data to the model constraints

  Calibration::CoherencyMeasurement state 
    (flux_calibrator_estimate[ichan].input_index);

  flux_calibrator_estimate[ichan].add_data_attempts ++;

  state.set_stokes( data );

  measurements.push_back (state);

  unsigned index = model[ichan]->get_fluxcal()->get_path_index();
  measurements.set_transformation_index (index);

  DEBUG ("ReceptionCalibrator::submit_flux_calibrator_data ichan=" << ichan);

  model[ichan]->get_equation()->add_data (measurements);
}
catch (Error& error)
{
  cerr << "Pulsar::ReceptionCalibrator::submit_flux_calibrator_data ichan="
       << ichan << " error\n" << error << endl;

  flux_calibrator_estimate[ichan].add_data_failures ++;
}

void Pulsar::ReceptionCalibrator::integrate_calibrator_data
(
 const Jones< Estimate<double> >& correct,
 const SourceObservation& data
 )
{
  Jones< Estimate<double> > use;
  if (previous)
    use = previous->get_response (data.ichan);
  else
    use = correct;

  if (data.source == Signal::FluxCalOn)
  {
    Stokes< Estimate<double> > result = transform( data.baseline, use );
    flux_calibrator_estimate.at(data.ichan).source_guess.integrate (result);

    if (multiple_flux_calibrators)
    {
      flux_calibrator_estimate[data.ichan].update_source ();
      setup_flux_calibrator (flux_calibrator_estimate[data.ichan]);
    }
  }

  SystemCalibrator::integrate_calibrator_data (use, data);
}

//! Add the ReferenceCalibrator observation to the set of constraints
void 
Pulsar::ReceptionCalibrator::add_calibrator (const ReferenceCalibrator* p) try 
{
  check_ready ("Pulsar::ReceptionCalibrator::add_calibrator");

  SystemCalibrator::add_calibrator (p);
}
catch (Error& error)
{
  throw error += "Pulsar::ReceptionCalibrator::add_calibrator";
}

void Pulsar::ReceptionCalibrator::solve ()
{
  if (!get_prepared())
    check_ready ("Pulsar::ReceptionCalibrator::solve");

  initialize ();
  SystemCalibrator::solve ();
}

void Pulsar::ReceptionCalibrator::export_prepare () const
{
  const_cast<ReceptionCalibrator*>(this)->initialize();
}

void Pulsar::ReceptionCalibrator::initialize ()
{
  if (calibrator_estimate.size() == 0)
  {
    cerr <<
      "Pulsar::ReceptionCalibrator::initialize WARNING: \n\t"
      "Without a ReferenceCalibrator observation, \n\t"
      "there remains a degeneracy along the Stokes V axis and \n\t"
      "an unconstrained scalar gain. \n"
      "\n\t"
      "Therefore, the boost along the Stokes V axis \n\t"
      "and the absolute gain will be fixed."
	 << endl;

    for (unsigned ichan=0; ichan<model.size(); ichan++)
      model[ichan]->no_reference_calibrators ();
  }

  /*
    Time variations are disengaged at the end of the call to solve.
    However, it is desireable to plot the time variation parameters
    after the solution is obtained.  The get_Info method calls this
    function before the model parameters are plotted; therefore, this
    method re-engages the time variations.
  */
  for (unsigned ichan=0; ichan<model.size(); ichan++)
    model[ichan]->engage_time_variations ();

  if (get_prepared())
    return;

  if (previous_cal)
  {
    cerr << "Pulsar::ReceptionCalibrator::initialize using previous solution"
	 << endl;
    for (unsigned ichan=0; ichan<model.size(); ichan++)
      calibrator_estimate[ichan].source
	-> set_stokes( (Stokes< Estimate<double> >)
		       previous_cal->get_stokes (ichan) );
  }
 
  SystemCalibrator::solve_prepare ();

  if (!multiple_flux_calibrators)
    for (unsigned ichan=0; ichan<flux_calibrator_estimate.size(); ichan++)
      flux_calibrator_estimate[ichan].update_source();

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    for (unsigned ichan=0; ichan<pulsar[istate].size(); ichan++)
      pulsar[istate][ichan].update_source ();

  /*
    The various calls to update_source can incorrectly reset values
    that should remain fixed because they are not free parameters
  */

  setup_calibrators ();
}

void Pulsar::ReceptionCalibrator::check_ready (const char* method, bool unc)
{
  if (get_solved())
    throw Error (InvalidState, method,
		 "Model has been solved. Cannot add data.");

  if (get_prepared())
    throw Error (InvalidState, method,
		 "Model has been initialized. Cannot add data.");

  if (unc && !has_calibrator())
    throw Error (InvalidState, method,
		 "Initial observation required.");
}

/*! Mask invalid SourceEstimate states */
void Pulsar::ReceptionCalibrator::valid_mask
(const std::vector<SourceEstimate>& src)
{
  if (src.size () != model.size())
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::valid_mask",
                 "src.size=%d != model.size=%d",
                 src.size (), model.size());

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    model[ichan]->set_valid( model[ichan]->get_valid() && src[ichan].valid );
}


void Pulsar::ReceptionCalibrator::solve_prepare ()
{
  SystemCalibrator::solve_prepare ();

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    if (output_report && model[ichan]->get_valid())
    {
      string report_name = "pcm_report_" + tostring(ichan) + ".txt";
      model[ichan]->get_equation()->get_solver()->add_acceptance_condition
	( Functor< bool(Calibration::ReceptionModel*) >
	  ( new Calibration::ReceptionModelReport (report_name),
	    &Calibration::ReceptionModelReport::report ) );
    }
}

Pulsar::Calibrator::Info* 
Pulsar::ReceptionCalibrator::new_info_pulsar (unsigned istate) const
{
  SourceInfo* info = new SourceInfo( pulsar[istate] );

  info->set_title( "Stokes Parameters of Phase Bin " 
		   + tostring(pulsar[istate][0].phase_bin) );

  return info;
}

