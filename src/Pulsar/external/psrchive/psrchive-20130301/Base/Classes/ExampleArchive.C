/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ExampleArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

using namespace std;

void Pulsar::ExampleArchive::init ()
{
  // initialize the ExampleArchive attributes
  // These are any "extra" attributes that have been added to the
  // ExampleArchive class, but don't exist in BasicArchive.  Typically
  // this would be some info specific to this file format.
}

Pulsar::ExampleArchive::ExampleArchive()
{
  init ();
}

Pulsar::ExampleArchive::~ExampleArchive()
{
  // destroy any ExampleArchive-specific resources
}

Pulsar::ExampleArchive::ExampleArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ExampleArchive::ExampleArchive (const ExampleArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive construct copy ExampleArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::ExampleArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive::copy dynamic cast call" << endl;
  
  const ExampleArchive* like_me = dynamic_cast<const ExampleArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive::copy another ExampleArchive" << endl;

  // copy ExampleArchive attributes
}

Pulsar::ExampleArchive* Pulsar::ExampleArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::ExampleArchive::clone" << endl;
  return new ExampleArchive (*this);
}

void Pulsar::ExampleArchive::load_header (const char* filename)
{
  // load all BasicArchive and ExampleArchive attributes from filename

  // This function should first determine if the file in question
  // actually is of the expected format.  If not, throw a error:
  int file_is_example_archive=0;
  if (!file_is_example_archive) 
    throw Error (InvalidState, "Pulsar::ExampleArchive::load_header",
        "File (%s) is not correct type", filename);

  // Next, we need to set the following class attributes using data
  // from the file.  Load these from your file now!  Here we just show
  // examples with dummy values.

  // Number of polarizations, channels, and subintegrations in file.
  set_npol(4);
  set_nchan(16);
  set_nsubint(1);

  // Number of bins per pulse period in folded profiles.
  set_nbin(2048);

  // Polarization state, see Util/genutil/Types.h for allowed values
  set_state(Signal::Coherence);

  // Data scale, values also defined in Types.h
  set_scale(Signal::FluxDensity);

  // Source name
  set_source("B1937+21");

  // TEMPO telescope site code
  set_telescope("3");

  // Observation type (PSR or CAL), again see Types.h
  set_type(Signal::Pulsar);

  // Total bandwidth, MHz. Negative value denotes reversed band.
  set_bandwidth(-64.0);

  // Center frequency, MHz.  Note correct spelling of "center" ;)
  set_centre_frequency(1410.0);

  // Dispersion measure, pc/cm^3.
  set_dispersion_measure(71.025);

  // Flags telling whether the data have had any of these operations
  // done to it already.  "dedispersed" refers to the inter-channel
  // dispersion delay, not in-channel coherent dedisp.
  set_dedispersed(false); 
  set_faraday_corrected(false);
  set_poln_calibrated(false);
}

Pulsar::Integration*
Pulsar::ExampleArchive::load_Integration (const char* filename, unsigned subint)
{
  // load all BasicIntegration attributes and data from filename.
  // This functions needs to read information and data values
  // for all channels/pols of a single subintegration in
  // the file.  We can assume that load_header has already been run
  // on this file, so nbin, nchan, npol, etc info is valid.

  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by 
  // load_header.
  resize_Integration(integration);

  // Load the following date and time values for the specified
  // subint.  See Util/genutil/MJD.h for more ways to set MJD values.
  // Subint epoch needs to correspond to bin 0 of the folded profile.
  int my_mjd=51544, my_seconds=5678;
  double my_secfrac=0.1234;
  MJD epoch(my_mjd, my_seconds, my_secfrac);
  integration->set_epoch(epoch);
  integration->set_duration(180.0); // Integration time, s.
  integration->set_folding_period(1.55e-3); // Pulsar period, s.

  // Set RFs for each channel, MHz.
  for (unsigned ichan=0; ichan<nchan; ichan++)
    integration->set_centre_frequency(ichan, 1442.0-(double)ichan*4.0);

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) return integration;

  // Load the actual data for each pol, channel from the file.
  float *data = new float[nbin]; // Temporary storage space
  for (unsigned ipol=0; ipol<npol; ipol++) {
    for (unsigned ichan=0; ichan<nchan; ichan++) {
      // Load data for ipol, ichan into data array here.
      // Put data in integration structure:
      integration->get_Profile(ipol,ichan)->set_amps(data);
    }
  }

  // Unallocate temp space
  delete [] data;

  return integration;
}

void Pulsar::ExampleArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ExampleArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // This does not need to be filled in if you never need to save files
  // in this data format.  Assuming this is the case, it's best to throw 
  // an error here:
  throw Error (InvalidState, "Pulsar::ExampleArchive::unload_file",
      "unload not implemented for ExampleArchive");

}

string Pulsar::ExampleArchive::Agent::get_description () 
{
  return "Example Archive Version 1.0";
}

bool Pulsar::ExampleArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by Example Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    ExampleArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    return false;
  }

  return false;
}

