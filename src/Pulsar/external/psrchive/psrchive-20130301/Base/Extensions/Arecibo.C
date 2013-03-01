/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Arecibo.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "tempo++.h"

using namespace std;

// Note: All polarization bases here refer to the native feeds of
// the receiver.  If a hybrid was in use during observing, that
// will have to be explicitly accounted for somehow outside these 
// routines.

void Pulsar::Arecibo::Greg_327 (Receiver* r)
{
  r->set_name ("327");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::Greg_430 (Receiver* r)
{
  r->set_name ("430");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::CH_430 (Receiver* r)
{
  r->set_name ("CH-430");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::L_narrow (Receiver* r)
{
  r->set_name ("L-narrow");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::L_wide (Receiver* r)
{
  r->set_name ("L-wide");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::L_wide_old (Receiver* r)
{
  r->set_name ("L-wide-OLD");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::S_wide (Receiver* r)
{
  r->set_name ("S-wide");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::S_narrow (Receiver* r)
{
  r->set_name ("S-narrow");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Arecibo::C_band (Receiver* r)
{
  r->set_name ("C-band");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

//! Initialize the Receiver Extension with Arecibo best guess
void Pulsar::Arecibo::guess (Receiver* r, Archive* a)
{

    // Guess based on the current set of commonly used receivers
    // and reported center freq.  Maybe add a date-based criterion
    // for older data?
    double f = a->get_centre_frequency() / 1000.0; // Freq in GHz
    if ((f>0.312) && (f<0.342)) { Arecibo::Greg_327(r); }
    else if ((f>0.422) && (f<0.442)) { Arecibo::Greg_430(r); }
    else if ((f>1.15) && (f<1.73)) { Arecibo::L_wide(r); }
    else if ((f>1.8) && (f<3.1)) { Arecibo::S_wide(r); }
    else if ((f>3.85) && ( f<6.00)) { Arecibo::C_band(r); }

    // If none matched, do nothing?
}
