/***************************************************************************
 *
 *   Copyright (C) 2011 by Jonathan Khoo
 *   Licensed under the Academic Free License version 3.0
 *
 ***************************************************************************/

#include <vector>

#include "Error.h"
#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

//! Calibration file (Pulsar Archive) averaging application.
class calavg : public Pulsar::Application
{
  public:

    //! Default constructor
    calavg();

    //! Process the given archive: extracts the S_sys and S_cal values
    //  from the FLUXCAL table
    void process(Pulsar::Archive*);

    //! Calculates the weighted average over all fluxcal files and outputs
    //  the results in a new fluxcal file
    void finalize();

  protected:

    //! Add command line options
    void add_options(CommandLine::Menu&);

    //! Name of the output file
    string unload_name;

    //! Unload options
    Reference::To<Pulsar::UnloadOptions> unload;

  private:

    // The number of polns of a fluxcal observation is always 2.
    static const unsigned NPOL = 2;

    //! Sanity-checking and verification of fluxcal parameters. The
    //  following parameters within each fluxcal file are checked so that
    //  each file (following) the first matches the first file.
    //      Parameters:
    //          - Centre frequency
    //          - Bandwidth
    //          - Receiever/frontend
    //          - Nchan

    //! Calculate the channel weight from error.
    //      weight = 1/error^2
    template<typename T> T get_weight_from_error(const T error);

    //! Enter command into the HISTORY table. Required because
    //  Pulsar::Application sets the command after average is cloned???
    void set_history_command();

    //!
    //  Extract - methods used to extract the must-be-consistent parameters.
    //

    //! Get the receiver/frontend of the fluxcal archive. Retrives the value
    //  from the Pulsar::Receiver extension.
    string get_receiver(Pulsar::Archive* archive) const;

    //! Get the centre frequency of the fluxcal.
    double get_frequency(Pulsar::Archive* archive) const;

    //! Get the bandwidth of the fluxcal.
    double get_bw(Pulsar::Archive* archive) const;

    //! Get the number of frequency channels of the fluxca.
    unsigned get_nchan(Pulsar::Archive* archive) const;

    //!
    //  Access - getters and settings to access the parameters 
    //  of the first fluxcal.
    //

    void set_average_centre_frequency(const double frequency);

    double get_average_centre_frequency() const;

    void set_average_receiver(const string receiver);

    string get_average_receiver() const;

    void set_average_bw(const double _bw);

    double get_average_bw() const;

    void set_average_nchan(const unsigned _nchan);

    unsigned get_average_nchan() const;

    //! Holds the S_sys values (S_sys and error) for all files given on the
    //  command line.
    vector<vector<vector<Estimate<double> > > > S_sys_values;

    //! Holds the S_cal values (S_cal and error) for all files given on the
    //  command line.
    vector<vector<vector< Estimate<double> > > > S_cal_values;

    //! The weighted average archive - a copy of the first fluxcal.
    Pulsar::Archive* average;

    //! Centre frequency of the first fluxcal.
    double average_centre_frequency;

    //! Receiver of the first fluxcal.
    string average_receiver;

    //! Bandwidth of the first fluxcal.
    double bw;

    //! Number of frequency chnanels of the first fluxcal.
    unsigned nchan;
};

string calavg::get_receiver(Pulsar::Archive* archive) const
{
  Reference::To<Pulsar::Receiver> ext = archive->get<Pulsar::Receiver>();

  if (ext) {
    const string receiver = ext->get_name();
    return receiver;
  }

  return "";
}

double calavg::get_frequency(Pulsar::Archive* archive) const
{
  const double centre_frequency = average->get_centre_frequency();
  return centre_frequency;
}

double calavg::get_bw(Pulsar::Archive* archive) const
{
  const double bw = archive->get_bandwidth();
  return bw;
}

unsigned calavg::get_nchan(Pulsar::Archive* archive) const
{
  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    archive->get<Pulsar::FluxCalibratorExtension>();

  if (ext) {
    const unsigned nchan = ext->get_nchan();
    return nchan;
  }

  return 0;
}

void calavg::set_average_nchan(const unsigned _nchan)
{
  nchan = _nchan;
}

unsigned calavg::get_average_nchan() const
{
  return nchan;
}

void calavg::set_average_bw(const double _bw)
{
  bw = _bw;
}

double calavg::get_average_bw() const
{
  return bw;
}

void calavg::set_average_centre_frequency(const double frequency)
{
  average_centre_frequency = frequency;
}

double calavg::get_average_centre_frequency() const
{
  return average_centre_frequency;
}

void calavg::set_average_receiver(const string receiver)
{
  average_receiver = receiver;
}

string calavg::get_average_receiver() const
{
  return average_receiver;
}

template<typename T>
T calavg::get_weight_from_error(const T error)
{
  return 1.0/pow(error, 2);
}

//! Enter command into the HISTORY table. This required because
//  Pulsar::Application sets the command after average is cloned???
void calavg::set_history_command()
{
  Reference::To<Pulsar::ProcHistory> ext =
    average->get<Pulsar::ProcHistory>();

  if (ext) {
    ext->set_command_str(command);
  }
}

//! Extract the S_sys and S_cal values from the FLUXCAL table of the given
//  archive.
void calavg::process(Pulsar::Archive* archive)
{
  if (verbose) {
    cerr << "Processing: " << archive->get_filename() << endl;
  }

  // Ensure that the loaded archive is a fluxcal.
  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    archive->get<Pulsar::FluxCalibratorExtension>();

  if (ext == NULL) {
    cerr << archive->get_filename() << " is not a fluxcal... " << endl;

    return;
  }

  // Compare parameters if not first file.
  if (average) {
    try {
      if (get_receiver(average) != get_receiver(archive)) {
        throw Error(InvalidState, "calavg::process",
            "Receivers are different (%s=%s; %s=%s)",
            average->get_filename().c_str(),
            get_receiver(average).c_str(),
            archive->get_filename().c_str(),
            get_receiver(archive).c_str());
      }
      if (get_frequency(average) != get_frequency(archive)) {
        throw Error(InvalidState, "calavg::process",
            "frequencies are different (%s=%g; %s=%g)",
            average->get_filename().c_str(),
            get_frequency(average),
            archive->get_filename().c_str(),
            get_frequency(archive));
      }
      if (get_bw(average) != get_bw(archive)) {
        throw Error(InvalidState, "calavg::process",
            "bandwidths are different (%s=%g; %s=%g)",
            average->get_filename().c_str(),
            get_bw(average),
            archive->get_filename().c_str(),
            get_bw(archive));
      }
      if (get_nchan(average) != get_nchan(archive)) {
        throw Error(InvalidState, "calavg::process",
            "nchans are different (%s=%g; %s=%g)",
            average->get_filename().c_str(),
            get_nchan(average),
            archive->get_filename().c_str(),
            get_nchan(archive));
      }
    } catch (Error& e) {
      cerr << "calavg error: " << e.get_message() << endl;
      exit(0);
    }
  }

  // Copy all S_sys values from the current fluxcal.
  S_sys_values.push_back(ext->get_S_sys());

  // Copy all S_cal values from the current fluxcal.
  S_cal_values.push_back(ext->get_S_cal());

  // If first file, extract parameters and store them for easy comparison.
  if (average == NULL) {
    if (verbose) {
      cerr << "Using " << archive->get_filename() <<
        " as a storage container." << endl;
    }

    // Retain a copy of the first cal to act as a container of the weighted
    // average fluxcal.
    average = archive->clone();

    // Store the parameters that are required to be consistent with
    // following fluxcal files.
    set_average_receiver(get_receiver(average));
    set_average_centre_frequency(get_frequency(average));
    set_average_bw(get_bw(average));
    set_average_nchan(get_nchan(average));
  }
}

//! Perform the weighted average over S_sys and S_cal values from all files.
//  Create an output file using the header information from the first fluxcal.
void calavg::finalize()
{
  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    average->get<Pulsar::FluxCalibratorExtension>();

  const unsigned nchan = ext->get_nchan();

  // Calculate the weighted average (channel by channel) for each file, channel,
  // and receptor.
  for (unsigned ipol = 0; ipol < NPOL; ++ipol) {

    // Ensure we're not going to read off the end of the vector.
    if (nchan > S_sys_values[ipol].size() || nchan > S_cal_values[ipol].size()) {
      throw Error(InvalidState, "calavg::finalize",
          "Error in extracting all frequency channels (nchan >= vector size %d)",
          S_sys_values.size());
    }

    for (unsigned ichan = 0; ichan < nchan; ++ichan) {
      // Iterate over the S_sys and S_sys_err values of each file and calculate
      // the average mean (for the current channel and receptor).
      MeanEstimate<double, double> total_S_sys; // Can I get away with this...?
      MeanEstimate<double, double> total_S_cal;

      // Perform the weighted average across each file.
      for (unsigned ifile = 0; ifile < S_sys_values.size(); ++ifile) {
        total_S_sys += S_sys_values[ifile][ichan][ipol];
        total_S_cal += S_cal_values[ifile][ichan][ipol];
      }

      ext->set_S_sys(ichan, ipol, total_S_sys);
      ext->set_S_cal(ichan, ipol, total_S_cal);
    }
  }

  // Calculate the channel weight by summing the channel weights (calculated
  // from the error) across both receptors.
  for (unsigned ichan = 0; ichan < nchan; ++ichan) {
    MeanEstimate<double, double> total_S_cal; // ... and get away with this...?
    for (unsigned ipol = 0; ipol < NPOL; ++ipol) {
      total_S_cal += ext->get_S_cal(ichan, ipol);
    }

    // Calculate channel weight from error.
    const double error = total_S_cal.get_Estimate().get_error();
    const double weight = get_weight_from_error(error); // UNCHECKED
    ext->set_weight(ichan, weight); // CHECKED
  }

  if (unload_name.empty()) {
    unload_name = unload->get_output_filename(average);
  }

  cerr << "calavg: Unloading archive: '" << unload_name << "'" << endl;

  set_history_command();
  average->unload(unload_name);
}

void calavg::add_options(CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add(unload_name, 'o', "fname");
  arg->set_help("output result to 'fname'");
}

static calavg program;

int main(int argc, char *argv[])
{
  return program.main(argc, argv);
}

calavg::calavg() :
  Pulsar::Application("calavg", "averages cal files"),
  average(NULL)
{
  update_history = true;

  add(new Pulsar::StandardOptions);

  unload = new Pulsar::UnloadOptions;
  unload->set_output_each(false);
  unload->set_extension("avg");
  add(unload);
}

