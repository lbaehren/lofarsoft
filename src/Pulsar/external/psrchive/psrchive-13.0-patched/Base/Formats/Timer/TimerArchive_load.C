/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TimerArchive.h"
#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Telescope.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/Backend.h"
#include "Pulsar/CalInfoExtension.h"

#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"

#include "load_factory.h"
#include "strutil.h"
#include "timer++.h"
#include "mini++.h"
#include "Horizon.h"

#include <string.h>

using namespace std;

/*****************************************************************************/
void Pulsar::TimerArchive::load_header (const char* filename)
{
  if (verbose > 2)
    cerr << "TimerArchive::load (" << filename << ")" << endl;

  FILE* fptr = fopen (filename,"r");

  if (fptr == NULL)
    throw Error (FailedSys, "TimerArchive::load", "fopen");

  if (verbose > 2) 
    cerr << "TimerArchive::load Opened '" << filename << endl;
  
  try {
    load (fptr);
  }
  catch (Error& error) {
    fclose (fptr);
    throw error += "TimerArchive::load(" + string(filename) + ")";
  }
  fclose (fptr);

  if (verbose > 2)
    cerr << "TimerArchive::load (" << filename << ") exit" << endl;
}


void Pulsar::TimerArchive::load (FILE* fptr)
{
  if (verbose > 2)
    cerr << "TimerArchive::load FILE*" << endl;

  hdr_load (fptr);

  backend_load (fptr);

  unpack_extensions ();

  ephemeris = 0;
  model = 0;

  if (get_type() == Signal::Pulsar)
    psr_load (fptr);

  subint_load (fptr);

  if (verbose > 2 && get_nsubint())
    cerr << "TimerArchive::load epoch[0]=" 
         << get_Integration(0)->get_epoch().printdays(15) << endl;

  if (hdr.obstype != PULSAR)
    unpack( getadd<CalInfoExtension>() );

  valid = true;

  if (verbose > 2)
    cerr << "TimerArchive::load FILE* exit" << endl;
}


void Pulsar::TimerArchive::subint_load (FILE* fptr)
{
  if (verbose > 2) 
    cerr << "TimerArchive::subint_load"
      " nsubint=" << hdr.nsub_int <<
      " nchan="   << hdr.nsub_band <<
      " npol="    << hdr.banda.npol <<
      " nbin="    << hdr.nbin << endl;
 
  resize (hdr.nsub_int, hdr.banda.npol, hdr.nsub_band, hdr.nbin);

  Telescope* telescope = get<Telescope>();
  if (!telescope) cerr << "Pulsar::TimerArchive::subint_load"
		    " WARNING no Telescope extension" << endl;

  string machine = hdr.machine_id;
  float version  = hdr.version + hdr.minorversion;
  bool  circular = hdr.banda.polar == 0;
  bool  baseband = machine == "CPSR" || machine == "S2" || machine == "CPSR2";

  bool reverse_U = false;
  bool reverse_V = false;

  if( string(hdr.software).find("psrdisp",0) != string::npos ) {
    // feed_offset is set to -1 by default in older versions of psrdisp
    if (baseband && version < 15.1)
      hdr.banda.feed_offset = 0.0;

    if (baseband && version < 15.1 && hdr.banda.npol==4 && circular) {
      
      if (verbose > 2)
	cerr << "TimerArchive::subint_load correct psrdisp circular" << endl;
      
      // an unjustified sign reversal of Stokes U in psrdisp was 
      // removed in version 14.2.  It turns out that it was necessary.
      if (version >= 14.2)
	reverse_U = true;
      
      // and that Stokes V need also be inverted
      reverse_V = true;
      
      // reversing the sign of Stokes U and V (for circular feeds)
      // is equivalent to swapping Left and Right inputs.  This has
      // been done in version 15.1
      hdr.version = 15.0;
      hdr.minorversion = 0.1;
    }

    // Before psrdisp v.17.1, all archives contained Stokes IQUV
    // With v.17.1 and onward, they may or may not.
    if (baseband && version<17.1 && hdr.banda.npol==4)  {
      set_state (Signal::Stokes);
      hdr.version = 17.0;
      hdr.minorversion = 0.1;
    }
  }

  Backend* backend = get<Backend>();
  if (!backend)
    throw Error (InvalidState, "TimerArchive::subint_load",
		 "no Backend extension");

  if ( baseband && hdr.banda.npol==4 && hdr.banda.bw < 0.0 &&
       !backend->get_downconversion_corrected() )  {
    
    if ( version < 18.1 )  {

      // Before psrdisp v.18.1, lower sideband data did not have their sign
      // of Stokes V (linear) or Stokes U (circular) properly flipped.
 
      if (verbose > 2)
	cerr << "TimerArchive::subint_load"
	  " correcting lower-sideband downconversion" << endl;
      
      hdr.version = 18.0;
      hdr.minorversion = 0.1;
      reverse_V = true;

    }    

    if (verbose > 2)
      cerr << "TimerArchive::subint_load"
	" setting downconversion corrected" << endl;

    backend->set_downconversion_corrected (true);

  }

  bool nint_corrected = false;
  bool s2ros_corrected = false;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
  { 
    if (verbose > 2)
      cerr << "TimerArchive::subint_load " 
	   << isub+1 << "/" << get_nsubint() << endl;
    
    // by over-riding Archive::new_Integration, the subints array points to
    // instances of TimerIntegrations

    TimerIntegration* subint;
    subint = dynamic_cast<TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "TimerArchive::subint_load",
		   "Integration[%d] is not a TimerIntegration", isub);

    subint -> load (fptr, hdr.wts_and_bpass, big_endian);

    if (verbose > 2)
      cerr << "TimerArchive::subint_load " 
	   << isub+1 << "/" << get_nsubint() << " loaded" << endl;

        
    if (subint->mini.version < 1.1) {
      // update the mini header with the integration time of the sub_int
      // added by WvS 24 Sep, 1999
      if (verbose > 2)
        cerr << "TimerArchive::subint_load updating mini version" << endl;
      subint->mini.integration = hdr.sub_int_time;
      subint->mini.version = 1.1;
    }

    /* CPSR psrdisp MJD error fix.  Correct misinterpretation of
     * Revision 1.4  1999/08/11 10:28:35 of pspm_search_header.h
     * in psrdisp Version 10.0  -  fix added 2000 Mar 19 */
    if (hdr.mjd < 51401 && strcmp(hdr.machine_id,"CPSR")==0 
        && version == 10.0)  {
 
      if (verbose > 2) 
	cerr << "TimerArchive::subint_load Correcting psrdisp v10.0 MJD error"
	     << endl;
      
      double seconds_per_file = 53.6870912;

      if (isub == 0)
	Timer::set_MJD (hdr, Timer::get_MJD (hdr) - seconds_per_file);

      MJD minimjd = Mini::get_MJD (subint->mini) - seconds_per_file;
      Mini::set_MJD (subint->mini, minimjd);
      hdr.minorversion = 1.0;  // problem fixed in psrdisp v.10.1
    }

    /* nint() correction  jss */
    /***** test mjd and correct if less than NOV 94     *******/
    /***** and not already corrected && Correlator data *******/

    string machine = hdr.machine_id;

    if (hdr.mjd < 49650 && (machine == "A=Jo" || machine == "FPTM"))
    {
      if (hdr.corrected == 1)
        hdr.corrected = NINT_CORRECTED;

      if (hdr.corrected & NINT_CORRECTED == 0)
      {
        /* nint() correction  jss */
        double difftime,tcorr;
        double freqerror = 32.0e6/4.0/(pow(2.0,32.0))/2.0/1024.0;
        /* end nint() correction  jss */
        if (verbose > 2) 
	  cerr << "TimerArchive::subint_load Correcting nint error" << endl;
        difftime = subint->mini.mjd + subint->mini.fracmjd 
	  - hdr.mjd - hdr.fracmjd; 
        tcorr = difftime*freqerror*subint->mini.pfold;
        subint->mini.fracmjd += tcorr;
        if (subint->mini.fracmjd > 1.0) 
	{
          subint->mini.mjd++;
          subint->mini.fracmjd -= 1.0;
        }

        nint_corrected = true;
      }
    }

    // fix ROS 1 second error if necessary
    // This bug labels the times 1 second earlier than they should be.
    // The error occurs in archives prior to julian date 200 in 1997 (MJD 50647)
    // that were also processed before the end of 1997 (at which point
    // ROS was upgraded to a new version without the bug)

    if (subint->mini.mjd < 50647 && machine == "S2")
    {
      if (hdr.corrected == 1)
        hdr.corrected = S2ROS_CORRECTED;

      if (hdr.corrected & S2ROS_CORRECTED == 0)
      {
        /* get the year data was processed */
        char comment[64];
        char* key;
        const char* whitespace = " \n\t";
        int year;
        strcpy(comment, hdr.comment);
        key = strtok(comment, whitespace);
        for (int ntok = 0; ntok<6; ++ntok)
        {
	  if (key==NULL)
            break;
	  key = strtok(NULL, whitespace);
        }
        if (key && sscanf(key, "%d", &year)==1 && year<=1997)
        {
	  cerr << "TimerArchive::subint_load performing S2 MJD correction" 
	       << endl;
	  MJD old_mjd = Mini::get_MJD (subint->mini);
	  Mini::set_MJD (subint->mini, old_mjd + 1.0);

	  if (isub == 0)
          {
	    old_mjd = Timer::get_MJD (hdr);
	    Timer::set_MJD (hdr, old_mjd + 1.0);
	  }
          s2ros_corrected = true;
        }
      } // if S2 ROS bug was not already corrected
    } // if S2 ROS bug might need correction

    if (baseband && telescope) try
    {
      // Correct the direction and parallactic angles
      if (verbose > 2)
        cerr << "TimerArchive::subint_load correcting parallactic angle\n";

      Horizon horizon;

      sky_coord coord( hdr.ra, hdr.dec );
      horizon.set_source_coordinates( coord );
      horizon.set_epoch( subint->get_epoch() );
      horizon.set_observatory_latitude(telescope->get_latitude().getRadians());
      horizon.set_observatory_longitude(telescope->get_longitude().getRadians());
    
      // Correct the LST for baseband systems - LST in hours
      subint->mini.lst_start = horizon.get_local_sidereal_time()*12.0/M_PI;
 
      double rad2deg = 180.0/M_PI;
      subint->mini.tel_az = horizon.get_azimuth() * rad2deg;
      subint->mini.tel_zen = horizon.get_zenith() * rad2deg;
      subint->mini.para_angle = horizon.get_parallactic_angle() * rad2deg;
    }
    catch (Error& error)
    {
      cerr << error << endl;
    }

    if (reverse_U && !Profile::no_amps) {
      if (verbose > 2)
	cerr << "TimerArchive::subint_load reversing sign of ipol=2" 
	     << endl;
      for (int ichan=0; ichan<hdr.nsub_band; ichan++)
	*(subint->profiles[2][ichan]) *= -1.0;
    }
    
    if (reverse_V && !Profile::no_amps)
    {
      if (verbose > 2)
	cerr << "TimerArchive::subint_load reversing sign of ipol=3" 
	     << endl;
      for (int ichan=0; ichan<hdr.nsub_band; ichan++)
	*(subint->profiles[3][ichan]) *= -1.0;
    }

    // initialize book-keeping attributes
    init_Integration (subint, true);

  } // end for each sub_int

  // Weights initialized to one in subint constructor, will always be 
  // unloaded as such - MCB
  hdr.wts_and_bpass = 1;

  if (verbose > 2) 
    cerr << "TimerArchive::subint_load Read in " << get_nsubint()
         << " sub-Integrations" << endl;

  // profile.wt correction - MCB
  // profile weights from the S2 set to zero during software coherent
  // dedispersion.  Here we fill in the weights with the integration
  // time if they are all zero, but leave them alone if they are not.
  if (strcmp(hdr.machine_id, "S2")==0)
  {
    int weights = 0;

    for (unsigned i=0; i < get_nsubint(); i++)
      for (unsigned j=0; j < get_npol(); ++j)
	for (unsigned k=0; k < get_nchan(); ++k)
	  if (get_Profile(i,j,k)->get_weight() != 0) 
	    weights = 1;

    if (weights==0)
    {
      cerr << "TimerArchive::subint_load"
	"replacing profile weights by sub int time" << endl;
      
      for(unsigned i=0;i<get_nsubint();i++)
	for(unsigned j=0; j<get_npol(); ++j)
	  for(unsigned k=0; k<get_nchan(); ++k)
	    get_Profile(i,j,k)->set_weight(get_Integration(i)->get_duration());
    }
  }

  // Weight correction and calibration for FB archives
  // RNM Feb 17, 2000
  if(strcmp(hdr.machine_id, "FB")==0)
  {
    int weights = 0; float nch_sub, tdmp;  double chbw;
    tdmp = hdr.sub_int_time / hdr.ndump_sub_int;
    chbw = fabs(hdr.banda.bw)/hdr.banda.nlag;

    for(unsigned i=0; i < get_nsubint(); i++) {    
      nch_sub = (float)hdr.banda.nlag / (float)get_nchan();
      for(unsigned j=0; j<get_npol(); ++j){
	for(unsigned k=0; k<get_nchan(); ++k){
	  if(verbose > 2)printf("wt,nch_sub,tdmp: %f %f %f\n",
	       get_Profile(i,j,k)->get_weight(),nch_sub,tdmp);
	  if(get_Profile(i,j,k)->get_weight() > nch_sub*hdr.ndump_sub_int) weights = 1;
	}
      }
    }
    
    if(hdr.wts_and_bpass == 0 || weights == 0){
      if(verbose > 2) 
	cerr << "Multiply FB wts by dump time:" << tdmp << endl;
      for(unsigned i=0;i<get_nsubint();i++)
	for(unsigned j=0; j<get_npol(); ++j)
	  for(unsigned k=0; k<get_nchan(); ++k) {
	    Profile* profile = get_Profile(i,j,k);
	    profile->set_weight( profile->get_weight() * tdmp );
	  }
    }
    if(!(hdr.calibrated & FB_CALIBRATED)){
      if(verbose > 2) 
	cerr << "Setting FB calibration" << endl;
      if (!Profile::no_amps)
        for(unsigned i=0;i<get_nsubint();i++)
	  for(unsigned j=0; j<get_npol(); ++j)
	    for(unsigned k=0; k<get_nchan(); ++k){
	      // Scale to counts per sample and by sqrt of channel bandwidth 
	      // and sample time. Data were summed for original dump time and  
	      // this scaling is preserved by tscrunch and fscrunch.
	      *get_Profile(i,j,k) *= hdr.tsmp / tdmp * sqrt(chbw * hdr.tsmp);
	  }

      if(verbose > 2) 
	cerr << "ndmp:" <<hdr.ndump_sub_int << " tsmp:" << hdr.tsmp 
	     << " tsub_int:" << hdr.sub_int_time
	     << " chbw: " << chbw << endl;
      hdr.calibrated |= FB_CALIBRATED;
    }
  }
  
  // Set flag so weights always unloaded
  hdr.wts_and_bpass = 1;

  /* set a flag to indicate nint() correction has been done */
  if (nint_corrected)
    hdr.corrected |= NINT_CORRECTED;

  if (s2ros_corrected)
    hdr.corrected |= S2ROS_CORRECTED;

  /* Unpack the Pointing class attributes */

  for (int isub=0; isub < hdr.nsub_int; isub++)
  { 
    Pointing* pointing = get_Integration(isub)->get<Pointing>();
    if (!pointing)
      continue;

    Angle angle;

    angle.setRadians (hdr.ra);
    pointing->set_right_ascension (angle);

    angle.setRadians (hdr.dec);
    pointing->set_declination (angle);

    angle.setDegrees (hdr.l);
    pointing->set_galactic_latitude (angle);

    angle.setDegrees (hdr.b);
    pointing->set_galactic_longitude (angle);
  }

  if (verbose > 2) 
    cerr << "TimerArchive::subint_load - exiting" << endl;
}


void Pulsar::TimerArchive::hdr_load (FILE* fptr)
{
  if (verbose > 2)
    cerr << "TimerArchive::hdr_load reading timer header" << endl;

  if (Timer::load (fptr, &hdr, big_endian) < 0)
    throw Error (FailedCall, "TimerArchive::hdr_load", "Timer::load");

  hdr.banda.npol = Timer::get_npol (hdr);
}

/*!
  Unless this method is overloaded by the appropriate sub-class, all
  backend-specific information will be lost when the file is loaded.
*/
void Pulsar::TimerArchive::backend_load (FILE* fptr)
{
  unsigned long backend_offset = Timer::backend_data_size(hdr);

  if (verbose > 2)
    cerr << "TimerArchive::backend_load offset=" << backend_offset << endl;

  if (backend_offset == 0)
    return;

  // simply ignore the backend-specific information
  int ret = fseek (fptr, backend_offset, SEEK_CUR);
  if (ret != 0)
    throw Error (FailedSys, "TimerArchive::backend_load", "fseek");

  // all backend-specific information is lost
  strcpy (hdr.backend, "NONE");
  hdr.be_data_size = 0;
}

void Pulsar::TimerArchive::psr_load (FILE* fptr)
{
  if (hdr.nbytespoly > 0) {
    if (verbose > 2)
      cerr << "TimerArchive::psr_load "
	   << hdr.nbytespoly << " bytes in polyco" << endl;

    model = factory<Pulsar::Predictor> (fptr, hdr.nbytespoly);

    if (verbose > 2) {
      if (model) {
        cerr << "TimerArchive::psr_load read predictor:" << endl;
        model->unload (stderr);
        cerr << "TimerArchive::psr_load end of predictor" << endl;
      }
      else
        cerr << "TimerArchive::psr_load failed to read predictor" << endl;
    }
  }
  else if (verbose > 2)
    cerr << "TimerArchive::psr_load no predictor" << endl;

  if (hdr.nbytesephem > 0) {
    if (verbose > 2) 
      cerr << "TimerArchive::psr_load "
	   << hdr.nbytesephem << " bytes in ephemeris" << endl;

    ephemeris = factory<Pulsar::Parameters> (fptr, hdr.nbytesephem);

    if (verbose > 2)
    {
      if (ephemeris)
      {
	cerr << "TimerArchive::psr_load read parameters:" << endl;
	ephemeris->unload (stderr);
	cerr << "TimerArchive::psr_load end of parameters" << endl;
      }
      else
	cerr << "TimerArchive::psr_load failed to read parameters" << endl;
    }
  }
  else {
    if (verbose > 2)
      cerr << "TimerArchive::psr_load no ephemeris" << endl;
  }
}
