/***************************************************************************
 *
 *   Copyright (C) 2002-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/FaradayRotation.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

/*! 
  Calls FaradayRotation::transform
*/
void Pulsar::Integration::defaraday () try {

  Pauli::basis().set_basis( get_basis() );

  if (verbose)
    cerr << "Pulsar::Integration::defaraday RM=" << get_rotation_measure()
	 << " fixed=" << get_faraday_corrected() << endl;

  Reference::To<Pulsar::FaradayRotation> xform = new FaradayRotation;

  xform->transform (this);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::defaraday";
}


/*! This worker function corrects faraday rotation without asking many
  questions.

   \param ichan the first channel to be defaradayd
   \param kchan one greater than the last channel to be defaradayd
   \param rm the rotation measure
   \param f0 the reference frequency
*/
void Pulsar::Integration::defaraday (unsigned ichan, unsigned kchan,
                                     double rm, double f0)
try {

  Pauli::basis().set_basis( get_basis() );

  Reference::To<Pulsar::FaradayRotation> xform = new FaradayRotation;

  xform->set_rotation_measure( rm );
  xform->set_reference_frequency( f0 );

  // ensure that delta is equal to the identity matrix
  xform->set_delta( 1.0 );

  xform->range (this, ichan, kchan);

}
catch (Error& error)
{
  throw error += "Pulsar::Integration::defaraday [private]";
}

