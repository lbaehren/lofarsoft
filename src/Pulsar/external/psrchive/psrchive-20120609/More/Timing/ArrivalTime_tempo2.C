/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/Archive.h"

#include "Pulsar/ObsExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"

using namespace Pulsar;
using namespace std;


std::string Pulsar::ArrivalTime::get_tempo2_aux_txt (const Tempo::toa& toa)
{
  string args;

  args = observation->get_filename();

  if (format_flags.find("i")!=string::npos)
  {
    const Backend* backend = observation->get<Backend>();
    if (backend)
      args += " -i " + backend->get_name();
  }

  if (format_flags.find("r")!=string::npos)
  {
    const Receiver* receiver = observation->get<Receiver>();
    if (receiver)
      args += " -r " + receiver->get_name();
  }

  if (format_flags.find("c")!=string::npos)
    args += " -c " + tostring(observation->get_nchan());

  if (format_flags.find("s")!=string::npos)
    args += " -s " + tostring(observation->get_nsubint());

  for (unsigned i = 0; i < attributes.size(); i++)
  {
    string value = get_value (attributes[i], toa);
    args += " -" + attributes[i] + " " + value;
  }

  if (format_flags.find("o")!=string::npos) /* Include observer info. */
  {
    const ObsExtension* ext = 0;
    ext = observation->get<ObsExtension>();
    if (!ext) {
      args += " -o N/A";
    }
    else {
      args += " -o " + ext->observer;
    }
  }

  return args;
}
