/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/psrchive.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/StandardCandles.h"

#include "Pulsar/FixFluxCal.h"
#include "Pulsar/CalSource.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/OffPulseCalibrator.h"

#include "Pulsar/Database.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Config.h"

#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>
#include <iostream>
#include <algorithm>

using namespace std;

// class name of the processed calibrator archive
static string archive_class = "PSRFITS";
static string output_ext = "fluxcal";
static string unload_path;

static double interval = 5.0 * 60.0;
static Pulsar::Database* database = 0;

void usage ()
{
  cout << endl <<
    "fluxcal - produces flux calibrator solutions from sets of observations\n"
    "\n"
    "fluxcal [options] filename[s]\n"
    "options:\n"
    "  -a class     Pulsar::Archive class used to represent output\n"
    "  -c file.cfg  name of file containing standard candle information\n"
    "  -B           fix the off-pulse baseline statistics \n"
    "  -C           calibrate flux calibrator observation with itself \n"
    "  -d database  get FluxCal archives from database and file solutions\n"
    "  -e extension filename extension added to output archives\n"
    "  -O path      directory in which output will be written \n"
    "  -f           fix the type and name attributes, based on coordinates\n"
    "  -i minutes   maximum number of minutes between archives in same set\n"
    "  -I freq_mhz  Print all cal sources fluxes at the given frequency\n"
    "\n"
    "By default, standard candle information is read from \n" 
       << Pulsar::StandardCandles::default_filename << "\n"
    "and the maximum interval between archives in the same\n"
    "flux calibrator set is " << interval/60 << " minutes.\n"
    "\n"
    "See " PSRCHIVE_HTTP "/manuals/fluxcal for more details\n"
       << endl;
}

static bool verbose = false;

void unload (Pulsar::FluxCalibrator* fluxcal) try {

  if (!fluxcal->complete()) 
  {
    cerr << "fluxcal: unload called on incomplete data set, skipping" << endl;
    return;
  }

  Reference::To<Pulsar::Archive> archive;
  cerr << "fluxcal: creating " << archive_class << " Archive" << endl;
  archive = fluxcal->new_solution (archive_class);

  if (fluxcal->meanTsys() == 0.0)
  {
    cerr << "fluxcal: unload called with mean Tsys==0, skipping" << endl;
    return;
  }

  if (verbose)
    cerr << endl
	 << "fluxcal: standard candle " 
	 << fluxcal->get_standard_candle_info()
	 << "\nfluxcal: mean system equivalent flux density = "
	 << tostring(fluxcal->meanTsys()/1e3,4) << " Jy" << endl
	 << endl;

  string newname = fluxcal->get_filenames ();
  const char* whitespace = " ,\t\n";
  newname = replace_extension( stringtok (newname, whitespace), output_ext );

  if (!unload_path.empty())
    newname = unload_path + "/" + basename (newname);

  cerr << "fluxcal: unloading " << newname << endl;
  archive -> unload (newname);

  if (database) {
    cerr << "fluxcal: adding new entry to database" << endl;
    database->add (archive);
  }

}
catch (Error& error) {
  cerr << "fluxcal: error unloading solution\n\t"
       << error.get_message() << endl;
}

// print configuration information to cerr
void configuration_report (Reference::To<Pulsar::StandardCandles>);

// print all fluxes to cerr
void print_fluxes (Reference::To<Pulsar::StandardCandles>, double freq);

int main (int argc, char** argv) try {

  Pulsar::Option<bool> self_calibrate ("fluxcal::self_calibrate", false);

  bool offpulse_calibrator = false;

  Reference::To<Pulsar::StandardCandles> standards;

  Reference::To<Pulsar::FixFluxCal> fix;

  string database_filename;

  bool print_flux = false;
  double print_ref_freq = 0.0;

  char c;
  while ((c = getopt(argc, argv, "hqvVa:BCc:d:e:fi:I:O:")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      verbose = true;
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'a':
      archive_class = optarg;
      cerr << "fluxcal: will write to " << archive_class << " files" << endl;
      break;

    case 'B':
      offpulse_calibrator = true;
      break;

    case 'C':
      self_calibrate = true;
      break;

    case 'c':
      standards = new Pulsar::StandardCandles (optarg);
      cerr << "fluxcal: standard candles loaded from " << optarg << endl;
      break;

    case 'd':
      database = new Pulsar::Database (optarg);
      cerr << "fluxcal: database loaded from " << optarg << endl;
      database_filename = optarg;
      break;

    case 'e':
      output_ext = optarg;
      cerr << "fluxcal: output file extension: " << output_ext << endl;
      break;

    case 'f':
      fix = new Pulsar::FixFluxCal;
      break;

    case 'i':
      interval = atof(optarg) * 60.0;
      cerr << "fluxcal: maximum interval between archives: " << interval/60
	   << " minutes" << endl;
      break;

    case 'I':
      print_flux = true;
      print_ref_freq = atof(optarg);
      break;

    case 'O':
      unload_path = optarg;
      break;

    default:
      cerr << "fluxcal: invalid command line option: -" << c << endl;
      break;
    } 

  if (verbose)
    configuration_report (standards);

  if (print_flux)
    print_fluxes(standards, print_ref_freq);

  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0 && !database) {
    if (print_flux)
      return 0;
    else {
      cerr << "fluxcal: please specify filename[s]" << endl;
      return -1;
    }
  }

  sort (filenames.begin(), filenames.end());

  if (database) {

    if (filenames.size() != 0) {
      cerr << "fluxcal: do not specify filename[s] with -d option" << endl;
      return -1;
    }

    // get all of the fluxcal on/off observations and sort them
    Pulsar::Database::Criterion criterion;
    vector<Pulsar::Database::Entry> entries;

    criterion = database->criterion(Pulsar::Database::any, Signal::FluxCalOn);
    database->all_matching (criterion, entries);

    criterion = database->criterion(Pulsar::Database::any, Signal::FluxCalOff);
    database->all_matching (criterion, entries);

    sort (entries.begin(), entries.end());

    if (!entries.size()) {
      cerr << "fluxcal: no FluxCalOn|Off observations in database" << endl;
      return -1;
    }

    // break them into sets

    // check for solutions (requires set identification)

    // add to filenames
    filenames.resize( entries.size() );
    for (unsigned ifile=0; ifile < filenames.size(); ifile++)
      filenames[ifile] = database->get_filename (entries[ifile]);

  }

  Reference::To<Pulsar::Archive> last;
  Reference::To<Pulsar::Archive> archive;
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    cerr << "fluxcal: loading " << filenames[ifile] << endl;
    
    archive = Pulsar::Archive::load(filenames[ifile]);

    if (fix) {

      fix->apply (archive);

      if (verbose && fix->get_changes() != "none")
	cerr << "fluxcal: type fixed to " << fix->get_changes() << endl;

    }

    if (self_calibrate) {

      Reference::To<Pulsar::PolnCalibrator> pcal;

      if (offpulse_calibrator)
	pcal = new Pulsar::OffPulseCalibrator (archive);
      else
	pcal = new Pulsar::SingleAxisCalibrator (archive);

      pcal->calibrate (archive);

    }

    if (fluxcal) {

      double gap = (archive->start_time() - last->end_time()).in_seconds();

      if (gap > interval) {
	cerr << "fluxcal: gap=" << time_string(gap)
	     << " > interval=" << time_string(interval) << endl;
        unload (fluxcal);
        fluxcal = 0;
      }

    }

    if (verbose)
      cerr << "fluxcal: processing " << archive->get_type() 
	   << " observation" << endl;

    if (fluxcal) {

      if (verbose)
        cerr << "fluxcal: adding observation to FluxCalibrator" << endl;

      try {
        fluxcal->add_observation (archive);
        cerr << "fluxcal: observation added to FluxCalibrator" << endl;
      }
      catch (Error& error) {
        cerr << "fluxcal: failed to add observation\n\t" 
             << error.get_message() << endl;
        unload (fluxcal);
        fluxcal = 0;
      }

    }
    
    if (!fluxcal) {

      cerr << "fluxcal: starting new FluxCalibrator" << endl;
      fluxcal = new Pulsar::FluxCalibrator (archive);
      if (standards)
	fluxcal->set_database (standards);

    }

    last = archive;

  }
  catch (Error& error) {
    cerr << "fluxcal: error handling " << filenames[ifile] << endl
	 << error.get_message() << endl;
  }

  if (fluxcal)
    unload (fluxcal);

  if (database) {

    string backup = database_filename + ".bkp";
    cerr << "fluxcal: backing up old database" << endl;
    rename (database_filename.c_str(), backup.c_str());
    
    cerr << "fluxcal: writing new database" << endl;
    database->unload (database_filename);

  }

  return 0;
}
catch (Error& error) {
  cerr << "fluxcal: error" << error << endl;
  return -1;
}

void configuration_report (Reference::To<Pulsar::StandardCandles> standards)
{
  if (!standards)
    standards = new Pulsar::StandardCandles;

  cerr << endl
       << "fluxcal: " << standards->size() 
       << " standard candles loaded from\n  "
       << standards->get_filename() << endl;

  Reference::To<Pulsar::CalSource> correct = new Pulsar::CalSource;

  cerr << "fluxcal: " << correct->get_on_size()
       << " FluxCal-On aliases loaded from\n  "
       << correct->get_on_filename () << endl;

  cerr << "fluxcal: " << correct->get_off_size()
       << " FluxCal-Off aliases loaded from\n  "
       << correct->get_off_filename () << endl
       << endl;
}

void print_fluxes (Reference::To<Pulsar::StandardCandles> standards, 
    double freq)
{
  if (!standards)
    standards = new Pulsar::StandardCandles;

  cerr << endl
    << "fluxcal: using file " << standards->get_filename() 
    << endl;
  cerr << "  Cal source fluxes at " << freq << " MHz:"
    << endl;

  for (unsigned i=0; i<standards->size(); i++) 
    cerr << "  " << setw(11) << standards->get_entry(i).source_name[0]
      << ":  " << standards->get_entry(i).get_flux_mJy(freq)/1000. 
      << " Jy" << endl;

  cerr << endl;
}

