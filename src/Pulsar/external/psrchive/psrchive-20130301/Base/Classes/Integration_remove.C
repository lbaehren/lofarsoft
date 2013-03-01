/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <algorithm>

using namespace std;

void Pulsar::Integration::remove (unsigned ichan)
{
  remove(ichan,ichan);
}

void Pulsar::Integration::remove (unsigned ichan_first, unsigned ichan_last)
try
{
  unsigned npol  = get_npol();
  unsigned nchan = get_nchan();

  if (ichan_last>nchan-1 || ichan_first>nchan-1)
    throw Error(InvalidRange, "Pulsar::Integration::remove",
        "channel out of range");

  if (ichan_first > ichan_last)
  {
    unsigned tmp = ichan_first;
    ichan_first = ichan_last;
    ichan_last = tmp;
  }

  // Inclusive remove range
  int nchan_to_remove = ichan_last - ichan_first + 1; 
  unsigned new_nchan = nchan - nchan_to_remove;

  for (unsigned ipol=0; ipol < npol; ipol++)
    profiles[ipol].erase(profiles[ipol].begin() + ichan_first, 
        profiles[ipol].begin() + ichan_last + 1);

  set_nchan (new_nchan);
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::remove";
}

