/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ObsExtension.h"
#include "psrfitsio.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const ObsExtension* ext)
{
  psrfits_update_key (fptr, "OBSERVER", ext->observer);
  psrfits_update_key (fptr, "PROJID", ext->project_ID);
}
