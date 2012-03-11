/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Pulsar.h"
#include "Pulsar/TimerArchive.h"
#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Telescope.h"
#include "Horizon.h"
#include "polyco.h"

#include "Error.h"

#include "timer++.h"
#include "strutil.h"

#include <unistd.h>
#include <string.h>

using namespace std;

//! files are big endian by default
bool Pulsar::TimerArchive::big_endian = true;

//
//
//
Pulsar::TimerArchive::TimerArchive ()
{
  if (verbose == 3) {
    cerr << "Pulsar::TimerArchive default constructor" << endl;
    Timer::verbose = true;
  }

  Timer::init (&hdr);
  valid = false;
}

//
//
//
Pulsar::TimerArchive::TimerArchive (const TimerArchive& arch)
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive copy construct" << endl;

  Timer::init (&hdr);
  Archive::copy (arch); // results in call to TimerArchive::copy
}

//
//
//
Pulsar::TimerArchive::~TimerArchive ()
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive destructor" << endl;
}

//
//
//
const Pulsar::TimerArchive&
Pulsar::TimerArchive::operator = (const TimerArchive& arch)
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to TimerArchive::copy
  return *this;
}

//
//
//
Pulsar::TimerArchive::TimerArchive (const Archive& arch)
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive base copy construct" << endl;

  Timer::init (&hdr);
  Archive::copy (arch); // results in call to TimerArchive::copy
}

//
//
//
void Pulsar::TimerArchive::copy (const Archive& archive) 
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::copy Entering" << endl;

  if (this == &archive)
  {
    if (verbose == 3)
      cerr << "Returning from TimerArchive::copy early " << endl;
    return;
  }
  else
  {
    if (verbose == 3)
      cerr << "this == &archive passed test " << endl;
  }
  
  Archive::copy (archive);

  const TimerArchive* tarchive = dynamic_cast<const TimerArchive*>(&archive);
  if (!tarchive)
    return;

  unsigned good_nsubint = get_nsubint();

  if (verbose == 3) cerr << "Pulsar::TimerArchive::copy copying headers " << endl;
  hdr = tarchive->hdr;

  set_nsubint( good_nsubint );

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::copy check validity" << endl;

  valid = tarchive->valid;
}

//
//
//
Pulsar::TimerArchive* Pulsar::TimerArchive::clone () const
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::clone" << endl;
  return new TimerArchive (*this);
}

/*!  
  By over-riding this funciton, inherited types may re-define the type
  of Integration to which the elements of the subints vector point.
*/
Pulsar::Integration* 
Pulsar::TimerArchive::new_Integration (const Integration* subint)
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::new_Integration" << endl;

  TimerIntegration* integration;

  if (subint)
    integration = new TimerIntegration (*subint);
  else
    integration = new TimerIntegration;

  if (!integration)
    throw Error (BadAllocation, "Pulsar::TimerArchive::new_Integration");
  
  return integration;
}

//! Returns a pointer to a newly loaded TimerIntegration instance
Pulsar::Integration* 
Pulsar::TimerArchive::load_Integration (const char* filename, unsigned isubint)
{
  throw Error (InvalidState, "Pulsar::TimerArchive::load_Integration",
	       "not implemented");
}


std::string Pulsar::TimerArchive::get_telescope () const
{
  return hdr.telid;
}

void Pulsar::TimerArchive::set_telescope (const std::string& code)
{
  if (code.length() > TELID_STRLEN-1)
    throw Error (InvalidParam, "Pulsar::TimerArchive::set_telescope",
		 "code string='%s' length > TELID_STRLEN=%d",
		 code.c_str(), TELID_STRLEN);

  strncpy (hdr.telid, code.c_str(), TELID_STRLEN);
}

Signal::Source Pulsar::TimerArchive::get_type () const
{
  switch (hdr.obstype)
    {
    case PULSAR:
      return Signal::Pulsar;
    case CAL:
      return Signal::PolnCal;
    case HYDRA:
      return Signal::FluxCalOn;
    case NHYDRA:
    case SHYDRA:
      return Signal::FluxCalOff;
    case FLUX_ON:
      return Signal::FluxCalOn;
    case FLUX_OFF:
      return Signal::FluxCalOff;
    default:
      return Signal::Unknown;
    }
}

int Pulsar::TimerArchive::hydra_obstype ()
{
  double dec = get_coordinates().dec().getDegrees();

  /* figure out the obstype */
  if      ((dec <= -9.083) && (dec > -11.083))
    return NHYDRA;
  else if ((dec <= -11.083) && (dec > -13.083))
    return HYDRA;
  else if ((dec <= -13.083) && (dec > -15.083))
    return SHYDRA;

  throw Error (InvalidRange, "Pulsar::TimerArchive::hydra_obstype",
	       "invalid HYDRACAL dec=%lf", dec);
}

void Pulsar::TimerArchive::set_type (Signal::Source type)
{
  switch (type)
    {
    case Signal::Pulsar:
      hdr.obstype = PULSAR;
      break;
    case Signal::PolnCal:
      hdr.obstype = CAL;
      break;
    case Signal::FluxCalOn:
      hdr.obstype = FLUX_ON;
      break;
    case Signal::FluxCalOff:
      hdr.obstype = FLUX_OFF;
      break;
    default:
      cerr << "Pulsar::TimerArchive::set_type warning unrecognized type="
	   << type << endl;
      hdr.obstype = -1;
      break;
    }
}

string Pulsar::TimerArchive::get_source () const
{
  return string (hdr.psrname);
}

void Pulsar::TimerArchive::set_source (const string& source)
{
  strncpy (hdr.psrname, source.c_str(), PSRNAME_STRLEN);
  hdr.psrname[PSRNAME_STRLEN-1]='\0';
}

//! Get the coordinates of the source
sky_coord Pulsar::TimerArchive::get_coordinates () const
{
  sky_coord coordinates;
  coordinates.ra().setRadians( hdr.ra );
  coordinates.dec().setRadians( hdr.dec );

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::get_coordinates " << coordinates << endl;

  return coordinates;
}

//! Set the coordinates of the source
void Pulsar::TimerArchive::set_coordinates (const sky_coord& coordinates)
{
  hdr.dec = coordinates.dec().getRadians();
  hdr.ra  = coordinates.ra().getRadians();

  AnglePair galactic = coordinates.getGalactic();
  hdr.l = galactic.angle1.getDegrees();
  hdr.b = galactic.angle1.getDegrees();
}
 
unsigned Pulsar::TimerArchive::get_nbin () const
{
  if (hdr.nbin < 0)
    return 0;
  return hdr.nbin;
}

void Pulsar::TimerArchive::set_nbin (unsigned numbins)
{
  hdr.nbin = numbins;
}


unsigned Pulsar::TimerArchive::get_nchan () const
{
  if (hdr.nsub_band < 0)
    return 0;
  return hdr.nsub_band;
}

void Pulsar::TimerArchive::set_nchan (unsigned numchan)
{
  hdr.nsub_band = numchan;
}


unsigned Pulsar::TimerArchive::get_npol () const
{  
  if (hdr.banda.npol < 0)
    return 0;
  return hdr.banda.npol;
}

void Pulsar::TimerArchive::set_npol (unsigned numpol)
{
  hdr.banda.npol = numpol;
}

unsigned Pulsar::TimerArchive::get_nsubint () const
{
  if (hdr.nsub_int < 0)
    return 0;
  return hdr.nsub_int;
}

void Pulsar::TimerArchive::set_nsubint (unsigned nsubint)
{
  hdr.nsub_int = nsubint;
}

double Pulsar::TimerArchive::get_bandwidth () const
{
  return hdr.banda.bw;
}

void Pulsar::TimerArchive::set_bandwidth (double bw)
{
  hdr.banda.bw = bw;
}

double Pulsar::TimerArchive::get_centre_frequency () const
{
  return hdr.banda.centrefreq;
}

void Pulsar::TimerArchive::set_centre_frequency (double cf)
{
  hdr.banda.centrefreq = cf;
}


Signal::State Pulsar::TimerArchive::get_state () const
{
  // from band.h, version 1
  /* (T) 0=I,1=AA BB,2=AA,AB,BA,BB, 3=AA, 4=BB   */

  // Willem van Straten added the poln_storage_type =
  // [IQ|XY]_POLN_STORAGE variable to distinguish between coherence
  // products and stokes parameters states

  // Willem van Straten changed this so that band.correlator_mode
  // could store the polarization state in the header of the data file
  // without making any assumptions regarding the machine

  // correlator_mode == 6 now distingishes between IQUV and AA,AB,BA,BB

  // correlator_mode == 5 added to handle storage of invariant interval

  switch (hdr.banda.correlator_mode) 
    {
    case 0:
      return Signal::Intensity;
    case 1:
      return Signal::PPQQ;
    case 2:
      return Signal::Coherence;
    case 5:
      return Signal::Invariant;
    case 6:
      return Signal::Stokes;
    default:
      return Signal::Intensity;
    }
}

void Pulsar::TimerArchive::set_state (Signal::State state)
{
  switch (state) 
    {

    case Signal::Intensity:
      hdr.banda.correlator_mode = 0;
      break;

    case Signal::PPQQ:
      hdr.banda.correlator_mode = 1;
      break;

    case Signal::Coherence:
      hdr.banda.correlator_mode = 2;
      break;

    case Signal::Invariant:
      hdr.banda.correlator_mode = 5;
      break;

    case Signal::Stokes:
      hdr.banda.correlator_mode = 6;
      break;

    default:
      hdr.banda.correlator_mode = -1;
      throw Error (InvalidParam, "Pulsar::TimerArchive::set_state",
		   "unrecognized state=" + State2string(state));
    }
}

//! Get the scale of the profiles
Signal::Scale Pulsar::TimerArchive::get_scale () const
{
  if (hdr.calibrated & FLUX_CALIBRATED)
    return Signal::Jansky;
  else if (hdr.calibrated & FLUX_REFERENCE)
    return Signal::ReferenceFluxDensity;
  else
    return Signal::FluxDensity;
}

//! Set the scale of the profiles
void Pulsar::TimerArchive::set_scale (Signal::Scale scale)
{
  switch (scale) {
    case Signal::FluxDensity:
      set_calibrated (FLUX_REFERENCE, false);
      set_calibrated (FLUX_CALIBRATED, false);
      break;
    case Signal::ReferenceFluxDensity:
      set_calibrated (FLUX_REFERENCE, true);
      set_calibrated (FLUX_CALIBRATED, false);
      break;
    case Signal::Jansky:
      set_calibrated (FLUX_REFERENCE, false);
      set_calibrated (FLUX_CALIBRATED, true);
      break;
    default:
      throw Error (InvalidParam, "Pulsar::TimerArchive::set_scale",
                   "unknown scale");
  }
}


//! Get the centre frequency of the observation
double Pulsar::TimerArchive::get_dispersion_measure () const
{
  return hdr.dm;
}

//! Set the centre frequency of the observation
void Pulsar::TimerArchive::set_dispersion_measure (double dm)
{
  hdr.dm = dm;
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::TimerArchive::get_rotation_measure () const
{
  return hdr.rotm;
}

//! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
void Pulsar::TimerArchive::set_rotation_measure (double rm)
{
  hdr.rotm = rm;
}


bool Pulsar::TimerArchive::get_poln_calibrated () const
{
  return hdr.calibrated & POLN_CALIBRATED;
}

void Pulsar::TimerArchive::set_poln_calibrated (bool done)
{
  set_calibrated (POLN_CALIBRATED, done);
}


bool Pulsar::TimerArchive::get_faraday_corrected () const
{
  return hdr.corrected & RM_ISM_CORRECTED;
}

void Pulsar::TimerArchive::set_faraday_corrected (bool done)
{
  set_corrected (RM_ISM_CORRECTED, done);
}


bool Pulsar::TimerArchive::get_dedispersed () const
{
  return hdr.corrected & DEDISPERSED;
}

void Pulsar::TimerArchive::set_dedispersed (bool done)
{
  set_corrected (DEDISPERSED, done);
}

void Pulsar::TimerArchive::set_corrected (int code, bool done)
{
  if (done)
    hdr.corrected |= code;
  else
    hdr.corrected &= ~code;
}

void Pulsar::TimerArchive::set_calibrated (int code, bool done)
{
  if (done)
    hdr.calibrated |= code;
  else
    hdr.calibrated &= ~code;
}

void Pulsar::TimerArchive::correct_Integrations () try {

  if( get_nsubint()==0 )
    return;

  Telescope* telescope = get<Telescope>();
  if (!telescope)
    cerr << "Pulsar::TimerArchive::correct_Integrations WARNING "
            "no Telescope extension" << endl;

  TimerIntegration* subint;

  for (unsigned isub=0; isub<get_nsubint(); isub++) {

    subint = dynamic_cast<TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "Pulsar::TimerArchive::correct_Integrations",
		   "Integration[%d] is not a TimerIntegration", isub);

    if (subint->get_duration () <= 0.0) {
      if (verbose == 3)
	cerr << "Pulsar::TimerArchive::correct_Integrations"
	  " warning empty sub-int " << isub << endl;
      continue;
    }

    if (telescope)
    {
      Horizon hzon;

      hzon.set_epoch (subint->get_epoch());
      hzon.set_source_coordinates (get_coordinates());
      hzon.set_observatory_latitude (telescope->get_latitude().getRadians());
      hzon.set_observatory_longitude (telescope->get_longitude().getRadians());

      // correct the mini header LST
      subint->mini.lst_start = hzon.get_local_sidereal_time() * 12.0/M_PI;

      double rad2deg = 180.0/M_PI;
      subint->mini.tel_az = hzon.get_azimuth() * rad2deg;
      subint->mini.tel_zen = hzon.get_zenith() * rad2deg;
      subint->mini.para_angle = hzon.get_parallactic_angle() * rad2deg;
    }

    // set the mini header version
    subint->mini.version = 1.1;

    // feed angle is unknown
    // subint->mini.feed_ang = 0.0;

    // garbage fields
    subint->mini.junk = subint->mini.junk2 = subint->mini.junk3 = 0;
    // subint->mini.pulse_phase = -99999999;
    // subint->mini.flux_A = mini.flux_B = -9999.0;

  }
}
catch (Error& error) {
  throw error += "Pulsar::TimerArchive::correct_Integrations";
}

void Pulsar::TimerArchive::correct () try
{
  MJD mjd = start_time();

  // correct the MJD
  hdr.mjd     = mjd.intday  ();  
  hdr.fracmjd = mjd.fracday ();

  // correct the utdate string
  mjd.datestr (hdr.utdate, 16, "%d-%m-%Y");

  try
  {
    Telescope* telescope = getadd<Telescope>();
    telescope->set_coordinates (get_telescope());

    // correct the LST
    hdr.lst_start = mjd.LST (telescope->get_longitude().getDegrees());
  }
  catch (Error& error)
  {
    if (verbose > 2)
      warning << "Pulsar::TimerArchive::correct"
                 " could not set telescope coordinates \n\t"
              << error.get_message() << endl;
  }

  // correct the folding period and integration length
  hdr.nominal_period = 0.0;
  hdr.dump_time = 0.0;
  hdr.sub_int_time = 0.0;

  if (get_nsubint() > 0) {
    hdr.nominal_period = get_Integration(0) -> get_folding_period();
    hdr.sub_int_time = get_Integration(0) -> get_duration();
    hdr.dump_time = hdr.sub_int_time;
  }

  // not sure what these mean
  hdr.narchive_int = 1;
  hdr.ndump_sub_int = 1;

  // correct the polyco parameters
  polyco* t1model = dynamic_cast<polyco*> (model.ptr());
  if (t1model) {
    hdr.nspan = (int) t1model->get_nspan();
    hdr.ncoeff = t1model->get_ncoeff();
  }
  else
    hdr.nspan = hdr.ncoeff = 0;

  /* General info */
  char hostname[50];
  if (gethostname (hostname,50) != 0)
    strcpy (hostname, "unkown host");

  utc_t utc_start;
  mjd.UTC (&utc_start, NULL);
  utc2str (hdr.schedule, utc_start, "yyyydddhhmmss");

  time_t now;
  now = time(NULL);
  snprintf (hdr.comment, COMMENT_STRLEN, "TimerArchive created on %.12s - %s",
	   hostname, ctime(&now));

  correct_Integrations();

  // data will be unloaded in the new style
  hdr.wts_and_bpass = 1;
  valid = true;
}
catch (Error& error) {
  throw error += "Pulsar::TimerArchive::correct";
}

