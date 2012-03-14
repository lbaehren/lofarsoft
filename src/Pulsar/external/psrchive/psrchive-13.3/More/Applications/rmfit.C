/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/DeltaRM.h"
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/FaradayRotation.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

#include "dirutil.h"
#include "Error.h"
#include "strutil.h"
#include "templates.h"


#if HAVE_PGPLOT
#include "Pulsar/StokesCylindrical.h"
#include <cpgplot.h>
#endif

#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <cmath>

using namespace std;
using TextInterface::parse_indeces;

#include "njkk08/njkk08.h"
#include "njkk08/linchi2.h"
#include "njkk08/KaraFit.h"

bool verbose = false;

void cpg_next ();

// make an estimate of the rotation measure using matrix template matching
void mtm_estimate (Pulsar::Archive* std, Pulsar::Archive* obs);

void
do_singlebin(Reference::To<Pulsar::Archive> data,
	  float x1,float x2,bool display,
	  const vector<int> goodchans,
	  vector<double> freqs,
	  vector<double>& pa, vector<double>& pa_stddev,
	  float nsigma);


void
do_window(Reference::To<Pulsar::Archive> data,
	  float x1,float x2,bool display,
	  const vector<int> goodchans,
	  vector<double> freqs,
	  vector<double>& pa, vector<double>& pa_stddev,
	  float nsigma);


void
pa_plot( int rise_bin, int fall_bin,
	 Reference::To<Pulsar::Archive> data,
	 bool display,
	 vector<double>& pa, vector<double>& pa_stddev,
	 vector<double>& freqs, const vector<int>& goodchans,
	 float nsigma);

void
do_display(vector<double> freqs,
	   vector<double> pa,
	   vector<double> pa_stddev,
	   string filename);

void 
fitstuff(vector<double> freqs,vector<double> pa,vector<double> pa_stddev,
	 bool display,Reference::To<Pulsar::Archive> data,vector<int> good_chans);


void do_refine(Reference::To<Pulsar::Archive> data,bool log_results);

Reference::To<Pulsar::Archive> get_data (string archive_filename);

double do_maxmthd(double minrm,double maxrm,unsigned rmsteps,
		  Reference::To<Pulsar::Archive> data);

// prints various numbers out to file
void
rmresult (Pulsar::Archive* archive, const Estimate<double>& rm, unsigned used);

static bool display = false;

void usage ()
{
  cout <<
    "A program for finding rotation measures\n"
    "Usage: rmfit [options] filenames\n"
    "\n"
    "Fourier method search options: \n"
    "  -m min,max,steps  Specify the bounds and number of steps \n"
    "  -t                Use the default bounds and resolution \n"
    "  -i nchannels      Subdivide the band (fscrunch-x2) until \"nchannels\" channels are left (incorporates systematics) \n"
    "  -j ntimes         Subdivide the pulse profile (bscrunch-x2) ntimes (incorporates systematics) \n"
    "  -u max            Set the upper bound on the default maximum RM \n"
    "  -U rm/dm          Set upper bound on default maximum RM = DM * rm/dm \n"
    "\n"
    "Fit options: \n"
    "  -r                Refine the RM using two halves of band \n"
    "\n"
    "Preprocessing options: \n"
    "  -b [+/-][range]   Include/exclude phase bins \n"
    "  -B [factor]       Scrunch in phase bins \n"
    "  -F [factor]       Scrunch in frequency \n"
    "  -R RM             Set the input rotation measure \n"
    "  -z \"z1 z2 z3...\"  Zap these channels\n"
    "\n"
    "Plotting options: \n"
    "  -D                Display results \n"
    "  -K dev            Specify pgplot device\n"
    "  -p [x1,x2]        Fit for every phase bin in window \n"
    "  -w [x1,x2]        Average over phase window \n"
    "  -Y                Produce a postscript plot of V against frequency\n"
    "\n"
    "Standard options:\n"
    "  -h                This help page \n"
    "  -v                Verbose mode \n"
    "  -V                Very verbose mode \n"
    "  -L                Log results in source.log \n"
       << endl;
}

static bool auto_maxmthd = false;
static float auto_maxrm_dm = 0.0;
static float auto_maxrm = 1500.0;
static float auto_minrm = 0.0;
static float auto_step_rad = 0.0;

static unsigned auto_minsteps = 10;
static float refine_threshold = 0;
static unsigned max_iterations = 10;

Estimate<double> best_search_rm;

static vector<unsigned> include_bins;
static vector<unsigned> exclude_bins;

int main (int argc, char** argv)
{
  bool singlebin = false;
  bool window = false;

  bool refine  = false;
  bool maxmthd = false;

  minrm = -1000.0;
  maxrm = 1000.0;
  rmstep = 1.0;
  rmsteps = 50;

  bool iter_fscr = false;
  bool iter_bscr = false;
  unsigned nbscr = 0;
  int fscr_init = 1;
  unsigned nfscr = 1;

  bool fscrunchme = false;
  int fscr = 1;
  int nchannels = 32;

  float fbscr_RMs_weighted_mean = 0;
  float fbscr_RMs_var = 0;

  bool bscrunchme = false;
  int bscr = 1;

  good_fbscrunch = false;


  float x1 = 0.0;
  float x2 = 0.0;

  float threshold = 0.01;

  bool log_results = false;

  bool rm_set = false;
  double rotation_measure = 0.0;

  string cpg_device = "?";
  float nsigma = 1.0;
  vector<unsigned> zap_chans;

  vector<string> archives;

  string exclude_range;
  string include_range;

  // estimate the RM using MTM
  Reference::To<Pulsar::Archive> mtm_std;

  const char* args = "b:B:DeF:i:j:hJK:Lm:M:p:PrR:S:T:tu:U:vVw:W:Yz:";

  int gotc = 0;

  while ((gotc = getopt(argc, argv, args)) != -1) {
    switch (gotc) {

    case 'b':
      switch (optarg[0])
	{
	case '-':
	  exclude_range = optarg + 1;
	  cerr << "rmfit: will exclude " << exclude_range << endl;
	  break;
	case '+':
	  include_range = optarg + 1;
	  cerr << "rmfit: will include " << include_range << endl;
	  break;
	default:
	  cerr << "rmfit: invalid bin inclusion/exclusion string "
	    "'" << optarg << "'" << endl;
	  return 0;
	}
      break;

    case 'B':
      bscrunchme = true;
      if (sscanf(optarg, "%d", &bscr) != 1) {
	cerr << "That is not a valid scrunch factor!" << endl;
	return -1;
      }
      break;

    case 'p':
      singlebin = true;
      if (sscanf(optarg, "%f,%f", &x1, &x2) != 2) {
	cerr << "That is not a valid phase window!" << endl;
	return -1;
      }

      break;


    case 'D':
      display = true;
      break;

    case 'F':
      fscrunchme = true;
      if (sscanf(optarg, "%d", &fscr) != 1) {
	cerr << "That is not a valid scrunch factor!" << endl;
	return -1;
      }
      break;

    case 'h':
      usage ();
      return -1;
      break;

    case 'i':
      iter_fscr = true;
      fscrunchme = true;
      fscr = 2;
      fscr_init = 16;
      if (sscanf(optarg, "%d", &nchannels) != 1) {
	cerr << "That is not a valid number of channels!" << endl;
	return -1;
      }
      if (nchannels < 8) {
	cerr << "Very few channels; it will compromise the statistics!" << endl;
	return -1;
      }

      break;

    case 'j':
      iter_bscr = true;
      bscrunchme = true;
      bscr=2;
      if (sscanf(optarg, "%u", &nbscr) != 1) {
	cerr << "That is not a valid number of scrunches!" << endl;
	return -1;
      }
      if (nbscr > 8) {
	cerr << "Too many bscrunch times; it will compromise the statistics!" << endl;
	return -1;
      }

      break;

    case 'K':
      cpg_device = optarg;
      break;

    case 'L':
      log_results = true;
      break;

    case 'm':
      maxmthd = true;
      if (sscanf(optarg, "%f,%f,%ud", &minrm, &maxrm, &rmsteps) != 3) {
	cerr << "Invalid paramaters!" << endl;
	return -1;
      }
      if ((maxrm < minrm) || (rmsteps < 2)) {
	cerr << "Invalid paramaters!" << endl;
	return -1;
      }
      
      rmstep = fabs(maxrm-minrm)/float(rmsteps);

      
      break;

    case 'M':
      mtm_std = get_data(optarg);
      break;

    case 'r':
      refine = true;
      break;

    case 'R':
      rotation_measure = atof (optarg);
      rm_set = true;
      break;

    case 'S': nsigma = atof(optarg); break;

    case 't':
      maxmthd = true;
      auto_maxmthd = true;
      break;

    case 'T':
      if (sscanf(optarg, "%f", &threshold) != 1) {
	cerr << "That is not a valid cut-off!" << endl;
	return -1;
      }
      break;

    case 'J':

      refine = true;          // <delta PA> two halves of band
      auto_maxrm = 2000.0;
      auto_minrm = 100.0;
      auto_maxrm_dm = 6.0;    // max RM = 6.0 * DM
      auto_step_rad = 1.0;    // compute step such that one radian

      log_results = true;

      cpg_device = "rmsearch.ps/cps";
      display = true;

      break;

    case 'u':
      auto_maxrm = atoi (optarg);
      break;

    case 'U':
      auto_maxrm_dm = atof (optarg);
      break;

    case 'v':
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'w':
      window = true;
      if (sscanf(optarg, "%f,%f", &x1, &x2) != 2) {
	cerr << "That is not a valid phase window!" << endl;
	return -1;
      }
      break;

    case 'Y':
      plotv = true;
      break;

    case 'z': 
      {
	vector<string> words = stringdecimate(optarg," \t");
	for( unsigned i=0; i<words.size(); i++)
	  zap_chans.push_back( fromstring<unsigned>(words[i]) );
	break;
      }


    default:
      cout << "Unrecognised option" << endl;
    }
  }

  // Parse the list of archives

  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);

  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  }

#if HAVE_PGPLOT
  if (display)
    {
      unsigned nx = 1;
      unsigned ny = 1;
      cpgopen(cpg_device.c_str());
      cpgsubp(nx,ny);
    }
#endif

  lthresh = -3.5;// threshold for good phase bins (if negative, allow all bins)

  Reference::To<Pulsar::Archive> data;

  for (unsigned i = 0; i < archives.size(); i++) try
  {
    data = get_data(archives[i]);

    if (rm_set)
    { 
      data->set_rotation_measure (rotation_measure);
      data -> defaraday();
    }

    if (mtm_std)
    {
      mtm_estimate (mtm_std, data);
      continue;
    }

    if (iter_fscr)
    {
      cerr <<endl<< "Entering RM-fitting with channel systematics checks:" <<endl;
	    
      int the_channo = data -> get_nchan();
      float the_freq = data->get_centre_frequency();
      float the_date = atof(data->start_time().printdays(0).c_str());
	    
      cerr << "MJD = " << the_date <<endl;
	    
      if (the_freq<2000.0 && the_freq>1000.0)
      {
	cerr << "*** 20cm data ***" <<endl; 
	if (the_date > 53900.0)
	  the_channo = 800;
	else
	  the_channo = 768;//OLD DATA
      }
      else if (the_freq<4000.0 && the_freq>2000.0)
      {
	cerr << "*** 10cm data ***" <<endl; 
	if (the_date > 53900.0)
	  the_channo = 829; 
	else
	  the_channo = 888;//OLD DATA
      }
      else if (the_freq<1000.0 && the_freq>500.0)
      {
	cerr << "*** 50cm data ***" <<endl; 
	the_channo = 308;
      }

      nfscr = 1+int(ceil( (  log10(float(the_channo)/float(fscr_init)) - log10(float(nchannels))  ) / log10(2.) )); //10cm
      cerr << "       The data channels will be halved " << nfscr-1 <<" times."<<endl<<endl;
    }


    best_fbscr_probmax = -1e10;

    best_fbscr_RM = -100000.;
    best_fbscr_RM_err = 0.;

    outofrange_err=false;
    zero_err = false;
    anomalous_err = false;


    for (unsigned ifscr=0; ifscr<nfscr; ifscr++)
    {
      if(fscrunchme){
	if(iter_fscr){
	  if(ifscr>0){
	    
	    cerr <<endl<<endl<< "***************************************" <<endl;
	    cerr << "Scrunching frequency channels by factor" << fscr <<endl;
	    cerr << "***************************************" <<endl<<endl;
	    
	    data -> fscrunch(fscr);
	    
	  }
	  else
	    data -> fscrunch(fscr_init);
	}
	else
	  data -> fscrunch(fscr);
      }

      cerr << endl << endl
	   << "Number of frequency channels = " << data -> get_nchan() 
	   << endl << endl;

      for(unsigned ibscr=0; ibscr<nbscr+1; ibscr++)
      {
	if (bscrunchme){   
	  if(iter_bscr){
	    if(ibscr>0){
	      	
	      cerr <<endl<<endl<< "***************************************" <<endl;
	      cerr << "Scrunching phase bins by factor" << bscr <<endl;
	      cerr << "***************************************" <<endl<<endl;
	      
	      data -> bscrunch(bscr);      
	      
	    }
	  }
	  else{
	    data -> bscrunch(bscr);
	    cerr <<endl<<endl<<endl<<"Scrunched by factor "<< bscr <<endl<<endl;
	  }
	}
	
	
	for( unsigned izap=0; izap<zap_chans.size(); izap++)
	  for( unsigned iint=0; iint<data->get_nsubint(); iint++)
	    data->get_Integration(iint)->set_weight(zap_chans[izap],0.0);
	
	if (maxmthd && !(singlebin || window)) {
	  
	  double best_rm = do_maxmthd (minrm, maxrm, rmsteps, data);
	  

	  data->set_rotation_measure (best_rm);

	  if( verbose )
	    fprintf(stderr,"Completed do_maxmthd and got out best_rm=%f\n",
		    best_rm);
	  
	  if( !refine )
	    continue;
	}

	  if( verbose )
	    fprintf(stderr,"Continuing with specialist methods\n");

	  // This must be done after maxmthd because it re-labels
	  // all the channel frequencies.

	  if (refine) try {

	    if (!include_range.empty())
	      parse_indeces (include_bins, include_range, data->get_nbin());

	    if (!exclude_range.empty())
	      parse_indeces (exclude_bins, exclude_range, data->get_nbin());

	    do_refine (data,log_results);
	    continue;

	  }
	  catch (Error& error) { 
	    cerr << "rmfit: Error during refine" << error << endl; 
	    return -1;
	  } 

	  if( verbose )
	    fprintf(stderr,"Going to generate good_chans\n");

	  vector<int> goodchans;
          ofstream test_goodchans;

	  for (unsigned i = 0; i < data->get_nchan(); i++) {
	    if (data->get_Integration(0)->get_weight(i) > threshold){ 
	      goodchans.push_back(i);

	      }
	   }


	  if (goodchans.size() < 2) {
	    cerr << "Not enough channels above threshold!" << endl;
	    return -1;
	  }

	  int good_nchan = goodchans.size();

	  vector<double> pa(good_nchan);
	  vector<double> pa_stddev(good_nchan);  
	  vector<double> freqs(good_nchan);

	  for (int i = 0; i < good_nchan; i++)
	    freqs[i] = data->get_Profile(0,0,goodchans[i])->get_centre_frequency();


	  if (singlebin)	      do_singlebin    (data,x1,x2,display,goodchans,freqs,
						     pa, pa_stddev, nsigma);
	  else if (window)	      do_window       (data,x1,x2,display, goodchans, freqs,
						     pa, pa_stddev, nsigma);	  

	  if( verbose )
	    fprintf(stderr,"Going to go in to fitstuff\n");



          if(good_fbscrunch) fitstuff(goodfreqs,goodpa,goodpa_stddev,display,data,goodchans);

        }//bscrunching iteration loop (end)
      }// fscrunching iteration loop (end)

      float fbscr_RM_weight = 0.;
      float fbscr_RM_weight_sum = 0.;
      fbscr_RMs_weighted_mean = 0.;
      fbscr_RMs_var = 0.;
      
      for(unsigned i=0; i<fbscr_RMs.size(); i++)
      {	 
	 fbscr_RM_weight =
	        1./fabs(fbscr_RM_probmax[i])*
	        1./fabs(fbscr_RM_probmax[i]);

		
	 fbscr_RM_weight_sum += fbscr_RM_weight;
	 
	 fbscr_RMs_weighted_mean += fbscr_RM_weight*fbscr_RMs[i];
      }

      fbscr_RMs_weighted_mean /= fbscr_RM_weight_sum;
      
      for (unsigned i=0; i<fbscr_RMs.size(); i++)
	 fbscr_RMs_var += (fbscr_RMs[i]-fbscr_RMs_weighted_mean)*(fbscr_RMs[i]-fbscr_RMs_weighted_mean);

      fbscr_RMs_var /= float(fbscr_RMs.size()-1);
      fbscr_RMs_var = sqrt(fbscr_RMs_var);



    }
  catch (Error& error)
    { 
      cerr << error << endl;
      continue;
    }

  if (mtm_std)
    return 0;

  ofstream test_bestRMs;
  test_bestRMs.open("all_bestRMs.out",ios::app);


  
  for (unsigned k=0; k<fbscr_RMs.size();k++)
  {
      test_bestRMs <<setw(12) << data->get_source()                   << " "
		 << setw(5)   << k                                    << " " 
		 << setw(8)   << k + int(x1 * data->get_nbin())       << " " 
		 << setw(8)   << fbscr_RMs[k]                         << " " 
                 << setw(12)   << fbscr_RM_errs[k]                     << " "
		 << setw(12)  << fbscr_RM_probmax[k]                  << " "  
                 
		 << setw(12)  << pow(1./fabs(fbscr_RM_probmax[k]),2.) << " "   
		 
		 << fmod (double(k + int(x1 * data->get_nbin()))/double(data->get_nbin()-1), 1.0)<<endl;
  }
  test_bestRMs.close();

  ofstream bestRM_result;
  bestRM_result.open("rmfit_results.out",ios::app);

  bestRM_result <<setw(30)      << data->get_filename()          <<"  "
                <<setw(12)      << data->get_source()            <<"  "
                <<setw(8)       << data->get_centre_frequency()  <<"  "
                <<setw(8)       << fabs(data->get_bandwidth())   <<"  "
                <<setw(8)       << data->get_nchan()             <<"  "
                <<setw(8)       << data->get_nbin()              <<"  ";
       
  if(best_fbscr_RM<-10000.)
    bestRM_result << "none found!" << "   " 
         << "n/a" << "   " 
         << "n/a" << "   " 
         << "n/a" << "   " 
         << "n/a";
  else
    bestRM_result <<setw(8)<< fbscr_RMs[0]            << "   "
                  <<setw(8)<< fbscr_RM_errs[0]        << "   "
                  <<setw(8)<< best_fbscr_RM           << "   " 
                  <<setw(8)<< best_fbscr_RM_err       << "   " 
                  <<setw(8)<< fbscr_RMs_weighted_mean << "   " 
                  <<setw(8)<< fbscr_RMs_var ;


  if(outofrange_err) bestRM_result << "   " << "OoR" <<endl;   
  else if(zero_err) bestRM_result << "   " << "Z" <<endl;
  else if(anomalous_err) bestRM_result << "   " << "AER" <<endl;
  else bestRM_result << "   " << "OK" <<endl;	   

  bestRM_result.close();

  ofstream StokeStats;
  StokeStats.open("StokeStats.out");

  for (unsigned k=0; k<fbscr_RMs.size();k++)
  {

      StokeStats << setw(12)  << data->get_source()                   << " "
		 << setw(5)   << k                                    << " " 
		 << setw(8)   << k + int(x1 * data->get_nbin())       << " " 
		 << fmod (double(k + int(x1 * data->get_nbin()))/double(data->get_nbin()-1), 1.0)  << " " 
		 << setw(8)   << delta_V[k]                           << " " 
		 << setw(8)   << delta_Verr[k]                        << " " 
		 << setw(8)   << fbscr_RMs[k]                         << " " 
                 << setw(12)  << fbscr_RM_errs[k]                     << " "                    
 		 << setw(10)   << delta_L[k]                          << " " 
		 << setw(10)   << delta_Lerr[k]                       << " " <<endl;

  }

  StokeStats.close();



#if HAVE_PGPLOT
  if (display)
    cpgend ();
#endif

  return 0;
}

// defined in width.C
float width (const Pulsar::Profile* profile, float& error, float pc, float dc);

void rmresult (Pulsar::Archive* archive,
	       const Estimate<double>& rm, unsigned used)
{
  unsigned original_nchan = archive->get_nchan();

  archive->set_rotation_measure( rm.get_value() );
  archive->defaraday();
  archive->fscrunch();
  archive->remove_baseline();
  archive->centre_max_bin();

#if HAVE_PGPLOT

  string dev = archive->get_source () + ".ps/cps";
  cerr << "\nrmresult: PGPLOT to " << dev << endl << endl;
  cpgbeg (0, dev.c_str(), 0, 0);
  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  Pulsar::StokesCylindrical plot;

  plot.get_frame()->publication_quality ();
  plot.get_frame()->set_character_height (1.0);

  // phase in degrees, with origin at pulse peak
  plot.get_scale()->set_units (Pulsar::PhaseScale::Degrees);
  plot.get_scale()->set_origin_norm (-0.5);

  // linear polarization with dash-dot-dash-dot
  plot.get_flux()->set_plot_lines ("131");

  // plot the time resolution and flux error box
  plot.get_flux()->set_original_nchan (original_nchan);
  plot.get_flux()->set_plot_error_box (2);

  // zoom so that the on-pulse region occupyies a fraction of the phase axis
  plot.get_flux()->auto_scale_phase (archive->get_Profile(0,0,0), 2.5);
  // increase the buffer to 10% on either side
  plot.get_flux()->get_frame()->get_y_scale()->set_buf_norm (0.1);
  // separate the name and frequency
  plot.get_flux()->get_frame()->get_label_below()->set_left ("$name");
  plot.get_flux()->get_frame()->get_label_below()->set_right ("$freq MHz");

  // plot -90 to 180 in the PA plot
  plot.get_orientation()->set_span (180.0);
  plot.get_orientation()->get_frame()->get_y_scale()->set_range_norm (0, 1.5);

  plot.get_orientation()->set_marker( (Pulsar::AnglePlot::Marker)
				      ( Pulsar::AnglePlot::ErrorTick ) );

  plot.plot (archive);

  cpgend ();

#endif

  FILE* fptr = fopen ("rmfit.out", "a");
  if (!fptr) {
    cerr << "Could not open rmfit.out ";
    perror ("");
  }

  fprintf (stderr, "Setting up PolnProfileStats\n");

  Pulsar::PolnProfileStats stats;
  stats.set_profile( archive->get_Integration(0)->new_PolnProfile(0) );
  unsigned nbin = archive->get_nbin();

  fprintf (stderr, "Writing title\n");

#define FORMAT1 "%10s "
#define FORMAT2 "%10.4f "

  fprintf (fptr, FORMAT1, "NAME");
  fprintf (fptr, FORMAT1, "<I>");
  fprintf (fptr, FORMAT1, "<p>");
  fprintf (fptr, FORMAT1, "<L>");
  fprintf (fptr, FORMAT1, "<V>");
  fprintf (fptr, FORMAT1, "<|V|>");
  fprintf (fptr, FORMAT1, "sigma");
  fprintf (fptr, FORMAT1, "unit");
  fprintf (fptr, FORMAT1, "W50");
  fprintf (fptr, FORMAT1, "W10");
  fprintf (fptr, FORMAT1, "res");
  fprintf (fptr, FORMAT1, "RM");
  fprintf (fptr, FORMAT1, "RM_err");
  fprintf (fptr, FORMAT1, "freq");
  fprintf (fptr, FORMAT1, "bw");
  fprintf (fptr, FORMAT1, "nchan");
  fprintf (fptr, FORMAT1, "nbin");
  fprintf (fptr, FORMAT1, "filename");
  fprintf (fptr, "\n");

  fprintf (fptr, FORMAT1, archive->get_source().c_str());

  Estimate<double> intensity = stats.get_total_intensity () / nbin;
  fprintf (stderr, "flux       %lf mJy\n", intensity.get_value());
  fprintf (stderr, "flux_err   %lf\n\n",   intensity.get_error());


  fprintf (fptr, FORMAT2, intensity.get_value());

  Estimate<double> polarization = stats.get_total_polarized () / nbin;
  fprintf (stderr, "poln       %lf mJy\n", polarization.get_value());
  fprintf (stderr, "poln_err   %lf\n\n",   polarization.get_error());

  fprintf (fptr, FORMAT2, polarization.get_value());

  Estimate<double> linear = stats.get_total_linear () / nbin;
  fprintf (stderr, "linear     %lf mJy\n", linear.get_value());
  fprintf (stderr, "linear_err %lf\n\n",   linear.get_error());

  fprintf (fptr, FORMAT2, linear.get_value());

  Estimate<double> circular     = stats.get_total_circular () / nbin;
  Estimate<double> abs_circular = stats.get_total_abs_circular () / nbin;
  fprintf (stderr, "circ       %lf mJy\n", circular.get_value());
  fprintf (stderr, "abs(circ)  %lf\n",     abs_circular.get_value());
  fprintf (stderr, "circ_err   %lf\n\n",   circular.get_error());

  fprintf (fptr, FORMAT2, circular.get_value());
  fprintf (fptr, FORMAT2, abs_circular.get_value());

  // sigma
  Estimate<double> rms = sqrt( stats.get_baseline_variance(0) );

  fprintf (fptr, FORMAT2, rms.get_value() );

  if (archive->get_scale() == Signal::Jansky)
    fprintf (fptr, FORMAT1, "mJy");
  else
    fprintf (fptr, FORMAT1, "Arb");

  double period = archive->get_Integration(0)->get_folding_period () * 1e3;

  Estimate<double> width;
  float width_err;
  width.val = ::width (archive->get_Profile(0,0,0), width_err, 50.0, 0.15);
  width.var = width_err * width_err;
  width *= period;

  fprintf (stderr, "W50        %lf ms\n", width.get_value());
  fprintf (stderr, "W50_err    %lf \n\n", width.get_error());

  // degrees written to file
  width *= 360.0/period;
  fprintf (fptr, FORMAT2, width.get_value());

  width.val = ::width (archive->get_Profile(0,0,0), width_err, 10.0, 0.15);
  width.var = width_err * width_err;

  width *= period;

  fprintf (stderr, "W10        %lf ms\n", width.get_value());
  fprintf (stderr, "W10_err    %lf \n\n", width.get_error());

  // degrees written to file
  width *= 360.0/period;
  fprintf (fptr, FORMAT2, width.get_value());

#if HAVE_PGPLOT
  // resolution in degrees
  float res_deg = plot.get_flux()->get_phase_error(archive) * 360.0;
  fprintf (fptr, FORMAT2, res_deg);
#endif

  fprintf (fptr, FORMAT2, rm.get_value());

  fprintf (fptr, FORMAT2, rm.get_error());

  fprintf (fptr, FORMAT2, archive->get_centre_frequency());

  fprintf (fptr, FORMAT2, fabs(archive->get_bandwidth()));

  fprintf (fptr, "%10u ", original_nchan);

  fprintf (fptr, "%5u/%u ", used, archive->get_nbin());

  fprintf (fptr, "%s \n", archive->get_filename().c_str());

  fclose (fptr);

  cerr << "\nrmresult: data written to rmfit.out" << endl;

  string filename = archive->get_source() + ".data";
  fptr = fopen (filename.c_str(), "w");
  if (!fptr) {
    cerr << "Could not open " << filename;
    perror ("");
  }

  fprintf (fptr, "# Source:%s Freq:%f BW:%f RM:%f nbin:%u\n",
	   archive->get_source().c_str(),
	   archive->get_centre_frequency(),
	   archive->get_bandwidth(),
	   archive->get_rotation_measure(),
	   archive->get_nbin());

  Reference::To<Pulsar::PolnProfile> profile;
  profile = archive->get_Integration(0)->new_PolnProfile(0);

  Pulsar::Profile L;
  vector< Estimate<double> > PA;

  profile->get_linear( &L );

  profile->get_orientation( PA, 0 );

  fprintf (fptr, "# bin I Q U V L PA\n");
  for (unsigned ibin=0; ibin < archive->get_nbin(); ibin++)
    fprintf (fptr, "%u %f %f %f %f %f %lf\n", ibin,
	     profile->get_Profile(0)->get_amps()[ibin],
	     profile->get_Profile(1)->get_amps()[ibin],
	     profile->get_Profile(2)->get_amps()[ibin],
	     profile->get_Profile(3)->get_amps()[ibin],
	     L.get_amps()[ibin],
	     PA[ibin].get_value());

  fclose (fptr);

  cerr << "\nrmresult: data written to " << filename << endl;

}

Reference::To<Pulsar::Archive> get_data(string filename)
{
  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(filename);

  // data -> set_filename( "Archive: " + filename );

  data -> convert_state(Signal::Stokes);

  data -> dedisperse();
  data -> tscrunch();

  data -> remove_baseline();
//  data -> defaraday();

  return data;
}

double do_maxmthd (double minrm, double maxrm, unsigned rmsteps,
		   Reference::To<Pulsar::Archive> data)
{
  if (auto_maxmthd)
    {
      // compute the maximum (and minimum) measurable rotation measure ...

      // frequency in Hz
      double nu = data->get_centre_frequency () * 1e6;
      // wavelength in metres
      double lambda = Pulsar::speed_of_light / nu;
      // channel bandwidth in Hz
      double dnu = fabs(data->get_bandwidth()) * 1e6 / data->get_nchan();

      double RM_max = 0.5 / (lambda*lambda) * nu / dnu;

      rmsteps = 2 * data->get_nchan();

      double step_size = 2 * RM_max / rmsteps;

      cerr << "rmfit: default auto maximum RM=" << RM_max << " steps=" << rmsteps
	   << " (step size=" << step_size << ")" << endl;

      if (auto_maxrm_dm > 0) {
	double dm = data->get_dispersion_measure();
	float dm_auto_maxrm = auto_maxrm_dm * dm;
	cerr << "rmfit: maximum RM = (" << auto_maxrm_dm << " * DM=" << dm
	     << ") = " << dm_auto_maxrm << endl;
	auto_maxrm = std::min (auto_maxrm, dm_auto_maxrm);
      }

      bool override = false;

      if (auto_maxrm > 0 && RM_max > auto_maxrm) {
	cerr << "rmfit: default auto maximum RM over limit = " << auto_maxrm 
	     << endl;
	RM_max = auto_maxrm;
	override = true; 
      }

      if (auto_minrm > 0 && RM_max < auto_minrm) {
	cerr << "rmfit: default auto maximum RM under limit = " << auto_minrm 
	     << endl;
	RM_max = auto_minrm;
	override = true;
      }

      if (auto_step_rad > 0)
	{
	  double centre_0 = nu - data->get_nchan() * dnu;
	  double lambda_0 = Pulsar::speed_of_light / centre_0;

	  double centre_1 = nu + data->get_nchan() * dnu;
	  double lambda_1 = Pulsar::speed_of_light / centre_1;

	  // cerr << "lambda_0=" << lambda_0 << " lambda_1=" << lambda_1 << endl;

	  step_size = auto_step_rad / (lambda_0*lambda_0 - lambda_1*lambda_1);

	  cerr << "rmfit: setting step size = " << step_size 
	       << " (" << auto_step_rad << " radians between two halves of band)"
	       << endl;

	  override = true;
	}

      if (override)
	{
	  rmsteps = unsigned( 2 * RM_max / step_size );
	  cerr << "rmfit: override maximum RM=" << RM_max
	       << " steps=" << rmsteps << endl;
	}

      if (auto_minsteps > 0 && rmsteps < auto_minsteps)
	{
	  rmsteps = auto_minsteps;
	  cerr << "rmfit: override steps=" << auto_minsteps 
	       << " (step size=" << 2*RM_max/rmsteps << ")" << endl;
	}

      minrm = -RM_max;
      maxrm = RM_max;
    }

  vector<float> fluxes (rmsteps);
  vector<float> rms (rmsteps);
  vector<float> err (rmsteps);  

  float rmstepsize = (maxrm-minrm)/float(rmsteps-1);

  Reference::To<Pulsar::Archive> backup = data->clone();

  for (unsigned step=0; step < rmsteps; step++)
    {
      double rm = minrm + step * rmstepsize;

      /*
	WvS, 12 July 2006: Note that Archive::defaraday can handle the
	case that the data have already been corrected for a certain
	rotation measure.

	Wvs, 26 September 2007: Then again, perhaps round-off error
	can build up over many iterations.
      */

      data->set_rotation_measure( rm );
      data->defaraday ();

      Reference::To<Pulsar::Archive> useful = data->clone();
      useful->fscrunch();
      useful->remove_baseline();

      Pulsar::PolnProfileStats stats;
      stats.set_profile( useful->get_Integration(0)->new_PolnProfile(0) );
      Estimate<float> rval = stats.get_total_linear ();

      fluxes[step] = rval.get_value();
      err[step] = rval.get_error();
      rms[step] = rm;

      data = backup->clone();
    }

#if HAVE_PGPLOT

  float ymin = fluxes[0];
  float ymax = fluxes[0];

  if (display)
  {
    cpg_next();

    for( unsigned i=0; i<rms.size(); i++){
      if( fluxes[i]-err[i] < ymin )
	ymin = fluxes[i]-err[i];
      if( fluxes[i]+err[i] > ymax )
	ymax = fluxes[i]+err[i];
    }

    float yrange = ymax-ymin;
    ymin -= 0.1*yrange;
    ymax += 0.1*yrange;

    float buffer = fabs((maxrm-minrm)/float(rmsteps));
    cpgswin(minrm-buffer, maxrm+buffer, ymin, ymax);
    cpgbox("BCINTS", 0.0, 0, "BCINTS", 0.0, 0);
    cpglab("Rotation Measure", "Polarised Flux", "");
    for (unsigned k = 0; k < rms.size(); k++) {
      cpgpt1(rms[k], fluxes[k], 0);
      cpgerr1(6, rms[k], fluxes[k], err[k], 1.0);
      cpgsci(1);
    }

  }

#endif

  unsigned index = max_element(fluxes.begin(), fluxes.end()) - fluxes.begin();
  float max = fluxes[index];

  // fit data only to the first set of minima
  unsigned index_min = 0;
  for (index_min = index -1; index_min > 1; index_min --)
    if (fluxes[index_min] > fluxes[index_min+1]) {
      index_min ++;
      break;
    }

  unsigned index_max = index;
  for (index_max = index +1; index_max < rmsteps; index_max ++)
    if (fluxes[index_max] > fluxes[index_max-1]) {
      index_max --;
      break;
    }

  double bestrm = rms[index];

  // Fit a Gaussian curve to the points      
  float threshold = 0.001;

  MEAL::Gaussian gm;

  gm.set_centre (bestrm);
  gm.set_width  (rms[index_max] - rms[index_min]);
  gm.set_height (max);

  cerr << "initial peak index=" << index 
       << "  (" << index_min << "->" << index_max << ") or\n"
       << "  centre=" << bestrm << "  width=" << gm.get_width() 
       << "  height=" << gm.get_height() << endl;

  MEAL::Axis<double> argument;
  gm.set_argument (0, &argument);

  vector< MEAL::Axis<double>::Value > data_x; // x-ordinate of data
  vector< Estimate<double> > data_y; // y-ordinate of data with error

  for (unsigned j = index_min; j < index_max; j++) {
    data_x.push_back ( argument.get_Value(rms[j]) );
    data_y.push_back( Estimate<double>(fluxes[j],err[j]) );
  }

  if (data_x.size() == 0) {
    cerr << "WARNING: zero width; returning best RM = " << bestrm << endl;
    return bestrm;
  }

  MEAL::LevenbergMarquardt<double> fit;

  try {

    float chisq = fit.init (data_x, data_y, gm);
    if (verbose)
      cerr << "initial chisq = " << chisq << endl;

    unsigned iter = 1;
    unsigned not_improving = 0;
    while (not_improving < 25) {
      if (verbose)
	cerr << "iteration " << iter << endl;

      float nchisq = fit.iter (data_x, data_y, gm);

      if (verbose)
	cerr << "     chisq = " << nchisq << endl;

      if (nchisq < chisq) {
	float diffchisq = chisq - nchisq;
	chisq = nchisq;
	not_improving = 0;
	if (diffchisq/chisq < threshold && diffchisq > 0) {
	  if (verbose)
	    cerr << "No big diff in chisq = " << diffchisq << endl;
	  break;
	}
      }
      else
	not_improving ++;

      iter ++;
    }

    double free_parms = data_x.size() + gm.get_nparam();

    cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
	 << chisq / free_parms << endl;

    cerr << "Width="<< gm.get_width() <<" Height="<< gm.get_height() << endl;

    bestrm = gm.get_centre();

    // Compute the error based on how far you have to move the
    // centre of the Gaussian before the two-sigma

    float aim = chisq + 4.0;
    float cst = chisq;

    double raw = gm.get_centre();
    double itr = 0.0;

    //  fprintf(stderr,"Got raw=%f cst=%f chisq=%f aim=%f\n",raw,cst,chisq,aim);
    double maxitr = gm.get_width() / 2.0;
    while (cst < aim && itr < maxitr) {
      gm.set_centre(raw + itr);
      cst = fit.init(data_x, data_y, gm);
      itr += 0.01;
    }

    cout << "Best RM is: " << bestrm << " +/- " << itr << endl;

    best_search_rm.set_value ( bestrm );
    best_search_rm.set_error ( itr );

  }
  catch (Error& error) {
    cerr << "rmfit: failed to fit for best RM.  Using peak value." << endl;
    cout << "Best RM is: " << bestrm << endl;
  }

#if HAVE_PGPLOT

  if (display) {

    cpgsci(2);
    cpgsls(2);
    cpgmove(bestrm, ymax);
    cpgdraw(bestrm, ymin);
    cpgsci(1);
    cpgsls(1);

    gm.set_centre(bestrm);
    gm.set_abscissa(rms[0]);
    cpgmove(rms[0],gm.evaluate());
    cpgsci(2);

    for (unsigned k = 1; k < rms.size(); k++) {
      gm.set_abscissa(rms[k]);
      cpgdraw(rms[k],gm.evaluate());
    }

    cpgsci(1);
  }

#endif

  return bestrm;
}

void do_refine (Reference::To<Pulsar::Archive> data, bool log_results)
{
  Pulsar::DeltaRM delta_rm;

  if (refine_threshold)
    delta_rm.set_threshold (refine_threshold);

  delta_rm.set_include (include_bins);
  delta_rm.set_exclude (exclude_bins);

  bool converged = false;
  unsigned iterations = 0;

  Estimate<double> old_rm;
  Estimate<double> new_rm;

  while (!converged) {

    if (iterations > max_iterations) {

      cerr << "rmfit: maximum iterations (" << max_iterations << ") exceeded"
	   << endl;

      cerr << "new=" << new_rm << " old=" << old_rm << endl;

      double diff_new = fabs( best_search_rm.get_value() - new_rm.get_value() );
      double diff_old = fabs( best_search_rm.get_value() - old_rm.get_value() );

      if ( diff_old < diff_new )
	new_rm = old_rm;

      cerr << "rmfit: best search RM=" << best_search_rm 
	   << " using closest RM=" << new_rm << endl;

      if (log_results)
	rmresult (data, new_rm, data->get_nbin());

      return;

    }

    old_rm = new_rm;

    try {
      delta_rm.set_data (data->clone());
      delta_rm.refine ();
    }
    catch (Error& error) {
      cerr << "\nrmfit: DeltaRM::refine failed \n\t" << error.get_message() << endl;
      cerr << "rmfit: using best search RM=" << best_search_rm << endl;
      if (log_results)
	rmresult (data, best_search_rm, data->get_nbin());
      return;
    }

    cerr << endl;

    new_rm = delta_rm.get_rotation_measure();
    double new_RM = new_rm.get_value();
    double err_RM = new_rm.get_error();
    double old_RM = data->get_rotation_measure();

    converged = fabs (old_RM - new_RM) <= err_RM;
    iterations ++;

    if (iterations > max_iterations / 2) {
      new_RM = (new_RM + old_RM) / 2.0;
      cerr << "Getting old ... try mean=" << new_RM << endl;
    }

    data->set_rotation_measure (new_RM);

  }

  cerr << "rmfit: converged in " << iterations << " iterations" << endl;
  cerr << "       final rotation measure = " 
       << delta_rm.get_rotation_measure() << endl;

  if (log_results)
    rmresult (data, delta_rm.get_rotation_measure(), delta_rm.get_used_bins());
}

#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/ReceptionModel.h"
#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/Faraday.h"
#include "MEAL/ProductRule.h"
#include "MEAL/Gain.h"

double get_frequency (const Pulsar::Archive* data)
{
  if (data->get_faraday_corrected())
    return data->get_centre_frequency();
  else
    return data->get_Integration(0)->get_centre_frequency(0);
}

void mtm_estimate (Pulsar::Archive* std, Pulsar::Archive* obs) try
{
  Pulsar::PolnProfileFit mtm;

  MEAL::ProductRule<MEAL::Complex2> xform;
  MEAL::Gain<MEAL::Complex2> gain;
  Calibration::Faraday faraday;

  xform.add_model (&gain);
  xform.add_model (&faraday);

  mtm.set_transformation (&xform);

  faraday.set_reference_frequency( get_frequency(std) );
  faraday.set_frequency( get_frequency(obs) );
  faraday.set_rotation_measure( std->get_rotation_measure() );

  cerr << "initial RM=" << std->get_rotation_measure() << endl;

  // mtm.set_choose_maximum_harmonic (true);
  mtm.set_maximum_harmonic (45);
  mtm.set_standard( std->get_Integration(0)->new_PolnProfile(0) );

  cerr << "rmfit: last MTM harmonic = " << mtm.get_nharmonic() << endl;

  mtm.add_observation( obs->get_Integration(0)->new_PolnProfile(0) );

  mtm.solve ();

  float chisq = mtm.get_equation()->get_solver()->get_chisq();
  float nfree = mtm.get_equation()->get_solver()->get_nfree();

  cerr << "reduced chisq=" << chisq <<"/"<< nfree <<"="<< chisq/nfree << endl;
  cerr << "gain=" << gain.get_gain() << endl;
  cerr << "phase=" << mtm.get_phase() << endl;

  cerr << "final RM=" << faraday.get_rotation_measure() << endl;

}
catch (Error& error)
{
  cerr << "rmfit: Error MTM " << obs->get_filename() << error << endl;
}


void
do_singlebin(Reference::To<Pulsar::Archive> data,
	  float x1,float x2,bool display,
	  const vector<int> goodchans,
	  vector<double> freqs,
	  vector<double>& pa, vector<double>& pa_stddev,
	  float nsigma)
{
  int nbin = data->get_nbin();



  int bin1 = int(x1 * nbin);
  int bin2 = int(x2 * nbin);


  for(int ibin=bin1; ibin < bin2+1; ibin++)
  {
  	pa_plot( ibin, ibin+1, data, display,
	 	  pa, pa_stddev, freqs, goodchans, nsigma);
  }  
}



void
do_window(Reference::To<Pulsar::Archive> data,
	  float x1,float x2,bool display,
	  const vector<int> goodchans,
	  vector<double> freqs,
	  vector<double>& pa, vector<double>& pa_stddev,
	  float nsigma)
{
  int nbin = data->get_nbin();

  int bin1 = int(x1 * nbin);
  int bin2 = int(x2 * nbin);

  pa_plot( bin1, bin2, data, display,
	   pa, pa_stddev, freqs, goodchans, nsigma);
}








void
pa_plot( int rise_bin, int fall_bin,
	 Reference::To<Pulsar::Archive> data,
	 bool display,
	 vector<double>& pa, vector<double>& pa_stddev,
	 vector<double>& freqs, const vector<int>& goodchans,
	 float nsigma)
{
  int nbin = data->get_nbin();
  int good_nchan = goodchans.size();
  
  vector<int> vgoodchans;



  goodfreqs.clear();
  goodpa.clear();
  goodpa_stddev.clear();
  
  goodi.clear();
  goodirms.clear();

  goodv.clear();
  goodvrms.clear();
  
  goodl.clear();
  goodlrms.clear();

  static int iplot = -1;
  iplot++;

  for (int i = 0; i < good_nchan; i++) { // channel loop (begin)	  

    float* ichan = data->get_Profile(0,0,goodchans[i])->get_amps();   
    float* qchan = data->get_Profile(0,1,goodchans[i])->get_amps();
    float* uchan = data->get_Profile(0,2,goodchans[i])->get_amps();   
    float* vchan = data->get_Profile(0,3,goodchans[i])->get_amps();   


    Pulsar::Profile L;
    vector<float> lchan_vec;
    
    for (int j = 0; j < nbin; j++) 
        lchan_vec.push_back(sqrt(qchan[j]*qchan[j]+uchan[j]*uchan[j]));
    L.set_amps(lchan_vec);
    
    L -= L.mean(L.find_min_phase());
    float* lchan = L.get_amps();

    
    double lmin_avg = 0.;
    double lmin_var = 0.;
    
    L.stats(L.find_min_phase(),&lmin_avg,&lmin_var);
    float lrms = sqrt(lmin_var);
    
    Pulsar::Profile I = data->get_Profile(0,0,goodchans[i]);
    
    I -= I.mean(I.find_min_phase());

   
    double imin_avg = 0.;
    double imin_var = 0.;
    
    I.stats(I.find_min_phase(),&imin_avg,&imin_var);
    float irms = sqrt(imin_var);




///////////////////// Final sums for bins above threshold ///////////////////////    

    float tot_i = 0.0;
    float tot_q = 0.0;
    float tot_u = 0.0;
    float tot_v = 0.0;
    float tot_l = 0.0;
    
    int good_nbins=0;
    if(rise_bin<=fall_bin){
      for (int ibin = rise_bin; ibin < fall_bin; ibin++) {


	float snr = lchan[ibin]/lrms;
	
	if(snr > lthresh){      


	  tot_q += qchan[ibin];
	  tot_u += uchan[ibin];
          tot_v += vchan[ibin];
	  
	  tot_i += ichan[ibin];

	  good_nbins++;

	}
      }
    }
    else{
      for (int ibin = rise_bin; ibin < nbin; ibin++) {


	float snr = lchan[ibin]/lrms;

	if(snr > lthresh){      

	  tot_q += qchan[ibin];
	  tot_u += uchan[ibin];
          tot_v += vchan[ibin];
	  tot_i += ichan[ibin];

	  

	  good_nbins++;

	}
      }
      for (int ibin = 0; ibin < fall_bin; ibin++) {


	float snr = lchan[ibin]/lrms;

	if(snr > lthresh){      

	  tot_q += qchan[ibin];
	  tot_u += uchan[ibin];
          tot_v += vchan[ibin];
	  tot_i += ichan[ibin];



	  good_nbins++;

	}
      }

    }

    float mult  = sqrt(float(good_nbins));
    float vrms = mult * sqrt( variance(vchan,vchan+nbin) );
    float qrms = mult * sqrt( variance(qchan,qchan+nbin) );
    float urms = mult * sqrt( variance(uchan,uchan+nbin) );

    if (i==0 || i==8 || i==16) 
             cerr <<endl<<endl<<"***********"<< "CHANNEL " << goodchans[i] <<" : ["
	          << good_nbins << " summed]" 
		  << endl<<"***********"<<endl<<endl;


  


    tot_l = sqrt(tot_q*tot_q+tot_u*tot_u);

    goodl.push_back(tot_l);
    goodlrms.push_back(lrms);

    if(1.0/sqrt(float(good_nbins))*tot_l/irms >= 0.5*M_PI) // unbiasing
          tot_l=sqrt(pow(1.0/sqrt(float(good_nbins))*tot_l/irms,2.0)-1.0)*irms;
    else
          tot_l=0.0;
	  


	
    goodfreqs.push_back(freqs[i]);
    
    goodpa.push_back(90.0 + 90.0/M_PI * atan2 (tot_u, tot_q));    

    goodi.push_back(tot_i);
    goodirms.push_back(irms);
    goodv.push_back(tot_v);
    goodvrms.push_back(vrms);
    goodq.push_back(tot_q);
    goodqrms.push_back(qrms);
    goodu.push_back(tot_u);
    goodurms.push_back(urms);


    float PA_err=0.5*(sqrt(float(good_nbins))*irms/tot_l);// (works well for snr>~10)

    float P0=0.5*(1.0/PA_err);

    float true_PA_err=PA_err;

    if(P0 <= 9.99){
         true_PA_err=lookup_PA_err(P0);// derived from the pdf of the PA function (Everett & Weisberg (2001))
    }

  
    goodpa_stddev.push_back((180.0/M_PI)*true_PA_err); 

    vgoodchans.push_back(i);  



  }// channel loop (end)


  
  if(vgoodchans.size()<2) return;


  good_fbscrunch = true;

  if(lthresh>0.) 
      cerr <<endl<<endl<<endl<< "  ***  Got "<<vgoodchans.size()<<" channels above threshold! *** " <<endl<<
                            "Proceeding with fit ..." <<endl<<endl;  
  else
      cerr <<endl<<endl<<endl<< "  ***  Summed "<<vgoodchans.size()<<" channels  *** " <<endl<<
                            "Proceeding with fit ..." <<endl<<endl;  
  

    ofstream info;
    
    info.open("PAs.dat",ios::app);
    
    for (unsigned k=0; k<goodfreqs.size(); k++) 
         info << rise_bin << "  " << goodfreqs[k]<<" "<<goodpa[k] <<" " <<goodpa_stddev[k]<<endl;
	 
    info.close();
	 


//************************** Fit with Aris's routine ********************



  double PA_min = 0.0;
  double PA_max = 180.0;
  double PA_step = 1.0;
  
  
  KaraFit PAfit;
  
  PAfit.passData(goodpa,goodpa_stddev,goodfreqs,minrm,maxrm,rmstep,PA_min,PA_max,PA_step);

  if(PAfit.doFit()==1) exit(1);
  
  double RM = PAfit.fitValues[0];  
  double pa0 = PAfit.fitValues[1];
  double probmax = PAfit.fitValues[3];


////////////// Error interpolation (2D) /////////////////////////////////////////

  float RM_luperr=0.;
  
  float mean_pa_stddev=0.;
  for (unsigned i=0; i<goodpa_stddev.size(); i++)
    mean_pa_stddev += goodpa_stddev[i];
  mean_pa_stddev /= float(goodpa_stddev.size()); 
  if(mean_pa_stddev<1.) mean_pa_stddev = 1.; // if the error is very small, just use the minimum (1 deg) on the lookup table.
  


  vector<float> xint;
  xint.push_back(fabs(RM));
  xint.push_back(mean_pa_stddev);

  
  cerr<<"Error will be based on (RM,PA_err) = (" 
      << xint[0] <<","<< xint[1] << endl;

  //////// safe set //////////
  
  vector<float> xsafe;
  


  /// RM ///
  float val_diff=0.;
  float diff_min=10000.;
  float table_val = 0.; 
  int safe_val=0;
  for(unsigned si=0; si<11; si++){

    val_diff = fabs(xint[0]-table_val);
    
    if(val_diff<=diff_min){
     
      diff_min = val_diff;
      safe_val=int(table_val);
      
    }
    if(int(table_val)==0) table_val=5.;
    else if(int(table_val)==5) table_val=10.;
    else if(int(table_val)==10) table_val=20.;
    else if(int(table_val)==20) table_val=50.;
    else if(int(table_val)==50) table_val=100.;
    else if(int(table_val)==100) table_val=200.;
    else if(int(table_val)==200) table_val=300.;
    else if(int(table_val)==300) table_val=500.;
    else if(int(table_val)==500) table_val=800.;
    else table_val=1000.;
  }
  
  xsafe.push_back(safe_val);

  /// PA_err ///
  val_diff=0.;
  diff_min=10000.;
  table_val = 1.; 
  safe_val=0;
  for(unsigned si=0; si<63; si++){

    val_diff = fabs(xint[1]-table_val);
    
    if(val_diff<=diff_min){
     
      diff_min = val_diff;
      safe_val=int(table_val);
      
    }

    table_val+=1.;

  }

  xsafe.push_back(safe_val);



///////////////////////////////

//// Out of range checks ////

  if(   xint[0]<0. || xint[0]>1000. 
     || xint[1]<1. || xint[1]>63.){
  
    cerr <<endl<<endl
         << "*** Out of range values detected!!! ****"<<endl
	 <<"Using safe error from (RM,PA_err) = (" << xsafe[0] <<","<< xsafe[1] <<")" <<endl<<endl; 


    outofrange_err = true;
    
    RM_luperr = lookup_RM_err(xsafe); //Requires header file "4D_interpol.h" and data file "2D_data.asc"
    
    while(fabs(RM_luperr)<1e-5){
    
     cerr <<endl<<endl<< "*** Zero error found!!! ***"<<endl<<"Using the next non-zero error value ..." <<endl;
     
     xsafe[1]+=1.;
     
     RM_luperr = lookup_RM_err(xsafe);
     
    
    }
  
  } 
  else{ 

    cerr <<endl<<endl
         << "Interpolating from (RM,PA_err) = (" << xint[0] <<","<< xint[1] << ")" <<endl<<endl; 

    RM_luperr = lookup_RM_err(xint); //Requires header file "4D_interpol.h" and data file "2D_data.asc"

    vector<float> xxsafe = xsafe;

    float RM_luperr_safe = lookup_RM_err(xxsafe); //Requires header file "4D_interpol.h" and data file "2D_data.asc"
    
    while(fabs(RM_luperr_safe)<1e-5){
    
     xxsafe[1]+=1.;
     
     RM_luperr_safe = lookup_RM_err(xxsafe);
     
    }

    float RM_luperr_safe_down = 0.;
    float RM_luperr_safe_up = 0.;

    if(fabs(RM_luperr)<1e-5){
      RM_luperr = RM_luperr_safe;
      zero_err = true;
    }
    else{
     
      xxsafe = xsafe;

      if(RM_luperr_safe>RM_luperr){

	RM_luperr_safe_up = RM_luperr_safe;

	xxsafe[1]=xsafe[1];
	xxsafe[1]-=1.;
	if(xxsafe[1]<1.) xxsafe[1]=1.;

	RM_luperr_safe_down = lookup_RM_err(xxsafe);

      }
      else{

	RM_luperr_safe_down = RM_luperr_safe;

	xxsafe[1]=xsafe[1];
	xxsafe[1]+=1.;
	if(xxsafe[1]>63.) xxsafe[1]=63.;

	RM_luperr_safe_up = lookup_RM_err(xxsafe);

      }
    
      if((RM_luperr > RM_luperr_safe_up && RM_luperr > RM_luperr_safe_down) ||
	 (RM_luperr < RM_luperr_safe_up && RM_luperr < RM_luperr_safe_down)){

	cerr <<endl<<endl<< "*** Anomalous RM_err value found!!! ("<<RM_luperr<<") ***" 
	     << endl << "Using the top error of its subrange ..." <<endl<<endl;


        anomalous_err = true;
	RM_luperr = RM_luperr_safe_up;
	if(fabs(RM_luperr)<1e-5) RM_luperr = RM_luperr_safe;

      }

    }

    

  }




  cerr <<endl<<endl<< " ***  Results from Karastergiou fitting (RM) and Monte Carlo (lookup) errors *** "<<endl;
  cerr << "***   RM = " << RM <<" +/- "<< RM_luperr << " rad/m^2"<<endl;   
  cerr <<endl<<       " ********************************************************************** "<<endl<<endl;

  fbscr_RMs.push_back(RM);
  fbscr_RM_errs.push_back(RM_luperr);
  fbscr_RM_probmax.push_back(probmax);

  
  if(probmax >= best_fbscr_probmax){
  
    best_fbscr_probmax = probmax;
    best_fbscr_RM = RM;
    best_fbscr_RM_err = RM_luperr;
    
  }


/////////////////////////////////////////////////////////////////////////////////////
//////      STORE SOME BINWISE STATS /////


double V0 = 0.0;
double Vslope = 0.0;
double V0err = 0.0;
double Vslopeerr = 0.0;
double Vchi2 = 0.0;
double Vfitgoodness = 0.0;

vector<double> ffreqs;

vector<double> VoverI;
vector<double> VoverIerr;
vector<double> LoverI;
vector<double> LoverIerr;

for(unsigned m=0; m<goodfreqs.size(); m++)
{
 VoverI.push_back(goodv[m]/goodi[m]);
 VoverIerr.push_back(  sqrt(  pow(goodvrms[m],2)/pow(goodi[m],2) + pow(goodv[m],2)/pow(goodi[m],4)*pow(goodirms[m],2)  )   );
 LoverI.push_back(goodl[m]/goodi[m]);
 LoverIerr.push_back(  sqrt(  pow(goodlrms[m],2)/pow(goodi[m],2) + pow(goodl[m],2)/pow(goodi[m],4)*pow(goodirms[m],2)  )   );
}


lin_chi2_fit(goodfreqs,VoverI,VoverIerr,true,V0,Vslope,V0err,Vslopeerr,Vchi2,Vfitgoodness);

delta_V.push_back(Vslope*(goodfreqs[goodfreqs.size()-1]-goodfreqs[0]));
delta_Verr.push_back(fabs(Vslopeerr*(goodfreqs[goodfreqs.size()-1]-goodfreqs[0])));


double L0 = 0.0;
double Lslope = 0.0;
double L0err = 0.0;
double Lslopeerr = 0.0;
double Lchi2 = 0.0;
double Lfitgoodness = 0.0;


lin_chi2_fit(goodfreqs,LoverI,LoverIerr,true,L0,Lslope,L0err,Lslopeerr,Lchi2,Lfitgoodness);

delta_L.push_back(Lslope*(goodfreqs[goodfreqs.size()-1]-goodfreqs[0]));
delta_Lerr.push_back(fabs(Lslopeerr*(goodfreqs[goodfreqs.size()-1]-goodfreqs[0])));





  set_plotparams(pa0,RM,RM_luperr);

#if HAVE_PGPLOT

  if (display)
    do_display(goodfreqs, goodpa, goodpa_stddev, data->get_filename()); //position angle and frequency for a single band can be accessed here

/////////////      PLOT STOKES PARAMETERS ACROSS THE BAND   ///////////////
  if(plotv)
  {
     cpgend();
     
     cpgbeg(0,"Vflux.ps/ps",0,0);

     double vmin = 0.0, vmax = 0.0;  
     minmax (VoverI, vmin, vmax);

     cpg_next ();

     cpgsvp(0.1, 0.9, 0.2, 0.8);

     double flo = goodfreqs.front();
     double fhi = goodfreqs.back();

     if( flo > fhi )
       swap(flo,fhi);

     double frange = fhi - flo;

     cpgswin (flo - 0.05*frange, fhi + 0.05*frange, vmin-1.0*(vmax-vmin), vmax+1.0*(vmax-vmin));

     cpgsch(1.2);
     cpgscf (2);
     cpgbox("BCINTS", 0.0, 0, "BCINTS", 0.0, 0);    
     cpglab("Frequency (MHz)", "V (a.u.)", " ");

     cpgslw(1);
     cpgsci(1);
     cpgsch(1.0);

     float vfit_x[2];
     float vfit_y[2];

     ofstream Vflux;
     Vflux.open("QUVflux.out");

     for (unsigned k = 0; k < goodfreqs.size(); k++)
     {
       cpgpt1(goodfreqs[k], VoverI[k], 3);
       cpgerr1(6, goodfreqs[k], VoverI[k], VoverIerr[k], 1.0);
       cpgsci(2);
       cpgsls(2);
       
       vfit_x[0] = goodfreqs[0];
       vfit_x[1] = goodfreqs[goodfreqs.size()-1];
       vfit_y[0] = VoverI[0];
       vfit_y[1] = VoverI[0]+Vslope*(goodfreqs[goodfreqs.size()-1]-goodfreqs[0]);
       cpgline(2,vfit_x,vfit_y);
       cpgsci(1);
       cpgsls(1);

      cerr << "V0 = " << V0 << "   Vslope = " << Vslope <<endl;

       Vflux << setw(10) << goodfreqs[k] 
             << "  " << setw(10) << goodi[k] <<"  " << setw(10) << goodirms[k]
             << "  " << setw(10) << goodq[k] <<"  " << setw(10) << goodqrms[k]
             << "  " << setw(10) << goodu[k] <<"  " << setw(10) << goodurms[k]
             << "  " << setw(10) << goodv[k] <<"  " << setw(10) << goodvrms[k] <<endl;
     }
  
     Vflux.close();
     
     cpgend();

  }

#endif

}











#if HAVE_PGPLOT

void
do_display(vector<double> freqs,
	   vector<double> pa,
	   vector<double> pa_stddev,
	   string filename)
{
  double ymin = 0.0, ymax = 0.0;  
  minmax (pa, ymin, ymax);
  double errmax = *max_element (pa_stddev.begin(), pa_stddev.end());
  
  cpg_next ();


  cpgpap(8.0,1.5);
  cpgsvp(0.15, 0.95, 0.58, 0.95);
  
  double flo = freqs[0];
  double fhi = freqs.back();

  if( flo > fhi )
    swap(flo,fhi);
  
  double frange = fhi - flo;


  if( verbose )
    fprintf(stderr,"Calling cpgswin(%f,%f,%f,%f) where errmax=%f\n",
	    flo, fhi, ymin - errmax, ymax + errmax, errmax);
  


  cpgswin (flo - 0.05*frange, fhi + 0.05*frange, -20.0, 200.0);
  
  
  cpgsch(1.0);
  cpgscf (2);
  cpgbox("BCITS", 0.0, 0, "BCINTS", 0.0, 0);    
  cpgmtxt("L",4.0,.5,.5,"Position Angle (deg)");

  
  cpgslw(1);
  cpgsci(7);
  cpgsch(1.0);
  
  for (unsigned k = 0; k < freqs.size(); k++) {
    if( verbose )
      fprintf(stderr,"Plotting %f %f\n",freqs[k],pa[k]);
    cpgpt1(freqs[k], pa[k], 0);
    cpgerr1(6, freqs[k], pa[k], pa_stddev[k], 1.0);
  }
  cpgsci(1);
}

#endif




void 
fitstuff(vector<double> freqs,vector<double> pa,vector<double> pa_stddev,
	 bool display,Reference::To<Pulsar::Archive> data,vector<int> good_chans)
{
  int good_nchan = good_chans.size();

  double a = 0.0, b = 0.0, siga = 0.0, sigb = 0.0, chi2 = 0.0, prob = 0.0;
  
  cerr << "Fitting..." << endl;    
  lin_chi2_fit(freqs,pa,pa_stddev,true,a,b,siga,sigb,chi2,prob);
  cerr << "Fit Complete." << endl;





  double bw = data->get_bandwidth();
  double cf = data->get_centre_frequency();

#if HAVE_PGPLOT

  if (display) {

    vector<float> plotf;
    vector<float> plotpa;
    vector<float> pa_resids;
    vector<float> pa_BayesMeasures;
    float dumdum = 0.;

    unsigned fsize = goodfreqs.size();
    float fbegin = goodfreqs[0];
    float fend = goodfreqs[fsize-1];

    float plotf_step = (fend-fbegin)/float(fsize-1);
    
    float pf = fbegin;
    float ppa = plotpa0;

    const double c0 = Pulsar::speed_of_light * 1e-6;

    for (unsigned i=0; i<fsize; i++)
    {
      plotf.push_back(freqs[i]);

      ppa = plotpa0+(180./M_PI)*c0*c0*plotRM*( 1./pow(freqs[i],2) - 1./pow(fbegin,2) );

      ppa = modf(ppa/180.,&dumdum);
      ppa *= 180.;
      if(ppa < 0.0) ppa += 180;
      
      plotpa.push_back(ppa);

      double minresid = min(double(pa[i]),double(ppa));
      double maxresid = max(double(pa[i]),double(ppa));

      float resid = min(maxresid-minresid,180.-(maxresid-minresid));

      
      if(pa[i]>ppa) resid *=-1.0;
      
      pa_resids.push_back(resid); //store fit residuals

      float BayesMeasure = -resid*resid/(2.0*pa_stddev[i]*pa_stddev[i]);
      
      
      pa_BayesMeasures.push_back(BayesMeasure);



      pf += plotf_step;
      


    }
    

    stringstream RM_strm,RM_err_strm;
    RM_strm << plotRM;
    RM_err_strm << plotRM_err;
  
    string RM_label="RM=";
    string radpm2=" rad/m^2";
    string plusminus="+/-";
    string psrstr="PSR ";
    
    string RM_str=RM_label+RM_strm.str()+plusminus+RM_err_strm.str()+radpm2;
    string jname_str=psrstr+data->get_source();
    
    const char* RM_char=RM_str.c_str();
    const char* jname_char=jname_str.c_str();

    cerr << " RM(char) = " << RM_char <<endl;

    cpgsci(3);
    cpgsls(1);
    cpgmove(fbegin, plotpa.front());
    cpgline(plotf.size(),&plotf.front(),&plotpa.front());
    cpgsci(8);
    cpgsch(0.85);
    cpgmtxt("B",-0.8,0.02,0.0,RM_char);

    cpgsci(1);
    cpgsls(1);
    cpgsch(1.0);
    cpgmtxt("T",2.0,0.5,0.5,jname_char);

    float x1,x2,y1,y2;
    cpgqvp(0, &x1, &x2, &y1, &y2);

    float winht = fabs(y2-y1); 

    cpgsvp(x1,x2,y1-0.55*winht,y1-0.08*winht);

    float maxval=-1000.0;
    float minval=1000.0;
    float maxerrval=-1000.0;
    float minerrval=1000.0;

    for(unsigned m=0; m<pa_resids.size(); m++){

      if(pa_resids[m]>maxval) maxval=pa_resids[m];
      if(pa_resids[m]<minval) minval=pa_resids[m];
      if(pa_resids[m]+pa_stddev[m]>maxerrval) maxerrval=pa_resids[m]+pa_stddev[m]+1.0;
      if(pa_resids[m]-pa_stddev[m]<minerrval) minerrval=pa_resids[m]-pa_stddev[m]-1.0;

    }
    
    float dataht = fabs(maxval-minval);
    float dataerrht = fabs(maxerrval-minerrval);

   
    float toperrmargin = maxerrval + 0.2*dataerrht;    
    float bottomerrmargin = minerrval - 0.2*dataerrht;    

    cpgswin (fend - 0.05*fabs(fbegin-fend), fbegin + 0.05*fabs(fbegin-fend), bottomerrmargin, toperrmargin);


    cpgbox("bcnst",0.0,0,"bcnvst",0.0,0);
    cpgmtxt("L",4.0,.5,.5,"Residual (deg)");
    cpglab("Frequency (MHz)", "", " ");
     

    float residuals_array[freqs.size()];

    std::copy(pa_resids.begin(), pa_resids.end(),residuals_array);

    cpgsci(7);
    for (unsigned k = 0; k < freqs.size(); k++) {
    
      cpgpt1(freqs[k], pa_resids[k], 1);
      cpgerr1(6, freqs[k], pa_resids[k], pa_stddev[k], 1.0);
    
    }

    cpgsci(1);

    y1 -= 0.5*winht;
    y2 -= 0.2*winht;
    winht = fabs(y2-y1);

    cpgsvp(x1,x2,y1-0.6*winht,y1-0.25*winht);

    
    
    unsigned histbins = 20;
    float num[histbins];
    
    for (unsigned k=0; k<histbins; k++)
         num[k] = 0;
    
    for (unsigned k=0; k<pa_resids.size(); k++)
    {
         int ibin = int((pa_resids[k]-minval)/dataht*histbins);
         if(ibin>=0 && unsigned(ibin)<histbins) num[ibin]++;
    }
    
    int nummax = 0;
    for (unsigned k=0; k<histbins; k++)
          nummax = int( max( double(nummax),double(num[k]) ) );
      
    cpgswin (minval-1.0,maxval+1.0,0.0,ceil(1.01*nummax));

    cpgbox("bcsnt",0.0,0,"bcnvst",0.0,0);
    cpgmtxt("L",4.0,.5,.5,"N");
    cpglab("Residual (deg)", "", " ");
   
      
    cpghist(pa_resids.size(),residuals_array,minval-1.0,maxval+1.0,histbins,3);

    cpgsci(1);



  }

#endif

  double pa1 = a + b*(cf-((bw)/2.0));
  double pa2 = a + b*(cf+((bw)/2.0));  

  double rotation = pa2 - pa1;      
  if (freqs[0] > freqs[good_nchan-1])
    rotation = pa1 - pa2;
  

  
  rotation = rotation * ( M_PI / 180.0 );

  /*  
  float RM = rotation / (-2.0 * pow(300 / data->get_centre_frequency(), 2.0) 
			 * fabs(data->get_bandwidth() / 
				data->get_centre_frequency()));
  */

}








#if HAVE_PGPLOT

void cpg_next ()
{
  cpgsch (1.2);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  cpgsvp (0.1, 0.9, 0.2, 0.8);
  cpgpage ();
}

#endif

