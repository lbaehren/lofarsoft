/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/IntegrationExpert.h"
#include "psrephem_orbital.h"

using namespace std;
using Legacy::psrephem;

//! Default constructor
Pulsar::PeriastronOrder::PeriastronOrder ()
  : IntegrationOrder ("BINPHSPERI")
{
  IndexState = "Binary Phase (w.r.t. Periastron)";
  Unit = "phase";
}

//! Destructor
Pulsar::PeriastronOrder::~PeriastronOrder ()
{
}

//! Copy constructor
Pulsar::PeriastronOrder::PeriastronOrder (const PeriastronOrder& extension)
  : IntegrationOrder ("BINPHSPERI")
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
}

//! Operator =
const Pulsar::PeriastronOrder&
Pulsar::PeriastronOrder::operator= (const PeriastronOrder& extension)
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::PeriastronOrder::clone () const
{
  return new PeriastronOrder( *this );
}

// These are the methods that do the real work:

void Pulsar::PeriastronOrder::organise (Archive* arch, unsigned newsub) 
{
  // Define a vector to hold the binary phases
  vector<float> phases;
  // Define a vector of flags to help avoid counting
  // subints twice when re-ordering
  vector<bool>  used;

  float minphs = 1.0;
  float maxphs = 0.0;
  
  // Pad to avoid thorwing exceptions when get_Integration is called
  indices.resize(arch->get_nsubint());

  Reference::To<const psrephem> eph = new_psrephem( arch->get_ephemeris() );

  for (unsigned i = 0; i < arch->get_nsubint(); i++) 
  {
    phases.push_back(get_binphs_peri((arch->get_Integration(i)->get_epoch()).in_days(),
				       *eph, 
				       arch->get_Integration(i)->get_centre_frequency(),
				       arch->get_telescope()[0]));
    if (phases[i]!=phases[i]) {
      throw Error(FailedCall, "PeriastronOrder::organise",
		  "get_binphs_peri returned nan");
    }
    
    used.push_back(false);

    if (phases[i] > maxphs)
      maxphs = phases[i];
    
    if (phases[i] < minphs)
      minphs = phases[i];
  }
 
  // Interperate the newsub parameter as the number of integrations
  // required across a full phase wrap. This must be adjusted depending
  // on the phase coverage available in the archive

  float    phs_coverage = maxphs - minphs;
  unsigned mysub        = 0;

  if (phs_coverage == 0.0)
    mysub = 1;
  else
    mysub = unsigned(ceil(phs_coverage * float(newsub)));
  
  // This is equivalent to 1.0 / newsub given the above condition
  float PhaseGap = phs_coverage / float(mysub);

  // The "used" vector will ensure that no subints are counted
  // twice, but since they cannot be sub-divided, if you ask
  // for more subints in the resulting archive than effectively
  // exist in the original, the end result will include blank
  // integrations.

  Reference::To<Pulsar::Archive> copy = arch->clone();
  Reference::To<Pulsar::Integration> integ = 0;

  // Resize for the new configuration
  arch->resize(mysub);

  for (unsigned i = 0; i < mysub; i++) {
    arch->get_Integration(i)->zero();
    bool first = true;
    int tally = 0;
    for (unsigned j = 0; j < phases.size(); j++) {
      if ((phases[j] >= (minphs + (i*PhaseGap))) && 
	  (phases[j] <= (minphs + ((i+1)*PhaseGap))) && !used[j]) {
	if (first) {
	  arch->get_Integration(i)->expert()->copy(copy->get_Integration(j),false);
	  set_Index(i, phases[j]);
	  used[j] = true;
	  tally += 1;
	  first = false;
	}
	else {
	  arch->get_Integration(i)->expert()->combine(copy->get_Integration(j));
	  indices[i] += phases[j];
	  used[j] = true;
	  tally += 1;
	}
      }
    }
    if (tally > 0)
      indices[i] /= tally;
  }
}

void Pulsar::PeriastronOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "PeriastronOrder::append",
	      "This method is not implemented");
}

void Pulsar::PeriastronOrder::combine (Archive* arch, unsigned nscr)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::PeriastronOrder::combine nscr=" << nscr << endl;

  if (nscr == 0)
    return;

  vector<double> oldind(indices);
  
  unsigned newsub = 0;
  if ((arch->get_nsubint() % nscr) == 0)
    newsub = arch->get_nsubint() / nscr;
  else
    newsub = (arch->get_nsubint() / nscr) + 1;
  
  Reference::To<Pulsar::Archive> copy = arch->clone();
  arch->resize(0);
  arch->resize(newsub);
  indices.resize(newsub);

  unsigned count = 0;
  for (unsigned i = 0; i < newsub; i++) {
    *(arch->get_Integration(i)) = 
      *(arch->new_Integration(copy->get_Integration(count)));
    count++;
    for (unsigned j = 1; j < nscr; j++) {
      if (count >= copy->get_nsubint())
	return;
      arch->get_Integration(i)->expert()->combine(copy->get_Integration(count));
      count++;
    }
  }

  count = 0;
  for (unsigned i = 0; i < newsub; i++) {
    indices[i] = oldind[count];
    count++;
    for (unsigned j = 1; j < nscr; j++) {
      if (count >= oldind.size()) {
	indices[i] /= j;
	return;
      }
      indices[i] += oldind[count];
      count++;
    }
    indices[i] /= nscr;
  }
}
