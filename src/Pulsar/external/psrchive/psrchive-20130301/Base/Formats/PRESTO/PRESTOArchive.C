/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PRESTOArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"
#include "FTransform.h"
#include "polyco.h"
#include "Warning.h"

#include "machine_endian.h"
#include "prepfold.h"

using namespace std;

static Warning warn;

void Pulsar::PRESTOArchive::init ()
{
  // initialize the PRESTOArchive attributes
  header_size=0;
  endian_swap=0;
  polyco_load_failed=false;
}

Pulsar::PRESTOArchive::PRESTOArchive()
{
  init ();
}

Pulsar::PRESTOArchive::~PRESTOArchive()
{
  // destroy any PRESTOArchive-specific resources
}

Pulsar::PRESTOArchive::PRESTOArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::PRESTOArchive::PRESTOArchive (const PRESTOArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive construct copy PRESTOArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::PRESTOArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive::copy dynamic cast call" << endl;
  
  const PRESTOArchive* like_me = dynamic_cast<const PRESTOArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive::copy another PRESTOArchive" << endl;

  // copy PRESTOArchive attributes
}

Pulsar::PRESTOArchive* Pulsar::PRESTOArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::PRESTOArchive::clone" << endl;
  return new PRESTOArchive (*this);
}

int Pulsar::PRESTOArchive::read_string(char **out, FILE *f) 
{
  // Strings stored in the files have a int length followed by the
  // string (no null-term).
  int len=0;
  const int max_len=256; // XXX is this enough?
  int rv = fread(&len, sizeof(int), 1, f);
  if (rv!=1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::read_string",
        "fread(int)");
  }
  if ((len<0) || (len>max_len)) {
    fclose(f);
    throw Error (InvalidParam, "Pulsar::PRESTOArchive::read_string",
        "Bad string len (%d)", len);
  }
  *out = new char[len+1];
  if (len>0) { rv = fread(*out, sizeof(char), len, f); }
  (*out)[len]='\0';
  return len;
}

void Pulsar::PRESTOArchive::read_position(position *out, FILE *f) 
{
  int rv;
  float jnk;
  rv = fread(&out->pow, sizeof(float), 1, f);
  rv += fread(&jnk, sizeof(float), 1, f);
  rv += fread(&out->p1, sizeof(double), 1, f);
  rv += fread(&out->p2, sizeof(double), 1, f);
  rv += fread(&out->p3, sizeof(double), 1, f);
  if (rv != 5) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::read_position",
        "fread");
  }
}

void Pulsar::PRESTOArchive::change_header_endian()
{
  ChangeEndian(pfd.numdms);
  ChangeEndian(pfd.numperiods);
  ChangeEndian(pfd.numpdots);
  ChangeEndian(pfd.nsub);
  ChangeEndian(pfd.npart);
  ChangeEndian(pfd.proflen);
  ChangeEndian(pfd.numchan);
  ChangeEndian(pfd.pstep);
  ChangeEndian(pfd.dmstep);
  ChangeEndian(pfd.ndmfact);
  ChangeEndian(pfd.npfact);
  ChangeEndian(pfd.npfact);

  ChangeEndian(pfd.dt);
  ChangeEndian(pfd.startT);
  ChangeEndian(pfd.endT);
  ChangeEndian(pfd.tepoch);
  ChangeEndian(pfd.bepoch);
  ChangeEndian(pfd.avgvoverc);
  ChangeEndian(pfd.lofreq);
  ChangeEndian(pfd.chan_wid);
  ChangeEndian(pfd.bestdm);

  ChangeEndian(pfd.topo.pow);
  ChangeEndian(pfd.topo.p1);
  ChangeEndian(pfd.topo.p2);
  ChangeEndian(pfd.topo.p3);

  ChangeEndian(pfd.bary.pow);
  ChangeEndian(pfd.bary.p1);
  ChangeEndian(pfd.bary.p2);
  ChangeEndian(pfd.bary.p3);

  ChangeEndian(pfd.fold.pow);
  ChangeEndian(pfd.fold.p1);
  ChangeEndian(pfd.fold.p2);
  ChangeEndian(pfd.fold.p3);
}

void Pulsar::PRESTOArchive::change_foldstats_endian(foldstats *f)
{
  ChangeEndian(f->numdata);
  ChangeEndian(f->data_avg);
  ChangeEndian(f->data_var);
  ChangeEndian(f->numprof);
  ChangeEndian(f->prof_avg);
  ChangeEndian(f->prof_var);
  ChangeEndian(f->redchi);
}

#define param_test(test) \
  if (test) { whynot = #test; return(1); }

int Pulsar::PRESTOArchive::test_param_range(std::string &whynot)
{
  param_test(pfd.numdms<0);
  param_test(pfd.numperiods<0);
  param_test(pfd.numpdots<0);
  param_test(pfd.nsub<0);
  param_test(pfd.npart<0);
  param_test(pfd.proflen<0);
  param_test(pfd.numchan<0);

  param_test(pfd.tepoch<39000.0);
  param_test(pfd.tepoch>65000.0); // Y2036 noncompliant

  return(0);
}

int Pulsar::PRESTOArchive::is_spigot()
{
  // Try to guess whether or not we're observing with the Spigot.
  if (get_telescope()!="GBT") return 0;
  if (pfd.dt!=8.192e-05) return 0;
  if (pfd.chan_wid==800.0/1024. || pfd.chan_wid==800.0/2048.)
    return -1;
  if (pfd.chan_wid==50.0/1024. || pfd.chan_wid==50.0/2048.)
    return 1;
  return 0;
}

void Pulsar::PRESTOArchive::load_header (const char* filename)
{
  // load all BasicArchive and PRESTOArchive attributes from filename

  // Open file
  FILE *f = fopen(filename, "r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fopen(%s)", filename);
    
  // Starts with 12 ints beginning at numdms.
  int rv = fread(&pfd.numdms, sizeof(int), 12, f);
  if (rv!=12) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fread");
  }

  // Read several strings
  read_string(&pfd.filenm, f);
  read_string(&pfd.candnm, f);
  read_string(&pfd.telescope, f);
  read_string(&pfd.pgdev, f);

  // TODO : position strings may not exist?  Need to deal w/ that case.
  // This reads both strings at once.
  rv = fread(pfd.rastr, sizeof(char), 32, f);
  if (rv!=32) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fread");
  }

  // Some doubles are next
  rv = fread(&pfd.dt, sizeof(double), 9, f);
  if (rv!=9) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fread");
  }

  // Various period info
  read_position(&pfd.topo, f);
  read_position(&pfd.bary, f);
  read_position(&pfd.fold, f);

  // TODO: period, orbital params, etc?  Need to at least seek past them.
  // see header_size addition below

  // Get header size
  header_size = ftell(f);
  if (header_size==-1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "ftell");
  }

  // Check that various parameters are in correct range.  If not,
  // try changing endianness.  If that fails, it's probably not 
  // a pfd file.
  string s1, s2;
  if (test_param_range(s1)) {
    change_header_endian();
    if (test_param_range(s2)) {
      fclose(f);
      throw Error (InvalidParam, "Pulsar::PRESTOArchive::load_header",
          "Param out of range: " + s1 + ", " + s2);
    } else {
      endian_swap = 1;
    }
  }
  
  // Account for items not yet read from header
  header_size += sizeof(double)*7 // orbital params
      + sizeof(double)*pfd.numdms  // various arrays
      + sizeof(double)*pfd.numperiods
      + sizeof(double)*pfd.numpdots;

  // Read foldstats
  rv = fseek(f, header_size 
      + sizeof(double)*pfd.nsub*pfd.npart*pfd.proflen, SEEK_SET);
  if (rv==-1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fseek");
  }
  pfd.stats = new foldstats[pfd.nsub*pfd.npart];
  rv = fread(pfd.stats, sizeof(foldstats), pfd.nsub*pfd.npart, f);
  if (rv!=pfd.nsub*pfd.npart) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_header",
        "fread");
  }
  if (endian_swap) {
    for (int i=0; i<pfd.nsub*pfd.npart; i++) {
      change_foldstats_endian(&pfd.stats[i]);
    }
  }
  
  // Done w/ file
  fclose(f);

  // Set Archive attributes

  // General dims
  set_npol(1);  // Always 1 for presto
  set_nchan(pfd.nsub);
  set_nsubint(pfd.npart);

  // Number of bins per pulse period in folded profiles.
  set_nbin(pfd.proflen);

  // Polarization state, see Util/genutil/Types.h for allowed values
  set_state(Signal::Intensity);

  // Data scale, values also defined in Types.h
  set_scale(Signal::FluxDensity);

  // Source name
  set_source(pfd.candnm);

  // TEMPO telescope site code
  set_telescope(pfd.telescope); // code or name?

  // Observation type (PSR or CAL), again see Types.h
  set_type(Signal::Pulsar); 

  // Total bandwidth, MHz. Negative value denotes reversed band.
  set_bandwidth(pfd.chan_wid*pfd.numchan);

  // Center frequency, MHz.
  set_centre_frequency(pfd.lofreq + ((double)pfd.numchan/2-0.5)*pfd.chan_wid);

  // Dispersion measure, pc/cm^3.
  set_dispersion_measure(pfd.bestdm);

  // Flags telling whether the data have had any of these operations
  // done to it already.  "dedispersed" refers to the inter-channel
  // dispersion delay, not in-channel coherent dedisp.
  set_dedispersed(false); 
  set_faraday_corrected(false);
  set_poln_calibrated(false);
}

Pulsar::Integration*
Pulsar::PRESTOArchive::load_Integration (const char* filename, unsigned subint)
{
  
  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by 
  // load_header.
  resize_Integration(integration);

  // Read polyco file if it hasn't been read already
  if (!has_model() && !polyco_load_failed) try {
    model = new polyco(string(filename) + ".polycos");
  }
  catch (Error &e) {
    cerr << "PRESTOArchive::load_Integration WARNING:  Polyco load failed. " 
      << "These results will NOT be accurate for timing!"  << e << endl;
    polyco_load_failed = true;
    model = NULL;
  }

  // Subint epoch needs to correspond to bin 0 of the folded profile.
  // Count samples to get offset to current subint.
  // Prepfold puts first sample in bin 0.
  double nsamp=0.0;
  for (unsigned isub=0; isub<subint; isub++) {
    nsamp += pfd.stats[isub*pfd.nsub].numdata;
  }
  nsamp += pfd.stats[subint*pfd.nsub].numdata/2; // Get to mid-subint
  MJD epoch(pfd.tepoch);
  double fs = epoch.get_fracsec();      // Round to nearest integer second
  if (fabs(fs) < 1e-3)                  // if we're within 1 ms.
    epoch -= fs;  
  else if (fabs(1.0-fs) < 1e-3) 
    epoch += 1.0 - fs;
  else  
    warn << "PRESTOArchive warning: Start time has second fraction " 
      << fs*1e3 << "ms.  Not rounding to nearest second." << endl;
  MJD epoch0 = epoch;  // XXX Do this before or after int sec correction?
  epoch += nsamp * pfd.dt;
  Phase midphase;
  double midfreq;
  if (has_model()) {                    // Correct to zero phase point
    midphase = model->phase(epoch) - model->phase(epoch0); 
    midfreq = model->frequency(epoch);
    epoch -= midphase.fracturns() / midfreq;
  }
  integration->set_epoch(epoch);

  // Integration time, s.
  integration->set_duration(pfd.dt*pfd.stats[subint*pfd.nsub].numdata);

  // Nominal (midpoint) pulse period for this subint, s.
  if (has_model()) 
    integration->set_folding_period(1.0/midfreq);
  else 
    integration->set_folding_period(1.0/pfd.fold.p1);

  // Set RFs for each channel, MHz.
  //
  // NOTE: Shift all spigot data down by half a channel, to
  // compensate for an error in converting ACFs to spectra.
  // This should also apply to WAPP data processed by presto
  // but the situation is more complex there since each WAPP can
  // have different sideband, etc.  
  // 
  // We also calculate how much we need to correct each profile
  // to get it dedispersed to the middle of the subband
  //
  // Also calculate a correction for the 'integer time sample
  // dedispersion' error.
  int chan_per_subband = pfd.numchan/pfd.nsub;
  double f0 = pfd.lofreq - 0.5*pfd.chan_wid 
    + 0.5*(double)chan_per_subband*pfd.chan_wid;
  if (is_spigot()!=0) 
    f0 += 0.5*(double)is_spigot()*pfd.chan_wid;
  double *dm_phase_shift = new double[pfd.nsub];
  for (unsigned ichan=0; ichan<nchan; ichan++) {
    double f_mid = f0 + (double)ichan * chan_per_subband * pfd.chan_wid;
    double f_high_chan = f_mid + (double)(chan_per_subband-1)*pfd.chan_wid/2.0;
    integration->set_centre_frequency(ichan, f_mid);
    double dm_eff = get_dispersion_measure() / (1.0 + pfd.avgvoverc);
    dm_phase_shift[ichan] = 
      Pulsar::dispersion_delay(dm_eff,f_mid,f_high_chan) / 
      integration->get_folding_period();

    double integer_dd_error = 0.0; // In seconds
    for (unsigned isubchan=0; isubchan<chan_per_subband; isubchan++) {
      double fdelt = 0.5*(double)is_spigot()*pfd.chan_wid;
      double fsub = f_high_chan - (double)isubchan*pfd.chan_wid;
      double subshift = Pulsar::dispersion_delay(dm_eff,
          fsub - fdelt, f_high_chan - fdelt);
      double subshift_samp = subshift / pfd.dt;
      double subshift_error = (subshift_samp - round(subshift_samp)) * pfd.dt;
      integer_dd_error += subshift_error / (double)chan_per_subband;
    }
    dm_phase_shift[ichan] -= integer_dd_error / 
      integration->get_folding_period();
    //cerr << "freq " << f_mid << " int_dd_err " << integer_dd_error*1e6 << endl;

  }

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) return integration;

  // Reopen file
  FILE *f = fopen(filename, "r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_Integration",
        "fopen");

  // Seek to correct spot
  int rv = fseek(f, header_size + subint*sizeof(double)*pfd.nsub*pfd.proflen,
      SEEK_SET);
  if (rv==-1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::PRESTOArchive::load_Integration",
        "fseek");
  }

  // Load the actual data for each pol, channel from the file.
  double *data = new double[nbin]; // Temporary storage space
  for (unsigned ichan=0; ichan<nchan; ichan++) {
    // Load data for ichan 
    rv = fread(data, sizeof(double), pfd.proflen, f);
    if (rv!=pfd.proflen) {
      fclose(f);
      throw Error (FailedSys, "Pulsar::PRESTOArchive::load_Integration",
          "fread");
    }
    // Swap if needed
    if (endian_swap) { array_changeEndian(pfd.proflen, data, sizeof(double)); }
    // Put data in integration structure:
    integration->get_Profile(0,ichan)->set_amps(data);
    // Shift to dedisperse to center freq of channel
    FTransform::shift(nbin, integration->get_Profile(0,ichan)->get_amps(),
        (double)nbin*dm_phase_shift[ichan]);
  }

  // Close file
  fclose(f);

  // Unallocate temp space
  delete [] data;
  delete [] dm_phase_shift;

  return integration;
}

void Pulsar::PRESTOArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and PRESTOArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // This does not need to be filled in if you never need to save files
  // in this data format.  Assuming this is the case, it's best to throw 
  // an error here:
  throw Error (InvalidState, "Pulsar::PRESTOArchive::unload_file",
      "unload not implemented for PRESTOArchive");

}

string Pulsar::PRESTOArchive::Agent::get_description () 
{
  return "PRESTO Prepfold (.pfd) output files";
}

bool Pulsar::PRESTOArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by PRESTO Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    PRESTOArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    if (verbose > 2)
      cerr << "PRESTO load failed due to: " << e << endl;
    return false;
  }

  return false;
}

