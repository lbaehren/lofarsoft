/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#define PGPLOT 1

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/DoPCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Pulsar/Feed.h"

#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorSpectrum.h"

#include "Pulsar/CalibratorStokesInfo.h"
#include "Pulsar/SolverInfo.h"

#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

#include "Pauli.h"
#include "strutil.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <iostream>
#include <unistd.h>

using namespace std;

void usage ()
{
  cout << "pacv - Pulsar Archive Calibrator Viewer\n"
    "usage: pacv [options] file1 [file2 ...]\n"
    "options:\n"
    " -n name      add the named plot to the output \n"
    "              name = three-character string: [which][what][when]  \n"
    "                 which = 'c' for CAL (on-pulse minus off-pulse) \n"
    "                       = 's' for SYS (off-pulse) \n"
    "                 what  = 's' for Stokes parameters \n"
    "                       = 'p' for total and polarized flux \n"
    "                 when  = 'c' for calibrated data \n"
    "                       = 'u' for uncalibrated data \n"
    " -N           plot the calibrator solution [default] \n"
    " -a archive   set the output archive class name\n"
    " -c [i|j-k]   mark channel or range of channels as bad\n"
    " -C           plot only calibrator Stokes\n"
    " -D dev       specify PGPLOT device\n"
    " -d           use the Degree of Polarization Calibrator\n"
    " -f           treat all archives as members of a fluxcal observation\n"
    " -j           print Jones matrix elements of calibrator solution \n"
    " -p           use the polar model\n"
    " -P           produce publication-quality plots\n"
    " -r feed.txt  set the feed transformation [not used] \n"
    " -s           plot only the reduced chisq of the pcm solution \n"
    " -S pcm.out   combine each calibrator with the pcm solution \n"
    " -u           unload the derived calibrator \n"
    " -2 m or d    multiply or divide cross products by factor of two \n"
       << endl;
}

int main (int argc, char** argv) 
{
  // report the Mean for each plot
  EstimatePlotter::report_mean = true;

  // use the Single Axis model
  bool single_axis = true;

  // use the Degree of Polarization Calibrator
  bool dop_calibrator = false;

  bool CAL[2][2] = { { false, false }, { false, false } };
  bool SYS[2][2] = { { false, false }, { false, false } };

  const unsigned Stokes = 0;
  const unsigned Ip = 1;

  // treat all of the Archives as one FluxCalibrator observation set
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  // filename of filenames
  char* metafile = NULL;

  // class name of the special calibrator solution archives to be produced
  string archive_class = "PSRFITS";

  // vector of bad channels
  vector<unsigned> zapchan;

  // produce publication quality plots
  bool publication = false;

  bool plot_derived_calibrator = false;
  bool unload_derived_calibrator = false;

  bool plot_calibrator_solution = true;
  bool plot_calibrator_stokes = false;
  bool plot_calibrator_solver = false;

  bool plot_specified = false;
  bool print_titles = true;

  bool print_jones = false;
  bool print_mueller = false;

  //
  float cross_scale_factor = 1.0;

  // known feed transformation
  Calibration::Feed* feed = 0;

  // Hybrid transformation
  Reference::To<Pulsar::HybridCalibrator> hybrid;

  string device = "?";

  // verbosity flag
  bool verbose = false;
  char c;

  while ((c = getopt(argc, argv, "2:a:c:CD:dfhjM:mn:Ppr:S:stuqvV")) != -1)
  {
    switch (c)
    {
    case '2':
      if (optarg[0] == 'm')
	cross_scale_factor = 2.0;
      else if (optarg[0] == 'd')
	cross_scale_factor = 0.5;
      else
      {
	cerr << " -2 " << optarg << " not recognized" << endl;
	return -1;
      }
      break;

    case 'h':
      usage();
      return 0;

    case 'a':
      archive_class = optarg;
      break;

    case 'c':
    {
      unsigned ichan1 = 0;
      unsigned ichan2 = 0;

      if (sscanf (optarg, "%u-%u", &ichan1, &ichan2) == 2)
	for (unsigned ichan=ichan1; ichan<=ichan2; ichan++)
	  zapchan.push_back(ichan);

      else if (sscanf (optarg, "%u", &ichan1) == 1)
	zapchan.push_back(ichan1);

      else
      {
	cerr << "pacv: Error parsing " << optarg << " as zap range" << endl;
	return -1;
      }

      break;
    }
      
    case 'C':
      plot_calibrator_stokes = true;
      plot_calibrator_solution = false;
      break;

    case 'D':
      device = optarg;
      break;

    case 'd':
      dop_calibrator = true;
      break;

    case 'f':
      fluxcal = new Pulsar::FluxCalibrator;
      break;

    case 'j':
      print_jones = true;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'm':
      print_mueller = true;
      break;

    case 'N':
      plot_calibrator_solution = true;
      break;

    case 'n':
    {
      unsigned when = 0;
      switch (optarg[2])
      {
      case 'c':
	when = 1; break;
      case 'u':
	when = 0; break;
      default:
	cerr << "invalid plot 'when' code=" << optarg[2] << endl; return -1;
      }

      unsigned what = 0;
      switch (optarg[1])
      {
      case 's':
	what = Stokes; break;
      case 'p':
	what = Ip; break;
      default:
	cerr << "invalid plot 'what' code=" << optarg[1] << endl; return -1;
      }

      switch (optarg[0])
      {
      case 'c':
	CAL[what][when] = true; break;
      case 's':
	SYS[what][when] = true; break;
      default:
	cerr << "invalid plot 'which' code=" << optarg[0] << endl; return -1;
      }

      plot_specified = true;
      break;
    }

    case 'P':
      publication = true;
      print_titles = false;
      break;

    case 'p':
      single_axis = false;
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
      break;

    case 'S':
    {
      Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(optarg);
      hybrid = new Pulsar::HybridCalibrator (data);
      break;
    }

    case 's':
      plot_calibrator_solver = true;
      plot_calibrator_solution = false;
      break;

    case 't':
      print_titles = true;
      break;

    case 'u':
      unload_derived_calibrator = true;
      plot_calibrator_solution = false;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::CalibratorPlotter::verbose = true;
      Pulsar::Calibrator::verbose = 3;
      verbose = true;
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      Pulsar::Calibrator::verbose = 2;
      verbose = true;
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    } 
  }

  if (!plot_specified)
    plot_derived_calibrator = true;

  if (!metafile && optind >= argc) {
    cerr << "pacv requires a list of archive filenames as parameters.\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (!(print_jones || print_mueller || unload_derived_calibrator))
  {
    cpgbeg (0, device.c_str(), 0, 0);
    cpgask(1);
    cpgsvp (.1,.9, .1,.9);
  }

  // the input calibrator archive
  Reference::To<Pulsar::Archive> input;
  // the output calibrator archive
  Reference::To<Pulsar::Archive> output;
 
  Reference::To<Pulsar::PolnCalibrator> calibrator;

  Reference::To<Pulsar::CalibratorStokes> calibrator_stokes;
  
  Pulsar::CalibratorPlotter plotter;

  if (publication)
  {
    plotter.npanel = 5;
    plotter.between_panels = 0.08;
    cpgsvp (.25,.75,.1,.9);
    cpgslw (3);
    cpgscf (2);
  }

  Pulsar::CalibratorSpectrum archplot;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    if (verbose)
      cerr << "pacv: Loading " << filenames[ifile] << endl;

    input = Pulsar::Archive::load( filenames[ifile] );

    plotter.print_titles = print_titles;
    plotter.title = filenames[ifile];

    if (input->get_type() == Signal::Calibrator)
    {
      cerr << "pacv: " << filenames[ifile] << " is a processed Calibrator"
           << endl;

      if (input->get<Pulsar::FluxCalibratorExtension>())
      {

        cerr << "pacv: constructing FluxCalibrator from Extension" << endl;
        fluxcal = new Pulsar::FluxCalibrator (input);

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  fluxcal->set_invalid (zapchan[ichan]);

        if (plot_calibrator_stokes) {

          calibrator_stokes = fluxcal->get_CalibratorStokes();

          for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
            calibrator_stokes->set_valid (zapchan[ichan], false);

          cerr << "pacv: Plotting fluxcal-derived CalibratorStokes" << endl;
          cpgpage ();
          plotter.plot( new Pulsar::CalibratorStokesInfo (calibrator_stokes),
                        fluxcal->get_nchan(),
                        fluxcal->get_Archive()->get_centre_frequency(),
                        fluxcal->get_Archive()->get_bandwidth() );

          calibrator_stokes = 0;
            
        } else {

          cerr << "pacv: Plotting FluxCalibrator" << endl;
          cpgpage ();
          plotter.plot (fluxcal);

        }

        // disable attempt to plot again after end of main loop
        fluxcal = 0;

        continue;
      }

      calibrator = new Pulsar::PolnCalibrator (input);

      cerr << "pacv: Archive Calibrator with nchan=" 
	   << calibrator->get_nchan() << endl;

      if (print_jones || print_mueller)
      {
	cerr << "pacv: Printing Jones matrix elements" << endl;
	for (unsigned ichan=0; ichan<calibrator->get_nchan(); ichan++)
	{
	  if (!calibrator->get_transformation_valid (ichan))
	    continue;

	  Jones<double> xform;
	  xform = calibrator->get_transformation(ichan)->evaluate();

	  if (print_jones)
	    cout << ichan << " " << xform << endl;

	  if (print_mueller)
	  {
	    cout << ichan;
	    Matrix<4,4,double> M = Mueller (xform);
	    for (unsigned i=0; i<4; i++)
	      for (unsigned j=0; j<4; j++)
		cout << " " << M[i][j];

	    cout << endl;
	  }
	}
	continue;
      }

      if (plot_calibrator_solution)
      {
	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  calibrator->set_transformation_invalid (zapchan[ichan]);
	
	if (verbose)
	  cerr << "pacv: Plotting PolnCalibrator" << endl;
	
	cpgpage ();
	plotter.plot (calibrator);
      }

      if (plot_calibrator_stokes)
      {
        calibrator_stokes = input->get<Pulsar::CalibratorStokes>();

	cerr << "pacv: Plotting CalibratorStokes" << endl;

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  calibrator_stokes->set_valid (zapchan[ichan], false);

	cpgpage ();
	plotter.plot( new Pulsar::CalibratorStokesInfo (calibrator_stokes),
		      calibrator->get_nchan(),
		      calibrator->get_Archive()->get_centre_frequency(),
		      calibrator->get_Archive()->get_bandwidth() );
      }

      if (plot_calibrator_solver)
      {
	cerr << "pacv: Plotting SystemCalibrator solver" << endl;

	for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
	  calibrator->get_solver(zapchan[ichan])->set_solved(false);

	cpgpage ();
	plotter.plot( new Pulsar::SolverInfo (calibrator),
		      calibrator->get_nchan(),
		      calibrator->get_Archive()->get_centre_frequency(),
		      calibrator->get_Archive()->get_bandwidth() );
      }

      continue;
    }

    for (unsigned ichan=0; ichan<zapchan.size(); ichan++)
    {
      if (verbose)
	cerr << "pacv: Zapping channel " << zapchan[ichan] << endl;

      for (unsigned isub=0; isub<input->get_nsubint(); isub++)
	input->get_Integration(isub)->set_weight (zapchan[ichan], 0.0);
    }

    if (cross_scale_factor != 1.0)
    {
      cerr << "Scaling cross products by " << cross_scale_factor << endl;

      for (unsigned isub=0; isub < input->get_nsubint(); isub++)
	for (unsigned ichan=0; ichan < input->get_nchan(); ichan++)
	{
	  input->get_Profile (isub, 2, ichan) -> scale (cross_scale_factor);
	  input->get_Profile (isub, 3, ichan) -> scale (cross_scale_factor);
	}
    }

    if (fluxcal)
    {
      if (verbose)
	cerr << "pacv: Adding Archive to FluxCalibrator" << endl;
      
      fluxcal->add_observation (input);
      continue;
    }

    input->convert_state (Signal::Stokes);

    string cal = "uncalibrated";

    for (unsigned ical=0; ical < 2; ical++)
    {
      if (CAL[Stokes][ical])
      {
	cerr << "pacv: Plotting " << cal << " CAL Stokes parameters" << endl;
	cpgpage ();
	archplot.plot (input);
      }
      
      if (CAL[Ip][ical])
      {
	cerr << "pacv: Plotting " << cal << " CAL total and polarized flux"
	     << endl;
	cpgpage ();
	archplot.set_plot_Ip (true);
	archplot.plot (input);
	archplot.set_plot_Ip (false);
      }
      
      if (SYS[Stokes][ical])
      {
	cerr << "pacv: Plotting " << cal << " SYS Stokes parameters"
	     << endl;
	cpgpage ();
	archplot.set_plot_low (true);    
	archplot.plot (input);
	archplot.set_plot_low (false);    
      }
      
      if (SYS[Ip][ical])
      {
	cerr << "pacv: Plotting " << cal << " SYS total and polarized flux"
	     << endl;
	cpgpage ();
	archplot.set_plot_Ip (true);
	archplot.set_plot_low (true);    
	archplot.plot (input);
	archplot.set_plot_Ip (false);
	archplot.set_plot_low (false);    
      }
      
      if (ical)
	break;
      
      if (verbose)
	cerr << "pacv: Constructing PolnCalibrator" << endl;
      
      if (hybrid)
      {
	hybrid -> set_reference_observation 
	  ( new Pulsar::SingleAxisCalibrator (input) );
	calibrator = hybrid;
      }
      else if (dop_calibrator)
	calibrator = new Pulsar::DoPCalibrator (input);
      else if (single_axis)
	calibrator = new Pulsar::SingleAxisCalibrator (input);
      else
	calibrator = new Pulsar::PolarCalibrator (input);
      
      if (verbose)
	cerr << "pacv: Calibrating Archive" << endl;
      
      calibrator -> calibrate (input);

      if (plot_derived_calibrator)
      {
	cerr << "pacv: Plotting calibrator solution parameters" << endl;
	cpgpage ();
	plotter.plot (calibrator);
      }

      if (dop_calibrator)
	continue;

      if (unload_derived_calibrator)
      {
	output = calibrator->new_solution (archive_class);

	string newname = replace_extension (filenames[ifile], "pacv");
	cerr << "pacv: Unloading solution " << newname << endl;
	output -> unload (newname);
      }
    }
  }
  catch (Error& error)
  {
    cerr << "pacv: Error during " << filenames[ifile] << error << endl;
  }
  
  if (fluxcal) try
  {
    cerr << "pacv: Plotting FluxCalibrator" << endl;
    plotter.plot (fluxcal);

    cerr << "pacv: Creating " << archive_class << " Archive" << endl;

    output = fluxcal->new_solution (archive_class);

    cerr << "pacv: Creating new filename" << endl;
    int index = filenames[0].find_first_of(".", 0);
    string newname = filenames[0].substr(0, index) + ".fcal";

    cerr << "pacv: Unloading " << newname << endl;
    output -> unload (newname);
  }
  catch (Error& error) {
    cerr << "pacv: Error plotting FluxCalibrator" << error << endl;
    return -1;
  }

  cpgend();

  return 0;
}
