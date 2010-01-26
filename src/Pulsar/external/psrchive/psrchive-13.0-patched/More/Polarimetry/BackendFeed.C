/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendFeed.h"
#include "Pulsar/SingleAxis.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;

void Calibration::BackendFeed::init ()
{
  backend = new Calibration::SingleAxis;
  backend_chain = new MEAL::ChainRule<MEAL::Complex2>;
  backend_chain->set_model( backend );

  add_model( backend_chain );
}

Calibration::BackendFeed::BackendFeed ()
{
  init ();
}

Calibration::BackendFeed::BackendFeed (const BackendFeed& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const Calibration::BackendFeed& 
Calibration::BackendFeed::operator = (const BackendFeed& s)
{
  if (&s != this)
    *backend = *(s.backend);
  return *this;
}

Calibration::BackendFeed::~BackendFeed ()
{
  if (verbose)
    cerr << "Calibration::BackendFeed::dtor" << endl;
}

//! Return the name of the class
string Calibration::BackendFeed::get_name () const
{
  return "BackendFeed";
}

MEAL::Complex2* Calibration::BackendFeed::get_frontend ()
{
  const BackendFeed* thiz = this;
  return const_cast<MEAL::Complex2*>( thiz->get_frontend() );
}

//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> Calibration::BackendFeed::get_gain () const
{
  if (gain_variation)
    return gain_variation->estimate();
  else
    return backend->get_gain ();
}

//! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
Estimate<double> Calibration::BackendFeed::get_diff_gain () const
{
  if (diff_gain_variation)
    return diff_gain_variation->estimate();
  else
    return backend->get_diff_gain ();
}

//! Get the differential phase, \f$ \phi \f$, in radians
Estimate<double> Calibration::BackendFeed::get_diff_phase () const
{
  if (diff_phase_variation)
    return diff_phase_variation->estimate();
  else
    return backend->get_diff_phase ();
}

//! Set the instrumental gain, \f$ G \f$, in calibrator flux units
void Calibration::BackendFeed::set_gain (const Estimate<double>& g)
{
  if (gain_variation)
    throw Error (InvalidState, "Calibration::BackendFeed::set_gain",
		 "cannot set gain when it is constrained by a function");

  backend->set_gain (g);
}

//! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
void Calibration::BackendFeed::set_diff_gain (const Estimate<double>& gamma)
{
  if (diff_gain_variation)
    throw Error (InvalidState, "Calibration::BackendFeed::set_diff_gain",
		 "cannot set diff_gain when it is constrained by a function");

  backend->set_diff_gain (gamma);
}
   
//! Set the differential phase, \f$ \phi \f$, in radians
void Calibration::BackendFeed::set_diff_phase (const Estimate<double>& phi)
{
  if (diff_phase_variation)
    throw Error (InvalidState, "Calibration::BackendFeed::set_diff_phase",
		 "cannot set diff_phase when it is constrained by a function");

  backend->set_diff_phase (phi);
}

void Calibration::BackendFeed::set_cyclic (bool flag)
{
  backend->set_cyclic (flag);
}

Calibration::SingleAxis* Calibration::BackendFeed::get_backend ()
{
  return backend;
}

const Calibration::SingleAxis* Calibration::BackendFeed::get_backend () const
{
  return backend;
}

//! Set the instrumental gain variation
void Calibration::BackendFeed::set_gain (MEAL::Scalar* function)
{
  gain_variation = function;
  backend_chain->set_constraint (0, function);
}

//! Set the differential gain variation
void Calibration::BackendFeed::set_diff_gain (MEAL::Scalar* function)
{
  diff_gain_variation = function;
  backend_chain->set_constraint (1, function);
}

//! Set the differential phase variation
void Calibration::BackendFeed::set_diff_phase (MEAL::Scalar* function)
{
  diff_phase_variation = function;
  backend_chain->set_constraint (2, function);
}

//! Set the instrumental gain variation
const MEAL::Scalar* Calibration::BackendFeed::get_gain_variation () const
{
  return gain_variation.ptr();
}

//! Set the differential gain variation
const MEAL::Scalar* Calibration::BackendFeed::get_diff_gain_variation () const
{
  return diff_gain_variation.ptr();
}
   
//! Set the differential phase variation
const MEAL::Scalar* Calibration::BackendFeed::get_diff_phase_variation () const
{
  return diff_phase_variation.ptr();
}
 
