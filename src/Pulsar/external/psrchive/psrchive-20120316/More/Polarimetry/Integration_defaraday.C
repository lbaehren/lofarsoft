/***************************************************************************
 *
 *   Copyright (C) 2002-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/FaradayRotation.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

/*! 
  Calls FaradayRotation::transform
*/
void Pulsar::Integration::defaraday () try
{
  Pauli::basis().set_basis( get_basis() );

  if (verbose)
    cerr << "Pulsar::Integration::defaraday RM=" << get_rotation_measure()
	 << " fixed=" << get_faraday_corrected() << endl;

  Reference::To<Pulsar::FaradayRotation> xform = new FaradayRotation;

  xform->transform (this);

  if (orphaned)
    orphaned->set_faraday_corrected( true );
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::defaraday";
}


/*!
   \param ichan the first channel to be defaradayd
   \param kchan one greater than the last channel to be defaradayd
   \param reference_frequency the reference frequency
*/
void Pulsar::Integration::defaraday (unsigned ichan, unsigned kchan,
                                     double reference_frequency)
try
{
  Pauli::basis().set_basis( get_basis() );

  Reference::To<Pulsar::FaradayRotation> xform = new FaradayRotation;

  xform->correct (this, ichan, kchan, reference_frequency);
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::defaraday [private]";
}
