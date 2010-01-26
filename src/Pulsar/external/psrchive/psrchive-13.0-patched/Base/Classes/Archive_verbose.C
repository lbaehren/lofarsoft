/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"

#include "tempo++.h"
#include "Error.h"

void Pulsar::Archive::set_verbosity (unsigned level)
{
  // level 3
  Profile::verbose     = (level >= 3);
  Tempo::verbose       = (level >= 3);
  Parameters::verbose  = (level >= 3);
  Predictor::verbose   = (level >= 3);

  // level 3
  Integration::verbose = (level >= 3);

  // all levels
  Archive::verbose = level;
}
