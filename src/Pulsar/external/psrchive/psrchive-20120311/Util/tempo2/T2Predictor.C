/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "T2Generator.h"

#include "tempo2pred_int.h"
#include "inverse_phase.h"
#include "Error.h"

//#include <vector>
using namespace std;

Tempo2::Predictor::Predictor ()
{
  T2Predictor_Init (&predictor);
  observing_frequency = 0;
}

Tempo2::Predictor::Predictor (const Predictor& copy)
{
  T2Predictor_Init (&predictor);
  T2Predictor_Copy (&predictor, &copy.predictor);
  observing_frequency = copy.observing_frequency;
}

Tempo2::Predictor::~Predictor ()
{
  T2Predictor_Destroy (&predictor);
}

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Predictor::clone () const
{
  if (verbose)
    cerr << "Tempo2::Predictor::clone" << endl;
  return new Predictor (*this);
}

Pulsar::Generator* Tempo2::Predictor::generator () const
{
  Tempo2::Generator* t2g = new Tempo2::Generator;

  t2g->set_time_ncoeff( get_time_ncoeff() );
  t2g->set_frequency_ncoeff( get_frequency_ncoeff() );
  t2g->set_segment_length( get_segment_length() );

  return t2g;
}

//! Set the number of time coefficients
unsigned Tempo2::Predictor::get_time_ncoeff () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].cheby.nx;
}

//! Set the number of frequency coefficients
unsigned Tempo2::Predictor::get_frequency_ncoeff () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].cheby.ny;
}

//! Set the length of each polynomial segment in days
long double Tempo2::Predictor::get_segment_length () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].mjd_end
    - predictor.modelset.cheby.segments[0].mjd_start;
}

//! Add the information from the supplied predictor to self
void Tempo2::Predictor::insert (const Pulsar::Predictor* from)
{
  const Predictor* t2p = dynamic_cast<const Predictor*>(from);
  if (!t2p)
    throw Error (InvalidParam, "Tempo2::Predictor::insert",
		 "Predictor is not a Tempo2 Predictor");

  T2Predictor_Insert (&predictor, &t2p->predictor);
}

void Tempo2::Predictor::keep (const std::vector<MJD>& epochs)
{
  if (verbose)
    cerr << "Tempo2::Predictor::keep not implemented" << endl;
}

string Tempo2::Predictor::get_psrname () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return "";
  return predictor.modelset.cheby.segments[0].psrname;
}

string Tempo2::Predictor::get_sitename () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return "";
  return predictor.modelset.cheby.segments[0].sitename;
}

long double Tempo2::Predictor::get_freq_start () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].freq_start;
}

long double Tempo2::Predictor::get_freq_end () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].freq_end;
}

long double Tempo2::Predictor::get_dispersion_constant () const
{
  if (predictor.modelset.cheby.nsegments < 1)
    return 0;
  return predictor.modelset.cheby.segments[0].dispersion_constant;
}

bool Tempo2::Predictor::matches (const Pulsar::Predictor* other) const
{
  const Predictor* t2p = dynamic_cast<const Predictor*>(other);
  if (!t2p)
    return false;

  return 
    get_psrname () == t2p->get_psrname() &&
    get_sitename () == t2p->get_sitename() &&
    get_freq_start () == t2p->get_freq_start() &&
    get_freq_end () == t2p->get_freq_end() &&
    get_dispersion_constant () == t2p->get_dispersion_constant();
}

//! Set the observing frequency at which predictions will be made
void Tempo2::Predictor::set_observing_frequency (long double MHz)
{
  observing_frequency = MHz;
}

//! Get the observing frequency at which phase and epoch are related
long double Tempo2::Predictor::get_observing_frequency () const
{
  return observing_frequency;
}

//! convert an MJD to long double
long double from_MJD (const MJD& t)
{
  const long double secs_in_day = 86400.0L;

  return 
    (long double) (t.intday()) +
    (long double) (t.get_secs()) / secs_in_day +
    (long double) (t.get_fracsec()) / secs_in_day;
}

//! convert a long double to an MJD
MJD to_MJD (long double t)
{
  long double ld = floorl(t);
  t -= ld;
  long double lsec = t * 86400.0L;
  long double ls = floorl(lsec);
  lsec -= ls;

  return MJD (double(ld), double(ls), double(lsec));
}

//! convert a long double to Phase
Phase to_Phase (long double p)
{
  int64_t turns = int64_t (p);
  double fturns = p - turns;

  return Phase (turns, fturns);
}

//! Return the phase, given the epoch
Phase Tempo2::Predictor::phase (const MJD& t) const
{
  if (verbose)
    cerr << "Tempo2::Predictor::phase epoch=" << t << " frequency=" 
	 << observing_frequency << endl;

  if (observing_frequency <= 0)
    Error error (InvalidState, "Tempo2::Predictor::phase",
		 "observing_frequency=%lf", (double) observing_frequency);

  long double p = T2Predictor_GetPhase ( &predictor, from_MJD (t),
					 observing_frequency );

  if (ChebyModelSet_OutOfRange)
    throw Error (InvalidParam, "Tempo2::Predictor::phase",
		 "epoch %s not spanned by ChebyModelSet",
		 t.printdays(20).c_str());

  if (!finite(p)) {
    Error error (InvalidState, "Tempo2::Predictor::phase",
		 "T2Predictor_GetPhase result = ");
    error << p;
    throw error;
  }

  return to_Phase( p );
}

//! Return the spin frequency, given the epoch
long double Tempo2::Predictor::frequency (const MJD& t) const
{
  long double f = T2Predictor_GetFrequency (&predictor, from_MJD (t),
					    observing_frequency);

  if (ChebyModelSet_OutOfRange)
    throw Error (InvalidParam, "Tempo2::Predictor::frequency",
		 "epoch %s not spanned by ChebyModelSet",
		 t.printdays(20).c_str());

  return f;
}

//! Return the phase correction for dispersion delay
Phase Tempo2::Predictor::dispersion (const MJD &t, long double MHz) const
{
  return get_dispersion_constant() / (MHz*MHz);
}

void Tempo2::Predictor::load (FILE* fptr)
{
  if (T2Predictor_FRead (&predictor, fptr) < 0)
    throw Error (InvalidParam, "Tempo2::Predictor::load",
                 "failed T2Predictor_FRead");

  observing_frequency = 0.5L *
    (T2Predictor_GetStartFreq(&predictor)+T2Predictor_GetEndFreq(&predictor));

  if (verbose)
    cerr << "Tempo2::Predictor::load observing_frequency="
         << observing_frequency << endl;
}

void Tempo2::Predictor::unload (FILE* fptr) const
{
  T2Predictor_FWrite (&predictor, fptr);
}

//
// The rest of this code implements an interface to the inverse_phase
// template method, which is used to implement Tempo2::Predictor::iphase.
//

class cheby_interface {

public:

  static double precision;

  void construct (ChebyModel*, long double obs_freq);

  MJD get_reftime() const { return t0; }
  Phase get_refphase() const { return p0; }
  long double get_reffrequency() const { return f0; }

  Phase phase (const MJD&) const;
  long double frequency (const MJD&) const;

  MJD iphase (const Phase&, const MJD*) const;

protected:

  ChebyModel* model;
  long double obs_freq;

  MJD t0;
  Phase p0;
  long double f0;
  
};

// Half nanosecond precision
double cheby_interface::precision = 5e-10;

void cheby_interface::construct (ChebyModel* _model, long double _obs_freq)
{
  model = _model;
  obs_freq = _obs_freq;

  long double tmid = 0.5 * (model->mjd_start + model->mjd_end);
  t0 = to_MJD( tmid );
  p0 = to_Phase( ChebyModel_GetPhase (model, tmid, obs_freq) );
  f0 = ChebyModel_GetFrequency (model, tmid, obs_freq);
  // cerr << "t0=" << t0 << " p0=" << p0 << " f0=" << f0 << endl;
}

Phase cheby_interface::phase (const MJD& t) const
{
 return to_Phase( ChebyModel_GetPhase (model, from_MJD (t), obs_freq) );
}

long double cheby_interface::frequency (const MJD& t) const
{
 return ChebyModel_GetFrequency (model, from_MJD (t), obs_freq);
}

MJD cheby_interface::iphase (const Phase& phase, const MJD* guess) const
{
  if (guess)
    return Pulsar::inverse_phase (*this, phase, *guess);
  else
    return Pulsar::inverse_phase (*this, phase);
}

//! Return the epoch, given the phase
MJD Tempo2::Predictor::iphase (const Phase& phase, const MJD* guess) const
{
  vector<cheby_interface> chebys ( predictor.modelset.cheby.nsegments );

  float min_dist = 0;
  int imin = -1;
  unsigned icheby = 0;

  if (verbose)
    cerr << "Tempo2::Predictor::iphase observing_frequency=" 
	 << observing_frequency << endl;

  for (icheby=0; icheby<chebys.size(); icheby ++)
  {
    if (verbose)
      cerr << "Tempo2::Predictor::iphase icheby=" << icheby << endl;

    chebys[icheby].construct( predictor.modelset.cheby.segments + icheby,
			      observing_frequency );

    float dist = fabs ( (chebys[icheby].get_refphase() - phase).in_turns() );

    if (icheby == 0 || dist < min_dist)
    {
      imin = icheby;
      min_dist = dist;
    }
  }

  if (imin < 0)
    throw Error (InvalidParam, "Tempo2::Predictor::iphase",
                 "no cheby for phase");

  return chebys[imin].iphase (phase, guess);
}

