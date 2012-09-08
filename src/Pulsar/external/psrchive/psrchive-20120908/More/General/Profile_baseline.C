/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/BaselineWindow.h"
#include "interface_stream.h"

/*!  
  The BaselineInterpreter class sets the baseline_strategy
  attribute according to commands specified either in the
  configuration file or via the psrsh interpreter.  It enables
  convenient experimentation with the baseline estimation algorithm.
*/
Pulsar::Option<Pulsar::Profile::Mask> Pulsar::Profile::baseline_strategy
(
 "Profile::baseline", new Pulsar::BaselineWindow,

 "Baseline estimation algorithm",

 "The name of the algorithm used to estimate the off-pulse baseline.\n"
 "Possible values: minimum, normal"
);


//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  /*
    In a multi-threaded application, each Profile instance must
    manage its own copy of this resource
  */

  Mask mask = baseline_strategy.get_value()->clone();
  return mask->operate (this);
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::baseline";
}
