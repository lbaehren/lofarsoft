/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Britton2000.h"

#include "MEAL/Boost1.h"
#include "MEAL/Rotation1.h"

using namespace std;

class Calibration::Britton2000::Feed 
  : public MEAL::ProductRule<MEAL::Complex2>
{
  public:

  //! Default Constructor
  Feed ()
  {
    MEAL::Boost1* boost = 0;
    MEAL::Rotation1* rotation = 0;

    // b_{\hat u} (\delta_\theta/2)
    add_model( boost = new MEAL::Boost1 (Vector<3, double>::basis(1)) );
    boost->set_param_name ("b_1");

    // b_{\hat v} (\delta_\chi/2)
    add_model( boost = new MEAL::Boost1 (Vector<3, double>::basis(2)) );
    boost->set_param_name ("b_2");

    // r_{\hat u} (\sigma_\chi/2)
    add_model( rotation = new MEAL::Rotation1 (Vector<3, double>::basis(1)) );
    rotation->set_param_name ("r_1");

    // b_{\hat v} (\sigma_\theta/2)
    add_model( rotation = new MEAL::Rotation1 (Vector<3, double>::basis(2)) );
    rotation->set_param_name ("r_2");
  }

  //! Assignment Operator
  Feed& operator = (const Feed& feed)
  {
    Function::operator=( feed );
    return *this;
  }

};

void Calibration::Britton2000::init ()
{
  add_model( feed = new Feed );
}

Calibration::Britton2000::Britton2000 ()
{
  init ();
}

Calibration::Britton2000::Britton2000 (const Britton2000& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const Calibration::Britton2000& 
Calibration::Britton2000::operator = (const Britton2000& s)
{
  if (&s != this)
  {
    BackendFeed::operator = (s);
    *feed = *(s.feed);
  }
  return *this;
}

Calibration::Britton2000* Calibration::Britton2000::clone () const
{
  return new Britton2000 (*this);
}

Calibration::Britton2000::~Britton2000 ()
{
  if (verbose)
    cerr << "Calibration::Britton2000::dtor" << endl;
}

//! Return the name of the class
string Calibration::Britton2000::get_name () const
{
  return "Britton2000";
}

void Calibration::Britton2000::equal_ellipticities ()
{
  if (verbose)
    cerr << "Calibration::Britton2000::equal_ellipticities name="
	 << feed->get_param_name (1) << endl;
  
  feed->set_param (1, 0.0);
  feed->set_infit (1, false);
}

void Calibration::Britton2000::equal_orientations ()
{
  if (verbose)
    cerr << "Calibration::Britton2000::equal_orientations name="
	 << feed->get_param_name (0) << endl;

  feed->set_param (0, 0.0);
  feed->set_infit (0, false);
}

//! Fix the orientation of the frontend
void Calibration::Britton2000::set_constant_orientation (bool flag)
{
  if (verbose)
    cerr << "Calibration::Britton2000::set_constant_orientation name="
	 << feed->get_param_name (3) << endl;
  feed->set_infit (3, !flag);
}

const MEAL::Complex2* Calibration::Britton2000::get_frontend () const
{
  return feed;
}

void Calibration::Britton2000::set_cyclic (bool flag)
{
  BackendFeed::set_cyclic (flag);
  cerr << "Calibration::Britton2000::set_cyclic" << endl;
}
