/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/MatrixTemplateMatching.h"

#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/SincInterpShift.h"
#include "Pulsar/GaussianShift.h"
#include "Pulsar/ParIntShift.h"
#include "Pulsar/ZeroPadShift.h"
#include "Pulsar/FourierDomainFit.h"
#include "Pulsar/FluxCentroid.h"
#include "Pulsar/ComponentModel.h"

#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/FITSHdrExtension.h"

#include "Pulsar/ArchiveTemplates.h"
#include "Pulsar/SmoothSinc.h"

#if HAVE_PGPLOT
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"
#include "Pulsar/ProfilePlot.h"
#include <cpgplot.h>
#endif

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"
#include "tostring.h"

#include <fstream>
#include <iostream>

#include <math.h>
#include <string.h>
#include <unistd.h>

using namespace std;
using namespace Pulsar;

vector<string> commands;

void loadGaussian(string file,  
		  Reference::To<Pulsar::Archive> &stdarch,  
		  Reference::To<Pulsar::Archive> arch);

void compute_dt(Reference::To<Archive> archive, vector<Tempo::toa>& toas,
        string std_name);

double truncateDecimals(double d, int decimalPlaces);

#if HAVE_PGPLOT
void plotDifferences(Reference::To<Archive> arch,
        Reference::To<Archive> stdarch,
        vector<Tempo::toa>& toas, const double min_phase,
        const double max_phase);

void set_phase_zoom(vector<Reference::To<Plot> >& plots,
        const double min, const double max);

void setPlotLabels(Reference::To<Archive> diff, Reference::To<Archive> arch, 
        Reference::To<Archive> profile, Pulsar::Plot* diff_plot, 
        vector<Tempo::toa>& toas, const unsigned subint, const unsigned chan);

void calculateDifference(Pulsar::Profile* diff, const float* prof,
        const float* std);

void setupPlotters(Pulsar::Plot* diff_plot, Pulsar::Plot* profile_plot,
        Pulsar::Plot* template_plot, const double min_phase,
        const double max_phase, const string filename);

void scaleProfile(Reference::To<Profile> profile);

void rotate_archive(Reference::To<Archive> archive, vector<Tempo::toa>& toas);

float getMean(const float rms, const float* bins, const unsigned nbin);

float getRms(const float* bins, const unsigned nbin, const float mean, const float oldRms);

void resize_archives(Reference::To<Archive> archive,
        Reference::To<Archive> diff, Reference::To<Archive> original);

void prepare_difference_archive(Reference::To<Archive> diff,
        Reference::To<Archive> arch, Reference::To<Profile> prof);

void difference_plot(Reference::To<Plot> plot,
        Reference::To<Archive> arch, Reference::To<Archive> diff_arch,
        vector<Tempo::toa>& toas, const unsigned subint, const unsigned chan);

void template_plot(Reference::To<Plot> plot, Reference::To<Archive> arch);

void profile_plot(Reference::To<Plot> plot,
        Reference::To<Archive> profile_archive,
        Reference::To<Profile> profile_to_copy, Reference::To<Archive> archive,
        const double freq);

void diff_profiles(Reference::To<Archive> diff, Reference::To<Archive> stdarch, Reference::To<Profile> profile);

string get_xrange(const double min, const double max);
#endif // HAVE_PGPLOT


void usage ()
{
  cout << "pat - Pulsar::Archive timing \n"
    "Usage: pat [options] filenames \n"
    "  -q               Quiet mode \n"
    "  -v               Verbose mode \n"
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "  -M metafile      List of archive filenames in metafile \n"
    "\n"
    "Preprocessing options:\n"
    "  -F               Frequency scrunch before fitting \n"
    "  -T               Time scrunch before fitting \n"
    "  -d               Discard profiles with zero weight\n"
    "\n"
    "Fitting options:\n"
    "  -a stdfiles      Automatically select standard from specified group\n"
    "  -D               Denoise standard \n"
    "  -g datafile      Gaussian model fitting \n"
    "  -s stdfile       Location of standard profile \n"
    "  -S period        Zap harmonics due to periodic spikes in profile \n"
    "                   (use of this option implies SIS) \n"
    "\n"
    "Matrix template matching options: \n"
    "  -c               Choose the maximum harmonic \n"
    "  -n harmonics     Use up to the specified number of harmonics\n"
    "  -p               Enable matrix template matching \n"
    "  -P               Do not fscrunch the standard \n"
    "\n"
    "Algorithm Selection:\n"
    "  -A name          Select shift algorithm [default: PGS] \n"
    "                   PGS = Fourier phase gradient \n"
    "                   GIS = Gaussian interpolation \n"
    "                   PIS = Parabolic interpolation \n"
    "                   ZPF = Zero pad interpolation \n"
    "                   SIS = Sinc interpolation of cross-corration function\n"
    "                   FDM = Fourier domain with Markov chain Monte Carlo \n"
    "                   COF = Centre of Flux \n"
    "\n"
    "  -m filename      Load a component model as output by paas \n"
    "\n"
    "Output options:\n"
    "  -f \"fmt <flags>\" Select output format [default: parkes]\n"
    "                   Available formats: parkes tempo2, itoa, princeton \n"
    "                   For tempo2, <flags> include i = display instrument \n"
    "                                               r = display receiver   \n"
    "  -C \"<options>\"   Select vap-like options to be displayed on output \n"
    "  -C \"<subint> <chan>\" Print subint and/or channel                    \n"
    "  -r               Print reference phase and dt \n"
    "  -R               Print only the phase shift and error in turns \n"
    "  -u               Print as pat-like format smjd + dt \n"
    "\n"
    "Plotting options (if compiled with pgplot):\n"
    "  -K               Specify plot device\n"
    "  -t               Plot template, profile and difference \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pat for more details\n"
       << endl;
}


int main (int argc, char** argv) try
{
  Reference::To<ArrivalTime> arrival = new ArrivalTime;

  arrival->set_shift_estimator (new PhaseGradShift);
  
  bool verbose = false;
  bool std_given = false;
  bool std_multiple = false;
  bool gaussian = false;
  bool full_freq = false;
  bool phase_only = false;

  // the matrix template matching algorithm and related flags
  Pulsar::MatrixTemplateMatching* full_poln = 0;
  bool choose_maximum_harmonic = false;
  unsigned maximum_harmonic = 0;

  bool fscrunch = false;
  bool tscrunch = false;
  bool skip_bad = false;
  bool phase_info = false;
  bool tempo2_output = false;

#if HAVE_PGPLOT
  bool plot_difference = false;
  bool centre_template_peak = false;
#endif

  char *metafile = NULL;

  Pulsar::SmoothSinc* sinc = 0;

  string std,gaussFile;
  string outFormat("parkes"),outFormatFlags;
  string tname;

  double min_phase = 0.0;
  double max_phase = 1.0;
  string plot_device = "/xs";

  vector<string> archives;
  vector<string> stdprofiles;
  vector<Tempo::toa> toas;

  Reference::To<Archive> arch;
  Reference::To<Archive> stdarch;
  Reference::To<Profile> prof;

  float chisq_max = 2.0;

#if HAVE_PGPLOT
#define PLOT_ARGS "t::"
#else
#define PLOT_ARGS
#endif

  const char* args = "a:A:cC:Ddf:Fg:hiK:m:M:n:pPqRrS:s:TuvVx:z:" PLOT_ARGS;

  int gotc = 0;

  while ((gotc = getopt(argc, argv, args)) != -1)
  {
    switch (gotc)
    {

    case 'a':
      std_given = true;
      std_multiple = true;
      std = optarg;

      /* Break up inputs (e.g. have "10cm.std 20cm.std 50*.std") */
      {
	char *str;
	str = strtok(optarg," ");
	do 
	{
	  dirglob (&stdprofiles, str);
	}
	while ((str = strtok(NULL," "))!=NULL);
      }

      if (stdprofiles.size() == 0)
      {
	cerr << "pat -a \"" << optarg << "\" failed: no such file" << endl;
	return -1;
      }
      break;

    case 'A':
    {
      ShiftEstimator* shift = 0;

      if (strcasecmp (optarg, "PGS") == 0)
        shift = new PhaseGradShift;

      else if (strcasecmp (optarg, "GIS") == 0)
        shift = new GaussianShift;

      else if (strcasecmp (optarg, "PIS") == 0)
        shift = new ParIntShift;

      else if (strcasecmp (optarg, "ZPS") == 0)
        shift = new ZeroPadShift;

      else if (strcasecmp (optarg, "SIS") == 0)
        shift = new SincInterpShift;

      else if (strcasecmp (optarg, "FDM") == 0)
        shift = new FourierDomainFit;

      else if (strcasecmp (optarg, "COF") == 0)
        shift = new FluxCentroid;

      arrival->set_shift_estimator( shift );

      break;
    }

    case 'c':
      choose_maximum_harmonic = true;
      break;

    case 'C':
      separate(optarg, commands, " ,");
      break;

    case 'D':
      sinc = new Pulsar::SmoothSinc;
      sinc -> set_bins (8);
      break;

    case 'd':
      skip_bad = true;
      break;

    case 'F':
      fscrunch = true;
      break;

    case 'f':
      {
	/* Set the output format */
	outFormat = string(optarg).substr(0,string(optarg).find(" "));
	if (string(optarg).find(" ")!=string::npos)
	  outFormatFlags = string(optarg).substr(string(optarg).find(" "));
	break;
      }

    case 'g':
      gaussian  = true;
      gaussFile = optarg;
      break;

    case 'h':
      usage ();
      return 0;

    case 'i':
      cout << "$Id: pat.C,v 1.98 2009/11/30 03:15:12 sixbynine Exp $" << endl;
      return 0;

    case 'K':
      plot_device = optarg;
      break;

    case 'm':
      cerr << "pat: loading component model from " << optarg << endl;
      arrival->set_shift_estimator( new ComponentModel(optarg) );
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maximum_harmonic = atoi(optarg);
      break;

    case 'P':
      full_freq = true;
      break;

    case 'p':
      arrival = full_poln = new Pulsar::MatrixTemplateMatching;
      break;

    case 'q':
      Archive::set_verbosity(0);
      break;

    case 'r':
      phase_info = true;
      outFormat = "tempo2";
      break;

    case 'R':
      phase_only = true;
      break;

    case 'S':
    {
      Pulsar::SincInterpShift* sis = new Pulsar::SincInterpShift;
      sis->set_zap_period (atoi(optarg));
      arrival->set_shift_estimator( sis );
      break;
    }

    case 's':
      std_given = true;
      std = optarg;
      break;

    case 'T':
      tscrunch = true;
      break;

#if HAVE_PGPLOT
    case 't':
      if (optarg && tostring(optarg) == "2") {
        centre_template_peak = true;
      }
      plot_difference = true;
      break;
#endif

    case 'u':
      tempo2_output = true;
      outFormat = "tempo2";
      break;

    case 'v':
      Archive::set_verbosity(2);
      verbose = true;
      break;

    case 'V':
      Archive::set_verbosity(3);
      ShiftEstimator::verbose = true;
      verbose = true;
      break;

    case 'x':
      chisq_max = atof(optarg);
      if (chisq_max == 0.0)
        cerr << "pat: disabling reduced chisq cutoff" << endl;
      else
        cerr << "pat: omitting TOAs with reduced chisq > " << chisq_max << endl;
      break;

    case 'z':
      {
        string s1, s2;
        string_split(optarg, s1, s2, ",");
        min_phase = fromstring<double>(s1);
        max_phase = fromstring<double>(s2);
      }
      break;

    default:
      cout << "Unrecognised option " << gotc << endl;
    }
  }

#if HAVE_PGPLOT
  if (plot_difference)
      if (cpgopen(plot_device.c_str()) <= 0)
          cpgopen("?");
#endif

  if (metafile)
    stringfload (&archives, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  } 
  
  arrival->set_format (outFormat);
  arrival->set_format_flags (outFormatFlags);
  arrival->set_attributes (commands);

  if (full_poln)
  {
    cerr << "pat: using full polarization" << endl;

    if (maximum_harmonic)
      full_poln->set_maximum_harmonic (maximum_harmonic);

    full_poln->set_choose_maximum_harmonic (choose_maximum_harmonic);
  }

  if (!std.empty() && !std_multiple && !gaussian) try
  {
    stdarch = Archive::load(std);

    if (!full_freq)
      stdarch->fscrunch();

    stdarch->tscrunch();
    
    if (sinc)
      Pulsar::foreach (stdarch, sinc);
    
    arrival->preprocess( stdarch );
    arrival->set_standard( stdarch );
  }
  catch (Error& error)
  {
    cerr << "\n" "Error processing standard profile:" << error << endl;
    return -1;
  }

  // Give format information for Tempo2 output 

  if (strcasecmp(outFormat.c_str(),"tempo2")==0 && !phase_info)
#if HAVE_PGPLOT
      if (!plot_difference)
#endif
          cout << "FORMAT 1" << endl;

  for (unsigned i = 0; i < archives.size(); i++) try {

    if (verbose)
      cerr << "Loading " << archives[i] << endl;
      
    arch = Archive::load(archives[i]);
    if (i==0 && gaussian)
    {
      loadGaussian(gaussFile, stdarch, arch);
      arrival->set_standard (stdarch);
    }

    if (fscrunch)
      arch->fscrunch();
    if (tscrunch)
      arch->tscrunch();
    
    if (full_freq)
    {
      if (stdarch->get_nchan() < arch->get_nchan())
	arch->fscrunch(arch->get_nchan() / stdarch->get_nchan());
      else if (stdarch->get_nchan() > arch->get_nchan())
	stdarch->fscrunch(stdarch->get_nchan() / arch->get_nchan());
    }

    arrival->preprocess (arch);

    /* If multiple standard profiles given must now choose and load 
       the one closest in frequency */
    if (std_multiple)
    {	  
      double freq = arch->get_centre_frequency();
      double minDiff=0.0;
      int    jDiff=0;
      unsigned j;
      for (j = 0;j < stdprofiles.size();j++)	    
      {
	stdarch = Archive::load(stdprofiles[j]);	      
	if (j==0 || fabs(stdarch->get_centre_frequency() - freq)<minDiff)
	{
	  minDiff = fabs(stdarch->get_centre_frequency()-freq);
	  jDiff   = j;
	}
      }
      stdarch = Archive::load(stdprofiles[jDiff]);
      stdarch->fscrunch();
      stdarch->tscrunch();
      
      if (sinc)
	Pulsar::foreach (stdarch, sinc);
      
      stdarch->pscrunch ();
      arrival->set_standard (stdarch);
    }

#if HAVE_PGPLOT
    if (centre_template_peak) {
      stdarch->centre_max_bin(0.5);
    }
#endif

    toas.resize (0);

    arrival->set_observation (arch);
    arrival->get_toas (toas);

    if (verbose)
      cerr << "pat: got " << toas.size() << " TOAs" << endl;

#if HAVE_PGPLOT
    if (plot_difference)
    {
      arch->remove_baseline();
      rotate_archive(arch, toas);
      plotDifferences(arch, stdarch, toas, min_phase, max_phase);
    }
#endif

    if (phase_only)
    {
      for (unsigned i = 0; i < toas.size(); i++)
      {
	unsigned isub = toas[i].get_subint();
	unsigned ichan = toas[i].get_channel();
	Integration* subint = arch->get_Integration(isub);
	double P = subint->get_folding_period();

	cout << arch->get_filename() << " " << isub << " " << ichan << " "
	     << toas[i].get_phase_shift () << " "
	     << toas[i].get_error()*1e-6 / P << endl;
      }
    }
    else
    {
        if (phase_info) {
            compute_dt(arch, toas, std);
        }

        if (tempo2_output)
        {
            vector<Tempo::toa>::iterator tit;
            for (tit = toas.begin(); tit != toas.end(); ++tit)
                (*tit).set_phase_info(true);
        }

        for (unsigned i = 0; i < toas.size(); i++)
            toas[i].unload(stdout);
    }
  }
  catch (Error& error)
  {
    fflush(stdout); 
    cerr << error << endl;
  }

#if HAVE_PGPLOT
  if (plot_difference)
    cpgend();
#endif

  fflush(stdout);
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}

//! Return the square of x
template<typename T> T sqr (T x) { return x*x; }

void loadGaussian(string file,  Reference::To<Archive> &stdarch,  Reference::To<Archive> arch)
{
  ifstream fin(file.c_str());
  string psr,str;
  char   buf[1000];
  float  tau,freq,spos,swidth,samp;
  static vector<float> pos,width,amp;
  static unsigned int    ncomp;
  float version;
  unsigned n=0;
  static bool firstTime=true;

  if (firstTime)
    {
      while (!fin.eof())
	{
	  fin >> str;
	  if (str.at(0)=='#') fin.getline(buf,1000);
	  else if (str.compare("Version:")==0) fin >> version;
	  else if (str.compare("Pulsar:")==0)  fin >> psr;
	  else if (str.compare("Freq")==0)     fin >> str >> freq;
	  else if (str.compare("Tau")==0)      fin >> str >> tau;
	  else if (str.compare("Components:")==0) fin >> ncomp;
	  else
	    {
	      fin >> spos >> samp >> swidth;  n++;
	      pos.push_back(spos);
	      amp.push_back(samp);
	      width.push_back(swidth);
	    }
	}
      if (n != ncomp)
	{
	  cerr << "Warning: number of Gaussian components " << 
	    ncomp << " does not equal components provided " << n << endl;
	  ncomp = n;
	}
    }
 
  stdarch = arch->clone();
  stdarch->fscrunch();
  stdarch->tscrunch();
  stdarch->pscrunch();
  
  // Now replace profile with Gaussian components
  Reference::To<Profile> prof = stdarch->get_Profile(0,0,0);
  prof->zero();

  float* amps = prof->get_amps();

  double degrees = 360.0;

  for (double offset = -degrees; offset <= degrees; offset += degrees)
  {
    for (unsigned i=0;i<stdarch->get_nbin();i++)
    {
      double x = double(i)/double(stdarch->get_nbin()) * degrees - offset;
      double y = 0.0;

      for (unsigned j=0;j<ncomp;j++)
	y+=amp[j]*exp(-sqr(x-pos[j])/sqr(width[j]));

      amps[i] += y;
    }
  }

  if (firstTime)
    for (unsigned i=0;i<stdarch->get_nbin();i++)
      cout << i << " " << amps[i] << " PROFILE " << endl;

  firstTime = false;
}


/**
 * @brief Plot three profiles on one page:
 *        - difference between the template and profile
 *        - template
 *        - archive for each subint and channel
 *
 * @param arch Target archive whose profiles will be subtracted from the
 *             template.
 * @param stdarch Template archive.
 * @param min_phase min x-value when zooming
 * @param max_phase max x-value when zooming
 */

#if HAVE_PGPLOT
void plotDifferences(Reference::To<Archive> arch,
        Reference::To<Archive> stdarch, vector<Tempo::toa>& toas,
        const double min_phase, const double max_phase)
{
    // remove baseline for all templates (except caldelay)
    const bool cal_delay_file = arch->get_source() == "CalDelay";
    if (!cal_delay_file)
        stdarch->remove_baseline();

    // difference between template and profile
    Reference::To<Archive> profile_diff = Archive::new_Archive("PSRFITS");

    // current profile
    Reference::To<Archive> profile_archive = Archive::new_Archive("PSRFITS");
    resize_archives(profile_archive, profile_diff, arch);

    Pulsar::PlotFactory factory;
    Reference::To<Plot> plotter = factory.construct("flux");

    // adjust x-range if zoom has been specified
    if (min_phase != 0.0 || max_phase != 1.0)
        plotter->configure("x:range=" + get_xrange(min_phase, max_phase));

    cpgsubp(1, 3);
    for (unsigned isub = 0; isub < arch->get_nsubint(); ++isub) {
        for (unsigned ichan = 0; ichan < arch->get_nchan(); ++ichan) {
            cpgpage();
            Pulsar::Profile* profile = arch->get_Profile(isub, 0, ichan);
            if (cal_delay_file)
                scaleProfile(profile);

            double freq = profile->get_centre_frequency();
            freq = truncateDecimals(freq, 3);

            diff_profiles(profile_diff, stdarch, profile);
            difference_plot(plotter, arch, profile_diff, toas, isub, ichan);
            template_plot(plotter, stdarch);
            profile_plot(plotter, profile_archive, profile, arch, freq);
        }
    }
    cout << "Plotting " << arch->get_filename() << endl;
}
#endif

/**
 * @brief compute delta time using the phase shift and start time offset
 * @param archive archive where the TOAs were calculated
 * @param toas vector of TOAs for each subint and chan
 * @param std_name filename of the standard template
 */

double get_stt_offs (const Archive* arch)
{
  const FITSHdrExtension* ext = arch->get<FITSHdrExtension>();
  if (ext)
    return ext->get_stt_offs();

  throw Error (InvalidParam, "get_stt_offs",
	       arch->get_filename() + " does not contain a FITSHdrExtension");
}

double get_period (const Archive* arch)
{
  return arch->get_Integration(0)->get_folding_period();
}

void compute_dt(Reference::To<Archive> archive, vector<Tempo::toa>& toas,
        string std_name)
{
    const double period = get_period(archive);
    const double stt_offs = get_stt_offs(archive);

    vector<Tempo::toa>::iterator tit;
    for (tit = toas.begin(); tit != toas.end(); ++tit) {
        const double phaseShift = (*tit).get_phase_shift();
        double dt = phaseShift * period;

        dt += stt_offs;
        dt *= 1000.0; // ms
        dt = fmod(dt + 10.0, 1.0); // to allow for dt < 1.0

        if (dt > 0.5)
            dt -= 1.0;

        dt *= 1000.0;  // microsec

        // remove preceeding path to shorten output line
        string::size_type pos = std_name.find_last_of('/');
        if (pos != string::npos)
            std_name = std_name.substr(pos + 1, std_name.length() - pos);

        cout << fixed << archive->get_filename() << " " <<  std_name << " " <<
            (*tit).get_subint() << " " << (*tit).get_channel() << " ";

        cout << setprecision(9) << stt_offs << " "; 
        cout << setprecision(7) << phaseShift << " "; 

        cout << setprecision(3) << dt << " " << setprecision(3) <<
            (*tit).get_error() << endl;
    }
}


/**
 * @param min min x-value
 * @param max max x-value
 * @throws InvalidRange if input min >= max
 * @returns the x:range string command to implement zoom 
 */

string get_xrange(const double min, const double max)
{
    if (min >= max)
        throw Error(InvalidRange, "set_phase_zoom", "min (%g) >= max (%g)",
                min, max);

    return string("(") +
        tostring<double>(min) +
        string(")") +
        tostring<double>(max) +
        string( ")");
}


/**
 * @brief resize the new archives (for plotting) to match the original
 *        archive
 */

void resize_archives(Reference::To<Archive> archive,
        Reference::To<Archive> diff, Reference::To<Archive> original)
{
    const unsigned nsub = original->get_nsubint();
    const unsigned nchan = original->get_nchan();
    const unsigned npol = original->get_npol();
    const unsigned nbin = original->get_nbin();

    diff->resize(nsub, npol, nchan, nbin);
    archive->resize(nsub, npol, nchan, nbin);
}


/**
 * @brief rotate each profile in the archive by the corresponding phase shift
 * @param archive archive whose profiles will be rotated
 * @param toas vector of TOAs for each subint and chan
 */

void rotate_archive(Reference::To<Archive> archive, vector<Tempo::toa>& toas)
{
  const unsigned nchan = archive->get_nchan();
  const unsigned nsub = archive->get_nsubint();

  vector<Tempo::toa>::iterator it = toas.begin();
  for (unsigned isub = 0; isub < nsub; ++isub) {
    for (unsigned ichan = 0; ichan < nchan; ++ichan) {

      if (isub == (*it).get_subint() && ichan == (*it).get_channel()) {
        const double phase_shift = (*it).get_phase_shift();
        archive->get_Profile(isub, 0, ichan)->rotate_phase(phase_shift);
        ++it;
      }
    }
  }
}


/**
 * @brief copy the frequency (3dp) and source name from one archive to another
 * @param prof profile of the current plotted profile
 */

void prepare_difference_archive(Reference::To<Archive> diff,
        Reference::To<Archive> arch, Reference::To<Profile> prof)
{
    const double freq = truncateDecimals(prof->get_centre_frequency(), 3);
    diff->set_source(arch->get_source());
    diff->set_centre_frequency(freq);
}

/**
 * @brief set up and plot the difference profile
 */

#if HAVE_PGPLOT
void difference_plot(Reference::To<Plot> plot,
        Reference::To<Archive> arch, Reference::To<Archive> diff_arch,
        vector<Tempo::toa>& toas, const unsigned subint, const unsigned chan)
{
    Reference::To<Profile> profile = arch->get_Profile(subint, 0, chan);
    prepare_difference_archive(diff_arch, arch, profile);

    const unsigned i = subint * arch->get_nchan() + chan;
    const double snr = diff_arch->get_Profile(0,0,0)->snr();

    char phaseShift[50];
    char phaseError[50];
    sprintf(phaseShift, "%.4f", toas[i].get_phase_shift());
    sprintf(phaseError, "%.4g", toas[i].get_error());

    // set the heading to read:
    // filename, subint, channel, snr, phase shift, phase error
    plot->configure("above:c=File: " + arch->get_filename() +
            "\nSubint: " + tostring(subint) + " Chan: " + tostring(chan) +
            " S/N: " + tostring(snr) + " Phase Shift: " + phaseShift +
            " Phase Error: " + phaseError);

    plot->configure("ch=2");
    plot->plot(diff_arch);
    cpgpage();
}


/**
 * @brief set up and plot the standard template
 */

void template_plot(Reference::To<Plot> plot, Reference::To<Archive> arch)
{
    plot->configure("above:c=Template: " + arch->get_filename());
    plot->plot(arch);
    cpgpage();
}

void profile_plot(Reference::To<Plot> plot,
        Reference::To<Archive> profile_archive,
        Reference::To<Profile> profile_to_copy, Reference::To<Archive> archive,
        const double freq)
{
    *(profile_archive->get_Profile(0,0,0)) = *profile_to_copy;

    profile_archive->set_source(archive->get_source());
    profile_archive->set_centre_frequency(freq);

    plot->configure("above:c=Profile");
    plot->plot(profile_archive);
}

/**
 * @brief calculate the difference between the template and current profile
 *        and store it as a separate profile - 'diff'
 */

void diff_profiles(Reference::To<Archive> diff, Reference::To<Archive> stdarch, Reference::To<Profile> profile)
{
    Reference::To<Profile> std_profile = stdarch->get_Profile(0,0,0);
    const double stdarch_area = std_profile->sum();
    const double profile_area = profile->sum();
    const double scale = stdarch_area / profile_area;
    profile->scale(fabs(scale));

    *(diff->get_Profile(0,0,0)) = *std_profile;
    diff->get_Profile(0,0,0)->diff(profile);
}

void scaleProfile(Reference::To<Profile> profile)
{
    float* bins = profile->get_amps();
    const unsigned nbin = profile->get_nbin();

    // if baseline removal is not effective, subtract the mean from the profile
    double stats_mean = profile->mean(1.0);

    if (profile->min(0, profile->get_nbin()) > 0.0)
        *profile -= stats_mean;

    // calculate mean and rms from all points < 3 * rms
    // subtract mean from all points
    // recompute mean and rms of all points < 3 * rms
    // repeat until delta_mean < 0.01*RMS

    float mean = 0.0;
    float deviation = 0.0;
    float oldMean = 0.0;

    while (true) {
        mean = getMean(deviation, bins, nbin);
        deviation = getRms(bins, nbin, mean, deviation);

        if (fabs(mean - oldMean) < 0.01 * fabs(deviation)) {
            *profile -= mean;
            break;
        }

        *profile -= mean;
        oldMean = mean;
    }
}


/**
 * @brief calculate new mean for all points < 3(rms)
 */

float getMean(const float rms, const float* bins, const unsigned nbin)
{
    float mean = 0.0;
    int count = 0;

    // calculate mean
    for (unsigned i = 0; i < nbin; ++i) {
        if (bins[i] < 3.0 * rms) {
            mean += bins[i];
            ++count;
        }
    }

    mean /= (float)count;
    return mean;
}


/**
 * @brief calculate new rms for all points < 3(rms)
 */

float getRms(const float* bins, const unsigned nbin, const float mean,
        const float oldRms)
{
    float deviation = 0.0;
    int count = 0;

    for (unsigned i = 0; i < nbin; ++i) {
        if (bins[i] < 3.0 * oldRms) {
            deviation += sqr(bins[i] - mean);
            ++count;
        }
    }

    deviation /= (float)count;
    return sqrt(deviation);
}
#endif // if HAVE_PGPLOT


/**
 * @brief a (stupid?) way to remove decimal places from a number
 * @param d floating-point number to be truncated
 * @param decimalPlaces number of decimal places to truncate to
 * @returns a truncated floating-point number
 */

double truncateDecimals(double d, int decimalPlaces)
{
    double result = d * pow(10, (double)decimalPlaces);
    result = (int)result;
    result /= pow(10, (double)decimalPlaces);

    return result;
}
