/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <assert.h>

#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "mini++.h"

using namespace std;

//! Null constructor simply intializes defaults
void Pulsar::TimerIntegration::init()
{
  Mini::init (mini);

  npol = nchan = nbin = 0;
}

void Pulsar::TimerIntegration::resize (unsigned _npol,
				       unsigned _nchan,
				       unsigned _nbin)
{
  if (verbose)
    cerr << "TimerIntegration::resize (npol=" << _npol 
	 << ", nchan=" << _nchan << ", nbin=" << _nbin << ")" << endl;

  Integration::resize (_npol, _nchan, _nbin);

  wts.resize (nchan);
  med.resize (npol);
  bpass.resize (npol);

  for (unsigned ipol=0; ipol < npol; ipol++) {
    med[ipol].resize(nchan);
    bpass[ipol].resize(nchan);
  }
}

void Pulsar::TimerIntegration::insert (Integration* from)
{
  unsigned original_nchan = get_nchan();

  Integration::insert (from);

  TimerIntegration* timer = dynamic_cast<TimerIntegration*> (from);
  if (!timer)
    return;

  unsigned from_nchan = from->get_nchan();

  wts.resize (original_nchan + from_nchan);
  for (unsigned ichan=0; ichan < from_nchan; ichan++)
    wts[original_nchan+ichan] = timer->wts[ichan];

  for (unsigned ipol=0; ipol < npol; ipol++) {
    med[ipol].resize (original_nchan + from_nchan);
    bpass[ipol].resize (original_nchan + from_nchan);
    for (unsigned ichan=0; ichan < from_nchan; ichan++) {
      med[ipol][original_nchan+ichan] = timer->med[ipol][ichan];
      bpass[ipol][original_nchan+ichan] = timer->bpass[ipol][ichan];
    }
  }
}

Pulsar::TimerIntegration::~TimerIntegration ()
{
  if (verbose)
    cerr << "TimerIntegration destructor" << endl;
}

//! Copy constructor
Pulsar::TimerIntegration::TimerIntegration (const TimerIntegration& t_subint)
{
  if (verbose)
    cerr << "TimerIntegration construct copy TimerIntegration" << endl;

  init();
  TimerIntegration::copy (&t_subint);
}

//! General copy constructor
Pulsar::TimerIntegration::TimerIntegration (const Integration& subint)
{
  if (verbose)
    cerr << "TimerIntegration construct copy Integration" << endl;

  init();
  TimerIntegration::copy (&subint);
}

void Pulsar::TimerIntegration::copy (const Integration* subint, bool manage)
{
  if (verbose)
    cerr << "TimerIntegration::copy management=" << manage << endl;

  if (this == subint)
    return;

  Integration::copy (subint, manage);

  const TimerIntegration* t_subint;
  t_subint = dynamic_cast<const TimerIntegration*> (subint);

  if (!t_subint)
    return;

  if (verbose)
    cerr << "TimerIntegration::copy another TimerIntegration" << endl;

  mini = t_subint->mini;
  wts = t_subint->wts;
  med = t_subint->med;
  bpass = t_subint->bpass;
}

//! Return the pointer to a new copy of self
Pulsar::Integration*
Pulsar::TimerIntegration::clone () const
{
  return new TimerIntegration (*this);
}

MJD Pulsar::TimerIntegration::get_epoch () const
{
  return Mini::get_MJD (mini);
}

void Pulsar::TimerIntegration::set_epoch (const MJD& mjd)
{
  Mini::set_MJD (mini, mjd);
}

double Pulsar::TimerIntegration::get_duration () const
{
  return mini.integration;
}

void Pulsar::TimerIntegration::set_duration (double seconds)
{
  mini.integration = seconds;
}

double Pulsar::TimerIntegration::get_folding_period () const
{
  return mini.pfold;
}

void Pulsar::TimerIntegration::set_folding_period (double seconds)
{
  mini.pfold = seconds;
}

