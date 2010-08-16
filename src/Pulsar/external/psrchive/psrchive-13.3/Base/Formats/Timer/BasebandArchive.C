/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BasebandArchive.h"
#include "Error.h"

#include <string.h>

#include "machine_endian.h"
#include "compressed_io.h"
#include "timer++.h"

using namespace std;

void init (baseband_header& bhdr)
{
  memset (&bhdr, 0, sizeof(baseband_header));

  bhdr.endian  = BASEBAND_HEADER_ENDIAN;
  bhdr.version = BASEBAND_HEADER_VERSION;
  bhdr.size =    BASEBAND_HEADER_SIZE;
}

void Pulsar::BasebandArchive::init ()
{
  Timer::init (&hdr);
  ::init (bhdr);
}

//
//
//
Pulsar::BasebandArchive::BasebandArchive ()
{
  if (verbose == 3)
    cerr << "BasebandArchive default construct" << endl;

 init ();
}

//
//
//
Pulsar::BasebandArchive::BasebandArchive (const BasebandArchive& archive)
{
  if (verbose == 3)
    cerr << "BasebandArchive copy construct" << endl;

  init ();
  Archive::copy (archive); // results in call to BasebandArchive::copy
}

//
//
//
Pulsar::BasebandArchive::~BasebandArchive ()
{
  if (verbose == 3)
    cerr << "BasebandArchive destructor" << endl;
}

//
//
//
const Pulsar::BasebandArchive&
Pulsar::BasebandArchive::operator = (const BasebandArchive& arch)
{
  if (verbose == 3)
    cerr << "BasebandArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to BasebandArchive::copy
  return *this;
}

//
//
//
Pulsar::BasebandArchive::BasebandArchive (const Archive& archive)
{
  if (verbose == 3)
    cerr << "BasebandArchive base copy construct" << endl;

  init ();
  Archive::copy (archive); // results in call to BasebandArchive::copy
}

//
//
//
void Pulsar::BasebandArchive::copy (const Archive& archive)
{
  if (verbose == 3)
    cerr << "BasebandArchive::copy" << endl;

  if (this == &archive)
    return;

  TimerArchive::copy (archive);
  set_be_data_size ();

  const BasebandArchive* barchive;
  barchive = dynamic_cast<const BasebandArchive*>(&archive);
  if (!barchive)
    return;

  if (verbose == 3)
    cerr << "BasebandArchive::copy another BasebandArchive" << endl;

  bhdr = barchive->bhdr;
}



//
//
//
Pulsar::BasebandArchive* Pulsar::BasebandArchive::clone () const
{
  if (verbose == 3)
    cerr << "BasebandArchive::clone" << endl;
  return new BasebandArchive (*this);
}

// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////


string Pulsar::BasebandArchive::Agent::get_description ()
{
  return "TIMER Archive with Baseband Extensions version 4";
}

//! return true if filename refers to a timer archive
bool Pulsar::BasebandArchive::Agent::advocate (const char* filename)
{
  struct timer hdr;
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )
    return false;

  if ( strncmp (hdr.backend, "baseband", BACKEND_STRLEN) != 0 )
    return false;

  return true;
}


// //////////////////////////////////////////////////////////////////////
// Size of the baseband_header struct as a function of Version number
static unsigned header_sizes [BASEBAND_HEADER_VERSION + 1] =
{ 104,   // Version 0
  112,   // Version 1
  112,   // Version 2
  128,   // Version 3
  176,   // Version 4
  184,   // Version 5
  184    // Version 6 (memory alignment correction, no additions)
};

// //////////////////////////////////////////////////////////////////////
// FLAGS
#define DynamicLevels  0x0000000000000001
// 3 unused bits

// //////////////////////////////////////////////////////////////////////
// pre-detection data windowing in the time domain
#define TimeHanning    0x0000000000000010
#define TimeWelch      0x0000000000000020
#define TimeParzen     0x0000000000000040

// //////////////////////////////////////////////////////////////////////
// how the data was detected
#define Stokes         0x0000000000000100
#define Britton        0x0000000000000200

// //////////////////////////////////////////////////////////////////////
// post-detection smoothing in the time domain
#define DetectHanning  0x0000000000001000  // Hanning smoothing

#define TUsedVersion5  0x0000000000001371  // bits used up to Version 5

// //////////////////////////////////////////////////////////////////////
// how data was manipulated in frequency space
#define DeDispersion   0x0000000000000001  // coherent de-dispersion
#define FbankDedisp    0x0000000000000002  // kernel applied to subchannels
#define CorInstDepol   0x0000000000000004  // correct instrumental depol

#define ScatteredPower 0x0000000000000010  // scattered power correction
#define IntegratePband 0x0000000000001000  // integrated pass-band
#define PbandIntensity 0x0000000000010000  // pass-band is intensity

#define FUsedVersion5  0x0000000000011017  // bits used up to Version 5

void Pulsar::BasebandArchive::convert_hdr_Endian ()
{
  ChangeEndian (bhdr.endian);
  ChangeEndian (bhdr.version);
  ChangeEndian (bhdr.size);
  
  ChangeEndian (bhdr.totaljobs);
  ChangeEndian (bhdr.gulpsize);
  ChangeEndian (bhdr.seeksize);
  
  ChangeEndian (bhdr.voltage_state);
  ChangeEndian (bhdr.analog_channels);
  ChangeEndian (bhdr.ppweight);
  ChangeEndian (bhdr.dls_threshold_limit);
  
  ChangeEndian (bhdr.pband_channels);
  ChangeEndian (bhdr.pband_resolution);
  ChangeEndian (bhdr.f_resolution);
  ChangeEndian (bhdr.t_resolution);
  
  ChangeEndian (bhdr.nfft);
  ChangeEndian (bhdr.nsmear_pos);
 
  ChangeEndian (bhdr.nscrunch);
  ChangeEndian (bhdr.power_normalization);

  ChangeEndian (bhdr.time_domain);
  ChangeEndian (bhdr.frequency_domain);
  ChangeEndian (bhdr.mean_power_cutoff);    // Version 1 addition
  ChangeEndian (bhdr.hanning_smoothing);    // Version 2 addition  
  ChangeEndian (bhdr.nsmear_neg);           // Version 5 addition
  ChangeEndian (bhdr.nchan);                // Version 5 addition
}

/*! Update the baseband_header struct with the current state of the
  Archive::Extension attributes */
void Pulsar::BasebandArchive::set_header ()
{
  ::init (bhdr);

  if (verbose == 3)
    cerr << "Pulsar::BasebandArchive::set_header" << endl;

  const dspReduction* reduction = get<dspReduction>();
  if (reduction) {

    bhdr.gulpsize = reduction->get_block_size();
    bhdr.seeksize = reduction->get_block_size() - reduction->get_overlap();
    if (bhdr.seeksize)
      bhdr.totaljobs = reduction->get_total_samples() / bhdr.seeksize;
    else
      bhdr.totaljobs = 0;

    bhdr.voltage_state = int( reduction->get_state() ) + 1;

    bhdr.nfft = reduction->get_nsamp_fft ();
    bhdr.nsmear_pos = reduction->get_nsamp_overlap_pos ();
    bhdr.nsmear_neg = reduction->get_nsamp_overlap_neg ();
    
    bhdr.nchan = reduction->get_nchan ();
    bhdr.nscrunch = reduction->get_ScrunchFactor ();
    
    bhdr.power_normalization = reduction->get_scale ();
    
    bhdr.f_resolution = reduction->get_freq_res ();
    bhdr.t_resolution = reduction->get_time_res ();

    string software = reduction->get_software ();
    if( software.length()+1 > SOFTWARE_STRLEN )
      throw Error (InvalidParam, "Pulsar::BasebandArchive::set_header",
		   "length of '%s'=%d > SOFTWARE_STRLEN=%d",
		   software.c_str(), software.length()+1, SOFTWARE_STRLEN);
    
    strcpy( hdr.software, software.c_str() );

  }

  check_extensions ();

  bhdr.time_domain = 0;
  bhdr.frequency_domain = 0;

  bhdr.mean_power_cutoff = 0;
  bhdr.hanning_smoothing = 0;

  bhdr.pcalid[0] = '\0';

}

void Pulsar::BasebandArchive::check_extensions ()
{
  const TwoBitStats* twobit = get<TwoBitStats>();
  if (twobit) {

    bhdr.ppweight = twobit->get_nsample ();
    bhdr.analog_channels = twobit->get_ndig ();
    bhdr.dls_threshold_limit = 0;

  }
  else
    bhdr.analog_channels = 0;

  const Passband* passband = get<Passband>();
  if (passband) {

    bhdr.pband_resolution = passband->get_nchan() * passband->get_nband();
    bhdr.pband_channels = passband->get_npol ();

    if (verbose == 3) cerr << "Pulsar::BasebandArchive::set_header Passband"
		   " nfreq=" << bhdr.pband_resolution <<
		   " nchan=" << bhdr.pband_channels << endl;

  }
  else
    bhdr.pband_resolution = bhdr.pband_channels = 0;
}


/*! Update the dspReduction attribute with the current state of the
  baseband_header struct */
void Pulsar::BasebandArchive::set_reduction ()
{
  if (verbose == 3)
    cerr << "Pulsar::BasebandArchive::set_reduction" << endl;

  dspReduction* reduction = getadd<dspReduction>();

  if (!reduction)
    return;

  reduction->set_block_size( bhdr.gulpsize );
  reduction->set_overlap( reduction->get_block_size() - bhdr.seeksize );
  reduction->set_total_samples( bhdr.totaljobs * bhdr.seeksize );

  reduction->set_state( Signal::State(bhdr.voltage_state - 1) );

  reduction->set_nsamp_fft ( bhdr.nfft );
  reduction->set_nsamp_overlap_pos ( bhdr.nsmear_pos );
  reduction->set_nsamp_overlap_neg ( bhdr.nsmear_neg );

  reduction->set_nchan ( bhdr.nchan );

  reduction->set_ScrunchFactor ( bhdr.nscrunch );

  reduction->set_scale ( bhdr.power_normalization );

  reduction->set_freq_res ( bhdr.f_resolution );
  reduction->set_time_res ( bhdr.t_resolution );

  reduction->set_name ( hdr.machine_id );
  reduction->set_software ( hdr.software );
}



void Pulsar::BasebandArchive::correct ()
{
  if (verbose == 3)
    cerr << "Pulsar::BasebandArchive::correct" << endl;

  set_be_data_size ();

  TimerArchive::correct ();
}


void Pulsar::BasebandArchive::set_be_data_size ()
{
  set_header ();
  check_be_data_size ();
}

void Pulsar::BasebandArchive::check_be_data_size ()
{
  check_extensions ();

  //
  // Calculate the size of the data as it will be written to disk
  //

  // size of the compressed float vector header (see fwrite_compressed)
  int c_hdr = (int) 2 * sizeof(float) + sizeof(uint64_t);
  // size of unsigned short (elements of compressed float vector
  int s_ush = (int) sizeof(unsigned short);

  bhdr.size = sizeof (baseband_header);
  if (bhdr.ppweight)
    bhdr.size += bhdr.analog_channels * ( c_hdr + bhdr.ppweight * s_ush );
  if (bhdr.pband_resolution)
    bhdr.size += bhdr.pband_channels * (c_hdr + bhdr.pband_resolution * s_ush);
  
  //
  // Set the information in the 'timer' header that allows TimerArchive
  // to ignore the information specific to this class
  //
  Timer::set_backend (&hdr, "baseband");
  hdr.be_data_size = bhdr.size;

}


void Pulsar::BasebandArchive::fix_header_memory_alignment ()
{
  if (verbose == 3)
    cerr << "BasebandArchive::fix_header_memory_alignment" << endl;

  int* realignment = &(bhdr.nchan);
  int* end_alignment = &(bhdr.memory_align);

  while (realignment > end_alignment) {
    *realignment = *(realignment - 1);
    realignment --;
  }
}

// written for the sake of fixing a limited set of broken files
// that first came out on monolith
static bool uint64_t_bug = false;

void Pulsar::BasebandArchive::backend_load (FILE* fptr)
{
  bool swap_endian = false;

  long file_start = ftell (fptr);

  if (fread (&bhdr, sizeof(baseband_header), 1, fptr) < 1)
    throw Error (FailedSys, "BasebandArchive::backend_load",
		 "fread baseband_header");

  if (bhdr.endian == BASEBAND_OPPOSITE_ENDIAN) {
    if (verbose == 3)
      cerr << "BasebandArchive::backend_load opposite_endian" << endl;
    convert_hdr_Endian();
    swap_endian = true;
  }

  if (bhdr.endian != BASEBAND_HEADER_ENDIAN)
    throw Error (InvalidState, "BasebandArchive::backend_load",
		 "invalid endian code: %x", bhdr.endian);

  /* Unfortunately, the memory map of the baseband_header struct was not
     carefully considered during its design, and it changes depending on
     if the system aligns double variables on 8-byte boundaries.

     Although the structure was fixed in Version 6, it still remains to 
     correct all data written before Version 6.  */

  if ( bhdr.version < 6 && 
       (bhdr.time_domain & ~TUsedVersion5 ||
	bhdr.frequency_domain & ~FUsedVersion5) ) 
    fix_header_memory_alignment ();

#if 0

  cerr << "power_normalization=" << bhdr.power_normalization << endl;
  cerr << "f_resolution=" <<  bhdr.f_resolution << endl;
  cerr << "t_resolution=" << bhdr.t_resolution << endl;
  cerr << "time_domain=" << bhdr.time_domain << endl;
  cerr << "frequency_domain=" << bhdr.frequency_domain << endl;
  cerr << "mean_power_cutoff=" << bhdr.mean_power_cutoff << endl;
  cerr << "hanning_smoothing=" << bhdr.hanning_smoothing << endl;
  cerr << "nsmear_neg=" << bhdr.nsmear_neg << endl;
  cerr << "nchan=" << bhdr.nchan << endl;

#endif

  int header_size_diff = 0;

  if (bhdr.version != BASEBAND_HEADER_VERSION)
    header_size_diff = BASEBAND_HEADER_SIZE - header_sizes[bhdr.version];

  if (header_size_diff) {
    // correct the FILE* for differing Version header size
    if (fseek(fptr, -header_size_diff, SEEK_CUR) < 0)
      throw Error (FailedSys, "BasebandArchive::backend_load", 
		   "fseek(%ld)\n", -header_size_diff);
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Version corrections performed here must be done in incremental order
  //
  // ////////////////////////////////////////////////////////////////////////

  bool swap_passband = false;

  if (verbose == 3) cerr << "Pulsar::BasebandArchive::backend_load version "
			 << bhdr.version << endl;

  if (bhdr.version < 1) {
    bhdr.mean_power_cutoff = 0.0;

    if (hdr.nsub_band == 1 && bhdr.voltage_state == 2)
      swap_passband = true;

    bhdr.version = 1;
  }

  if (bhdr.version < 2) {
    if (bhdr.t_resolution == 2)
      set_hanning_smoothing_factor (2);
    else
      bhdr.hanning_smoothing = 0;
    bhdr.version = 2;
  }

  if (bhdr.version < 3) {
    bhdr.pcalid[0] = '\0';
    bhdr.version = 4;
  }

  if (bhdr.version < 5) {
    bhdr.nsmear_pos /= 2;
    bhdr.nsmear_neg = bhdr.nsmear_pos;

    bhdr.nchan = hdr.nsub_band;

    bhdr.version = 5;
  }

  // memory alignment corrected in fix_header_memory_alignment
  bhdr.version = 6;

  // ////////////////////////////////////////////////////////////////////////
  // end Version corrections
  // ////////////////////////////////////////////////////////////////////////

  // temporary holding space
  vector<float> temp;

  if (bhdr.ppweight) {

    if (verbose == 3) cerr << "BasebandArchive::backend_load " 
		      << bhdr.analog_channels << " histograms.\n";

    Reference::To<TwoBitStats> twobit = new TwoBitStats;
    twobit->resize (bhdr.ppweight, bhdr.analog_channels);

    for (int idc=0; idc<bhdr.analog_channels; idc++) {

      if (fread_compressed (fptr, &temp, swap_endian) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_load",
		     "fread_compressed histogram[%d]", idc);

      twobit->set_histogram (temp, idc);
    }

    add_extension (twobit);
  }

  if (bhdr.pband_resolution) {

    if (verbose == 3) 
      cerr << "BasebandArchive::backend_load "
           << bhdr.pband_channels << " bandpasses with " 
           << bhdr.pband_resolution << " channels\n";

    Reference::To<Passband> passband = new Passband;
    passband->resize (bhdr.pband_resolution, bhdr.pband_channels);

    for (int ipb=0; ipb<bhdr.pband_channels; ipb++) {

      if (fread_compressed (fptr, &temp, swap_endian) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_load",
		     "fread_compressed passband[%d]", ipb);

      if (swap_passband) {
	unsigned nswap = bhdr.pband_resolution / 2;
	for (unsigned ipt=0; ipt<nswap; ipt++)
	  swap (temp[ipt], temp[nswap+ipt]);
      }

      passband->set_passband (temp, ipb);
      
    }

    add_extension (passband);
  }
  
  long file_end = ftell (fptr);

  if ((file_end - file_start) != (long)hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_load",
		 "loaded %d bytes != %d bytes",
		 file_end - file_start, hdr.be_data_size);

  if (uint64_t_bug) {
    hdr.be_data_size += (bhdr.pband_channels + bhdr.analog_channels) * 4;
    bhdr.size = hdr.be_data_size;
  }

  // correct the timer header in preparation for unloading this archive
  bhdr.size = hdr.be_data_size += header_size_diff;

  set_reduction ();
}

void Pulsar::BasebandArchive::backend_unload (FILE* fptr) const
{
  const_cast<BasebandArchive*>(this)->check_be_data_size ();

  if (verbose == 3) cerr << "BasebandArchive::backend_unload header size=" 
                    << bhdr.size << endl;

  if (bhdr.size != hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "invalid header size (timer:%d, bhdr:%d)\n", 
		 hdr.be_data_size, bhdr.size);
  
  long file_start = ftell (fptr);

  if (fwrite (&bhdr, sizeof(baseband_header), 1, fptr) < 1)
    throw Error (FailedSys, "BasebandArchive::backend_unload",
		 "fwrite baseband_header");

  long file_end = ftell (fptr);

  if (file_end - file_start != BASEBAND_HEADER_SIZE)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "unloaded %d bytes of BASEBAND header (size=%d)\n",
		 file_end - file_start, BASEBAND_HEADER_SIZE);


  if (bhdr.ppweight) {

    const TwoBitStats* twobit = get<TwoBitStats>();
    if (!twobit)
      throw Error (InvalidState, "BasebandArchive::backend_unload",
		   "no TwoBitStats Extension");

    if (verbose == 3) cerr << "BasebandArchive::backend_unload " 
                      << bhdr.analog_channels << " 2-bit histograms" << endl;
         
    for (int idc=0; idc<bhdr.analog_channels; idc++)
      if (fwrite_compressed (fptr, twobit->get_histogram(idc)) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_unload",
		     "fwrite_compressed histogram[%d]", idc);

  }

  if (bhdr.pband_resolution) {

    const Passband* passband = get<Passband>();
    if (!passband) 
      throw Error (InvalidState, "BasebandArchive::backend_unload",
		   "no Passband Extension");

    if (verbose == 3) cerr << "BasebandArchive::backend_unload " 
                      << bhdr.pband_channels << " bandpasses with "
                      << bhdr.pband_resolution << " channels" << endl;

    vector<float> temp (bhdr.pband_resolution);

    for (int ipb=0; ipb<bhdr.pband_channels; ipb++)  {

      unsigned itemp = 0;

      for (unsigned iband=0; iband<passband->get_nband(); iband++)  {
        const vector<float>& pb = passband->get_passband (ipb, iband);
        for (unsigned ichan=0; ichan < pb.size(); ichan++) {
          temp[itemp] = pb[ichan];  itemp++;
        }
      }

      if (itemp != unsigned(bhdr.pband_resolution))
        throw Error (InvalidState, "BasebandArchive::backend_unload",
                     "passband nband=%d*nchan=%d != resolution=%d",
                     passband->get_nband(), passband->get_nband(),
                     bhdr.pband_resolution);

      if (fwrite_compressed (fptr, temp) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_unload",
		     "fwrite_compressed passband[%d]", ipb);
    }

  }

  file_end = ftell (fptr);

  if ((file_end - file_start) != (long) hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "unloaded %d bytes != %d bytes\n",
		 file_end - file_start, hdr.be_data_size);

  if (verbose == 3)
    cerr << "BasebandArchive::backend_unload exit" << endl;
}

const vector<float>& 
Pulsar::BasebandArchive::get_passband (unsigned channel) const
{
  const Passband* passband = get<Passband>();

  if (!passband)
    throw Error (InvalidState, "Pulsar::BasebandArchive::get_histogram",
		 "no Passband Extension");

  return passband->get_passband (channel);
}

const vector<float>& Pulsar::BasebandArchive::get_histogram (unsigned channel) const
{
  const TwoBitStats* twobit = get<TwoBitStats>();

  if (!twobit)
    throw Error (InvalidState, "Pulsar::BasebandArchive::get_histogram",
		 "no TwoBitStats Extension");

  return twobit->get_histogram (channel);
}

bool Pulsar::BasebandArchive::get_scattered_power_corrected () const {
  return bhdr.frequency_domain & ScatteredPower;
}

bool Pulsar::BasebandArchive::get_coherent_calibration () const {
  return bhdr.frequency_domain & CorInstDepol;
}

void Pulsar::BasebandArchive::set_hanning_smoothing_factor (int factor)
{
  bhdr.hanning_smoothing = factor;    // Version 2 addition
  if (factor < 2)
    bhdr.time_domain &= ~DetectHanning;
  else
    bhdr.time_domain |= DetectHanning;
}

int Pulsar::BasebandArchive::get_hanning_smoothing_factor () const
{
  if ((bhdr.time_domain & DetectHanning) == 0)
    return 0;
  else
    return bhdr.hanning_smoothing;
}

float Pulsar::BasebandArchive::get_tolerance () const
{
  return bhdr.mean_power_cutoff;
}

const char* Pulsar::BasebandArchive::get_apodizing_name () const
{
  if (bhdr.time_domain & TimeHanning)
    return "Hanning";
  else if (bhdr.time_domain & TimeParzen)
    return "Parzen";
  else if (bhdr.time_domain & TimeWelch)
    return "Welch";
  else
    return "None";
}
