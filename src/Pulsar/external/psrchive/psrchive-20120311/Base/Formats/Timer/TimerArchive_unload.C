/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Pulsar/TimerArchive.h"
#include "Pulsar/TimerIntegration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"
#include "load_factory.h"

#include "timer++.h"
#include "mini++.h"
#include "coord.h"

using namespace std;

/*****************************************************************************/
void Pulsar::TimerArchive::unload_file (const char* filename) const try
{
  if (get<IntegrationOrder>())
    throw Error (InvalidState, "Pulsar::TimerArchive::unload_file",
		 "The TimerArchive class cannot unload files with"
		 " alternate IntegrationOrder extensions");
  
  FILE* fptr = fopen (filename, "w");
  if (!fptr)
    throw Error (FailedSys, "TimerArchive::unload", "fopen");

  if (verbose == 3) 
    cerr << "TimerArchive::unload opened '" << filename << "'" << endl;
  
  try
  {
    unload (fptr);
  }
  catch (Error& error)
  {
    fclose (fptr);
    throw error += "TimerArchive::unload(" + string(filename) + ")";
  }
  fclose (fptr);
  
}
catch (Error& error) {
  throw error += "Pulsar::TimerArchive::unload_file";
}

void Pulsar::TimerArchive::unload (FILE* fptr) const
{
  pack_extensions ();

  hdr_unload (fptr);

  backend_unload (fptr);

  if (get_type() == Signal::Pulsar)
    psr_unload (fptr);

  subint_unload (fptr);
}


void Pulsar::TimerArchive::subint_unload (FILE* fptr) const
{
  if (verbose == 3) 
    cerr << "TimerArchive::unload"
      " nsubint=" << hdr.nsub_int <<
      " nchan="   << hdr.nsub_band <<
      " npol="    << hdr.banda.npol <<
      " nbin="    << hdr.nbin << endl;
 
  for (int isub=0; isub < hdr.nsub_int; isub++)
  { 
    if (verbose == 3)
      cerr << "TimerArchive::subint_unload " 
	   << isub+1 << "/" << hdr.nsub_int << endl;

    // by over-riding Archive::new_Integration, the subints array points to
    // instances of TimerIntegrations

    const TimerIntegration* subint;
    subint = dynamic_cast<const TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "TimerArchive::subint_unload",
		   "subints[%d] is not a TimerIntegration", isub);

    subint -> unload (fptr);

    if (verbose == 3)
      cerr << "TimerArchive::subint_unload " 
	   << isub+1 << "/" << hdr.nsub_int << " unloaded" << endl;
  }

  if (verbose == 3) 
    cerr << "TimerArchive::subint_unload exit\n";
}


void Pulsar::TimerArchive::hdr_unload (FILE* fptr) const
{
  string text;

  if (!valid) {
    if (verbose == 3)
      cerr << "TimerArchive::hdr_unload correcting archive" << endl;
    const_cast<TimerArchive*>(this)->correct();
  }

  struct timer* header = const_cast<struct timer*> (&hdr);

  if (model)
    header->nbytespoly = nbytes<Predictor> (model);
  else
    header->nbytespoly = 0;

  if (verbose == 3) cerr << "TimerArchive::hdr_unload predictor size = " 
			 << hdr.nbytespoly << " bytes" << endl;

  // const_cast<TimerArchive*>(this)->ephemeris = 0; // avoid segfault in Fortran code

  if (ephemeris)
    header->nbytesephem = nbytes<Parameters> (ephemeris);
  else
    header->nbytesephem = 0;

  if (verbose == 3) cerr << "TimerArchive::hdr_unload parameters size = " 
			 << hdr.nbytesephem << " bytes" << endl;

  if (get_nsubint() == 1)
    header->sub_int_time = integration_length();

  if (verbose == 3)
    cerr << "TimerArchive::hdr_unload writing timer header" << endl;

  string reason;
  if (!Timer::is_timer(hdr, &reason))
    throw Error (FailedCall, "TimerArchive::hdr_unload", reason);

  if (Timer::unload (fptr, hdr) < 0)
    throw Error (FailedCall, "TimerArchive::hdr_unload", "Timer::unload");
}

void Pulsar::TimerArchive::backend_unload (FILE* fptr) const
{
  // nothing to do
}

void Pulsar::TimerArchive::psr_unload (FILE* fptr) const
{
  if (model && hdr.nbytespoly > 0) {
    if (verbose == 3)
      cerr << "TimerArchive::psr_unload "
	   << hdr.nbytespoly << " bytes in predictor" << endl;

    long before = ftell(fptr);
    model->unload (fptr);
    long after = ftell(fptr);

    if (after - before != hdr.nbytespoly)
      throw Error (FailedCall, "TimerArchive::psr_unload",
		   "Predictor::unload wrote %d != %d bytes",
		   after - before, hdr.nbytespoly);
  }

  if (ephemeris && hdr.nbytesephem > 0) {
    if (verbose == 3) 
      cerr << "TimerArchive::psr_unload "
	   << hdr.nbytesephem << " bytes in ephemeris" << endl;

    long before = ftell(fptr);
    ephemeris->unload (fptr);
    long after = ftell(fptr);

    if (after - before != hdr.nbytesephem)
      throw Error (FailedCall, "TimerArchive::psr_unload",
		   "Paremeters::unload wrote %d != %d bytes",
		   after - before, hdr.nbytesephem);
  }
}

