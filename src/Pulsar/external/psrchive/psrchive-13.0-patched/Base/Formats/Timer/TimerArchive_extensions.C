/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimerArchive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/TapeInfo.h"
#include "Pulsar/Backend.h"
#include "Pulsar/CalInfoExtension.h"

#include <string.h>

using namespace std;

void Pulsar::TimerArchive::unpack_extensions () try
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions" << endl;

  Receiver* receiver = getadd<Receiver>();
  unpack (receiver);

  Telescope* telescope = getadd<Telescope>();

  try
  {
    telescope->set_coordinates (get_telescope());
  }
  catch (Error& error)
  {
    warning << "Pulsar::TimerArchive::unpack_extensions " 
            << error.get_message().c_str() << endl;
  }
  
  TapeInfo* tape = getadd<TapeInfo>();
  unpack (tape);

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions set Backend" << endl;

  Backend* backend = get<Backend>();

  if (!backend)
  {
    Backend* ben = getadd<Backend>();
    ben->set_name (hdr.machine_id);
    backend = ben;
  }

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions " 
	 << backend->get_extension_name() << " name="
	 << backend->get_name() << endl;

}
catch (Error& error)
{
  throw error += "Pulsar::TimerArchive::unpack_extensions";
}

void Pulsar::TimerArchive::pack_extensions () const
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::pack_extensions" << endl;

  struct timer* header = const_cast<struct timer*>( &hdr );

  const Receiver* receiver = get<Receiver>();
  if (receiver)
    const_cast<TimerArchive*>(this)->pack (receiver);
  else if (verbose == 3)
    cerr << "Pulsar::TimerArchive::pack_extensions no Receiver" << endl;

  const TapeInfo* tape = get<TapeInfo>();
  if (tape)
    const_cast<TimerArchive*>(this)->pack (tape);
  else if (verbose == 3)
    cerr << "Pulsar::TimerArchive::pack_extensions no TapeInfo" << endl;

  // nothing done with Telescope Extension for now

  const Backend* backend = get<Backend>();

  if (backend) {

    string name = backend->get_name();
    
    if( name.length()+1 > MACHINE_ID_STRLEN )
      throw Error (InvalidParam, "Pulsar::TimerArchive::pack_extensions",
		   "length of backend '%s'=%d > MACHINE_ID_STRLEN=%d",
		   name.c_str(), name.length()+1, MACHINE_ID_STRLEN);

    strcpy (header->machine_id, name.c_str());

  }

}

void Pulsar::TimerArchive::unpack (TapeInfo* tape)
{
  tape->set_tape_label (hdr.tape_label);
  tape->set_file_number (hdr.file_number);
}

void Pulsar::TimerArchive::pack (const TapeInfo* tape)
{
  strncpy (hdr.tape_label, tape->get_tape_label().c_str(), TLABEL_STRLEN);
  hdr.file_number = tape->get_file_number();
}

void Pulsar::TimerArchive::unpack (CalInfoExtension* cal)
{
  cal->cal_mode = "N/A";
  cal->cal_frequency = -1.0;
  cal->cal_dutycycle = 0.5;
  cal->cal_phase = -1.0;

  if (get_nsubint())
    cal->cal_frequency = 1.0 / get_Integration(0)->get_folding_period();
}
