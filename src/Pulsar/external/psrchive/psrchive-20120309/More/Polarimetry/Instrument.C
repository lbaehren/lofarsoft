/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Instrument.h"
#include "Pulsar/Feed.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;

void Calibration::Instrument::init ()
{
  feed = new Calibration::Feed;
  feed_chain = new MEAL::ChainRule<MEAL::Complex2>;
  feed_chain->set_model( feed );

  add_model( feed_chain );

#ifdef _DEBUG
  for (unsigned i=0; i<get_nparam(); i++)
    cerr << "Calibration::Instrument::init name[" << i << "]="
	 << get_param_name (i) << endl;
#endif
}

Calibration::Instrument::Instrument ()
{
  init ();
}

Calibration::Instrument::Instrument (const Instrument& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const Calibration::Instrument& 
Calibration::Instrument::operator = (const Instrument& s)
{
  if (&s != this)
  {
    BackendFeed::operator = (s);
    *feed = *(s.feed);
  }
  return *this;
}

Calibration::Instrument* Calibration::Instrument::clone () const
{
  return new Instrument (*this);
}

Calibration::Instrument::~Instrument ()
{
  if (verbose)
    cerr << "Calibration::Instrument::dtor" << endl;
}

//! Return the name of the class
string Calibration::Instrument::get_name () const
{
  return "Instrument";
}

//! Get the orientation
Estimate<double> Calibration::Instrument::get_orientation (unsigned ir) const
{
  if (orientations)
    return orientations->get_value ();
  else
    return feed->get_orientation (ir);
}

//! Get the ellipticity
Estimate<double> Calibration::Instrument::get_ellipticity (unsigned ir) const
{
  if (ellipticities)
    return ellipticities->get_value ();
  else
    return feed->get_ellipticity (ir);
}

//! Set the orientation
void Calibration::Instrument::set_orientation (unsigned ir,
					       const Estimate<double>& theta)
{
  if (orientations)
    orientations->set_value (theta);
  else
    feed->set_orientation (ir, theta);
}

//! Set the ellipticity
void Calibration::Instrument::set_ellipticity (unsigned ir,
					       const Estimate<double>& chi)
{
  if (ellipticities)
    ellipticities->set_value (chi);
  else
    feed->set_ellipticity (ir, chi);
}

void Calibration::Instrument::equal_ellipticities ()
{
  if (ellipticities)
    return;

  ellipticities = new MEAL::ScalarParameter;
  feed_chain->set_constraint (0, ellipticities);
  feed_chain->set_constraint (2, ellipticities);
}

void Calibration::Instrument::independent_ellipticities ()
{
  if (!ellipticities)
    return;

  ellipticities = 0;
  feed_chain->set_constraint (0, ellipticities.ptr());
  feed_chain->set_constraint (2, ellipticities.ptr());
}

void Calibration::Instrument::equal_orientations ()
{
  if (orientations)
    return;

  orientations = new MEAL::ScalarParameter;
  feed_chain->set_constraint (1, orientations);
  feed_chain->set_constraint (3, orientations);
}

void Calibration::Instrument::independent_orientations ()
{
  if (!orientations)
    return;

  orientations = 0;
  feed_chain->set_constraint (1, orientations.ptr());
  feed_chain->set_constraint (3, orientations.ptr());
}

void Calibration::Instrument::set_cyclic (bool flag)
{
  BackendFeed::set_cyclic (flag);
  feed->set_cyclic (flag);
}

void Calibration::Instrument::set_constant_orientation (bool flag)
{
  feed->get_orientation_transformation(0)->set_infit (0, !flag);
}

Calibration::Feed* Calibration::Instrument::get_feed ()
{
  return feed;
}

const Calibration::Feed* Calibration::Instrument::get_feed () const
{
  return feed;
}

const MEAL::Complex2* Calibration::Instrument::get_frontend () const
{
  return feed;
}
