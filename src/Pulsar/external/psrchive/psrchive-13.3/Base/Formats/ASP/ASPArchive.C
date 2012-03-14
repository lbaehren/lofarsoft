/***************************************************************************
 *
 *   Copyright (C) 2007 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ASPArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"
#include "sky_coord.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Telescopes.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/GBT.h"
#include "Pulsar/Arecibo.h"
#include "Pulsar/Backend.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/Pointing.h"

#include <fitsio.h>
#include "FITSError.h"
#include "FTransform.h"

using namespace std;

void Pulsar::ASPArchive::init ()
{
  // initialize the ASPArchive attributes
  // ASP-specific things we may want:
  //   - filetype version
  //   - per-channel timestamps?
  asp_file_version=0;
}

Pulsar::ASPArchive::ASPArchive()
{
  init ();
}

Pulsar::ASPArchive::~ASPArchive()
{
  // destroy any ASPArchive-specific resources
}

Pulsar::ASPArchive::ASPArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASPArchive::ASPArchive (const ASPArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive construct copy ASPArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::ASPArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy dynamic cast call" << endl;
  
  const ASPArchive* like_me = dynamic_cast<const ASPArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy another ASPArchive" << endl;

  // copy ASPArchive attributes
  // XXX which direction are we copying?
}

Pulsar::ASPArchive* Pulsar::ASPArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::clone" << endl;
  return new ASPArchive (*this);
}

void Pulsar::ASPArchive::load_header (const char* filename)
{
  // load all BasicArchive and ASPArchive attributes from filename.
  // Includes most info found in primary ASP fits header.

  // Try to open fits file
  fitsfile *f;
  int status=0, zstatus=0; 
  int int_tmp=0;
  long int_tmp2=0;
  if (fits_open_file(&f, filename, READONLY, &status)) 
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "fits_open_file(%s)", filename);

  // Look for appropriate header version
  char ctmp[80];
  fits_movabs_hdu(f, 1, NULL, &status);
  fits_read_key(f, TSTRING, "HDRVER", ctmp, NULL, &status);
  if (status) {
    fits_close_file(f, &int_tmp);
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "Couldn't find HDRVER key");
  }

  // check version string here
  string version = ctmp;
  if (version.compare("Ver1.0")==0) { asp_file_version=ASP_FITS_V10; }
  else if (version.compare("Ver1.1")==0) { asp_file_version=ASP_FITS_V11; }
  else if (version.compare("Ver1.0.1")==0) { asp_file_version=ASP_FITS_V101; }
  else { asp_file_version=0; }

  // Unrecognized HDRVER keyword
  if (asp_file_version==0) {
    fits_close_file(f, &int_tmp);
    throw Error (InvalidState, "Pulsar::ASPArchive::load_header",
        "Unrecognized HDRVER=%s in file %s", version.c_str(), filename);
  }

  // Ver1.1 not supported now.  Not very much data in this version
  // exists.
  if (asp_file_version==ASP_FITS_V11) {
    fits_close_file(f, &int_tmp);
    throw Error (InvalidState, "Pulsar::ASPArchive::load_header",
        "ASP Ver1.1 not currently supported");
  }

  // Some tmp variables
  float flt_tmp, flt_tmp2;
  string stmp;

  // Determine number of subints
  // TODO: make this more robust?  In all versions, data is stored 
  // in ASPOUTn binary table extensions.  In V101, timestamp info is 
  // stored in DUMPREFn ascii table extensions.  Maybe we could store
  // a list of which of these exist here, and then refer to it in 
  // load_Integration.
  fits_get_num_hdus(f, &int_tmp, &status);
  int_tmp -= 3; // info HDUs
  if (asp_file_version==ASP_FITS_V101) { int_tmp /= 2; }
  if (!status) set_nsubint(int_tmp);

  // Check if last subint is empty
  if (asp_file_version==ASP_FITS_V101) { 
    fits_movabs_hdu(f, 4+(int_tmp-1)*2+1, NULL, &status);
  } else {
    fits_movabs_hdu(f, (int_tmp-1)+4, NULL, &status);
  }
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_header", 
        "Couldn't move to last subint (%d)", int_tmp);
  }
  fits_get_num_rows(f, &int_tmp2, &status);
  if (int_tmp2==0) set_nsubint(int_tmp-1);

  // Pol info always the same for ASP
  set_npol(4);
  set_state(Signal::Coherence);
  set_scale(Signal::FluxDensity);
  set_faraday_corrected(false);
  set_poln_calibrated(false);

  // TODO: This may change for multi-polyco support
  set_dedispersed(false);

  // Info from main header
  fits_movabs_hdu(f, 1, NULL, &status);
  
  // Number of bins per pulse period in folded profiles.
  fits_read_key(f, TINT, "NPTSPROF", &int_tmp, NULL, &status);
  if (!status) set_nbin(int_tmp);

  // Source name
  fits_read_key(f, TSTRING, "SRC_NAME", ctmp, NULL, &status);
  if (!status) set_source(ctmp);

  // Source coordinates
  double ra, dec;
  sky_coord s;
  fits_read_key(f, TDOUBLE, "RA", &ra, NULL, &status);
  fits_read_key(f, TDOUBLE, "DEC", &dec, NULL, &status);
  if (!status) {
    s.ra().setDegrees(ra*15.0);
    s.dec().setDegrees(dec);
    set_coordinates(s);
  }

  // TEMPO telescope site code
  fits_read_key(f, TSTRING, "OBSVTY", ctmp, NULL, &status);
  if (!status) set_telescope(ctmp);

  // Observation type (PSR or CAL), again see Types.h
  fits_read_key(f, TSTRING, "OBS_MODE", ctmp, NULL, &status);
  if (!status) {
    stmp = ctmp;
    if (stmp.compare("CAL")==0) {
      set_type(Signal::PolnCal);
      // TODO : check for flux cal srcs?
    } else {
      set_type(Signal::Pulsar);
    }
  }

  // Approx center frequency, MHz.  
  fits_read_key(f, TFLOAT, "FSKYCENT", &flt_tmp, NULL, &status);
  if (!status) set_centre_frequency(flt_tmp);

  // Move to BECONFIG table.
  fits_movnam_hdu(f, ASCII_TBL, "BECONFIG", 0, &status);

  // Number of channels in file.
  int col=0;
  fits_get_colnum(f, CASEINSEN, "NChan", &col, &status);
  fits_read_col(f, TINT, col, 1, 1, 1, NULL, &int_tmp, NULL, &status);
  if (!status) set_nchan(int_tmp);

  // get sign of BW, compute archive center freq
  int bw_sign=1;
  fits_get_colnum(f, CASEINSEN, "CFRQ0", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  if (get_nchan() > 1) {
    fits_get_colnum(f, CASEINSEN, "CFRQ1", &col, &status);
    fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp2, NULL, &status);
    if (flt_tmp2<flt_tmp) { bw_sign=-1; }
  }
  flt_tmp2=0.0;
  for (unsigned i=0; i<nchan; i++) {
    sprintf(ctmp, "CFRQ%d", i);
    fits_get_colnum(f, CASEINSEN, ctmp, &col, &status);
    fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
    flt_tmp2 += flt_tmp;
  }
  set_centre_frequency(flt_tmp2/(double)nchan);

  // Bandwidth
  fits_get_colnum(f, CASEINSEN, "BW0", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  if (!status) set_bandwidth((float)bw_sign * flt_tmp * nchan);

  // Move to COHDDISP table
  fits_movnam_hdu(f, ASCII_TBL, "COHDDISP", 0, &status);

  // Dispersion measure, pc/cm^3.
  fits_get_colnum(f, CASEINSEN, "DM", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  if (!status) {
    if (get_type()==Signal::Pulsar) 
      set_dispersion_measure(flt_tmp);
    else 
      set_dispersion_measure(0.0);
  }

  // Create extensions
  load_extensions(f, &status);

  // Done for now
  int_tmp=0;
  fits_close_file(f, &int_tmp);

  // Catch any FITS errors
  if (status) 
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "Error reading header values (file=%s)", filename);

}

Pulsar::Integration*
Pulsar::ASPArchive::load_Integration (const char* filename, unsigned subint)
{

  // Basic check to see if header has been loaded correctly, 
  // and we support this version.
  int version_ok=0;
  if (asp_file_version==ASP_FITS_V101) { version_ok=1; }
  else if (asp_file_version==ASP_FITS_V10) { version_ok=1; }
  else { version_ok=0; }
  if (!version_ok) 
    throw Error (InvalidState, "Pulsar::ASPArchive::load_Integration",
        "Invalid asp_file_version (%d) or header not loaded", asp_file_version);

  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by 
  // load_header.
  resize_Integration(integration);

  // Reopen file
  fitsfile *f;
  int status=0, zstatus=0; 
  if (fits_open_file(&f, filename, READONLY, &status)) 
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
        "fits_open_file(%s)", filename);

  // tmp vars
  int int_tmp;
  float flt_tmp;

  // Figure out integration time, mjd.
  int imjd=0;
  fits_movabs_hdu(f, 1, NULL, &status); 
  fits_read_key(f, TINT, "NDUMPS", &int_tmp, NULL, &status);
  fits_read_key(f, TFLOAT, "SCANLEN", &flt_tmp, NULL, &status);
  if (!status) integration->set_duration(flt_tmp/(float)int_tmp);
  fits_read_key(f, TINT, "STT_IMJD", &imjd, NULL, &status);
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Error reading integration time or MJD", subint);
  }

  // Read in RFs
  int col=0;
  char ctmp[16];
  fits_movnam_hdu(f, ASCII_TBL, "BECONFIG", 0, &status);
  for (unsigned i=0; i<nchan; i++) {
    sprintf(ctmp, "CFRQ%d", i);
    fits_get_colnum(f, CASEINSEN, ctmp, &col, &status);
    fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
    integration->set_centre_frequency(i, flt_tmp);
  }
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Error reading RF array", subint);
  }

  // Move to specified subint 
  if (asp_file_version==ASP_FITS_V101) { 
    fits_movabs_hdu(f, 4+subint*2, NULL, &status);
  } else {
    fits_movabs_hdu(f, subint+4, NULL, &status);
  }
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Couldn't find specified subint (%d)", subint);
  }

  // Load in ref phase/freq stamps and time for this subint
  double midsecs;
  double *midphase = new double[nchan];
  double *midper = new double[nchan];
  if (asp_file_version==ASP_FITS_V101) { 
    fits_read_key(f, TDOUBLE, "MIDSECS", &midsecs, NULL, &status);
    fits_get_colnum(f, CASEINSEN, "REFPHASE", &col, &status);
    fits_read_col(f, TDOUBLE, col, 1, 1, nchan, NULL, midphase, NULL, &status);
    fits_get_colnum(f, CASEINSEN, "REFPERIOD", &col, &status);
    fits_read_col(f, TDOUBLE, col, 1, 1, nchan, NULL, midper, NULL, &status);
  } else if (asp_file_version==ASP_FITS_V10) { 
    fits_read_key(f, TDOUBLE, "DUMPMIDSECS", &midsecs, NULL, &status);
    fits_read_key(f, TDOUBLE, "DUMPREFPER", &midper[0], NULL, &status);
    fits_read_key(f, TDOUBLE, "DUMPREFPHASE", &midphase[0], NULL, &status);
    midphase[0] = fmod(midphase[0],1.0);
    for (unsigned i=1; i<nchan; i++) {
      midphase[i] = midphase[0];
      midper[i] = midper[0];
    }
  }

  // Use avg folding period
  // TODO: This will change for multi-polyco support
  double pfold=0.0;
  for (unsigned i=0; i<nchan; i++) pfold += midper[i];
  pfold /= (double)nchan;
  if (!status) integration->set_folding_period(pfold);

  // Use a middle channel as ref phase, adjust epoch
  // TODO: This will change for multi-polyco support
  double refphase = midphase[nchan/2];
  midsecs -= refphase * midper[nchan/2];

  // Move to data HDU
  if (asp_file_version==ASP_FITS_V101) {
    fits_movrel_hdu(f, 1, NULL, &status);
  }

  // Read time offset
  double tdiff=0.0;
  if (!status) {
    fits_read_key(f, TDOUBLE, "DUMPTDIFF", &tdiff, NULL, &status);
    if (status==KEY_NO_EXIST) {
      tdiff=155.0;
      status=0;
      // warn?
    }
  }
  midsecs += tdiff*1e-9;

  // Set timestamp
  MJD epoch(imjd, midsecs/86400.0);
  integration->set_epoch(epoch);

  integration->uniform_weight(1.0);

#if 1 
  // Create pointing extension
  Reference::To<Pointing> pnt = new Pointing;
  pnt->set_right_ascension(get_coordinates().ra());
  pnt->set_declination(get_coordinates().dec());
  pnt->set_local_sidereal_time(0.0);
  pnt->set_galactic_longitude(0.0);
  pnt->set_galactic_latitude(0.0);
  pnt->set_feed_angle(0.0);
  pnt->set_position_angle(0.0);
  pnt->set_parallactic_angle(0.0);
  pnt->set_telescope_azimuth(0.0);
  pnt->set_telescope_zenith(0.0);
  pnt->update(integration,this);
  integration->add_extension(pnt);
#endif

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) {
    fits_close_file(f, &zstatus);
    delete [] midphase;
    delete [] midper;
    if (status)
      throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
          "FITS error");
    return integration;
  }

  // Load the actual data for each pol, channel from the file.
  // Still relies on column numbers... probably ok, though.
  float *data = new float[nbin]; // Temporary storage space
  int *count = new int[nbin];
  for (unsigned ichan=0; ichan<nchan; ichan++) {
    // Load counts for this chan
    fits_read_col(f, TINT, 5*ichan+5, 1, 1, nbin, NULL, count, NULL, &status);
    for (unsigned ipol=0; ipol<npol; ipol++) {
      // Load data for ipol, ichan 
      fits_read_col(f, TFLOAT, 5*ichan+ipol+1, 1, 1, nbin, NULL, data, NULL, 
          &status);
      // Normalize by counts
      for (unsigned ibin=0; ibin<nbin; ibin++) data[ibin] /= (float)count[ibin];
      // Rotate to align phase0 w/ epoch.
      // TODO: This will change for multi-polyco support
      FTransform::shift(nbin, data, (double)nbin*(midphase[ichan]-refphase));
      // Put data in integration structure:
      integration->get_Profile(ipol,ichan)->set_amps(data);
    }
  }

  // Unallocate temp space
  delete [] data;
  delete [] count;
  delete [] midphase;
  delete [] midper;

  // close file
  fits_close_file(f, &zstatus);

  // Catch any FITS errors
  if (status) 
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
        "FITS Error");

  return integration;
}

void Pulsar::ASPArchive::load_extensions(fitsfile *f, int *status)
{

  // Telescope extension
  Telescope *t = getadd<Telescope>();
  Telescopes::set_telescope_info(t, this);

  // Backend extension
  Backend *b = getadd<Backend>();
  if ((get_telescope()=="1") || (get_telescope()=="a") 
      || (get_telescope()=="b")) {
    b->set_name("GASP"); 
    b->set_delay(32.0*8.0/(128.0e6)/2.0); // 8x overlap PFB
  } else if (get_telescope()=="3") {
    b->set_name("ASP");
    b->set_delay(32.0*24.0/(128.0e6)/2.0); // 24x overlap PFB
  } else if (get_telescope()=="f") {
    b->set_name("LBP");
    b->set_delay(32.0*24.0/(128.0e6)/2.0); // 24x overlap PFB
  } else {
    b->set_name("xASP");
  }
  b->set_argument(Signal::Conventional); // XXX check this
  b->set_downconversion_corrected(false);

  // Replace tempo code with telescope name
  set_telescope(t->get_name());

  // ObsExtension
  char ctmp[64];
  ObsExtension *o = getadd<ObsExtension>();
  fits_movabs_hdu(f, 1, NULL, status);
  fits_read_key(f, TSTRING, "OBSERVER", ctmp, NULL, status);
  if (*status==0) o->observer = ctmp; 
  fits_read_key(f, TSTRING, "PROJID", ctmp, NULL, status);
  if (*status==0) o->project_ID = ctmp; 

  // Receiver ext
  Receiver *r = getadd<Receiver>();
  if (get_telescope()=="1" || get_telescope()=="GBT") {
    GBT::guess(r, this); // Uses center freq to determine rcvr
  } else if (get_telescope()=="3" || get_telescope()=="Arecibo") {
    Arecibo::guess(r, this); // Uses center freq
    // Arecibo recvrs need hand=-1, at least for ASP.  So far
    // I've only checked this for L-wide and S-wide (July 2008).
    r->set_hand(Signal::Left);
  } 
  // If still no recvr found, use info from FITS
  if (r->get_name()=="unknown") {
    fits_read_key(f, TSTRING, "FRONTEND", ctmp, NULL, status);
    if (*status==0) r->set_name(ctmp);
  }
  // Override default receiver pol basis with info from file
  // TODO: check whether what's in the files tends to be right..
  // Also find out if we can note the use of a hybrid separately.
  fits_read_key(f, TSTRING, "FD_POLN", ctmp, NULL, status);
  if (*status==0) {
    if ((ctmp[0]=='L') || (ctmp[0]=='l')) r->set_basis(Signal::Linear);
    if ((ctmp[0]=='C') || (ctmp[0]=='c')) r->set_basis(Signal::Circular);
  }

  // CalInfo Extension
  CalInfoExtension *c = getadd<CalInfoExtension>();
  if (get_type()==Signal::PolnCal
      || get_type()==Signal::FluxCalOn
      || get_type()==Signal::FluxCalOff) {
    c->cal_mode = "EXT1";  // Is this meaningful??
    c->cal_frequency = 25.0;
    c->cal_dutycycle = 0.5;
    c->cal_phase = 0.0; // We generally don't know this
  } else {
    // Fill this in even in non-cal mode to make PSRFITS happy.
    c->cal_mode = "OFF";
    c->cal_frequency = 0.0;
    c->cal_dutycycle = 0.0;
    c->cal_phase = 0.0;
  }

}

void Pulsar::ASPArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ASPArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // We don't want to save files in ASP format.
  throw Error (InvalidState, "Pulsar::ASPArchive::unload_file",
      "unload not implemented for ASPArchive");

}

string Pulsar::ASPArchive::Agent::get_description () 
{
  return "ASP FITS versions 1.0, 1.0.1";
}

bool Pulsar::ASPArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by ASP Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    ASPArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    if (verbose>2)
      cerr << "ASP load failed due to: " << e << endl;
    return false;
  }

  return false;
}


