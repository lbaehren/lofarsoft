/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SimplePredictor.h"

#include "inverse_phase.h"
#include "Physical.h"
#include "strutil.h"
#include "Error.h"

using namespace std;

double Pulsar::SimplePredictor::precision = 1e-10;

Pulsar::SimplePredictor::SimplePredictor (const char* filename)
{
  if (verbose)
    cerr << "Pulsar::SimplePredictor this=" << this << endl;

  if (!filename)
    return;

  if (verbose)
    cerr << "Pulsar::SimplePredictor filename=" << filename << endl;

  FILE* fptr = fopen (filename, "r");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::SimplePredictor ctor",
		 "fopen (%s)", filename);

  load (fptr);
  fclose (fptr);
}

Pulsar::SimplePredictor::SimplePredictor (const SimplePredictor& copy)
{
  coefs = copy.coefs;
  name = copy.name;
  coordinates = copy.coordinates;
  dispersion_measure = copy.dispersion_measure;
}

Pulsar::SimplePredictor::~SimplePredictor ()
{
}

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Pulsar::SimplePredictor::clone () const
{
  if (verbose)
    cerr << "Pulsar::SimplePredictor::clone" << endl;
  return new SimplePredictor (*this);
}

//! Add the information from the supplied predictor to self
void Pulsar::SimplePredictor::insert (const Pulsar::Predictor* from)
{
  throw Error (InvalidState, "Pulsar::SimplePredictor::insert",
	       "Pulsar::SimplePredictor::insert not implemented"); 
}

void Pulsar::SimplePredictor::keep (const std::vector<MJD>& epochs)
{
  if (verbose)
    cerr << "Pulsar::SimplePredictor::keep not implemented" << endl;
}

//! Add the information from the supplied predictor to self
bool Pulsar::SimplePredictor::matches (const Predictor* other) const
{
  const SimplePredictor* like = dynamic_cast<const SimplePredictor*> (other);
  if (!like)
    return false;

  return 
    name == like->name &&
    coordinates == like->coordinates &&
    dispersion_measure == like->dispersion_measure;
}

//! Set the observing frequency at which predictions will be made
void Pulsar::SimplePredictor::set_observing_frequency (long double MHz)
{
}

//! Get the observing frequency at which phase and epoch are related
long double Pulsar::SimplePredictor::get_observing_frequency () const
{
  return 0;
}

//! Return the phase, given the epoch
Phase Pulsar::SimplePredictor::phase (const MJD& t) const
{
  if (reference_epoch == MJD::zero)
    const_cast<SimplePredictor*>(this)->reference_epoch = t;

  MJD diff = t - reference_epoch;
  long double seconds = diff.get_secs();
  seconds += diff.get_fracsec();

  long double power_of_t = seconds;
  long double result = 0;

  for (unsigned i=0; i<coefs.size(); i++)
  {
    result += coefs[i] * power_of_t;
    power_of_t *= seconds;
  }

  int64_t iphase = int64_t(result);

  return Phase(iphase,result-iphase);
}

//! Return the spin frequency, given the epoch
long double Pulsar::SimplePredictor::frequency (const MJD& t) const
{
  if (reference_epoch == MJD::zero)
     const_cast<SimplePredictor*>(this)->reference_epoch = t;

  MJD diff = t - reference_epoch;
  long double seconds = diff.get_secs();
  seconds += diff.get_fracsec();

  long double power_of_t = 1.0;
  long double result = 0;

  for (unsigned i=0; i<coefs.size(); i++)
  {
    result += coefs[i] * (i+1) * power_of_t;
    power_of_t *= seconds;
  }

  return result;
}

//! Return the phase correction for dispersion delay
Phase Pulsar::SimplePredictor::dispersion (const MJD &t, long double MHz) const
{
  throw Error (InvalidState, "Pulsar::SimplePredictor::dispersion",
	       "Pulsar::SimplePredictor dispersion not implemented"); 
}

void Pulsar::SimplePredictor::load (FILE* fptr)
{
  vector<char> buffer (256);
  char* buf = &buffer[0];

  if (verbose)
    cerr << "Pulsar::SimplePredictor::load" << endl;

  while ( fgets (buf, buffer.size(), fptr) == buf )
  {
    string line = buf;

    line = stringtok (&line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // the first key loaded should be the name of the instance
    string key = stringtok (&line, " \t");

    if (verbose)
      cerr << "Pulsar::SimplePredictor::load key ='" << key << "'" << endl;

    if (!line.length())
      continue;

    string val = stringtok (&line, " \t");

    if (verbose)
      cerr << "Pulsar::SimplePredictor::load val ='" << val << "'" << endl;

    if (key == "SOURCE:")
      name = val;

    else if (key == "DM:")
      dispersion_measure = fromstring<double>(val);

    else if (key == "RA:")
      coordinates.ra().setHMS (val.c_str());

    else if (key == "DEC:")
      coordinates.dec().setDMS (val.c_str());

    else if (key == "PERIOD:")
    {
      long double period = fromstring<long double>(val);
      if (coefs.size() == 0)
	coefs.resize(1);
      coefs[0] = 1/period;
    }

    else if (key == "PDOT:")
    {
      if (coefs.size() == 0)
        throw Error (InvalidState, "Pulsar::SimplePredictor::load",
                     "PERIOD not specified before PDOT");
      long double pdot = fromstring<long double>(val);
      if (coefs.size() < 2)
	coefs.resize(2);
      coefs[1] = -0.5 * pdot / (coefs[0] * coefs[0]);
    }

    else if (key == "ACC:")
    {
      if (coefs.size() == 0)
        throw Error (InvalidState, "Pulsar::SimplePredictor::load",
                     "PERIOD not specified before ACC");
      long double acc = fromstring<long double>(val);
      if (coefs.size() < 2)
	coefs.resize(2);
      coefs[1] = -0.5 * coefs[0] * acc / Pulsar::speed_of_light;
    }

    else
      throw Error (InvalidParam, "Pulsar::SimplePredictor::load",
		   "unrecognized key='" + key + "'");
  }
}

void Pulsar::SimplePredictor::unload (FILE* fptr) const
{
  if (verbose)
    cerr << "Pulsar::SimplePredictor::unload fptr=" << fptr << endl;

  if (name.length())
    fprintf (fptr, "SOURCE: %s\n", name.c_str());

  fprintf (fptr, "DM: %lf\n", dispersion_measure);

  fprintf (fptr, "RA: %s\n", coordinates.ra().getHMS().c_str());
  fprintf (fptr, "DEC: %s\n", coordinates.dec().getDMS().c_str());

  if (coefs.size())
  {
    long double period = 1/coefs[0];
    fprintf (fptr, "PERIOD: %s\n", tostring(period).c_str());
  }

  if (coefs.size() > 1)
  {
    long double acc = 2.0 * coefs[1] * Pulsar::speed_of_light / coefs[0];
    fprintf (fptr, "ACC: %s\n", tostring(acc).c_str());
  }
}


//! Return the epoch, given the phase
MJD 
Pulsar::SimplePredictor::iphase (const Phase& phase, const MJD* guess) const
{
  if (!guess)
    throw Error (InvalidState, "Pulsar::SimplePredictor::iphase",
		 "cannot compute inverse phase without a first guess");

  return Pulsar::inverse_phase (*this, phase, *guess);
}

Pulsar::Generator* Pulsar::SimplePredictor::generator () const
{
  throw Error (InvalidState, "Pulsar::SimplePredictor::generator",
               "no Generator has been implemented");
}

