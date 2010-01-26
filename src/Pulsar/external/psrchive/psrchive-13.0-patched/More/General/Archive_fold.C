/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  \param nfold the number of sections to integrate
*/
void Pulsar::Archive::fold (unsigned nfold)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> fold (nfold);

  set_nbin (get_Integration(0)->get_nbin());
}

