/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Transposer.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::Transposer::Transposer (const Archive* _archive)
{
  archive = _archive;

  dim[0] = Signal::Phase;
  dim[1] = Signal::Frequency;
  dim[2] = Signal::Polarization;

  verbose = false;
}

Pulsar::Transposer::~Transposer ()
{
}

void Pulsar::Transposer::set_Archive (const Pulsar::Archive* _archive)
{
  archive = _archive; 
}

void Pulsar::Transposer::set_dim (unsigned idim, Signal::Dimension _dim)
{
  range_check (idim, "Pulsar::Transposer::set_dim");
  dim[idim] = _dim;
}

unsigned Pulsar::Transposer::get_ndim (unsigned idim)
{
  range_check (idim, "Pulsar::Transposer::get_ndim");

  Dimensions dims (archive);
  return dims.get_ndim( dim[idim] );
}

void Pulsar::Transposer::range_check (unsigned idim, const char* method) const
{
  if (idim > 3)
    throw Error (InvalidRange, method, "idim=%d > 3", idim);
}

Pulsar::Dimensions Pulsar::Transposer::get_stride () const
{
  return get_stride( Dimensions(archive) );
}

Pulsar::Dimensions Pulsar::Transposer::get_stride (const Dimensions& d) const
{
  int increment = 1;
  Dimensions stride;

  stride.set_ndim (dim[0], increment);
  increment *= d.get_ndim (dim[0]);

  stride.set_ndim (dim[1], increment);
  increment *= d.get_ndim (dim[1]);

  stride.set_ndim (dim[2], increment);
  increment *= d.get_ndim (dim[2]);

  stride.set_if_zero (increment);

  return stride;
}

void Pulsar::Transposer::get_amps (vector<float>& amps) const
{
  if (!archive)
    throw Error (InvalidState, "Pulsar::Transposer::get_amps",
		 "no Archive set");

  Dimensions dims (archive);
  amps.resize ( dims.nsub * dims.npol * dims.nchan * dims.nbin );

  Dimensions stride = get_stride ();

  if (verbose)
    cerr << "Transposer::get_amps nsub=" << dims.nsub << " npol=" << dims.npol 
	 << " nchan=" << dims.nchan << " nbin=" << dims.nbin << endl
         << "Transposer::get_amps jsub=" << stride.nsub 
         << " jpol=" << stride.npol 
	 << " jchan=" << stride.nchan << " jbin=" << stride.nbin << endl;
  
  for (unsigned isub=0; isub < dims.nsub; isub++)
    get_amps (archive->get_Integration(isub), 
	      &(amps[0]) + isub*stride.nsub,
	      stride);

}

void Pulsar::Transposer::get_amps (const Integration* integration,
				   float* amps, const Dimensions& dims) const
{
  unsigned npol = integration->get_npol();
  unsigned nchan = integration->get_nchan();

  if (verbose)
    cerr << "int.npol=" << npol << " int.nchan=" << nchan << endl;

  for (unsigned ipol=0; ipol<npol; ipol++) {
    float* chandat = amps + ipol * dims.npol;
    for (unsigned ichan=0; ichan<nchan; ichan++)
      get_amps (integration->get_Profile(ipol, ichan), 
		chandat + ichan * dims.nchan,
		dims.nbin);
  }
}

void Pulsar::Transposer::get_amps (const Profile* profile,
				   float* amps, unsigned jbin) const
{
  const float* ptr = profile->get_amps();
  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin<nbin; ibin++) {
    *amps = *ptr;
    ptr ++;
    amps += jbin;
  }
}



Pulsar::Dimensions::Dimensions (const Pulsar::Archive* archive)
{
  if (!archive)
    nsub = npol = nchan = nbin = 0;
  else {
    nsub  = archive -> get_nsubint ();
    npol  = archive -> get_npol ();
    nchan = archive -> get_nchan ();
    nbin  = archive -> get_nbin ();
  }
}

void Pulsar::Dimensions::set_if_zero (unsigned ndim)
{
  if (!nsub) nsub = ndim;
  if (!npol) npol = ndim;
  if (!nchan) nchan = ndim;
  if (!nbin) nbin = nbin;
}

unsigned Pulsar::Dimensions::get_ndim (Signal::Dimension axis) const
{
  switch (axis)
    {
    case Signal::Phase:
      return nbin;
    case Signal::Frequency:
      return nchan;
    case Signal::Polarization:
      return npol;
    case Signal::Time:
      return nsub;
    default:
      return 0;
    }
}

void Pulsar::Dimensions::set_ndim (Signal::Dimension axis, unsigned ndim)
{
  switch (axis)
    {
    case Signal::Phase:
      nbin = ndim; break;
    case Signal::Frequency:
      nchan = ndim; break;
    case Signal::Polarization:
      npol = ndim; break;
    case Signal::Time:
      nsub = ndim; break;
    default:
      break;
    }
}
