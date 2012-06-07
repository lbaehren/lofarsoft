/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "psrfitsio.h"

void Pulsar::FITSArchive::unload (fitsfile* f,
				  const WidebandCorrelator* ext)
try {

  psrfits_update_key (f, "BECONFIG", ext->configfile);
  psrfits_update_key (f, "NRCVR",    ext->nrcvr);
  psrfits_update_key (f, "TCYCLE",   ext->tcycle);

}
catch (Error& error) {
  throw error += "Pulsar::FITSArchive::unload WidebandCorrelator";
}
