/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/BaselineInterpreter.h"

/*!  
  The BaselineInterpreter class sets the baseline_strategy
  attribute according to commands specified either in the
  configuration file or via the psrsh interpreter.  It enables
  convenient experimentation with the baseline estimation algorithm.
*/
Pulsar::Option<Pulsar::Profile::Mask> Pulsar::Profile::baseline_strategy
(
 new Pulsar::BaselineInterpreter (Pulsar::Profile::baseline_strategy),
 "Profile::baseline", "minimum",

 "Baseline estimation algorithm",

 "The name of the algorithm used to estimate the off-pulse baseline.\n"
 "Possible values: minimum, normal"
);


//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  return baseline_strategy.get_value() (this);
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::baseline";
}
