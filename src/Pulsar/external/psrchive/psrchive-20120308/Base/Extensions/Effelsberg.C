/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Effelsberg.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "tempo++.h"

using namespace std;

void Pulsar::Effelsberg::Rcvr_UHF (Receiver* r)
{
  r->set_name ("UHF");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Effelsberg::Rcvr_Multibeam (Receiver* r)
{
  r->set_name ("Multibeam");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Effelsberg::Rcvr_20cm (Receiver* r)
{
  r->set_name ("20cm");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Effelsberg::Rcvr_11cm (Receiver* r)
{
  r->set_name ("11cm");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Effelsberg::Rcvr_6cm (Receiver* r)
{
  r->set_name ("6cm");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::Effelsberg::Rcvr_3_6cm (Receiver* r)
{
  r->set_name ("3_6cm");
  r->set_basis(Signal::Circular);
  r->set_nrcvr(2); 
  r->set_tracking_mode(Receiver::Feed);
}

//! Initialize the Receiver Extension with Effelsberg best guess
void Pulsar::Effelsberg::guess (Receiver* r, Archive* a)
{
  // Guess based on center frequency

  double f = a->get_centre_frequency() / 1000.0; // Freq in GHz

  if ((f>0.80) && (f<1.30))     { Rcvr_UHF(r); }
  else if ((f>1.3) && (f<1.43)) { Rcvr_Multibeam(r); }
  else if ((f>1.3) && (f<1.7))  { Rcvr_20cm(r); }
  else if ((f>2.6) && (f<2.68)) { Rcvr_11cm(r); }
  else if ((f>4.6) && (f<5.1))  { Rcvr_6cm(r); }
  else if ((f>7.8) && (f<8.9))  { Rcvr_3_6cm(r); }
}
