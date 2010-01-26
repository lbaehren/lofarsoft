/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "MEAL/Polar.h"

#include "Pulsar/StokesPlot.h"

#include "strutil.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <unistd.h>

using namespace std;

void usage ()
{
  cerr << 
    "psrdiff - measure the difference between two pulse profiles \n"
    "\n"
    "psrdiff [options] filename[s]\n"
    "options:\n"
    " -d               StokesPlot difference\n"
    " -c min_chisq     StokesPlot difference only when reduced chisq > min \n"
    " -h               Help page \n"
    " -M metafile      Specify list of archive filenames in metafile \n"
    " -q               Quiet mode \n"
    " -v               Verbose mode \n"
    " -V               Very verbose mode \n"
    "\n"
       << endl;
}

int main (int argc, char** argv) try
{
  Pulsar::Profile::default_duty_cycle = 0.10;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of the archive containing the standard
  char* std_filename = NULL;

  // write the difference to stdout
  bool plot = false;
  float plot_when = 0;

  bool verbose = false;

  char c;
  while ((c = getopt(argc, argv, "c:dhM:qs:vV")) != -1) 

    switch (c)
    {
    case 'c':
      plot_when = atof(optarg);
      break;

    case 'd':
      plot = true;
      break;

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 's':
      std_filename = optarg;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity (2);
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    } 


  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    cerr << "psrdiff: please specify filename[s]" << endl;
    return -1;
  } 

  if (!std_filename)
  {
    cerr << "psrdiff: please specify standard (-s std.ar)" << endl;
    return -1;
  } 

  Pulsar::Archive* std_archive = Pulsar::Archive::load( std_filename );

  unsigned std_nsub = std_archive->get_nsubint();
  unsigned std_nchan = std_archive->get_nchan();
  unsigned std_npol = std_archive->get_npol();
  unsigned std_nbin = std_archive->get_nbin();

  if (std_nsub > 1)
    cerr << "psrdiff: warning! standard has more than one subint" << endl;

  std_archive->convert_state (Signal::Stokes);
  std_archive->remove_baseline ();

  Pulsar::Integration* std_subint = std_archive->get_Integration(0);

  vector< vector< double > > std_variance;
  std_subint->baseline_stats (0, &std_variance);

  Pulsar::PolnProfileFit fit;
  fit.choose_maximum_harmonic = true;

  if (verbose)
    fit.set_fit_debug();

  MEAL::Polar* polar = new MEAL::Polar;

  fit.set_transformation( polar );

  Pulsar::StokesPlot splot;

  if (plot || plot_when)
  {
    cpgopen ("?");
    cpgsvp (0.1, 0.9, 0.15, 0.9);
    cpgask (1);
  }

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );

    unsigned nsub = archive->get_nsubint();
    unsigned nchan = archive->get_nchan();
    unsigned npol = archive->get_npol();
    unsigned nbin = archive->get_nbin();

    if (nbin != std_nbin)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nbin=" << nbin
	   << " != standard nbin=" << std_nbin << endl;
      continue;
    }

    if (nchan != std_nchan)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nchan=" << nchan
	   << " != standard nchan=" << std_nchan << endl;
      continue;
    }

    if (npol != std_npol)
    {
      cerr << "psrdiff: " << archive->get_filename() << "\n    npol=" << npol
	   << " != standard npol=" << std_npol << endl;
      continue;
    }

    archive->convert_state (Signal::Stokes);
    archive->remove_baseline ();

    unsigned isub=0;

    double total_chisq = 0.0;
    unsigned count = 0;

    for (isub=0; isub < nsub; isub++)
    {
      Pulsar::Integration* subint = archive->get_Integration(isub);

      vector<float> fit_chisq (nchan, 0.0);

      cerr << "psrdiff: performing fit in " << nchan << " channels" << endl;

      for (unsigned ichan=0; ichan < nchan; ichan++) try
      {
	if (std_subint->get_weight(ichan) == 0 ||
	    subint->get_weight(ichan) == 0)
	  continue;

	Reference::To<Pulsar::PolnProfile> std_profile;
	std_profile = std_subint->new_PolnProfile (ichan);

	Reference::To<Pulsar::PolnProfile> profile;
	profile = subint->new_PolnProfile (ichan);
	
	// polar->set_rotationEuler (0, M_PI/2);

	fit.set_standard (std_profile);
        fit.fit (profile);
  
	unsigned nfree = fit.get_equation()->get_solver()->get_nfree ();
	float chisq = fit.get_equation()->get_solver()->get_chisq ();

	float reduced_chisq = chisq / nfree;

	if (verbose)
	  cerr << ichan << " REDUCED CHISQ=" << reduced_chisq << endl;

	fit_chisq[ichan] = reduced_chisq;

	total_chisq += reduced_chisq;
	count ++;

	bool plot_this = (plot || (plot_when && reduced_chisq > plot_when));

	Estimate<double> pulse_phase = fit.get_phase ();

	MEAL::Complex2* xform = fit.get_transformation();

	if (verbose)
	  for (unsigned i=0; i<xform->get_nparam(); i++)
	    cerr << i << ":" << xform->get_param_name(i) << "=" 
		 << xform->get_Estimate(i) << endl;

	Jones<double> transformation = xform->evaluate();
	
	if (verbose)
	  cerr << ichan << " shift=" << pulse_phase << endl;

	profile->rotate_phase (pulse_phase.get_value());
	profile->transform (inv(transformation));

	if (plot_this) {

	  profile->diff(std_profile);

	  Pulsar::Profile::rotate_in_phase_domain = false;
	  profile->rotate_phase (.5);

	  cpgpage();
	  splot.set_subint (isub);
	  splot.set_chan (ichan);
	  splot.plot(archive);

	}

      }
      catch (Error& error) {
	cerr << error << endl;
      }

    }

    total_chisq /= count;

    double alt_chisq = 0.0;
    count = 0;

    for (isub=0; isub < nsub; isub++)
    {
      Pulsar::Integration* subint = archive->get_Integration(isub);

      vector< vector< double > > variance;
      subint->baseline_stats (0, &variance);

      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	if (std_subint->get_weight(ichan) == 0 ||
	    subint->get_weight(ichan) == 0)
	  continue;

	for (unsigned ipol=0; ipol < npol; ipol++)
	{
	  double var = variance[ipol][ichan] + std_variance[ipol][ichan];

	  float* std_amps = std_subint->get_Profile(ipol,ichan)->get_amps();
	  float* amps = subint->get_Profile (ipol,ichan)->get_amps();

	  double diff = 0;
	  for (unsigned ibin = 0; ibin < nbin; ibin++)
	  {
	    double val = std_amps[ibin] - amps[ibin];
	    diff += val * val;
	  }

	  double reduced_chisq = diff / (nbin * var);

	  if (verbose)
	    cout << ichan << " ipol=" << ipol
		 << " var=" << var << " chisq=" << reduced_chisq << endl;

	  alt_chisq += reduced_chisq;
	  count ++;
	}

      }

    }

    alt_chisq /= count;

    cout << archive->get_filename() << " " << total_chisq << " " << alt_chisq
	 << endl;
    
  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl
	 << error.get_message() << endl;
  }

  if (plot || plot_when)
    cpgend();

  return 0;

}
catch (Error& error)
{
  cerr << "psrdiff: error" << error << endl;
  return -1;
}

