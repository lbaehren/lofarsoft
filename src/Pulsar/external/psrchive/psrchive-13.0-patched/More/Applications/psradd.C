/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/psrchive.h"

#include "Pulsar/TimeAppend.h"
#include "Pulsar/FrequencyAppend.h"
#include "Pulsar/PatchTime.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"

#include "Pulsar/ProcHistory.h"
#include "Pulsar/TimeSortedOrder.h"

#include "Pulsar/Interpreter.h"

#include "load_factory.h"
#include "dirutil.h"
#include "strutil.h"
#include "separate.h"

#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <math.h>

using namespace std;

static const char* args
= "b:c:C:E:e:f:FG:hiI:j:J:LM:m:O:o:p:PqRr:sS:tTUvVwzZ:";

void reorder(Reference::To<Pulsar::Archive> arch);

void usage () {
  cout <<
    "A program for adding Pulsar::Archives together \n"
    "USAGE: psradd [" << args << "] filenames \n"
    " -h          This help page \n"
    " -i          Show revision information \n"
    " -q          Quiet mode (suppress warnings) \n"
    " -v          Verbose mode (informational) \n"
    " -V          Very verbose mode (debugging) \n"
    "\n"
    " -E f.eph    Load and install new ephemeris from f.eph \n"
    " -o fname    Output result to 'fname' \n"
    " -F          Force append despite mismatch of header parameters \n"
    " -j j1[,jN]  preprocessing job[s] \n"
    " -J jobs     multiple preprocessing jobs in 'jobs' file \n"
    " -L          Log results in source.log \n"
    " -M meta     Filename with list of files \n"
    " -m domain   Patch missing sub-integrations: domain = time or phase \n"
    " -P          Phase align archive with total before adding \n"
    " -R          Append data in the frequency direction \n"
    " -r freq     Add archive only if it has this centre frequency \n"
    " -t          Make no changes to file system (testing mode) \n"
    " -T          Tscrunch result after each new file (nice on RAM) \n"
    " -z          Only add archives that have integration length > 0 \n"
    " -Z time     Only add archives that are time (+/- 0.5) seconds long \n"
    "\n"
    "AUTO ADD options:\n"
    " -e ext      Extension added to output filenames (default .it) \n"
    " -C turns    Tscrunch+unload when CAL phase changes by >= turns \n"
    " -G sec      Tscrunch+unload when time to next archive > 'sec' seconds \n"
    " -I sec      Tscrunch+unload when archive contains 'sec' seconds \n"
    " -O path     Path to which output files are written \n"
    " -S s/n      Tscrunch+unload when archive has this S/N \n"
    "\n"
    "Note: AUTO ADD options (-C, -G, -I, and -S) are incompatible with -f \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/psradd for more details\n"
       << endl;
}

// returns the phase of the mid-point of the on cal hi
static float mid_hi (Pulsar::Archive* archive);

// set the total to the specified archive
static void set_total (Pulsar::Archive* archive);

// the accumulated total
static Reference::To<Pulsar::Archive> total;

// name of the output file
static string unload_name;

// verbose output
static bool verbose = false;

// very verbose output
static bool vverbose = false;

// log the results
static bool log_results = false;

// tscrunch+unload when certain limiting conditions are met
static bool auto_add = false; 

// after loading the next archive, reset the total
static bool reset_total_next_load = true;

// extension added to auto-added output files
static string integrated_extension ("it");

// directory to which auto-added output files are written
static string integrated_path;

// the file stream to which log messages are written
static FILE* log_file = 0;

// the name of the currently open log file
static string log_filename;

// the ephemeris to be installed
static Reference::To<Pulsar::Parameters> ephemeris;

// the append algorithm in use
static Pulsar::Append* append = 0;

int main (int argc, char **argv) try
{
  // append in the time direction
  bool time_direction = true;

  // do not make changes to file system when true
  bool testing = false;

  // if specified, bscrunch each archive to nbin
  int nbin = 0;

  // if specified, fscrunch each archive to nchan
  int nchan = 0;

  // ensure that archive has data before adding
  bool check_has_data = false;

  // tscrunch total after each new file is appended
  bool tscrunch_total = false;

  // phase align each archive before appending to total
  bool phase_align = false;

  // auto_add features:
  // maximum amount of data (in seconds) to integrate into one archive
  float integrate = 0.0;
  // maximum signal to noise to integrate into one archive
  float max_ston = 0.0;
  // maximum interval (in seconds) across which integration should occur
  float interval = 0.0;
  // maximum amount by which cal phase can differ
  float cal_phase_diff = 0.0;


  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of the new ephemeris file
  string parname;

  // The centre frequency to select upon
  double centre_frequency = -1.0;

  // Only add in archives if their length matches this time
  float required_archive_length = -1.0;

  // for writing into history
  string command = "psradd";
  
  // Preprocessing jobs
  vector<string> jobs;

  // Append algorithms
  Pulsar::TimeAppend time;
  Pulsar::FrequencyAppend frequency;

  // Patch algorithm
  Reference::To<Pulsar::PatchTime> patch;

  append = &time;

  int c;  
  while ((c = getopt(argc, argv, args)) != -1)  {
    switch (c)  {

    case 'h':
      usage();
      return 0;
      
    case 'i':
      cout << "$Id: psradd.C,v 1.69 2010/01/17 23:07:00 straten Exp $" 
	   << endl;
      return 0;

    case 'b':
      nbin = atoi (optarg);
      
      command += " -b ";
      command += optarg;
      
      break;

    case 'c':
      nchan = atoi(optarg);
      
      command += " -c ";
      command += optarg;
      
      break;

    case 'C':
      if (sscanf (optarg, "%f", &cal_phase_diff) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"'"
	  " as max CAL phase change" << endl;
	return -1;
      }
      auto_add = true;
      command += " -C ";
      command += optarg;
      break;

    case 'e':
      integrated_extension = optarg;
      
      command += " -e ";
      command += optarg;
      
      break;  

    case 'E':
    case 'p':
      parname = optarg;
      command += " -p ";
      command += optarg;
      break;

    case 'o':
      command += " -o ";
    case 'f':
      unload_name = optarg;
      command += optarg;
      
      if (c == 'f')
	command += " -f ";
      break;

    case 'F':

      time.chronological = false;
      time.must_match = false;

      frequency.must_match = false;

      command += " -F";
      
      break;

    case 'G':
      if (sscanf (optarg, "%f", &interval) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as maximum interval\n";
	return -1;
      }
      auto_add = true;
      command += " -G ";
      command += optarg;
      
      break;

    case 'I':
      if (sscanf (optarg, "%f", &integrate) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as integration total\n";
	return -1;
      }
      auto_add = true;
      command += " -I ";
      command += optarg;
      break;

    case 'j':
      separate (optarg, jobs, ",");
      break;
      
    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'O':
      integrated_path = optarg;
      command += " -O ";
      command += optarg;
      break;

    case 'S':
      if (sscanf (optarg, "%f", &max_ston) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as max S/N\n";
	return -1;
      }
      auto_add = true;
      command += " -S ";
      command += optarg;
      break;

    case 'L':
      log_results = true;
      break;

    case 'M':
      metafile = optarg;
      command += " -M ";
      command += optarg;
      break;

    case 'm':
    {
      Pulsar::Contemporaneity* policy = 0;
      if (optarg == string("time"))
	policy = new Pulsar::Contemporaneity::AtEarth;
      else if (optarg == string("phase"))
	policy = new Pulsar::Contemporaneity::AtPulsar;
      else
      {
	cerr << "psradd: unknown contemporaneity policy '"<<optarg<<"'"<< endl;
	return -1;
      }

      patch = new Pulsar::PatchTime;
      patch->set_contemporaneity_policy( policy );
      break;
    }

    case 'P':
      phase_align = true;
      command += " -P";
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'R':
      time_direction = false;
      append = &frequency;
      break;

    case 'r':
      centre_frequency = atof(optarg);
      command += " -r ";
      command += optarg;
      break;

      // lower-case 's' kept for backward-compatibility
    case 's':
    case 'T':
      tscrunch_total = true;
      command += " -T";
      break;

    case 't':
      testing = true;
      break;

    case 'U':
      cerr << "Will print message on Error creation" << endl;
      Error::verbose = true;
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      vverbose = true;
      verbose = true;
      break;

    case 'z':
      check_has_data = true;
      break;

    case 'Z': 
      required_archive_length = atof(optarg); 
      command += " -Z ";
      command += optarg;
      break;

    } 
  }

  if (!auto_add && !unload_name.length()) {
    cerr << "psradd requires a new filename on the command line (use -f) \n";
    return -1;
  }

  if (auto_add && !time_direction) {
    cerr << "psradd cannot combine AUTO ADD features with -R option \n";
    return -1;
  }

  if (auto_add && interval && tscrunch_total) {
    cerr << "psradd cannot combine AUTO ADD -G with tscrunch -T \n";
    return -1;
  }

  if (auto_add && unload_name.length())
    cerr << "psradd ignores -f when AUTO ADD features are used\n";

  if (integrated_path.length() && unload_name.length())
    cerr << "psradd ignores -O when -f is used \n";

  if (!parname.empty()) {

    if (verbose)
      cerr << "psradd: loading ephemeris from '"<< parname <<"'" << endl;

    try {

      ephemeris = factory<Pulsar::Parameters> (parname);

    }
    catch (Error& error) {
      cerr << "psradd could not load ephemeris from '" << parname << "'\n" 
	   << error.get_message() << endl;
      return -1;
    }

    if (vverbose) {
      cerr << "psradd: ephemeris loaded=" << endl;
      ephemeris->unload(stderr); 
      cerr << endl;
    }

  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
  {
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

    sort (filenames.begin(), filenames.end());
  }

  if (!filenames.size()) {
    cerr << "psradd requires a list of archive filenames as parameters.\n";
    return -1;
  }

  // the individual archive
  Reference::To<Pulsar::Archive> archive;

  Pulsar::Interpreter* preprocessor = standard_shell();

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    if (verbose) 
      cerr << "psradd: Loading [" << filenames[ifile] << "]\n";
    
    archive = Pulsar::Archive::load (filenames[ifile]);

    if (vverbose)
      cerr << "psradd: after load, instance count = " 
	   << Reference::Able::get_instance_count() << endl;

    if (vverbose && archive->has_model())
    {
      for (unsigned isub=0; isub < archive->get_nsubint(); isub++) {
	MJD epoch = archive->get_Integration(isub)->get_epoch();
	cerr << isub << ": phase=" 
	     << archive->get_model()->phase( epoch ) << endl;
      }
    }

    if (check_has_data && archive->integration_length() == 0) {
      cerr << "psradd: archive [" << filenames[ifile] << "]"
              "  integration length is zero.\n"
              "  (use -F to disable this check)" << endl;
      continue;
    }

    if (required_archive_length > 0
	&& fabs(archive->integration_length()-required_archive_length) > 0.5)
      {
	cerr << "psradd: archive [" <<filenames[ifile] << "] not "
	     << required_archive_length << " seconds long- it was "
	     << archive->integration_length() << " seconds long" << endl;
	continue;
      }

    if( centre_frequency > 0.0 
	&& fabs(archive->get_centre_frequency()-centre_frequency) > 0.0001 )
      continue;

    try
    {
      if (jobs.size())
      {
	if (verbose)
	  cerr << "psradd: preprocessing " << filenames[ifile] << endl;
	preprocessor->set(archive);
	preprocessor->script(jobs);
      }

      if (nbin)
	archive->bscrunch_to_nbin (nbin);

      if (nchan)
	archive->fscrunch_to_nchan (nchan);
    }
    catch (Error& error) {
      cerr << "psradd: preprocessing error\n"
	"  " << error.get_message() << "\n"
	"  on " << filenames[ifile] << endl;
      continue;
    }

    if (reset_total_next_load)
    {
      if (verbose) cerr << "psradd: Setting total" << endl;
      set_total (archive);
      
      if (vverbose)
	cerr << "psradd: after reset total, instance count = " 
	     << Reference::Able::get_instance_count() << endl;

      reset_total_next_load = false;
      continue;
    }

    bool reset_total_current = false;
    
    if (interval != 0.0)
    {   
      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -G: check the gap between the end of total
      // and the start of archive
      
      double gap = (archive->start_time() - total->end_time()).in_seconds();
      
      if (verbose)
	cerr << "psradd: Auto add - gap = " << gap << " seconds" << endl;
      
      if (fabs(gap) > interval)
      {
	if (verbose)
	  cerr << "psradd: gap=" << gap << " greater than interval=" 
	       << interval << endl;
	reset_total_current = true;
      }
    }

    if( cal_phase_diff )
    {
      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -C: check that the calibrator observations are aligned

      if (!archive->type_is_cal())
      {
	cerr << "psradd: Auto add - not a CAL" << endl;
	continue;
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
	reset_total_current = true;
      }

    }

    if (!reset_total_current)
    {

      try {

	if (phase_align) {

	  Reference::To<Pulsar::Archive> standard;
	  standard = total->total();
	  Pulsar::Profile* std = standard->get_Profile(0,0,0);

	  Reference::To<Pulsar::Archive> observation;
	  observation = archive->total();
	  Pulsar::Profile* obs = observation->get_Profile(0,0,0);

	  archive->rotate_phase( obs->shift(std).get_value() );

	}

	if (archive->get_state() != total->get_state()) {

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
	  cerr << "psradd: appending archive to total" << endl;

	if (time_direction)
	  time.append (total, archive);
	else
	  frequency.append (total, archive);

	if (vverbose)
	  cerr << "psradd: after append, instance count = " 
	       << Reference::Able::get_instance_count() << endl;

      }
      catch (Error& error) {
	cerr << "psradd: Archive::append exception:\n" << error << endl;
	if (auto_add)
	  reset_total_current = true;
      }
      catch (...) {
        cerr << "psradd: Archive::append exception thrown" << endl;
        if (auto_add)
          reset_total_current = true;
      }

      if (log_file)
	fprintf (log_file, " %s", archive->get_filename().c_str());
    }

    if (integrate != 0.0)
    {
      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -I: check that amount of data integrated in total
      // is less than the limit
      
      double integration = total->integration_length();

      if (verbose)
	cerr << "psradd: Auto add - integration = " << integration
	     << " seconds" << endl;

      if (integration > integrate)
	reset_total_next_load = true;
    }

    if (max_ston != 0.0)
    {
      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -S: check that S/N of the integrated data is less
      // than the limit
      
      float ston = total->total()->get_Profile(0,0,0)->snr();
      
      if (verbose)
	cerr << "psradd: Auto add - S/N = " << ston
	     << " seconds" << endl;
      
      if (ston > max_ston)
	reset_total_next_load = true;
    }
    
    if (tscrunch_total)
    {
      if (verbose) cerr << "psradd: tscrunch total" << endl;

      // tscrunch the archive
      total->tscrunch();

      if (vverbose)
	cerr << "psradd: after tscrunch, instance count = " 
	     << Reference::Able::get_instance_count() << endl;
    }

    /////////////////////////////////////////////////////////////////
    // See if the archive contains a history that should be updated:

    Pulsar::ProcHistory* fitsext = total->get<Pulsar::ProcHistory>();

    if (fitsext)
      fitsext->set_command_str(command);

    if (reset_total_next_load || reset_total_current)
    {
      if (verbose)
	cerr << "psradd: Auto add - tscrunch and unload " 
	     << total->integration_length() << " s archive" << endl;

      total->tscrunch();

      if (vverbose)
	cerr << "psradd: after tscrunch, instance count = " 
	     << Reference::Able::get_instance_count() << endl;

      if (!testing)
      {
	reorder( total );
	total->unload (unload_name);
      }      

      if (reset_total_current)
      {
	if (verbose)
	  cerr << "psradd: Auto add - reset total to current" << endl;

	set_total( archive );

	if (vverbose)
	  cerr << "psradd: after reset total, instance count = " 
	       << Reference::Able::get_instance_count() << endl;
      }
    }
  }
  catch (Error& error)
  {
    cerr << "psradd: Error handling [" << filenames[ifile] << "]\n" 
	 << error << endl;
  }

  if (!reset_total_next_load) try
  {
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
    
      reorder( total );
      total->unload (unload_name);
    }
  }
  catch (Error& error)
  {
    cerr << "psradd: Error unloading total\n" << error << endl;
    return -1;
  }

  if (log_file)
  {
    fprintf (log_file, "\n");
    fclose (log_file);
  }

  return 0;
}
catch (Error& error)
{
  cerr << "psradd: Unhandled error" << error << endl;
  return -1;
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

void
reorder(Reference::To<Pulsar::Archive> arch)
{
  Reference::To<Pulsar::TimeSortedOrder> tso = new Pulsar::TimeSortedOrder;
  tso->organise(arch,0);
}

void set_total (Pulsar::Archive* archive)
{
  total = archive;

  append->init (total);

  if (auto_add)
  {
    unload_name = total->get_filename() + "." + integrated_extension;
    if (!integrated_path.empty())
      unload_name = integrated_path + "/" + basename (unload_name);
  }

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
    
    reset_total_next_load = true;

    throw error;
  }
}
