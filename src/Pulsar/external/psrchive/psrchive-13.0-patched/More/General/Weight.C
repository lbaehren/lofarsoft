/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Weight.h"
#include "Pulsar/Archive.h"

void Pulsar::Weight::operator () (Archive* archive)
{
  for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
    weight (archive->get_Integration (isub));
}
