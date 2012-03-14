/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ProfileColumn.h"
#include "Pulsar/MoreProfiles.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"

#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/CalInfoExtension.h"

#include "setup_profiles.h"
#include "FITSError.h"
#include "psrfitsio.h"
#include "templates.h"

#include <float.h>

using namespace std;

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
//! A function to write an integration to a row in a FITS file on disk.

void Pulsar::FITSArchive::unload_Integration (fitsfile* thefptr, int row, 
                                              const Integration* integ) const
{
  const IntegrationOrder* order = get<IntegrationOrder>();
  if (order)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_integration order=" 
           << order->get_extension_name() << endl;

    psrfits_write_col (thefptr, "INDEXVAL", row, order->get_Index(row-1));
  }

  // Set the duration of the integration
  psrfits_write_col (thefptr, "TSUBINT", row, integ->get_duration());
  
  // Set the start time of the integration

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration row=" << row
         << " epoch=" << integ->get_epoch () << endl;

  double time = (integ->get_epoch () - reference_epoch).in_seconds();

  psrfits_write_col (thefptr, "OFFS_SUB", row, time);

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration row=" << row 
         << " OFFS_SUB = " << time << " written" << endl;

  // Write other useful info
  
  const Pointing* theExt = integ->get<Pointing>();
  if (theExt)
    unload (thefptr,theExt,row);

  const CalInfoExtension* calinfo = get<CalInfoExtension>();
  bool calfreq_set = calinfo && calinfo->cal_frequency > 0.0;

  // Write folding period if predictor model does not exist
  if (!has_model() && !calfreq_set)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_integration"
              " neither predictor nor calibrator frequency set" << endl;

    psrfits_write_col (thefptr, "PERIOD", row, integ->get_folding_period());

    if (verbose > 2)
      cerr << "FITSArchive::unload_integration PERIOD column written" << endl;
  }

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration writing DAT_FREQ" << endl;

  vector<float> temp (nchan);

  for (unsigned j = 0; j < nchan; j++)
    temp[j] = integ->get_centre_frequency(j);

  psrfits_write_col (thefptr, "DAT_FREQ", row, temp, vector<unsigned> ());

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration writing DAT_WTS" << endl;

  for (unsigned j = 0; j < nchan; j++)
    temp[j] = integ->get_weight(j);

  psrfits_write_col (thefptr, "DAT_WTS", row, temp, vector<unsigned> ());

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration setup profiles" << endl;

  // Temporary profiles vector
  vector<const Profile*> profiles;
  setup_profiles_dat (integ, profiles);

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration profile vector size=" 
         << profiles.size () << " dat_io=" << unload_dat_io.ptr() << endl;

  unload_dat_io->unload (row, profiles);

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration DATA written" << endl;

  if (naux_profile)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_integration auxiliary nprof="
	   << naux_profile << endl;

    setup_profiles<const MoreProfiles> (integ, profiles);
    unload_aux_io->unload (row, profiles);
  }

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration finished" << endl;
}

//
// End of unload_integration function
// //////////////////////////////////
// //////////////////////////////////
