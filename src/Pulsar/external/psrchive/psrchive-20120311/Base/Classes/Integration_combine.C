/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

/*! Users of this method should be aware that it does not keep
  full track of all the parameters of an Integration. It is
  designed to be used for low level arithmetic style addition
  only, not fully consistent combination. The supplied tscrunch
  routines should be used for such purposes.

  This method is provided for experimental purposes and should
  not be used lightly. If the time spans of the two combined
  subints are not contiguous, several parameters in the result are
  no longer meaningful. Also, there is no way of knowing whether
  the profiles are aligned in phase. Developers who use this
  operator should perform the necessary rotations at the archive
  level (where the polyco resides) before summing the data in the
  Integrations. */

void Pulsar::Integration::combine (const Integration* from)
{
  float total_weight = 0.0;
  
  for (unsigned i = 0; i < from->get_nchan(); i++) {
    total_weight += from->get_weight(i);
  }
  
  if (total_weight <= 0.0)
    return;
  
  string reason;
  
  if (!mixable(from, reason))
    throw Error(InvalidParam, "Pulsar::IntegrationOrder::combine", reason);
  
  for (unsigned i = 0; i < get_nchan(); i++)
    for (unsigned j = 0; j < get_npol(); j++)
      get_Profile(j,i)->average( from->get_Profile(j,i) );
  
  double total = get_duration() + from->get_duration();
  
  set_duration(total);
  
}

