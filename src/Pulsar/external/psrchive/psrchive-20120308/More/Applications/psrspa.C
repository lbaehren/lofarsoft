/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski, Tim Dolley and Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Singla Pulse Analysis - reimplementation and extension of Tim Dolley and Aidan Hotan's spa as an application along with new features.
// Parts of the code corresponding to the functionality of spa are taken directly out of the spa.C, hence I included Tim and Aidan in the note above.
// TODO scan_pulses could be extended to fit the pulses with Gaussian / von Mises / Triangle / etc and use this to determine pulse width
// TODO range setting for flux / PA / polarisation degree histograms

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"
#include "Pulsar/ProfileWeightFunction.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Axis.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/PhaseWeight.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include <cpgplot.h>

#include "strutil.h"
#include "dirutil.h"
#include "Error.h"
#include "Warning.h"

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_errno.h>

using namespace std;
using namespace Pulsar;

static Warning warn;

class psrspa : public Application
{
public:
  //! Default constructor:
  psrspa ();
  //! Process the given archive:
  void process ( Archive* );

  //! Final processing:
  void finalize ();

  //! Initial setup;
  void setup ();

  typedef struct pulse
  {
    string file;           // filename
    unsigned intg;           // integration number
    double flx;            // pulse flux
    double phs;            // phase
    double width;            // width of the pulse
    unsigned bscrunch_factor;		//widht of one bin
  } pulse;

  typedef struct identifier
  {
    string file;
    unsigned integration;
    unsigned phase_min;
    unsigned phase_max;
    unsigned bscrunch_factor;
    unsigned max_amp_bin;
    float max_amp_in_range;
    double b_sigma; 
  } identifier;

protected:
  //! add program options:
  void add_options ( CommandLine::Menu& );

  unsigned nbin;
  bool  gaussian;
  float factor;
  bool  log;
  bool  shownoise;
  int   method;
  unsigned bins;
  float cphs;
  float dcyc;
  float norm;

  vector<pulse>  pulses;

  //! phase resolved histograms
  unsigned hist_count;
  vector<float> phase_range;
  vector<gsl_histogram*> h_polar_degree_vec;
  vector<gsl_histogram*> h_polar_angle_vec;
  vector<gsl_histogram*> h_flux_pr_vec; // pr = phase resolved as opposed to the one calculated in the "traditional" spa

  unsigned polar_degree_bins;
  unsigned polar_angle_bins;
  unsigned flux_pr_bins;

  bool dynamic_histogram;

  //! maximal amplitude (and the corresponding bin) in given range:
  vector < identifier > max_amp_info;

  //! Vector of bins corresponding to given histogram
  vector<unsigned> h_bin;

  bool create_flux;
  bool create_polar_degree;
  bool create_polar_angle;

  bool find_max_amp_in_range;
  bool perform_bscrunch_loop;
  unsigned max_bscrunch;
  unsigned current_bscrunch;

  bool not_initialised; 

  void choose_phase ( float );
  void choose_range ( string );
  void choose_bscrunch ( unsigned );

  //! ProfileStats
  ProfileStats stats;
  //! baseline sigma
  double b_sigma;

  //! initialise phase resolved histograms
  void initialise_histograms () ;
  //! Create phase resolved histograms
  void create_histograms ( Reference::To<Archive> );
  //! Dynamic range of histograms:
  gsl_histogram* update_histogram_range ( gsl_histogram*, float );

  //! convenience function to perform all of the "traditional" functions of spa
  void traditional_spa ();

  //! output control
  string prefix;
  string ext;
  string path;
  bool binary_output;

  //! Handle output
  void write_histograms ();
  void write_pulses ();

  //! The algorithm used to find pulses
  Reference::To<Pulsar::ProfileWeightFunction> finder;
  //! Set and configure the algorithm used to find pulses
  void set_finder (const std::string& name);
  //! Use the finder to list pulse information
  void matched_finder (const Archive*);
  void matched_report (string name, unsigned isub, const PhaseWeight& weight, const Profile& profile);
};


// constructor
psrspa::psrspa ()
	: Application ( "psrspa", "Single Pulse Analysis" )
{
  add ( new StandardOptions );
  polar_degree_bins = polar_angle_bins = flux_pr_bins = bins = 30;
  nbin = 0;

  create_flux = false;
  create_polar_degree = false;
  create_polar_angle = false;

  hist_count = 0;

  dynamic_histogram = false;

  log = false;

  find_max_amp_in_range = false;
  perform_bscrunch_loop = false;

  max_bscrunch = 1;
  current_bscrunch = 1;
  
  not_initialised = true;

  prefix = "psrspa";
  ext = "dat";
  path = "./";
  binary_output = false;

  b_sigma = -1.0;
}

void psrspa::setup ()
{
  if ( bins != 30 )
    polar_degree_bins = polar_angle_bins = flux_pr_bins = bins ;
  if ( ! finder &&  ! create_flux && ! create_polar_degree && ! create_polar_angle && ! find_max_amp_in_range )
    throw Error ( InvalidState, "psrspa::setup", "at least one of -a, -hf, -hd, -ha or -fm needs to be used" );
}

void psrspa::process ( Archive* arch )
{
  if ( nbin == 0 )
    nbin = arch->get_nbin();
  else if ( nbin != arch->get_nbin() )
    throw Error ( InvalidParam, "psrspa::process", "number of bins mismatach" );
  if ( ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range ) && ( not_initialised ) )
    initialise_histograms ();
  arch->fscrunch();        // remove frequency and polarisation resolution
  if ( !create_polar_degree && !create_polar_angle )
  {
    if ( verbose )
      cerr << "psrspa::process polarisation histograms not requested, removing polarisation resolution" << endl;
    arch->pscrunch();
  }
  arch->remove_baseline(); // Remove the baseline level

  //this has to precede create_histograms as the latter can modify the archives (bscrunch them). This can be easily fixed by cloning the archive, if that would prove necessary
  if (finder)
  {
    matched_finder (arch);
    return;
  }

  if ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range )
  {
    create_histograms ( arch );
  }
}

void psrspa::matched_finder ( const Archive* arch )
{
  string name = arch->get_filename ();
  for ( unsigned isub = 0; isub < arch->get_nsubint(); isub++ )
  {
    Reference::To<Profile> profile = arch->get_Profile(isub,0,0)->clone();

    while (profile->get_nbin() > 256)
    {
      finder->set_Profile( profile );  // might finder optimize on &profile?

      PhaseWeight weight;
      finder->get_weight( &weight );

      matched_report (name, isub, weight, *profile);

      profile->bscrunch (2);
    }
  }
}

void psrspa::matched_report (string name, unsigned isub, const PhaseWeight& weight, const Profile& profile)
{
  pulse newentry;
  const unsigned _nbin = weight.get_nbin();
  assert (_nbin == profile.get_nbin());

  unsigned ibin = 0;

  while (ibin < _nbin)
  {
    // find the first on-pulse phase bin
    while (ibin < _nbin && !weight[ibin]) ibin++;

    if (ibin == _nbin)
      break;

    // start of an on-pulse region ... sum up the flux in this region
    unsigned istart = ibin;
    double flux = 0;
    while (weight[ibin] && ibin < _nbin)
    {
      flux += profile.get_amps()[ibin];
      ibin++;
    }

    unsigned iend = ibin-1;

    // mid-point of region defines phase
    double phase = 0.5*(istart + iend);

    // end-points of region define width
    double width = iend - istart;

    newentry.file = name; 
    newentry.intg = isub;
    newentry.flx = flux;
    newentry.phs = phase;
    newentry.width = width;
    newentry.bscrunch_factor = nbin / _nbin ;
    pulses.push_back ( newentry );
  }
}

void psrspa::finalize ()
{
  if ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range )
    write_histograms ();
  if ( finder )
    write_pulses ();
}

void psrspa::set_finder (const std::string& name)
{
  finder = ProfileWeightFunction::factory (name);
}

void psrspa::choose_phase ( float _cphs )
{
  cphs = _cphs;
  if ( cphs > 0.0 && cphs < 1.0 )
  {
    method = 2;
  }
  else
  {
    method = 1;
  }
}

void psrspa::choose_range ( string _range )
{
  if ( verbose )
    cerr << "psrspa::chose_range parsing " << _range << " as a list of phase ranges" << endl;
  for ( string sub = stringtok ( _range, "," ); !sub.empty(); sub = stringtok ( _range, "," ) )
  {
    phase_range.push_back(fromstring<float>(sub));
  }
  if ( phase_range.size()%2 != 0 )
    throw Error ( InvalidParam, "psrspa::chose_range", "Minimum and maximum needs to be provided" );
}


int main (int argc, char** argv)
{
    psrspa program;
      return program.main (argc, argv);
}

void psrspa::add_options ( CommandLine::Menu& menu )
{
  CommandLine::Argument* arg;

  menu.add ( "" );
  menu.add ( "Common options" );

  arg = menu.add ( log, 'l' );
  arg->set_help ( "Use a logarithmic flux scale" );

  arg = menu.add ( bins, 'b', "bins" );
  arg->set_help ( "Set the number of all bins" );

  arg = menu.add ( dynamic_histogram, 'd' );
  arg->set_help ( "Allow dynamic setting of range of histograms" );

  menu.add ( "" );
  menu.add ( "Scan pulses and find giants - options:" );

  arg = menu.add ( this, &psrspa::set_finder, 'a',"algorithm");
  arg->set_help ( "Set the algorithm used to find pulses" );    
  arg->set_long_help ( "\t\tThis is a different implementation of the pulse searching\n"
		 "\t\tAvailable algorithms (name):\n"
		"\t\t - OnPulseThreshold (above)\n" 
		"\t\t - PeakConsecutive (consecutive)\n"
		"\t\t - PeakCumulative (cumulative)\n" 
		"\t\t - ProfileWeightStatic (set)\n" 
		"\t\t - ExponentialBaseline (exponential)\n"
		"\t\t - GaussianBaseline (normal)\n"
		"\t\t - BaselineWindow (minimum)\n" );

  menu.add ( "" );
  menu.add ( "Phased resolved histograms" );

  arg = menu.add ( this, &psrspa::choose_range, "pr","min,max(,min,max...)" );
  arg->set_help ( "Specify phase range (in turns) for the phase resolved algorithms and max amplitude algorithm" );
  arg->set_long_help ( "Multiple ranges can be provided, e.g., '0.0,0.1,0.3,0.35' will create two ranges 0.0-0.1 and 0.3-0.35" );

  arg = menu.add ( create_flux, "hf");
  arg->set_help ( "Create phase resolved histogram of flux" );

  arg = menu.add ( create_polar_degree, "hd");
  arg->set_help ( "Create phase resolved histogram of polarisation degree" );

  arg = menu.add ( create_polar_angle, "ha");
  arg->set_help ( "Create phase resolved histogram of polarisation angle" );

  arg = menu.add ( find_max_amp_in_range, "fm");
  arg->set_help ( "Find maximum amplitude in the given phase range as provided by -pr" );

  arg = menu.add ( flux_pr_bins, "hfb", "bins" );
  arg->set_help ( "Set the number of phase resolved flux bins" );

  arg = menu.add ( polar_degree_bins, "hdb", "bins" );
  arg->set_help ( "Set the number of polarisation degree bins" );

  arg = menu.add ( polar_angle_bins, "hab", "bins" );
  arg->set_help ( "Set the number of polarisation angle bins" );

  arg = menu.add ( this, &psrspa::choose_bscrunch, "bl", "max_bscrunch" );
  arg->set_help ( "When searching for maximum amplitude in the given phase range, perform calculations\n"
		  "also for bscrunched profiles, with the given maximum bscrunch factor" );

  menu.add ( "" );
  menu.add ( "Output of the histograms" );

  arg = menu.add ( prefix, "pfix","prefix" );
  arg->set_help ( "Write files with this prefix" );

  arg = menu.add ( ext, 'e', "extension" );
  arg->set_help ( "Write files with this extension" );
  
  arg = menu.add ( path, 'O', "path" );
  arg->set_help ( "Write files in this location" ); 

  arg = menu.add ( binary_output, "bo" );
  arg->set_help ( "write the phase resolved histograms in binary format" );
}

void psrspa::initialise_histograms ()
{
  //TODO clean this function up, moe the phase_range initialisations somewhere else?
  if ( verbose )
    cerr << "psrspa::initialise_histograms entered" << endl;
  // determine the count of request phase_bins
  unsigned low_end, high_end;
  // if the phase range was not chosen by the user, default to full range
  if ( phase_range.size () == 0 )
  {
    phase_range.push_back ( 0.0 );
    phase_range.push_back ( 1.0 );

    hist_count = nbin;
    // populate the vector of bins for each histogram
    for ( unsigned ibin = 0; ibin <= nbin; ibin++ )
    {
      h_bin.push_back(ibin);
    }
  }
  else
  {
    for ( unsigned irange = 0; irange < phase_range.size (); irange++ )
    {
      if ( irange%2 == 0)
      {
	low_end = unsigned( floor ( phase_range[irange] * float(nbin) + 0.5) );
      }
      else
      {
	high_end = unsigned( floor ( phase_range[irange] * float(nbin) + 0.5 ) );
	if ( low_end > high_end )
	{
	  cerr << low_end << " (" << phase_range[irange-1] << ") " << high_end << " (" << phase_range[irange] << ") "<< endl;
	  throw Error ( InvalidParam, "psrspa::initialise_histograms", "wrong range provided" );
	}

	hist_count += high_end - low_end + 1;

	// populate the vector of bins for each histogram
	for ( unsigned ibin = low_end; ibin <= high_end ; ibin++ )
	{
	  h_bin.push_back(ibin);
	}
      }
    }
  }

  if ( create_polar_degree )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " polar degree histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_polar_degree_vec.push_back ( gsl_histogram_alloc ( polar_degree_bins ) );
      gsl_histogram_set_ranges_uniform ( h_polar_degree_vec[ihist], 0.0, 1.0 );
    }
  }
  if ( create_polar_angle )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " polar angle histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_polar_angle_vec.push_back ( gsl_histogram_alloc ( polar_angle_bins ) );
      gsl_histogram_set_ranges_uniform ( h_polar_angle_vec[ihist], -2.0 * M_PI, 2.0 * M_PI );
    }
  }
  if ( create_flux )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " phase resolved flux histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_flux_pr_vec.push_back ( gsl_histogram_alloc ( flux_pr_bins ) );
      if ( log )
	gsl_histogram_set_ranges_uniform ( h_flux_pr_vec[ihist], 0.0, 5.0 );
      else 
	gsl_histogram_set_ranges_uniform ( h_flux_pr_vec[ihist], 0.0, 100000.0 );
    }
  }
  not_initialised = false;
  if ( verbose )
    cerr << "psrspa::initialise_histograms finished" << endl;
} // initialise_histograms

void psrspa::create_histograms ( Reference::To<Archive> archive )
{
  if ( verbose )
    cerr << "psrspa::create_histograms entered" << endl;
  // ensure Stokes parameters if creating polarisation histograms
  if ( create_polar_degree || create_polar_angle )
  {
    archive->convert_state ( Signal::Stokes );
    if ( verbose )
      cerr << "psrspa::create_histograms converted state of the archive to Stokes" << endl;
  }

  // auxillary vectors
  //vector< Estimate<float > > aux_vec_f;
  vector< Estimate<double > > aux_vec_d;

  // Full Stokes profile
  Reference::To<PolnProfile> profile;
  // Polarized flux
  Reference::To<Profile> P;
  P = new Profile;
  // Total flux
  float *T_amps = new float [ nbin ];
  float *P_amps = new float [ nbin ];

  unsigned bin_min, bin_max;

  if ( verbose && max_bscrunch > 1 )
    cerr << "psrspa::create_histograms entering the bscrunch loop for the " << archive->get_filename () << " archive" << endl;
  // TODO Uh, this should be fixed up. The first idea was to enable the phase resolved histograms to be bscrunch-aware as well, but I think it doesn't make much sense so I decided to keep only the max flux bscrunch aware. This can be done much more neatly probably in such a case
  for ( current_bscrunch = 1 ; current_bscrunch <= max_bscrunch ; current_bscrunch *= 2 )
  {
    // in each passage, we bscrunch by a factor of 2
    if ( current_bscrunch > 1 )
    {
      if ( verbose )
	cerr << "psrspa::create_histograms bscrunching the archive " << archive->get_filename () << " by a factor of 2" << endl;
      archive->bscrunch ( 2 );
    }
    if ( verbose )
      cerr << "psrspa::create_histograms entering the loop through subints of the " << archive->get_filename () << " archive" << endl;
    // loop through subints
    for ( unsigned isub = 0; isub < archive->get_nsubint (); isub++ )
    {
      if ( verbose )
	cerr << "psrspa::create_histograms creating necessary profiles for subint " << isub << " of " << archive->get_filename () << endl;
      if ( create_polar_angle || create_polar_degree && current_bscrunch == 1 )
      {
	profile = archive->get_Integration(isub)->new_PolnProfile(0);
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved PolnProfile for subint " << isub << " of " << archive->get_filename () << endl;
      }
      if ( create_polar_angle && current_bscrunch == 1 )
      {
	profile->get_orientation ( aux_vec_d, 0 );
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved polarisation angle for subint " << isub << " of " << archive->get_filename () << endl;
      }
      if ( create_polar_degree || create_flux || find_max_amp_in_range )
      {
	stats.set_profile ( archive->get_Profile ( isub, 0, 0 ) );
	b_sigma = sqrt ( stats.get_baseline_variance ().get_value () );
	T_amps = archive->get_Profile ( isub, 0, 0 )->get_amps (); 
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved total flux amps for subint " << isub << " of " << archive->get_filename () << endl;
	if ( create_polar_degree && current_bscrunch == 1 )
	{
	  profile->get_polarized ( P );
	  if ( verbose )
	    cerr << "psrspa::create_histograms retrieved polarized flux profile for subint "  << isub << " of " << archive->get_filename () << endl;
	  P_amps = P->get_amps ();
	  if ( verbose )
	    cerr << "psrspa::create_histograms retrieved polarized flux amps for subint " << isub << " of " << archive->get_filename () << endl;
	}
      }

      if ( verbose )
	cerr << "psrspa::create_histograms looping through the provided phase ranges for subint " << isub << " of " << archive->get_filename () << endl;
      unsigned curr_hist = 0;
      // loop through phase ranges
      for ( unsigned irange = 0; irange < phase_range.size () ; irange++ )
      {
	if ( irange%2 == 0)
	{
	  bin_min = unsigned( floor ( phase_range[irange] * float(nbin / current_bscrunch ) + 0.5 ) );
	  if ( verbose ) 
	    cerr << "psrspa::create_histograms set minimal bin to " << bin_min << endl;
	}
	else
	{
	  bin_max = unsigned( floor ( phase_range[irange] * float(nbin / current_bscrunch ) + 0.5 ) );
	  if ( bin_max == nbin )
	    bin_max = nbin - 1 ;
	  if ( verbose ) 
	    cerr << "psrspa::create_histograms set maximum bin to " << bin_max << endl;
	  // loop through bins in the given phase range
	  for ( unsigned ibin = bin_min; ibin <= bin_max; ibin++ )
	  {
	    if ( create_polar_angle && current_bscrunch == 1 )
	    {
	      int result = gsl_histogram_increment ( h_polar_angle_vec[curr_hist], aux_vec_d[ibin].get_value () / 180.0 * M_PI );
	      if ( result == GSL_EDOM )
	      {
		if ( dynamic_histogram )
		{
		  gsl_histogram *temp_hist = gsl_histogram_clone ( h_polar_angle_vec[curr_hist] );
		  h_flux_pr_vec[curr_hist] = update_histogram_range ( temp_hist, aux_vec_d[ibin].get_value () / 180.0 * M_PI );
		}
		else {
		  warn << "WARNING psrspa::create_histograms polarisation angle the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << endl;
		}
	      }
	    }
	    if ( create_polar_degree && current_bscrunch == 1 )
	    {
	      // if P_amps[ibin] or T_amps[ibin] < 3 sigma, set polar degree to zero
	      int result = gsl_histogram_increment ( h_polar_degree_vec[curr_hist], ( fabs ( P_amps[ibin] ) < 3.0 * b_sigma || fabs ( T_amps[ibin] ) < 3.0 * b_sigma ) ? 0.0 : P_amps[ibin] / T_amps[ibin] );
	      if ( result == GSL_EDOM )
	      {
		if ( dynamic_histogram )
		{
		  gsl_histogram *temp_hist = gsl_histogram_clone ( h_polar_degree_vec[curr_hist] );
		  h_flux_pr_vec[curr_hist] = update_histogram_range ( temp_hist, ( fabs ( P_amps[ibin] ) < 3.0 * b_sigma || fabs ( T_amps[ibin] ) < 3.0 * b_sigma ) ? 0.0 : P_amps[ibin] / T_amps[ibin] );
		}
		else {
		warn << "WARNING psrspa::create_histograms polarisation degree outside the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << endl;
		}
	      }
	    }
	    if ( create_flux && current_bscrunch == 1 )
	    {
	      int result = 0;
	      if ( log && T_amps[ibin] > 0.0 )
	      {
		result = gsl_histogram_increment ( h_flux_pr_vec[curr_hist], log10f ( T_amps[ibin] ) );
	      }
	      else if ( !log )
	      {
		result = gsl_histogram_increment ( h_flux_pr_vec[curr_hist], T_amps[ibin] );
	      }
	      if ( result == GSL_EDOM )
	      {
		if ( dynamic_histogram )
		{
		  gsl_histogram *temp_hist = gsl_histogram_clone ( h_flux_pr_vec[curr_hist] );
		  h_flux_pr_vec[curr_hist] = update_histogram_range ( temp_hist, log ? log10f ( T_amps[ibin] ) : T_amps[ibin] );
		}
		else {
		  warn << "WARNING psrspa::create_histograms phase resolved flux outside the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << " flux = " << T_amps[ibin] << endl;
		}
	      }
	    }
	    // increment the histogram id
	    curr_hist ++;
	  } // loop through bins in the given phase range
	  // find the maximum amplitude in given phase range.
	  if ( find_max_amp_in_range )
	  {
	    if ( verbose )
	      cerr << "psrspa::create_histograms finding the maximum amplitude in the phase range " << irange << " of the subint " << isub << " (archive " << archive->get_filename () << ")" << endl;
	    int max_bin = archive->get_Profile ( isub, 0, 0 )->find_max_bin ( (int)bin_min, (int)bin_max );
	    identifier current_identifier = { archive->get_filename (), isub, bin_min, bin_max, current_bscrunch,
	      (unsigned)max_bin, T_amps[max_bin], b_sigma }; 
	    max_amp_info.push_back ( current_identifier );
	  } // find maximal amplitude in the given phase range
	} // handle given phase range
      } // loop through phase ranges
    } // loop through subints
  } // bscrunch loop
  if ( verbose )
    cerr << "psrspa::create_histograms finished" << endl;
}// create_histograms

void psrspa::write_histograms ()
{
  FILE *out;
  stringstream ss;

  if ( verbose )
    cerr << "psrspa::write_histograms entered" << endl;
  if ( create_flux )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing flux histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_flux_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_flux_pr_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_flux_pr_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( create_polar_degree )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing polarisation degree histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_poldeg_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_polar_degree_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_polar_degree_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( create_polar_angle )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing polarisation angle histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_polang_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_polar_angle_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_polar_angle_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( find_max_amp_in_range )
  {
    ss << path+"/"+prefix+"_maxamp" << "." << ext;
    ofstream out_stream;
    if ( binary_output )
    {
      out_stream.open ( ss.str().c_str(), ios::out | ios::trunc | ios::binary );
    }
    else
      out_stream.open ( ss.str().c_str(), ios::out | ios::trunc );
    unsigned b_scrunch = 1;
    for ( unsigned ientry = 0; ientry < max_amp_info.size (); ientry++ )
    {
      out_stream << max_amp_info[ientry].file << " " << max_amp_info[ientry].integration << " " << max_amp_info[ientry].bscrunch_factor;
      for ( unsigned irange = 0; irange < phase_range.size () / 2; irange++ )
      {
	out_stream << " " << max_amp_info[ientry].phase_min << " " << max_amp_info[ientry].phase_max << " " << max_amp_info[ientry].max_amp_bin << " " << max_amp_info[ientry].max_amp_in_range << " " << max_amp_info[ientry].b_sigma ;
      }
      out_stream << endl;
    }
    //TODO for some reason this close here can cause a seg fault / bus error on Max OS X running on an Intel core.
    out_stream.close ();
  }
} // write_histograms

void psrspa::write_pulses ()
{
  stringstream ss;

  ss << path+"/"+prefix+"_pulses."+ext;
  ofstream out_stream;

  out_stream.open ( ss.str().c_str(), ios::out | ios::trunc );
  for ( unsigned ipulse = 0; ipulse < pulses.size() ; ipulse ++ )
    out_stream << pulses[ipulse].file << " " << pulses[ipulse].intg << " " << pulses[ipulse].flx << " " << pulses[ipulse].phs << " " << pulses[ipulse].width << " " << pulses[ipulse].bscrunch_factor << endl;
  out_stream.close ();

}

void psrspa::choose_bscrunch ( unsigned _max_bscrunch )
{
  max_bscrunch = _max_bscrunch;
  while ( _max_bscrunch > 1 )
  {
    if ( _max_bscrunch % 2 != 0 )
      throw Error ( InvalidParam, "psrspa::choose_bscrunch", "the max_bscrunch factor should be a power of two" ) ;
    _max_bscrunch /= 2;
  }
  perform_bscrunch_loop = true;
}

gsl_histogram* psrspa::update_histogram_range( gsl_histogram* hist_in, float value )
{
  size_t bins_in = hist_in->n;
  size_t offset;
  double min_in = hist_in->range[0];
  double max_in = hist_in->range[bins_in];
  double min_out, max_out;
  min_out = min_in;
  max_out = max_in;

  double step_in = hist_in->range[1] - hist_in->range[0];

  size_t add_bins ;
  if ( value > max_in )
  {
    //need to extended on the max side
    add_bins = (size_t) ceil( ( value - max_in ) / (double)step_in );
    max_out += ( (double) ( add_bins + 1 ) ) * step_in;
    offset = 0;
  }
  else
  {
    //need to extended on the min side
    add_bins = (size_t) ceil( ( min_in - value ) / (double)step_in );
    min_out -= ( (double) ( add_bins + 1 ) ) * step_in;
    offset = add_bins + 1;
  }

  gsl_histogram *hist_out = gsl_histogram_alloc ( bins_in + add_bins + 1 ); // +1 to be on the safe side
  gsl_histogram_set_ranges_uniform ( hist_out, min_out, max_out );

  memcpy ( hist_out->bin + offset, hist_in->bin, bins_in * sizeof(double) );

  gsl_histogram_increment ( hist_out, value );

  return hist_out;
}
