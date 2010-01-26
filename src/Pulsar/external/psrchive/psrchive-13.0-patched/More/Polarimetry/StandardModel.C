/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardModel.h"
#include "Pulsar/SingleAxis.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Instrument.h"
#include "Pulsar/Britton2000.h"

#include "Pulsar/ReceptionModelSolver.h"

#include "MEAL/Polar.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Gain.h"
#include "MEAL/Steps.h"
#include "MEAL/Complex2Value.h"
#include "MEAL/JonesMueller.h"

#include <iostream>
#include <assert.h>

using namespace std;
using namespace MEAL;

// #define _DEBUG 1

bool Calibration::StandardModel::verbose = false;


Calibration::StandardModel::StandardModel (Pulsar::Calibrator::Type* _type)
{
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the model of the instrument
  //

  type = _type;

  Pulsar_path = 0;
  ReferenceCalibrator_path = 0;
  FluxCalibrator_path = 0;

  valid = true;
  built = false;

  time_variations_engaged = true;
  constant_pulsar_gain = false;

  time.signal.connect (&convert, &Calibration::ConvertMJD::set_epoch);
}

void Calibration::StandardModel::set_impurity (MEAL::Real4* x)
{
  impurity = x;
}

void Calibration::StandardModel::set_basis (MEAL::Complex2* x)
{
  basis = x;

  // if the instrument has already been constructed, add the basis to it
  if (instrument && basis)
    *instrument *= basis;
}

void Calibration::StandardModel::set_solver (ReceptionModel::Solver* s)
{
  solver = s;
  if (equation)
    equation->set_solver (s);
}

//! Set true when the pulsar Stokes parameters have been normalized
void Calibration::StandardModel::set_constant_pulsar_gain (bool value)
{
  constant_pulsar_gain = value;

  if (!constant_pulsar_gain)
    return;

  if (!built)
    return;

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    return;

  Scalar* function 
    = const_cast<Scalar*>( physical->get_gain_variation() );

  if (function)
  {
    if (verbose)
      cerr << "Calibration::StandardModel::set_constant_pulsar_gain"
	" disabling gain variation" << endl;
      
    if (pcal_gain)
    {
      if (verbose)
	cerr << "Calibration::StandardModel::set_constant_pulsar_gain"
	  " transfer to pcal gain" << endl;
      pcal_gain_chain->set_constraint (0, function);
    }

    function = 0;
    physical->set_gain (function);
  }

  physical->set_gain (1.0);
  physical->set_infit (0, false);
}

//! Get the measurement equation solver
Calibration::ReceptionModel* Calibration::StandardModel::get_equation ()
{
  if (!built)
    build ();

  return equation;
}

void Calibration::StandardModel::set_equation (Calibration::ReceptionModel* e)
{
  if (equation)
    throw Error (InvalidState, "Calibration::StandardModel::set_equation",
		 "equation already set; cannot be reset after construction");

#ifdef _DEBUG
  cerr << "Calibration::StandardModel::set_equation " << e << endl;
#endif

  equation = e;
}

//! Get the signal path experienced by the pulsar
const MEAL::Complex2*
Calibration::StandardModel::get_transformation () const
{
  if (!built)
    const_build ();

  return response;
}

const MEAL::Complex2*
Calibration::StandardModel::get_pulsar_transformation () const
{
  if (!built)
    const_build ();

  return pulsar_path;
}

MEAL::Complex2*
Calibration::StandardModel::get_transformation ()
{
  const StandardModel* thiz = this;
  return const_cast<MEAL::Complex2*>( thiz->get_transformation() );
}

void Calibration::StandardModel::const_build () const
{
  const_cast<StandardModel*>(this)->build();
}

void Calibration::StandardModel::add_transformation (MEAL::Complex2* xform)
{
  if (!impurity)
  {
    equation->add_transformation (xform);
    return;
  }

  Reference::To< MEAL::ProductRule<MEAL::Real4> > combination;
  combination = new MEAL::ProductRule<MEAL::Real4>;

  combination->add_model( impurity );
  combination->add_model( new MEAL::JonesMueller (xform) );

  equation->add_transformation( combination );
}

void Calibration::StandardModel::build ()
{
  if (built)
    return;

  if (verbose)
    cerr << "Calibration::StandardModel using " << type->get_name() << endl;

  response = Pulsar::new_transformation (type);

  instrument = new MEAL::ProductRule<MEAL::Complex2>;

  *instrument *= response;

  if (basis)
    *instrument *= basis;

  if (constant_pulsar_gain)
    instrument->set_infit (0, false);

  // the known transformation from the source to the receptors
  MEAL::Complex2Value* sky_to_receptors = new MEAL::Complex2Value;
  projection.signal.connect (sky_to_receptors,
			     &MEAL::Complex2Value::set_value);

  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the pulsar
  //

  pulsar_path = new MEAL::ProductRule<MEAL::Complex2>;

  *pulsar_path *= instrument;
  *pulsar_path *= sky_to_receptors;

  if (!equation)
  {
    if (verbose)
      cerr << "Calibration::StandardModel::build new ReceptionModel" << endl;
    equation = new Calibration::ReceptionModel;
  }

  add_transformation ( pulsar_path );

  Pulsar_path = equation->get_transformation_index ();

  if (verbose)
    cerr << "Calibration::StandardModel::build pulsar path="
	 << Pulsar_path << endl;

  if (solver)
    equation->set_solver( solver );

  built = true;
}

void Calibration::StandardModel::add_fluxcal_backend ()
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState,
		 "Calibration::StandardModel::add_fluxcal_backend",
		 "Cannot model flux calibrator with Hamaker model");

  MEAL::ProductRule<MEAL::Complex2>* path = 0;
  path = new MEAL::ProductRule<MEAL::Complex2>;

  fluxcal_backend = new Calibration::SingleAxis;

  *path *= fluxcal_backend;
  *path *= physical->get_frontend();

  if (basis)
    *path *= basis;

  add_transformation ( path );
  FluxCalibrator_path = equation->get_transformation_index ();
}

void
Calibration::StandardModel::set_foreach_calibrator (const MEAL::Complex2* x)
{
  foreach_pcal = x;
}


void Calibration::StandardModel::add_polncal_backend ()
{
  if (!built)
    build ();

  Reference::To< MEAL::ProductRule<MEAL::Complex2> > pcal_path;
  pcal_path = new MEAL::ProductRule<MEAL::Complex2>;

  if (constant_pulsar_gain)
  {
    if (!pcal_gain)
    {
      pcal_gain = new MEAL::Gain<MEAL::Complex2>;
      pcal_gain_chain = new MEAL::ChainRule<MEAL::Complex2>;
      pcal_gain_chain->set_model( pcal_gain );
      if (gain)
	pcal_gain_chain->set_constraint( 0, gain );
    }

    *pcal_path *= pcal_gain_chain;
  }

  if (foreach_pcal && ReferenceCalibrator_path)
  {
    Reference::To< MEAL::Complex2 > clone = foreach_pcal->clone();
    *pcal_path *= clone;
  }

  *pcal_path *= instrument;

  add_transformation ( pcal_path );

  ReferenceCalibrator_path = equation->get_transformation_index ();
}

void Calibration::StandardModel::fix_orientation ()
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::fix_orientation",
		 "cannot fix orientation when type=" + type->get_name());

  // fix the orientation of the first receptor
  physical->set_constant_orientation (true);
}

void Calibration::StandardModel::update () try
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
    polar_estimate.update (polar);

  if (fluxcal_backend)
    fluxcal_backend_estimate.update (fluxcal_backend);

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    return;

  Calibration::SingleAxis* backend = physical->get_backend();

  backend_estimate.update (backend);

  if (gain)
    update_parameter( gain, backend->get_gain().get_value() );
  else if (pcal_gain)
    pcal_gain->set_gain( backend->get_gain() );

  if (pcal_gain)
    backend->set_gain( 1.0 );
  
  if (diff_gain)
    update_parameter( diff_gain, backend->get_diff_gain().get_value() );
    
  if (diff_phase)
    update_parameter( diff_phase, backend->get_diff_phase().get_value() );
}
catch (Error& error)
{
  throw error += "Calibration::StandardModel::update";
}

void Calibration::StandardModel::update_parameter (MEAL::Scalar* function,
						   double value)
{
  MEAL::Polynomial* polynomial = dynamic_cast<MEAL::Polynomial*>( function );
  if (polynomial)
    polynomial->set_param( 0, value );
}


void Calibration::StandardModel::check_constraints ()
{
  /* for now, do nothing.

  When flux calibrator observations are not available, it is assumed
  that the noise diode has zero circular polarization.

  */
}

void Calibration::StandardModel::no_reference_calibrators ()
{
  /*
    ReferenceCalibrator observations are used to constrain the boost
    component of the degeneracy along the Stokes V axis.  If there
    no such observations, then it is assumed that the boost is 
    zero ...
  */

  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
  {
    polar->set_infit (0, false); // gain
    polar->set_infit (3, false); // boost along V
    return;
  }

  /*
    ... in the phenomenological parameterization, no boost along
    the Stokes V axis means that the ellipticities are equal.
  */
  Britton2000* bri00 = dynamic_cast<Britton2000*>( response.get() );
  if (bri00)
  {
    bri00->set_infit (0, false); // gain
    bri00->equal_ellipticities();
    return;
  }

  Instrument* van04 = dynamic_cast<Instrument*>( response.get() );
  if (van04)
  {
    van04->set_infit (0, false); // gain
    van04->equal_ellipticities();
    return;
  }

  throw Error (InvalidState, 
               "Calibration::StandardModel::no_reference_calibrators",
               "don't know how to handle this");
}

bool decrement_nfree (MEAL::Scalar* function)
{
  MEAL::Steps* steps = dynamic_cast<MEAL::Steps*> (function);
  if (steps && steps->get_nstep())
  {
    steps->remove_step (0);
    return true;
  }

  MEAL::Polynomial* poly = dynamic_cast<MEAL::Polynomial*> (function);
  if (poly && poly->get_nparam() > 1)
  {
    poly->resize( poly->get_nparam() - 1 );
    return true;
  }

  return false;
}

//! Attempt to reduce the number of degrees of freedom in the model
bool Calibration::StandardModel::reduce_nfree ()
{
  bool reduced = false;

  if (gain && decrement_nfree (gain))
    reduced = true;

  if (diff_gain && decrement_nfree (diff_gain))
    reduced = true;

  if (diff_phase && decrement_nfree (diff_phase))
    reduced = true;

  return reduced;
}

void copy_param (MEAL::Function* to, const MEAL::Function* from)
{
  unsigned nparam = to->get_nparam ();

  if (nparam != from->get_nparam())
    throw Error (InvalidParam, "copy_param", "to.nparam=%d != from.nparam=%d",
		 nparam, from->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++)
    to->set_param( iparam, from->get_param(iparam) );
}


void 
Calibration::StandardModel::copy_transformation (const MEAL::Complex2* xform)
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
  {
    const MEAL::Polar* copy = dynamic_cast<const MEAL::Polar*>( xform );
    if (!copy)
      throw Error (InvalidState, "StandardModel::copy_transformation",
		   "solution is not of the required type");

    copy_param( polar, copy );
  }

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (physical)
  {
    const BackendFeed* copy = dynamic_cast<const BackendFeed*>( xform );
    if (!copy)
      throw Error (InvalidState, "StandardModel::copy_transformation",
		   "solution is not of the required type");

    copy_param( physical, copy );
  }
}

void 
Calibration::StandardModel::integrate_parameter (MEAL::Scalar* function,
						 double value)
{
  MEAL::Steps* steps = dynamic_cast<MEAL::Steps*>( function );
  if (!steps)
    return;

  unsigned istep = steps->get_step();

  // cerr << "StandardModel set step " << istep << endl;

  steps->set_param( istep, value );
}

void 
Calibration::StandardModel::integrate_calibrator (const MEAL::Complex2* xform,
						  bool flux_calibrator)
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
  {
    const MEAL::Polar* polar_solution;

    polar_solution = dynamic_cast<const MEAL::Polar*>( xform );

    if (polar_solution)
      polar_estimate.integrate( polar_solution );

    return;
  }

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (physical)
  {
    const Calibration::SingleAxis* sa;

    sa = dynamic_cast<const Calibration::SingleAxis*>( xform );

    if (!sa)
      return;

    if (flux_calibrator)
      fluxcal_backend_estimate.integrate( sa );
    else
    {
      backend_estimate.integrate( sa );

      if (gain)
	integrate_parameter( gain, sa->get_gain().get_value() );
      
      if (diff_gain)
	integrate_parameter( diff_gain, sa->get_diff_gain().get_value() );
      
      if (diff_phase)
	integrate_parameter( diff_phase, sa->get_diff_phase().get_value() );
    }
  }
}

void Calibration::StandardModel::set_gain (Univariate<Scalar>* function)
{
  if (!built)
    build ();

  if (constant_pulsar_gain)
  {
    if (pcal_gain)
    {
      if (verbose)
	cerr << "Calibration::StandardModel::set_gain set pcal gain" << endl;
      pcal_gain_chain->set_constraint (0, function);
    }
  }
  else
  {
    BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
    if (!physical)
      throw Error (InvalidState, "Calibration::StandardModel::set_gain",
		   "cannot set gain variation in polar model");

    if (verbose)
      cerr << "Calibration::StandardModel::set_gain set physical gain" << endl;
    physical->set_gain( function );
  }

  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  gain = function;
}

void Calibration::StandardModel::set_diff_gain (Univariate<Scalar>* function)
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::set_diff_gain",
		 "cannot set gain variation in polar model");

  physical -> set_diff_gain( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_gain = function;
}

void Calibration::StandardModel::set_diff_phase (Univariate<Scalar>* function)
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::set_diff_phase",
		 "cannot set diff_phase variation in polar model");

  physical -> set_diff_phase( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_phase = function;
}

void Calibration::StandardModel::set_reference_epoch (const MJD& epoch)
{
#ifdef _DEBUG
  cerr << "Calibration::StandardModel::set_reference_epoch " << epoch << endl;
#endif

  MJD current_epoch = convert.get_reference_epoch();
  convert.set_reference_epoch( epoch );

  time.set_value (current_epoch);
  double offset = convert.get_value();

  if (gain)
    offset_steps( gain, offset );
  
  if (diff_gain)
    offset_steps( diff_gain, offset );
  
  if (diff_phase)
    offset_steps( diff_phase, offset );
}

//! Set gain to the univariate function of time
const MEAL::Scalar* Calibration::StandardModel::get_gain () const
{
  return gain;
}

//! Set differential gain to the univariate function of time
const MEAL::Scalar* Calibration::StandardModel::get_diff_gain () const
{
  return diff_gain;
}

//! Set differential phase to the univariate function of time
const MEAL::Scalar* Calibration::StandardModel::get_diff_phase () const
{
  return diff_phase;
}


void Calibration::StandardModel::offset_steps (Scalar* function, double offset)
{
  Steps* steps = dynamic_cast<Steps*> (function);
  if (!steps)
    return;

#ifdef _DEBUG
  cerr << "Calibration::StandardModel::offset_steps offset=" << offset << " ";
#endif
  for (unsigned i=0; i < steps->get_nstep(); i++)
  {
    steps->set_step( i, steps->get_step(i) + offset );
#ifdef _DEBUG
    cerr << i << "=" << steps->get_step(i) << " ";
#endif
  }
#ifdef _DEBUG
  cerr << endl;
#endif
}

void Calibration::StandardModel::add_observation_epoch (const MJD& epoch)
{
  MJD zero;

  if (min_epoch == zero || epoch < min_epoch) 
    min_epoch = epoch;

  if (max_epoch == zero || epoch > max_epoch) 
    max_epoch = epoch;

  MJD half_minute (0.0, 30.0, 0.0);
  time.set_value (min_epoch - half_minute);

  double min_step = convert.get_value();

  if (gain)
    set_min_step( gain, min_step );
  
  if (diff_gain)
    set_min_step( diff_gain, min_step );
  
  if (diff_phase)
    set_min_step( diff_phase, min_step );
}

void Calibration::StandardModel::set_min_step (Scalar* function, double step)
{
  Steps* steps = dynamic_cast<Steps*> (function);
  if (!steps)
    return;

  if (!steps->get_nstep())
  {
#ifdef _DEBUG
    cerr << "Calibration::StandardModel::set_min_step add step[0]="
         << step << endl;
#endif
    steps->add_step( step );
  }
 
  else if (step < steps->get_step(0))
  {
#ifdef _DEBUG
    cerr << "Calibration::StandardModel::set_min_step set step[0]="
         << step << endl;
#endif
    steps->set_step( 0, step );
  }
}

void Calibration::StandardModel::add_calibrator_epoch (const MJD& epoch)
{
  MJD zero;

  if (convert.get_reference_epoch() == zero)
    convert.set_reference_epoch ( epoch );

  // it may be necessary to remove this signal path if
  // the add_data step fails and no other calibrator succeeds
  if (!get_polncal_path() || foreach_pcal)
  {
    if (verbose)
      cerr << "Calibration::StandardModel::add_calibrator_epoch"
	" add_polncal_backend" << endl;

    add_polncal_backend();
  }

#ifdef _DEBUG
  cerr << "Calibration::StandardModel::add_calibrator_epoch epoch="
       << epoch << endl;
#endif

  MJD half_minute (0.0, 30.0, 0.0);

  time.set_value( epoch-half_minute );

  if (gain)
    add_step( gain, convert.get_value() );
  
  if (diff_gain)
    add_step( diff_gain, convert.get_value() );
  
  if (diff_phase)
    add_step( diff_phase, convert.get_value() );
}

void Calibration::StandardModel::add_step (Scalar* function, double step)
{
  Steps* steps = dynamic_cast<Steps*> (function);
  if (steps)
  {
#ifdef _DEBUG
    cerr << "Calibration::StandardModel::add_step step=" << step << endl;
#endif
    steps->add_step (step);
  }
}



void Calibration::StandardModel::engage_time_variations () try
{
  if (time_variations_engaged)
    return;

  time_variations_engaged = true;

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    return;

#ifdef _DEBUG
  cerr << "before engage nparam = " << physical->get_nparam() << endl;
#endif

  if (gain) 
  {
#ifdef _DEBUG
    cerr << "engage constant_pulsar_gain=" << constant_pulsar_gain << endl;
#endif

    if (!constant_pulsar_gain)
      physical->set_gain( gain );
    else if (pcal_gain_chain)
      pcal_gain_chain->set_constraint (0, gain);

  }

  if (pcal_gain)
  {
#ifdef _DEBUG
    cerr << "engage fix physical gain = 1" << endl;
#endif
    physical->set_gain( 1.0 );
  }

  if (diff_gain)
  {
#ifdef _DEBUG
    cerr << "engage diff_gain" << endl;
#endif
    physical->set_diff_gain( diff_gain );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "engage diff_phase" << endl;
#endif
    physical->set_diff_phase( diff_phase );
  }

#ifdef _DEBUG
  cerr << "after engage nparam = " << physical->get_nparam() << endl;
#endif
}
catch (Error& error)
{
  throw error += "Calibration::StandardModel::engage_time_variations";
}

void Calibration::StandardModel::disengage_time_variations (const MJD& epoch) 
try
{
  /*
  If the Instrument class ellipticities are set equal to one
  independent parameter via a ChainRule, then it will look as though
  the model has eight parameters (instead of seven), which messes up
  the creation of the PolnCalibrator extension.
  */
  Instrument* van04 = dynamic_cast<Instrument*>( response.get() );
  if (van04)
    van04->independent_ellipticities();

  if (!time_variations_engaged)
    return;

  time_variations_engaged = false;

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    return;

  time.set_value (epoch);

  Univariate<Scalar>* zero = 0;

#ifdef _DEBUG
  cerr << "before disengage nparam = " << physical->get_nparam() << endl;
#endif

  if (gain)
  {
#ifdef _DEBUG
    cerr << "disengage gain" << endl;
#endif

    if (!constant_pulsar_gain)
    {
      physical->set_gain( zero );
      physical->set_gain( gain->estimate() );
    }
    else if (pcal_gain_chain)
    {
      pcal_gain_chain->set_constraint( 0, zero );
      pcal_gain->set_gain( gain->estimate() );
    }

  }

  if (pcal_gain)
    physical->set_gain( pcal_gain->get_gain() );

  if (diff_gain)
  {
#ifdef _DEBUG
    cerr << "disengage diff_gain" << endl;
#endif
    physical->set_diff_gain( zero );
    physical->set_diff_gain( diff_gain->estimate() );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "disengage diff_phase" << endl;
#endif
    physical->set_diff_phase( zero );
    physical->set_diff_phase( diff_phase->estimate() );
  }

#ifdef _DEBUG
  cerr << "after disengage nparam = " << physical->get_nparam() << endl;
#endif

}
catch (Error& error)
{
  throw error += "Calibration::StandardModel::disengage_time_variations";
}

void Calibration::StandardModel::solve () try
{
  engage_time_variations ();

  get_equation()->solve();

  if (impurity)
    for (unsigned i=0; i<impurity->get_nparam(); i++)
    {
      cerr << i << ":" << impurity->get_Estimate(i) << " ";
      if (i%3 == 2)
	cerr << endl;
    }
}
catch (Error& error)
{
  cerr << "Calibration::StandardModel::solve failure \n\t"
       << error.get_message() << endl;
}


void Calibration::StandardModel::compute_covariance
( unsigned index, vector< vector<double> >& covar,
  vector<unsigned>& function_imap, MEAL::Scalar* function )
{
  vector<double> gradient;
  function->evaluate( &gradient );
  unsigned nparam = function->get_nparam();

  for (unsigned i=0; i<covar.size(); i++)
  {
    assert( covar[index][i] == 0.0 );

    if (i == index) {
      covar[i][i] = function->estimate().get_variance();
      continue;
    }

    double covariance = 0;
    for (unsigned iparam=0; iparam < nparam; iparam++)
      covariance += gradient[iparam] * covar[ function_imap[iparam] ][i];

    covar[index][i] = covar[i][index] = covariance;
  }
}

void Calibration::StandardModel::get_covariance( vector<double>& covar,
						 const MJD& epoch )
{
  vector< vector<double> > Ctotal;
  vector< unsigned > imap;

  get_equation()->get_solver()->get_covariance (Ctotal);

  if (Ctotal.size() != get_equation()->get_nparam())
    throw Error( InvalidState, "Calibration::StandardModel::get_covariance",
		 "covariance matrix size=%u != nparam=%u",
		 Ctotal.size(), get_equation()->get_nparam() );

  vector< unsigned > gain_imap;
  if (gain)
    MEAL::get_imap( get_equation(), gain, gain_imap );
  else if (pcal_gain)
    MEAL::get_imap( get_equation(), pcal_gain, gain_imap );

  vector< unsigned > diff_gain_imap;
  if (diff_gain)
    MEAL::get_imap( get_equation(), diff_gain, diff_gain_imap );

  vector< unsigned > diff_phase_imap;
  if (diff_phase)
    MEAL::get_imap( get_equation(), diff_phase, diff_phase_imap );

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  vector< unsigned > frontend_imap;
  if (physical)
    MEAL::get_imap( get_equation(), physical->get_frontend(), frontend_imap );

  disengage_time_variations( epoch );

  MEAL::Complex2* xform = get_transformation();

  // extract the indeces of the transformation within the model
  MEAL::get_imap( get_equation(), xform, imap );

  if (gain)
    compute_covariance( imap[0], Ctotal, gain_imap, gain );
  else if (pcal_gain)
    imap[0] = gain_imap[0];

  if (diff_gain)
    compute_covariance( imap[1], Ctotal, diff_gain_imap, diff_gain );

  if (diff_phase)
    compute_covariance( imap[2], Ctotal, diff_phase_imap, diff_phase );

  if (physical)
  {
    unsigned ifeed = 3;
    for (unsigned i=0; i<frontend_imap.size(); i++)
      imap[ifeed+i] = frontend_imap[i];
  }

  unsigned nparam = xform->get_nparam();
  unsigned ncovar = nparam * (nparam+1) / 2;

  covar.resize (ncovar);
  unsigned count = 0;

  for (unsigned i=0; i<nparam; i++)
  {
#ifdef _DEBUG
    cerr << i << ":" << imap[i] << " " << xform->get_param_name(i) << endl;
#endif
    for (unsigned j=i; j<nparam; j++)
    {
      assert( count < ncovar );
      covar[count] = Ctotal[imap[i]][imap[j]];

      if (i > 0 && i == j && physical)
      {
	// ensure that variances are sensible
	if (covar[count] > 0.5)
	  throw Error( InvalidState, 
		       "Calibration::StandardModel::get_covariance",
		       "invalid %s variance=%lf",
		       physical->get_param_name(i).c_str(), covar[count] );
      }
      count ++;
    }
  }

  if (count != ncovar)
    throw Error( InvalidState, "Calibration::StandardModel::get_covariance",
		 "count=%u != ncovar=%u", count, ncovar );
  
}
