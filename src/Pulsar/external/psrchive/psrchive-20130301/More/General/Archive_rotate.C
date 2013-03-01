/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

void Pulsar::Archive::rotate (double time)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> rotate (time);
}

void Pulsar::Archive::rotate_phase (double phase)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> rotate_phase (phase);
}

