/***************************************************************************
 *
 *   Copyright (C) 1998-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "polyco.h"
#include "Predict.h"

#include "strutil.h"
#include "Error.h"

#include "inverse_phase.h"

#include <stdio.h> 
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#include <algorithm>

using namespace std;

bool Pulsar::Predictor::verbose = false;
bool polyco::debug = false;

// not going to get > ns precision out of a routine based in minutes
double polynomial::precision = 1e-10;

// allow requested time to be about ten percent out of advertised span
double polynomial::flexibility = 0.1;

void polynomial::init()
{
  dm = 0;
  doppler_shift = 0;
  log_rms_resid = 0;
  ref_freq = 0;
  telescope = '\0';
  nspan_mins = 0.0;
  freq = 0;
  binph = 0;
  binfreq = 0;
  binary = 0;
  tempov11 = 0;
}

polynomial::polynomial ()
{
  init ();
}

polynomial::polynomial (const polynomial & copy)
{
  init ();
  operator = (copy);
}

const polynomial & polynomial::operator = (const polynomial & copy)
{
  if (this == &copy) 
    return *this;

  psrname = copy.psrname;
  date = copy.date;
  utc = copy.utc;
  ref_time = copy.ref_time;
  dm = copy.dm;
  doppler_shift = copy.doppler_shift;
  log_rms_resid = copy.log_rms_resid;
  ref_phase = copy.ref_phase;
  ref_freq = copy.ref_freq;
  telescope = copy.telescope;
  nspan_mins = copy.nspan_mins;
  freq = copy.freq;
  binph = copy.binph;
  binfreq = copy.binfreq;
  coefs = copy.coefs; 
  binary = copy.binary;
  tempov11 = copy.tempov11;
  keep = copy.keep;

  return *this;
}  

polynomial::~polynomial ()
{
}


/* ************************************************************************
   polynomial::load

   Loads from a string formatted as described below (taken from TEMPO
   reference manual:

   POLYCO.DAT
   
   The polynomial ephemerides are written to file 'polyco.dat'.  Entries
   are listed sequentially within the file.  The file format is:
   
   Line  Columns     Item
   ----  -------   -----------------------------------
     1      1-10   Pulsar Name
           11-19   Date (dd-mmm-yy)
	   20-31   UTC (hhmmss.ss)
	   32-51   TMID (MJD)
	   52-72   DM
	   74-79   Doppler shift due to earth motion (10^-4)
	   80-86   Log_10 of fit rms residual in periods
     2      1-20   Reference Phase (RPHASE)
           21-38   Reference rotation frequency (F0)
	   39-43   Observatory number 
	   44-49   Data span (minutes)
	   50-54   Number of coefficients
	   55-75   Observing frequency (MHz)
	   76-80   Binary phase
     3*     1-25   Coefficient 1 (COEFF(1))
           26-50   Coefficient 2 (COEFF(2))
	   51-75   Coefficient 3 (COEFF(3))
   
   * Subsequent lines have three coefficients each, up to NCOEFF
   ************************************************************************ */

int polynomial::load (string* instr)
{
  this->init();    

  if (instr->length() < 160)
    return -1;

  string whitespace (" \t\n");
  string line;

  line = stringtok (*instr, "\n");
  if (line.length() < 1)
    return -1;

  if (polyco::verbose)
    cerr << "polynomial::load line 1='" << line << "'" << endl;

  psrname = line.substr(0,10);
  psrname = stringtok(psrname, whitespace);
  line.erase(0,10);

  date = stringtok (line, whitespace);
  if (date.length() < 1)
    return -1;

  utc = stringtok (line, whitespace);
  if (utc.length() < 1)
    return -1;

  if (polyco::verbose)
    cerr << "polynomial::load scanned name='"<<psrname<<"' date="<<date
         << " UTC:"<<utc<<endl;

  int mjd_day_num;
  double frac_mjd;
  int scanned = sscanf (line.c_str(), "%d %lf %f %lf %lf\n",
		&mjd_day_num, &frac_mjd, &dm, &doppler_shift, &log_rms_resid);
  if (scanned < 3)
  {
    if (polyco::verbose)
    {
      cerr << "polynomial::load(string*) error stage 1 parsing '"<<line<<"'";
      if (scanned < 1)
        perror ("");
      else
        cerr << "\npolynomial::load(string*) scanned "<<scanned<<"/5 values\n";
    }
    return -1;
  }
  if (polyco::verbose)
    cerr << "polynomial::load scanned mjd=" << mjd_day_num 
	 << "+" << frac_mjd << " dm=" << dm << " dop=" << doppler_shift
	 << " rms=" << log_rms_resid << endl;

  ref_time = MJD(mjd_day_num, frac_mjd);

  if (scanned < 5)
    tempov11 = 0;
  else
    tempov11 = 1;

  line = stringtok (*instr, "\n");
  if (line.length() < 1)
    return -1;

  if (polyco::verbose)
    cerr << "polynomial::load line 2='" << line << "'" << endl;

  string refphstr = stringtok (line, whitespace);
  if (refphstr.length() < 1)
    return -1;

  int64_t turns;
  double fracturns;

  sscanf (refphstr.c_str(), "%" PRIi64 " %lf\n", &turns, &fracturns);
  if (polyco::verbose)
    cerr << "polynomial::load scanned phase=" << turns
	 << "+" << fracturns << endl;

  if(turns>0) ref_phase = Phase(turns, fracturns);
  else ref_phase = Phase(turns, -fracturns);

  if (polyco::verbose)
    cerr << "polynomial::load Phase=" << ref_phase << endl;

  int ncoeftmp=0;

  char teletxt[8];
  scanned = sscanf (line.c_str(), "%lf %s %lf %d %lf %lf %lf\n",
		    &ref_freq, teletxt, &nspan_mins, 
		    &ncoeftmp, &freq, &binph, &binfreq);

  if (scanned < 5)
  {
    fprintf (stderr, "polynomial::load(string*) error stage 2 parsing '%s'\n",
                line.c_str());
    return -1;
  }
  if (scanned < 7)
    binary = 0;
  else
    binary = 1;

  if (strlen(teletxt) == 1)
    telescope = teletxt[0];
  else
  {
    telescope = 'a';
    int code = 0;
    sscanf (teletxt, "%d", &code);
    telescope += code-10;
    if (polyco::verbose)
      cerr << "polynomial::load telescope code=" << code << "->" 
           << telescope << endl;
  }

  coefs.resize(ncoeftmp);  
  // Read in the coefficients 
  int i;
  for (i = 0;i<ncoeftmp;i++)
  {
    line = stringtok (*instr, whitespace);

    size_t letterd = line.find('D');
    if (letterd != string::npos)
      line[letterd] = 'e';

    if (sscanf (line.c_str(), "%lf", &coefs[i]) != 1)
    {
      fprintf (stderr, "polynomial::load(string*) did not parse '%s'\n",
	       line.c_str());
      return -1;
    }

    if (polyco::verbose)
      fprintf (stderr, "polynomial::load coeff %2d = '%s' = %e\n",
	       i, line.c_str(), coefs[i]);

  }

  if (polyco::verbose)
    cerr << "polynomial::load parsed " << i << " coefficients" << endl;
  return 0;
}

// ///////////////////////////////////////////////////////////////////
// polynomial::unload (string*)
//
// Adds characters to the string to which outstr points.  
// The text added is the tempo formatted text of this polynomial.
// Return value: the number of characters added (not including the \0)
// ///////////////////////////////////////////////////////////////////
int polynomial::unload (string* outstr) const
{
  vector<char> temp (100);  // max length of string set by princeton at 86...
  char* numstr = &(temp[0]);

  int bytes = 0;

  if (tempov11)
  {
#ifdef _DEBUG
    cerr << "polynomial::unload tempo11" << endl;
#endif
    bytes += sprintf (numstr, "%-10.10s %9.9s%12.12s%22s%19f%7.3lf%7.3lf\n",
		      psrname.c_str(), date.c_str(), utc.c_str(),
		      ref_time.strtempo().c_str(),
		      dm, doppler_shift, log_rms_resid);
  }
  else
  {
#ifdef _DEBUG
    cerr << "polynomial::unload not tempo11" << endl;
#endif
    bytes += sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f\n",
		     psrname.c_str(), date.c_str(), utc.c_str(),
		     ref_time.strtempo().c_str(), dm); 
  }

  *outstr += numstr;
  
  if (binary)
  {

#ifdef _DEBUG
    cerr << "polynomial::unload binary" << endl;
#endif

    bytes += sprintf (numstr, "%20s%18.12lf    %c%5.0lf%5d%10.3f%7.4f%9.4f\n", 
		      ref_phase.strprint(6).c_str(),
		      ref_freq, telescope, nspan_mins, 
		      (int) coefs.size(), freq, binph, binfreq);
  }
  else 
    bytes += sprintf (numstr, "%20s%18.12lf    %c%5.0lf%5d%10.3f\n", 
		      ref_phase.strprint(6).c_str(),
		      ref_freq, telescope, nspan_mins, 
		      (int) coefs.size(), freq);

  *outstr += numstr;

  unsigned nrows = (int)(coefs.size()/3);
  if(nrows*3 < coefs.size()) nrows++;

#ifdef _DEBUG
    cerr << "polynomial::unload nrows=" << nrows << endl;
#endif

  const char* newline = "\n";

  for (unsigned i=0; i<nrows; ++i)
  {

#ifdef _DEBUG
      cerr << "polynomial::unload row=" << i << endl;
#endif

    for (unsigned j=0; j<3 && (i*3+j)<coefs.size(); ++j)
    {
      bytes += sprintf (numstr, " %+22.17le", coefs[i*3+j]);
      char* ep = strchr (numstr, 'e');
      if (ep)
        *ep = 'D';

      *outstr += numstr;
    }
    *outstr += newline;
    bytes += 1;
  }
  return bytes;
}

int polynomial::unload (FILE* fptr) const
{
  string out;
  if (unload(&out) < 0)
    return -1;

  int size = (int) out.length();
  int bout = fprintf (fptr, out.c_str());
  if (bout < size)
  {
    fprintf (stderr, "polynomial::unload(FILE*) ERROR fprintf only %d/%d",
	     bout, size);
    perror ("");
    return -1;
  }
  fflush (fptr);
  return bout;
}

Phase polynomial::phase (const MJD& t) const
{
  Phase dp (0.0);
  MJD dt = t - ref_time;
  long double tm = dt.in_minutes();

  long double poweroft = 1.0;
  for (unsigned i=0;i<coefs.size();i++)
  {
    dp += double(coefs[i]*poweroft);
    poweroft *= tm;
  }
  dp += double(tm*ref_freq*60.0);

  return ref_phase + dp;
}

// /////////////////////////////////////////////////////////////////////////
// polynomial::iphase

MJD polynomial::iphase (const Phase& p, const MJD* guess) const
{
  if (guess)
    return Pulsar::inverse_phase (*this, p, *guess);
  else
    return Pulsar::inverse_phase (*this, p);
}

long double polynomial::frequency (const MJD& t) const
{  
  long double dp = 0;                // dphase/dt starts as phase per minute.
  MJD dt = t - ref_time;
  long double tm = dt.in_minutes();

  long double poweroft = 1.0;
  for (unsigned i=1; i<coefs.size(); i++)
  {
    dp+=(long double)(i)*coefs[i]*poweroft;
    poweroft *= tm;
  }
  dp /= (long double) 60.0;          // Phase per second
  dp += ref_freq;
  return dp;
}

double polynomial::chirp (const MJD& t) const
{
  double d2p = 0;                    // d^2phase/dt^2 starts as phase/minute^2
  MJD dt = t - ref_time;
  double tm = dt.in_minutes();

  double poweroft = 1.0;
  for (unsigned i=2; i<coefs.size(); i++)
  {
    d2p+=(double)(i)*(i-1)*coefs[i]*poweroft;
    poweroft *= tm;
  }
  d2p /= (double) 60.0*60.0;          // Phase per second^2

  return d2p;
}

// Seconds per turn = period of pulsar
double polynomial::period (const MJD& t) const
{
   return 1.0/frequency(t);
}

Phase polynomial::dispersion (const MJD& t, long double obs_freq) const
{
  /* compute dispersion phase delay in observatory frame */
  long double dm_delay_in_secs =
    dm/2.41e-4L * (1.0L + doppler_shift*1.0e-4L) *
    (1.0/(obs_freq*obs_freq) - 1.0/(freq*freq));
  return - dm_delay_in_secs * frequency(t);
}

void polynomial::prettyprint() const
{
  cout << "PSR Name\t\t\t" << psrname << endl;
  cout << "Date\t\t\t\t" << date << endl;
  cout << "UTC\t\t\t\t" << utc << endl;
  cout << "Ref. Time\t\t\t" << ref_time.strtempo() << endl;
  cout << "DM\t\t\t\t" << dm << endl;
  if(tempov11){
    cout << "Doppler Shift\t\t\t" << doppler_shift << endl;
    cout << "Log RMS Residuals\t\t" << log_rms_resid << endl;
  }
  cout << "Ref. Phase\t\t\t" << ref_phase.intturns() << ref_phase.fracturns() << endl;
  cout << "Ref. Rotation Frequency\t\t" << ref_freq << endl;
  cout << "Telescope\t\t\t" << telescope << endl;
  cout << "Span in Minutes\t\t\t" << nspan_mins << endl;
  cout << "Number of Coefficients\t\t" << coefs.size() << endl;
  cout << "Reference Frequency\t\t" << freq << endl;
  if(binary){
    cout << "Binary Phase\t\t\t" << binph << endl;
    cout << "Binary Frequency\t\t" << binfreq << endl;
  }
  for(unsigned i=0; i<coefs.size(); ++i) 
    cout << "\tCoeff  " << i+1 << "\t\t" << coefs[i] << endl;
}

bool operator == (const polynomial & p1, const polynomial & p2)
{
  return ! (p1 != p2);
}

bool operator != (const polynomial & p1, const polynomial & p2)
{
  return 
    p1.ref_phase != p2.ref_phase ||
    p1.ref_time != p2.ref_time ||
    p1.dm != p2.dm ||
    p1.doppler_shift != p2.doppler_shift ||
    p1.log_rms_resid != p2.log_rms_resid ||
    p1.ref_freq != p2.ref_freq ||
    p1.telescope != p2.telescope ||
    p1.nspan_mins != p2.nspan_mins ||
    p1.freq != p2.freq ||
    p1.binph != p2.binph ||
    p1.binfreq != p2.binfreq ||
    p1.binary != p2.binary ||
    p1.tempov11 != p2.tempov11;
}

bool time_order (const polynomial & p1, const polynomial & p2)
{
  return p1.get_reftime() < p2.get_reftime();
}

/******************************************/

void polyco::init ()
{
  last_index = -1;
  last_span_epoch = last_span_phase = 0;
}

//! Default constructor
polyco::polyco ()
{
  if (verbose)
    cerr << "polyco::polyco this=" << this << endl;
  init();
}

//! Copy constructor
polyco::polyco (const polyco& copy)
{
  if (verbose)
    cerr << "polyco::polyco copy this=" << this << endl;
  init();
  operator = (copy);
}


const polyco& polyco::operator = (const polyco & copy)
{
  if (this == &copy)
    return *this;

  if (verbose)
    cerr << "polyco::operator= this=" << this << endl;

  pollys = copy.pollys;

  last_index = copy.last_index;

  last_epoch = copy.last_epoch;
  last_span_epoch = copy.last_span_epoch;

  last_phase = copy.last_phase;
  last_span_phase = copy.last_span_phase;

  return *this;
}

polyco::~polyco ()
{
  if (verbose)
    cerr << "polyco::~polyco this=" << this << endl;
}

polyco::polyco (const string& filename)
{
  init ();

  if (load (filename) < 1)  
    throw Error (FailedCall, "polyco::polyco filename",
		 "polyco::load (" + filename + ")");
}

Pulsar::Predictor* polyco::clone () const
{
  return new polyco (*this);
}

Pulsar::Generator* polyco::generator () const
{
  Tempo::Predict* predict = new Tempo::Predict;
  predict->set_nspan( (int) get_nspan() );
  predict->set_ncoef( get_ncoeff() );
  return predict;
}

//! Add the information from the supplied predictor to self
void polyco::insert (const Predictor* other)
{
  const polyco* like = dynamic_cast<const polyco*> (other);
  if (!like)
    throw Error (InvalidParam, "polyco::insert",
		 "Predictor is not a TEMPO polyco");
  append (*like);
}

//! Add the information from the supplied predictor to self
bool polyco::matches (const Predictor* other) const
{
  const polyco* like = dynamic_cast<const polyco*> (other);
  if (!like)
    return false;

  return
    get_telescope () == like->get_telescope() &&
    get_freq() == like->get_freq() &&
    get_psrname() == like->get_psrname() &&
    get_dm() == like->get_dm();
}

void polyco::set_observing_frequency (long double MHz)
{
}

long double polyco::get_observing_frequency () const
{
  return get_freq();
}


char polyco::get_telescope () const
{
  if (pollys.empty()) {
    return '\0';
  }
  else {
    return pollys.front().get_telescope();
  }
}

double polyco::get_freq () const 
{ 
  if (pollys.empty()) {
    return 0.0;
  }
  else {
    return pollys.front().get_freq();
  }
}

MJD polyco::get_reftime () const
{ 
  if (pollys.empty()) {
    return MJD();
  }
  else {
    return pollys.front().get_reftime();
  }
}

double polyco::get_refperiod () const 
{
  if (pollys.empty()) {
    return 0.0;
  }
  else {
    return 1.0/pollys.front().ref_freq;
  }
}

double polyco::get_nspan () const 
{ 
  if (pollys.empty()) {
    return 0.0;
  }
  else {
    return pollys.front().get_nspan();
  }
}

float polyco::get_dm () const
{
  if (pollys.empty()) {
    return 0.0;
  }
  else {
    return pollys.front().get_dm();
  }
}

int polyco::get_ncoeff () const
{
  if (pollys.empty()) {
    return 0;
  }
  else {
    return pollys.front().get_ncoeff();
  }
}

string polyco::get_psrname () const
{ 
  if (pollys.empty()) {
    return "";
  }
  else {
    return pollys.front().get_psrname();
  }
}

int polyco::load (const string& polyco_filename)
{
  if (verbose)
    cerr << "polyco::load (" << polyco_filename << ")" << endl;

  FILE* fptr = fopen (polyco_filename.c_str(), "r");
  if (!fptr)  {
    cerr << "polyco::load cannot open '" << polyco_filename << "' - "
	<< strerror (errno) << endl;
    return -1;
  }

  load (fptr);
  fclose (fptr);

  return pollys.size();
}

void polyco::load (FILE* fptr)
{
  if (verbose)
    cerr << "polyco::load FILE*" << endl;

  string total;
  if (stringload (&total, fptr) < 0)
    throw Error (FailedSys, "polyco::load(FILE*)", "stringload error");

  if (load (&total) <= 0)
    throw Error (InvalidState, "polyco::load(FILE*)", "no polynomials loaded");
}

int polyco::load (string* instr)
{
  if (verbose)
    cerr << "polyco::load string*" << endl;

  int npollys = 0;
  pollys.clear();

  polynomial tst;
  while (instr->length() && tst.load(instr)==0)
  {
    pollys.push_back(tst);      
    npollys++;
  }

  std::sort( pollys.begin(), pollys.end(), time_order );

  if (verbose)
    cerr << "polyco::load (string*) return npollys=" << npollys << endl;

  return npollys;
}

int polyco::unload (const string& filename) const
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)  {
    cerr << "polyco::unload cannot open '" << filename << "' - "
        << strerror (errno) << endl;
    return -1;
  }
  try {
    unload (fptr);
    return 0;
  }
  catch (...) {
    return -1;
  }
}

// ///////////////////////////////////////////////////////////////////
// polyco::unload (string*)
//
// Adds characters to the string to which outstr points.  
// The text added is the tempo formatted text of this polynomial.  
// Return value: the number of characters added (not including the \0)
// ///////////////////////////////////////////////////////////////////
int polyco::unload (string* outstr) const {
  int bytes = 0;

  if (verbose)
    cerr << "polyco::unload " << pollys.size() << " polynomials" << endl;

  for (unsigned i=0; i<pollys.size(); ++i)
    bytes += pollys[i].unload(outstr);

  return bytes;
}

void polyco::unload (FILE* fptr) const
{
  if (verbose)
    cerr << "polyco::unload(FILE*)" << endl;

  string out;
  if (unload(&out) < 0)
    throw Error (InvalidState, "polyco::unload(FILE*)",
		 "polyco::unload (string*) failed");

  int size = (int) out.length();
  int bout = fprintf (fptr, out.c_str());

  if (verbose)
    cerr << "polyco::unload(FILE*) size=" << size << " wrote=" << bout << endl;

  if (bout < size)
    throw Error (FailedSys, "polyco::unload(FILE*)",
		 "fprintf only %d/%d bytes written", bout, size);
}

void polyco::append (const polyco& poly)
{
  if (verbose)
    cerr << "polyco::append have=" << pollys.size() << " get="
	 << poly.pollys.size() << " polynomials" << endl;

  for (unsigned iget=0; iget<poly.pollys.size(); ++iget)
  {
    bool is_new = true;
    for (unsigned ihave=0; ihave<pollys.size(); ihave++)
      if (pollys[ihave] == poly.pollys[iget])
      {
	if (verbose)
        {
	  cerr << "polyco::append have["<< ihave <<"]=" << endl;
	  pollys[ihave].unload(stderr);
	  cerr << "polyco::append equals get["<< iget <<"]=" << endl;
	  poly.pollys[iget].unload(stderr);
	}
	is_new = false;
      }

    if (is_new)
    {
      if (verbose)
      {
	cerr << "polyco::append new get["<< iget <<"]=" << endl;
	poly.pollys[iget].unload(stderr);
      }

      pollys.push_back (poly.pollys[iget]);
    }
    else if (verbose)
      cerr << "polyco::append already have iget=" << iget << endl;
  }

  std::sort( pollys.begin(), pollys.end(), time_order );
}

//! Keep only the components required to span the given epochs
void polyco::keep (const std::vector<MJD>& epochs)
{
  unsigned i = 0;

  for (i=0; i<pollys.size(); ++i) 
    pollys[i].keep = false;

  for (i=0; i<epochs.size(); i++)
  {
    int ipolly = i_nearest( epochs[i] );

    // if not found, then don't worry about it
    if (ipolly < 0)
      continue;

    pollys[ipolly].keep = true;
  }

  unsigned orig_npoly = pollys.size();

  i=0;
  while (i < pollys.size())
    if ( !pollys[i].keep )
      pollys.erase( pollys.begin() + i );
    else
      i++;

  if (verbose)
    cerr << "polyco::keep had " << orig_npoly 
	 << " kept " << pollys.size() << endl;
}

void polyco::prettyprint() const 
{
  for(unsigned i=0; i<pollys.size(); ++i) 
    pollys[i].prettyprint();
}

// returns a pointer to the best polynomial for use over the period
// defined by t1 to t2
const polynomial* polyco::nearest (const MJD &t) const
{
  int ipolly = i_nearest (t);

  if (ipolly < 0)
    throw Error (InvalidParam, "polyco::nearest",
                 "no polynomial for MJD=" + t.printdays(13));

  return &pollys[ipolly];
}

const polynomial& polyco::best (const MJD &t) const
{
  return pollys[ i_nearest (t, true) ];
}

const polynomial& polyco::best (const Phase& p) const
{
  return pollys[ i_nearest (p, true) ];
}

void polyco::set_last (int i) const
{
  const polynomial* poly = &(pollys[i]);

  last_epoch = poly->ref_time;
  last_span_epoch
    = 0.5 * (poly->end_time(0.0)-poly->start_time(0.0)).in_minutes();

  last_phase = poly->ref_phase;
  last_span_phase
    = 0.5 * (poly->end_phase(0.0)-poly->start_phase(0.0)).in_turns();

  last_index = i;

  if (verbose)
    cerr << "polyco::set_last index=" << i << " limits: "
	 << " minutes=" << last_span_epoch 
	 << " turns=" << last_span_phase << endl;
}

int polyco::i_nearest (const MJD &t, bool throw_exception) const
{
  if (verbose)
    cerr << "polyco::i_nearest this=" << this << endl;

  if (last_index >= 0
      && fabs((pollys[last_index].ref_time-t).in_minutes()) < last_span_epoch)
  {
    if (verbose)
      cerr << "polyco::i_nearest using last_index=" << last_index << endl;
    return last_index;
  }

  float min_dist = FLT_MAX;
  int imin = -1;

  if (verbose)
    cerr << "polyco::i_nearest epoch=" << t.printdays(10) << endl;

  for (unsigned ipolly=0; ipolly<pollys.size(); ipolly ++)  {

    float dist = fabs ( (pollys[ipolly].ref_time - t).in_minutes() );

    if (verbose)  {
      cerr << "polyco::i_nearest distance=" << dist << " minutes";
      if (min_dist != FLT_MAX)
        cerr << " (min=" << min_dist << ")";
      cerr << endl;
    }

    if (dist < min_dist) {
      imin = ipolly;
      min_dist = dist;
    }
  }

  if (verbose)
    cerr << "polyco::i_nearest imin=" << imin << endl;

  // check if any polynomial matched
  if (imin < 0)
  {
    if (verbose)
      cerr << "polyco::i_nearest - no polynomial found" << endl;

    if (throw_exception)
      throw Error (InvalidParam, "polyco::i_nearest",
                   "no polynomial for epoch=" + t.printdays( 15 ));

    return -1;
  }

  // return if the time is within the range of the matched polynomial
  if ( (t > pollys[imin].start_time()) && (t < pollys[imin].end_time()) )
  {
    set_last (imin);
    return imin;
  }

  if (throw_exception)
    throw Error (InvalidParam, "polyco::i_nearest",
                 "no polynomial for MJD " + t.printdays(15) + "\n\t"
                 "closest range: " + pollys[imin].start_time().printdays(15)
                 + " - " + pollys[imin].end_time().printdays(15));

  // the time is out of range of the nearest polynomial
  if (verbose)
    cerr << "polyco::i_nearest closest polynomial does not span epoch=" 
         << t.printdays(15) << endl;

  return -1;
}

int polyco::i_nearest (const Phase& p, bool throw_exception) const
{
  if (last_index >= 0
      && fabs((pollys[last_index].ref_phase-p).in_turns()) < last_span_phase)
  {
    if (verbose)
      cerr << "polyco::i_nearest using last_index=" << last_index << endl;
    return last_index;
  }

  float min_dist = FLT_MAX;
  int imin = -1;

  if (verbose)
    cerr << "polyco::i_nearest phase=" << p << endl;

  for (unsigned ipolly=0; ipolly<pollys.size(); ipolly ++)  {

    float dist = fabs ( (pollys[ipolly].ref_phase - p).in_turns() );

    if (verbose)  {
      cerr << "polyco::i_nearest ref_phase=" << pollys[ipolly].ref_phase
	   << " distance=" << dist << " turns";
      if (min_dist != FLT_MAX)
        cerr << " (min=" << min_dist << ")";
      cerr << endl;
    }

    if (dist < min_dist) {
      imin = ipolly;
      min_dist = dist;
    }
  }

  // check if any polynomial matched
  if (imin < 0)
  {
    if (verbose)
      cerr << "polyco::i_nearest - no polynomial found" << endl;
    if (throw_exception)
      throw Error (InvalidParam, "polyco::i_nearest",
                   "no polynomial for phase=" + p.strprint( 15 ));
    return -1;
  }

  // return imin if the phase is within the range of the matched polynomial
  if ( (p > pollys[imin].start_phase()) &&
       (p < pollys[imin].end_phase()) ) {
    set_last (imin);
    return imin;
  }

  if (throw_exception)
    throw Error (InvalidParam, "polyco::i_nearest",
                 "no polynomial for Phase " + p.strprint(15) + "\n\t"
                 "closest range: " + pollys[imin].start_phase().strprint(15)
                 + " - " + pollys[imin].end_phase().strprint(15));

  // the time is out of range of the nearest polynomial
  if (verbose)
    cerr << "polyco::i_nearest closest polynomial does not span phase=" 
         << p.strprint(15) << endl;

  return -1;
}

bool polyco::is_tempov11() const
{
  for (unsigned i=0; i<pollys.size(); ++i)
    if (!pollys[i].is_tempov11())
      return pollys[i].is_tempov11();
  return pollys[0].is_tempov11();
}

bool operator == (const polyco & p1, const polyco & p2)
{
  if (p1.pollys.size() != p2.pollys.size())
    return false;
  for (unsigned i=0; i<p1.pollys.size(); ++i)
    if (p1.pollys[i] != p2.pollys[i])
      return false;
  return true;
}

bool operator != (const polyco & p1, const polyco & p2)
{
  return ! (p1 == p2);
}
