/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Parameters.h"

// Extensions this program understands

#include "Pulsar/ProcHistory.h"
#include "Pulsar/Backend.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/Dispersion.h"
#include "Pulsar/FaradayRotation.h"
#include "Pulsar/ReflectStokes.h"

#include "Pulsar/counter_drift.h"

#include "load_factory.h"
#include "dirutil.h"
#include "strutil.h"
#include "Error.h"

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <new>

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <Pulsar/Archive.h>
#include <Pulsar/ArchiveExpert.h>
#include <vector>



using std::vector;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::bad_alloc;
using std::exception;
using Pulsar::Archive;

/**
 * Number of possible DM entries in the input ephemeris
 * format: DMX
 * where X = [1-9]
 */
const unsigned NUMBER_OF_DM_KEYWORDS = 9;

/**
 * @brief Create a string listing the names of the registered Archive classes
 * @return a coma separated list of archive class names
 *
 * The list of registered classes will vary depending on what is compiled into
 * psrchive. So instead of having a static list, we query the registry to find
 * out what classes were actually compiled.
 **/

string GetArchiveTypes( void )
{
  vector< pair<string,string> > agents;
  string agents_string;

  Archive::Expert::get_agent_list( agents );

  if( agents.size() > 0 )
  {
    for( unsigned i = 0; i < agents.size() -1; i ++ )
    {
      agents_string += agents[i].first;
      agents_string += string(",");
    }
    agents_string += agents[ agents.size() - 1 ].first;
  }

  return agents_string;
}

//! Returns the difference between the start of observation and TEPOCH
double get_delta_time(Pulsar::Archive* archive,
        const MJD tepoch);

//! Populate the dms vector with the input DM[1-9] values
void get_dms(vector<double>& dms, const Pulsar::Archive* archive);

//! Returns the input value of 'DMEPOCH' (if it exists), otherwise, 'PEPOCH'
MJD get_tepoch(const Pulsar::Archive* archive);

//! Returns the input value of 'DM'
double get_dm(const Pulsar::Archive* archive);

//! Uses the input DM and DM[1-9] values to calculate the new DM
double calculate_new_dm(vector<double>& dms, const double dm,
        const double delta_time);

//! Calculates new DM from input ephemeris and sets it in the loaded Archive
void update_dm(Pulsar::Archive* archive);

//! Correct the absolute auxiliary Farday rotation
void correct_auxiliary_rm (Pulsar::Archive*, double aux_rm);

//! Populates the keywords vector
//  "DM[1-9]"
void get_dm_keywords(vector<string>& keywords);

void usage()
{
  cout << "A program for manipulating Pulsar::Archives \n"
    "Usage: pam [options] filenames \n"
    "  -q               Quiet mode \n"
    "  -v               Verbose mode \n"
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "  -m               Modify the original files on disk \n"
    "  -M metafile      List of archive filenames in metafile \n"
    "  -a archive       The archive class to create valid types are \n"
    "                   (" << GetArchiveTypes() << ") \n"
    "  -e extension     Write new files with this extension \n"
    "  -u path          Write files to this location \n"
    "  -T               Time scrunch to one subint \n"
    "  -F               Frequency scrunch to one channel \n"
    "  -p               Polarisation scrunch to total intensity \n"
    "  -D               Dedisperse (but not fscrunch) \n"
    "  --DD             Dededisperse (i.e. undo -D option) \n"
    "  -I               Transform to Invariant Interval \n"
    "  -S               Transform to Stokes parameters \n"
    "  --SS             Transform to coherence parameters (i.e. undo -S option)\n"
    "  -x \"start end\"   Extract subints in this inclusive range \n"
    "\n"
    "The following options take integer arguments \n"
    "  -t               Time scrunch by this factor \n"
    "  -f               Frequency scrunch by this factor \n"
    "  -b               Bin scrunch by this factor \n"
    "  --setnsub        Time scrunch to this many subints \n"
    "  --settsub        Time scrunch to this subint length \n"
    "  --setnchn        Frequency scrunch to this many channels \n"
    "  --setnbin        Bin scrunch to this many bins \n"
    "  --binphsperi     Convert to binary phase periastron order \n"
    "  --binphsasc      Convert to binary phase asc node order \n"
    "  --binlngperi     Convert to binary longitude periastron order \n"
    "  --binlngasc      Convert to binary longitude asc node order \n"
    "\n"
    "The following options take floating point arguments \n"
    "  -d DM            Alter the header dispersion measure \n"
    "  -R RM            Correct for ISM faraday rotation \n"
    "  --RM RM          Install a new RM but don't defaraday \n"
    "  --aux_rm RMi    Correct absolute auxiliary Faraday rotation \n"
    "  -s               Smear with this duty cycle \n"
    "  -r               Rotate profiles by this many turns \n" 
    "  -w               Reset profile weights to this value \n"
    "  --mult fac       Multiply amplitudes by 'fac'\n"
    "  --period period  Install a new folding period\n"
    "\n"
    "The following options override archive parameters \n"
    "  --receiver file  Install the Receiver described in the text file \n"
    "  -L               Set feed basis to Linear \n"
    "  -C               Set feed basis to Circular \n"
    "  -E ephfile       Install a new ephemeris and update model \n"
    "  -B               Flip the sideband sense \n"
    "  --flip freq      Flip the band about the given frequency \n"
    "  --reverse_freqs  Reverse the ordering of the frequency channels and\n"
    "                   change the bandwidth flag accordingly\n"
    "  -o centre_freq   Change the frequency labels \n"
    "  --type type      Change the 'type' parameter where 'type' is one of:\n"
    "                   'Pulsar', 'PolnCal', 'FluxCalOn', 'FluxCalOff', 'Calibrator'\n"
    "  --inst inst      Change the instrument name \n"
    "  --site site      Correct 'site' of telescope (One letter tempo code- GBT=1, PKS=7 etc)\n"
    "  --name name      Change source name\n"
    "  --update_dm      Replace header DM with current value from input ephemeris\n"

    "\n"
    "See " PSRCHIVE_HTTP "/manuals/pam for more details\n"
       << endl;

  return;
}

// PAM: A command line tool for modifying archives

void smear (Pulsar::Profile*, float duty_cycle);

int main (int argc, char *argv[]) try {
  
    bool verbose = false;
    char* metafile = 0;
 
    string ulpath;

    bool save = false;
    string ext;
  
    bool tscr = false;
    int tscr_fac = 0;

    bool fscr = false;
    int fscr_fac = 0;

    bool bscr = false;
    int bscr_fac = 0;

    bool newdm = false;
    double dm = 0.0;

    bool scattered_power_correction = false;

    bool defaraday = false;

    bool newrm = false;
    double rm = 0.0;

    bool reset_weights = false;
    float new_weight = 1.0;

    float smear_dc = 0.0;

    bool rotate = false;
    double rphase = 0.0;

    bool dedisperse = false;
    bool dededisperse = false;

    bool pscr = false;

    bool invint = false;

    bool stokesify = false;
    bool unstokesify = false;

    bool flipsb = false;
    bool flip_freq = false;
    double flip_freq_mhz = 0.0;

    Pulsar::Parameters* new_eph = 0;

    string command = "pam";

    char* archive_class = 0;

    int new_nchn = 0;
    int new_nsub = 0;
    int new_nbin = 0;

    float tsub = 0.0;

    bool circ = false;
    bool lin = false;

    unsigned ronsub = 0;
    bool cbppo = false;
    bool cbpao = false;
    bool cblpo = false;
    bool cblao = false;

    int subint_extract_start = -1;
    int subint_extract_end = -1;

    bool new_cfreq = false;
    double new_fr = 0.0;
    Signal::Source new_type = Signal::Unknown;
    string instrument;
    bool reverse_freqs = false;
    string site;
    string name;
    float mult = -1.0;
    double new_folding_period = -1.0;

    bool update_dm_from_eph = false;
    double aux_rm = 0.0;

    Reference::To<Pulsar::IntegrationOrder> myio;
    Reference::To<Pulsar::Receiver> install_receiver;

    Pulsar::ReflectStokes reflections;

    int c = 0;

    const int TYPE = 1208;
    const int INST = 1209;
    const int REVERSE_FREQS = 1210;
    const int SITE = 1211;
    const int NAME = 1212;
    const int DD   = 1213;
    const int RR   = 1214;
    const int SPC  = 1215;
    const int RM   = 1216;
    const int MULT = 1218;
    const int PERIOD=1219;
    const int SS   = 1220;
    const int FLIP = 1221;
    const int UPDATE_DM = 1222;
    const int AUX_RM = 1223;

    while (1) {

      int options_index = 0;

      static struct option long_options[] = {
	{"setnchn",    1, 0, 200},
	{"setnsub",    1, 0, 201},
	{"setnbin",    1, 0, 202},
	{"binphsperi", 1, 0, 203},
	{"binphsasc",  1, 0, 204},
	{"binlngperi", 1, 0, 205},
	{"binlngasc",  1, 0, 206},
	{"receiver",   1, 0, 207},
	{"settsub",    1, 0, 208},
	{"type",       1, 0, TYPE},
	{"inst",       1, 0, INST},
	{"reverse_freqs",no_argument,0,REVERSE_FREQS},
	{"flip",       1 ,0, FLIP},
	{"site",       1, 0, SITE},
	{"name",       1, 0, NAME},
	{"DD",         no_argument,      0,DD},
	{"RR",         no_argument,      0,RR},
	{"RM",         required_argument,0,RM},
	{"spc",        no_argument,      0,SPC},
	{"mult",       required_argument,0,MULT},
	{"period",     required_argument,0,PERIOD},
	{"SS",         no_argument,      0,SS},
	{"update_dm",   no_argument,      0,UPDATE_DM},
	{"aux_rm",    required_argument,0,AUX_RM},
	{0, 0, 0, 0}
      };

      c = getopt_long(argc, argv, "hqvViM:mn:a:e:E:TFpIt:f:b:d:o:s:r:u:w:DSBLCx:R:",
		      long_options, &options_index);

      if (c == -1)
	break;

      switch (c) {
      case 'h':
	usage();
	return (0);
	break;
      case 'q':
	Pulsar::Archive::set_verbosity(0);
	break;
      case 'v':
	verbose = true;
	Pulsar::Archive::set_verbosity(2);
	break;
      case 'V':
	verbose = true;
	Pulsar::Archive::set_verbosity(3);
	break;
      case 'i':
	cout << "$Id: pam.C,v 1.101 2010/10/05 23:59:50 jonathan_khoo Exp $" << endl;
	return 0;
      case 'm':
	save = true;
	break;
      case 'M':
        metafile = optarg;
        break;
      case 'L':
	lin = true;
	break;
      case 'C':
	circ = true;
	break;
      case 'a':
	archive_class = optarg;
	break;
      case 'e':
	ext = optarg;
	if( !ext.empty() )
	  save = true;
	break;
      case 'E':

	try {
	  new_eph = factory<Pulsar::Parameters> (optarg);
	}
	catch (Error& error) {
	  cerr << "Could not load new ephemeris from " << optarg << endl;
	  return -1;
	}

	command += " -E";
	break;
      case 'T':
	tscr = true;
	command += " -T";
	break;
      case 'F':
	fscr = true;
	command += " -F";
	break;
      case 'p':
	pscr = true;
	command += " -p";
	break;
      case 'I':
	invint = true;
	pscr = false;
	command += " -I";
	break;
      case 'f':
	fscr = true;
	if (sscanf(optarg, "%d", &fscr_fac) != 1) {
	  cout << "That is not a valid fscrunch factor" << endl;
	  return -1;
	}
	command += " -f ";
	command += optarg;
	break;
	
      case 'n':

	reflections.add_reflection( optarg[0] );

	command += " -n ";
	command += optarg;
	break;

      case 'o':
	new_cfreq = true;
	if (sscanf(optarg, "%lf", &new_fr) != 1) {
	  cout << "That is not a valid centre frequency" << endl;
	  return -1;
	}
	command += " -o ";
	command += optarg;
	break;
      case 't':
	tscr = true;
	if (sscanf(optarg, "%d", &tscr_fac) != 1) {
	  cout << "That is not a valid tscrunch factor" << endl;
	  return -1;
	}
	command += " -t ";
	command += optarg;
	break;
      case 'b':
	bscr = true;
	if (sscanf(optarg, "%d", &bscr_fac) != 1) {
	  cout << "That is not a valid bscrunch factor" << endl;
	  return -1;
	}
	if (bscr_fac <= 0) {
	  cout << "That is not a valid bscrunch factor" << endl;
	  return -1;
	}
	command += " -b ";
	command += optarg;
	break;
      case 'd':
	newdm = true;
	if (sscanf(optarg, "%lf", &dm) != 1) {
	  cout << "That is not a valid dispersion measure" << endl;
	  return -1;
	}
	command += " -d ";
	command += optarg;
	break;
      case 'D':
	dedisperse = true;
	command += " -D ";
	break;
      case 'R':
	if (sscanf(optarg, "%lf", &rm) != 1) {
	  cout << "That is not a valid rotation measure" << endl;
	  return -1;
	}
	newrm = true;
	defaraday = true;
	command += " -R ";
	command += optarg;
	break;
      case 's':
	if (sscanf(optarg, "%f", &smear_dc) != 1) {
	  cout << "That is not a valid smearing duty cycle" << endl;
	  return -1;
	}
	command += " -s ";
	command += optarg;
	break;
      case 'r':
	rotate = true;
	if (sscanf(optarg, "%lf", &rphase) != 1) {
	  cout << "That is not a valid rotation phase" << endl;
	  return -1;
	}
	if (rphase <= -1.0 || rphase >= 1.0) {
	  cout << "That is not a valid rotation phase" << endl;
	  return -1;
	}
	command += " -r ";
	command += optarg;
	break;
      case 'u':
	ulpath = optarg;
	if( !ulpath.empty() )
	{
	  save = true;
	  if (ulpath.substr(ulpath.length()-1,1) != "/")
	    ulpath += "/";
	}
	break;
      case 'w':
	reset_weights = true;
	if (sscanf(optarg, "%f", &new_weight) != 1) {
	  cout << "That is not a valid weight" << endl;
	  return -1;
	}
	command += " -w ";
	command += optarg;
	break;
      case 'S':
	stokesify = true;
	break;
      case SS:
        unstokesify = true;
        break;
      case 'B':
	flipsb = true;
	break;
      case 'x' :
	if (sscanf(optarg, "%d %d", 
		   &subint_extract_start, &subint_extract_end) !=2 ) {
	  cout << "That is not a valid subint range" << endl;
	  return -1;
	}
	subint_extract_end++;
	break;
      case 200:
	fscr = true;
	if (sscanf(optarg, "%d", &new_nchn) != 1) {
	  cout << "That is not a valid number of channels" << endl;
	  return -1;
	}
	if (new_nchn <= 0) {
	  cout << "That is not a valid number of channels" << endl;
	  return -1;
	}
	command += " --setnchn ";
	command += optarg;
	break;
      case 201:
	tscr = true;
	if (sscanf(optarg, "%d", &new_nsub) != 1) {
	  cout << "That is not a valid number of subints" << endl;
	  return -1;
	}
	if (new_nsub <= 0) {
	  cout << "That is not a valid number of subints" << endl;
	  return -1;
	}
	command += " --setnsub ";
	command += optarg;
	break;
      case 202:
	bscr = true;
	if (sscanf(optarg, "%d", &new_nbin) != 1) {
	  cout << "That is not a valid number of bins" << endl;
	  return -1;
	}
	if (new_nbin <= 0) {
	  cout << "That is not a valid number of bins" << endl;
	  return -1;
	}
	command += " --setnbin ";
	command += optarg;
	break;
      case 203: {
	if (cbpao || cblpo || cblao) {
	  cerr << "You can only specify one re-ordering scheme!"
	       << endl;
	  return -1;
	}
	if (sscanf(optarg, "%ud", &ronsub) != 1) {
	  cerr << "Invalid nsub given" << endl;
	  return -1;
	}
	cbppo = true;
	break;
      } 
      case 204: {
	if (cbppo || cblpo || cblao) {
	  cerr << "You can only specify one re-ordering scheme!"
	       << endl;
	  return -1;
	}
	if (sscanf(optarg, "%ud", &ronsub) != 1) {
	  cerr << "Invalid nsub given" << endl;
	  return -1;
	}
	cbpao = true;
	break;
      }      
      case 205: {
	if (cblao || cbppo || cbpao) {
	  cerr << "You can only specify one re-ordering scheme!"
	       << endl;
	  return -1;
	}
	if (sscanf(optarg, "%ud", &ronsub) != 1) {
	  cerr << "Invalid nsub given" << endl;
	  return -1;
	}
	cblpo = true;
	break;
      } 
      case 206: {
	if (cblpo || cbppo || cbpao) {
	  cerr << "You can only specify one re-ordering scheme!"
	       << endl;
	  return -1;
	}
	if (sscanf(optarg, "%ud", &ronsub) != 1) {
	  cerr << "Invalid nsub given" << endl;
	  return -1;
	}
	cblao = true;
	break;
      }

      case 207: try {
	install_receiver = Pulsar::Receiver::load (optarg);
	break;
      }
      catch (Error& error) {
	cerr << "pam: Error loading Receiver from " << optarg << endl
	     << error.get_message() << endl;
	return -1;
      }

      case 208: {
	if (sscanf(optarg, "%f", &tsub) != 1) {
	  cerr << "Invalid tsub given" << endl;
	  return -1;
	}
	tscr = true;
	break;
      }

      case TYPE:
	{
	  string s = optarg;
	  if(s=="Pulsar")     new_type = Signal::Pulsar;
	  else if(s=="PolnCal")    new_type = Signal::PolnCal;
	  else if(s=="FluxCalOn")  new_type = Signal::FluxCalOn;
	  else if(s=="FluxCalOff") new_type = Signal::FluxCalOff;
	  else if(s=="Calibrator") new_type = Signal::Calibrator;
	  else{
	    fprintf(stderr,"Unrecognised argument to --type: '%s'\n",optarg);
	    exit(-1);
	  }
	  command += " --type " + s;
	}
	break;

      case INST: instrument = optarg; break;

      case REVERSE_FREQS: reverse_freqs = true; break;

      case SITE: site = optarg; break;

      case NAME: name = optarg; break;

      case DD: dededisperse = true; break;

      case RM:
        aux_rm = fromstring<double>(optarg);
        newrm = true;
        command += " --RM ";
        command += optarg;
        break;

      case SPC: scattered_power_correction = true; break;
	
      case MULT: mult = atof(optarg); break;

      case PERIOD: new_folding_period = fromstring<double>(optarg); break;

      case FLIP: flip_freq = true; flip_freq_mhz = atof(optarg); break;

      case UPDATE_DM: update_dm_from_eph = true; break;

      case AUX_RM:
        aux_rm = fromstring<double>(optarg);
        command += " --aux_rm ";
        command += optarg;
        break;

      default:
	cout << "Unrecognised option" << endl;
      }
    }

   if (verbose)
     cerr << "pam: parsing filenames" << endl;
 
   vector <string> filenames;

    if (metafile)
      stringfload (&filenames, metafile);
    else
      for (int ai=optind; ai<argc; ai++)
        dirglob (&filenames, argv[ai]);
 
    if (filenames.empty())
    {
      cerr << "pam: no filenames were specified" << endl;
      exit(-1);
    } 
  
    Reference::To<Pulsar::Archive> arch;

    if (!save)
    {
      cout << "Changes will not be saved. Use -m, -u or -e to write results to disk"
	   << endl;
    }

    if (stokesify && unstokesify)
    {
      cerr << "pam: Both -S and --SS options were given.  Poln state will not be changed!" << endl;
      stokesify = false;
      unstokesify = false;
    }

    int flip_option_count=0;
    if (flipsb) flip_option_count++;
    if (flip_freq) flip_option_count++;
    if (reverse_freqs) flip_option_count++;
    if (flip_option_count > 1) {
      cerr << "pam: More than one band-flip option was given, exiting." << endl;
      exit(-1);
    }

    for (unsigned i = 0; i < filenames.size(); i++) try
    {
      if (verbose)
	cerr << "Loading " << filenames[i] << endl;
      
      arch = Pulsar::Archive::load(filenames[i]);

      if( mult > 0.0 ){
	for( unsigned isub=0; isub<arch->get_nsubint();isub++)
	  for( unsigned ichan=0; ichan<arch->get_nchan();ichan++)
	    for( unsigned ipol=0; ipol<arch->get_npol();ipol++)
	      arch->get_Profile(isub,ipol,ichan)->scale( mult );
      }

      if( new_folding_period > 0.0 ){
	Pulsar::counter_drift( arch, new_folding_period, 0.0);
	for( unsigned isub=0; isub<arch->get_nsubint();isub++)
	  arch->get_Integration(isub)->set_folding_period( new_folding_period );
      }

      if (install_receiver) {
	if (verbose)
	  cerr << "pam: Installing receiver: " << install_receiver->get_name()
	       << " in archive" << endl;

	arch->add_extension (install_receiver);
      }

      if (lin || circ) {
	Pulsar::Receiver* receiver = arch->get<Pulsar::Receiver>();

	if (!receiver)
	  cerr << "No Receiver Extension in " << filenames[i] << endl;
	else {
	  if (lin) {
	    receiver->set_basis (Signal::Linear);
	    cout << "Feed basis set to Linear" << endl;
	  }

	  if (circ) {
	    receiver->set_basis (Signal::Circular);
	    cout << "Feed basis set to Circular" << endl;
	  }
	}
      }

      reflections.transform( arch );

      if (new_cfreq)
      {
	double nc = arch->get_nchan();
	double bw = arch->get_bandwidth();
	double cw = bw / nc;

	double fr = new_fr - (bw / 2.0) + (cw / 2.0);
	
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    arch->get_Integration(i)->set_centre_frequency(j,(fr + (j*cw)));
	  }
	}
	
	arch->set_centre_frequency(new_fr);
      }

      if( new_type != Signal::Unknown )
	arch->set_type( new_type );

      if( instrument != string() ){
	Pulsar::Backend* b = arch->get<Pulsar::Backend>();
	if( !b )
	  fprintf(stderr,"Could not change instrument name- archive does not have Backend extension\n");
	else
	  b->set_name(instrument);
      }

      if( site != string() )
	arch->set_telescope( site );

      if( name != string() )
	arch->set_source( name );

      if (new_eph) try
      {
        arch->set_ephemeris(new_eph);

        if (update_dm_from_eph) {
          update_dm(arch);
        }
      }
      catch (Error& error)
      {
	cerr << "Error while installing new ephemeris: " 
	     << error.get_message() << endl;
        continue;
      }

      if (flipsb) {
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  vector<double> labels;
	  labels.resize(arch->get_nchan());
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    labels[j] = arch->get_Integration(i)->get_centre_frequency(j);
	  }
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    double new_frequency = labels[labels.size()-1-j];
	    arch->get_Integration(i)->set_centre_frequency(j,new_frequency);
	  }
	}
	arch->set_bandwidth(-1.0 * arch->get_bandwidth());
      }

      if (flip_freq) {
        for (unsigned isub = 0; isub < arch->get_nsubint(); isub++) {
          Reference::To<Pulsar::Integration> 
            subint = arch->get_Integration(isub);
          for (unsigned ichan = 0; ichan < arch->get_nchan(); ichan++) {
            double new_freq = flip_freq_mhz 
              - (subint->get_centre_frequency(ichan) - flip_freq_mhz);
            subint->set_centre_frequency(ichan, new_freq);
          }
        }
        arch->set_bandwidth(-1.0 * arch->get_bandwidth());
      }

      if( reverse_freqs ) {
	// Of course it would be nice to do this with pointers.... but oh well I guess copying will have to do HSK 27/8/04

	unsigned nchan = arch->get_nchan();

	for( unsigned isub=0; isub<arch->get_nsubint(); isub++){
	  for( unsigned ipol =0; ipol<arch->get_npol(); ipol++){
	    for( unsigned ichan=0; ichan<nchan/2; ichan++){
	      Reference::To<Pulsar::Profile> lo = arch->get_Profile(isub,ipol,ichan);	      
	      Reference::To<Pulsar::Profile> tmp = lo->clone();

	      Reference::To<Pulsar::Profile> hi = arch->get_Profile(isub,ipol,nchan-1-ichan);

	      lo->operator=(*hi);
	      hi->operator=(*tmp);
	    }
	  }
	}
	arch->set_bandwidth( -1.0 * arch->get_bandwidth() );
      }

      if (reset_weights) {
	arch->uniform_weight(new_weight);
	if (verbose)
	  cout << "All profile weights set to " << new_weight << endl;
      }
      
      if (rotate)
	arch->rotate_phase (rphase);

      if (scattered_power_correction) {

	Pulsar::ScatteredPowerCorrection spc;
	if (arch->get_state() == Signal::Stokes)
	  arch->convert_state(Signal::Coherence);

	spc.correct (arch);

      }

      if (newdm)
      {
	arch->set_dispersion_measure(dm);
	if (verbose)
	  cout << "Archive dispersion measure set to " << dm << endl;

	if (arch->get_dedispersed())
        {
	  arch->dedisperse();

	  if (verbose)
	    cout << "Archive re-dedipsersed" << endl;
        }
      }

      if (dedisperse)
      {
	arch->dedisperse();
	if (verbose)
	  cout << "Archive dedipsersed" << endl;
      }

      if (dededisperse)
      {
	Pulsar::Dispersion correction;
	correction.revert (arch);
      }

      if (stokesify) {
	if (arch->get_npol() != 4)
	  throw Error(InvalidState, "Convert to Stokes",
		      "Not enough polarisation information");
	arch->convert_state(Signal::Stokes);
	if (verbose)
	  cout << "Archive converted to Stokes parameters" << endl;
      }

      if (unstokesify) {
	if (arch->get_npol() != 4)
	  throw Error(InvalidState, "Convert to coherence",
		      "Not enough polarisation information");
	arch->convert_state(Signal::Coherence);
	if (verbose)
	  cout << "Archive converted to coherence parameters" << endl;
      }

      if (cbppo) {
	myio = new Pulsar::PeriastronOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cbpao) {
	myio = new Pulsar::BinaryPhaseOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cblpo) {
	myio = new Pulsar::BinLngPeriOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cblao) {
	myio = new Pulsar::BinLngAscOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if( subint_extract_start >= 0 && subint_extract_end >= 0 ) {
	vector<unsigned> subints;
	unsigned isub = subint_extract_start;

	while ( isub<arch->get_nsubint() && isub<unsigned(subint_extract_end) ) {
	  subints.push_back( isub );
	  isub++;
	}

	Reference::To<Pulsar::Archive> extracted( arch->extract(subints) );
	extracted->set_filename( arch->get_filename() );

	arch = extracted;
      }

      if (tscr) {
	if (tsub > 0.0) {
	  unsigned factor = 
	    unsigned (tsub / arch->get_Integration(0)->get_duration());
	  if (factor == 0) {
	    cerr << "Warning: subints already too long" << endl;
	  }
	  else {
	    arch->tscrunch(factor);
	  }
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched by a factor of " 
		 << factor << endl;
	}
	else if (new_nsub > 0) {
	  arch->tscrunch_to_nsub(new_nsub);
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched to " 
		 << new_nsub << " subints" << endl;
	}
	else if (tscr_fac > 0) {
	  arch->tscrunch(tscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched by a factor of " 
		 << tscr_fac << endl;
	}
	else {
	  arch->tscrunch();
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched" << endl;
	}
      }
      
      if (pscr) {
	arch->pscrunch();
	if (verbose)
	  cout << arch->get_filename() << " pscrunched" << endl;
      } 

      if (invint) {
	arch->invint();
	if (verbose)
	  cout << arch->get_filename() << " invinted" << endl;
      }

      if (newrm) {
	arch->set_rotation_measure (rm);
	if (verbose)
	  cout << arch->get_filename() << " RM set to " << rm << endl;
      }

      if (defaraday) {
	arch->defaraday();
	if (verbose)
	  cout << arch->get_filename() << " defaradayed" <<endl;
      }

      if (aux_rm)
      {
	if (verbose)
	  cout << "pam: correct auxiliary Faraday rotation; iono RM="
	       << aux_rm << endl;
	correct_auxiliary_rm (arch, aux_rm);
      }

      if (fscr) {
	if (new_nchn > 0) {
	  arch->fscrunch_to_nchan(new_nchn);
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched to " 
		 << new_nchn << " channels" << endl;
	}
	else if (fscr_fac > 0) {
	  arch->fscrunch(fscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched by a factor of " 
		 << fscr_fac << endl;
	}
	else {
	  arch->fscrunch();
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched" << endl;
	}
      }

      if (bscr) {
	if (new_nbin > 0) {
	  arch->bscrunch_to_nbin(new_nbin);
	  if (verbose)
	    cout << arch->get_filename() << " bscrunched to " 
		 << new_nbin << " bins" << endl;
	}
	else {
	  arch->bscrunch(bscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " bscrunched by a factor of " 
		 << bscr_fac << endl;
	}
      }     

      if (smear_dc) {
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_npol(); j++) {
	    for (unsigned k = 0; k < arch->get_nchan(); k++) {
	      smear (arch->get_Profile(i,j,k), smear_dc);
	    }
	  }
	}
      }
      
      if (save) {

	if (archive_class)  {

	  // unload an archive of the specified class
	  Reference::To<Pulsar::Archive> output;
	  output = Pulsar::Archive::new_Archive (archive_class);
	  output -> copy (*arch);
	  output -> set_filename ( arch->get_filename() );

	  arch = output;

	}


	// See if the archive contains a history that should be updated:
	
	Pulsar::ProcHistory* fitsext = arch->get<Pulsar::ProcHistory>();
	
	if (fitsext) {
	  
	  if (command.length() > 80) {
	    cout << "WARNING: ProcHistory command string truncated to 80 chars" 
		 << endl;
	    fitsext->set_command_str(command.substr(0, 80));
	  }
	  else {
	    fitsext->set_command_str(command);
	  }
	  
	}
	
	string out_filename = arch->get_filename();
	
	if( !ext.empty() )
	  out_filename = replace_extension( out_filename, ext );
	
	if( !ulpath.empty() )
	  out_filename = ulpath + basename(out_filename);
	
	arch->unload( out_filename );
	cout << out_filename << " written to disk" << endl;
      }
    }  
    catch (Error& error) {
      cerr << error << endl;
    } 
  
    return 0;

}
catch(Error& er) 
{
  cerr << er << endl;
  return -1;
}
catch (string& error)
{
  cerr << "exception thrown: " << error << endl;
  return -1;
}
catch (bad_alloc& ba)
{
  cerr << "Caught a bad_alloc: '" << ba.what() << "'" << endl ;
  return -1;
}
catch (exception& e)
{
  cerr << "caught an exception of type '" 
				 << typeid(e).name() << "'" << endl; 
  return -1;
}
catch(...)
{
  fprintf(stderr,"Unknown exception caught\n");
  return -1;
}




Pulsar::Profile* hat_profile (int nbin, float duty_cycle)
{
  if (duty_cycle >= 1.0 || duty_cycle <= 0.0)
    throw Error (InvalidParam, 
		 "hat_profile invalid duty cycle");
  
  if (nbin <= 0)
    throw Error (InvalidParam, "hat_profile invalid nbin");
  
  Pulsar::Profile* ptr = new Pulsar::Profile();

  ptr->set_centre_frequency(0.0);
  ptr->set_weight(1.0);

  ptr->resize(nbin);

  for (int i = 0; i < nbin; i++) {
    ptr->get_amps()[i] = 0.0;
  }

  int width = int(float(nbin) * duty_cycle);

  if (nbin % 2 == 0) {

    if (width %2 != 0)
      width += 1;
    
    int start_bin = (nbin / 2) - (width / 2);
    int end_bin = start_bin + (width);

    for (int i = start_bin; i < end_bin; i++) 
      ptr->get_amps()[i] = 1.0;
  }
  else {
    
    if (width %2 == 0)
      width += 1;
    
    int start_bin = ((nbin - 1) / 2) - ((width - 1) / 2);
    int end_bin = start_bin + width;

    for (int i = start_bin; i < end_bin; i++) 
      ptr->get_amps()[i] = 1.0;
  }

  return ptr;
}


void smear (Pulsar::Profile* profile, float duty_cycle)
{
  profile->fft_convolve( hat_profile(profile->get_nbin(), duty_cycle) );
}

double get_delta_time(Pulsar::Archive* archive, const MJD tepoch)
{
  const MJD start = archive->start_time();

  return (start - tepoch).in_days() / 365.25; // convert from days to years
}

MJD get_tepoch(const Pulsar::Archive* archive)
{
  string str = archive->get_ephemeris()->get_value("DMEPOCH");
  if (!str.empty()) {
    return MJD(fromstring<double>(str));
  }

  str = archive->get_ephemeris()->get_value("PEPOCH");
  if (!str.empty()) {
    return MJD(fromstring<double>(str));
  }

  throw Error(InvalidState, "pam - get_tepoch",
      "cannot find DMEPOCH or PEPOCH in new ephermeris");
}

double get_dm(const Pulsar::Archive* archive)
{
  string str = archive->get_ephemeris()->get_value("DM");

  if (!str.empty()) {
    return fromstring<double>(str);
  } else {
    throw Error(InvalidState, "pam - get_dm",
        "cannot find DM value in new ephermeris");
  }
}

void get_dms(vector<double>& dms, const Pulsar::Archive* archive)
{
  vector<string> dm_keywords;
  get_dm_keywords(dm_keywords);

  for (unsigned i = 0; i < dm_keywords.size(); ++i) {
    string str = archive->get_ephemeris()->get_value(dm_keywords[i]);

    if (!str.empty()) {
      dms.push_back(fromstring<double>(str));
    }
  }
}

double calculate_new_dm(vector<double>& dms, const double dm, const double delta_time)
{
    double new_dm = dm;
    unsigned power = 1;

    for (unsigned i = 0; i < dms.size(); ++i) {
        new_dm += dms[i] * pow(delta_time, power);
        ++power;
    }

    return new_dm;
}

void update_dm(Pulsar::Archive* archive)
{
  const MJD tepoch = get_tepoch(archive);
  const double delta_time = get_delta_time(archive, tepoch);
  const double dm = get_dm(archive);

  vector<double> dms;
  get_dms(dms, archive);

  const double new_dm = calculate_new_dm(dms, dm, delta_time);

  archive->set_dispersion_measure(new_dm);
}

void get_dm_keywords(vector<string>& keywords)
{
  for (unsigned i = 1; i <= NUMBER_OF_DM_KEYWORDS; ++i) {
    keywords.push_back("DM" + tostring<unsigned>(i));
  }
}

#include "Pauli.h"

#include "Pulsar/FaradayRotation.h"
#include "Pulsar/AuxColdPlasma.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"

using namespace Pulsar;

void correct_auxiliary_rm (Archive* archive, double aux_rm)
{
  Pauli::basis().set_basis(archive->get_basis());

  Reference::To<FaradayRotation> xform = new FaradayRotation;
  xform->set_rotation_measure( aux_rm );
  xform->set_reference_wavelength( 0 );
  xform->just_do_it (archive);

  AuxColdPlasma* history = archive->getadd<AuxColdPlasma>();
  history->set_birefringence_model_name ( "pam --aux_rm" );
  history->set_birefringence_corrected (true);

  for (unsigned i=0; i<archive->get_nsubint(); i++)
  {
    Integration* subint = archive->get_Integration(i);
    AuxColdPlasmaMeasures* aux = subint->getadd<AuxColdPlasmaMeasures>();
    aux->set_rotation_measure (aux_rm);
  }
}
