/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/EPNArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include <string.h>

#include "epnio.h"

using namespace std;

void Pulsar::EPNArchive::init ()
{
  current_record = 0;
  bandwidth = 0;
  centre_frequency = 0;
  state = Signal::Intensity;
}

Pulsar::EPNArchive::EPNArchive()
{
  if (verbose == 3)
    cerr << "EPNArchive construct" << endl;

  init ();
}

Pulsar::EPNArchive::~EPNArchive()
{
  // destroy any EPNArchive resources
}

Pulsar::EPNArchive::EPNArchive (const Archive& arch)
{
  if (verbose == 3)
    cerr << "EPNArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPNArchive::EPNArchive (const EPNArchive& arch)
{
  if (verbose == 3)
    cerr << "EPNArchive construct copy EPNArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::EPNArchive::copy (const Archive& archive) 
{
  if (verbose == 3)
    cerr << "EPNArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose == 3)
    cerr << "EPNArchive::copy dynamic cast call" << endl;
  
  const EPNArchive* like_me = dynamic_cast<const EPNArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose == 3)
    cerr << "EPNArchive::copy another EPNArchive" << endl;

  line1 = like_me->line1;
  line2 = like_me->line2;
  line3 = like_me->line3;
  line4 = like_me->line4;
  line5 = like_me->line5;
  
  // no block data is copied
  current_record = 0;

  centre_frequency = like_me->centre_frequency;
  bandwidth = like_me->bandwidth;
  state = like_me->state;

}

Pulsar::EPNArchive* Pulsar::EPNArchive::clone () const
{
  if (verbose == 3)
    cerr << "EPNArchive::clone" << endl;
  return new EPNArchive (*this);
}

std::string Pulsar::EPNArchive::get_telescope () const
{
  return "7";
}

void Pulsar::EPNArchive::set_telescope (const std::string& code)
{

}

Signal::Source Pulsar::EPNArchive::get_type () const
{
  return Signal::Pulsar;
}


void Pulsar::EPNArchive::set_type (Signal::Source type)
{

}


string Pulsar::EPNArchive::get_source () const
{
  return line2.jname;
}

void Pulsar::EPNArchive::set_source (const string& source)
{
  strncpy (line2.jname, source.c_str(), 12);
}



/*! From line 3 of the header:

  c     rah         - I2     - Hours of right ascension (J2000)
  c     ram         - I2     - Mins. of right ascension (J2000)
  c     ras         - F6.3   - Secs. of right ascension (J2000)
  c     ded         - I3     - Degrees of declination   (J2000)
  c     dem         - I2     - Minutes of declination   (J2000)
  c     des         - F6.3   - Seconds of declination   (J2000)

*/
sky_coord Pulsar::EPNArchive::get_coordinates () const
{
  sky_coord coordinates;

  coordinates.ra().setHMS( line3.rah, line3.ram, line3.ras );
  coordinates.dec().setDMS( line3.ded, line3.dem, line3.des );

  return coordinates;
}

//! Set the coordinates of the source
void Pulsar::EPNArchive::set_coordinates (const sky_coord& coordinates)
{
  double temp;
  coordinates.ra().getHMS( line3.rah, line3.ram, temp );
  line3.ras = temp;
  coordinates.dec().getDMS( line3.ded, line3.dem, temp );
  line3.des = temp;
}

unsigned Pulsar::EPNArchive::get_nbin () const
{
  if (line5.nbin < 0)
    return 0;

  return line5.nbin;
}

void Pulsar::EPNArchive::set_nbin (unsigned numbins)
{
  line5.nbin = numbins;
}

unsigned Pulsar::EPNArchive::get_nchan () const
{
  if (line5.nfreq < 0)
    return 0;

  return line5.nfreq;
}

void Pulsar::EPNArchive::set_nchan (unsigned numchan)
{
  line5.nfreq = numchan;
}


unsigned Pulsar::EPNArchive::get_npol () const
{
  if (line5.npol < 0)
    return 0;

  return line5.npol;
}

void Pulsar::EPNArchive::set_npol (unsigned numpol)
{
  line5.npol = numpol;
}

unsigned Pulsar::EPNArchive::get_nsubint () const
{
  return nsubint;
}

void Pulsar::EPNArchive::set_nsubint (unsigned _nsubint)
{
  nsubint = _nsubint;
}

double Pulsar::EPNArchive::get_bandwidth () const
{
  return bandwidth;
}

void Pulsar::EPNArchive::set_bandwidth (double bw)
{
  bandwidth = bw;
}

double Pulsar::EPNArchive::get_centre_frequency () const
{
  return centre_frequency;
}

void Pulsar::EPNArchive::set_centre_frequency (double cf)
{
  centre_frequency = cf;
}


Signal::State Pulsar::EPNArchive::get_state () const
{
  return state;
}

void Pulsar::EPNArchive::set_state (Signal::State _state)
{
  state = _state;
}

//! Get the scale of the profiles
Signal::Scale Pulsar::EPNArchive::get_scale () const
{
  switch (line5.fluxflag)  {
  case 'F':
    return Signal::Jansky;
  case 'R':
    return Signal::ReferenceFluxDensity;
  default:
    return Signal::FluxDensity;
  }
}

//! Set the scale of the profiles
void Pulsar::EPNArchive::set_scale (Signal::Scale scale)
{
  switch (scale) {
  case Signal::Jansky:
    line5.fluxflag = 'F';
    break;
  case Signal::ReferenceFluxDensity:
    line5.fluxflag = 'R';
    break;
  case Signal::FluxDensity:
    line5.fluxflag = ' ';
    break;
  default:
    throw Error (InvalidParam, "Pulsar::EPNArchive::set_scale",
		 "unknown scale");
  }
}


//! Get the centre frequency of the observation
double Pulsar::EPNArchive::get_dispersion_measure () const
{
  return line2.dm;
}

//! Set the centre frequency of the observation
void Pulsar::EPNArchive::set_dispersion_measure (double dm)
{
  line2.dm = dm;
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::EPNArchive::get_rotation_measure () const
{
  return line2.rm;
}

//! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
void Pulsar::EPNArchive::set_rotation_measure (double rm)
{
  line2.rm = rm;
}


bool Pulsar::EPNArchive::get_poln_calibrated () const
{
  return false;
}

void Pulsar::EPNArchive::set_poln_calibrated (bool done)
{

}


bool Pulsar::EPNArchive::get_faraday_corrected () const
{
  return false;
}

void Pulsar::EPNArchive::set_faraday_corrected (bool done)
{

}


bool Pulsar::EPNArchive::get_dedispersed () const
{
  return false;
}

void Pulsar::EPNArchive::set_dedispersed (bool done)
{

}

//
//
//
Pulsar::Integration* 
Pulsar::EPNArchive::new_Integration (const Integration* subint)
{
  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::new_Integration" << endl;

  BasicIntegration* integration;

  if (subint)
    integration = new BasicIntegration (subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "Pulsar::EPNArchive::new_Integration");
  
  return integration;
}

void Pulsar::EPNArchive::read_record (const char* filename, unsigned record)
{
  int readwri = -1;  // read
  int padout  = 0;   // no padding

  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::read_record call crwepn " << filename << endl;

  if (crwepn (filename, readwri, record, padout, 
	      &line1, &line2, &line3, &line4, &line5,
	      &sub_line1, &sub_line2, &data) < 0)
    throw Error (InvalidParam, "Pulsar::EPNArchive::read_record",
		 "error calling crwpen");
      
  if (verbose == 3)  {
    cerr << "Pulsar::EPNArchive::read_record rwepn called" << endl;
    epn_dump (&line1, &line2, &line3, &line4, &line5);
  }

  current_record = record;
}

void Pulsar::EPNArchive::load_header (const char* filename)
{
  int nrec = cnepnrec (filename);
  if (nrec < 0)
    nsubint = 0;
  else
    nsubint = nrec;

  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::load_header nsubint=" << nsubint << endl;

  read_record (filename, 1);

  if (get_npol() == 4)
    set_state (Signal::Stokes);
  else if (get_npol() == 1)
    set_state (Signal::Intensity);
  else
    throw Error (InvalidState, "Pulsar::EPNArchive::load_header",
		 "unknown npol=%d", get_npol());

  get_Integration(0);

  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::load_header exit" << endl;
}

double MHz_scale (const char* units)
{
  if (strcasecmp (units, "GHz"))
    return 1e3;
  if (strcasecmp (units, "MHz"))
    return 1.0;
  if (strcasecmp (units, "kHz"))
    return 1e-3;
  if (strcasecmp (units, "Hz"))
    return 1e-6;
  
  throw Error (InvalidParam, "MHz_scale", "unrecognized units '%s'", units);
}

Pulsar::Integration*
Pulsar::EPNArchive::load_Integration (const char* filename, unsigned subint)
{
  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::load_Integration" << subint << endl;

  if (subint+1 != current_record)
    read_record (filename, subint+1);

  BasicIntegration* integration = new BasicIntegration;
  resize_Integration (integration);

  MJD epoch (line3.epoch);
  epoch += sub_line1.tstart[0] * 1e-6;
  integration->set_epoch (epoch);

  integration->set_folding_period (line2.pbar);
  integration->set_duration (line2.pbar * line5.nint);

  for (unsigned ipol=0; ipol < get_npol(); ipol++) {

    double total_bw = 0.0;
    double total_freq = 0.0;

    for (unsigned ichan=0; ichan < get_nchan(); ichan++) {
      
      Profile* profile = integration->get_Profile (ipol, ichan);
      
      unsigned iblock = ipol * get_nchan() + ichan;

      if (verbose == 3) {
	cerr << "cf=" << sub_line1.f0[iblock] << sub_line1.f0u[iblock] << endl;
	cerr << "bw=" << sub_line1.df[iblock] << sub_line1.dfu[iblock] << endl;
	cerr << "state=" << sub_line1.idfield[iblock] << endl;
      }
      
      double freq = sub_line1.f0[iblock] * MHz_scale (sub_line1.f0u[iblock]);
      double bw = sub_line1.df[iblock] * MHz_scale (sub_line1.dfu[iblock]);

      profile->set_centre_frequency (freq);
      // profile->set_weight (sub_line1.navg[iblock]);
      profile->set_weight (1);

      if (!Profile::no_amps)
	profile->set_amps (data.rawdata[iblock]);

      total_freq += freq;
      total_bw   += bw;
     
      if (sub_line1.tstart[iblock] != sub_line1.tstart[0])
	throw Error (InvalidState, "Pulsar::EPNArchive::load_Integration",
		     "ipol=%d ichan=%d tstart=%lf != %lf",
		     sub_line1.tstart[iblock] != sub_line1.tstart[0]);

    }

    double avg_freq = total_freq / get_nchan();

    if (ipol == 0 && subint == 0) {

      set_centre_frequency (avg_freq);
      set_bandwidth (total_bw);

    }
    else {

      if ( avg_freq != get_centre_frequency() )
	throw Error (InvalidState, "Pulsar::EPNArchive::load_Integration",
		     "ipol=%d centre_frequency=%lf != %lf", ipol, avg_freq,
		     get_centre_frequency() );

      if ( total_bw != get_bandwidth() )
	throw Error (InvalidState, "Pulsar::EPNArchive::load_Integration",
		     "ipol=%d bandwidth=%lf != %lf", ipol, total_bw,
		     get_bandwidth() );		     

    }

  }

  return integration;

}

void Pulsar::EPNArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and EPNArchive attributes as well as
  // BasicIntegration attributes and data to filename

  throw Error( InvalidState, "Pulsar::EPNArchive::unload_file",
	       "EPN unload not implemented" );
}



string Pulsar::EPNArchive::Agent::get_description () 
{
  return "EPN Archive Version 6.1";
}

bool Pulsar::EPNArchive::Agent::advocate (const char* filename)
{
  FILE* fptr = fopen (filename, "r");
  if (!fptr)
    return false;

  float version;
  int scanned = fscanf (fptr, "EPN %f", &version);
  fclose (fptr);

  if (scanned != 1)
    return false;

  if (verbose == 3)
    cerr << "Pulsar::EPNArchive::Agent::advocate EPN version " << version << endl;

  return true;
}

