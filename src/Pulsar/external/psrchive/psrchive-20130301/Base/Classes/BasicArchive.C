/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BasicArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Error.h"

using namespace std;

void Pulsar::BasicArchive::ensure_linkage ()
{
}


Pulsar::BasicArchive::BasicArchive () 
{ 
  state = Signal::Intensity;
  scale = Signal::FluxDensity;
  type = Signal::Pulsar;

  source = "unknown";
  
  nbin = 0;
  nchan = 1;
  npol = 1;
  nsubint = 0;

  bandwidth = 0.0;
  centre_frequency = 0.0;
  dispersion_measure = 0.0;
  rotation_measure = 0.0;

  poln_calibrated = false;
  faraday_corrected = false;
  dedispersed = false;
}

const Pulsar::BasicArchive&
Pulsar::BasicArchive::operator = (const BasicArchive& copy)
{
  if (verbose == 3)
    cerr << "Pulsar::BasicArchive::operator =" << endl;

  if (this == &copy)
    return *this;

  Archive::copy (copy);

  return *this;
}

Pulsar::BasicArchive::BasicArchive (const BasicArchive& copy)
{
  if (verbose == 3)
    cerr << "Pulsar::BasicArchive:: copy ctor this=" << this << endl;

  Archive::copy (copy);
}

Pulsar::BasicArchive::~BasicArchive () 
{ 
  if (verbose == 3)
    cerr << "Pulsar::BasicArchive dtor this=" << this << endl;
}

/*!  
  By over-riding this funciton, inherited types may re-define the type
  of Integration to which the elements of the subints vector point.
*/
Pulsar::Integration*
Pulsar::BasicArchive::new_Integration (const Integration* subint)
{
  Integration* integration;

  if (subint)
    integration = new BasicIntegration (subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "BasicArchive::new_Integration");
  
  return integration;
}

//! Get the number of sub-integrations in the archive
unsigned Pulsar::BasicArchive::get_nsubint () const
{
  return nsubint;
}

//! Set the number of sub-integrations in the archive
void Pulsar::BasicArchive::set_nsubint (unsigned num_sub) 
{
  nsubint = num_sub;
}

//! Get the number of frequency polns used
unsigned Pulsar::BasicArchive::get_npol () const
{
  return npol;
}

//! Set the number of frequency polns used
void Pulsar::BasicArchive::set_npol (unsigned numpol)
{
  npol = numpol;
}

//! Get the number of frequency channels used
unsigned Pulsar::BasicArchive::get_nchan () const
{
  return nchan;
}

//! Set the number of frequency channels used
void Pulsar::BasicArchive::set_nchan (unsigned numchan)
{
  nchan = numchan;
}

//! Get the number of pulsar phase bins used
unsigned Pulsar::BasicArchive::get_nbin () const
{
  return nbin;
}

//! Set the number of pulsar phase bins used
void Pulsar::BasicArchive::set_nbin (unsigned numbins)
{
  nbin = numbins;
}

//! Get the name of the telescope
std::string Pulsar::BasicArchive::get_telescope () const
{
  return telescope;
}

//! Set the name of the telescope
void Pulsar::BasicArchive::set_telescope (const std::string& tel)
{
  telescope = tel;
}

//! Return the type of observation (psr, cal, etc.)
Signal::Source Pulsar::BasicArchive::get_type () const
{
  return type;
}

string Pulsar::BasicArchive::get_source () const
{
  return source;
}

//! Set the observation type (psr, cal etc.)
void Pulsar::BasicArchive::set_type (Signal::Source ob_type)
{
  type = ob_type;
}
    
void Pulsar::BasicArchive::set_source (const string& src)
{
  source = src;
}

//! Get the coordinates of the source
sky_coord Pulsar::BasicArchive::get_coordinates () const
{
  return coordinates;
}

//! Set the coordinates of the source
void Pulsar::BasicArchive::set_coordinates (const sky_coord& coords)
{
  coordinates = coords;
}

//! Return the bandwidth of the observation
double Pulsar::BasicArchive::get_bandwidth () const
{
  return bandwidth;
}
    
//! Set the bandwidth of the observation
void Pulsar::BasicArchive::set_bandwidth (double bw)
{
  bandwidth = bw;
}
    
//! Return the centre frequency of the observation
double Pulsar::BasicArchive::get_centre_frequency () const
{
  return centre_frequency;
}
    
//! Set the centre frequency of the observation
void Pulsar::BasicArchive::set_centre_frequency (double cf)
{
  centre_frequency = cf;
}

//! Return the polarisation state of the data
Signal::State Pulsar::BasicArchive::get_state () const
{
  return state;
}
    
//! Set the polarisation state of the data
void Pulsar::BasicArchive::set_state (Signal::State _state)
{
  state = _state;
}
    
//! Return the polarisation scale of the data
Signal::Scale Pulsar::BasicArchive::get_scale () const
{
  return scale;
}
    
//! Set the polarisation scale of the data
void Pulsar::BasicArchive::set_scale (Signal::Scale _scale)
{
  scale = _scale;
}
    
//! Get the centre frequency of the observation
double Pulsar::BasicArchive::get_dispersion_measure () const
{
  return dispersion_measure;
}

//! Set the centre frequency of the observation
void Pulsar::BasicArchive::set_dispersion_measure (double dm)
{
  dispersion_measure = dm;
}

double Pulsar::BasicArchive::get_rotation_measure () const
{
  return rotation_measure;
}

void Pulsar::BasicArchive::set_rotation_measure (double rm)
{
  rotation_measure = rm;
}

//! Data has been poln calibrated
bool Pulsar::BasicArchive::get_poln_calibrated () const
{
  return poln_calibrated;
}

//! Set the status of the poln calibrated flag
void Pulsar::BasicArchive::set_poln_calibrated (bool done)
{
  poln_calibrated = done;
}


//! Return true when the data have been corrected for ISM Faraday rotation
bool Pulsar::BasicArchive::get_faraday_corrected () const
{
  return faraday_corrected;
}
    
//! Set true when the data have been corrected for ISM Faraday rotation
void Pulsar::BasicArchive::set_faraday_corrected (bool done)
{
  faraday_corrected = done;
}

//! Inter-channel dispersion delay has been removed
bool Pulsar::BasicArchive::get_dedispersed () const
{
  return dedispersed;
}

//! Set true when the inter-channel dispersion delay has been removed
void Pulsar::BasicArchive::set_dedispersed (bool done)
{
  dedispersed = done;
}
