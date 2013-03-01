/***************************************************************************
 *
 *   Copyright (C) 2002-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/Dispersion.h"

using namespace std;

/*! 
  Calls Dispersion::transform
*/
void Pulsar::Integration::dedisperse () try
{
  if (verbose)
    cerr << "Pulsar::Integration::dedisperse" << endl;

  Dispersion xform;
  xform.transform (this);

  if (orphaned)
    orphaned->set_dedispersed( true );
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::dedisperse";
}

void Pulsar::Integration::dedisperse (unsigned ichan, unsigned kchan,
				      double reference_frequency)
try
{
  Dispersion xform;
  xform.correct (this, ichan, kchan, reference_frequency);
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::dedisperse [private]";
}
