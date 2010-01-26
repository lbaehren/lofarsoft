/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Predict.h"

#include "Pulsar/TextParameters.h"
#include "psrephem.h"

#include "tempo++.h"
#include "polyco.h"
#include "ephio.h"

#include "Error.h"
#include "Warning.h"

#include "strutil.h"
#include "lazy.h"

#include <unistd.h>
#include <ctype.h>

using namespace std;

LAZY_GLOBAL(Tempo::Predict, \
	    Configuration::Parameter<unsigned>,	minimum_nspan, 0);

LAZY_GLOBAL(Tempo::Predict, \
	    Configuration::Parameter<double>, maximum_rms, 0.0);

static Warning warn;

Tempo::Predict::Predict (const Pulsar::Parameters* parameters)
{
  nspan  = 120;
  ncoef = 15;
  maxha  = 12;
  asite  = '7';
  frequency = 1400.0;

  verify = true;

  if (parameters)
    set_parameters (parameters);
}

//! Return a new, copy constructed instance of self
Tempo::Predict::Predict* Tempo::Predict::clone () const
{
  return new Predict (*this);
}

//! Set the range of epochs over which to generate
void Tempo::Predict::set_time_span (const MJD& start, const MJD& finish)
{
  m1 = start;
  m2 = finish;
}

//! Set the range of frequencies over which to generate
void Tempo::Predict::set_frequency_span (long double low, long double high)
{
  set_frequency ( 0.5 * (low + high) );
}

//! Set the site at which the signal is observed
void Tempo::Predict::set_site (const std::string& site)
{
  set_asite( code(site) );
}

void Tempo::Predict::set_asite (char code)
{
  if (code < 10)
    code += '0';

  if (asite != code)
    cached = 0;

  asite = code;
}

void Tempo::Predict::set_frequency (double MHz)
{
  if (frequency != MHz)
    cached = 0;

  frequency = MHz;
}

void Tempo::Predict::set_maxha (unsigned hours)
{
  if (maxha != hours)
    cached = 0;

  maxha = hours;
}

void Tempo::Predict::set_nspan (unsigned minutes)
{
  if (nspan != minutes)
    cached = 0;

  if (get_minimum_nspan() && minutes < get_minimum_nspan())
  {
    if (Tempo::verbose)
      cerr << "Tempo::Predict::set_nspan avoiding 'Nspan too small' feature"
	"\n\t(nspan requested=" << minutes 
	   << " minimum=" << get_minimum_nspan() << ")" << endl;
    minutes = get_minimum_nspan();
  }

  nspan = minutes;
}

void Tempo::Predict::set_ncoef (unsigned _ncoef)
{
  if (ncoef != _ncoef)
    cached = 0;

  ncoef = _ncoef;
}

//! Set the parameters used to generate the predictor
void Tempo::Predict::set_parameters (const Pulsar::Parameters* p)
{
  if (parameters && parameters.get() != p)
    cached = 0;

  const Pulsar::TextParameters* text_parameters;
  text_parameters = dynamic_cast<const Pulsar::TextParameters*> (p);
  if (text_parameters)
  {
    if (verbose)
      cerr << "Tempo::Predict::set_parameters Pulsar::TextParameters" << endl;

    if (text_parameters->get_value (nspan, "TZNSPAN", false) && verbose)
      cerr << "Tempo::Predict::set_parameters TZNSPAN=" << nspan << endl;

    if (text_parameters->get_value (ncoef, "TZNCOEF", false) && verbose)
      cerr << "Tempo::Predict::set_parameters TZNCOEF=" << ncoef << endl;
  }

  const Legacy::psrephem* old_psrephem;
  old_psrephem = dynamic_cast<const Legacy::psrephem*> (p);
  if (old_psrephem)
  {
    if (verbose)
      cerr << "Tempo::Predict::set_parameters Legacy::psrephem" << endl;

    if (old_psrephem->parmStatus[EPH_TZNSPAN]==1)
    {
      nspan = old_psrephem->value_integer[EPH_TZNSPAN];
      if (verbose)
        cerr << "Tempo::Predict::set_parameters TZNSPAN=" << nspan << endl;
    }

    if (old_psrephem->parmStatus[EPH_TZNCOEF]==1)
    {
      ncoef = old_psrephem->value_integer[EPH_TZNCOEF];
      if (verbose)
        cerr << "Tempo::Predict::set_parameters TZNCOEF=" << ncoef << endl;
    }
  }

  parameters = p;

  psrname = parameters->get_name ();

  if (psrname.empty())
    throw Error (InvalidParam, "Tempo::Predict::set_parameters", 
		 "pulsar name is empty");

  // remove all garbage from front of pulsar name (not just J)
  while (psrname.length() && !isdigit(psrname[0]))
    psrname.erase (0,1);
}

void Tempo::Predict::set_verify (bool _verify)
{
  verify = _verify;
}

// ******************************************************************

/*! The file tz.in will be written in the directory returned by 
    Tempo::get_directory, according to the following format:

     TZ.IN FORMAT

     The tz.in file is a free-format, ASCII file.  Items on a line may be
     separated by any number of spaces and tabs.  
     
     The first line contains default parameters:
     ASITE       a one-character site code (as in TOA LINES, above)
     MAXHADEF    default maximum hour angle for observations (hr)
     NSPANDEF    default time span per ephemeris entry (minutes)
     NCOEFFDEF   default number of coefficients per ephemeris entry
     FREQDEF     default observing frequency (MHz)
    
*/
void Tempo::Predict::write_tzin () const
{
  if (psrname.empty())
    throw Error (InvalidState, "Tempo::Predict::write_tzin",
		 "psrname is empty");

  string tzin = get_directory() + "/tz.in";

  if (Tempo::verbose)
    cerr << "Tempo::Predict::write_tzin to " << tzin << endl;
    
  FILE* fptr = fopen (tzin.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Tempo::Predict::write_tzin",
		 "ERROR fopen (" + tzin + ")");

  fprintf (fptr,"%c %d %d %d %8g\n", 
	   asite, maxha, nspan, ncoef, frequency);

  fprintf(fptr,"\n");
  fprintf(fptr,"\n");
  fprintf(fptr,"%s\n", psrname.c_str());

  fclose(fptr);
}


//! Returns a polyco valid over the range in MJD specified by m1 and m2
polyco Tempo::Predict::get_polyco (const MJD& _m1, const MJD& _m2)
{
  m1 = _m1;
  m2 = _m2;

  return generate_work ();
}

//! Return a new Predictor instance
Pulsar::Predictor* Tempo::Predict::generate () const
{
  Reference::To<polyco> result = new polyco;
  *result = generate_work ();
  return result.release();
}

polyco Tempo::Predict::generate_work () const
{
  if (cached && cached->start_time() < m1 && cached->end_time() > m2  ) {

    if (Tempo::verbose)
      cerr << "Tempo::Predict::get_polyco returning cached polyco" << endl;

    return *cached;

  }

  MJD to_tempo_m1 = m1;
  MJD to_tempo_m2 = m2;

  MJD half_day (0, 0.5);

  if (m2-m1 < 2*half_day)  {
    to_tempo_m1 -= half_day;
    to_tempo_m2 += half_day;
  }

  unsigned retries = 5;
  
  for (unsigned trial=0; trial < retries; trial++) {
    
    /* TEMPO will often return a polyco that does not span the range of
       MJDs requested (even when you give it half a day of forgiveness on
       either end)...  so: increase the range until TEMPO gets it right
       and satisfies the requirements of an otherwise frustrated end-user */

    lock ();

    clean ();

    write_tzin();

    string ephname = get_directory() + "/" + ephem_filename;

    if (Tempo::verbose)
      cerr << "Tempo::predict writing " << ephname << endl;
    parameters->unload (ephname);
    
    string arguments = " -z -f " + ephem_filename;
    string input;

    if (m1 == unspecified)
      input = "\n";
    else
      input = stringprintf (" %g %g\n",
			    to_tempo_m1.in_days(), to_tempo_m2.in_days());
    if (Tempo::verbose)
      cerr << "Tempo::predict calling 'tempo " << arguments << "'" << endl
	   << "Tempo::predict input: '" << input << "'" << endl;

    tempo (arguments, input);

    string stderr_name = get_directory() + "/" + stderr_filename;

    bool error = false;

    // check for error and/or warning messages
    FILE* tempo_err = fopen (stderr_name.c_str(), "r");
    if (tempo_err) {
      char* inbuf = new char[80];
      while (fgets (inbuf, 80, tempo_err) != NULL) {
	if (!error)
	  fprintf (stderr, "Tempo::predict Errors/Warnings detected:\n");
	error = true;
	fprintf (stderr, inbuf);
      }
      fclose (tempo_err);
      delete [] inbuf;
    }

    if (error) {
      unlock ();
      throw Error (FailedSys, "Tempo::predict", "TEMPO Warnings detected");
    }

    string polyco_dat = get_directory() + "/polyco.dat";

    if (Tempo::verbose)  {
      fprintf (stderr, "Tempo::predict loading polyco.dat\n");
      fprintf (stderr, "****** BEGINNING OF FILE ******\n");
      system( ("cat " + polyco_dat).c_str() );
      fprintf (stderr, "********* END OF FILE *********\n");
    }

    const_cast<Predict*>(this)->cached = new polyco;

    if (cached->load(polyco_dat) < 1)
      throw Error (FailedSys, "Tempo::predict",
		   "failed polyco::load(" + polyco_dat + ")");

    unlock ();

    if (verbose)
      cerr << "Tempo::predict scanned " << cached->pollys.size() 
	   << " polynomials" << endl;
    
    if (!verify || m1 == unspecified)
      // no need for a validity check
      return *cached;
    
    // a simple validity check
    if ( cached->start_time() > m1 || cached->end_time() < m2  ) {

      if (verbose)
	cerr << "Tempo::predict insufficient span: "
	     << cached->start_time().in_days() << " -> " 
	     << cached->end_time().in_days() 
	     << "\n   requested: " << m1.in_days() << " -> " << m2.in_days()
	     << endl;
      
      if (cached->start_time() > m1)
	to_tempo_m1 -= half_day;

      if (cached->end_time() < m2)
	to_tempo_m2 += half_day;

      if (verbose)
	cerr << "Tempo::predict retry: " << to_tempo_m1.in_days() 
	     << " -> " << to_tempo_m2.in_days() << endl;
    }

    else
    {  // polyco OK

      if (get_maximum_rms())
      {
        for (unsigned i=0; i < cached->pollys.size(); i++)
        {
          double log_rms = cached->pollys[i].get_log_rms_resid();
          double rms_turns = pow (10.0, log_rms);

          if (Tempo::verbose)
            cerr << "Tempo::Predict::generate rms[" << i << "]=" 
                 << rms_turns << " turns" << endl;

          if (rms_turns > get_maximum_rms())
          {
            warn << "Tempo::Predict::generate WARNING rms=" 
                 << rms_turns * 1e3 << " milliturns" << endl;
            warn << "Tempo::Predict::generate "
                    "http://psrchive.sourceforge.net/warnings/tempo" << endl;
          }
        }
      }

      if (Tempo::verbose) {
	fprintf (stderr, 
		 "Tempo::predict:: polyco span OK.\n");
	fprintf (stderr, "span: %g->%g\n",
		 cached->start_time().in_days(), cached->end_time().in_days());
	
	fprintf (stderr, "reqd: %g->%g\n", m1.in_days(), m2.in_days());
      }

      return *cached;

    }  // end else polyco OK

  }  // end while not satisfied

  throw Error (InvalidState, "Tempo::Predict::get_polyco",
	       "could not generate a good polyco in %d retries", retries);

}

