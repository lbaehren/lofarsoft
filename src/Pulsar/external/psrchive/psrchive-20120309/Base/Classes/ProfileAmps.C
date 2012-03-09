/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Config.h"

#include "VirtualMemory.h"
#include "malloc16.h"

using namespace std;

static Pulsar::Option<string> profile_swap_filename
(
 "Profile::swap", "",

 "Virtual memory swap space filename base",

 "The Profile class can use an alternative virtual memory manager so that \n"
 "the kernel swap space is not consumed when processing large files. \n"
 "\n"
 "This configuration parameter should be set the full path of a filename \n"
 "to be used for page swapping; e.g. \"/tmp/psrchive.swap\" \n"
 "\n"
 "The filename will have a unique extension added so that multiple \n"
 "processes will not conflict."
);

static Reference::To<VirtualMemory> profile_swap;
static bool profile_swap_initialized = false;

void profile_swap_initialize ()
{
  profile_swap_initialized = true;

  string filename = profile_swap_filename;

  if (filename == "")
    return;

  DEBUG("psrchive: virtual memory swap file=" << filename);

  profile_swap = new VirtualMemory (filename);
}

/*! 
  Do not allocate memory for the amps
*/
bool Pulsar::ProfileAmps::no_amps = false;

Pulsar::ProfileAmps::ProfileAmps (unsigned _nbin)
{
  DEBUG("Pulsar::ProfileAmps ctor nbin=" << nbin);
  
  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (_nbin)
    resize( _nbin );
}

Pulsar::ProfileAmps::ProfileAmps (const ProfileAmps& copy)
{
  DEBUG("Pulsar::ProfileAmps copy ctor nbin=" << copy.nbin);

  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (copy.nbin) 
  {
    resize( copy.nbin );
    set_amps( copy.amps );
  }
}

static void amps_free (float* amps)
{
  if (profile_swap)
    profile_swap->destroy (amps);
  else
    free16 (amps);
}

Pulsar::ProfileAmps::~ProfileAmps () 
{
  DEBUG("Pulsar::ProfileAmps dtor amps=" << amps);

  if (amps != NULL) amps_free (amps); amps = 0;
}

/*
  If the size of the amps array, amps_size >= _nbin, then no new
  memory is allocated.

  If _nbin == 0, the allocated space is deleted.
*/
void Pulsar::ProfileAmps::resize (unsigned _nbin)
{
  nbin = _nbin;

  if (amps_size >= nbin && nbin != 0)
    return;

  if (amps) amps_free(amps); amps = NULL;
  amps_size = 0;

  if (nbin == 0)
    return;

  if (!no_amps)
  {
    DEBUG("Pulsar::ProfileAmps::resize nbin=" << nbin);

    if (!profile_swap_initialized)
      profile_swap_initialize();

    if (profile_swap)
      amps = profile_swap->create<float> (nbin);
    else
      amps = (float*) malloc16 (sizeof(float) * nbin);

    if (!amps)
      throw Error (BadAllocation, "Pulsar::ProfileAmps::resize",
		   "failed to allocate %u floats (using %s swap space)",
		   nbin, (profile_swap) ? "custom" : "system");

    amps_size = nbin;
  }
}

//! Return a pointer to the amplitudes array
const float* Pulsar::ProfileAmps::get_amps () const
{
  if (!amps)
    throw Error (InvalidState, "Pulsar::ProfileAmps::get_amps",
		 "amplitude array not allocated");

  return amps;
}

//! Return a pointer to the amplitudes array
float* Pulsar::ProfileAmps::get_amps ()
{
  if (!amps)
    throw Error (InvalidState, "Pulsar::ProfileAmps::get_amps",
		 "amplitude array not allocated");

  return amps;
}

