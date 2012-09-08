/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver.h"
#include "Pulsar/TimerArchive.h"
#include "Pulsar/Parkes.h"

#include <string.h>

#include "timer_supp.h"

using namespace std;

// return true if all characters up to the null terminator are alphanumeric
bool is_valid_string (const char* str, unsigned length)
{
  for (unsigned i=0; i < length; i++) {
    if (str[i] == '\0')
      return true;
    if (!isalnum(str[i]) && str[i]!='_' && str[i]!='-')
      return false;
  }
  return false;
}

void Pulsar::TimerArchive::unpack (Receiver* receiver)
{
  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver" << endl;

  if (hdr.banda.polar == 0)
    receiver->set_basis( Signal::Circular );
  else
    receiver->set_basis( Signal::Linear );

  switch (hdr.feedmode) {
    
  case 0:
    receiver->set_tracking_mode (Receiver::Feed);
    break;
    
  case 1:
    receiver->set_tracking_mode (Receiver::Celestial);
    break;
    
  case 2:
    receiver->set_tracking_mode (Receiver::Galactic);
    break;
    
  default:
    cerr << "Pulsar::TimerArchive::unpack Receiver"    << endl;
    cerr << "  unrecognized feedmode=" << hdr.feedmode << endl;
    // AWH 4/7/2005: Changed the exception to a warning in the hope that
    // psrchive programs can then be used to correct old headers.
    //throw Error (InvalidParam, "Pulsar::TimerArchive::unpack Receiver",
    //		   "unrecognized feedmode=%d", hdr.feedmode);
    
  }

  Angle angle;

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver feed_offset="
	 << hdr.banda.feed_offset << endl;

  angle.setDegrees (hdr.banda.feed_offset); 
  receiver->set_tracking_angle (angle);

  if (hdr.rcvr_id[0] == '\0')  {
    if (verbose > 1)
      cerr << "Pulsar::TimerArchive::unpack Receiver "
	"name not specified." << endl;
    return;
  }

  // check that the receiver is a null-terminated string
  if (!is_valid_string( hdr.rcvr_id, RCVR_ID_STRLEN )) {

    if (verbose > 1)
      cerr << "Pulsar::TimerArchive::unpack Receiver name corrupted." << endl;

    hdr.rcvr_id[0] = '\0';
    return;
  }

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver name="
	 << hdr.rcvr_id << endl;

  receiver->set_name (hdr.rcvr_id);

  // timer supplement - added 23 July 04

  void* temp = &(hdr.bandb);
  struct supp* supplement = reinterpret_cast<struct supp*>( temp );

  if (supplement->version > 0
      && hdr.bandb.nlag > 0
      && hdr.bandb.flux_A > 0
      && hdr.bandb.flux_B > 0)
    cerr << "Timer::load WARNING bandB apparently in use" << endl;

  if (supplement->version < -2)  {
    if (verbose)
      cerr << "Timer::load WARNING bandB uninitialized npol="
           << hdr.bandb.npol << endl;
    hdr.bandb.npol = 0;
  }

  // correct the Version -2 parameters
  if (supplement->version > -2) {

    if (verbose > 2)
      cerr << "Pulsar::TimerArchive::unpack Receiver no supplement" << endl;

    supplement->X_offset = 0.0;
    supplement->Y_offset = 0.0;
    supplement->calibrator_offset = 0.0;
    supplement->version = -2;

  }

  receiver->set_basis_corrected (hdr.corrected & FEED_CORRECTED);
  receiver->set_projection_corrected (hdr.corrected & PARA_CORRECTED);

  if ( receiver->get_basis() == Signal::Circular )  {
    if ( supplement->X_offset != 0 )
      throw Error (InvalidState, "Pulsar::TimerArchive::unpack",
		   "Receiver basis is Circular and X_offset=%g",
                   supplement->X_offset);
    if ( supplement->Y_offset != 0 )
      throw Error (InvalidState, "Pulsar::TimerArchive::unpack",
                   "Receiver basis is Circular and Y_offset=%g",
                   supplement->Y_offset);
    if ( supplement->calibrator_offset != 0 )
      throw Error (InvalidState, "Pulsar::TimerArchive::unpack",
                   "Receiver basis is Circular and calibrator_offset=%g",
                   supplement->calibrator_offset);
    if (verbose  > 2)
      cerr << "Pulsar::TimerArchive::unpack Receiver exit circular" << endl;
    return;
  }

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver X_offset="
         << supplement->X_offset << endl;

  angle.setDegrees (supplement->X_offset);
  receiver->set_X_offset (angle);

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver Y_offset="
	 << supplement->Y_offset << endl;

  angle.setDegrees (supplement->Y_offset);
  receiver->set_Y_offset (angle);

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver calibrator_offset="
	 << supplement->calibrator_offset << endl;

  angle.setDegrees (supplement->calibrator_offset);
  receiver->set_calibrator_offset (angle);

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::unpack Receiver exit linear" << endl;

}

void Pulsar::TimerArchive::pack (const Receiver* receiver)
{
  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::pack Receiver" << endl;

  if (verbose && receiver->get_name().length() >= RCVR_ID_STRLEN)
    cerr << "Pulsar::TimerArchive::pack Receiver WARNING"
      " truncating receiver name " << receiver->get_name() <<
      " to " << RCVR_ID_STRLEN-1 << " characters" << endl;

  strncpy (hdr.rcvr_id, receiver->get_name().c_str(), RCVR_ID_STRLEN);

  switch (receiver->get_basis()) {
  case Signal::Linear:
    hdr.banda.polar = 1;
    break;
  case Signal::Circular:
    hdr.banda.polar = 0;
    break;
  default:
    hdr.banda.polar = -1;
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack Receiver",
		 "unrecognized Basis=" 
		 + Signal::Basis2string( receiver->get_basis() ));
  }

  switch (receiver->get_tracking_mode()) {
  case Receiver::Feed:
    hdr.feedmode = 0;
    break;
  case Receiver::Celestial:
    hdr.feedmode = 1;
    break;
  case Receiver::Galactic:
    hdr.feedmode = 2;
    break;
  default:
    hdr.banda.polar = -1;
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack Receiver",
		 "unrecognized tracking mode=" 
		 + receiver->get_tracking_mode_string());
  }

  hdr.banda.feed_offset = receiver->get_tracking_angle().getDegrees (); 

  set_corrected (FEED_CORRECTED, receiver->get_basis_corrected());
  set_corrected (PARA_CORRECTED,  receiver->get_projection_corrected());

  void* temp = &(hdr.bandb);
  struct supp* supplement = reinterpret_cast<struct supp*>( temp );

  if ( receiver->get_basis() == Signal::Circular )  {
    supplement->X_offset = 0;
    supplement->Y_offset = 0;
    supplement->calibrator_offset = 0;
  }
  else {
    supplement->X_offset = receiver->get_X_offset().getDegrees ();
    supplement->Y_offset = receiver->get_Y_offset().getDegrees ();
    supplement->calibrator_offset
      = receiver->get_calibrator_offset().getDegrees ();
  }

  if (verbose > 2)
    cerr << "Pulsar::TimerArchive::pack Receiver\n\t"
      "X_offset=" << supplement->X_offset << "deg\n\t"
      "Y_offset=" << supplement->Y_offset << "deg\n\t"
      "calibrator_offset=" << supplement->calibrator_offset << "deg" << endl;

  supplement->version = -2;

}

