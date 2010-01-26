/***************************************************************************
 *
 *   Copyright (C) 2007 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BPPArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include <string.h>

#include "machine_endian.h"
#include "ierf.h"

#include "bpp_latent.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Telescopes.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/GBT.h"
#include "Pulsar/Arecibo.h"
#include "Pulsar/Effelsberg.h"

#include "Pulsar/ThresholdMatch.h"

using namespace std;

void Pulsar::BPPArchive::init ()
{
  // I think only BPP-specifc init needs to 
  // go in here.
  orig_rfs = NULL;
  orig_rfs_corrected = 0;
  lin_method = Mean;
  ThresholdMatch::set_BPP (this);
}

Pulsar::BPPArchive::BPPArchive()
{
  init ();
}

Pulsar::BPPArchive::~BPPArchive()
{
  // destroy any BPPArchive resources
  // Again, I think this is only for BPP-specific stuff.
  if (orig_rfs!=NULL) delete [] orig_rfs;
}

Pulsar::BPPArchive::BPPArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::BPPArchive::BPPArchive (const BPPArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive construct copy BPPArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::BPPArchive::copy (const Archive& archive)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy dynamic cast call" << endl;
  
  const BPPArchive* like_me = dynamic_cast<const BPPArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy another BPPArchive" << endl;

  // copy BPPArchive-specific attributes
}

Pulsar::BPPArchive* Pulsar::BPPArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::clone" << endl;
  return new BPPArchive (*this);
}

/* Actual data reading code below this point.
 * Structure of a raw BPP file is as follows:
 *
 * Overall file:
 *   1 x struct bpp_header
 *   hdr.bds x (rf_array mean_array data_array)
 *
 * rf_array:
 *   hdr.chsbd x double - RF(Hz) for each chan.
 * mean_array:
 *   hdr.chsbd*hdr.poln x float - Data offset for each chan,pol.
 * data_array:
 *   hdr.chsdb*hdr.poln*hdr.bins x float - Actual data for bin,chan,pol.
 *
 * Quirks:
 *   - The data in rf_array is not quite accurate, and needs to be corrected
 *     via the function fix_orig_rfs.
 *   - The endianess of the file agrees with PCs (little-endian).
 *   - Later files have 3 groups of 8 channels, where the middle group
 *     overlaps the frequency range of the outer 2.  In the end, we probably
 *     want to ignore this middle group (TODO)...
 */

void Pulsar::BPPArchive::fix_orig_rfs ()
{
  // Don't double-fix
  if (orig_rfs_corrected) return;
  
  // Error if rfs not loaded.  We'll assume that the
  // header is loaded if the RF array is present.
  if (orig_rfs==NULL) 
    throw Error (InvalidState, "Pulsar::BPPArchive::fix_orig_rfs", 
        "RFs not loaded yet");

  // Don't know what to do if nchan is not a multiple of 8
  int nchan_tmp = hdr.bds*hdr.chsbd;
  if (nchan_tmp % 8) 
    throw Error (InvalidState, "Pulsar::BPPArchive::fix_orig_rfs",
        "nchan not a multiple of 8 (nchan=%d)", nchan_tmp);

  // Fix RF values.  We'll only use info from the hdr struct.
  double rf0;
  int i, j;
  for (i=0; i<nchan_tmp; i+=8) {
    rf0 = 0.5*(orig_rfs[i+3] + orig_rfs[i+4]);
    for (j=0; j<8; j++) {
      orig_rfs[i+j] = rf0 + ((double)j-3.5)*hdr.bandwidth;
    }
  }

  // Update flag
  orig_rfs_corrected=1;
}

void Pulsar::BPPArchive::hdr_to_big_endian () 
{
#if (MACHINE_LITTE_ENDIAN==0) // Compile-time endian check
  if (verbose>2)
    cerr << "Pulsar::BPPArchive::hdr_to_big_endian" << endl;
  ChangeEndian(hdr.telescope);
  ChangeEndian(hdr.apparent_period);
  ChangeEndian(hdr.dispersion_measure);
  ChangeEndian(hdr.scan_number);
  ChangeEndian(hdr.crate_id);
  ChangeEndian(hdr.year);
  ChangeEndian(hdr.day);
  ChangeEndian(hdr.seconds);
  ChangeEndian(hdr.second_fraction);
  ChangeEndian(hdr.RF_of_chan_0);
  ChangeEndian(hdr.bandwidth);
  ChangeEndian(hdr.integration_time);
  ChangeEndian(hdr.bins);
  ChangeEndian(hdr.bds);
  ChangeEndian(hdr.chsbd);
  ChangeEndian(hdr.polns);
  ChangeEndian(hdr.IF0_gain);
  ChangeEndian(hdr.IF1_gain);
  ChangeEndian(hdr.IF2_gain);
  ChangeEndian(hdr.IF3_gain);
  ChangeEndian(hdr.RF_of_IFcenter_0);
  ChangeEndian(hdr.RF_of_IFcenter_1);
  ChangeEndian(hdr.RF_of_IFcenter_2);
  ChangeEndian(hdr.RF_of_IFcenter_3);
  ChangeEndian(hdr.SRAM_base_freq);
  ChangeEndian(hdr.SRAM_freq_incr);
  ChangeEndian(hdr.bytes);
#endif
}

void Pulsar::BPPArchive::load_header (const char* filename)
{
  // load all BasicArchive and BPPArchive attributes from filename
  // We need to set (at least?) nbin, nchan, npol, nsubint

  // Open file, read header struct
  FILE *f = fopen(filename,"r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_header",
        "fopen(%s)", filename);

  int rv = fread(&hdr, sizeof(bpp_header), 1, f);

  if (rv!=1) {
    fclose(f);
    throw Error (FailedCall, "Pulsar::BPPArchive::load_header", "fread");
  }

  // Check for expected version number
  if (strncmp(hdr.version,"1.0",3)==0) { rv=1; }
  else if (strncmp(hdr.version,"2.0",3)==0) { rv=1; }
  else { rv=0; }
  if (!rv) {
    fclose(f);
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "version not recognized");
  }

#if (MACHINE_LITTLE_ENDIAN==0) 
  // Byte-swap header if needed
  hdr_to_big_endian();
#endif

  // Convert raw header values to things PSRCHIVE wants
  set_npol(hdr.polns);
  if (npol==1) { set_state(Signal::Intensity); }
  else if (npol==2) { set_state(Signal::PPQQ); }
  else if (npol==4) { set_state(Signal::Coherence); }
  else {
    fclose(f);
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "invalid npol (%d)", npol);
  }

  set_nsubint(1); // Only 1 subint per bpp file ever
  set_nbin(hdr.bins);
  set_nchan(hdr.chsbd*hdr.bds); // chans/board * n_boards

  set_scale(Signal::FluxDensity);

  char site_tmp[2];
  site_tmp[0] = (char)hdr.telescope;
  site_tmp[1] = '\0';
  set_telescope(site_tmp);

  string src = hdr.pulsar_name;

  if (src[0]=='c' || src[0]=='C') {
    set_type(Signal::PolnCal);
    lin_method = Bins; // Use per-bin linearization for cal data
  } else if (src[0]=='d' || src[0]=='D') {
    if (src[src.length()-1]=='n' || src[src.length()-1]=='N') { 
      set_type(Signal::FluxCalOff); 
    } else { set_type(Signal::FluxCalOn); }
  } else {
    set_type(Signal::Pulsar);
  }

  // Strip leading d or c from cal scans
  if (get_type()!=Signal::Pulsar) 
    src.erase(0,1);

  set_source(src);

  // Read in RFs array now 
  if ((hdr.chsbd<0) || (hdr.chsbd>64)) {
    fclose(f);
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "invalid chsbd=%d", hdr.chsbd);
  }
  if ((hdr.bds<0) || (hdr.bds>64)) {
    fclose(f);
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "invalid bds=%d", hdr.bds);
  }
  int i;
  orig_rfs = new double[hdr.chsbd*hdr.bds];
  int means_array_size = sizeof(float)*hdr.chsbd*hdr.polns;
  int data_array_size = sizeof(float)*hdr.bins*hdr.chsbd*hdr.polns;
  for (i=0; i<hdr.bds; i++) {
    rv = fread(&orig_rfs[i*hdr.chsbd], sizeof(double), hdr.chsbd, f);
    if (rv!=hdr.chsbd) { 
      fclose(f);
      throw Error (FailedCall, "Pulsar::BPPArchive::load_header", 
          "fread(orig_rfs)");
    }
    rv = fseek(f, means_array_size + data_array_size, SEEK_CUR);
    if (rv) { 
      fclose(f);
      throw Error (FailedCall, "Pulsar::BPPArchive::load_header", 
          "fseek(orig_rfs)");
    }
  }
  fclose(f);
#if (MACHINE_LITTLE_ENDIAN==0)
  // Byte-swap RFs array if needed
  array_changeEndian(hdr.chsbd*hdr.bds, orig_rfs, sizeof(double));
#endif

  // Find total (non-overlapping) BW
  // BPP file convention: bw is always positive, rf<0 for reversed sideband.
  // PSRCHIVE freq convention is opposite (rf>0 always, bw<0 for reverse).
  // Units: BPP=Hz, PSRCHIVE=MHz
  double rf_min, rf_max, rf_avg=0.0;
  fix_orig_rfs();
  rf_min = orig_rfs[0];
  rf_max = orig_rfs[0];
  for (i=0; i<nchan; i++) { 
    rf_avg += orig_rfs[i];
    if (orig_rfs[i]<rf_min) rf_min = orig_rfs[i];
    if (orig_rfs[i]>rf_max) rf_max = orig_rfs[i];
  }
  rf_min = fabs(rf_min-hdr.bandwidth/2.0);
  rf_max = fabs(rf_max+hdr.bandwidth/2.0);
  set_bandwidth((rf_max-rf_min)/1e6);
  rf_avg /= (double)nchan;
  rf_avg = fabs(rf_avg);
  set_centre_frequency(rf_avg/1e6);

  // Alt version, this is probably a more accurate representation
  // of the total BW.  Depends on what one means by bandwidth in
  // the case where channels can either overlap or have small
  // gaps between them...
  set_bandwidth((double)nchan*fabs(hdr.bandwidth)/1e6);

  // Don't put in DM for cal data
  if (get_type()==Signal::Pulsar) { 
    set_dispersion_measure(hdr.dispersion_measure);
  } else {
    set_dispersion_measure(0.0);
  }

  set_dedispersed(false);
  set_faraday_corrected(false);
  set_poln_calibrated(false);

  // Fill extensions
  load_extensions();

}

int Pulsar::BPPArchive::get_mjd_from_hdr() 
{
  // Date is stored as year, day of year in the header.
  // Maybe we can add a constructor for this date type
  // to the MJD class at some point?
  const int mjd_1990_01_01 = 47892; // BPP data starts in 1994
  if (hdr.year<1990)
    throw Error (InvalidState, "Pulsar::BPPArchive::get_mjd_from_hdr",
        "year<1990 not supported (%d)", hdr.year);
  int sum = hdr.day-1; // Jan 1 = day 1
  for (int i=1990; i<hdr.year; i++) {
    sum += 365;
    if ((i%4)==0) { sum++; } // not y2.1k compliant ;)
  }
  return(mjd_1990_01_01 + sum);
}

// Computes "linearized" input power as a function of
// (non-linear) 2-bit quantized output power.  Also
// returns "gain" (derivative of non-linear power relation)
// at this point.  Returns 0 on success, or +/-1 if quant_power
// is out of range.  
//
// This process does not fix dedispersion "dips" around pulse
// but simply rescales things so the data is proportional 
// to flux.
int Pulsar::BPPArchive::linearize_power(float quant_power, 
    float *input_power, float *gain, float *xgain)
{
  // Bounds checking
  if (quant_power<=1.0) { return(-1); }
  if (quant_power>=8.5) { return(1); } // True upper limit would be 9.0

  // Formula is
  //   ip = 0.5*(ierf((9.0-qp)/8.0))^-2
  // Based on 2-bit quant with output levels -3,-1,+1,+3.
  // Computes input power in units of threshold_voltage^2.
  float res = ierf((9.0-quant_power)/8.0);
  res *= res;
  res = 0.5/res;
  if (input_power!=NULL) { *input_power = res; }

  // From above formula, compute d(ip)/d(qp).
  // deriv = (sqrt(2*pi)/8) * ip^(3/2) * exp(0.5/ip))
  float gres = sqrt(res);
  gres *= res; // ip^3/2
  gres *= exp(1.0/(2.0*res));
  gres *= sqrt(2.0*M_PI)/8.0;
  if (gain!=NULL) { *gain = gres; }

  // Cross-term gain factor for this pol.
  //   g_x = (ip)/E(ip)
  //   E(ip) = sqrt(2/pi) * sqrt(ip) * (1+2*exp(-0.5/ip))
  // Full cross-term gain is g_x*g_y
  float gxres = sqrt(2.0/M_PI * res);
  gxres *= 1.0 + 2.0*exp(-0.5/res);
  gxres = res/gxres;
  if (xgain!=NULL) { *xgain = gxres; }

  return(0);
}

Pulsar::Integration*
Pulsar::BPPArchive::load_Integration (const char* filename, unsigned subint)
{

  // load all BasicIntegration attributes and data from filename

  // Can we assume header has been loaded?  Check for presence
  // of RFs array.  If not there, should we call load_header here?
  if (orig_rfs==NULL) 
    throw Error (InvalidState, "Pulsar::BPPArchive::load_Integration", 
        "RFs not loaded yet");

  // There is only 1 subint per BPP file
  if (subint!=0) 
    throw Error (InvalidState, "Pulsar::BPPArchive::load_Integration",
        "Requested subint!=0 (%d)", subint);

  // Size of various data sections per board
  const size_t size_hdr = sizeof(struct bpp_header);
  int num_rfs = hdr.chsbd;
  int num_means = num_rfs * hdr.polns;
  //int num_data = num_means * hdr.bins;
  size_t size_rfs = num_rfs * sizeof(double);
  //size_t size_means = num_means * sizeof(float);
  //size_t size_data = num_data * sizeof(float);

  Pulsar::BasicIntegration* integration = new BasicIntegration;
  resize_Integration(integration); // Sizes integration to npol,nchan,nbin

  // Date/time stuff 
  // NOTES: the MJD/time stored in the BPP header refers to the START of
  // the scan, at which point the pulsar phase is 0.  To avoid systematic
  // timing problems, we need to know a reference epoch near the middle
  // of the subint where phase==0.  Unfortunately this can't be accurately
  // determined from the data in a single BPP file, so the correction
  // will have to be handled at a higher level somehow.  This is the
  // infamous "midscan correction" problem.  The hdr.apparent_period
  // parameter refers to the psr period at midscan.
  MJD epoch(get_mjd_from_hdr(), hdr.seconds, hdr.second_fraction);
  integration->set_epoch(epoch);
  integration->set_duration(hdr.integration_time);
  integration->set_folding_period(hdr.apparent_period);

  // Set center freqs
  for (int i=0; i<hdr.bds*hdr.chsbd; i++) {
      integration->set_centre_frequency(i, fabs(orig_rfs[i])/1e6);
  }

  // Set uniform weights
  integration->uniform_weight(1.0);

  // If no_amps is set, we don't need to read the actual data
  if (Profile::no_amps) return integration;

  // Open up file
  FILE *f = fopen(filename, "r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration",
        "fopen(%s)", filename);

  // Loop over boards, channels
  Pulsar::Profile *prof;
  int cur_chan;
  float *data = new float[hdr.bins];
  float *means = new float[num_means];
  float *gains = new float[num_means];
  int rv = fseek(f, size_hdr, SEEK_SET); // Already read this
  if (rv) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", "fseek");
  for (int i=0; i<hdr.bds; i++) {
    rv = fseek(f, size_rfs, SEEK_CUR); // Already read these
    if (rv) 
      throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", "fseek");
    rv = fread(means, sizeof(float), num_means, f);
    if (rv!=num_means)
      throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", 
          "fread(means) rv=%d", rv);
#if (MACHINE_LITTLE_ENDIAN==0)
    // Byte-swap means array if needed
    array_changeEndian(num_means, means, sizeof(float));
#endif

    // Gains default to 1.0
    for (int k=0; k<num_means; k++) { gains[k]=1.0; }

    // Correct 2-bit mean powers, get 2-bit "gain" factors
    // Blank any channels with funky values.
    if (lin_method==Mean) {

      int nblank=0;
      float meantmp, gxtmp[2];

      for (int k=0; k<hdr.chsbd; k++) {
        cur_chan = i*hdr.chsbd + k;
        for (int j=0; j<hdr.polns; j++) {
          if (j<2) {
            // Power terms
            gxtmp[j]=1.0;
            rv = linearize_power(means[j*hdr.chsbd+k], &meantmp,
                &gains[j*hdr.chsbd+k], &gxtmp[j]);
            if (rv!=0) { integration->set_weight(cur_chan,0.0); nblank++; } 
            else { means[j*hdr.chsbd+k] = meantmp; }
          } else if (j<4) { 
            // Cross terms
            gains[j*hdr.chsbd+k] = gxtmp[0]*gxtmp[1];
            means[j*hdr.chsbd+k] *= gains[j*hdr.chsbd+k];
          } else {
            // Shouldn't get here!
            throw Error (InvalidState, "Pulsar::BPPArchive::load_Integration",
                "ipol>4 (%d)", j);
          }
        }
      }

      if (nblank&&(verbose>2)) 
        cerr << "Pulsar::BPPArchive blanked " << nblank << " channels" << endl;

    }

    // Loop over profile data
    for (int j=0; j<hdr.polns; j++) {
      for (int k=0; k<hdr.chsbd; k++) {
        cur_chan = i*hdr.chsbd + k;
        rv = fread(data, sizeof(float), hdr.bins, f);
        if (rv!=hdr.bins)
          throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration",
              "fread(data) rv=%d", rv);
#if (MACHINE_LITTLE_ENDIAN==0)
        // Byte-swap data
        array_changeEndian(hdr.bins, data, sizeof(float));
#endif
        prof = integration->get_Profile(j,cur_chan);
        prof->set_amps(data);
        // Only need to do scaling if we already linearized
        if (lin_method==Mean) 
          prof->scale(gains[j*hdr.chsbd+k]);
        prof->offset(means[j*hdr.chsbd+k]);
      }
    }
  }
  fclose(f);

  // Per-bin linearization, if requested
  if (lin_method==Bins) {

    int nblank=0;

    for (unsigned ichan=0; ichan<get_nchan(); ichan++) {

      bool blanked=false;
      float tmp,gxtmp[2];

      for (unsigned ipol=0; ipol<get_npol(); ipol++) {

        prof = integration->get_Profile(ipol,ichan);

        for (unsigned ibin=0; ibin<get_nbin(); ibin++) {

          if (ipol<2) {
            // AA, BB polns
            gxtmp[ipol]=1.0;
            rv = linearize_power(prof->get_amps()[ibin],&tmp,
                NULL,&gxtmp[ipol]); 
            if (rv!=0) { 
              // Linearize failed, blank this channel
              integration->set_weight(ichan,0.0); 
              nblank++; 
              blanked=true;
              break;
            } 
            prof->get_amps()[ibin] = tmp;
          } else {
            // Cross-terms
            prof->get_amps()[ibin] *= gxtmp[0]*gxtmp[1];
          }

        }

        if (blanked) break;

      }

      // Don't put half-corrected data through, just zero it
      if (blanked) {
        for (unsigned ipol=0; ipol<get_npol(); ipol++) {
          prof = integration->get_Profile(ipol,ichan);
          prof->scale(0.0);
        }
      }

    }

    if (nblank && (verbose>2)) 
      cerr << "Pulsar::BPPArchive blanked " << nblank << " channels" << endl;

  }

  // Unalloc temp space
  delete [] data;
  delete [] means;
  delete [] gains;

  return integration;
}

void Pulsar::BPPArchive::load_extensions() {

    // Telescope extension
    Telescope *t = getadd<Telescope>();
    Telescopes::set_telescope_info(t, this);

    // Backend extension
    Backend *b = getadd<Backend>();
    if ((get_telescope()=="1") || (get_telescope()=="a") 
        || (get_telescope()=="b")) {
      b->set_name("GBPP");
    } else if (get_telescope()=="3") {
      b->set_name("ABPP");
    } else if (get_telescope()=="f") {
      b->set_name("NBPP");
    } else if (get_telescope()=="g") {
      b->set_name("EBPP");
    } else {
      b->set_name("BPP");
    }
    // Fill in latency for this bandwidth mode
    // Applies closest match from the table.
    double fdiff1, fdiff0=fabs(hdr.bandwidth/1e6 - bpp_chbw[0]);
    int idx=0;
    for (int i=1; i<BPP_NLATENT; i++) { 
      fdiff1 = fabs(hdr.bandwidth/1e6 - bpp_chbw[i]); 
      if (fdiff1<fdiff0) { 
        fdiff0=fdiff1;
        idx=i;
      }
    }
    b->set_delay(bpp_latent[idx]);

    // Receiver extension
    // Note: Arecibo recvr selection will definitely need some
    // tweaking in order to even semi-accurately deal with old
    // data.  No recvr info (other than center freq) is in the file.

    Receiver *r = getadd<Receiver>();
    if (get_telescope()=="1") GBT::guess(r,this);
    else if (get_telescope()=="3") Arecibo::guess(r,this);
    else if (get_telescope()=="g") Effelsberg::guess(r,this);

    // Convert tempo code to telescope name
    set_telescope(t->get_name());
}

void Pulsar::BPPArchive::unload_file (const char* filename) const
{
  // unload is not implemented
  throw Error (InvalidState, "Pulsar::BPPArchive::unload_file",
          "not implemented");
}



string Pulsar::BPPArchive::Agent::get_description () 
{
  return "BPP folded data version 2.0";
}

bool Pulsar::BPPArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by BPP Archive, this method should return true
  // Implement by trying to load header
  try {
    BPPArchive archive;
    archive.load_header (filename);
    return true;
  }
  catch (Error &e) { 
    if (verbose > 2) 
        cerr << "BPP load failed due to: " << e << endl;
    return false;
  }
  return false;
}

