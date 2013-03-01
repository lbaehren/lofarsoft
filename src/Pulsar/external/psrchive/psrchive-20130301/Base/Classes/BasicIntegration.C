/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BasicIntegration.h"

using namespace std;

/* not much to say really */
void Pulsar::BasicIntegration::init ()
{
  npol = nchan = nbin = 0;
  duration = pfold = 0.0;
}

Pulsar::BasicIntegration::BasicIntegration (const BasicIntegration& subint)
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::BasicIntegration copy ctor" << endl;
  
  init ();
  Integration::copy (&subint);
}

Pulsar::BasicIntegration::BasicIntegration (const Integration* subint)
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::BasicIntegration copy Integration*" << endl;
  
  init ();
  Integration::copy (subint);
}

//! Return the pointer to a new copy of self
Pulsar::Integration* Pulsar::BasicIntegration::clone () const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::BasicIntegration::clone entered" << endl;
  
  return new BasicIntegration (*this);
}

