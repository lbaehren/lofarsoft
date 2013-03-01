/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ScintArchive.h"
#include "Pulsar/Profile.h"
#include "Error.h"
#include "ScintPowerEstimator.h"

#include <string.h>

#include "compressed_io.h"
#include "timer++.h"

using namespace std;

//
//
//
Pulsar::ScintArchive::ScintArchive ()
{
  if (verbose == 3)
    cerr << "ScintArchive default construct" << endl;

  add_extension (new ScintPowerEstimator (this));
}

//
//
//
Pulsar::ScintArchive::ScintArchive (const ScintArchive& archive)
{
  if (verbose == 3)
    cerr << "ScintArchive copy construct" << endl;

  Archive::copy (archive); // results in call to ScintArchive::copy
}

//
//
//
Pulsar::ScintArchive::~ScintArchive ()
{
  if (verbose == 3)
    cerr << "ScintArchive destructor" << endl;
}

//
//
//
const Pulsar::ScintArchive&
Pulsar::ScintArchive::operator = (const ScintArchive& arch)
{
  if (verbose == 3)
    cerr << "ScintArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to ScintArchive::copy
  return *this;
}

//
//
//
void Pulsar::ScintArchive::copy (const Archive& archive)
{
  if (verbose == 3)
    cerr << "ScintArchive::copy" << endl;

  if (this == &archive)
    return;

  TimerArchive::copy (archive);
  set_be_data_size ();

  const ScintArchive* barchive;
  barchive = dynamic_cast<const ScintArchive*>(&archive);
  if (!barchive)
    return;

  if (verbose == 3)
    cerr << "ScintArchive::copy another ScintArchive" << endl;

  spectra = barchive->spectra;

}



//
//
//
Pulsar::ScintArchive* Pulsar::ScintArchive::clone () const
{
  if (verbose == 3)
    cerr << "ScintArchive::clone" << endl;
  return new ScintArchive (*this);
}

// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////


string Pulsar::ScintArchive::Agent::get_description ()
{
  return "TIMER Archive with Scint Extensions version 1";
}

//! return true if filename refers to a timer archive
bool Pulsar::ScintArchive::Agent::advocate (const char* filename)
{
  struct timer hdr;
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )
    return false;

  if ( strncmp (hdr.backend, "scint", BACKEND_STRLEN) != 0 )
    return false;

  return true;
}


void Pulsar::ScintArchive::set_be_data_size ()
{
  if (verbose == 3)
    cerr << "Pulsar::ScintArchive::set_be_data_size" << endl;

  // size of the compressed float vector header (see fwrite_compressed)
  unsigned c_hdr = (unsigned) 2 * sizeof(float) + sizeof(uint64_t);
  // size of unsigned short (elements of compressed float vector
  unsigned s_ush = sizeof(unsigned short);

  unsigned size = get_nbin() * ( c_hdr + get_nchan() * s_ush );

  //
  // Set the information in the 'timer' header that allows TimerArchive
  // to ignore the information specific to this class
  //
  Timer::set_backend (&hdr, "scint");
  hdr.be_data_size = size;

}

void Pulsar::ScintArchive::correct ()
{
  if (verbose == 3)
    cerr << "Pulsar::ScintArchive::correct" << endl;
  set_be_data_size ();
  TimerArchive::correct ();
}


void Pulsar::ScintArchive::backend_load (FILE* fptr)
{
  bool swap_endian = false;

  long file_start = ftell (fptr);

  spectra.resize(get_nbin());

  for (unsigned ibin=0; ibin<get_nbin(); ibin++)
    if (fread_compressed (fptr, &spectra[ibin], swap_endian) < 0)
	throw Error (FailedSys, "ScintArchive::backend_load",
		     "fread_compressed spectra[%d]", ibin);

  long file_end = ftell (fptr);

  if ((file_end - file_start) != (long)hdr.be_data_size)
    throw Error (InvalidState, "ScintArchive::backend_load",
		 "loaded %d bytes != %d bytes",
		 file_end - file_start, hdr.be_data_size);

  set_nsubint(0);
}

void Pulsar::ScintArchive::init_spectra ()
{
  unsigned nbin = get_nbin();
  unsigned nchan = get_nchan();

  spectra.resize( nbin );
  for (unsigned ibin=0; ibin < nbin; ibin++)
    spectra[ibin].resize( nchan );

  for (unsigned ichan=0; ichan < nchan; ichan++) {
    float* amps = get_Profile( 0, 0, ichan )->get_amps();
    for (unsigned ibin=0; ibin < nbin; ibin++) 
      spectra[ibin][ichan] = amps[ibin];
  }

  // delete all integrations    
  set_nsubint(0);
}


void Pulsar::ScintArchive::backend_unload (FILE* fptr) const
{
  long file_start = ftell (fptr);

  const_cast<ScintArchive*>(this)->init_spectra ();

  for (unsigned ibin=0; ibin<get_nbin(); ibin++)
    if (fwrite_compressed (fptr, spectra[ibin]) < 0)
        throw Error (FailedSys, "ScintArchive::backend_unload",
                     "fwrite_compressed spectra[%d]", ibin);

  long file_end = ftell (fptr);

  if ((file_end - file_start) != (long) hdr.be_data_size)
    throw Error (InvalidState, "ScintArchive::backend_unload",
		 "unloaded %d bytes != %d bytes\n",
		 file_end - file_start, hdr.be_data_size);

  if (verbose == 3)
    cerr << "ScintArchive::backend_unload exit" << endl;
}

const vector<float>& Pulsar::ScintArchive::get_passband (unsigned ibin) const
{
  if (ibin >= spectra.size())
    throw Error (InvalidParam, "Pulsar::ScintArchive::get_passband",
                 "ibin=%d >= nbin=%d", ibin, spectra.size());

  return spectra[ibin];
}

void Pulsar::ScintArchive::integrate (const ScintArchive* archive)
{
  unsigned nbin = get_nbin();
  unsigned nchan = get_nchan();

  if (nbin != archive->get_nbin())
    throw Error (InvalidParam, "Pulsar::ScintArchive::integrate",
                 "nbin=%d != archive nbin=%d", nbin, archive->get_nbin());

  if (nchan != archive->get_nchan())
    throw Error (InvalidParam, "Pulsar::ScintArchive::integrate",
                 "nchan=%d != archive nchan=%d", nchan, archive->get_nchan());

  for (unsigned ibin=0; ibin < nbin; ibin++)
    for (unsigned ichan=0; ichan < nchan; ichan++)
      spectra[ibin][ichan] += archive->spectra[ibin][ichan];

  hdr.sub_int_time += archive->hdr.sub_int_time;

  if (archive->hdr.mjd < hdr.mjd)
    hdr.mjd = archive->hdr.mjd;

  if (archive->hdr.fracmjd < hdr.fracmjd)
    hdr.fracmjd = archive->hdr.fracmjd;

}

bool Pulsar::ScintArchive::takes (const ScintArchive* archive) const
{
  double time_resolution = get_nchan()/(get_bandwidth()*1e6);
  cerr << "Pulsar::ScintArchive::takes tres=" << time_resolution << endl;

  return false;
}

