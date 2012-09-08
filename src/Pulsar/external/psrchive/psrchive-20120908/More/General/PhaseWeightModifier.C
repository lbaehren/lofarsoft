/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightModifier.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

void Pulsar::PhaseWeightModifier::operator () (PhaseWeight* _weight)
{
  set_weight (_weight);
  get_weight (_weight);
}

//! Set the PhaseWeight from which the PhaseWeight will be derived
void Pulsar::PhaseWeightModifier::set_weight (const PhaseWeight* _weight)
{
  input_weight = _weight;
}

//! Returns a PhaseWeight with the Weight attribute set
void Pulsar::PhaseWeightModifier::get_weight (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();

#ifdef _DEBUG
  cerr << "Pulsar::PhaseWeightModifier::get_weight nbin=" << nbin << endl;
#endif

  Reference::To<PhaseWeight> output_weight = weight;

  if (output_weight == input_weight)
    output_weight = new PhaseWeight (nbin);
  else
    output_weight->resize (nbin);

#ifdef _DEBUG
  cerr << "Pulsar::PhaseWeightModifier::get_weight call calculate" << endl;
#endif

  calculate (output_weight);

#ifdef _DEBUG
  cerr << "Pulsar::PhaseWeightModifier::get_weight done calculate" << endl;
#endif

  if (output_weight != weight)
    *weight = *output_weight;
}

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PhaseWeightModifier::get_interface ()
{
  throw Error (InvalidState, "Pulsar::PhaseWeightModifier::get_interface",
	       "not implemented");
}
