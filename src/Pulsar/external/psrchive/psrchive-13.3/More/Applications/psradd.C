/***************************************************************************
 *
 *   Copyright (C) 2002-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/TimeAppend.h"
#include "Pulsar/FrequencyAppend.h"
#include "Pulsar/PatchTime.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"

#include "Pulsar/Interpreter.h"

#include "load_factory.h"

#include <iostream>

using namespace std;

//! Pulsar Archive combination/integration application
class psradd: public Pulsar::Application
{
public:

  //! Default constructor
  psradd ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Unload the total
  void finalize ();

  //! Disable various sanity checks
  void force ();

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  // set the total to the archive
  void set_total (Pulsar::Archive* archive);

  // add the archive to the total
  void append (Pulsar::Archive* archive);

  //! check the gap between the end of total and start of archive
  void check_interval_between (Pulsar::Archive*);

  //! check that the total and current calibrator observations are aligned
  void check_cal_phase_diff (Pulsar::Archive*);

  //! check that the total and current archive are from the same division
  void check_division (Pulsar::Archive*);

  //! check the integration length of integrated total
  void check_integration_length ();

  //! check the S/N of the integrated total
  void check_signal_to_noise ();

  //! check the gap between the start and end of total
  void check_interval_within ();

  // the accumulated total
  Reference::To<Pulsar::Archive> total;

  // name of the output file
  string unload_name;
  
  // log the results
  bool log_results;

  // the file stream to which log messages are written
  FilePtr log_file;

  // the name of the currently open log file
  string log_filename;

  // the ephemeris to be installed
  Reference::To<Pulsar::Parameters> ephemeris;

  // Append algorithms
  Pulsar::TimeAppend time;
  Pulsar::FrequencyAppend frequency;

  // use the TimeAppend algorithm
  bool time_direction;

  // Patch algorithm
  Reference::To<Pulsar::PatchTime> patch;
  string patch_name;

  // Unload options
  Reference::To<Pulsar::UnloadOptions> unload;

private:

  // tscrunch+unload when certain limiting conditions are met
  bool auto_add;

  // reset the total to the current archive
  bool reset_total;
};

int main (int argc, char** argv)
{
  psradd program;
  return program.main (argc, argv);
}

psradd::psradd () : Pulsar::Application ("psradd", 
					 "combines archives together")
{
  version = "$Id: psradd.C,v 1.72 2010/05/03 04:46:20 straten Exp $";

  has_manual = true;
  update_history = true;
  sort_filenames = true;

  unload = new Pulsar::UnloadOptions;
  unload->set_output_each( false );
  unload->set_extension( "it" );

  add( new Pulsar::StandardOptions );
  add( unload );

  // append in the time direction by default
  time_direction = true;

  // do not log results by default
  log_results = false;

  auto_add = false; 

  reset_total = true;
}

/* ********************************************************************

   Global variables

   These eventually may/should be turned into attributes of the psradd
   class ... one thing I don't like about attributes is that they must
   be separately declared in the class definition and initialized in
   the constructor.

   ******************************************************************** */

// write results to disk by default
bool testing = false;

// ensure that archive has data before adding
bool check_has_data = false;

// tscrunch total after each new file is appended
bool tscrunch_total = false;

// phase align each archive before appending to total
bool phase_align = false;

// auto_add features:
// maximum amount of data (in seconds) to integrate into one archive
float max_integration_length = 0.0;
// maximum signal to noise to integrate into one archive
float max_signal_to_noise = 0.0;
// maximum interval (in seconds) across which integration should occur
float max_interval_between = 0.0;
// interval (in seconds) into which to divide the day
unsigned division_seconds = 0;
// maximum total integration interval (in seconds)  that goes into one archive
float max_interval_within = 0.0;
// maximum amount by which cal phase can differ
float cal_phase_diff = 0.0;

// name of the new ephemeris file
string parname;

// The centre frequency to select upon
double centre_frequency = -1.0;

// Only add in archives if their length matches this time
float required_archive_length = -1.0;

int backward_compatibility (int c)
{
  if (c == 'f')
    return 'o';

  if (c == 'p')
    return 'E';

  if (c == 's')
    return 'T';

  return c;
}

void psradd::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.filter = backward_compatibility;

  arg = menu.add (unload_name, 'o', "fname");
  arg->set_help ("output result to 'fname'");

  menu.add ("\n" "General options:");

  arg = menu.add (parname, 'E', "f.eph");
  arg->set_help ("Load and install new ephemeris from 'f.eph'");

  arg = menu.add (tscrunch_total, 'T');
  arg->set_help ("Tscrunch result after each new file added");

  arg = menu.add (time_direction, 'R');
  arg->set_help ("Append data in the frequency direction");

  arg = menu.add (patch_name, 'm', "domain");
  arg->set_help ("Patch missing sub-integrations: domain = time or phase");

  arg = menu.add (phase_align, 'P');
  arg->set_help ("Phase align archive with total before adding");

  arg = menu.add (log_results, 'L');
  arg->set_help ("Log results in <source>.log");

  arg = menu.add (testing, 't');
  arg->set_help ("Test mode: make no changes to file system");

  menu.add ("\n" "Restrictions:");

  arg = menu.add (this, &psradd::force, 'F');
  arg->set_help ("Force append against all conventional wisdom");

  arg = menu.add (centre_frequency, 'r', "freq");
  arg->set_help ("Add archives only with this centre frequency");

  arg = menu.add (check_has_data, 'z');
  arg->set_help ("Only add archives that have integration length > 0");

  arg = menu.add (required_archive_length, 'Z', "time");
  arg->set_help ("Only add archives that are time (+/- 0.5) seconds long");

  menu.add ("\n" "AUTO ADD: tscrunch and unload when ...");

  arg = menu.add (cal_phase_diff, 'C', "turns");
  arg->set_help ("... CAL phase changes by >= turns");
  arg->set_notification (auto_add);

  arg = menu.add (division_seconds, 'D', "sec");
  arg->set_help ("... next archive is from different 'sec' division of MJD");
  arg->set_notification (auto_add);

  arg = menu.add (max_interval_between, 'G', "sec");
  arg->set_help ("... time to next archive > 'sec' seconds");
  arg->set_notification (auto_add);

  arg = menu.add (max_interval_within, 'g', "sec");
  arg->set_help ("... time spanned exceeds 'sec' seconds");
  arg->set_notification (auto_add);

  arg = menu.add (max_integration_length, 'I', "sec");
  arg->set_help ("... integration length exceeds 'sec' seconds");
  arg->set_notification (auto_add);

  arg = menu.add (max_signal_to_noise, 'S', "s/n");
  arg->set_help ("... signal-to-noise ratio exceeds 's/n'");
  arg->set_notification (auto_add);

  menu.add ("\n" "Note: AUTO ADD options are incompatible with -o and -T");
}

void psradd::setup ()
{
  if (!auto_add && !unload_name.length())
    throw Error (InvalidState, "psradd::setup",
		 "please specify the output filename (use -o)");

  if (auto_add && !time_direction)
    throw Error (InvalidState, "psradd::setup",
		 "cannot combine AUTO ADD features with -R option");

  if (auto_add && tscrunch_total)
    throw Error (InvalidState, "psradd::setup",
		 "cannot combine AUTO ADD features with -T option\n");

  if (auto_add && unload_name.length())
    cerr << "psradd ignores -o when AUTO ADD features are used\n";

  if (unload->get_directory().length() && unload_name.length())
    cerr << "psradd ignores -O when -o is used \n";

  if (!parname.empty())
  {
    if (verbose)
      cerr << "psradd: loading ephemeris from '"<< parname <<"'" << endl;

    ephemeris = factory<Pulsar::Parameters> (parname);

    if (very_verbose)
    {
      cerr << "psradd: ephemeris loaded=" << endl;
      ephemeris->unload(stderr); 
      cerr << endl;
    }

  }

  if (!patch_name.empty())
  {
    Pulsar::Contemporaneity* policy = 0;
    if (patch_name == string("time"))
      policy = new Pulsar::Contemporaneity::AtEarth;
    else if (patch_name == string("phase"))
      policy = new Pulsar::Contemporaneity::AtPulsar;
    else
      throw Error (InvalidParam, "psradd::setup",
		   "unknown contemporaneity policy '" + patch_name + "'");

    patch = new Pulsar::PatchTime;
    patch->set_contemporaneity_policy( policy );
  }
}

void psradd::force ()
{
  time.chronological = false;
  time.must_match = false;

  frequency.must_match = false;
}

//! Process the given archive
void psradd::process (Pulsar::Archive* archive)
{
  if (very_verbose && archive->has_model())
  {
    for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
    {
      MJD epoch = archive->get_Integration(isub)->get_epoch();
      cerr << isub << ": phase=" 
	   << archive->get_model()->phase( epoch ) << endl;
    }
  }

  if (check_has_data && archive->integration_length() == 0)
  {
    cerr << "psradd: archive [" << archive->get_filename() << "]"
      " integration length is zero." << endl;
    return;
  }

  if (required_archive_length > 0
      && fabs(archive->integration_length()-required_archive_length) > 0.5)
  {
    cerr << "psradd: archive [" <<archive->get_filename() << "] not "
	 << required_archive_length << " seconds long- it was "
	 << archive->integration_length() << " seconds long" << endl;
    return;
  }

  if (centre_frequency > 0.0 
      && fabs(archive->get_centre_frequency()-centre_frequency) > 0.0001 )
  {
    cerr << "psradd: archive [" << archive->get_filename() << "]"
      " centre frequency=" << archive->get_centre_frequency() << " != "
      " required=" << centre_frequency << endl;
    return;
  }

  if (total)
  {
    if (max_integration_length != 0.0)
      check_integration_length ();

    if (max_signal_to_noise != 0.0)
      check_signal_to_noise ();

    if (max_interval_within != 0.0)
      check_interval_within ();

    if (division_seconds)
      check_division (archive);

    if (max_interval_between != 0.0)
      check_interval_between (archive);

    if (cal_phase_diff)
      check_cal_phase_diff (archive);
  }

  if (reset_total)
    set_total (archive);
  else
    append (archive);

  if (very_verbose)
    cerr << "psradd: total nsubint=" << total->get_nsubint() << endl;

  if (tscrunch_total)
  {
    if (verbose) cerr << "psradd: tscrunch total" << endl;

    // tscrunch the archive
    total->tscrunch();
    
    if (very_verbose)
      cerr << "psradd: after tscrunch, instance count = " 
	   << Reference::Able::get_instance_count() << endl;
  }
}

void psradd::finalize ()
{
  if (log_file)
    fprintf (log_file, "\n");

  if (reset_total)
    return;

  if (auto_add)
  {      
    if (verbose) cerr << "psradd: Auto add - tscrunching last " 
		      << total->integration_length()
		      << " seconds of data." << endl;
    total->tscrunch();
  }

  if (!time_direction)
  {
    // dedisperse to the new centre frequency
    if (total->get_dedispersed())
      total->dedisperse();
    
    // correct Faraday rotation to the new centre frequency
    if (total->get_faraday_corrected())
      total->defaraday();
    
    // re-compute the phase predictor to the new centre frequency
    if (total->has_model() && total->has_ephemeris())
      total->update_model ();
  }

  if (!testing)
  {
    if (verbose)
      cerr << "psradd: Unloading archive: '" << unload_name << "'" << endl;

    total->unload (unload_name);
  }
}

float mid_hi (Pulsar::Archive* archive)
{
  unsigned nbin = archive->get_nbin();

  int hi2lo, lo2hi, buffer;
  archive->find_transitions (hi2lo, lo2hi, buffer);
      
  if (hi2lo < lo2hi)
    hi2lo += nbin;

  return float (hi2lo + lo2hi) / (2.0 * nbin);
}

void psradd::set_total (Pulsar::Archive* archive)
{
  if (total)
  {
    if (verbose)
      cerr << "psradd: Auto add - tscrunch and unload " 
	   << total->integration_length() << " s archive" << endl;

    total->tscrunch();

    if (very_verbose)
      cerr << "psradd: after tscrunch, instance count = " 
	   << Reference::Able::get_instance_count() << endl;

    if (!testing)
      total->unload (unload_name);
  }

  if (verbose)
    cerr << "psradd: starting with " << archive->get_filename() << endl;

  total = archive;

  if (time_direction)
    time.init (total);
  else
    frequency.init (total);

  if (auto_add)
    unload_name = unload->get_output_filename( total );

  if (log_results)
  {
    string log_name = total->get_source() + ".log";

    if (log_name != log_filename)
    {
      if (log_file)
      {
	cerr << "psradd: Closing log file " << log_filename << endl;
	fclose (log_file);
      }
      
      cerr << "psradd: Opening log file " << log_name << endl;
      
      log_file = fopen (log_name.c_str(), "a");
      log_filename = log_name;

      if (!log_file)
	throw Error (FailedSys, "psradd", "fopen (" + log_name + ")");
    }

    fprintf (log_file, "\npsradd %s: %s", unload_name.c_str(),
	     total->get_filename().c_str());
  }

  if (verbose)
    cerr << "psradd: New filename: '" << unload_name << "'" << endl;

  if (ephemeris) try
  {
    if (verbose)
      cerr << "psradd: Installing new ephemeris" << endl;
    
    total->set_ephemeris (ephemeris);
  }
  catch (Error& error)
  {
    cerr << "psradd: Error installing ephemeris in "
	 << total->get_filename() << endl;
    
    if (verbose)
      cerr << error << endl;
    else
      cerr << "  " << error.get_message() << endl;
    
    if (!auto_add)
      exit (-1);
    
    throw error;
  }

  reset_total = false;
}

void psradd::append (Pulsar::Archive* archive) try
{
  if (phase_align)
  {
    Reference::To<Pulsar::Archive> standard;
    standard = total->total();
    Pulsar::Profile* std = standard->get_Profile(0,0,0);
    
    Reference::To<Pulsar::Archive> observation;
    observation = archive->total();
    Pulsar::Profile* obs = observation->get_Profile(0,0,0);
    
    archive->rotate_phase( obs->shift(std).get_value() );
  }

  if (archive->get_state() != total->get_state())
  {
    if (verbose)
      cerr << "psradd: converting state" 
	   << " from " << archive->get_state()
	   << " to " << total->get_state() << endl;
    
    archive->convert_state( total->get_state() );
  }
  
  if (patch)
  {
    if (verbose)
      cerr << "psradd: patching any missing sub-integrations" << endl;
    patch->operate (total, archive);
  }
  
  if (verbose)
    cerr << "psradd: appending " << archive->get_filename() << endl;

  if (time_direction)
    time.append (total, archive);
  else
    frequency.append (total, archive);

  if (log_file)
    fprintf (log_file, " %s", archive->get_filename().c_str());

  if (very_verbose)
    cerr << "psradd: after append, instance count = " 
	 << Reference::Able::get_instance_count() << endl;
}
catch (Error& error)
{
  cerr << "psradd: Archive::append exception:\n" << error << endl;
  if (auto_add)
    reset_total = true;
}
catch (...)
{
  cerr << "psradd: Archive::append exception thrown" << endl;
  if (auto_add)
    reset_total = true;
}


// ///////////////////////////////////////////////////////////////
//
// auto_add -G
//
void psradd::check_interval_between (Pulsar::Archive* archive)
{   
  double gap = (archive->start_time() - total->end_time()).in_seconds();
      
  if (verbose)
    cerr << "psradd: Auto add - gap = " << gap << " seconds" << endl;
      
  if (fabs(gap) > max_interval_between)
  {
    if (verbose)
      cerr << "psradd: gap=" << gap << " greater than interval between=" 
	   << max_interval_between << endl;
    reset_total = true;
  }
}

// ///////////////////////////////////////////////////////////////
//
// auto_add -C
//
void psradd::check_cal_phase_diff (Pulsar::Archive* archive)
{
  if (!archive->type_is_cal())
  {
    cerr << "psradd: Auto add - not a CAL" << endl;
    return;
  }

  total->tscrunch();
  archive->tscrunch();

  float midhi0 = mid_hi (total);
  float midhi1 = mid_hi (archive);

  float diff = fabs( midhi1 - midhi0 );

  if (verbose)
    cerr << "psradd: Auto add - CAL phase diff = " << diff << endl;

  if ( diff > cal_phase_diff )
  {
    if (verbose)
      cerr << "psradd: diff=" << diff << " greater than max diff=" 
	   << cal_phase_diff << endl;
    reset_total = true;
  }
}

MJD midtime (Pulsar::Archive* archive)
{
  return 0.5 * (archive->start_time() + archive->end_time());
}

double second (const MJD& mjd)
{
  return  mjd.get_secs() + mjd.get_fracsec();
}

// ///////////////////////////////////////////////////////////////
//
// auto_add -D
//
void psradd::check_division (Pulsar::Archive* archive)
{
  unsigned total_division = second( midtime( total ) ) / division_seconds;
  unsigned archive_division = second( midtime( archive ) ) / division_seconds;

  if (verbose)
    cerr << "psradd: Auto add - division = " << archive_division << endl;

  if ( total_division != archive_division )
  {
    if (verbose)
      cerr << "psradd: not in current division = " << total_division << endl;
    reset_total = true;
  }
}


// ///////////////////////////////////////////////////////////////
//
// auto_add -I
//
void psradd::check_integration_length ()
{
  double integration = total->integration_length();

  if (verbose)
    cerr << "psradd: Auto add - integration = " << integration
	 << " seconds" << endl;

  if (integration > max_integration_length)
    reset_total = true;
}

// ///////////////////////////////////////////////////////////////
//
// auto_add -S
//
void psradd::check_signal_to_noise ()
{     
  float signal_to_noise = total->total()->get_Profile(0,0,0)->snr();
      
  if (verbose)
    cerr << "psradd: Auto add - S/N = " << signal_to_noise
	 << " seconds" << endl;
      
  if (signal_to_noise > max_signal_to_noise)
    reset_total = true;
}

// ///////////////////////////////////////////////////////////////
//
// auto_add -g
//
void psradd::check_interval_within ()
{
  double gap = (total->start_time() - total->end_time()).in_seconds();

  if (verbose)
    cerr << "psradd: Auto add - gap " << gap << " seconds" << endl;

  if (fabs(gap) > max_interval_within)
  {
    if (verbose)
      cerr << "psradd: gap=" << gap << " greater than interval within="
	   << max_interval_within << endl;
    reset_total = true;
  }
}

