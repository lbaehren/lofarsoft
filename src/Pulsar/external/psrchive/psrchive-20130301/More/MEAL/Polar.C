/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Polar.h"
#include "MEAL/Gain.h"
#include "MEAL/Boost.h"
#include "MEAL/Rotation.h"
#include "MEAL/CyclicParameter.h"
#include "ModifyRestore.h"

#include "Pauli.h"

#include <assert.h>

using namespace std;

// #define _DEBUG 1

void MEAL::Polar::init ()
{

#ifdef _DEBUG
  cerr << "MEAL::Polar::init" << endl;
#endif

  // name = "Polar";

  // Note, these objects will be destroyed during Reference::To destructor
  gain = new MEAL::Gain<Complex2>;
  add_model (gain);
  // gain->name = "Polar::Gain";

  boost = new MEAL::Boost;
  add_model (boost);
  // boost->name = "Polar::Boost";

  rotation = new MEAL::Rotation;
  add_model (rotation);

#ifdef _DEBUG
  cerr << "MEAL::Polar::init exit" << endl;
#endif

}
  
MEAL::Polar::Polar ()
{
  init ();
}

MEAL::Polar::~Polar ()
{
#ifdef _DEBUG
  cerr << "MEAL::Polar destructor" << endl;
#endif
}

MEAL::Polar::Polar (const Polar& polar)
{

#ifdef _DEBUG
  cerr << "MEAL::Polar copy constructor" << endl;
#endif

  init ();
  operator = (polar);

#ifdef _DEBUG
  cerr << "MEAL::Polar copy constructor exit" << endl;
#endif

}

//! Equality Operator
const MEAL::Polar& 
MEAL::Polar::operator = (const Polar& polar)
{
  if (&polar == this)
    return *this;

#ifdef _DEBUG
  cerr << "MEAL::Polar operator =" << endl;
#endif

  *gain = *(polar.gain);
  *boost = *(polar.boost);
  *rotation = *(polar.rotation);

  return *this;
}

//! Return the name of the class
std::string MEAL::Polar::get_name () const
{
  return "Polar";
}


Estimate<double> MEAL::Polar::get_gain () const
{
  return gain->get_Estimate(0);
}


Estimate<double> MEAL::Polar::get_boost (unsigned i) const
{
  return boost->get_Estimate (i);
}

Estimate<double> MEAL::Polar::get_rotation (unsigned i) const
{
  return rotation->get_Estimate (i);
}

void MEAL::Polar::set_gain (const Estimate<double>& g)
{
  gain->set_Estimate (0, g);
}

void MEAL::Polar::set_boost (unsigned i, const Estimate<double>& b)
{
  boost->set_Estimate (i, b);
}
   
void MEAL::Polar::set_rotation (unsigned i, const Estimate<double>& phi_i)
{
  rotation->set_Estimate (i, phi_i);
}

//! Provide access to the gain transformation
const MEAL::Gain<MEAL::Complex2>* MEAL::Polar::get_gain_transformation () const
{
  return gain;
}

MEAL::Gain<MEAL::Complex2>* MEAL::Polar::get_gain_transformation ()
{
  return gain;
}

//! Provide access to the boost transformation
const MEAL::Boost* MEAL::Polar::get_boost_transformation () const
{
  return boost;
}

MEAL::Boost* MEAL::Polar::get_boost_transformation ()
{
  return boost;
}

//! Provide access to the rotation transformation
const MEAL::Rotation* MEAL::Polar::get_rotation_transformation () const
{
  return rotation;
}

MEAL::Rotation* MEAL::Polar::get_rotation_transformation ()
{
  return rotation;
}
