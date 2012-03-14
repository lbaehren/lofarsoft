/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FaradayRotation.h"

#if HAVE_PGPLOT
#include "Pulsar/PlotOptions.h"
#include "Pulsar/StokesCylindrical.h"
#endif

#include "Pulsar/ComplexRVMFit.h"
#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarParameter.h"

#include "pairutil.h"

using namespace Pulsar;
using namespace std;

//! Pulsar modeling application
class psrmodel: public Pulsar::Application
{
public:

  //! Default constructor
  psrmodel ();

  //! Add an orthogonally polarized mode
  void add_opm (const std::string& arg);

  //! Produce a chi-sqared map
  void map_chisq ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  void set_very_verbose () 
  { ComplexRVMFit::verbose = true; Application::set_very_verbose(); }

  // complex rotating vector model
  Reference::To<ComplexRVMFit> rvm;

  // perform a global search over first guesses in alpha and zeta
  string global_search;
  unsigned nalpha;
  unsigned nzeta;

  // perform a rotating vector model fit
  bool rvm_fit;

#if HAVE_PGPLOT

  // plot the result
  bool plot_result;
  void set_plot_result ()
  {
    plot_result = true; 
    plot.set_open_device (true);
  }

  // configures the plotting device
  Pulsar::PlotOptions plot;

#endif
};

int main (int argc, char** argv)
{
  psrmodel program;
  return program.main (argc, argv);
}

psrmodel::psrmodel () :
  Pulsar::Application ("psrmodel", "pulsar modeling program")
#if HAVE_PGPLOT
  , plot (false)
#endif
{
  has_manual = false;
  version = "$Id: psrmodel.C,v 1.13 2010/05/28 21:56:32 straten Exp $";

  add( new Pulsar::StandardOptions );
#if HAVE_PGPLOT
  add( &plot );
  plot_result = false;
#endif

  rvm = new ComplexRVMFit;
  rvm_fit = true;

  nalpha = nzeta = 0;
}

double deg_to_rad (const std::string& arg)
{
  return fromstring<double> (arg) * M_PI/180.0;
}

range range_deg_to_rad (const std::string& arg)
{
  range result = fromstring<range> (arg);
  result.first *= M_PI/180;
  result.second *= M_PI/180;

  return result;
}

void psrmodel::add_options (CommandLine::Menu& menu)
{
  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();
  rvm_fit = true;

  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

#if HAVE_PGPLOT
  arg = menu.add (this, &psrmodel::set_plot_result, 'd');
  arg->set_help ("plot the resulting model with data");
#endif

  arg = menu.add (rvm.get(), &ComplexRVMFit::set_threshold, 't', "sigma");
  arg->set_help ("cutoff threshold when selecting bins "
		 "[default " + tostring(rvm->get_threshold()) + "]");

  arg = menu.add (this, &psrmodel::add_opm, 'o', "deg0:deg1");
  arg->set_help ("window over which an orthogonal mode dominates");

  menu.add ("");

  arg = menu.add (RVM->magnetic_axis.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'a', "degrees");
  arg->set_help ("alpha: colatitude of magnetic axis");

  arg = menu.add (RVM->magnetic_axis.get(),
		  &MEAL::ScalarParameter::set_fit, 'A', false);
  arg->set_help ("hold alpha constant");

  arg = menu.add (RVM->line_of_sight.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'z', "degree");
  arg->set_help ("zeta: colatitude of line of sight");

  arg = menu.add (RVM->line_of_sight.get(),
		  &MEAL::ScalarParameter::set_fit, 'Z', false);
  arg->set_help ("hold zeta constant");

  arg = menu.add (RVM->magnetic_meridian.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'b', "degrees");
  arg->set_help ("phi0: longitude of magnetic meridian");

  arg = menu.add (RVM->magnetic_meridian.get(),
		  &MEAL::ScalarParameter::set_fit, 'B', false);
  arg->set_help ("hold phi0 constant");

  arg = menu.add (RVM->reference_position_angle.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'p', "degrees");
  arg->set_help ("psi0: position angle at magnetic meridian");

  arg = menu.add (RVM->reference_position_angle.get(),
		  &MEAL::ScalarParameter::set_fit, 'P', false);
  arg->set_help ("hold psi0 constant");

  menu.add ("\n" "chi^2 map options:");

  arg = menu.add (global_search, 's', "NxM");
  arg->set_help ("map an NxM grid in alpha X zeta/beta");

  arg = menu.add (this, &psrmodel::map_chisq, 'x');
  arg->set_help ("output map to stdout: alpha, zeta/beta, chi^2");

  arg = menu.add (rvm.get(), &ComplexRVMFit::set_range_alpha,
		  range_deg_to_rad, "alpha", "deg0:deg1");
  arg->set_help ("range of alpha on x-axis of grid");

  arg = menu.add (rvm.get(), &ComplexRVMFit::set_range_beta,
		  range_deg_to_rad, "beta", "deg0:deg1");
  arg->set_help ("range of beta on y-axis of grid");

  arg = menu.add (rvm.get(), &ComplexRVMFit::set_range_zeta,
		  range_deg_to_rad, "zeta", "deg0:deg1");
  arg->set_help ("range of zeta on y-axis of grid");

}



void psrmodel::map_chisq()
{
  rvm->set_chisq_map (true);
}


void psrmodel::add_opm (const std::string& arg)
{
  range opm = range_deg_to_rad (arg);
  cerr << "psrmodel: OPM at " << opm << " degrees" << endl;

  rvm->add_opm( opm );
}

void psrmodel::setup ()
{
  if (!rvm_fit)
    throw Error (InvalidState, "psrmodel",
		 "please use -r (can only do RVM fit for now)");

  if (!global_search.empty())
  {
    const char* str = global_search.c_str();
    char separator = 0;

    if ( sscanf (str, "%u%c%u", &nalpha, &separator, &nzeta) == 3 )
      {}
    else if ( sscanf (str, "%u", &nalpha) == 1 )
    {
      // avoid alpha == zeta pole crossing
      nzeta = nalpha - 1;
    }
    else
      throw Error (InvalidState, "psrmodel",
		   "cannot parse chi^2 map dimensions from '%s'", str);

    cerr << "psrmodel: search " << nalpha << "X" << nzeta << " grid" << endl;
  }
}

void psrmodel::process (Pulsar::Archive* data)
{
  if (verbose)
    cerr << "psrmodel: fitting " << data->get_filename() << endl;

  // correct PA to infinite frequency
  FaradayRotation xform;
  xform.set_reference_wavelength( 0 );
  xform.set_measure( data->get_rotation_measure() );
  xform.execute( data );

  data->tscrunch();
  data->fscrunch();
  data->convert_state(Signal::Stokes);
  data->remove_baseline();

  if (data->has_model())
    data->centre();

  Reference::To<PolnProfile> p = data->get_Integration(0)->new_PolnProfile(0);
  rvm->set_observation (p);

  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();
  double deg = 180/M_PI;

  if (nalpha && nzeta)
  {
    cerr << "psrmodel: performing search of chi^2 map" << endl;

    rvm->global_search (nalpha, nzeta);
  }
  else
  {
    cerr << "psrmodel: solving with initial guess: \n"
      "psi_0=" << deg*RVM->reference_position_angle->get_param(0) << " deg\n"
      "zeta =" << deg*RVM->line_of_sight->get_param(0) << " deg\n"
      "alpha=" << deg*RVM->magnetic_axis->get_param(0) << " deg\n"
      "phi_0=" << deg*RVM->magnetic_meridian->get_param(0) << " deg"
	 << endl;

    rvm->solve();
  }

  cerr << endl
       << "chisq=" << rvm->get_chisq() << "/nfree=" << rvm->get_nfree()
       << " = " << rvm->get_chisq()/ rvm->get_nfree() 
       << endl;

  cerr <<
    "psi_0=" << deg*RVM->reference_position_angle->get_value() << " deg\n"
    "zeta =" << deg*RVM->line_of_sight->get_value() << " deg\n"
    "alpha=" << deg*RVM->magnetic_axis->get_value() << " deg\n"
    "phi_0=" << deg*RVM->magnetic_meridian->get_value() << " deg"
	     << endl;

#if HAVE_PGPLOT
  if (plot_result)
  {
    StokesCylindrical plotter;

    AnglePlot* pa = plotter.get_orientation();

    pa->set_threshold( rvm->get_threshold() );
    pa->model.set (RVM, &MEAL::RotatingVectorModel::compute);
    pa->get_frame()->get_y_scale()->set_range_norm (0, 1.5);

    plotter.get_scale()->set_units( PhaseScale::Degrees );
    plotter.plot( data );
  }
#endif

}
