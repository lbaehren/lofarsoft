/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/BackendCorrection.h"
#include "Pulsar/BasisCorrection.h"
#include "Pulsar/ProjectionCorrection.h"

#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/InstrumentInfo.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Receiver.h"

#include "MEAL/Complex2Constant.h"
#include "MEAL/CongruenceTransformation.h"

#include "BatchQueue.h"
#include "Pauli.h"

#include <assert.h>

using namespace std;
using namespace Calibration;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  SystemCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::SystemCalibrator::SystemCalibrator (Archive* archive)
{
  set_initial_guess = true;

  is_prepared = false;
  is_solved = false;
  has_pulsar = false;

  retry_chisq = 0.0;
  invalid_chisq = 0.0;

  get_data_fail = 0;
  get_data_call = 0;

  report_projection = false;
  report_initial_state = false;

  if (archive)
    set_calibrator (archive);
}

void Pulsar::SystemCalibrator::set_calibrator (Archive* archive)
{
  if (!archive)
    return;

  PolnCalibrator::set_calibrator(archive);

  extension = archive->get<PolnCalibratorExtension>();
  calibrator_stokes = archive->get<CalibratorStokes>();
}

//! Return true if least squares minimization solvers are available
bool Pulsar::SystemCalibrator::has_solver () const
{
  return true;
}

//! Return the transformation for the specified channel
const Calibration::ReceptionModel::Solver* 
Pulsar::SystemCalibrator::get_solver (unsigned ichan) const
{
  check_ichan ("get_solver", ichan);
  return model[ichan]->get_equation()->get_solver();
}

//! Return the transformation for the specified channel
Calibration::ReceptionModel::Solver* 
Pulsar::SystemCalibrator::get_solver (unsigned ichan)
{
  assert (ichan < model.size());
  return model[ichan]->get_equation()->get_solver();
}

void 
Pulsar::SystemCalibrator::set_solver (Calibration::ReceptionModel::Solver* s)
{
  solver = s;
}

//! Copy constructor
Pulsar::SystemCalibrator::SystemCalibrator (const SystemCalibrator& calibrator)
{
}

//! Destructor
Pulsar::SystemCalibrator::~SystemCalibrator ()
{
}

Pulsar::Calibrator::Info*
Pulsar::SystemCalibrator::get_Info () const
{
  export_prepare ();

  return PolnCalibrator::get_Info ();
}

MJD Pulsar::SystemCalibrator::get_epoch () const
{
  return 0.5 * (start_epoch + end_epoch);
}

void Pulsar::SystemCalibrator::add_epoch (const MJD& epoch)
{
  if (epoch < start_epoch || start_epoch == MJD::zero)
    start_epoch = epoch;
  if (epoch > end_epoch || end_epoch == MJD::zero)
    end_epoch = epoch;
}

//! Get the total number of input polarization states
unsigned Pulsar::SystemCalibrator::get_nstate () const
{
  unsigned nstate = 0;

  for (unsigned i=0; i<model.size(); i++)
    nstate = std::max (nstate, model[i]->get_equation()->get_num_input ());

  return nstate;
}

//! Return true if the state index is a pulsar
unsigned Pulsar::SystemCalibrator::get_state_is_pulsar (unsigned istate) const
{
  return istate != calibrator_estimate.input_index;
}

//! Get the number of pulsar polarization states in the model
unsigned Pulsar::SystemCalibrator::get_nstate_pulsar () const
{
  return 0;
}
    
//! Retern a new plot information interface for the specified pulsar state
Pulsar::Calibrator::Info* 
Pulsar::SystemCalibrator::new_info_pulsar (unsigned istate) const
{
  throw Error (InvalidState, "Pulsar::SystemCalibrator::new_info_pulsar",
	       "not implemented");
}

unsigned Pulsar::SystemCalibrator::get_nchan () const
{
  unsigned nchan = 0;

  if (has_calibrator())
    nchan = get_calibrator()->get_nchan ();

  if (model.size())
    nchan = model.size ();

  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::get_nchan " << nchan << endl;

  return nchan;
}

unsigned Pulsar::SystemCalibrator::get_ndata (unsigned ichan) const
{
  check_ichan ("get_ndata", ichan);
  return model[ichan]->get_equation()->get_ndata ();
}

using namespace MEAL;

void Pulsar::SystemCalibrator::set_impurity( MEAL::Real4* f )
{
  impurity = f;
}

void Pulsar::SystemCalibrator::set_foreach_calibrator (const MEAL::Complex2* x)
{
  foreach_calibrator = x;
}

void Pulsar::SystemCalibrator::set_gain( Univariate<Scalar>* f )
{
  gain_variation = f;
}

void Pulsar::SystemCalibrator::set_diff_gain( Univariate<Scalar>* f )
{
  diff_gain_variation = f;
}

void Pulsar::SystemCalibrator::set_diff_phase( Univariate<Scalar>* f )
{
  diff_phase_variation = f;
}

void Pulsar::SystemCalibrator::preprocess (Archive* data)
{
  if (data->get_type() == Signal::Pulsar)
  {
    BackendCorrection correct_backend;
    if( correct_backend.required (data) )
    {
      if (verbose)
	cerr << "Pulsar::SystemCalibrator::preprocess correct backend" << endl;
      correct_backend (data);
    }
  }
}

//! Add the observation to the set of constraints
void Pulsar::SystemCalibrator::add_observation (const Archive* data) try
{
  if (!data)
    return;
  
  if (data->get_type() == Signal::Pulsar)
    add_pulsar (data);
  else
    add_calibrator (data);
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::add_observation";
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::add_pulsar (const Archive* data) try
{
  if (verbose)
    cerr << "Pulsar::SystemCalibrator::add_pulsar"
      " data->nchan=" << data->get_nchan() << endl;

  prepare (data);

  unsigned nsub = data->get_nsubint ();

  get_data_fail = 0;
  get_data_call = 0;

  for (unsigned isub=0; isub<nsub; isub++)
    add_pulsar( data, isub );

  if (get_data_fail)
    cerr << "\t" << get_data_fail << " failures in " << get_data_call
	 << " data points" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::add_pulsar";
}

void Pulsar::SystemCalibrator::prepare (const Archive* data) try
{
  match (data);

  if (model.size() == 0)
    create_model ();

  has_pulsar = true;

  load_calibrators ();
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::prepare";
}

//! Add the specified pulsar observation to the set of constraints
void 
Pulsar::SystemCalibrator::add_pulsar (const Archive* data, unsigned isub) try
{
  const Integration* integration = data->get_Integration (isub);
  unsigned nchan = integration->get_nchan ();

  if (model.size() != 1 && nchan != model.size())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::add_pulsar",
		 "input data nchan=%d != model nchan=%d", nchan, model.size());

  MJD epoch = integration->get_epoch ();
  add_epoch (epoch);

  // use the ProjectionCorrection class to calculate the transformation
  ProjectionCorrection correction;
  correction.set_archive (data);
  Jones<double> projection = correction (isub);

  if (report_projection)
    cerr << correction.get_summary () << endl;

  // an identifier for this set of data
  string identifier = data->get_filename() + " " + tostring(isub);

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::add_pulsar identifier="
	 << identifier << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++) try
  {
    if (integration->get_weight (ichan) == 0)
    {
      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_pulsar ichan="
	     << ichan << " flagged invalid" << endl;
      continue;
    }
    
    unsigned mchan = ichan;
    if (model.size() == 1)
      mchan = 0;

    using MEAL::Argument;
    
    // epoch abscissa
    Argument::Value* time = model[mchan]->time.new_Value( epoch );
    
    // projection transformation
    Argument::Value* xform = model[mchan]->projection.new_Value( projection );
    
    // pulsar signal path
    unsigned path = model[mchan]->get_pulsar_path();
    
    // measurement set
    Calibration::CoherencyMeasurementSet measurements (path);
    
    measurements.set_identifier( identifier );
    measurements.add_coordinate( time );
    measurements.add_coordinate( xform );
    measurements.set_coordinates();
    
    try
    {
      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_pulsar call add_pulsar ichan="
	     << ichan << endl;
  
      add_pulsar (measurements, integration, ichan);
    }
    catch (Error& error)
    {
      if (verbose > 2 || error.get_code() != InvalidParam)
        cerr << "Pulsar::SystemCalibrator::add_pulsar error" << error << endl;
    }
    
    model[mchan]->add_observation_epoch (epoch);
    
  }
  catch (Error& error)
  {
    cerr << "Pulsar::SystemCalibrator::add_pulsar ichan="
	 << ichan << " error\n" << error.get_message() << endl;
  }
  
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::add_pulsar subint";
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::match (const Archive* data)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::match",
		 "No Archive containing pulsar data has yet been added");

  string reason;
  if (!get_calibrator()->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::SystemCalibrator::match",
		 "'" + data->get_filename() + "' does not match "
		 "'" + get_calibrator()->get_filename() + reason);
}

void Pulsar::SystemCalibrator::set_calibrators (const vector<string>& n)
{
  calibrator_filenames = n;
}

void Pulsar::SystemCalibrator::load_calibrators ()
{
  if (calibrator_filenames.size() == 0)
    return;

  for (unsigned ifile = 0; ifile < calibrator_filenames.size(); ifile++) try
  {
    cerr << "Pulsar::SystemCalibrator::load_calibrators loading\n\t"
	 << calibrator_filenames[ifile] << endl;

    Reference::To<Archive> archive;
    archive = Pulsar::Archive::load(calibrator_filenames[ifile]);
    add_calibrator (archive);    
  }
  catch (Error& error)
  {
    cerr << "Pulsar::SystemCalibrator::load_calibrators" << error << endl;
  }

  calibrator_filenames.resize (0);

  unsigned nchan = model.size();

  cerr << "Setting " << nchan << " channel receiver" << endl;

  try
  {
    for (unsigned ichan=0; ichan<nchan; ichan++)
      model[ichan]->update ();
  }
  catch (Error& error)
  {
    throw error += "Pulsar::SystemCalibrator::load_calibrators";
  }

  if (previous && previous->get_nchan() == nchan)
  {
    cerr << "Using previous solution" << endl;
    set_initial_guess = false;
    for (unsigned ichan=0; ichan<nchan; ichan++)
      if (previous->get_transformation_valid(ichan))
        model[ichan]->copy_transformation(previous->get_transformation(ichan));
  }
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::add_calibrator (const Archive* data)
{
  if (!has_pulsar)
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibrator::add_calibrator"
	" postponed until pulsar added" << endl;

    calibrator_filenames.push_back( data->get_filename() );

    return;
  }

  if (!receiver)
    receiver = data->get<Receiver>();

  try
  {
    Reference::To<ReferenceCalibrator> polncal;

    if (type->is_a<CalibratorTypes::van09_Eq>())
    {
      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_calibrator"
	  " new PolarCalibrator" << endl;
    
      polncal = new PolarCalibrator (data);
    }
    else
    {
      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_calibrator"
	  " new SingleAxisCalibrator" << endl;
      
      polncal = new SingleAxisCalibrator (data);
    }

    polncal->set_nchan( get_calibrator()->get_nchan() );

    add_calibrator (polncal);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::SystemCalibrator::add_calibrator (Archive*)";
  }
}

//! Add the ReferenceCalibrator observation to the set of constraints
void 
Pulsar::SystemCalibrator::add_calibrator (const ReferenceCalibrator* p) try 
{
  unsigned nchan = get_nchan ();

  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::add_calibrator nchan=" << nchan << endl;

  if (!nchan)
    throw Error (InvalidState, "Pulsar::SystemCalibrator::add_calibrator",
		 "nchan == 0");

  const Archive* cal = p->get_Archive();

  if (cal->get_state() != Signal::Coherence)
    throw Error (InvalidParam, "Pulsar::SystemCalibrator::add_calibrator",
		 "Archive='" + cal->get_filename() + "' "
		 "invalid state=" + State2string(cal->get_state()));

  if ( cal->get_type() != Signal::FluxCalOn && 
       cal->get_type() != Signal::PolnCal )
    throw Error (InvalidParam, "Pulsar::SystemCalibrator::add_calibrator",
                 "invalid source=" + Source2string(cal->get_type()));

  string reason;
  if (!get_calibrator()->calibrator_match (cal, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_calibrator",
		 "mismatch between \n\t" 
		 + get_calibrator()->get_filename() +
                 " and \n\t" + cal->get_filename() + reason);

  unsigned nsub = cal->get_nsubint();
  unsigned npol = cal->get_npol();
  
  assert (npol == 4);

  Signal::Source source = cal->get_type();

  if (model.size() == 0)
    create_model ();

  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::add_calibrator prepare calibrator" << endl;

  prepare_calibrator_estimate( source );

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  epoch_added = vector<bool> (nchan, false);

  // ensure that model array is large enough
  check_ichan ("add_calibrator", nchan - 1);

  for (unsigned isub=0; isub<nsub; isub++)
  {
    const Integration* integration = cal->get_Integration (isub);

    MJD epoch = integration->get_epoch();
    add_epoch( epoch );

    ReferenceCalibrator::get_levels (integration, nchan, cal_hi, cal_lo);

    string identifier = cal->get_filename() + " " + tostring(isub);

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (integration->get_weight (ichan) == 0 || !model[ichan]->valid)
      {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::add_calibrator ichan="
	       << ichan << " flagged invalid" << endl;
	continue;
      }

      // transpose [ipol][ichan] output of ReferenceCalibrator::get_levels
      vector< Estimate<double> > calibtor (npol);
      vector< Estimate<double> > baseline (npol);

      for (unsigned ipol = 0; ipol<npol; ipol++)
      {
	calibtor[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];
	baseline[ipol] = cal_lo[ipol][ichan];
      }

      SourceObservation data;

      data.source = source;
      data.epoch = epoch;
      data.ichan = ichan;

      // convert to Stokes parameters
      data.observation = coherency( convert (calibtor) );
      data.baseline = coherency( convert (baseline) );

      try
      {
	Calibration::CoherencyMeasurementSet measurements;

	measurements.set_identifier( identifier );
	measurements.add_coordinate( model[ichan]->time.new_Value(epoch) );

        // convert to CoherencyMeasurement format
        Calibration::CoherencyMeasurement 
	  state (calibrator_estimate.input_index);

	state.set_stokes( data.observation );
        measurements.push_back( state );

	submit_calibrator_data (measurements, data);
      }
      catch (Error& error)
      {
        cerr << "Pulsar::SystemCalibrator::add_calibrator ichan="
             << ichan << " error\n" << error << endl;
	continue;
      }

      integrate_calibrator_data( p->get_response(ichan), data );

      if (p->get_nchan() == nchan && p->get_transformation_valid (ichan))
      {
	Signal::Source source = p->get_Archive()->get_type();
	model[ichan]->integrate_calibrator (p->get_transformation(ichan), 
					    source == Signal::FluxCalOn);
      }
    }
  }
}
catch (Error& error) 
{
  throw error +=
    "Pulsar::SystemCalibrator::add_calibrator (ReferenceCalibrator*)";
}

void Pulsar::SystemCalibrator::init_estimate (SourceEstimate& estimate)
{
  unsigned nchan = get_nchan ();
  unsigned nbin = get_calibrator()->get_nbin ();

  if (estimate.phase_bin >= nbin)
    throw Error (InvalidRange, "Pulsar::SystemCalibrator::init_estimate",
		 "phase bin=%d >= nbin=%d", estimate.phase_bin, nbin);

  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::init_estimate"
            " nchan=" << nchan << " nbin=" << nbin << endl;

  estimate.source.resize (nchan);
  estimate.source_guess.resize (nchan);

  bool first_channel = true;

  check_ichan ("init_estimate", nchan - 1);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->valid)
      continue;

#if 0
    if (physical_coherency)
      estimate.source[ichan] = new MEAL::PhysicalCoherency;
    else
#endif
      estimate.source[ichan] = new MEAL::Coherency;

    string prefix = "psr_" + tostring(estimate.phase_bin) + "_";
    estimate.source[ichan]->set_param_name_prefix( prefix );

    unsigned nsource = model[ichan]->get_equation()->get_num_input();

    if (first_channel)
      estimate.input_index = nsource;

    else if (estimate.input_index != nsource)
      throw Error (InvalidState, "Pulsar::SystemCalibrator::init_estimate",
		   "isource=%d != nsource=%d (ichan=%d)",
		   estimate.input_index, nsource, ichan);

    model[ichan]->get_equation()->add_input( estimate.source[ichan] );

    first_channel = false;
  }
}

void Pulsar::SystemCalibrator::prepare_calibrator_estimate ( Signal::Source s )
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::prepare_calibrator_estimate" << endl;

  if (calibrator_estimate.source.size() == 0)
    create_calibrator_estimate();
}

void Pulsar::SystemCalibrator::create_calibrator_estimate ()
{
  if (verbose)
    cerr << "Pulsar::SystemCalibrator::create_calibrator_estimate" << endl;

  // add the calibrator states to the equations
  init_estimate (calibrator_estimate);

  // set the initial guess and fit flags
  Stokes<double> cal_state (1,0,.5,0);

  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular)
    cal_state = Stokes<double> (1,.5,0,0);

  unsigned nchan = get_nchan ();

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (calibrator_estimate.source[ichan])
    {   
      calibrator_estimate.source[ichan]->set_stokes( cal_state );
      calibrator_estimate.source[ichan]->set_infit( 0, false );

      calibrator_estimate.source[ichan]->set_param_name_prefix( "cal_" );
    }
}

void Pulsar::SystemCalibrator::submit_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 const SourceObservation& data
 )
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::submit_calibrator_data ichan="
	 << data.ichan << endl;

  check_ichan ("subit_calibrator_data", data.ichan);

  if (!epoch_added[data.ichan])
  {
    model[data.ichan]->add_calibrator_epoch (data.epoch);
    epoch_added[data.ichan] = true;
  }

  measurements.set_transformation_index
    ( model[data.ichan]->get_polncal_path() );

  model[data.ichan]->get_equation()->add_data (measurements);
}

void Pulsar::SystemCalibrator::integrate_calibrator_data
(
 const Jones< Estimate<double> >& correct,
 const SourceObservation& data
 )
{
  Stokes< Estimate<double> > result = transform( data.observation, correct );

  assert( data.ichan < calibrator_estimate.source_guess.size() );

  calibrator_estimate.source_guess[data.ichan].integrate (result);
}


Pulsar::CalibratorStokes*
Pulsar::SystemCalibrator::get_CalibratorStokes () const
{
  if (calibrator_stokes)
    return calibrator_stokes;

  if (verbose > 2) cerr << "Pulsar::SystemCalibrator::get_CalibratorStokes"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_nchan ();

  if (nchan != calibrator_estimate.source.size())
    throw Error (InvalidState,
		 "Pulsar::SystemCalibrator::get_CalibratorStokes",
		 "Calibrator Stokes nchan=%d != Transformation nchan=%d",
		 calibrator_estimate.source.size(), nchan);

  Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
  ext->set_nchan (nchan);
    
  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    bool valid = get_transformation_valid(ichan);
      
    ext->set_valid (ichan, valid);
    if (!valid)
      continue;
    
    ext->set_stokes (ichan, calibrator_estimate.source[ichan]->get_stokes());
  }
  catch (Error& error)
  {
    cerr << "Pulsar::SystemCalibrator::get_CalibratorStokes ichan="
	 << ichan << " error\n" << error.get_message() << endl;
    ext->set_valid (ichan, false);
  }

  calibrator_stokes = ext;
  
  return calibrator_stokes;

}

void Pulsar::SystemCalibrator::create_model ()
{
  if (!receiver)
    receiver = get_calibrator()->get<Receiver>();

  MEAL::Complex2* basis = 0;

  if (receiver)
  {
    Pauli::basis().set_basis( receiver->get_basis() );
    
    if (!receiver->get_basis_corrected())
    {
      BasisCorrection basis_correction;
      basis = new MEAL::Complex2Constant( basis_correction(receiver) );

      if (verbose)
	cerr << "Pulsar::SystemCalibrator::create_model basis corrections:\n"
	     << basis_correction.get_summary () << endl
	     << "Pulsar::SystemCalibrator::create_model receiver=\n  " 
	     << basis->evaluate() << endl;
    }
  }

  unsigned nchan = get_nchan ();
  model.resize (nchan);

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::create_model nchan=" << nchan << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::create_model ichan=" << ichan << endl;

    model[ichan] = new Calibration::StandardModel (type);

    if (basis)
      model[ichan]->set_basis (basis);

    init_model( ichan );
  }

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::create_model exit" << endl;
}

void Pulsar::SystemCalibrator::init_model (unsigned ichan)
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::init_model ichan=" << ichan << endl;

  if (impurity)
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::init_model impurity" << endl;
    model[ichan]->set_impurity( impurity->clone() );
  }

  if (foreach_calibrator)
    model[ichan]->set_foreach_calibrator( foreach_calibrator );

  if (gain_variation)
    model[ichan]->set_gain( gain_variation->clone() );

  if (diff_gain_variation)
    model[ichan]->set_diff_gain( diff_gain_variation->clone() );
  
  if (diff_phase_variation)
    model[ichan]->set_diff_phase( diff_phase_variation->clone() );
  
  if (solver)
    model[ichan]->set_solver( solver->clone() );

  if (report_initial_state)
  {
    string filename = "prefit_model_" + tostring(ichan) + ".txt";
    get_solver(ichan)->set_prefit_report (filename);
  }
}

//! Return the StandardModel for the specified channel
const Calibration::StandardModel*
Pulsar::SystemCalibrator::get_model (unsigned ichan) const
{
  check_ichan ("get_model", ichan);
  return model[ichan];
}

void Pulsar::SystemCalibrator::set_nthread (unsigned nthread)
{
  queue.resize (nthread);
}

void
Pulsar::SystemCalibrator::set_equation_configuration (const vector<string>& c)
{
  equation_configuration = c;
}

void Pulsar::SystemCalibrator::configure ( MEAL::Function* equation ) try
{
  if (equation_configuration.size() == 0)
    return;

  Reference::To<TextInterface::Parser> interface = equation ->get_interface();
  for (unsigned i=0; i<equation_configuration.size(); i++)
    interface->process (equation_configuration[i]);
}
catch (Error& error)
{
  cerr << "Pulsar::SystemCalibrator::configure " << error << endl;

  unsigned nparam = equation->get_nparam();
  cerr << "Pulsar::SystemCalibrator::configure nparam=" << nparam << endl;
  for (unsigned i=0; i<nparam; i++)
    cerr << i << " name=" << equation->get_param_name(i) << endl;

  exit (-1);
}

void Pulsar::SystemCalibrator::solve_prepare ()
{
  if (is_prepared)
    return;

  if (set_initial_guess)
    calibrator_estimate.update_source();

  MJD epoch = get_epoch();

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::solve_prepare epoch=" << epoch << endl;

  for (unsigned ichan=0; ichan<model.size(); ichan++)
  {
    if (model[ichan]->get_equation()->get_ndata() == 0)
    {
      if (verbose)
	cerr << "Pulsar::SystemCalibrator::solve_prepare warning"
	  " ichan=" << ichan << " has no data" << endl;
      model[ichan]->valid = false;
    }

    if (!model[ichan]->valid)
      continue;

    if (ichan < calibrator_estimate.source.size())
    {
      // sanity check
      Estimate<double> I = calibrator_estimate.source[ichan]->get_stokes()[0];
      if (fabs(I.get_value()-1.0) > I.get_error() && verbose)
	cerr << "Pulsar::SystemCalibrator::solve_prepare warning"
	  " ichan=" << ichan << " reference flux=" << I << " != 1" << endl;
    }

    model[ichan]->set_reference_epoch ( epoch );

    configure (model[ichan]->get_equation());

    model[ichan]->check_constraints ();
    
    if (set_initial_guess)
      model[ichan]->update ();
    
    if (verbose > 2)
      get_solver(ichan)->set_debug();
  }

  is_prepared = true;
}

float Pulsar::SystemCalibrator::get_reduced_chisq (unsigned ichan) const
{
  check_ichan ("get_reduced_chisq", ichan);

  if (!model[ichan]->valid)
    return 0.0;

  const ReceptionModel* equation = model[ichan]->get_equation();

  float chisq = equation->get_solver()->get_chisq ();
  unsigned free = equation->get_solver()->get_nfree ();

  return chisq/free;
}

void Pulsar::SystemCalibrator::solve ()
{
  ReceptionModel::Solver::report_chisq = true;

  solve_prepare ();

  unsigned nchan = get_nchan ();

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->valid)
    {
      cerr << "channel " << ichan << " flagged invalid" << endl;
      continue;
    }

    queue.submit( model[ichan].get(), &StandardModel::solve );
  }

  queue.wait ();

  unsigned resolve_singular = 1;

  while (resolve_singular)
  {
    resolve_singular = 0;

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!get_solver(ichan)->get_singular())
	continue;

      if (model[ichan]->reduce_nfree())
      {
	cerr << "retry singular channel " << ichan << endl;
	resolve (ichan);
	resolve_singular ++;
      }
    }

    queue.wait ();
  }

  if (retry_chisq > 0.0)
  { 
    // attempt to fix up any channels that didn't converge well
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan]->valid)
	continue;

      float reduced_chisq = get_reduced_chisq (ichan);

      if (reduced_chisq > retry_chisq)
      {
	cerr << "try for better fit in ichan=" << ichan 
	     << " chisq/nfree=" << reduced_chisq << endl;

	resolve (ichan);
      }
    }

    queue.wait ();
  }

  if (invalid_chisq > 0.0)
  {
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan]->valid)
	continue;

      float reduced_chisq = get_reduced_chisq (ichan);

      if (reduced_chisq > invalid_chisq)
      {
	cerr << "invalid fit in ichan=" << ichan 
	     << " chisq/nfree=" << reduced_chisq << endl;

	model[ichan]->valid = false;
      }
    }
  }

  covariance.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    if (!get_solver(ichan)->get_solved())
      model[ichan]->valid = false;

    if (!model[ichan]->valid)
      continue;

    model[ichan]->get_covariance( covariance[ichan], get_epoch() );
  }
  catch (Error& error)
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibrator::solve get_covariance error"
	   << error << endl;
    model[ichan]->valid = false;
  }

  // ensure that calculate_transformation is called again
  transformation.resize (0);

  is_solved = true;
}

void Pulsar::SystemCalibrator::resolve (unsigned ichan) try
{
  unsigned nchan = get_nchan ();

  // look for the nearest neighbour with a solution
  for (int off=1; off < int(nchan); off++)
  {
    for (int dir=-1; dir <= 1; dir+=2)
    {
      int jchan = int(ichan) + dir * off;

      if (jchan < 0 || jchan >= int(nchan))
	continue;

#ifdef _DEBUG
      cerr << "testing " << jchan << " ... ";
#endif

      if (!model[jchan]->valid)
      {
#ifdef _DEBUG
        cerr << "not valid" << endl;
#endif
	continue;
      }

      ReceptionModel* equation = model[jchan]->get_equation();

      if (!equation->get_solver()->get_solved())
      {
#ifdef _DEBUG
        cerr << "not solved" << endl;
#endif
	continue;
      }

      float chisq = equation->get_solver()->get_chisq ();
      unsigned free = equation->get_solver()->get_nfree ();
      float reduced_chisq = chisq/free;

      if (reduced_chisq > retry_chisq)
      {
#ifdef _DEBUG
        cerr << "not good; reduced chisq=" << reduced_chisq << endl;
#endif
	continue;
      }

      cerr << "copying solution from jchan=" << jchan 
	   << " chisq/nfree=" << reduced_chisq << endl;

      model[ichan]->get_equation()->copy_fit( equation );      
      queue.submit( model[ichan].get(), &StandardModel::solve );

      return;
    }
  }

  if (get_solver(ichan)->get_singular())
  {
    queue.submit( model[ichan].get(), &StandardModel::solve );
  }
  else
    cerr << "could not find a suitable solution to copy for retry" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::resolve";
}

bool Pulsar::SystemCalibrator::get_prepared () const
{
  return is_prepared;
}

bool Pulsar::SystemCalibrator::get_solved () const
{
  return is_solved;
}

/*! Retrieves the transformation from the standard model in each channel */
void Pulsar::SystemCalibrator::calculate_transformation ()
{
  unsigned nchan = get_nchan ();

  transformation.resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    transformation[ichan] = 0;

    assert (ichan < model.size());

    if (model[ichan]->valid)
      transformation[ichan] = model[ichan]->get_transformation();   
  }
}

//! Calibrate the polarization of the given archive
void Pulsar::SystemCalibrator::precalibrate (Archive* data)
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::precalibrate" << endl;

  string reason;
  if (!get_calibrator()->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::precalibrate",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + data->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (nchan != model.size() && model.size() != 1)
    throw Error (InvalidState, "Pulsar::SystemCalibrator::precalibrate",
                 "model size=%u != data nchan=%u", model.size(), nchan);

  vector< Jones<float> > response (nchan);

  // use the ProjectionCorrection class to calculate the transformation
  ProjectionCorrection correction;
  correction.set_archive (data);

  bool projection_corrected = false;

  BackendCorrection correct_backend;
  correct_backend (data);

  for (unsigned isub=0; isub<nsub; isub++)
  {
    Integration* integration = data->get_Integration (isub);

    if (correction.required (isub))
      projection_corrected = true;

    Jones<double> projection = correction (isub);

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      assert (ichan < model.size());

      if (!model[ichan]->valid)
      {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan 
	       << " zero weight" << endl;

	integration->set_weight (ichan, 0.0);

	response[ichan] = Jones<double>::identity();
	continue;
      }

      /*
	remove the projection from the signal path;
	it will be included later, if necessary.
      */

      model[ichan]->projection.set_value( Jones<double>::identity() );

      try
      {
	response[ichan] = get_transformation(data, isub, ichan)->evaluate();
      }
      catch (Error& error)
      {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan
	       << endl << error.get_message() << endl;

        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();

	continue;
      }

      if ( norm(det( response[ichan] )) < 1e-9 )
      {
        if (verbose > 2)
          cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan
               << " faulty response" << endl;

        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();

	continue;
      }

      if ( data->get_type() == Signal::Pulsar )
	response[ichan] *= projection;

      response[ichan] = inv( response[ichan] );
    }

    integration->expert()->transform (response);
  }

  data->set_poln_calibrated (true);
  data->set_scale (Signal::ReferenceFluxDensity);

  Receiver* receiver = data->get<Receiver>();

  if (!receiver)
  {
    cerr << "Pulsar::SystemCalibrator::precalibrate WARNING: "
      "cannot record corrections" << endl;
    return;
  }

  receiver->set_projection_corrected (projection_corrected);
  receiver->set_basis_corrected (true);
}



MEAL::Complex2* 
Pulsar::SystemCalibrator::get_transformation (const Archive* data,
					      unsigned isubint, unsigned ichan)
{
  const Integration* integration = data->get_Integration (isubint);
  ReceptionModel* equation = model[ichan]->get_equation();

  MEAL::Transformation<Complex2>* signal_path = 0;

  switch ( data->get_type() )
  {
  case Signal::Pulsar:
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::get_transformation Pulsar" << endl;
    equation->set_transformation_index (model[ichan]->get_pulsar_path());
    signal_path = equation->get_transformation ();
    break;

  case Signal::PolnCal:
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::get_transformation PolnCal" << endl;
    equation->set_transformation_index (model[ichan]->get_polncal_path());
    signal_path = equation->get_transformation ();
    break;

  case Signal::FluxCalOn:
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::get_transformation FluxCal" << endl;
    equation->set_transformation_index (model[ichan]->get_fluxcal_path());
    signal_path = equation->get_transformation ();
    break;

  default:
    throw Error (InvalidParam, "Pulsar::SystemCalibrator::get_transformation",
		 "unknown Archive type for " + data->get_filename() );
    
  }

  MEAL::CongruenceTransformation* congruence = 0;
  if (signal_path)
    congruence = dynamic_cast<MEAL::CongruenceTransformation*>(signal_path);

  if (!congruence)
    throw Error (InvalidState, "Pulsar::SystemCalibrator::get_transformation",
		 "measurement equation is not a congruence transformation");

  model[ichan]->time.set_value( integration->get_epoch() );
  return congruence->get_transformation();
}

Pulsar::Archive*
Pulsar::SystemCalibrator::new_solution (const string& class_name) const try
{
  if (verbose > 2) cerr << "Pulsar::SystemCalibrator::new_solution"
    " create CalibratorStokes Extension" << endl;

  Reference::To<Archive> output = Calibrator::new_solution (class_name);

  // get rid of the pulsar attributes
  output->set_rotation_measure (0.0);
  output->set_dispersion_measure (0.0);
  output->set_ephemeris (0);
  output->set_model (0);

  if (calibrator_estimate.source.size())
  {
    Reference::To<CalibratorStokes> stokes = get_CalibratorStokes();
    output -> add_extension (stokes);
  }

  if (receiver)
    output -> add_extension (receiver->clone());

  return output.release();
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::new_solution";
}

void Pulsar::SystemCalibrator::set_retry_reduced_chisq (float reduced_chisq)
{
  retry_chisq = reduced_chisq;
}

void Pulsar::SystemCalibrator::set_invalid_reduced_chisq (float reduced_chisq)
{
  invalid_chisq = reduced_chisq;
}

void Pulsar::SystemCalibrator::set_report_projection (bool flag)
{
  report_projection = flag;
}

void Pulsar::SystemCalibrator::set_report_initial_state (bool flag)
{
  report_initial_state = flag;
}

void Pulsar::SystemCalibrator::check_ichan (const char* name, unsigned ichan)
const
{
  if (ichan >= model.size())
    throw Error (InvalidRange, "Pulsar::SystemCalibrator::" + string(name),
		 "ichan=%u >= nchan=%u", ichan, model.size());
}

