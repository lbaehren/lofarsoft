/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/IntegrationExpert.h"
#include "psrephem_orbital.h"

using namespace std;
using Legacy::psrephem;

//! Default constructor
Pulsar::BinLngAscOrder::BinLngAscOrder ()
  : IntegrationOrder ("BINLNGASC")
{
  IndexState = "Binary Longitude (w.r.t. Asc. Node)";
  Unit = "degrees";
}

//! Destructor
Pulsar::BinLngAscOrder::~BinLngAscOrder ()
{

}

//! Copy constructor
Pulsar::BinLngAscOrder::BinLngAscOrder (const BinLngAscOrder& extension)
  : IntegrationOrder ("BINLNGASC")
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
}

//! Operator =
const Pulsar::BinLngAscOrder&
Pulsar::BinLngAscOrder::operator= (const BinLngAscOrder& extension)
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::BinLngAscOrder::clone () const
{
  return new BinLngAscOrder( *this ); 
}

// These are the methods that do the real work:

void Pulsar::BinLngAscOrder::organise (Archive* arch, unsigned newsub) 
{
  // Define a vector to hold the binary lngs
  vector<float> lngs;

  // Define a vector of flags to help avoid counting
  // subints twice when re-ordering
  vector<bool>  used;

  float minlng = 360.0;
  float maxlng = 0.0;
  
  // Pad to avoid thorwing exceptions when get_Integration is called
  indices.resize(arch->get_nsubint());

  Reference::To<const psrephem> eph = new_psrephem( arch->get_ephemeris() );

  for (unsigned i = 0; i < arch->get_nsubint(); i++)
  {
    lngs.push_back(get_binlng_asc((arch->get_Integration(i)->get_epoch()).in_days(),
				  *eph, 
				  arch->get_Integration(i)->get_centre_frequency(),
				  arch->get_telescope()[0]));
    if (lngs[i] != lngs[i]) {
      throw Error(FailedCall, "BinLngAscOrder::organise",
		  "get_binlng_asc returned nan");
    }
    
    used.push_back(false);

    // The problem with this section is that archives with gaps in the
    // longitude coverage have the total coverage mis-represented. This
    // should just result in blank space in the end result, but I have
    // noticed on several occasions that this does not work properly.
    // AWH 9/1/2004
    
    if (lngs[i] > maxlng)
      maxlng = lngs[i];
    
    if (lngs[i] < minlng)
      minlng = lngs[i];
  }

  // Interperate the newsub parameter as the number of integrations
  // required across a full wrap. This must be adjusted depending
  // on the longitude coverage available in the archive
  
  float    lng_coverage = maxlng - minlng;
  unsigned mysub        = 0;
  
  if (lng_coverage == 0.0)
    mysub = 1;
  else
    mysub = unsigned(ceil(lng_coverage/360.0 * float(newsub)));

  // This is equivalent to 360.0 / newsub given the above condition
  float LngGap = lng_coverage / float(mysub);

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
    for (unsigned j = 0; j < lngs.size(); j++) {
      if ((lngs[j] >= (minlng + (i*LngGap))) && 
	  (lngs[j] <= (minlng + ((i+1)*LngGap))) && !used[j]) {
	if (first) {
	  arch->get_Integration(i)->expert()->copy(copy->get_Integration(j), false);
	  set_Index(i, lngs[j]);
	  used[j] = true;
	  tally += 1;
	  first = false;
	}
	else {
	  arch->get_Integration(i)->expert()->combine(copy->get_Integration(j));
	  indices[i] += lngs[j];
	  used[j] = true;
	  tally += 1;
	}
      }
    }
    if (tally > 0)
      indices[i] /= tally;
  }
}

void Pulsar::BinLngAscOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "BinLngAsc::append",
	      "This method is not implemented");
}

void Pulsar::BinLngAscOrder::combine (Archive* arch, unsigned nscr)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::BinLngAscOrder::combine nscr=" << nscr << endl;

  if (nscr == 0)
    return;

  vector<double> oldind = indices;
  
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
