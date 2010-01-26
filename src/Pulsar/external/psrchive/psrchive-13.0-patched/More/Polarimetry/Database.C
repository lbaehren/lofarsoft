/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Database.h"
#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/FluxCalibrator.h"

#include "Pulsar/DoPCalibrator.h"
#include "Pulsar/OffPulseCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/FeedExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ChannelSubsetMatch.h"

#include "ModifyRestore.h"
#include "Error.h"

#include "Stokes.h"
#include "Jones.h"
#include "MJD.h"
#include "sky_coord.h"

#include "dirutil.h"
#include "strutil.h"

#include <unistd.h> 
#include <errno.h>

using namespace std;
using namespace Pulsar;

bool Pulsar::Database::verbose = false;
bool Pulsar::Database::cache_last_cal = true;

/*! By default, the long time scale is set to four weeks. */
Pulsar::Option<double> 
Pulsar::Database::long_time_scale
(
 "Database::long_time_scale", 60.0 * 24 * 28,

 "Maximum time to flux calibrator [minutes]",

 "The maximum amount of time allowed between the epoch of an observation \n"
 "and the epoch of the flux calibrator and/or reception calibrator used to \n"
 "calibrate it.  By default, the long time scale is set to four weeks."
);

/*! By default, the short time scale is set to two hours. */
Pulsar::Option<double> 
Pulsar::Database::short_time_scale
(
 "Database::short_time_scale", 120.0,

 "Maximum time to poln calibrator [minutes]",

 "The maximum amount of time allowed between the epoch of an observation \n"
 "and the epoch of the reference calibrator used to calibrate it. \n"
 "By default, the long time scale is set to two hours."
);

/*! By default, the maximum angular separation is 5 degrees */
Pulsar::Option<double> 
Pulsar::Database::max_angular_separation 
(
 "Database::max_angular_separation", 5.0,

 "Maximum distance to poln calibrator [degrees]",

 "The maximum separation between the sky coordinates of an observations \n"
 "and those of the reference calibrator used to calibrate it."
);


/*! This null parameter is intended only to improve code readability */
const Pulsar::Archive* Pulsar::Database::any = 0;

// //////////////////////////////////////////////////////////////////////
//
// Pulsar::Database::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Pulsar::Database::Entry::init ()
{
  obsType = Signal::Unknown;

  bandwidth = 0.0;
  frequency = 0.0; 
  nchan = 0; 
  
  receiver = "unset";
  instrument = "unset";
  filename = "unset";
}

Pulsar::Database::Entry::Entry ()
{
  init ();
}

Pulsar::Database::Entry::Entry (const std::string& str)
{
  load (str);
}

//! Construct from a Pulsar::Archive
Pulsar::Database::Entry::Entry (const Pulsar::Archive& arch)
{
  init ();

  obsType = arch.get_type();

  const Pulsar::Backend* backend = arch.get<Backend>();
  if (!backend)
    throw Error (InvalidParam, "Pulsar::Database::Entry",
		 "Archive has no Backend Extension");

  const Pulsar::Receiver* receiver_ext = arch.get<Receiver>();
  if (!receiver_ext)
    throw Error (InvalidParam, "Pulsar::Database::Entry",
		 "Archive has no Receiver Extension");

  if (obsType == Signal::Calibrator)
  {
    const CalibratorExtension* ext = arch.get<CalibratorExtension>();

    if (!ext)
      throw Error (InvalidState, "Pulsar::Database::Entry",
		   "Archive::get_type==Signal::Calibrator but"
		   " no CalibratorExtension");

    calType = ext->get_type();
    time = ext->get_epoch();
  }
  else
  {
    unsigned nsubint = arch.get_nsubint();

    if (nsubint == 0)
      throw Error (InvalidParam, "Pulsar::Database::Entry",
		   "Archive has no Integrations");

    // assign the tscrunched epoch to the observation
    MJD epoch;
    for (unsigned isub=0; isub < nsubint; isub++)
      epoch += arch.get_Integration(isub)->get_epoch();

    time = epoch / double(nsubint);
  }

  position = arch.get_coordinates();
  bandwidth = arch.get_bandwidth();
  frequency = arch.get_centre_frequency();
  nchan = arch.get_nchan();
  filename = arch.get_filename();

  instrument = backend->get_name();
  receiver = receiver_ext->get_name();
}

//! Destructor
Pulsar::Database::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::Database::Entry::load (const string& str) try
{
  const char* whitespace = " \t\n";
  string line = str;

  // /////////////////////////////////////////////////////////////////
  // filename
  filename = stringtok (&line, whitespace);
  
  // /////////////////////////////////////////////////////////////////
  // type
  string typestr = stringtok (&line, whitespace);

  try
  {
    obsType = Signal::string2Source(typestr);
  }
  catch (Error& e)
  {
    obsType = Signal::Calibrator;

    // cerr << "Pulsar::Database::Entry::load name=" << typestr << endl;
    calType = Calibrator::Type::factory (typestr);
    // cerr << "Pulsar::Database::Entry::load type=" << calType->get_name() << endl;
  }

  // /////////////////////////////////////////////////////////////////
  // RA DEC
  string coordstr = stringtok (&line, whitespace);
  coordstr += " " + stringtok (&line, whitespace);
  position = sky_coord (coordstr.c_str());

  // /////////////////////////////////////////////////////////////////
  // MJD
  string mjdstr  = stringtok (&line, whitespace);
  time = MJD (mjdstr);

  // /////////////////////////////////////////////////////////////////
  // bandwidth, frequency, number of channels
  int s = sscanf (line.c_str(), "%lf %lf %d", &bandwidth, &frequency, &nchan);
  if (s != 3)
    throw Error (FailedSys, "Pulsar::Database::Entry::load",
                 "sscanf(" + line + ") != 3");

  stringtok (&line, whitespace);
  stringtok (&line, whitespace);
  stringtok (&line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // instrument
  instrument = stringtok (&line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // receiver
  receiver = stringtok (&line, whitespace);

  if (receiver.length() == 0)
    throw Error (InvalidParam, "Pulsar::Database::Entry::load",
                 "Could not parse '%s'");

}
catch (Error& error)
{
  throw error += "Pulsar::Database::Entry::load";
}

// unload to a string
void Pulsar::Database::Entry::unload (string& retval)
{
  retval = filename + " ";
  
  if (obsType == Signal::Calibrator)
    retval += calType->get_name();
  else
    retval += Signal::Source2string (obsType);

  retval += " ";
  retval += position.getRaDec().getHMSDMS();

  retval += " ";
  retval += time.printdays(15);

  retval += stringprintf (" %lf %lf %d", bandwidth, frequency, nchan);

  retval += " " + instrument + " " + receiver;
}

namespace Pulsar
{
  bool same (const Reference::To<const Calibrator::Type>& a, 
	     const Reference::To<const Calibrator::Type>& b)
  {
    if (!a && !b)
      return true;

    if (!a || !b)
      return false;

    return a->is_a(b) || b->is_a(a);
  }
}

bool Pulsar::operator == (const Database::Entry& a, const Database::Entry& b)
{
  return
    a.obsType == b.obsType &&
    same( a.calType, b.calType ) &&
    a.bandwidth == b.bandwidth &&
    a.frequency == b.frequency &&
    a.instrument == b.instrument &&
    a.receiver == b.receiver &&
    fabs( (a.time - b.time).in_seconds() ) < 10.0 &&
    a.position.angularSeparation(b.position).getDegrees() < 0.1;
}

bool Pulsar::operator < (const Database::Entry& a, const Database::Entry& b)
{ 
  if (a.instrument < b.instrument)
    return true;
  else if (a.instrument > b.instrument) 
    return false;

  if (a.receiver < b.receiver)
    return true;
  else if (a.receiver > b.receiver) 
    return false;

  if (a.frequency < b.frequency)
    return true;
  else if (a.frequency > b.frequency) 
    return false;

  if (a.bandwidth < b.bandwidth)
    return true; 
  else if (a.bandwidth > b.bandwidth)
    return false;

  return a.time < b.time;
}


bool Pulsar::Database::Criterion::match_verbose = false;

Pulsar::Database::Criterion::Criterion ()
{
  minutes_apart = short_time_scale;
  deg_apart  = max_angular_separation;

  policy = NoPolicy;

  check_receiver    = true;
  check_instrument  = true;
  check_frequency   = true;
  check_bandwidth   = true;
  check_obs_type    = true;
  check_time        = true;
  check_coordinates = true;
  check_frequency_array = false;
}

void Pulsar::Database::Criterion::no_data ()
{
  check_obs_type    = true;
  check_receiver    = false;
  check_instrument  = false;
  check_frequency   = false;
  check_bandwidth   = false;
  check_time        = false;
  check_coordinates = false;
  check_frequency_array = false;
}

//! returns true if this matches observation parameters
bool Pulsar::Database::Criterion::match (const Entry& have) const
{

  if (match_verbose)
    cerr << "Pulsar::Database::Criterion::match" << endl;
 
  if (check_obs_type)
  {
    if (match_verbose)
      cerr << "  Seeking obsType="
	   << Signal::source_string(entry.obsType) 
	   << " have obsType="
	   << Signal::source_string(have.obsType);
    
    if (entry.obsType == have.obsType)
    {
      if (match_verbose)
	cerr << "... matches" << endl; 
    }
    else
    {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }

    if (entry.obsType == Signal::Calibrator)
    {
      if (match_verbose)
	cerr << "  Seeking calType="
	     << entry.calType->get_name()
	     << " have calType="
	     << have.calType->get_name();

      if (same( entry.calType, have.calType ))
      {
	if (match_verbose)
	  cerr << "... matches" << endl; 
      }
      else
      {
	if (match_verbose)
	  cerr << "... no match" << endl;
	return false;
      }
    }
  }

  if (check_receiver)
  {
    if (match_verbose)
      cerr << "  Seeking receiver=" << entry.receiver
           << " have receiver=" << have.receiver;

    if (entry.receiver==have.receiver)
    {
      if (match_verbose)
        cerr << " ... matches" << endl;
    }
    else
    {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_instrument)
  {
    if (match_verbose)
      cerr << "  Seeking instrument=" << entry.instrument
           << " have instrument=" << have.instrument;

    if (entry.instrument==have.instrument)
    {
      if (match_verbose)
        cerr << " ... matches" << endl;
    }
    else
    {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_frequency)
  {
    if (match_verbose)
      cerr << "  Seeking frequency=" << entry.frequency
	   << " have frequency=" << have.frequency;

    double diff = fabs(entry.frequency - have.frequency);
    if (diff)
      diff /= entry.frequency + have.frequency;

    if ( diff < 1e-12 )
    {
      if (match_verbose)
	cerr << " ... matches" << endl; 
    }
    else
    {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_bandwidth)
  {
    if (match_verbose)
      cerr << "  Seeking bandwidth=" << entry.bandwidth
           << " have bandwidth=" << have.bandwidth;

    if (entry.bandwidth==have.bandwidth)
    {
      if (match_verbose)
        cerr << " ... matches" << endl;
    }
    else
    {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  double diff;

  if (check_time)
  {
    diff = (have.time - entry.time).in_minutes();

    switch (policy)
    {
    case NoPolicy:
    default:
      diff = fabs( diff );
      break;
    case CalibratorBefore:
      // do nothing
      break;
    case CalibratorAfter:
      diff = -diff;
      break;
    }

    if (match_verbose)
    {
      cerr << "  Seeking time=" << entry.time
	   << " have time=" << have.time
	   << "\n    difference=" << diff << " minutes "
	"(max=" << minutes_apart << ")";
    }

    if (diff < minutes_apart && diff >= 0)
    {
      if (match_verbose)
	cerr << " ... matches" << endl;
    }
    else
    {
      if (match_verbose)
	cerr << " do not match" << endl;
      return false;
    }
  }

  if (check_coordinates)
  {
    diff = have.position.angularSeparation(entry.position).getDegrees();

    if (match_verbose)
      cerr << "  Seeking position=" << entry.position.getHMSDMS() 
           << " have position=" << have.position.getHMSDMS()
           << "\n    difference=" << diff << " degrees "
           << "(max=" << deg_apart << ")";

    if (diff < deg_apart)
    {
      if (match_verbose)
	cerr << " ... matches" << endl;
    }
    else
    {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }

  }

  if (check_frequency_array)
  {
    // For now this assumes we can accurately recreate the original
    // freq arrays from nchan, bw, and freq.

    ChannelSubsetMatch chan_match;
    bool result = chan_match.match(have, entry);
    if (match_verbose)
    {
      cerr << "  Seeking channel subset match";
      if (result==true)
        cerr << "... matches!" << endl;
      else 
        cerr << "... no match (" << chan_match.get_reason() << ")" << endl;
    }
    if (result==false)
      return false;
  }

  if (match_verbose)
    cerr << "Pulsar::Database::Criterion::match found" << endl;
  
  return true;
}


// //////////////////////////////////////////////////////////////////////
//
// Pulsar::Database
//
// A group of Pulsar::Database::Entry objects
//

//! Null constructor
Pulsar::Database::Database ()
{
  path = "unset";
}

Pulsar::Database::Database (const string& filename)
{
  path = "unset";

  if (verbose)
    cerr << "Pulsar::Database load " << filename << endl;

  load (filename);
}

string get_current_path ()
{
  unsigned size = 128;
  char* fullpath = new char [size];

  while (getcwd(fullpath, size) == 0)
  {
    delete fullpath;
    if (errno != ERANGE)
      throw Error (FailedSys, "get_current_path", "getcwd");
    size *= 2;
    fullpath = new char[size];
  }

  string retval = fullpath;
  delete fullpath;
  return retval;
}

//! Construct a database from archives in a metafile
Pulsar::Database::Database (const std::string& _path, 
			    const std::string& metafile)
{
  vector <string> filenames;
  stringfload (&filenames, metafile);

  string current = get_current_path ();

  if (chdir(_path.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+_path+")");

  path = get_current_path ();

  construct (filenames);

  if (chdir(current.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+current+")");
}


/*! This constructor scans the given directory for calibrator files
  ending in the extensions specified in the second argument.
*/      
Pulsar::Database::Database (const string& _path, 
			    const vector<string>& extensions)
{
  string current = get_current_path ();

  if (chdir(_path.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+_path+")");

  path = get_current_path ();

  vector<string> filenames;
  vector<string> patterns (extensions.size());

  for (unsigned i = 0; i < extensions.size(); i++)
    patterns[i] = "*." + extensions[i];

  dirglobtree (&filenames, "", patterns);

  if (verbose)
    cerr << "Pulsar::Database " << filenames.size() 
         << " calibrator files found" << endl;

  construct (filenames);
  
  if (chdir(current.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+current+")");

}

void Pulsar::Database::construct (const vector<string>& filenames)
{
  ModifyRestore<bool> mod (Profile::no_amps, true);

  Reference::To<Pulsar::Archive> newArch;
  
  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    if (verbose)
      cerr << "Pulsar::Database loading "
	   << filenames[ifile] << endl;
    
    newArch = Archive::load(filenames[ifile]);
    
    if (verbose)
      cerr << "Pulsar::Database create new Entry" << endl;
    
    add (newArch);
  }
  catch (Error& error)
  {
    cerr << "Pulsar::Database error" << error.get_message() << endl;
  }

  if (verbose)
    cerr << "Pulsar::Database::construct "
         << entries.size() << " Entries" << endl; 
}

//! Destructor
Pulsar::Database::~Database ()
{
}

//! Loads an entire database from a file
void Pulsar::Database::load (const string& dbase_filename)
{
  string use_filename = expand (dbase_filename);

  FILE* fptr = fopen (use_filename.c_str(), "r");
  if (!fptr)
    throw Error (FailedCall, "Pulsar::Database::load",
		 "fopen (" + use_filename + ")");

  bool old_style = false;

  char temp[4096];
  int scanned = fscanf (fptr, "Pulsar::Database::path %s\n", temp);
  if (!scanned)
  {
    rewind (fptr);
    scanned = fscanf (fptr, "Pulsar::Calibration::Database::path %s\n", temp);
    if (scanned)
      cerr << "Pulsar::Database::load old database summmary file" << endl;
    else
      throw Error (InvalidParam, "Pulsar::Database::load",
                   use_filename + " is not a database file");
    old_style = true;
  }

  path = temp;

  if (verbose)
    cerr << "Pulsar::Database::load setting path = "
	 << path << endl;

  unsigned count = 0;

  if (!old_style)
    fscanf (fptr, "Pulsar::Database # of entries = %d\n", &count);
  else
    fscanf (fptr, "Pulsar::Calibration::Database # of entries = %d\n", &count);

  if (verbose)
    cerr << "Pulsar::Database::loading " << count << " entries" << endl;

  Entry entry;

  while (fgets (temp, 4096, fptr)) try
  {
    if (verbose)
      cerr << "Pulsar::Database::load '"<< temp << "'" << endl;

    entry.load (temp);
    add (entry);
  }
  catch (Error& error)
  {
    cerr << "Pulsar::Database::load discarding entry:" << error << endl;
  }

  if (verbose)
    cerr << "Pulsar::Database::load " << entries.size() << " entries" <<endl;

  fclose (fptr);
}

//! Unloads entire database to file
void Pulsar::Database::unload (const string& filename)
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::Database::unload" 
		 "fopen (" + filename + ")");
  
  fprintf (fptr, "Pulsar::Database::path %s\n", path.c_str());
  fprintf (fptr, "Pulsar::Database # of entries = %u\n", 
	   (unsigned)entries.size());

  string out;
  for (unsigned ie=0; ie<entries.size(); ie++) {
    entries[ie].unload(out);
    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);
}

//! Add the given Archive to the database
void Pulsar::Database::add (const Pulsar::Archive* archive)
{
  if (!archive)
    throw Error (InvalidParam, "Pulsar::Database::add Archive",
		 "null Archive*");
  try
  {
    Entry entry (*archive);
    add (entry);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::Database::add Archive";
  }
}


//! Add the given Archive to the database
void Pulsar::Database::add (Pulsar::Database::Entry& entry) try
{
  // strip the base path name off of the entry filename
  if (entry.filename.substr(0, path.length()) == path)
    entry.filename.erase (0, path.length()+1);
  
  if (entry.time == 0.0)
    throw Error (InvalidParam, "Pulsar::Database::add Entry",
		 entry.filename + " has epoch = 0 (MJD)");

  for (unsigned ie=0; ie < entries.size(); ie++) 
    if (entries[ie] == entry)
    {
      cerr << "Pulsar::Database::add keeping newest of duplicate entries:\n\t"
           << entries[ie].filename << " and\n\t" << entry.filename << endl;
      if ( file_mod_time (get_filename(entry).c_str()) >
	   file_mod_time (get_filename(entries[ie]).c_str()) )
	entries[ie] = entry;
      return;
    }

  entries.push_back (entry);
}
catch (Error& error)
{
  throw error += "Pulsar::Database::add Entry";
}

void Pulsar::Database::all_matching (const Criterion& criterion,
				     vector<Entry>& matches) const
{
  if (verbose)
    cerr << "Pulsar::Database::all_matching " << entries.size()
         << " entries" << endl;
  
  for (unsigned j = 0; j < entries.size(); j++)   
    if (criterion.match (entries[j]))
      matches.push_back(entries[j]);
}

Pulsar::Database::Entry 
Pulsar::Database::best_match (const Criterion& criterion) const
{
  if (verbose)
    cerr << "Pulsar::Database::best_match " << entries.size()
	 << " entries" << endl;
  
  Entry best_match;
  
  for (unsigned ient = 0; ient < entries.size(); ient++)
    if (criterion.match (entries[ient]))
      best_match = criterion.best (entries[ient], best_match);

  if (best_match.obsType == Signal::Unknown)
    throw Error (InvalidParam, "Pulsar::Calibration::Database::best_match",
                 "no match found");

  return best_match;
}

Pulsar::Database::Entry 
Pulsar::Database::Criterion::best (const Entry& a, const Entry& b) const
{
  double a_diff = fabs( (a.time - entry.time).in_minutes() );
  double b_diff = fabs( (b.time - entry.time).in_minutes() );

  if (a_diff < b_diff)
    return a;
  else
    return b;
}

static Pulsar::Database::Criterion* default_criterion = 0;

//! Get the default matching criterion for PolnCal observations
Pulsar::Database::Criterion
Pulsar::Database::get_default_criterion ()
{
  if (!default_criterion)
    default_criterion = new Criterion;

  return *default_criterion;
}

void Pulsar::Database::set_default_criterion (const Criterion& criterion)
{
  if (!default_criterion)
    default_criterion = new Criterion;

  *default_criterion = criterion;
}




Pulsar::Database::Criterion
Pulsar::Database::criterion (const Pulsar::Archive* arch,
			     Signal::Source obsType) const
try {

  Criterion criterion = get_default_criterion();

  if (obsType == Signal::FluxCalOn ||
      obsType == Signal::FluxCalOff)
  {

    criterion.minutes_apart = long_time_scale;
    criterion.check_coordinates = false;
    criterion.check_instrument = false;
    criterion.policy = NoPolicy;

  }
  else
    criterion.minutes_apart = short_time_scale;

  if (arch)
    criterion.entry = Entry (*arch);
  else
    criterion.no_data ();

  criterion.entry.obsType = obsType;

  return criterion;

}
catch (Error& error)
{
  throw error += "Pulsar::Database::criterion Signal::Source";
}

//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Criterion
Pulsar::Database::criterion (const Pulsar::Archive* arch, 
			     const Calibrator::Type* calType) const
try {

  Criterion criterion = get_default_criterion();

  if (calType->is_a<CalibratorTypes::Flux>() || 
      calType->is_a<CalibratorTypes::CompleteJones>())
  {
    criterion.minutes_apart = long_time_scale;

    // these solutions are global
    criterion.check_coordinates = false;

    // in principle, these solutions are indepenent of backend
    criterion.check_instrument = false;

    criterion.policy = NoPolicy;
  }
  else
    criterion.minutes_apart = short_time_scale;

  if (arch)
    criterion.entry = Entry (*arch);
  else
    criterion.no_data ();

  criterion.entry.obsType = Signal::Calibrator;
  criterion.entry.calType = calType;

  return criterion;
}
catch (Error& error)
{
  throw error += "Pulsar::Database::criterion Calibrator::Type";
}






/*! This routine is given a pointer to a Pulsar::Archive. It scans the
  database for all FluxCal observations that were made within one month
  of the observation mid-time and uses these to construct a
  Pulsar::FluxCalibrator class which can then be used to perform a
  flux calibration of the original pulsar observation.
*/      
Pulsar::FluxCalibrator* 
Pulsar::Database::generateFluxCalibrator (Archive* arch, bool allow_raw) try {

  Entry match = best_match (criterion(arch, new CalibratorTypes::Flux));
  Reference::To<Archive> archive = Archive::load( get_filename(match) );
  return new FluxCalibrator (archive);

}
catch (Error& error)
{  
  if (verbose)
    cerr << "Pulsar::Database::generateFluxCalibrator failure"
      " generating processed FluxCal\n" << error.get_message() << endl;
  
  if (allow_raw)
    return rawFluxCalibrator (arch);
  
  else
    throw error += "Pulsar::Database::generateFluxCalibrator";
}

Pulsar::FluxCalibrator* 
Pulsar::Database::rawFluxCalibrator (Pulsar::Archive* arch)
{
   vector<Pulsar::Database::Entry> oncals;
  all_matching (criterion (arch, Signal::FluxCalOn), oncals);

  if (!oncals.size())
    throw Error (InvalidState, 
                 "Pulsar::Database::generateFluxCalibrator",
                 "no FluxCalOn observations found to match observation");

  vector<Pulsar::Database::Entry> offcals;
  all_matching (criterion (arch, Signal::FluxCalOff), offcals);

  if (!offcals.size())
    throw Error (InvalidState,
                 "Pulsar::Database::generateFluxCalibrator",
                 "no FluxCalOff observations found to match observation");


  Reference::To<FluxCalibrator> fluxcal = new FluxCalibrator;
  Reference::To<Archive> archive;

  unsigned ifile = 0;

  for (ifile=0; ifile < oncals.size(); ifile++)
    fluxcal->add_observation (Archive::load( get_filename(oncals[ifile]) ));

  for (ifile=0; ifile < oncals.size(); ifile++)
    fluxcal->add_observation (Archive::load( get_filename(offcals[ifile]) ));

  return fluxcal.release();
}

template<class Container>
double get_centre_frequency (const Container* container, unsigned ichan)
{
  return container->get_centre_frequency (ichan);
}

double get_centre_frequency (const Pulsar::Archive* archive, unsigned ichan)
{
  return archive->get_Integration(0)->get_centre_frequency (ichan);
}

template<class Container, class TagAlong>
void remove_channels (const Pulsar::Archive* arch, 
                      Container* super, TagAlong* other = 0)
{
  Pulsar::ChannelSubsetMatch chan_match;

  if (Pulsar::Database::verbose)
    cerr << "Pulsar::Database::generatePolnCalibrator " 
	 << "BW mismatch, trying channel truncation... " << endl;

  unsigned nremoved = 0;

  // Loop over polcal channels
  for (unsigned ichan=0; ichan<super->get_nchan(); ichan++)
  {
    DEBUG("remove_channels ichan=" << ichan);

    double freq = get_centre_frequency (super, ichan);

    // Try to match them to archive channels
    try
    {
      chan_match.match_channel (arch->get_Integration(0), freq);
    }
    catch (...)
    {
      DEBUG("no match for " << ichan << " of " << super->get_nchan());
      // If no match, delete it
      super->remove_chan(ichan,ichan);
      if (other)
        other->remove_chan(ichan,ichan);
      ichan--;
      nremoved++;
    }
  }

  if (Pulsar::Database::verbose) 
    cerr << "Pulsar::Database::generatePolnCalibrator removed " 
	 << nremoved << " channels." << endl;
}

//! Removes channels from calarch if necessary
void match_channels (Pulsar::Archive* calarch, const Pulsar::Archive* arch)
{
  if ( calarch->get_bandwidth() == arch->get_bandwidth()
       || calarch->get_nchan() <= arch->get_nchan()
       || arch->get_nchan() == 0 )
    return;

  CalibratorStokes* calstokes = calarch->get<CalibratorStokes>();
  CalibratorExtension* ext = calarch->get<CalibratorExtension>();
  if (ext)
    remove_channels (arch, ext, calstokes);

  if (calarch->get_nsubint())
    remove_channels (arch, calarch, (Pulsar::Archive*) 0);
  
  calarch->set_centre_frequency( arch->get_centre_frequency() );
  calarch->set_bandwidth( arch->get_bandwidth() );
}

/*! This routine is given a pointer to a Pulsar::Archive. It scans the
  database for all PolnCal observations from the same part of sky that
  were made within 8 hours of the observation mid-time and uses these
  to construct a Pulsar::PolnCalibrator class which can then be used
  to perform a polarisation calibration of the original pulsar
  observation. */

Pulsar::PolnCalibrator* 
Pulsar::Database::generatePolnCalibrator (Archive* arch,
					  const Calibrator::Type* type)
{
  if (!arch)
    throw Error (InvalidParam, "Database::generatePolnCalibrator",
		 "no Pulsar::Archive given");
  
  if (verbose)
    cerr << "Pulsar::Database::generatePolnCalibrator type="
	 << type->get_name() << endl;

  Entry entry;

  //
  // unless a CompleteJones transformation is requested,
  // try loading a raw PolnCal observation
  //
  if (! type->is_a<CalibratorTypes::CompleteJones>()) try
  {
    if (verbose)
      cerr << "Pulsar::Database::generatePolnCalibrator search for " 
	"Signal::PolnCal match" << endl;
    entry = best_match (criterion (arch, Signal::PolnCal));
  }
  catch (Error& error)
  {
    //
    // the Hybrid transformations require access to a raw PolnCal observation
    //
    if (type->is_a<CalibratorTypes::Hybrid>())
    {
      error << "\n\tHybrid Calibrator requires raw PolnCal observation";
      throw error += "Pulsar::Database::generatePolnCalibrator";
    }
  }

  if (! type->is_a<CalibratorTypes::Hybrid>()) try
  {
    if (verbose)
      cerr << "Pulsar::Database::generatePolnCalibrator search for " 
	   << type->get_name() << " match" << endl;
    
    Criterion cal_criterion = criterion (arch, type);
    Entry cal_entry = best_match (cal_criterion);
    entry = cal_criterion.best (entry, cal_entry);
  }
  catch (Error& error)
  {
    if (entry.obsType == Signal::Unknown)
    {
      error << "\n\tneither raw nor processed calibrator archives found";
      throw error += "Pulsar::Database::generatePolnCalibrator";
    }
  }

  if (cache_last_cal && lastPolnCal && entry == lastEntry)
  {
    if (verbose)
      cerr << "Pulsar::Database::generatePolnCalibrator using cached calibrator\n";
    return lastPolnCal;
  }

  if (verbose)
    cout << "Pulsar::Database::generatePolnCalibrator constructing from file "
	 << entry.filename << endl;

  Reference::To<Pulsar::Archive> polcalarch;
  polcalarch = Pulsar::Archive::load( get_filename(entry) );

  match_channels (polcalarch, arch);

  if (feed)
  {
    FeedExtension* feed_ext = polcalarch->getadd<FeedExtension>();
    feed_ext -> set_transformation ( feed->evaluate() );
  }

  if (verbose > 2)
  {
    if (entry.obsType == Signal::Calibrator)
      cerr << "CAL OF TYPE " <<  entry.calType->get_name() << endl;
    else
      cerr << "FILE OF TYPE " << Signal::Source2string (entry.obsType) << endl;
  }

  if (entry.obsType == Signal::Calibrator)
    // if a solved model, return the solution
    return new Pulsar::PolnCalibrator (polcalarch);

  // otherwise, construct a solution
  Reference::To<Pulsar::ReferenceCalibrator> ref_cal;

  if ( type->is_a<CalibratorTypes::Hybrid>() ||
       type->is_a<CalibratorTypes::SingleAxis>() )
    ref_cal = new Pulsar::SingleAxisCalibrator (polcalarch);

  else if ( type->is_a<CalibratorTypes::van02_EqA1>() )
    ref_cal = new Pulsar::PolarCalibrator (polcalarch);

  else if ( type->is_a<CalibratorTypes::DoP>() )
    ref_cal = new Pulsar::DoPCalibrator (polcalarch);

  else if ( type->is_a<CalibratorTypes::OffPulse>() )
    ref_cal = new Pulsar::OffPulseCalibrator (polcalarch);

  else
    cerr << "Pulsar::Database::generatePolnCalibrator"
      " unknown type=" << type->get_name() << endl;
  
  if ( type->is_a<CalibratorTypes::Hybrid>() )
  {
    cerr << "Pulsar::Database::generatePolnCalibrator Hybrid" << endl;
    return generateHybridCalibrator (ref_cal, arch);
  }

  // store last calibrator, if we are caching
  if (cache_last_cal)
  {
    if (verbose)
      cerr << "Pulsar::Database::generatePolnCalibrator caching PolnCalibrator" << endl;
    lastEntry = entry;
    lastPolnCal = ref_cal;
  }

  return ref_cal.release();
}

Pulsar::HybridCalibrator* 
Pulsar::Database::generateHybridCalibrator (ReferenceCalibrator* arcal,
					    Archive* arch)
{
  if (!arch) throw Error (InvalidParam,
			  "Database::generateHybridCalibrator",
			  "no Pulsar::Archive given");
  if (!arcal) throw Error (InvalidParam,
			    "Database::generateHybridCalibrator",
			    "no Pulsar::ReferenceCalibrator given");
 
  if (verbose)
    cerr << "Pulsar::Database::generateHybridCalibrator" << endl;

  Entry entry;

  try
  {
    if (verbose)
      cerr << "  Attempting to find a matching Phenomenological Model" << endl;

    entry = best_match (criterion(arch, new CalibratorTypes::CompleteJones));
  }
  catch (Error& error)
  {
    throw Error (InvalidState, "Pulsar::Database::generateHybridCalibrator",
		 "No complete parameterization (e.g. pcm output) found");
  }

  Reference::To<Pulsar::HybridCalibrator> hybrid;

  try
  {
    Reference::To<Pulsar::Archive> polncalarch;
    polncalarch = Pulsar::Archive::load(get_filename(entry));

    match_channels (polncalarch, arch);

    Reference::To<Pulsar::PolnCalibrator> polncal;
    polncal = new Pulsar::PolnCalibrator (polncalarch);

    Reference::To<Pulsar::CalibratorStokes> calstokes;
    calstokes = polncalarch->get<CalibratorStokes>();

    hybrid = new Pulsar::HybridCalibrator (polncal);

    if (calstokes)
      hybrid->set_reference_input (calstokes);
    hybrid->set_reference_observation (arcal);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::Database::generateHybridCalibrator";
  }

  return hybrid.release();

}

//! Returns the full pathname of the Entry filename
string Pulsar::Database::get_filename (const Entry& entry) const
{
  if (entry.filename[0] == '/')
    return entry.filename;
  else
    return path + "/" + entry.filename;
}

//! Returns the full pathname of the Entry filename
string Pulsar::Database::get_path () const
{
  return path;
}
