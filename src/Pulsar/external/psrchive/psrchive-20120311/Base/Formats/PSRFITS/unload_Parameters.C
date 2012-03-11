/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Parameters.h"
#include "unload_text.h"

using namespace std;

void Pulsar::FITSArchive::unload_Parameters (fitsfile* fptr) const
{
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::unload_Parameters" << endl;

  unload_text (fptr, "PSRPARAM", "PARAM", ephemeris.get(), verbose > 3);
}
