/***************************************************************************
 *
 *   Copyright (C) 2003-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pcm.C,v $
   $Revision: 1.117 $
   $Date: 2011/02/17 13:07:23 $
   $Author: straten $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/psrchive.h"
#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/PulsarCalibrator.h"
#include "Pulsar/SystemCalibratorUnloader.h"

#include "Pulsar/Database.h"
#include "Pulsar/StandardPrepare.h"

#include "Pulsar/ReceptionModelSolveMEAL.h"
#if HAVE_GSL
#include "Pulsar/ReceptionModelSolveGSL.h"
#endif

#include "MEAL/Depolarizer.h"
#include "MEAL/Steps.h"

#include "Pulsar/Interpreter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include "Pulsar/SingleAxis.h"
#include "Pulsar/ReflectStokes.h"
#include "Pulsar/ProjectionCorrection.h"

#include "RealTimer.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#if HAVE_PGPLOT
#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/StokesSpherical.h"
#include <cpgplot.h>
#endif

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace std;
using namespace Pulsar;

void usage ()
{
  cout << "pcm - polarimetric calibration model \n"
    "Usage: pcm [options] [filenames] \n"
    "\n"
    "  -h         this help page \n"
    "  -V level   set verbosity level [0->4] \n"
    "\n"
    "  -A archive set the output archive class name \n"
    "  -D name    enable diagnostic: name=report,guess,residual,result,total\n"
    "  -m model   receiver model name: van09, bri00e19 or van04e18 [default]\n"
    "  -l solver  solver: MEAL [default] of GSL \n"
    "  -I impure  load impurity transformation from file \n"
    "  -y         always trust the Pointing::feed_angle attribute \n"
    "  -Z         ignore the sky coordinates of PolnCal observations \n"
    "\n"
    "  -C meta    filename with list of calibrator files \n"
    "  -d dbase   filename of Calibration Database \n"
    "  -M meta    filename with list of pulsar files \n"
    "  -j job     preprocessing job \n"
    "  -J jobs    multiple preprocessing jobs in 'jobs' file \n"
    "\n"
    "MEM: Measurement Equation Modeling - observations of an unknown source \n"
    "\n"
    "  -t nproc   solve using nproc threads \n"
    "\n"
    "  -U PAR     model PAR with a unique value for each CAL \n"
    "  -u PAR[:W] model PAR with a step at each CAL \n"
    "  -o PAR:N   model PAR as N degree polyomial \n"
    "             where PAR is one of \n"
    "               g = absolute gain \n"
    "               b = differential gain \n"
    "               r = differential phase \n"
    "               a = all of the above \n"
    "             and where W can be one of \n"
    "               A = cals are observed after pulsars \n"
    "               B = cals are observed before pulsars (default) \n"
    "\n"
    "  -B choose  set the phase bin selection policy: int, pol, orth, inv \n"
    "             separate multiple policies with commas \n"
    "  -c archive choose best input states from archive \n"
    "\n"
    "  -b bin     add phase bin to constraints \n"
    "  -n nbin    set the number of phase bins to choose as input states \n"
    "  -p pA,pB   set the phase window from which to choose input states \n"
    "\n"
    "  -a align   set the threshold for testing input data phase alignment \n"
    "  -g         unique absolute gain for each pulsar observation [DEVEL]\n"
    "  -r         enforce physically realizable Stokes parameters [DEVEL]\n"
    "  -s         normalize Stokes parameters by total invariant interval \n"
    "\n"
    "  -q         assume that CAL Stokes Q = 0 (linear feeds only)\n"
    "  -v         assume that CAL Stokes V = 0 (linear feeds only)\n"
    "\n"
    "  -F days    use flux calibrators within days of pulsar data mid-time\n"
    "  -L hours   use reference sources within hours of pulsar data mid-time\n"
    "\n"
    "MTM: Matrix Template Matching -- observations of a known source \n"
    "\n"
    "  -S fname   filename of calibrated standard \n"
    "  -H         allow software to choose the number of harmonics \n"
    "  -n nbin    set the number of harmonics to use as input states \n"
    "  -1         solve independently for each observation \n"
    "  -z         share a single phase shift estimate b/w all observations \n"
    "\n"
    "See " PSRCHIVE_HTTP "/manuals/pcm for more details\n"
       << endl;
}

// Construct a calibrator model for MEM mode
SystemCalibrator* measurement_equation_modeling (const char* binname,
						 unsigned nbin);

// Construct a calibrator model for MTM mode
SystemCalibrator* matrix_template_matching (const char* stdname);

// Plot the various components of the model
void plot_state (SystemCalibrator* model, const std::string& state);

Reference::To<Calibration::StandardPrepare> prepare;

void auto_select (Pulsar::ReceptionCalibrator& model,
		  Pulsar::Archive* archive,
		  unsigned maxbins)
{
  cerr << "pcm: choosing up to " << maxbins << " pulse phase bins" << endl;
  vector<unsigned> bins;

  prepare->set_input_states (maxbins);
  prepare->choose (archive);
  prepare->get_bins (bins);

  sort (bins.begin(), bins.end());

  for (unsigned ibin=0; ibin < bins.size(); ibin++)
  {
    // cerr << "pcm: adding phase bin " << bins[ibin] << endl;
    model.add_state (bins[ibin]);
  }

  archive->fscrunch ();
  archive->tscrunch ();
  prepare->prepare (archive);

  model.set_standard_data( archive );

#if HAVE_PGPLOT

  cpgbeg (0, "chosen.ps/CPS", 0, 0);

  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting chosen phase bins" << endl;
  Pulsar::StokesSpherical plot;
  plot.plot (archive);

  cpgswin (0,1,0,1);
  cpgsls (2);

  for (unsigned ibin=0; ibin < bins.size(); ibin++)
  {
    float phase = float(bins[ibin])/float(archive->get_nbin());
    cpgmove (phase, 0);
    cpgdraw (phase, 1);
  }

  cpgend();

  cpgbeg (0, "onpulse.ps/CPS", 0, 0);

  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  cerr << "pcm: plotting on-pulse phase bins" << endl;
  plot.get_flux()->set_selection( model.get_onpulse() );
  plot.plot (archive);

  cpgend();

#endif
}


void range_select (Pulsar::ReceptionCalibrator& model,
		   float phmin, float phmax,
		   unsigned nbin, unsigned maxbins)
{
  if (phmin > phmax)
    phmax += 1.0;

  float increment = (phmax - phmin)/maxbins;

  unsigned last_bin = unsigned (phmax * nbin);

  for (float bin = phmin; bin<=phmax; bin += increment)
  {
    unsigned ibin = unsigned (bin * nbin) % nbin;

    if (ibin != last_bin)
    {
      cerr << "pcm: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}

#if HAVE_PGPLOT

void plot_pulsar (Pulsar::SystemCalibratorPlotter& plotter,
		  Pulsar::SystemCalibrator& model)
{

  unsigned panels = plotter.npanel;
  plotter.npanel = 4;

  unsigned nstate = model.get_nstate_pulsar();
  double centre_frequency = model.get_Archive()->get_centre_frequency();
  double bandwidth = model.get_Archive()->get_bandwidth();

  Reference::To<Pulsar::Calibrator::Info> info;

  for (unsigned istate=0; istate<nstate; istate++)
  {
    info = model.new_info_pulsar (istate);
    cpgpage();
    plotter.plot (info, model.get_nchan(), centre_frequency, bandwidth);
  }

  plotter.npanel = panels;

}

void plot_constraints (Pulsar::SystemCalibratorPlotter& plotter,
		       unsigned nchan, unsigned only_chan=0)
{
  unsigned ichan = 1;

  if (nchan == 1)
    ichan = only_chan;

  const unsigned nstate = plotter.get_calibrator()->get_nstate();

  for (; ichan < nchan; ichan++)
  {
    // don't try to plot if the equation for this channel has no data
    if (plotter.get_calibrator()->get_ndata (ichan) == 0)
      continue;

    char filename [256];
    sprintf (filename, "channel_%d.ps/CPS", ichan);
    cpgbeg (0, filename, 0, 0);

    if (plotter.use_colour)
      cpgsvp (.15,.9, .15,.9);
    else
      cpgsvp (.25,.75,.15,.95);

    plotter.plot_cal_constraints (ichan);

    // cerr << "pcm: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++)
    {
      if (!plotter.get_calibrator()->get_state_is_pulsar (istate))
	continue;

      cpgpage();

      // cerr << "ichan=" << ichan << " istate=" << plot_state << endl;
      plotter.plot_psr_constraints (ichan, istate);
    }

    cpgend ();
  }
}

#endif // HAVE_PGPLOT

// name of the default parameterization
Reference::To<Pulsar::Calibrator::Type> model_type =
	      new Pulsar::CalibratorTypes::van04_Eq18;

// unloads the solution(s)
Pulsar::SystemCalibrator::Unloader unloader;

// verbosity flags
bool verbose = false;

// The maximum number of bins to use
unsigned maxbins = 16;

// Flag raised when the above value is set using -n
bool maxbins_set = false;

// The pulse phase window to use
float phmin = 0, phmax = 0;

// The phase bins to add to the model
vector<unsigned> phase_bins;

// Flag raised when software may choose the maximum harmonic
bool choose_maximum_harmonic = false;

// Mode B: Solve the measurement equation for each observation
bool solve_each = false;

// Mode B: Share a single phase estimate between all observations
bool shared_phase = false;

// significance of phase shift required to fail test
float alignment_threshold = 4.0; // sigma

// total instensity profile of first archive, used to check for phase jumps
Reference::To<Pulsar::Profile> phase_std;

// names of the calibrator files
vector<string> calibrator_filenames;

// Each flux calibrator observation may have unique values of I, Q & U
bool multiple_flux_calibrators = false;

bool measure_cal_V = true;
bool measure_cal_Q = true;
bool equal_ellipticities = false;

bool normalize_by_invariant = false;
bool independent_gains = false;
bool step_after_cal = false;

bool physical_coherency = false;

float retry_chisq = 0.0;
float invalid_chisq = 0.0;

int actual_main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
#ifdef _DEBUG
  size_t in = Reference::Able::get_instance_count();
#endif

  int ret = actual_main (argc, argv);

#ifdef _DEBUG
  size_t out = Reference::Able::get_instance_count();
  cerr << "Leaked: " << out - in << endl;
#endif

  return ret;
}

Reference::To< MEAL::Real4 > impurity;
Reference::To< MEAL::Univariate<MEAL::Scalar> > gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_gain_variation;
Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_phase_variation;

bool get_time_variation ()
{
  return gain_variation || diff_gain_variation || diff_phase_variation;
}

void set_time_variation (char code, MEAL::Univariate<MEAL::Scalar>* function)
{
  switch (code) {
  case 'g':
    cerr << "gain" << endl;
    gain_variation = function;
    return;
  case 'b':
    cerr << "differential gain" << endl;
    diff_gain_variation = function;
    return;
  case 'r':
    cerr << "differential phase" << endl;
    diff_phase_variation = function;
    return;
  case 'a':
    cerr << "all backend parameters" << endl;
    gain_variation = function;
    diff_gain_variation = function;
    diff_phase_variation = function;
    return;
  }
  throw Error (InvalidParam, "set_time_variation",
	       "unrecognized PAR code = %c", code);
}


bool gain_foreach_calibrator = false;
bool diff_gain_foreach_calibrator = false;
bool diff_phase_foreach_calibrator = false;

bool get_foreach_calibrator ()
{
  return gain_foreach_calibrator ||
    diff_gain_foreach_calibrator ||
    diff_phase_foreach_calibrator;
}

void set_foreach_calibrator (char code)
{
  switch (code)
  {
  case 'g':
    cerr << "gain" << endl;
    gain_foreach_calibrator = true;
    return;
  case 'b':
    cerr << "differential gain" << endl;
    diff_gain_foreach_calibrator = true;
    return;
  case 'r':
    cerr << "differential phase" << endl;
    diff_phase_foreach_calibrator = true;
    return;
  case 'a':
    cerr << "all backend parameters" << endl;
    gain_foreach_calibrator = true;
    diff_gain_foreach_calibrator = true;
    diff_phase_foreach_calibrator = true;
    return;
  }
  throw Error (InvalidParam, "set_foreach_calibrator",
	       "unrecognized PAR code = %c", code);
}

Calibration::ReceptionModel::Solver* new_solver (const string& name)
{
  if (name == "MEAL")
    return new Calibration::SolveMEAL;

#if HAVE_GSL
  if (name == "GSL")
    return new Calibration::SolveGSL;
#endif

  throw Error (InvalidParam, "pcm", "no solver named " + name);
}

// The command language interpreter
Reference::To<Pulsar::Interpreter> preprocessor = standard_shell();

// preprocessing jobs
vector<string> jobs;

Pulsar::Archive* load (const std::string& filename)
{
  if (verbose)
    cerr << "pcm: loading " << filename << endl;
	
  Reference::To<Pulsar::Archive> archive = Pulsar::Archive::load (filename);

  cout << "pcm: loaded archive: " << filename << endl;

  if (jobs.size())
  {
    if (verbose)
      cerr << "pcm: preprocessing " << archive->get_filename() << endl;
    preprocessor->set(archive);
    preprocessor->script(jobs);
  }

  return archive.release();
}

static bool output_report = false;
static bool prefit_report = false;

static bool plot_guess = false;
static bool plot_residual = false;
static bool plot_total = false;
static bool plot_result = false;
static bool publication_plots = false;

void enable_diagnostic (const string& name)
{
  if (name == "prefit")
    prefit_report = true;

  else if (name == "report")
    output_report = true;

  else if (name == "guess")
    plot_guess = true;

  else if (name == "residual")
    plot_residual = true;

  else if (name == "total")
    plot_total = true;

  else if (name == "result")
    plot_result = true;

  else
  {
    cerr << "pcm: unrecognized diagnostic name '" << name << "'" << endl;
    exit (-1);
  }
}

// names of files containing a Calibration Database
vector<string> cal_dbase_filenames;

// names of files containing data
vector <string> filenames;

// name of file containing the calibrated template
char* template_filename = NULL;

// hours from mid-time within which PolnCal observations will be selected
float polncal_hours = 12.0;

// days from mid-time within which FluxCalOn observations will be selected
float fluxcal_days = 7.0;

// look for PolnCal observations with nearby sky coordinates
bool check_coordinates = true;

bool must_have_cals = true;

// name of file containing list of calibrator Archive filenames
char* calfile = NULL;

void load_calibrator_database ();

int actual_main (int argc, char *argv[]) try
{
  unloader.set_program ( "pcm" );
  unloader.set_filename( "pcm.fits" );

  // Number of threads used to solve equations
  unsigned nthread = 0;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of file from which phase bins will be chosen
  char* binfile = NULL;

  // name of least squares minimization algorithm
  char* least_squares = NULL;

  // name of file containing MEAL::Function text interface commands
  vector<string> equation_configuration;

  bool unload_each_calibrated = true;

  int gotc = 0;

  const char* args =
    "1A:a:B:b:C:c:D:d:E:e:fF:gHhI:j:J:kL:l:"
    "M:m:Nn:o:Pp:qR:rS:st:T:u:U:vV:X:yzZ";

  while ((gotc = getopt(argc, argv, args)) != -1)
  {
    switch (gotc)
    {
    case '1':
      solve_each = true;
      break;

    case 'A':
      unloader.set_archive_class( optarg );
      break;

    case 'a':
      alignment_threshold = atof (optarg);
      break;

    case 'B':
      prepare = Calibration::StandardPrepare::factory (optarg);
      break;

    case 'b':
    {
      unsigned bin = atoi (optarg);
      cerr << "pcm: adding phase bin " << bin << endl;
      phase_bins.push_back (bin);
      break;
    }

    case 'C':
      calfile = optarg;
      break;

    case 'c':
      binfile = optarg;
      break;

    case 'd':
      cal_dbase_filenames.push_back (optarg);
      break;

    case 'D':
      enable_diagnostic (optarg);
      break;

    case 'e':
      separate (optarg, equation_configuration, ",");
      break;

    case 'E':
      loadlines (optarg, equation_configuration);
      break;

    case 'F':
      fluxcal_days = atof (optarg);
      break;

    case 'f':
      multiple_flux_calibrators = true;
      break;

    case 'g':
      independent_gains = true;
      break;
      
    case 'H':
      choose_maximum_harmonic = true;
      break;

    case 'I':
      impurity = MEAL::Function::load<MEAL::Real4> (optarg);
      break;

    case 'j':
      separate (optarg, jobs, ",");
      break;

    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'k':
      equal_ellipticities = true;
      break;

    case 'L':
      polncal_hours = atof (optarg);
      break;

    case 'l':
      least_squares = optarg;
      break;

    case 'm':
      model_type = Pulsar::Calibrator::Type::factory (optarg);
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maxbins = atoi (optarg);
      cerr << "pcm: using a maximum of " << maxbins << " bins or harmonics" 
	   << endl;
      maxbins_set = true;
      break;

    case 'N':
      unload_each_calibrated = false;
      break;

    case 'o': {
      char code;
      unsigned order;
      if( sscanf (optarg, "%c:%u", &code, &order) != 2 ) {
	cerr << "pcm: error parsing '" << optarg << "' as PAR:N" << endl;
	return -1;
      }
      cerr << "pcm: using a polynomial of degree " << order << " to model ";
      set_time_variation( code, new MEAL::Polynomial (order+1) );
      break;
    }

    case 'P':
      publication_plots = true;
      break;

    case 'p':
    {
      char dummy;
      if (sscanf (optarg, "%f%c%f", &phmin, &dummy, &phmax) != 3)
      {
	cerr << "pcm: error parsing " << optarg << " as phase range" << endl;
	return -1;
      }
      cerr << "pcm: selecting input states from " << phmin << " to " << phmax
	   << endl;
      break;
    }

    case 'r':
      physical_coherency = true;
      break;

    case 'R':
      retry_chisq = atof (optarg);
      break;

    case 's':
      normalize_by_invariant = true;
      break;

    case 'S':
      alignment_threshold = 0.0;
      template_filename = optarg;
      break;

    case 't':
      nthread = atoi (optarg);
      if (nthread == 0)  {
        cerr << "pcm: invalid number of threads = " << nthread << endl;
        return -1;
      }

      cerr << "pcm: solving using " << nthread << " threads" << endl;
      break;

    case 'u':
      cerr << "pcm: using a multiple-step function to model ";
      set_time_variation( optarg[0], new MEAL::Steps );
      cerr << "pcm: assuming cals are observed ";
      if (optarg[1]==':')
      {
        if (optarg[2]=='A')
        {
          step_after_cal = true;
        }
      }
      cerr << (step_after_cal ? "after" : "before") << " pulsars" << endl;
      break;

    case 'U':
      cerr << "pcm: for each calibrator, a unique value of ";
      set_foreach_calibrator( optarg[0] );
      break;

    case 'q':
      measure_cal_Q = false;
      break;

    case 'v':
      measure_cal_V = false;
      break;

    case 'X':
      invalid_chisq = atof (optarg);
      break;

    case 'y':
      ProjectionCorrection::trust_pointing_feed_angle = true;
      break;

    case 'h':
      usage ();
      return 0;

    case 'V':
    {
      int level = atoi (optarg);
      verbose = true;

      if (level > 4)
        Calibration::ReceptionModel::very_verbose = true;

      if (level > 3) 
        Calibration::ReceptionModel::verbose = true;

      if (level > 2)
	Calibration::SignalPath::verbose = true;

      Pulsar::Calibrator::verbose = level;

      level --; if (level < 0) level = 0;

      Pulsar::Archive::set_verbosity (level);

      break;
    }

    case 'z':
      shared_phase = true;
      break;

    case 'Z':
      check_coordinates = false;
      break;

    default:
      cout << "Unrecognised option" << endl;
    }
  }

  if (!template_filename && phmin == phmax && !binfile)
  {
    cerr << "pcm: In mode A, at least one of the following options"
      " must be specified:\n"
      " -p min,max  Choose constraints from the specified pulse phase range \n"
      " -c archive  Choose optimal constraints from the specified archive \n"
	 << endl;
    return -1;
  }

  if (metafile)
    stringfload (&filenames, metafile);
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);
  
  if (filenames.empty())
  {
    cerr << "pcm: no archives were specified" << endl;
    return -1;
  } 

  // assumes that sorting by filename also sorts by epoch
  sort (filenames.begin(), filenames.end());

  if (calfile)
    stringfload (&calibrator_filenames, calfile);

  load_calibrator_database();

  if (!prepare)
  {
    Calibration::MultipleRanking* mult = new Calibration::MultipleRanking;
    mult->add( new Calibration::MaximumPolarization );
    mult->add( new Calibration::MaximumIntensity );
    prepare = mult;
  }

  Reference::To<Pulsar::SystemCalibrator> model;
  Reference::To<Pulsar::Archive> total;
  Reference::To<Pulsar::Archive> archive;

  cerr << "pcm: loading archives" << endl;
  
  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    archive = load (filenames[i]);

    if (archive->get_type() == Signal::Pulsar)
    {
      if (verbose)
	cerr << "pcm: preparing pulsar data" << endl;
      
      prepare->prepare (archive);
    }

    if (!model) try
    {
      cerr << "pcm: creating model" << endl;

      if (template_filename)
	model = matrix_template_matching (template_filename);
      else
	model = measurement_equation_modeling (binfile, archive->get_nbin());

      model->set_nthread (nthread);
      model->set_report_projection (true);

      model->set_report_initial_state (prefit_report);

      if (impurity)
	model->set_impurity( impurity );

      if (gain_variation)
	model->set_gain( gain_variation );

      if (diff_gain_variation)
	model->set_diff_gain( diff_gain_variation );

      if (diff_phase_variation)
	model->set_diff_phase( diff_phase_variation );

      if (get_foreach_calibrator())
      {
	Reference::To< Calibration::SingleAxis > foreach;
	foreach = new Calibration::SingleAxis;
	foreach->set_infit (0, gain_foreach_calibrator);
	foreach->set_infit (1, diff_gain_foreach_calibrator);
	foreach->set_infit (2, diff_phase_foreach_calibrator);

	model->set_foreach_calibrator (foreach);
      }

      if (least_squares)
	model->set_solver( new_solver(least_squares) );

      if (retry_chisq)
        model->set_retry_reduced_chisq( retry_chisq );

      if (invalid_chisq)
        model->set_invalid_reduced_chisq( invalid_chisq );

      model->set_equation_configuration( equation_configuration );

    }
    catch (Error& error)
    {
      cerr << "pcm: ERROR while creating model\n" << error << endl;
      return -1;
    }

    /*
      test for phase shift only if phase_std is not from current archive.
      this test will fail if binfile is a symbollic link.
    */
    if (phase_std && (binfile==NULL || archive->get_filename() != binfile)) try
    {
      if (verbose)
	cerr << "pcm: creating checking phase" << endl;

      Reference::To<Pulsar::Archive> temp = archive->total();
      Estimate<double> shift = temp->get_Profile(0,0,0)->shift (*phase_std);

      double abs_shift = fabs( shift.get_value() );

      /* if the shift is greater than 1 phase bin and significantly
	 more than the error, then there may be a problem */

      if( abs_shift > 1.0 / phase_std->get_nbin() &&
	  abs_shift > alignment_threshold * shift.get_error() )
      {
	cerr << endl <<
	  "pcm: ERROR apparent phase shift between input archives\n"
	  "\tshift = " << shift.get_value() << " +/- " << shift.get_error () <<
	  "  =  " << int(shift.get_value() * phase_std->get_nbin()) <<
	  " phase bins" << endl << endl;

	archive = 0;
	continue;
      }
    }
    catch (Error& error)
    {
      cerr << "pcm: ERROR while testing phase shift\n" << error << endl;
      return -1;
    }

    if (alignment_threshold && !phase_std)
    { 
      cerr << "pcm: creating phase reference" << endl;

      // store an fscrunched and tscrunched clone for phase reference
      Reference::To<Archive> temp = archive->total();
      phase_std = temp->get_Profile (0,0,0);	
    }

#if 0

    MIGHT WANT TO MAKE PCM AUTO-ALIGN WHEN THE EPHEMERIS IS NO GOOD

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

#endif

    cerr << "pcm: adding observation" << endl;
    model->preprocess( archive );
    model->add_observation( archive );

    if (archive->get_type() == Signal::Pulsar)
    {    
      if (verbose)
	cerr << "pcm: calibrate with current best guess" << endl;
      
      model->precalibrate (archive);
      
      if (solve_each)
      {
	string newname = replace_extension (filenames[i], ".calib");
	archive->unload (newname);    
	cerr << "pcm: unloaded " << newname << endl;
      }

      if (verbose)
	cerr << "pcm: add to total" << endl;

#if 0
      if (!total)
	total = archive;
      else
	total->append (archive);

      total->tscrunch ();
#endif

    }

    archive = 0;
  }
  catch (Error& error)
  {
    cerr << "pcm: error while handling " << filenames[i] << endl;
    cerr << error << endl;
    archive = 0;
  }

  if (solve_each)
  {
    if (total)
    {
      cerr << "pcm: writing total calibrated pulsar archive" << endl;
      total->unload ("total.ar");
    }
    return 0;
  }

  if (total)
  {
    cerr << "pcm: writing total uncalibrated pulsar archive" << endl;
    total->unload ("first.ar");
  }


#if HAVE_PGPLOT

  Pulsar::SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;

  try {

    if (plot_guess)
      plot_state (model, "guess");
    
    if (plot_total && total)
    {
      cerr << "pcm: plotting uncalibrated total PSR" << endl;
	
      cpgbeg (0, "uncalibrated.ps/CPS", 0, 0);
      cpgask(1);
      cpgslw(2);
      cpgsvp (.1,.9, .1,.9);
      
      total->fscrunch();
      
      cerr << "pcm: plotting uncalibrated pulsar total stokes" << endl;
      Pulsar::StokesSpherical plot;
      plot.plot (total);
      
      cpgend();
    }

    if (plot_residual && model->get_nstate_pulsar())
    {
      cerr << "pcm: plotting pulsar constraints" << endl;
      plot_constraints (plotter, model->get_nchan());
    }

  }
  catch (Error& error)
  {
    cerr << "pcm: error while producing plots ignored" << endl;
  }

#endif // HAVE_PGPLOT

  total = 0;

  try
  {
    cerr << "pcm: solving model" << endl;
    model->solve ();
  }
  catch (Error& error)
  {
    cerr << error << endl;
    return -1;
  }

  unloader.unload (model);

#if HAVE_PGPLOT

  if (plot_result)
  {
    plot_state (model, "result");

    if (get_time_variation())
    {
      cpgbeg (0, "result_variations.ps/PS", 0, 0);
      cpgsvp (0.1,.9, 0.1,.9);

      unsigned panels = plotter.npanel;
      plotter.npanel = 4;

      cerr << "pcm: plotting time variation functions" << endl;
      plotter.plot_time_variations ();

      plotter.npanel = panels;
      cpgend ();
    }
  }

  if (plot_residual && model->get_nstate_pulsar())
  {
    plotter.set_plot_residual (true);
    
    cerr << "pcm: plotting pulsar constraints with model" << endl;
    plot_constraints (plotter, model->get_nchan());
  }

#endif // HAVE_PGPLOT

  for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
    dirglob (&filenames, calibrator_filenames[ical]);
  
  cerr << "pcm: calibrating archives (PSR and CAL)" << endl;

  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    if (verbose)
      cerr << "pcm: loading " << filenames[i] << endl;

    archive = Pulsar::Archive::load(filenames[i]);

    cout << "pcm: loaded archive: " << filenames[i] << endl;
    
    model->precalibrate( archive );

    if (unload_each_calibrated)
    {
      string newname = replace_extension (filenames[i], ".calib");
    
      if (verbose)
	cerr << "pcm: calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);
    
      cout << "New file " << newname << " unloaded" << endl;
    }

    if (archive->get_type() == Signal::Pulsar)
    {
      if (verbose)
	cerr << "pcm: correct and add to calibrated total" << endl;
      
      if (!total)
	total = archive;
      else
	total->append (archive);
      
      total->tscrunch ();
    }
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  if (total)
  {
    cerr << "pcm: writing total integrated pulsar archive" << endl;
    total->unload ("total.ar");
  }

#if HAVE_PGPLOT

  if (plot_total && total)
  {
    cpgbeg (0, "calibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgslw(2);
    cpgsvp (.1,.9, .1,.9);

    total->fscrunch();
    total->remove_baseline();

    cerr << "pcm: plotting calibrated pulsar total stokes" << endl;
    Pulsar::StokesSpherical plot;
    plot.plot (total);

    cpgend ();
  }

#endif // HAVE_PGPLOT

  cerr << "pcm: finished" << endl;

  return 0;
}
catch (Error& error)
{
  cerr << "pcm: error" << error << endl;
  return -1;
}


using namespace Pulsar;

SystemCalibrator* measurement_equation_modeling (const char* binfile, 
						 unsigned nbin) try
{
  ReceptionCalibrator* model = new ReceptionCalibrator (model_type);

  model->output_report = output_report;

  if (measure_cal_V)
    cerr << "pcm: if available, will use fluxcal data to constrain"
      " CAL Stokes V" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes V = 0" << endl;

  model->measure_cal_V = measure_cal_V;

  if (measure_cal_Q)
    cerr << "pcm: allowing CAL Stokes Q to vary" << endl;
  else
    cerr << "pcm: assuming that CAL Stokes Q = 0" << endl;

  model->measure_cal_Q = measure_cal_Q;

  if (equal_ellipticities)
    cerr << "pcm: assuming that the receptor ellipticities are equal" << endl;

  model->equal_ellipticities = equal_ellipticities;

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  model->set_step_after_cal( step_after_cal );

  if (independent_gains)
    cerr << "pcm: each observation has a unique gain" << endl;

  model->independent_gains = independent_gains;

  if (physical_coherency)
    cerr << "pcm: enforcing physically realizable Stokes parameters" << endl;
  else
    cerr << "pcm: risking physically unrealizable Stokes parameters" << endl;

  model->physical_coherency = physical_coherency;

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model->add_state (phase_bins[ibin]);

  if (multiple_flux_calibrators)
    cerr <<
      "pcm: each flux calibrator observation "
      "will be independently modeled" << endl;

  model->multiple_flux_calibrators = multiple_flux_calibrators;

  cerr << "pcm: set calibrators" << endl;
  model->set_calibrators (calibrator_filenames);

  if (model->get_nstate_pulsar() == 0)
  {
    // archive from which pulse phase bins will be chosen
    Reference::To<Pulsar::Archive> autobin;

    if (binfile) try 
    {
      autobin = load (binfile);

      auto_select (*model, autobin, maxbins);

      if (alignment_threshold)
	phase_std = autobin->get_Profile (0,0,0);
    }
    catch (Error& error)
    {
      error << "\ncould not load constraint archive '" << binfile << "'";
      throw error;
    }
    else
      range_select (*model, phmin, phmax, nbin, maxbins);
      
    cerr << "pcm: " << model->get_nstate_pulsar() << " states" << endl;
    if ( model->get_nstate_pulsar() == 0 )
      throw Error (InvalidState, "pcm:mode A",
		   "no pulsar phase bins have been selected");
  }

  return model;
}
catch (Error& error)
{
  throw error += "pcm:mode A";
}

SystemCalibrator* matrix_template_matching (const char* stdname)
{
  PulsarCalibrator* model = new PulsarCalibrator (model_type);

  if (shared_phase)
    model->share_phase ();

  if (maxbins_set)
    model->set_maximum_harmonic (maxbins);

  model->set_choose_maximum_harmonic (choose_maximum_harmonic);

  if (solve_each)
  {
    SystemCalibrator::Unloader* mtm = new SystemCalibrator::Unloader(unloader);
    mtm->set_extension ("mtm");

    model->set_solve_each (true);
    model->set_unload_each (mtm);
  }

  if (normalize_by_invariant)
    cerr << "pcm: normalizing Stokes parameters by invariant" << endl;
  else
    cerr << "pcm: not normalizing Stokes parameters" << endl;

  model->set_normalize_by_invariant( normalize_by_invariant );

  Reference::To<Archive> standard;

  cerr << "pcm: loading and setting standard" << endl;

  standard = Archive::load (stdname);
  standard->convert_state (Signal::Stokes);

  RealTimer clock;

  clock.start();

  model->set_standard (standard);

  clock.stop();
  cerr << "pcm: standard set in " << clock << endl;

  if (calibrator_filenames.size())
    cerr << "pcm: adding " << calibrator_filenames.size() << " calibrators" 
	 << endl;

  for (unsigned ical=0; ical < calibrator_filenames.size(); ical++)
    model->add_observation( Archive::load (calibrator_filenames[ical]) );

  return model;
}

/* **********************************************************************

   FIND APPROPRIATE CALIBRATOR OBSERVATIONS IN THE DATABASE

   ********************************************************************** */

void load_calibrator_database () try
{
  if (!cal_dbase_filenames.size())
    return;
    
  Reference::To<Pulsar::Archive> archive;

  archive = Pulsar::Archive::load( filenames.back() );
  MJD end = archive->end_time();

  archive = Pulsar::Archive::load( filenames.front() );
  MJD start = archive->start_time();

  MJD mid = 0.5 * (end + start);

  Reference::To<Pulsar::Database> database;

  for (unsigned i=0; i<cal_dbase_filenames.size(); i++)
  {
    cout << "pcm: loading database from " << cal_dbase_filenames[i] << endl;
    if (!database)
      database = new Pulsar::Database (cal_dbase_filenames[i]);
    else
      database->load (cal_dbase_filenames[i]);
  }

  cerr << "pcm: database constructed with " << database->size() 
       << " entries" << endl;

  char buffer[256];

  cerr << "pcm: searching for reference source observations"
    " within " << polncal_hours << " hours of midtime" << endl;

  cerr << "pcm: midtime = "
       << mid.datestr (buffer, 256, "%Y-%m-%d-%H:%M:00") << endl;
  
  Pulsar::Database::Criterion criterion;
  criterion = database->criterion (archive, Signal::PolnCal);
  criterion.entry.time = mid;
  criterion.check_coordinates = check_coordinates;
  criterion.minutes_apart = polncal_hours * 60.0;
  
  vector<Pulsar::Database::Entry> oncals;
  database->all_matching (criterion, oncals);
  
  unsigned poln_cals = oncals.size();
  
  if (poln_cals == 0)
  {
    cerr << "pcm: no PolnCal observations found; closest match was \n\n"
	 << database->get_closest_match_report () << endl;
    
    if (must_have_cals && !calfile)
    {
      cerr << "pcm: cannot continue" << endl;
      exit (-1);
    }
  }

  if (template_filename)
    cerr << "pcm: no need for flux calibrator observations" << endl;
  else
  {
    cerr << "pcm: searching for flux calibrator observations"
      " within " << fluxcal_days << " days of midtime" << endl;

    criterion.entry.obsType = Signal::FluxCalOn;
    criterion.check_coordinates = false;
    criterion.minutes_apart = fluxcal_days * 24.0 * 60.0;
    
    database->all_matching (criterion, oncals);
  
    if (oncals.size() == poln_cals)
      cerr << "pcm: no FluxCalOn observations found; closest match was \n\n"
	   << database->get_closest_match_report () << endl;
  }

  for (unsigned i = 0; i < oncals.size(); i++)
  {
    string filename = database->get_filename( oncals[i] );
    cerr << "pcm: adding " << oncals[i].filename << endl;
    calibrator_filenames.push_back (filename);
  }
}
catch (Error& error)
{
  cerr << "pcm: error loading CAL database" << error << endl;
  exit (-1);
}














#if HAVE_PGPLOT

void plot_state (SystemCalibrator* model, const std::string& state) try
{
  using namespace Pulsar;

  SystemCalibratorPlotter plotter (model);
  plotter.use_colour = !publication_plots;

  //
  // if the SystemCalibrator is a ReceptionCalibrator (MEM mode)
  // and one or more flux calibrators has been included in the fit,
  // then plot the flux calibrator information as well
  //
  Reference::To<ReceptionCalibratorPlotter> rplotter = 0;
  ReceptionCalibrator* rmodel = dynamic_cast<ReceptionCalibrator*> (model);
  if (rmodel && rmodel->has_fluxcal())
  {
    rplotter = new ReceptionCalibratorPlotter (rmodel);
    rplotter->use_colour = !publication_plots;
  }

  //
  //
  //

  cpgbeg (0, (state + "_response.ps/CPS").c_str(), 0, 0);
  cpgsvp (.1,.9, .1,.9);
      
  cerr << "pcm: plotting " + state + " of instrumental response" << endl;
  plotter.plot (model);
      
  cpgend();

  //
  //
  //

  cpgbeg (0, (state + "_cal.ps/CPS").c_str(), 0, 0);
  cpgsvp (0.1,.9, 0.1,.9);
      
  cerr << "pcm: plotting " + state + " of CAL" << endl;
  plotter.plot_cal();
      
  cpgend();
 
  //
  //
  //

  if (rplotter)
  {
    cpgbeg (0, (state + "_fluxcal.ps/CPS").c_str(), 0, 0);
    cpgsvp (0.1,.9, 0.1,.9);
      
    cerr << "pcm: plotting " + state + " of flux calibrator" << endl;
    rplotter->plot_fluxcal();
      
    cpgend();
  }
 
  //
  //
  //

  cpgbeg (0, (state + "_psr.ps/CPS").c_str(), 0, 0);
  cpgsvp (0.1,.9, 0.1,.9);
      
  cerr << "pcm: plotting " + state + " pulsar states" << endl;
  plot_pulsar (plotter, *model);
      
  cpgend ();
}
 catch (Error& error)
  {
    cerr << "pcm: error while producing plots ignored" << endl;
  }

#endif // HAVE_PGPLOT
