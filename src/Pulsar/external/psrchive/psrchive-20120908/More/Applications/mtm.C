/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileFitAnalysis.h"
#include "Pulsar/BoostShiftAnalysis.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Fourier.h"
#include "MEAL/Polar.h"

#include "Error.h"
#include "dirutil.h"

#include <fstream>
#include <iomanip>

#include <getopt.h>

using namespace std;
using namespace Pulsar;

void usage ()
{
  cout << "mtm - matrix template matching analysis \n"
    "Usage: mtm [options] filenames \n"
    "  -q            Quiet mode \n"
    "  -v            Verbose mode \n"
    "  -V            Very verbose mode \n"
    "\n"
    "Preprocessing options:\n"
    "  -F            Frequency scrunch template \n"
    "  -T            Time scrunch template \n"
    "\n"
    "Fitting options:\n"
    "  -c            Choose the maximum harmonic \n"
    "  -n harmonics  Use up to the specified number of harmonics\n"
    "\n"
    "Output options: \n"
    "  -b            Print the systematic phase shifts induced by boosts \n"
    "  -t            Output latex table like Figure 1 of van Straten (2006) \n"
       << endl;

}

static bool verbose = false;
static bool tabular = false;
static bool boost_analysis = false;

static string source;
static double folding_period = 0.0;

static vector< string > names;
static vector< sky_coord > coordinates;
static vector< Vector<3,double> > susceptibility;

void mtm_analysis (PolnProfileFit::Analysis& analysis,
		   PolnProfileFit& fit,
		   const std::string& name)
{
  if (verbose)
    cerr << "mtm: set fit" << endl;

  if (!fit.choose_maximum_harmonic && !fit.get_maximum_harmonic())
    cerr <<
      "\nWARNING: The maximum harmonic was not set (with -n) \n"
      "nor was it allowed to be automatically chosen (with -c). \n"
      "As described in Section 3.2 of van Straten (2006) [first full \n"
      "paragraph following Equation 17], noise artificially decreases \n"
      "the multiple correlation (which approaches zero) and \n"
      "the conditional variance (which approaches one half). \n";

  analysis.set_fit (&fit);

  if (tabular)
  {
    cout.setf(ios::fixed);
    cout << setprecision(2);

    cout << source 
	 << " & " << analysis.get_relative_conditional_error().get_value()
	 << " & " << analysis.get_multiple_correlation().get_value()
	 << " & " << analysis.get_relative_error().get_value();
  }
  else
  {
    if (fit.choose_maximum_harmonic)
      cout << "\nMaximum harmonic = " << fit.get_nharmonic() << endl;

    cout <<
      "\nThe following four values are those presented (in the same order) \n"
      "in Table 1 of van Straten (2006 ApJ, 642:1004)" << endl;

    cout << "\nFull Polarization TOA (matrix template matching): "
      "\n MTM Relative conditional error = "
	 << analysis.get_relative_conditional_error () <<
      "\n Multiple correlation = "
	 << analysis.get_multiple_correlation() << 
      "\n MTM Relative error = "
	 << analysis.get_relative_error () << endl;
  }

  ScalarProfileFitAnalysis scalar;
  scalar.set_fit (&fit);

  // the phase shift error for the total intensity profile
  Estimate<double> I_error = scalar.get_error();

  if (verbose)
    cerr << "I_error=" << I_error << endl;

  Profile invariant;
  fit.get_standard()->invint( &invariant );
  scalar.set_spectrum ( fourier_transform (&invariant), 2.0 );

  // the phase shift error for the invariant profile
  Estimate<double> S_error = scalar.get_error();

  if (verbose)
    cerr << "S_error=" << S_error << endl;

  if (tabular)
    cout << " & " << ( S_error/I_error ).get_value();
  else
    cout << "\nLorentz Invariant TOA: "
      "\n Invariant relative error = " << S_error/I_error << endl << endl;

  if (boost_analysis)
  {
    Pulsar::BoostShiftAnalysis boost;
    boost.set_profile (fit.get_standard());

    susceptibility.push_back( boost.dotvarphi() );
    names.push_back( name );

    double delshift = boost.delvarphi_delbeta ();

    // a 1% gain error
    double boost0 = 5e-3;

    // nanoseconds
    double ns = 1e9;

    if (tabular)
    {
      cout.unsetf(ios::floatfield);
      cout << " & " << setprecision(2) << delshift;

      cout.setf(ios::fixed);
      cout << setprecision(0)
	   << " & " << folding_period * delshift * ns * boost0;
    }
    else
      cout << "BOOST factor=" << delshift
	   << " or " << folding_period * delshift * ns * boost0
	   << " ns per unit of boost" << endl;
  }

  if (tabular)
    cout << " \\\\" << endl;
}

int main (int argc, char *argv[])
{
  bool fscrunch = false;
  bool tscrunch = false;

  bool choose_maximum_harmonic = false;
  unsigned maximum_harmonic = 0;

  int gotc = 0;

  while ((gotc = getopt(argc, argv, "bcFhn:qTtvV")) != -1) {
    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'q':
      Pulsar::Archive::set_verbosity(0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity(2);
      Pulsar::PolnProfileFit::Analysis::verbose = true;
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      MEAL::Function::verbose = true;
      break;

    case 'b':
      boost_analysis = true;
      break;

    case 'c':
      choose_maximum_harmonic = true;
      break;

    case 't':
      tabular = true;
      break;

    case 'i':
      cout << "$Id: mtm.C,v 1.5 2011/02/23 20:53:21 straten Exp $" << endl;
      return 0;

    case 'F':
      fscrunch = true;
      break;

    case 'n':
      maximum_harmonic = atoi(optarg);
      break;

    case 'T':
      tscrunch = true;
      break;

    default:
      cout << "Unrecognised option " << gotc << endl;

    }
  }

  vector<string> filenames;

  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);
  
  if (filenames.empty())
  {
    cerr << "mtm: no filenames were specified" << endl;
    return -1;
  } 
  
  if (tabular)
  {
    // five columns by default
    string columns = "lllll";
    // boost analysis adds two more
    if (boost_analysis)
      columns += "ll";

    cout << 
      "\\begin{table*} \n"
      "\\caption{Relative Arrival Time Uncertainties} \n"
      "\\begin{center} \n"
      "\\begin{tabular}{" << columns << "} \n"
      "\\tableline \n"
      "\\tableline \n"
      "Pulsar &"
      " $\\hat\\sigma_{\\varphi|{\\bf J}}$ &"
      " $R_{\\varphi{\\bf J}}$ & $\\hat\\sigma_{\\varphi}$ &"
      " $\\hat\\sigma_{\\tilde\\varphi}$ ";

    if (boost_analysis)
      cout <<
	" & $\\dot\\varphi_\\beta$ "
	" & $\\tau_\\beta$ (ns)";

    cout <<
      "\\\\ \n"
      "\\tableline \n"
	 << endl;
  }

  for (unsigned i = 0; i < filenames.size(); i++) try
  {     
    if (verbose)
      cerr << "mtm: loading " << filenames[i] << endl;
    
    Reference::To<Pulsar::Archive> arch = Pulsar::Archive::load(filenames[i]);

    if (verbose)
      cerr << "mtm: pre-processing" << endl;

    if (fscrunch)
      arch->fscrunch();
    if (tscrunch)
      arch->tscrunch();
    
    arch->remove_baseline();
    arch->convert_state( Signal::Stokes );

    source = arch->get_source();

    coordinates.push_back( arch->get_coordinates() );

    if (verbose)
      cerr << "mtm: creating new PolnProfile" << endl;

    Reference::To<Pulsar::PolnProfile> profile;
    profile = arch->get_Integration(0)->new_PolnProfile(0);

    Pulsar::PolnProfileFit::Analysis analysis;
    Pulsar::PolnProfileFit fit;
  
    fit.choose_maximum_harmonic = choose_maximum_harmonic;

    if (maximum_harmonic)
      fit.set_maximum_harmonic( maximum_harmonic );

    MEAL::Polar polar;

    if (verbose)
      cerr << "mtm: setting transformation = " << polar.evaluate() << endl;

    fit.set_transformation( &polar );

    if (verbose)
      cerr << "mtm: setting standard" << endl;

    fit.set_standard( profile );

    if (verbose)
      cerr << "mtm: running analysis" << endl;

    folding_period = arch->get_Integration(0)->get_folding_period();

    mtm_analysis (analysis, fit, arch->get_source());
  }
  catch (Error& error) {
    cerr << "Error processing " << filenames[i] << endl;
    cerr << error << endl;
  }

  if (tabular)
  {
    cout << "\n"
      "\\tableline \n"
      "\\end{tabular} \n"
      "\\end{center} \n"
      "\\end{table*} \n"
	 << endl;
  }

  if (susceptibility.size() > 1 && susceptibility.size() == coordinates.size())
  {
    ofstream out ( "susceptibility_correlation.txt" );

    for (unsigned i=0; i<susceptibility.size(); i++)
    {
      Vector<3,double> A = susceptibility[i];
      double Anorm = sqrt(A*A);
      sky_coord Acoord = coordinates[i];

      for (unsigned j=i+1; j<susceptibility.size(); j++)
      {
	Vector<3,double> B = susceptibility[j];
	double Bnorm = sqrt(B*B);
	sky_coord Bcoord = coordinates[j];

	out << names[i] << " " << names[j] << " "
	    << Anorm << " " << Bnorm << " "
	    << Acoord.angularSeparation(Bcoord).getDegrees() << " "
	    << A*B/(Anorm*Bnorm) << endl;
      }
    }
  }

  return 0;

}
