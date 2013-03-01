/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver_Native.h"

Pulsar::Receiver::Native::Native ()
{
  basis = Signal::Linear;
  hand = Signal::Right;
}

//! Copy the state from another
void Pulsar::Receiver::Native::copy (const State* state)
{
  set_basis(state->get_basis());
  set_hand(state->get_hand());
  set_orientation (state->get_orientation());
  set_reference_source_phase (state->get_reference_source_phase());
}
