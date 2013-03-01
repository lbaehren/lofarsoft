/***************************************************************************
 *
 *   Copyright (C) 2006 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeSortedOrder.h"

#include <algorithm>
//#include <utility>

using namespace std;

//! Default constructor
Pulsar::TimeSortedOrder::TimeSortedOrder ()
  : IntegrationOrder ("TimeSortedOrder")
{
  IndexState = "Time Sorted";
  Unit = "time";
}

//! Destructor
Pulsar::TimeSortedOrder::~TimeSortedOrder ()
{

}

//! Copy constructor
Pulsar::TimeSortedOrder::TimeSortedOrder (const TimeSortedOrder& extension)
  : IntegrationOrder ("TimeSortedOrder")
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
}

//! Operator =
const Pulsar::TimeSortedOrder&
Pulsar::TimeSortedOrder::operator= (const TimeSortedOrder& extension)
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::TimeSortedOrder::clone () const
{
  return new TimeSortedOrder( *this ); 
}

class Subint{
public:
  Subint(){}
  Subint(MJD _mjd,Reference::To<Pulsar::Integration> _integration)
  { mjd = _mjd; integration = _integration; }

  bool operator<(const Subint& s) const
  { return mjd < s.mjd; }

  MJD mjd;
  Reference::To<Pulsar::Integration> integration;
};

// These are the methods that do the real work:
void Pulsar::TimeSortedOrder::organise (Archive* arch, unsigned) 
{
  vector<Subint> subints(arch->get_nsubint());

  for( unsigned i=0; i<subints.size(); i++)
    subints[i] = Subint( arch->get_Integration(i)->get_start_time(),arch->get_Integration(i) );

  // Assume sorts on first attribute
  sort(subints.begin(),subints.end());

  arch->resize( 0 );

  for( unsigned i=0; i<subints.size(); i++)
    arch->manage( subints[i].integration );
  
  indices.resize( subints.size() );
  for( unsigned i=0; i<subints.size(); i++)
    indices[i] = i;
}

void Pulsar::TimeSortedOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "TimeSortedOrder::append",
	      "This method is not implemented");
}

void Pulsar::TimeSortedOrder::combine (Archive* arch, unsigned nscr)
{
  throw Error(FailedCall, "TimeSortedOrder::combine",
	      "This method is not implemented");
}
