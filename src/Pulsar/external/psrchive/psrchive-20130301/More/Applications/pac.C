/***************************************************************************
 *
 *   Copyright (C) 2003-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#if 1
#include "Pulsar/psrchive.h"
#include "Pulsar/Interpreter.h"
#else
#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"
#endif

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/PolnProfile.h"

#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/ProjectionCorrection.h"
#include "Pulsar/ReflectStokes.h"
#include "Pulsar/BackendFeed.h"

#include "Pulsar/ProcHistory.h"
#include "Pulsar/Feed.h"

#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>
#include <string.h>

using namespace std;

// A command line tool for calibrating Pulsar::Archives
const char* args = "A:aBbCcDd:Ee:fFGhiIJ:j:k:lLM:m:n:O:op:PqRr:sSt:Tu:UvVwWxyZ";

void usage ()
{
  cout << "A program for calibrating Pulsar::Archives\n"
    "Usage: pac [options] filenames\n"
    "  -q             Quiet mode\n"
    "  -v             Verbose mode\n"
    "  -V             Very verbose mode\n"
    "  -i             Show revision information\n"
    "\n"
    "Database options: \n"
    "  -d database    Read calibration database summary \n"   
    "  -p path        Search for CAL files in the specified path \n"
    "  -u ext         Add to file extensions recognized in search \n"
    "                 (defaults: .cf .pcal .fcal .pfit) \n"
    "  -w             Write a new database summary file \n"
    "  -W             Create database from calibrators listed in metafiles \n"
    "  -k filename    Output database to filename \n"
    "  -l             Cache last calibrator\n"
    "\n"
    "Calibrator options: \n"
    "  -A filename    Use the calibrator in filename, as output by pcm/pacv \n"
    "  -P             Calibrate polarisations only \n"
    "  -R             Calibrate the receiver (feed) only \n"
    "  -r filename    Use the specified receiver parameters file \n"
    "  -S             Use the complete Reception model \n"
    "  -s             Use the Polar Model \n"
    "  -I             Correct ionospheric Faraday rotation using IRI\n"
    "  -x             Derive calibrator Stokes parameters from fluxcal data\n"
    "  -y             Always trust the Pointing::feed_angle attribute \n"
    "\n"
    "Rough Alignment options [not recommended]: \n"
    "  -B             Fix the off-pulse baseline statistics \n"
    "  -D             Fix the reference degree of polarization \n"
    "\n"
    "Matching options: \n"
    "  -m [b|a]       Use only calibrator before|after observation\n"
    "  -T             Take closest time (no maximum interval)\n"
    "  -c             Take closest sky coordinates (no maximum distance)\n"
    "  -Z             Do not try to match instruments\n"
    "  -F             Do not try to match frequencies\n"
    "  -b             Do not try to match bandwidths\n"
    "  -o             Allow opposite sidebands\n"
    "  -a             Per-channel matching\n"
    "  -L             Print verbose matching information \n"
    "\n"
    "Expert options: \n"
    "  -f             Override flux calibration flag\n"
    "  -G             Normalize profile weights by absolute gain \n"
    "  -U             Disable frontend corrections (parallactic angle, etc)\n"
    "\n"
    "Input/Output options: \n"
    "  -e ext         Extension added to output filenames (default .calib) \n"
    "  -j job1[,jobN] Preprocessing job[s] \n"
    "  -J jobs        Multiple preprocessing jobs in 'jobs' file \n"
    "  -M meta        File from which input filenames are read \n"
    "  -n [q|u|v]     Flip the sign of Stokes Q, U, or V \n"
    "  -O path        Path to which output files are written \n"
    "\n"
    "See " PSRCHIVE_HTTP "/manuals/pac for more details\n"
       << endl;
}

// cut down the calibrator solution to only the feed
void keep_only_feed( Pulsar::PolnCalibrator* );

int main (int argc, char *argv[]) try
{    
  bool verbose = false;
  bool do_fluxcal = true;
  bool do_polncal = true;
  bool use_fluxcal_stokes = false;
  bool enable_frontend = true;

  // Flag for only displaying the system-equivalent flux density.
  bool only_display_sefd = false;

  // Preprocessing jobs
  vector<string> jobs;

  bool write_database_file = false;
  bool check_flags = true;
  // By default, don't use last calibrator caching
  Pulsar::Database::cache_last_cal = false;

  // name of the file containing the list of Archive filenames
  char* metafile = NULL;

  // treat all files as metafiles containing lists of calibrator filenames
  bool cals_metafile = false;

  // known feed transformation
  Calibration::Feed* feed = 0;

  // model ionosphere
  Pulsar::IonosphereCalibrator* ionosphere = 0;

  // default calibrator type
  Reference::To<const Pulsar::Calibrator::Type> pcal_type;
  pcal_type = new Pulsar::CalibratorTypes::SingleAxis;

  // default searching criterion
  Pulsar::Database::Criterion criterion;

  string cals_are_here = "./";

  string database_filename;
  vector<string> cal_dbase_filenames;

  // directory to which calibrated output files are written
  string unload_path;
  string unload_ext = "calib";

  // filename from which calibrator solution to be applied will be loaded
  string model_file;
  // strip down the above-named calibrator solution to keep only the feed
  bool apply_only_feed = false;

  vector<string> exts;

  string pcal_file;

  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";

  string command = "pac ";

  string::size_type index;
  
  string optarg_str;

  Pulsar::ReflectStokes reflections;

  while ((gotc = getopt(argc, argv, args)) != -1) 

    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'q':
      Pulsar::Archive::set_verbosity(0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity(2);
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'i':
      cout << "$Id: pac.C,v 1.109 2011/02/17 07:43:53 straten Exp $" << endl;
      return 0;

    case 'k':
      database_filename = optarg;
      break;

    case 'A':
      model_file = optarg;
      command += " -A ";

      // Just take the filename, not the full path
      optarg_str = optarg;
      index = optarg_str.rfind("/", optarg_str.length()-2);

      if (index == string::npos)
      	command += optarg_str;      
      else
      {
        // Larger than last index but doesn't matter. String class will 
	// just take the rest of the string safely
        command += optarg_str.substr(index+1, optarg_str.length()); 
      }
      break;

    case 'B':
      pcal_type = new Pulsar::CalibratorTypes::OffPulse;
      command += " -B";
      break;

    case 'C':
      pcal_type = new Pulsar::CalibratorTypes::van04_Eq18;
      command += " -C";
      break;

    case 'D':
      pcal_type = new Pulsar::CalibratorTypes::DoP;
      command += " -D";
      break;

    case 'd':
      cal_dbase_filenames.push_back (optarg);
      command += " -d ";

      // Just take the filename, not the full path
      optarg_str = optarg;
      index = optarg_str.rfind("/", optarg_str.length()-2);

      if (index == string::npos) {
      	command += optarg_str;
      }
      
      else {
        // Larger than last index but doesn't matter. String class will 
	// just take the rest of the string safely
        command += optarg_str.substr(index+1, optarg_str.length()); 
      }
      break;

    case 'e':
      unload_ext = optarg;
      command += " -e ";
      command += optarg;
      break;

    case 'E':
      only_display_sefd = true;
      do_polncal = false;
      break;

    case 'f':
      check_flags = false;
      command += " -f";
      break;

    case 'G':
      Pulsar::PolnProfile::normalize_weight_by_absolute_gain = true;
      command += " -g";
      break;

    case 'I':
      ionosphere = new Pulsar::IonosphereCalibrator;
      break;

    case 'j':
      separate (optarg, jobs, ",");
      break;
      
    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'l':
      Pulsar::Database::cache_last_cal = true;
      command += " -l";
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'm': 
      if (optarg[0] == 'b')
	criterion.policy = Pulsar::Database::CalibratorBefore;
      else if (optarg[0] == 'a')
	criterion.policy = Pulsar::Database::CalibratorAfter;
      else {
	cerr << "pac: unrecognized matching policy code" << endl;
	return -1;
      }
      command += " -m ";
      command += optarg;
      break;

    case 'n': {

      reflections.add_reflection( optarg[0] );

      command += " -n ";
      command += optarg;
      break;

    }

    case 'O':
      unload_path = optarg;
      command += " -O ";
      command += optarg;
      break;

    case 'o':
      Pulsar::Archive::Match::opposite_sideband = true;
      command += " -o";
      break;

    case 'p':
      cals_are_here = optarg;
      command += " -p ";
      
      // Just take the filename, not the full path
      optarg_str = optarg;
      index = optarg_str.rfind("/", optarg_str.length()-2);
      
      if (index == string::npos) {
      	cout << "No slash present\n";
      	command += optarg_str;
      }
      
      else {
        // Larger than last index but doesn't matter. String class will 
	// just take the rest of the string safely
        command += optarg_str.substr(index+1, optarg_str.length()); 
      }
      
      break;

    case 'P':
      do_fluxcal = false;
      command += " -P";
      break;

    case 'R':
      apply_only_feed = true;
      command += " -R";
      break;

    case 'r':
      feed = new Calibration::Feed;
      feed -> load (optarg);
      cerr << "pac: Feed parameters loaded:"
	"\n  orientation 0 = "
	   << feed->get_orientation(0).get_value() * 180/M_PI << " deg"
	"\n  ellipticity 0 = "
	   << feed->get_ellipticity(0).get_value() * 180/M_PI << " deg"
	"\n  orientation 1 = "
	   << feed->get_orientation(1).get_value() * 180/M_PI << " deg"
	"\n  ellipticity 1 = "
	   << feed->get_ellipticity(1).get_value() * 180/M_PI << " deg"
	   << endl;
      command += " -r ";

      // Just take the filename, not the full path
      optarg_str = optarg;
      index = optarg_str.rfind("/", optarg_str.length()-2);

      if (index == string::npos) {
      	command += optarg_str;
      }
      
      else {
        // Larger than last index but doesn't matter. String class will 
	// just take the rest of the string safely
        command += optarg_str.substr(index+1, optarg_str.length()); 
      }
      break;

    case 's':
      pcal_type = new Pulsar::CalibratorTypes::van02_EqA1;
      command += " -s";
      break;

    case 'S':
      pcal_type = new Pulsar::CalibratorTypes::ovhb04;
      command += " -S";
      break;

    case 'u':
      key = strtok (optarg, whitespace);
      while (key) {
	// remove the leading .
	while (*key == '.')
	  key ++;
        exts.push_back(key);
        key = strtok (NULL, whitespace);
      }
      command += " -u ";
      command += optarg;
      break;

    case 'U':
      enable_frontend = false;
      command += " -U ";
      break;

    case 'w':
      write_database_file = true;
      command += " -w";
      break;

    case 'W':
      write_database_file = true;
      cals_metafile = true;
      command += " -W";
      break;

    case 'x':
      use_fluxcal_stokes = true;
      command += " -x";
      break;

    case 'y':
      Pulsar::ProjectionCorrection::trust_pointing_feed_angle = true;
      command += " -y";
      break;

    case 'b':
      criterion.check_bandwidth = false;
      command += " -b";
      break;
    case 'c':
      criterion.check_coordinates = false;
      command += " -c";
      break;
    case 'T':
      criterion.check_time = false;
      command += " -T";
      break;
    case 'F':
      criterion.check_frequency = false;
      command += " -F";
      break;
    case 'Z':
      criterion.check_instrument = false;
      command += " -Z";
      break;
    case 'a':
      criterion.check_frequency = false;
      criterion.check_bandwidth = false;
      criterion.check_frequency_array = true;
      break;

    default:
      return -1;
    }
 

  Pulsar::Database::set_default_criterion (criterion);

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    if (!write_database_file || cals_metafile)
    {
      cout << "pac: No filenames specified. Exiting" << endl;
      exit(-1);
    }
  } 

  // the archive from which a calibrator will be constructed
  Reference::To<Pulsar::Archive> model_arch;

  // the calibrator constructed from the specified archive
  Reference::To<Pulsar::PolnCalibrator> model_calibrator;

  // the database from which calibrators will be selected
  Reference::To<Pulsar::Database> dbase;
  
  if ( !model_file.empty() ) try
  {
    cerr << "pac: Loading calibrator from " << model_file << endl;

    if (criterion.check_frequency_array)
      cerr << "pac: Warning: -a and -A options are incompatible" << endl;

    model_arch = Pulsar::Archive::load(model_file);
    model_calibrator = new Pulsar::PolnCalibrator(model_arch);

    if (apply_only_feed)
      keep_only_feed (model_calibrator);

    pcal_type = model_calibrator->get_type();
  }
  catch (Error& error)
  {
    cerr << "pac: Could not load calibrator from " << model_file << endl;
    cerr << error << endl;
    return -1;
  }

  if ( use_fluxcal_stokes && 
       ! pcal_type->is_a<Pulsar::CalibratorTypes::SingleAxis>() )
  {
    cerr << "pac: Fluxcal-derived Stokes params are incompatible with the " 
      << "selected calibration method" << endl;
    return -1;
  }

  if (cal_dbase_filenames.size()) try
  {
    for (unsigned i=0; i<cal_dbase_filenames.size(); i++)
    {
      cout << "pac: Loading database from " << cal_dbase_filenames[i] << endl;
      if (i==0)
	dbase = new Pulsar::Database (cal_dbase_filenames[i]);
      else
	dbase->load (cal_dbase_filenames[i]);
    }
  }
  catch (Error& error)
  {
    cerr << "pac: Error loading CAL database" << error << endl;
    return -1;
  }

  else if ( model_file.empty() ) try
  {   
    // Generate the CAL file database
    cout << "pac: Generating new calibrator database" << endl;
    
    if (cals_metafile)
    {
      Reference::To<Pulsar::Database> temp;

      for (unsigned i=0; i < filenames.size(); i++)
      {
        cout << "pac: Loading calibrator filenames from metafile=" 
             << filenames[i] << endl;

        temp = new Pulsar::Database (cals_are_here, filenames[i]);

        if (temp->size() <= 0)
        {
          cerr << "pac: No calibrators found in " << cals_are_here
               << " listed in " << filenames[i] << endl;
          continue;
        }

        if (!dbase)
          dbase = temp;
        else
          dbase -> merge (temp);
      }

      if (dbase->size() <= 0)
      {
	cerr << "pac: No calibrators found in the provided metafiles" << endl;
	return -1;
      }    
    }
    else
    {
      exts.push_back("cf");
      exts.push_back("pcal");
      exts.push_back("fcal");
      exts.push_back("pfit");
    
      dbase = new Pulsar::Database (cals_are_here, exts);

      if (dbase->size() <= 0)
      {
	cerr << "pac: No calibrators found in " << cals_are_here << endl;
	return -1;
      }    
    }

    if (verbose)
      cerr << "pac: " << dbase->size() << " calibrators found" << endl;
    
    if (write_database_file)
    {
      if (database_filename.empty ())
	database_filename = dbase->get_path() + "/database.txt";

      cout << "pac: Writing database summary file to " 
	   << database_filename << endl;
      
      dbase -> unload (database_filename);
    }

    // all of the supplied filenames are treated as metafiles
    if (cals_metafile)
      return 0;
  }
  catch (Error& error)
  {
    cerr << "pac: Error generating CAL database" << error << endl;
    return -1;
  }

  if (feed)
    dbase -> set_feed (feed);

  // Start calibrating archives
  
  Pulsar::Interpreter* preprocessor = standard_shell();

  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    cout << endl;

    if (verbose)
      cerr << "pac: Loading " << filenames[i] << endl;
    
    Reference::To<Pulsar::Archive> arch = Pulsar::Archive::load(filenames[i]);

    cout << "pac: Loaded archive " << filenames[i] << endl;

    if (jobs.size())
    {
      if (verbose)
	cerr << "pac: preprocessing " << filenames[i] << endl;
      preprocessor->set (arch);
      preprocessor->script (jobs);
    }

    bool successful_polncal = false;

    if (do_polncal && arch->get_poln_calibrated() )
      cout << "pac: " << filenames[i] << " already poln calibrated" << endl;

    else if (do_polncal && !arch->get_poln_calibrated())
    {
      Reference::To<Pulsar::PolnCalibrator> pcal_engine;

      if (model_calibrator)
      {
	if (verbose)
	  cout << "pac: Applying specified calibrator" << endl;

	pcal_engine = model_calibrator;
      }

      else try
      {
	if (verbose)
	  cout << "pac: Finding PolnCalibrator" << endl;

	pcal_engine = dbase->generatePolnCalibrator(arch, pcal_type);
      }
      catch (Error& error)
      {
	error << " -- closest match: \n\n"
	      << dbase->get_closest_match_report ();
	throw error;
      }

      if (use_fluxcal_stokes) try
      {
        if (verbose)
          cout << "pac: Calculating fluxcal Stokes params" << endl;

        // Find appropriate fluxcal from DB 
        Reference::To<Pulsar::FluxCalibrator> flux_cal;
	try
	{
	  flux_cal = dbase->generateFluxCalibrator(arch);
	}
	catch (Error& error)
	{
	  error << " -- closest match: \n\n"
		<< dbase->get_closest_match_report ();
	  throw error;
	}

        // Combine already-selected pcal_engine with fluxcal stokes
        // into a new HybridCalibrator
        Reference::To<Pulsar::HybridCalibrator> hybrid_cal;
        hybrid_cal = new Pulsar::HybridCalibrator;
        hybrid_cal->set_reference_input(flux_cal->get_CalibratorStokes(),
            flux_cal->get_filenames());
        hybrid_cal->set_reference_observation(
            static_cast<Pulsar::ReferenceCalibrator*>(pcal_engine.get()));

        pcal_engine = hybrid_cal;
      }
      catch (Error& error)
      {
        cerr << "pac: Error computing cal Stokes for " << arch->get_filename() 
          << endl
          << "\t" << error.get_message() << endl;
      }
      
      pcal_file = pcal_engine->get_filenames();

      cout << "pac: PolnCalibrator constructed from:\n\t" << pcal_file << endl;
      pcal_engine->calibrate (arch);

      if (arch->get_npol() == 4)
      {
        if (enable_frontend)
	{
          if (verbose)
            cerr << "pac: Correcting platform, if necessary" << endl;

          Pulsar::FrontendCorrection correct;
          correct.calibrate (arch);

        }
	else 
          cerr << "pac: Frontend corrections disabled, skipping" << endl;
      }

      if (ionosphere)
      {
	cerr << "pac: Correcting ionospheric Faraday rotation" << endl;
	ionosphere->calibrate (arch);
      }

      cout << "pac: Poln calibration complete" << endl;

      successful_polncal = true;
    }
        
    /* The PolnCalibrator classes normalize everything so that flux
       is given in units of the calibrator flux.  Unless the calibrator
       is flux calibrated, it will undo the flux calibration step.
       Therefore, the flux cal should take place after the poln cal */

    bool successful_fluxcal = false;
    
    if (do_fluxcal && arch->get_scale() == Signal::Jansky && check_flags)
      cout << "pac: " << filenames[i] << " already flux calibrated" << endl;
    
    else if (!dbase)
      cout << "pac: Not performing flux calibration (no database)." << endl;

    else if (do_fluxcal) try
    {
      if (verbose)
	cout << "pac: Generating flux calibrator" << endl;
      
      Reference::To<Pulsar::FluxCalibrator> fcal_engine;

      try
      {
	fcal_engine = dbase->generateFluxCalibrator(arch);
      }
      catch (Error& error)
      {
	error << " -- closest match: \n\n"
	      << dbase->get_closest_match_report ();
	throw error;
      }

      cout << "pac: Mean SEFD = " << fcal_engine->meanTsys() * 1e-3
        << " Jy" << endl;

      if (only_display_sefd)
      {
        // Skip the flux-calibration, HISTORY table modification, and unloading 
        // of a new file.
        continue;
      }
      else
      {
        cout << "pac: FluxCalibrator constructed from:\n\t"
          << fcal_engine->get_filenames() << endl;

        if (verbose) 
          cerr << "pac: Calibrating Archive fluxes" << endl;

        fcal_engine->calibrate(arch);

        cout << "pac: Flux calibration complete" << endl;

        successful_fluxcal = true;
      }
    }
    catch (Error& error)
    {
      cerr << "pac: Could not flux calibrate " << arch->get_filename() << endl
	   << "\t" << error.get_message() << endl;
    }

    string newname = replace_extension( filenames[i], unload_ext );

    if (!successful_fluxcal)
      newname += "P";

    if (!unload_path.empty())
      newname = unload_path + "/" + basename (newname);

    if (verbose)
      cerr << "pac: Calibrated Archive name '" << newname << "'" << endl;
    
    reflections.transform (arch);

    // See if the archive contains a history that should be updated
    
    Pulsar::ProcHistory* fitsext = arch->get<Pulsar::ProcHistory>();

    if (fitsext)
    {
      if (command.length() > 80)
      {
	cerr << "pac: ProcHistory command string truncated to 80 chars"
	     << endl;
	fitsext->set_command_str(command.substr(0, 80));
      }
      else
      {
	fitsext->set_command_str(command);
      }
    }

    if (verbose)
      cerr << "pac: Unloading " << newname << endl;
    
    arch->unload(newname);
    
    cout << "pac: Calibrated archive " << newname << " unloaded" << endl;
    
  }
  catch (Error& error)
  {
    cerr << "pac: Error while handling " << filenames[i] << ":" << endl; 
    cerr << error << endl;
  }

  cout << "\npac: Finished all files" << endl;
  return 0;

}
 catch (Error& error)
 {
   cerr << "pac: error" << error << endl;
   return -1;
 }



using Calibration::BackendFeed;

void keep_only_feed( Pulsar::PolnCalibrator* cal )
{
  const unsigned nchan = cal->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!cal->get_transformation_valid(ichan))
      continue;

    MEAL::Complex2* xform = cal->get_transformation(ichan);

    BackendFeed* feed = dynamic_cast<BackendFeed*> (xform);

    if (!feed)
      throw Error (InvalidState, "keep_only_feed",
		   "PolnCalibrator at ichan=%u is not a "
		   "BackendFeed transformation", ichan);

    feed->set_gain( 1.0 );
    feed->set_diff_gain( 0.0 );
    feed->set_diff_phase( 0.0 );
  }
}
