#include "Pulsar/PuMaArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Pulsar.h"

#include <libpuma.h>
#include <bigendian.h>

using namespace std;

/* Plans:

   hdr.redn.Command - will incorporate into Backend Extension

*/

void Pulsar::PuMaArchive::init ()
{
  npol = 0;
  header_bytes = sizeof(Header_type);

  // runtime determination of machine endian! - courtesy of R. Edwards
  int one = 1;
  unsigned char *bytep = (unsigned char *)&one;

  little_endian = (bytep[0]==1);
}

Pulsar::PuMaArchive::PuMaArchive()
{
  init ();
}

Pulsar::PuMaArchive::~PuMaArchive()
{
}

Pulsar::PuMaArchive::PuMaArchive (const Archive& arch)
{
  if (verbose == 3)
    cerr << "PuMaArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::PuMaArchive::PuMaArchive (const PuMaArchive& arch)
{
  if (verbose == 3)
    cerr << "PuMaArchive construct copy PuMaArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::PuMaArchive::copy (const Archive& archive) 
{
  if (verbose == 3)
    cerr << "PuMaArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose == 3)
    cerr << "PuMaArchive::copy dynamic cast call" << endl;
  
  const PuMaArchive* farchive = dynamic_cast<const PuMaArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose == 3)
    cerr << "PuMaArchive::copy another PuMaArchive" << endl;
  
  hdr   = farchive->hdr;
  npol  = farchive->npol;
  state = farchive->state;
  source= farchive->source;

}

Pulsar::Archive* Pulsar::PuMaArchive::clone () const
{
  if (verbose == 3)
    cerr << "PuMaArchive::clone" << endl;
  return new PuMaArchive (*this);
}

//! Get the number of pulsar phase bins used
unsigned Pulsar::PuMaArchive::get_nbin () const
{
  return hdr.redn.NBins;
}

//! Set the number of pulsar phase bins
void Pulsar::PuMaArchive::set_nbin (unsigned numbins)
{
  hdr.redn.NBins = numbins;
}

//! Get the number of frequency channels used
unsigned Pulsar::PuMaArchive::get_nchan () const
{
  return hdr.redn.NFreqs;
}

//! Set the number of frequency channels
void Pulsar::PuMaArchive::set_nchan (unsigned numchan)
{
  hdr.redn.NFreqs = numchan;
}

//! Get the number of frequency channels used
unsigned Pulsar::PuMaArchive::get_npol () const
{
  return npol;
}

//! Set the number of polarization measurements
void Pulsar::PuMaArchive::set_npol (unsigned numpol)
{
  npol = numpol;
}

//! Get the number of sub-integrations stored in the file
unsigned Pulsar::PuMaArchive::get_nsubint () const
{
  return hdr.redn.NTimeInts;
}

//! Set the number of sub-integrations
void Pulsar::PuMaArchive::set_nsubint (unsigned nsubint)
{
  hdr.redn.NTimeInts = nsubint;
}

//! Get the telescope name
std::string Pulsar::PuMaArchive::get_telescope () const
{
  return "wsrt";
}

//! Set the telescope name
void Pulsar::PuMaArchive::set_telescope (const string&)
{
  warning << "Pulsar::PuMaArchive::set_telescope unimplemented" << endl;
}


//! Get the state of the profiles
Signal::State Pulsar::PuMaArchive::get_state () const
{
  return state;
}

//! Set the state of the profiles
void Pulsar::PuMaArchive::set_state (Signal::State _state)
{
  state = _state;
}


//! Get the scale of the profiles
Signal::Scale Pulsar::PuMaArchive::get_scale () const
{
  if (hdr.redn.GenType & PuMa_Absolute_Flux_Density)
    return Signal::Jansky;
  else if (hdr.redn.GenType & PuMa_Reference_Flux_Density)
    return Signal::ReferenceFluxDensity;
  else
    return Signal::FluxDensity;
}

//! Set the scale of the profiles
void Pulsar::PuMaArchive::set_scale (Signal::Scale scale)
{
  switch (scale) {
    case Signal::FluxDensity:
      set_GenType (PuMa_Reference_Flux_Density, false);
      set_GenType (PuMa_Absolute_Flux_Density, false);
      break;
    case Signal::ReferenceFluxDensity:
      set_GenType (PuMa_Reference_Flux_Density, true);
      set_GenType (PuMa_Absolute_Flux_Density, false);
      break;
    case Signal::Jansky:
      set_GenType (PuMa_Reference_Flux_Density, false);
      set_GenType (PuMa_Absolute_Flux_Density, true);
      break;
    default:
      throw Error (InvalidParam, "Pulsar::TimerArchive::set_scale",
                   "unknown scale");
  }
}

//! Get the observation type (psr, cal)
Signal::Source Pulsar::PuMaArchive::get_type () const
{
  return source;
}

//! Set the observation type (psr, cal)
void Pulsar::PuMaArchive::set_type (Signal::Source type)
{
  source = type;
}

//! Get the source name
string Pulsar::PuMaArchive::get_source () const
{
  unsigned offset = 0;
  if (strncmp (hdr.src.Pulsar, "PSR ", 4) == 0)
    offset = 4;

  return string (hdr.src.Pulsar + offset);
}

//! Set the source name
void Pulsar::PuMaArchive::set_source (const string& source)
{
  /* PuMa uses the Taylor (1993) convention. e.g. "PSR J0218+4232" */
  strcpy (hdr.src.Pulsar, "PSR ");
  strcat (hdr.src.Pulsar, source.c_str());
}


//! Get the coordinates of the source
sky_coord Pulsar::PuMaArchive::get_coordinates () const
{
  sky_coord position;
  
  /* double RA;  RA of the target (in radians)  */
  position.ra().setRadians( hdr.src.RA );
  /* double Dec; Dec of the target (in radians) */
  position.dec().setRadians( hdr.src.Dec );
 
  return position;
}

//! Set the coordinates of the source
void Pulsar::PuMaArchive::set_coordinates (const sky_coord& coordinates)
{
  hdr.src.RA = coordinates.ra().getRadians();
  hdr.src.Dec = coordinates.dec().getRadians();
}

//! Get the centre frequency of the observation
double Pulsar::PuMaArchive::get_centre_frequency () const
{
  return hdr.redn.FreqCent;
}

//! Set the centre frequency of the observation
void Pulsar::PuMaArchive::set_centre_frequency (double cf)
{
  hdr.redn.FreqCent = cf;
}

//! Get the overall bandwidth of the observation
double Pulsar::PuMaArchive::get_bandwidth () const
{
  return hdr.redn.DeltaFreq * hdr.redn.NFreqs;
}

//! Set the overall bandwidth of the observation
void Pulsar::PuMaArchive::set_bandwidth (double bw)
{
  hdr.redn.DeltaFreq = bw / hdr.redn.NFreqs;
}

//! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
double Pulsar::PuMaArchive::get_dispersion_measure () const
{
  return hdr.redn.DM;
}

//! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
void Pulsar::PuMaArchive::set_dispersion_measure (double dm)
{
  hdr.redn.DM = dm;
}

//! Inter-channel dispersion delay has been removed
bool Pulsar::PuMaArchive::get_dedispersed () const
{
  return hdr.redn.IsDedisp;
}

//! Set the status of the parallactic angle flag
void Pulsar::PuMaArchive::set_dedispersed (bool done)
{
  hdr.redn.IsDedisp = done;
}

double Pulsar::PuMaArchive::get_rotation_measure () const
{
  return hdr.obs.IonRM;
}

void Pulsar::PuMaArchive::set_rotation_measure (double rm)
{
  hdr.obs.IonRM = rm;
}


//! Data has been corrected for ISM faraday rotation
bool Pulsar::PuMaArchive::get_faraday_corrected () const
{
  return hdr.redn.GenType & PuMa_RM_Corrected;
}

//! Set the status of the ISM RM flag
void Pulsar::PuMaArchive::set_faraday_corrected (bool done)
{
  set_GenType (PuMa_RM_Corrected, done);
}


//! Data has been poln calibrated
bool Pulsar::PuMaArchive::get_poln_calibrated () const
{
  return hdr.redn.GenType & PuMa_Polarization_Calibrated;
}

//! Set the status of the poln calibrated flag
void Pulsar::PuMaArchive::set_poln_calibrated (bool done)
{
  set_GenType (PuMa_Polarization_Calibrated, done);
}

//! Set the status of the poln calibrated flag
void Pulsar::PuMaArchive::set_GenType (int code, bool done)
{
  if (done)
    hdr.redn.GenType |= code;
  else
    hdr.redn.GenType &= ~code;
}


//! The subints vector will point to BasicIntegrations
Pulsar::Integration* 
Pulsar::PuMaArchive::new_Integration (const Integration* subint)
{
  Integration* integration;

  if (subint)
    integration = new BasicIntegration (subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "BasicArchive::new_Integration");
  
  return integration;
}


void Pulsar::PuMaArchive::load_header (Header_type& hdr, const char* filename)
{
  FILE* pfile = fopen (filename, "r");
  if (!pfile)
    throw Error (FailedSys, "Pulsar::PuMaArchive::load_header",
		 "fopen(%s)", filename);

  int ret = prheader (&hdr, pfile);

#if 0

  int              ret;
  Header_type      behdr;
  Header_type      behdr2;

#if defined ( __linux__) || defined (__alpha__)
  cerr << "LINUX!" << endl;
  ret = fread(&behdr,sizeof(Header_type),1,pfile);
  header_convert(&behdr,&hdr);
#else
  ret = fread(&hdr,sizeof(Header_type),1,pfile);
#endif

#endif

  fclose (pfile);


  if (!ret)
    throw Error (FailedCall, "Pulsar::PuMaArchive::load_header", "prheader");

  if (hdr.redn.Raw)
    throw Error (InvalidState, "Pulsar::PuMaArchive::load_header",
		 "%s contains raw data", filename);

  //#if 0

  if (!hdr.redn.Folded)
    throw Error (InvalidState, "Pulsar::PuMaArchive::load_header",
		 "%s does not contain folded data", filename);

  //#endif

  if (hdr.redn.IsPwr)
    throw Error (InvalidState, "Pulsar::PuMaArchive::load_header",
		 "%s contains power spectrum data", filename);

  /* Check on the number of polarisations present */

  if( hdr.redn.OP || hdr.redn.OTheta ||
      hdr.redn.Op || hdr.redn.Ov || hdr.redn.Opoldeg )
    throw Error (InvalidState, "Pulsar::PuMaArchive::load_header",
		 "%s contains unsupported data state", filename);
}

void Pulsar::PuMaArchive::load_header (const char* filename)
{

  load_header (hdr, filename);

  if( hdr.redn.OI ) {

    if( hdr.redn.OQ && hdr.redn.OU && hdr.redn.OV ) {
      npol = 4;
      state = Signal::Stokes;
    }
    else {
      npol = 1;
      state = Signal::Intensity;
    }

  }
  else if( hdr.redn.OX && hdr.redn.OY ) {
    npol = 2;
    state = Signal::PPQQ;
  }
  else
    throw Error (InvalidState, "Pulsar::PuMaArchive::load_header",
		 "%s contains unsupported data state", filename);

  // TODO
  source = Signal::Pulsar;

  /* char ObsType[TXTLEN]; Research/test/calibration */
  if (verbose == 3) cerr << "Pulsar::PuMaArchive::load_header ObsType=" 
			 << hdr.obs.ObsType << endl;

  header_bytes = sizeof(Header_type);

  if (hdr.redn.Polyco && verbose == 3)
    cerr << "Pulsar::PuMaArchive::load_header polyco driven folding\n"
      "  ncoef = " << hdr.redn.NCoef << "\n"
      "  span  = " << hdr.redn.PolycoSpan << endl;


  if (hdr.redn.PolycoStored)
    cerr << "Pulsar::PuMaArchive::load_header polyco stored in Parameter Block"
	 << endl;

  if (hdr.redn.IsAdjusted)
    cerr << "Pulsar::PuMaArchive::load_header adjustments have been applied"
	 << endl;
}

Pulsar::Integration*
Pulsar::PuMaArchive::load_Integration (const char* filename, unsigned subint)
{
  if (verbose == 3)
    cerr << "Pulsar::PuMaArchive::load_Integration subint=" << subint 
	 << " nbin=" << get_nbin() << " npol=" << get_npol() 
	 << " nchan=" << get_nchan() << endl;

  Pulsar::BasicIntegration* integration = new BasicIntegration;

  resize_Integration (integration);
  init_Integration (integration);

  MJD epoch (hdr.redn.MJDint, hdr.redn.MJDfrac);
  epoch += double(subint) * hdr.redn.DeltaTime;

  integration->set_epoch (epoch);
  integration->set_duration (hdr.redn.DeltaTime);

  integration->set_folding_period (hdr.redn.FoldPeriod);

  FILE* pfile = fopen (filename, "r");
  if (!pfile)
    throw Error (FailedSys, "Pulsar::PuMaArchive::load_Integration",
		 "fopen(%s)", filename);

  unsigned nread = get_nbin();
  unsigned seek_between = get_nsubint() * nread * sizeof(float);
  unsigned offset = header_bytes + subint * nread * sizeof(float);

  // from rpuma.c
  // first_freq = hdr.redn.FreqCent - ((p.nsub -1) * hdr.redn.DeltaFreq/2);

  double first_freq = hdr.redn.FreqCent-((get_nchan()-1)*hdr.redn.DeltaFreq/2);

  for (unsigned ipol=0; ipol < get_npol(); ipol++) {
    for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

      if (verbose == 3)
	cerr << "Pulsar::PuMaArchive::load_Integration isub=" << subint
	     << " ipol=" << ipol << " ichan=" << ichan 
	     << " offset=" << offset << endl;

      fseek (pfile, offset, SEEK_SET);
      offset += seek_between;

      Profile* profile = integration->get_Profile (ipol, ichan);

      float* buffer = profile->get_amps();

      if (!buffer)
	throw Error (InvalidState, "Pulsar::PuMaArchive::load_Integration",
		     "Profile(ipol=%d, ichan=%d) unitialized");

      size_t got = fread (buffer, sizeof(float), nread, pfile);

      if (got < nread)
	throw Error (FailedSys, "Pulsar::PuMaArchive::load_Integration",
		     "fread %d floats", nread);

      if (little_endian) {
	if (verbose == 3) cerr << "Pulsar::PuMaArchive::load_Integration"
			    " convert endian" << endl;
	ConvertArrayFromBE (buffer, nread, sizeof(float));
      }

      profile->set_weight (1.0);

      // from rpuma.c
      /* shift for each channel relative to the centre frequency */
      double ch_freq = first_freq + ichan*hdr.redn.DeltaFreq;

      profile->set_centre_frequency (ch_freq);

    }
  }

  fclose (pfile);

  return integration;
}

void Pulsar::PuMaArchive::unload_file (const char* filename) const
{
  Header_type* unconst_hdr = const_cast<Header_type*>(&hdr);

  /* Adjusting the reduction time */
  /* int TRedn; Time at moment of reduction (sec after 1970) */
  unconst_hdr->redn.TRedn = time(0);

  /* Filling in the command line slot in the header */
  string operations;
  strncpy (unconst_hdr->redn.Command, operations.c_str(), CMDLEN);

  FILE* pfile = fopen (filename, "r");
  if (!pfile)
    throw Error (FailedSys, "Pulsar::PuMaArchive::unload_file",
		 "fopen(%s)", filename);

  if (!pwheader (unconst_hdr, pfile)) {
    fclose (pfile);
    throw Error (FailedCall, "Pulsar::PuMaArchive::unload_file", "pwheader");
  }

  int nwrite = 0;
  float* buffer = 0;
  if (pumawrite (buffer,sizeof(float),nwrite,pfile) < nwrite-1)
    throw Error (FailedCall, "Pulsar::PuMaArchive::unload_file", "pumawrite");
}


Pulsar::PuMaArchive::Agent::Agent ()
{
  // cerr << "Pulsar::PuMaArchive::Agent construct" << endl;
}

string Pulsar::PuMaArchive::Agent::get_description () 
{
  char version [16];
  char garbage [128];
  sscanf (puma_h_RCSid, "%s %s %s", garbage, garbage, version);

  return "PuMa Archive Version " + string(version);
}

bool Pulsar::PuMaArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by PuMa Archive, this method should return true

  try {
    Header_type header;
    load_header (header, filename);
  }
  catch (Error& error) {
    if (verbose == 3)
      cerr << "Pulsar::PuMaArchive::Agent::advocate not a PuMa file:\n"
	   << error.warning() << endl;
    return false;
  }

  return true;
}

