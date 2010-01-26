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

string get_name (const Archive* archive )
{
  return archive->get_source();
}

string get_nbin (const Archive* archive )
{
  return tostring( archive->get_nbin() );
}

string get_nchan (const Archive* archive )
{
  return tostring( archive->get_nchan() );
}

string get_npol (const Archive* archive )
{
  return tostring( archive->get_npol() );
}

string get_nsub (const Archive* archive )
{
  return tostring( archive->get_nsubint() );
}

string get_length (const Archive* archive )
{
  return tostring( archive->integration_length(), 6 );
}

string get_bw (const Archive* archive )
{
  return tostring(archive->get_bandwidth(), 3);
}

string get_parang (const Archive* archive )
{
  stringstream result;
  int nsubs = archive->get_nsubint();

  if (nsubs != 0) {
    const Integration* integration = archive->get_Integration( nsubs / 2 );

    if (integration) {
      const Pointing* ext = integration->get<Pointing>();
      
      if (ext) {
	result << ext->get_parallactic_angle().getDegrees();
      }
    }
  }
  
  return result.str();
}

string get_tsub (const Archive* archive )
{
  string result;
  int nsubs = archive->get_nsubint();
  
  if ( nsubs != 0 )
  {
    const Integration* first_int = archive->get_first_Integration();
    
    if( first_int )
      return tostring( first_int->get_duration(), 6 );
  }
  
  return result;
}

string get_observer (const Archive* archive )
{
  string result;
  const ObsExtension* ext = archive->get<ObsExtension>();
  
  if( !ext ) {
    result = "UNDEF";
    
  } else {
    result = ext->get_observer();
  }
  
  return result;
}

string get_projid (const Archive* archive )
{
  string result = "";
  const ObsExtension* ext = archive->get<ObsExtension>();
  
  if( !ext ) {
    result = "UNDEF";
    
  } else {
    result = ext->get_project_ID();
  }
  
  return result;
}

string get_rcvr (const Archive* archive )
{
  string result;
  const Receiver* ext = archive->get<Receiver>();
  
  if( ext ) {
    result = ext->get_name();
  }
  
  return result;
}

string get_backend (const Archive* archive )
{
  string result = "";
  const Backend* ext = archive->get<Backend>();
  
  if (!ext)
    ext = archive->get<WidebandCorrelator>();
  
  if( !ext ) {
    result = "UNDEF";
  } else {
    result = ext->get_name();
  }
  
  return result;
}

string get_period_as_string (const Archive* archive )
{
  // TODO check this
  return tostring( archive->get_Integration(0)->get_folding_period(), 14 );
}

int get_stt_smjd (const Archive* arch)
{
  return arch->get<FITSHdrExtension>()->get_stt_smjd();
}

int get_stt_imjd (const Archive* arch)
{
  return arch->get<FITSHdrExtension>()->get_stt_imjd();
}

string get_be_delay (const Archive* archive )
{
  const Backend* ext = archive->get<WidebandCorrelator>();
  
  if ( !ext )
    return "UNDEF";
  else
    return tostring ( ext->get_delay(), 14 );
}

string FetchValue (const Archive* archive, string command) try
{
  if (command == "name") return get_name( archive );
  else if(command == "nbin") return get_nbin( archive );
  else if(command == "nchan") return get_nchan( archive );
  else if(command == "npol") return get_npol( archive );
  else if(command == "nsub") return get_nsub( archive );
  else if(command == "length") return get_length( archive );
  else if(command == "bw") return get_bw( archive );
  else if(command == "parang") return get_parang( archive );
  else if(command == "tsub") return get_tsub( archive );
  else if(command == "observer") return get_observer( archive );
  else if(command == "projid") return get_projid( archive );
  else if(command == "rcvr") return get_rcvr( archive );
  else if(command == "backend") return get_backend( archive );
  else if(command == "period") return get_period_as_string( archive );
  else if(command == "be_delay") return get_be_delay( archive );
  else if(command == "subint") return "";
  else if(command == "chan") return "";
  
  else return "INVALID";
  
} 
catch (Error& e)
{
  return "*error*";
}

std::string Pulsar::ArrivalTime::get_tempo2_aux_txt ()
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
    string value = FetchValue(observation, attributes[i]);
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
