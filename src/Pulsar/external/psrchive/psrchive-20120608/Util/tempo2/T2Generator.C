/***************************************************************************
 *
 *   Copyright (C) 2007-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG

#include "T2Generator.h"
#include "T2Predictor.h"

#include "Pulsar/Parameters.h"

#include "TemporaryDirectory.h"
#include "DirectoryLock.h"
#include "SystemCall.h"
#include "RealTimer.h"
#include "Error.h"

#include "file_cast.h"
#include "strutil.h"
#include "lazy.h"
#include "debug.h"

#include <tempo2pred_int.h>
#include <string.h>

using namespace std;

LAZY_GLOBAL(Tempo2::Generator, \
	    Configuration::Parameter<std::string>, keyword_filename, "");

static bool loaded = false;
static std::vector<std::string> keywords;

bool Tempo2::Generator::print_time = false;

std::vector<std::string>& Tempo2::Generator::get_keywords ()
{
  if (loaded)
    return keywords;

  loaded = true;

  string filename = get_keyword_filename();

  DEBUG("Tempo2::Generator::get_keywords filename=" << filename);

  if (!filename.empty()) try
  {
    stringfload (&keywords, filename);

    DEBUG("Tempo2::Generator::get_keywords array size=" << keywords.size());
  }
  catch (Error& error)
  {
    if (Predictor::verbose)
      cerr << "Tempo2::Generator::get_keywords could not load filename "
	   << error.get_message() << endl;
  }

  return keywords;
}

Tempo2::Generator::Generator (const Pulsar::Parameters* parameters)
{
  epoch1 = epoch2 = 0;
  freq1 = freq2 = 0;

  ntimecoeff = 12;
  nfreqcoeff = 2;

  // one hour
  segment_length = 1.0/24.0;

  if (parameters)
    set_parameters( parameters );
}

Tempo2::Generator* Tempo2::Generator::clone () const
{
  return new Generator (*this);
}

Tempo2::Generator::~Generator ()
{
}

//! Set the parameters used to generate the predictor
void Tempo2::Generator::set_parameters (const Pulsar::Parameters* p)
{
  parameters = p;
}

//! Set the range of epochs over which to generate
void Tempo2::Generator::set_time_span (const MJD& start, const MJD& finish)
{
  epoch1 = from_MJD (start);
  epoch2 = from_MJD (finish);
}

//! Set the range of frequencies over which to generate
void Tempo2::Generator::set_frequency_span (long double low, long double high)
{
  freq1 = low;
  freq2 = high;
}

//! Set the site at which the signal is observed
void Tempo2::Generator::set_site (const std::string& site)
{
  sitename = site;
}

//! Set the number of time coefficients
void Tempo2::Generator::set_time_ncoeff (unsigned ncoeff)
{
  ntimecoeff = ncoeff;
}

//! Set the number of frequency coefficients
void Tempo2::Generator::set_frequency_ncoeff (unsigned ncoeff)
{
  nfreqcoeff = ncoeff;
}

//! Set the length of each polynomial segment in days
void Tempo2::Generator::set_segment_length (long double days)
{
  segment_length = days;
}

static TemporaryDirectory directory ("tempo2");
static DirectoryLock lock;

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Generator::generate () const
{
  Reference::To<Tempo2::Predictor> pred = new Tempo2::Predictor;

  long double use_epoch1 = epoch1;
  long double use_epoch2 = epoch2;

  if ( (epoch2 - epoch1) < segment_length )
  {
    use_epoch1 -= 0.5 * segment_length;
    use_epoch2 += 0.5 * segment_length;
  }

  if (Predictor::verbose)
    cerr << "Tempo2::Generator::generate call tempo2\n" 
      " sitename=" << sitename <<
      " epoch1=" << use_epoch1 << " epoch2=" << use_epoch2 << "\n"
      " segment_length=" << segment_length <<
      " freq1=" << freq1 << " freq2=" << freq2 << "\n"
      " coeffs: ntime=" << ntimecoeff << " nfreq=" << nfreqcoeff
	 << endl;

  lock.set_directory( directory.get_directory() );
  DirectoryLock::Push raii (lock);

  lock.clean ();

  RealTimer timer;

  string parfile = "pular.par";
  parameters->unload (parfile);

  string tempo = "tempo2 -npsr 1 -f " + parfile + " -pred ";

  double seconds_in_day = 24.0 * 60.0 * 60.0;

  string arguments = sitename 
    + " " + tostring(use_epoch1,15) + " " + tostring(use_epoch2,15)
    + " " + tostring(freq1) + " " + tostring(freq2)
    + " " + tostring(ntimecoeff) + " " + tostring(nfreqcoeff)
    + " " + tostring(segment_length * seconds_in_day);

  string redirect = " > stdout.txt 2> stderr.txt";

  string system_call = tempo + "\"" + arguments + "\"" + redirect;

  if (Predictor::verbose)
    cerr << "Tempo2::Generator::generate system call \n"
	 << system_call << endl;

  SystemCall shell;
  shell.run( system_call );

  string predfile = "t2pred.dat";

  pred->load_file( predfile );

  if (print_time)
  {
    timer.stop ();
    cerr << "Tempo2::Generator::generate construction took " << timer << endl;
  }

  pred->set_observing_frequency (0.5L * (freq1 + freq2));

  return pred.release();
}


template<typename T> 
void Tempo2::Generator::work_around_tempo2_tzr_bug (T& psr) const
{
  /*
    Work around TZRMJD, TZRFRQ, and TZRSITE bugs in older versions of
    the tempo2 predictor library
  */

  if (psr.param[param_tzrmjd].paramSet[0]==0)
  {
    if (Predictor::verbose)
      cerr << "Tempo2::Generator::generate TZRMJD not set. Setting to "
	   << psr.param[param_pepoch].val[0] << endl;

    psr.param[param_tzrmjd].paramSet[0]=1;
    psr.param[param_tzrmjd].val[0] = psr.param[param_pepoch].val[0];
  }

  if (psr.param[param_tzrfrq].paramSet[0]==0)
  {
    if (Predictor::verbose)
      cerr << "Tempo2::Generator::generate TZRFRQ not set. Setting to "
	   << 0.5L * (freq1 + freq2) << endl;

    psr.param[param_tzrfrq].paramSet[0]=1;
    psr.param[param_tzrfrq].val[0] = 0.5L * (freq1 + freq2);
  }

  if (strlen(psr.tzrsite)<1)
  {
    if (Predictor::verbose)
      cerr << "Tempo2::Generator::generate TZRSITE not set. Setting to "
	   << sitename << endl;

    // tzrsite[100] defined in tempo2.h
    strncpy (psr.tzrsite, sitename.c_str(), 100);
  }
}
