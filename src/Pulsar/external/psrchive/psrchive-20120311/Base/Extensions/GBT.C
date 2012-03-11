/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/GBT.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "tempo++.h"

using namespace std;

// Note: All polarization bases here refer to the native feeds of
// the receiver.  If a hybrid was in use during observing, that
// will have to be explicitly accounted for somehow outside these 
// routines.
//
// Also, this is not a complete list of all GBT receivers, just
// the ones that are commonly used for pulsar observing.

void Pulsar::GBT::Rcvr_350 (Receiver* r)
{
  r->set_name ("Rcvr_350");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::GBT::Rcvr_800 (Receiver* r)
{
  r->set_name ("Rcvr_800");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::GBT::Rcvr1_2 (Receiver* r)
{
  r->set_name ("Rcvr1_2");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
  Angle a;
  a.setDegrees(-45.0);
  r->set_field_orientation(a);
}

void Pulsar::GBT::Rcvr2_3 (Receiver* r)
{
  r->set_name ("Rcvr2_3");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
  Angle a;
  a.setDegrees(-45.0);
  r->set_field_orientation(a);
}

//! Initialize the Receiver Extension with GBT best guess
void Pulsar::GBT::guess (Receiver* r, Archive* a)
{
    // Guess based on center freq.  could be tricky since
    // some receivers have overlapping bands.
    double f = a->get_centre_frequency() / 1000.0; // Freq in GHz
    if ((f>0.290) && (f<0.395)) { GBT::Rcvr_350(r); }
    else if ((f>0.680) && (f<0.920)) { GBT::Rcvr_800(r); }
    else if ((f>1.15) && (f<1.73)) { GBT::Rcvr1_2(r); }
    else if ((f>1.73) && (f<2.60)) { GBT::Rcvr2_3(r); }
}
