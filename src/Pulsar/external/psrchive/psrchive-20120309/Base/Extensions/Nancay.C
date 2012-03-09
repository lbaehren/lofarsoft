/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Nancay.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/CalInfoExtension.h"
#include "tempo++.h"

using namespace std;

// Note: All polarization bases here refer to the native feeds of
// the receiver.  If a hybrid was in use during observing, that
// will have to be explicitly accounted for somehow outside these 
// routines.

void Pulsar::Nancay::L_band (Receiver* r)
{
  r->set_name("L_Band");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2);
  r->set_tracking_mode(Receiver::Feed);
}

//! Initialize the Receiver Extension with Nancay best (only??) guess
void Pulsar::Nancay::guess (Receiver* r, Archive* a)
{
  Nancay::L_band(r);
}
