/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"
#include "psrfitsio.h"

using namespace std;

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const AuxColdPlasmaMeasures* ext,
				  int row)
{
  if (verbose == 3)
    cerr << "FITSArchive::unload_AuxColdPlasmaMeasures" << endl;

  psrfits_write_col (fptr, "AUX_DM", row, ext->get_dispersion_measure());
  psrfits_write_col (fptr, "AUX_RM", row, ext->get_rotation_measure());
}
