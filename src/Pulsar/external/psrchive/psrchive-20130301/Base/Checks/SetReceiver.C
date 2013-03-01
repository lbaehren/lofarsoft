/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SetReceiver.h"
#include "Pulsar/Receiver.h"

using namespace std;

//! Get the name of the log file for the given observation
string Pulsar::SetReceiver::get_directory (const Archive* archive)
{
  return "Parkes";

  /*
    Could also return the actual telscope name here.

    The name specifies the directory in which a file called
    "receiver.log" lists the UTC at which the receiver name changed.
    The directory must also contain a <ReceiverName>.rcvr for each
    receiver listed in receiver.log.
  */
}

void Pulsar::SetReceiver::apply (Archive* archive) try
{
  string directory = get_directory (archive);

  if (directory != current)
  {
    string filename = Config::get_runtime()
      + "/" + directory + "/receiver.log";

    receiver_log = new LogFile (filename);
    current = directory;
  }

  MJD epoch = archive->start_time();

  const char* date_format = "%Y-%m-%d-%H:%M:%S";
  char datebuf [64];

  string utc = epoch.datestr (datebuf, 64, date_format);

  string name = receiver_log->get_message (utc);

  string filename = Config::get_runtime()
    + "/" + directory + "/" + name + ".rcvr";

  Reference::To<Receiver> receiver = Receiver::load (filename);

  archive->add_extension( receiver );
}
catch (Error& error)
{
  throw error += "SetReceiver::apply";
}
