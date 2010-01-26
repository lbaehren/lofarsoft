/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Profile.h"
#include "Pulsar/FourthMoments.h"
#include "Pulsar/ProfileColumn.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/CalInfoExtension.h"

#include "Pulsar/Predictor.h"

#include "setup_profiles.h"
#include "psrfitsio.h"

using namespace std;

// //////////////////////////////////////////////////////////////////

//! A function to read a single integration from a FITS file on disk.
/*! This function assumes that the Integration will have the global
  attributes of the file. */
Pulsar::Integration* 
Pulsar::FITSArchive::load_Integration (const char* filename, unsigned isubint)
try {

  if (!filename)
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "filename unspecified");

  if (search_mode)
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "SEARCH mode data -- no Integrations to load");

  Reference::To<Pulsar::Integration> integ = new_Integration();
  init_Integration (integ);

  int row = isubint + 1;
  
  int status = 0;  

  if (verbose > 2)
    cerr << "FITSArchive::load_Integration number " << isubint << endl;
  
  double nulldouble = 0.0;
  float nullfloat = 0.0;
  
  int initflag = 0;
  int colnum = 0;
  
  // Open the file

  fitsfile* fptr = 0;
  
  if (verbose > 2)
    cerr << "FITSArchive::load_Integration"
      " fits_open_file (" << filename << ")" << endl;
  
  fits_open_file (&fptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_open_file(%s)", filename);
  

  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu SUBINT");


  // Load the convention for the epoch definition
  string epoch_def;
  string default_def = "STT_MJD";
  psrfits_read_key (fptr, "EPOCHS", &epoch_def,
		    default_def, verbose > 2);

  if (verbose > 2)
    cerr << "FITSArchive::load_Integration epochs are " << epoch_def << endl;

  // By default, correct epochs using the phase model
  bool correct_epoch_phase = true;

  // By default, correct epochs such that phase(epoch)=phase(start)
  bool phase_match_start_time = true;

  if (epoch_def == "VALID")
    correct_epoch_phase = phase_match_start_time = false;

  if (epoch_def == "MIDTIME")
    phase_match_start_time = false;

  if (get<Pulsar::IntegrationOrder>())
  {
    colnum = 0;
    fits_get_colnum (fptr, CASEINSEN, "INDEXVAL", &colnum, &status);
    
    double value = 0.0;

    fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		   &value, &initflag, &status);
    
    if (status != 0)
      throw FITSError (status, "FITSArchive::load_Integration", 
		       "fits_read_col INDEXVAL");
    
    get<Pulsar::IntegrationOrder>()->set_Index(row-1,value);
  }
  
  // Get the reference epoch from the primary header
  const Pulsar::FITSHdrExtension* hdr_ext = get<Pulsar::FITSHdrExtension>();
  
  if (!hdr_ext)
  {
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "No FITSHdrExtension found");
  }
  
  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  double duration = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &duration, &initflag, &status);
  
  integ->set_duration (duration);

  // Set the start time of the integration
  
  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  double time = 0.0;
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &time, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_read_col OFFS_SUB");


  MJD epoch = hdr_ext->get_start_time() + time;

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration"
      " header epoch=" << hdr_ext->get_start_time().printdays(13) << "\n "
      " offset=" << time << "s epoch=" << epoch.printdays(13) << endl;
  
  // Set a preliminary epoch to avoid problems loading the polyco
  integ->set_epoch (epoch);

  // Set the folding period to 0 until one of three possible methods succeeds
  integ->set_folding_period (0.0);

  /* **********************************************************************

     METHOD 1: folding period defined by a pulse phase model

     ********************************************************************** */

  if (hdr_model)
  {
    // Set the folding period, using the polyco from the file header
    // This was taken out of the condition clause below because period
    // wasn't set when TSUB was 0
    integ->set_folding_period (1.0 / hdr_model->frequency(epoch));

    if (integ->get_folding_period() <= 0.0)
      throw Error( InvalidState, "Pulsar::FITSArchive::load_Integration",
		   "header polyco/predictor corrupted; "
		   "period(epoch=%s)=%lf", epoch.printdays(5).c_str(),
		   integ->get_folding_period() );

    if (integ->get_folding_period() < 1.0e-3)
      warning << "Pulsar::FITSArchive::load_Integration folding_period=" 
	      << integ->get_folding_period() << " is less than 1ms" << endl;

    else if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_Integration folding_period = "
      	   << integ->get_folding_period () << endl;

    if (duration && correct_epoch_phase)
    {

      if (verbose > 2)
	cerr << "Pulsar::FITSArchive::load_Integration correcting epoch phase"
	     << endl;

      Phase reference_phs = 0.0;

      if (phase_match_start_time)
      {
	// Correct epoch such that its phase equals that of the start time
	if (verbose > 2)
	  cerr << "Pulsar::FITSArchive::load_Integration matching phase(start)"
	       << endl;

	reference_phs = hdr_model->phase(hdr_ext->get_start_time());
      }

      Phase off_phs = hdr_model->phase(epoch);
      Phase dphase  = off_phs - reference_phs;
      
      double dtime = dphase.fracturns() * integ->get_folding_period();
      epoch -= dtime;
      integ->set_epoch (epoch);

      if (verbose > 2)
      {
      	cerr << "Pulsar::FITSArchive::load_Integration row=" << row <<
	  "\n  PRED_PHS=" << predicted_phase;

	if (phase_match_start_time)
	  cerr << "\n  reference epoch=" 
	       << hdr_ext->get_start_time().printdays(13);

	cerr <<
	  "\n  reference phase=" << reference_phs <<
	  "\n      input phase=" << off_phs <<
	  "\n     phase offset=" << dphase << " = " << dtime << "s" 
	  "\n     subint epoch=" << epoch.printdays(13) << 
	  "\n     subint phase=" << hdr_model->phase(epoch) << endl;
      }
    }
  }
  else
  {

    /* *******************************************************************

       METHOD 2: folding period defined by CAL_FREQ in primary header

       ******************************************************************* */

    CalInfoExtension* calinfo = get<CalInfoExtension>();
    if (calinfo && calinfo->cal_frequency > 0.0)
    {
      if (verbose > 2)
        cerr << "FITSArchive::load_Integration CAL_FREQ=" 
	     << calinfo->cal_frequency << endl;
      integ->set_folding_period( 1.0/calinfo->cal_frequency );
    }

    /* *******************************************************************

       METHOD 3: folding period defined by PERIOD column of SUBINT HDU

       ******************************************************************* */

    double period = 0.0;
    status = 0;
    fits_get_colnum (fptr, CASEINSEN, "PERIOD", &colnum, &status);
    fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
                   &period, &initflag, &status);

    if (status == 0 && period > 0.0)
    {
      if (verbose > 2)
	cerr << "FITSArchive::load_Integration PERIOD=" << period << endl;
      integ->set_folding_period (period);
    }

    if (integ->get_folding_period() == 0.0)
      throw FITSError (status, "FITSArchive::load_Integration",
                       "folding period unknown: no model, CAL_FREQ or PERIOD");
  }

  status = 0;

  // Load other useful info

  load_Pointing (fptr,row,integ);

  // Set up the data vector, only Pulsar::Archive base class is friend

  resize_Integration (integ);

  const unsigned nchan = get_nchan();

  if (naux_profile)
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      FourthMoments* fourth = new FourthMoments;
      fourth->resize (naux_profile, get_nbin());
      integ->get_Profile(0,ichan)->add_extension (fourth);
    }

  // Load the channel centre frequencies
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration reading channel freqs" 
	 << endl;
  
  int counter = 1;
  vector < float >  chan_freqs(get_nchan());
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, row, counter, get_nchan(),
		 &nullfloat, &(chan_freqs[0]), &initflag, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration",
                     "fits_read_col DAT_FREQ");

  // Set the profile channel centre frequencies
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration setting frequencies" 
	 << endl;

  bool all_ones = true;
  for (unsigned j = 0; j < get_nchan(); j++)
    if (chan_freqs[j] != 1)
      all_ones = false;
  
  double chanbw = get_bandwidth() / get_nchan();
  
  if ( all_ones )
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_Integration all frequencies unity - reseting"
	   << endl;
    for (unsigned j = 0; j < get_nchan(); j++)
      integ->set_centre_frequency (j, get_centre_frequency()
				   -0.5*(get_bandwidth()+chanbw)+j*chanbw);
  }
  else
  {
    for (unsigned j = 0; j < get_nchan(); j++)
      integ->set_centre_frequency(j, chan_freqs[j]);
  }
  
  // Load the profile weights

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration reading weights" 
	 << endl;
  
  counter = 1;
  vector < float >  weights(get_nchan());
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  for (unsigned b = 0; b < get_nchan(); b++) {
    fits_read_col (fptr, TFLOAT, colnum, row, counter, 1, &nullfloat, 
		   &weights[b], &initflag, &status);
    counter ++;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration",
                     "fits_read_col DAT_WTS");

  // Set the profile weights
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration setting weights" 
	 << endl;
  
  for(unsigned j = 0; j < get_nchan(); j++)
    integ->set_weight(j, weights[j]);
  
  // Load the profile offsets
  
  if (!Profile::no_amps)
  {
    vector<Profile*> profiles;

    setup_profiles_dat (integ, profiles);
    setup_dat_io (fptr);
    dat_io->load (isubint + 1, profiles);

    if (scale_cross_products && integ->get_state() == Signal::Coherence)
      for (unsigned ichan=0; ichan < get_nchan(); ichan++)
      {
	integ->get_Profile(2, ichan)->scale(2.0);
	integ->get_Profile(3, ichan)->scale(2.0);
      }

    if (naux_profile)
    {
      setup_profiles<MoreProfiles> (integ, profiles);
      setup_aux_io (fptr, naux_profile);
      aux_io->load (isubint + 1, profiles);
    }
  }

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration load complete" << endl;  
  
  // Finished with the file for now
  
  fits_close_file (fptr, &status);

  return integ.release();
}
catch (Error& error)
{
  throw error += "Pulsar::FITSArchive::load_Integration";
}


