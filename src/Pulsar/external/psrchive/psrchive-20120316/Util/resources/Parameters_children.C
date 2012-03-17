/***************************************************************************
 *
 *   Copyright (C) 2007-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Pulsar/TextParameters.h"
#include "psrephem.h"

#ifdef HAVE_TEMPO2
#include "T2Parameters.h"
#endif

using namespace std;

void Pulsar::Parameters::children (vector< Reference::To<Parameters> >& child)
{
  child.resize (0);

  child.push_back (new Pulsar::TextParameters);
  child.push_back (new Legacy::psrephem);

#ifdef HAVE_TEMPO2
  child.push_back (new Tempo2::Parameters);
#endif
}
