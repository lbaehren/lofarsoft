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
void Pulsar::FITSArchive::load_Plasma (fitsfile* fptr, int row,
				       Pulsar::Integration* integ)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_AuxColdPlasmaMeasures" << endl;

  double zero = 0.0;

  double aux_dm = zero;
  psrfits_read_col (fptr, "AUX_DM", &aux_dm, row, 
		    zero, zero, Pulsar::Archive::verbose > 2);

  double aux_rm = zero;
  psrfits_read_col (fptr, "AUX_RM", &aux_rm, row, 
		    zero, zero, Pulsar::Archive::verbose > 2);

  if (aux_rm == 0 && aux_dm == 0)
    return;

  Reference::To<AuxColdPlasmaMeasures> ext = new AuxColdPlasmaMeasures;
  ext->set_dispersion_measure (aux_dm);
  ext->set_rotation_measure (aux_rm);

  integ->add_extension (ext);
}
