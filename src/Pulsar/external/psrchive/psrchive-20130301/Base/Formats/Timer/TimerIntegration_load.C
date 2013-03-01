/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Pointing.h"
#include "Error.h"

#include "convert_endian.h"

#include <memory>
#include <assert.h>

using namespace std;

/* assumes that all profiles have equal nbins */
void unpackprofiles (vector<vector<Reference::To<Pulsar::Profile> > >& profs,
		     int npol, int nsub, int nbin,
		     unsigned short int packed[],
		     float scale, float offset)
{
    int ipol, isub, ibin, index;

    index = 0;
    for (ipol=0; ipol<npol; ipol++)
	for (isub=0; isub<nsub; isub++) {
	    float* amps = profs[ipol][isub]->get_amps();
	    for (ibin=0; ibin<nbin; ibin++) {
		amps[ibin] = offset + packed[index] * scale;
		index ++;
	    }
	}
}

//! load the subint from file
void Pulsar::TimerIntegration::load (FILE* fptr, int extra, bool big_endian)
{
  // read in the mini header
  if (fread (&mini,sizeof(mini),1,fptr) < 1)
    throw Error (FailedSys, "TimerIntegration::load", "fread mini");

  if (big_endian)
    mini_fromBigEndian(&mini);
  else
    mini_fromLittleEndian(&mini);

  if (verbose)
    cerr << "Pulsar::TimerIntegration::load epoch=" << get_epoch().printdays(15) << endl;

  unpack_Pointing ();

  if (extra)
    load_extra (fptr, big_endian);
  else
    load_old (fptr, big_endian);
}

void Pulsar::TimerIntegration::unpack_Pointing ()
{
  Angle angle;

  Reference::To<Pointing> pointing = new Pointing;

  pointing->set_local_sidereal_time (mini.lst_start);

  angle.setDegrees (mini.feed_ang);
  pointing->set_feed_angle (angle);

  angle.setDegrees (mini.para_angle);
  pointing->set_parallactic_angle (angle);

  angle.setDegrees (mini.tel_az);
  pointing->set_telescope_azimuth (angle);

  angle.setDegrees (mini.tel_zen);
  pointing->set_telescope_zenith (angle);

  add_extension (pointing);
}

/*! This method is called for archives that store weights for each channel,
  as well as the bandpass and median for each channel and polarization.
  NOTE: none of these three arrays are computed for filter bank archives.
  (meaning that extra variable should be 0 for FB archives). */
void Pulsar::TimerIntegration::load_extra (FILE* fptr, bool big_endian)
{
  if (verbose) cerr << "Pulsar::TimerIntegration::load_extra start offset=" 
                    << ftell(fptr) << endl;

  if (verbose)
    cerr << "Pulsar::TimerIntegration::load_extra npol=" << npol 
         << " nchan=" << nchan << endl;

    if ( fread (&(wts[0]),nchan*sizeof(float),1,fptr) != 1 )
	throw Error (FailedSys, "TimerIntegration::load", "fread wts");

    if (big_endian)
	N_FromBigEndian (nchan, &(wts[0]));
    else
	N_FromLittleEndian (nchan, &(wts[0]));

    unsigned ipol;

    for (ipol=0; ipol < npol; ipol++) {
	if (fread(&(med[ipol][0]),nchan*sizeof(float),1,fptr)!=1)
	    throw Error (FailedSys,"TimerIntegration::load", "fread medians");
	if (big_endian)
	    N_FromBigEndian (nchan, &(med[ipol][0]));
	else
	    N_FromLittleEndian (nchan, &(med[ipol][0]));
    }

    for (ipol=0; ipol < npol; ipol++) {
	if(fread(&(bpass[ipol][0]),nchan*sizeof(float),1,fptr)!=1)
	    throw Error (FailedSys,"TimerIntegration::load", "fread bpass");
	if (big_endian)
	    N_FromBigEndian (nchan, &(bpass[ipol][0]));
	else
	    N_FromLittleEndian (nchan, &(bpass[ipol][0]));
    }

  if (verbose) cerr << "Pulsar::TimerIntegration::load_extra end offset="
                    << ftell(fptr) << endl;

    if (verbose) cerr << "Pulsar::TimerIntegration::load loading profiles\n";
    // Read the array of profiles
    for (ipol=0; ipol < npol; ipol++)
	for (unsigned ichan=0; ichan < nchan; ichan++) try {
	    TimerProfile_load (fptr, profiles[ipol][ichan], big_endian);
	}
	catch (Error& error) {
	    error << "\n\tprofile[ipol=" << ipol << "][ichan=" << ichan << "]";
	    throw error += "Pulsar::TimerIntegration::load";
	}
} 

void Pulsar::TimerIntegration::load_old (FILE* fptr, bool big_endian)
{
  if (verbose)
    cerr << "TimerIntegration::load in the old style." << endl;

  unsigned ipol, ichan;

  // No weights available.
  for (ichan=0; ichan<nchan; ichan++)
  {
    wts[ichan] = 1.0;
    for(ipol=0;ipol<npol;ipol++)
    {
      med[ipol][ichan]   = 1.0;
      bpass[ipol][ichan] = 1.0;
    }
  }

  double centrefreq = get_centre_frequency();
  double bw = get_bandwidth();

  for(ipol=0; ipol<npol; ipol++)
  {
    for(ichan=0; ichan<nchan; ichan++)
    {
      profiles[ipol][ichan]->set_weight (1.0);
      double cfreq = centrefreq-(bw/2.0)+(ichan+0.5)*bw/nchan;
      profiles[ipol][ichan]->set_centre_frequency (cfreq);
    }
  }

  float scale = 0;
  // old style + Filter bank
  if (fread (&scale, sizeof(scale), 1,fptr) != 1)
    throw Error (FailedSys,"TimerIntegration::load", "fread scale");

  float offset = 0;
  if (fread (&offset,sizeof(offset),1,fptr) != 1)
    throw Error (FailedSys,"TimerIntegration::load", "fread offset");

  int npts = nbin*nchan*npol;

  vector<unsigned short> auto_delete (npts);
  unsigned short* packed = &(auto_delete[0]);

  size_t ptsrd = fread(packed, sizeof(unsigned short int), npts, fptr);

  if (int(ptsrd) < npts)
  {
    ErrorCode code = InvalidState;
    if (ferror(fptr))
      code = FailedSys;

    throw Error (code, "TimerIntegration::load",
                 " read %d/%d pts", ptsrd, npts);
  }

  if (big_endian)
  {
    FromBigEndian (scale);
    FromBigEndian (offset);
    N_FromBigEndian (npts, packed);
  }
  else
  {
    FromLittleEndian (scale);
    FromLittleEndian (offset);
    N_FromLittleEndian (npts, packed);
  }

  if (Profile::verbose) 
    fprintf (stderr, "TimerIntegration::load scale:%f offset:%f\n",
		 scale, offset);

  if (scale == 0.0)
    throw Error (InvalidState, "TimerIntegration::load",
		     "Corrupted scale factor");

  if (!Profile::no_amps)
    unpackprofiles (profiles, npol, nchan, nbin, packed, scale, offset);
}
