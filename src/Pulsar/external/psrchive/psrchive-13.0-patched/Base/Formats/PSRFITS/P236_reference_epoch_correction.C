/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/Predictor.h"

using namespace std;

/* This correction applies only to pulsar observations made during the
   commissioning of the WBC.  As far as I know, this affects only P236.

   Symptom: The pulses from different archives line up, despite the fact
   that the phases predicted by the polyco for the reference epochs do not.
   Consequently, if you try to line them up using the polyco, they do not.

   Remedy: Truncate the phase of the reference epoch to zero.
*/

void Pulsar::FITSArchive::P236_reference_epoch_correction ()
{
  FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();

  if (!hdr_ext)
    throw Error (InvalidParam, "FITSArchive::P236_reference_epoch_correction",
		 "No FITSHdrExtension found");

  if (!model || get_type() != Signal::Pulsar)
    return;

  MJD original_reference_epoch = hdr_ext->get_start_time();
  Phase original_phase = model->phase(hdr_ext->get_start_time());

  hdr_ext->set_start_time( model->iphase( original_phase.Floor() ) );

  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::P236_reference_epoch_correction"
      "\n   original reference epoch=" << original_reference_epoch <<
      "\n            phase=" << original_phase <<
      "\n  corrected reference epoch=" << hdr_ext->get_start_time() <<
      "\n            phase=" << model->phase(hdr_ext->get_start_time())
	 << endl;
}
