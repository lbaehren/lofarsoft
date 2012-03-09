/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Feed.h"

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2Constant.h"
#include "MEAL/Rotation1.h"
#include "MEAL/CyclicParameter.h"

#include "ModifyRestore.h"

#include <assert.h>

using namespace std;

void Calibration::Feed::init ()
{
  // name = "Feed";

  // The transformation of each receptor
  MEAL::ProductRule<MEAL::Complex2>* receptor;

  // The selection matrix of each receptor
  MEAL::Complex2Constant* selection;

  for (unsigned ir=0; ir<2; ir++) {

    string rname (1, char('0' + ir));

    receptor = new MEAL::ProductRule<MEAL::Complex2>;

    Jones<double> select_jones;
    select_jones(ir, ir) = 1.0;

    // construct the 2x2 selection matrix
    selection = new MEAL::Complex2Constant (select_jones);
    receptor->add_model (selection);

    // construct the elipticity matrix
    ellipticity[ir] = new MEAL::Rotation1 (Vector<3, double>::basis(1));
    // construct the orientation matrix
    orientation[ir] = new MEAL::Rotation1 (Vector<3, double>::basis(2));

    string describe = " of receptor " + rname + " (radians)";

    ellipticity[ir]->set_param_name( "el" + rname );
    ellipticity[ir]->set_param_description ("ellipticity" + describe);

    orientation[ir]->set_param_name( "or" + rname );
    orientation[ir]->set_param_description ("orientation" + describe);

    receptor->add_model (ellipticity[ir]);
    receptor->add_model (orientation[ir]);

    // add the receptor
    add_model (receptor);

  }

  if (get_nparam() != 4)
    throw Error (InvalidState, "Calibration::Feed::Feed",
		 "number of parameters != 4");

}

Calibration::Feed::Feed ()
{
  init ();
}

Calibration::Feed::Feed (const Feed& feed)
{
  init ();
  operator = (feed);
}

Calibration::Feed& Calibration::Feed::operator = (const Feed& feed)
{
  if (this != &feed)
    for (unsigned ir=0; ir<2; ir++) {
      *(ellipticity[ir]) = *(feed.ellipticity[ir]);
      *(orientation[ir]) = *(feed.orientation[ir]);
    }
  return *this;
}

Calibration::Feed::~Feed ()
{
}

void Calibration::Feed::set_cyclic (bool flag)
{
  if (flag) {

    for (unsigned ir=0; ir<2; ir++) {

      // set up the cyclic boundary for orientation
      MEAL::CyclicParameter* o_cyclic = 0;
      o_cyclic = new MEAL::CyclicParameter;

      o_cyclic->set_period (M_PI);
      o_cyclic->set_upper_bound (M_PI/2);
      o_cyclic->set_lower_bound (-M_PI/2);

      orientation[ir]->set_parameter_policy (o_cyclic);

      // set up the cyclic boundary for ellipticity
      MEAL::CyclicParameter* e_cyclic = 0;
      e_cyclic = new MEAL::CyclicParameter;

      e_cyclic->set_period (M_PI);
      e_cyclic->set_upper_bound (M_PI/4);
      e_cyclic->set_lower_bound (-M_PI/4);
      e_cyclic->set_azimuth (o_cyclic);

      ellipticity[ir]->set_parameter_policy (e_cyclic);

    }

  }
  else {

    MEAL::OneParameter* noncyclic = 0;

    for (unsigned ir=0; ir<2; ir++) {

      noncyclic = new MEAL::OneParameter;
      ellipticity[ir]->set_parameter_policy (noncyclic);

      noncyclic = new MEAL::OneParameter;
      orientation[ir]->set_parameter_policy (noncyclic);

    }

  }

}

//! Return the name of the class
string Calibration::Feed::get_name () const
{
  return "Feed";
}

void Calibration::Feed::set_ellipticity (unsigned ireceptor,
					 const Estimate<double>& chi)
{
  if (verbose)
    cerr << "Calibration::Feed::set_ellipticity " << chi << endl;

  assert( ireceptor < 2 );
  ellipticity[ireceptor]->set_Estimate (0, chi);
}

Estimate<double> Calibration::Feed::get_ellipticity (unsigned ireceptor) const
{
  assert( ireceptor < 2 );
  return ellipticity[ireceptor]->get_Estimate (0);
}

void Calibration::Feed::set_orientation (unsigned ireceptor,
					 const Estimate<double>& theta)
{
  if (verbose)
    cerr << "Calibration::Feed::set_orientation " << theta << endl;

  assert( ireceptor < 2 );
  orientation[ireceptor]->set_Estimate (0, theta);
}

Estimate<double> Calibration::Feed::get_orientation (unsigned ireceptor) const
{
  assert( ireceptor < 2 );
  return orientation[ireceptor]->get_Estimate (0);
}

//! Get the orientation tranformation for the specified receptor
MEAL::Rotation1* Calibration::Feed::get_orientation_transformation (unsigned i)
{
  assert( i < 2 );
  return orientation[i];
}

//! Get the ellipticity tranformation for the specified i
MEAL::Rotation1* Calibration::Feed::get_ellipticity_transformation (unsigned i)
{
  assert( i < 2 );
  return ellipticity[i];
}
