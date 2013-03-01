/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DisperseWeight.h"
#include "Pulsar/Integration.h"

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/PhaseWeightShift.h"
#include "Pulsar/Dispersion.h"

//! Default constructor
Pulsar::DisperseWeight::DisperseWeight (const Integration* subint)
{
  integration = subint;

  shift = new PhaseWeightShift;
  dispersion = new Dispersion;

  // set up the dispersion computation attributes
  dispersion->set( integration );

  Functor<double()> get_shift ( dispersion.get(), &Dispersion::get_shift );
  shift->get_shift = get_shift;
}

//! Destructor
Pulsar::DisperseWeight::~DisperseWeight  ()
{
}

Pulsar::DisperseWeight* Pulsar::DisperseWeight::clone () const
{
  return new DisperseWeight (*this);
}

//! Set the PhaseWeight mask to be shifted
void Pulsar::DisperseWeight::set_weight (const PhaseWeight* weight)
{
  // set the input to the PhaseWeight shifter
  shift->set_weight( weight );
}

//! Get the shifted PhaseWeight mask for the specified channel
void Pulsar::DisperseWeight::get_weight (unsigned ichan, PhaseWeight* weight)
{
  // compute the dispersion shift for this channel (input to shifter)
  dispersion->set_Profile( integration->get_Profile(0, ichan) );

  // get the shifted PhaseWeight mask
  shift->get_weight( weight );
}

