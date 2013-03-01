/***************************************************************************
 *
 *   Copyright (C) 2006 by Tim Dolley and Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/***************************************************************************
 ***     spa.C  -  Single Pulse Analysis  -  T. Dolley  -  Feb 2006      ***
 ***                     Searches for giant pulses                       ***
 ***                Plots distribution of pulse fluxes                   ***
 *** Lists filename, integration number, flux and phase of giant pulses  ***
 ***************************************************************************/

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Axis.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <cpgplot.h>

#include "strutil.h"
#include "dirutil.h"
#include "Error.h"

using namespace std;

static Warning warn;

typedef struct pulse
{
  string file;           // filename
  int    intg;           // integration number
  float  flx;            // pulse flux
  float  phs;            // phase
  float  err;            // error in flux
} pulse;

typedef struct hbin
{
  float x;
  float y;
  float e;
} hbin;

//////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES

int   scan_pulses(Reference::To<Pulsar::Archive> arch, vector<pulse>& data, 
		  int method, float cphs, float dcyc);

float find_giants(vector<pulse>& data, vector<pulse>& giants, 
		  float factor,  float norm, float offs);

void  prob_hist(vector<pulse>& data, vector<hbin>& hist, unsigned nbin,
		float min = -99.0, float max = 99.0);

void  fit_gauss(vector<hbin>& data, MEAL::Gaussian& gm);

void  display_giants(vector<pulse>& giants);


// END FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

void usage ()
{
  cout << "spa - Single Pulse Analysis (finds giant pulses) \n"
    "Usage: spa [options] filenames \n"
    "  -v             Verbose mode \n"
    "  -V             Very verbose mode\n"
    "  -h             Display this help message\n"
    "  -b             Set the number of flux bins\n"
    "  -l             Use a logarithmic flux scale\n"
    "  -g <factor>    Show giant pulses with flux = <factor>*(norm flux)\n"
    "  -p <phase>     Use peak flux, calculated at phase = <phase>\n"
    "                   (enter 0.0 to use the overall peak flux)\n"
    "  -w <width>     Use flux summed over the given phase width\n"
    "                   (only with the -p or -d flags)\n"
    "  -n <norm>      Force the use of a specific normalisation factor\n"
    "  -G             Fit a Gaussian model to the probability distribution\n"
    "  -s             Show the best-fit model for the background noise\n"
       << endl;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// MAIN PROCEDURE

int main (int argc, char *argv[])
{
  bool  verbose    = false;
  bool  gaussian   = false;
  float factor     = 5.0;
  bool  log        = false;
  bool  shownoise  = false;
  int   method     = 0;
  int   bins       = 30;
  float cphs       = 0.0;
  float dcyc       = 0.0;
  float norm       = 0.0;

  int gotc = 0;

  while ((gotc = getopt(argc, argv, "qvVhlg:p:w:n:b:Gs")) != -1) {
    switch (gotc) {
    case 'h':
      usage ();
      return 0;

    case 'v':
      Pulsar::Archive::set_verbosity(2);
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'l':
      log = true;
      break;

    case 'g':
      factor = atof(optarg);
      break;

    case 'n':
      norm = atof(optarg);
      break;
      
    case 'p':
      cphs = atof(optarg);
      if (cphs > 0.0 && cphs < 1.0) {
	method = 2;
      }
      else {
	method = 1;
      }
      break;
      
    case 'w':
      dcyc = atof(optarg);
      break;

    case 'b':
      bins = atoi(optarg);
      break;

    case 'G':
      gaussian = true;
      break;

    case 's':
      shownoise = true;
      break;
      
    default:
      cout << "Unrecognised option: " << gotc << endl;
    }
  }
 
  vector<string> archives;
  vector<pulse>  pulses;
  vector<pulse>  noise;
  vector<pulse>  pgiants;
  vector<pulse>  ngiants;
  vector<hbin>   pdata;
  vector<hbin>   ndata;

  Reference::To<Pulsar::Archive> arch;

  for (int ai = optind; ai < argc; ai++)             // extract filenames
    dirglob (&archives, argv[ai]);
  
  try {
    
    cout << "Loading archives and extracting data..." << endl;
    
    for (unsigned i = 0; i < archives.size(); i++) {
      
      if (verbose) cerr << "Loading " << archives[i] << endl;

      arch = Pulsar::Archive::load(archives[i]);
      
      arch->fscrunch();        // remove frequency and polarisation resolution
      arch->pscrunch();
      arch->remove_baseline(); // Remove the baseline level

      scan_pulses(arch, pulses, method, cphs, dcyc);
      if (shownoise)
	scan_pulses(arch, noise, 2, arch->find_min_phase(dcyc), dcyc);
    }
    
    float offset = 2.0;
    
    if (shownoise) {
      offset = noise[0].flx;
      for (unsigned i = 1; i < noise.size(); i++)
	if (noise[i].flx < offset) offset = noise[i].flx;
      offset = fabs(offset);
      offset /= norm;
    }
    
    float threshold = find_giants(pulses, pgiants, factor, norm, offset);
    cout << "Detection threshold is roughly " << threshold << endl;

    display_giants(pgiants);

    prob_hist(pulses, pdata, bins);

    float submin = pdata[0].x;
    float submax = pdata[0].x;
    
    for (unsigned i = 1; i < pdata.size(); i++) {
      if (pdata[i].x > submax) submax = pdata[i].x;
      if (pdata[i].x < submin) submin = pdata[i].x;
    }

    if (shownoise) {
      find_giants(noise, ngiants, factor, norm, offset);
      prob_hist(noise, ndata, bins, submin, submax);
    }
    
    unsigned useful = 0;
    if (log) {
      vector<hbin>::iterator it = pdata.begin();
      while (it != pdata.end()) {
	if (pdata[useful].x < 0) {
	  pdata.erase(it);
	  continue;
	}
	else {
	  pdata[useful].x = logf(pdata[useful].x);
	  it++;
	  useful++;
	}
      }
      threshold = logf(threshold);
    }
    
    float xmin, xmax, ymin, ymax;
    
    xmin = xmax = pdata[0].x;
    ymin = ymax = pdata[0].y;
    
    // Find the extremes of the data set
    
    for (unsigned i = 1; i < pdata.size(); i++) {
      if (pdata[i].x > xmax) xmax = pdata[i].x;
      if (pdata[i].x < xmin) xmin = pdata[i].x;
      if (pdata[i].y > ymax) ymax = pdata[i].y;
      if (pdata[i].y < ymin) ymin = pdata[i].y;
    }

    
    // Plot pulse intensity probability distribution
    
    cpgopen("?");
    cpgsvp(0.1,0.9,0.15,0.9);
    cpgsci(7);
    cpgsch(1.3);
    
    cpgswin(xmin-(xmin/100.0), xmax+(xmax/100.0),
	    logf(ymin-(ymin/2.0)), logf(ymax+(ymax/2.0)));
    
    cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
    cpgsci(8);
    if (log)
      cpglab("Log (Normalised Pulse Flux)", "Log (P(I))", "");
    else 
      cpglab("Normalised Pulse Flux", "Log (P(I))", "");
    cpgsci(5);
    
    // Plot the pulse detection threshold
    cpgsls(3);
    cpgsci(3);
    cpgmove(threshold, logf(ymin-(ymin/2.0)));
    cpgdraw(threshold, logf(ymax+(ymax/2.0)));
    cpgsls(1);

    // Plot points and associated error bars
    for (unsigned i = 0; i < pdata.size(); i++) {
      cpgpt1(pdata[i].x, logf(pdata[i].y), 4);
      cpgerr1(6, pdata[i].x, logf(pdata[i].y), pdata[i].e/pdata[i].y, 1.0);
    }

    if (gaussian) {
          
      MEAL::Gaussian gm1;
      
      fit_gauss(pdata, gm1);
      
      gm1.set_abscissa(xmin);
      cpgmove(xmin,logf(gm1.evaluate()));

      cpgsci(2);
      cpgsls(2); 
      
      float xval = 0.0;

      for (unsigned i = 1; i < 1000; i++) {
	xval = xmin+((xmax-xmin)/1000.0*i);
	gm1.set_abscissa(xval);
	cpgdraw(xval,logf(gm1.evaluate()));
      }

      if (shownoise) {
	
	MEAL::Gaussian ngm;

	fit_gauss(ndata, ngm);
	
	ngm.set_abscissa(xmin);
	cpgmove(xmin,logf(ngm.evaluate()));
	
	cpgsci(5);
	cpgsls(4); 

	for (unsigned i = 1; i < 1000; i++) {
	  xval = xmin + ((xmax-xmin)/250.0*i);
	  ngm.set_abscissa(xval);
	  if (log)
	    cpgdraw(logf(xval),logf(ngm.evaluate()));
	  else
	    cpgdraw(xval,logf(ngm.evaluate()));
	}
      }
    }

    cpgclos();

    // Free any used memory
    archives.clear();
    pulses.clear();
    noise.clear();
    pgiants.clear();
    ngiants.clear();
    pdata.clear();
    ndata.clear();
  }
  catch (Error& error) {
    fflush(stdout); 
    cerr << error << endl;
    exit(1);
  }
  
  fflush(stdout);
  return 0;
}

// END MAIN PROCEDURE
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

int scan_pulses(Reference::To<Pulsar::Archive> arch, vector<pulse>& data, 
		int method, float cphs, float dcyc)
{
  /* Find and store the flux and phase of each pulse in the file
     to the data vector. */
  
  pulse newentry;

  Reference::To<Pulsar::Profile> prof;
  
  newentry.file = arch->get_filename();

  double nm, nv, vm;
  
  int nbin = arch->get_nbin();
  int bwid = int(float(nbin) * dcyc);
  int cbin = 0;

  for (unsigned i = 0; i < arch->get_nsubint(); i++) {

    newentry.intg = i;
    
    prof = arch->get_Profile(i, 0, 0);
    
    prof->stats(prof->find_min_phase(), &nm, &nv, &vm);
    newentry.err = sqrt(nv);

    switch (method) {
      
    case 0: // Method of total flux
      
      newentry.flx = prof->sum();
      newentry.phs = prof->find_max_phase();
      break;
      
    case 1: // Method of maximum amplitude

      if (dcyc == 0.0) {
	newentry.flx = prof->max();
	newentry.phs  = prof->find_max_phase();
      }
      else {
	cbin = int(prof->find_max_phase(dcyc) * float(nbin));
	newentry.flx = prof->sum(cbin - bwid/2, cbin + bwid/2);
	newentry.phs  = prof->find_max_phase(dcyc);
      }
      break;

    case 2: // User-defined phase centre
      
      cbin = int(float(nbin) * cphs);

      if (dcyc == 0.0) {
	newentry.flx = (prof->get_amps())[cbin];
	newentry.phs = cphs;
      }
      else {
	newentry.flx = prof->sum(cbin - bwid/2, cbin + bwid/2);
	newentry.phs = cphs;
      }
      
      break;
      
    default:
      cerr << "No phase selection method chosen!" << endl;
    }
    
    data.push_back(newentry);
  }
  
  return data.size();
}

//////////////////////////////////////////////////////////////////////////////

float find_giants(vector<pulse>& data, vector<pulse>& giants, 
		  float factor, float norm, float offs)
{
  /* Finds the mean flux from the data vector. Any flux greater than
     <factor> times the mean is considered a giant and is copied to
     the giants array. */
  
  float mean1 = 0.0;
  float mean2 = 0.0;
  float thres = 0.0;

  // First, calculate mean flux:
  
  for (unsigned i = 0; i < data.size(); i++)
    mean1 += data[i].flx;  

  mean1 /= data.size();

  // Then, normalise the flux values (put in form (flux/norm)) 
  // and find the new mean.

  for (unsigned i = 0; i < data.size(); i++)
    {
      if (norm > 0.0) {
	data[i].flx /= norm;
	data[i].flx += offs;
      }
      else {
	data[i].flx /= mean1;
	data[i].flx += offs;
      }
      mean2 += data[i].flx;
    }
  
  mean2 /= data.size();
  
  // Finally, find the giants and push onto the new vector
  
  giants.clear();
  
  for (unsigned i = 0; i < data.size(); i++) {
    if (data[i].flx > factor)
      giants.push_back(data[i]);

    thres += (data[i].flx - mean2) * (data[i].flx - mean2);
  }
  
  // Call the detection threshold the standard deviation of the
  // fluxes, added to the mean flux. I suppose this assumes most of
  // the pulses are noise-dominated so that the mean represents
  // background emission...

  thres = sqrt(thres/data.size());
  thres += mean2;

  return thres;
}

//////////////////////////////////////////////////////////////////////////////

void prob_hist(vector<pulse>& data, vector<hbin>& hist, unsigned nbin,
	       float min, float max) {
  
  hist.clear();

  if (min == -99.0 && max == 99.0) {
    max = data[0].flx;
    min = data[0].flx;
    
    for(unsigned i = 0; i < data.size(); i++) {
      if (data[i].flx < min) min = data[i].flx;
      if (data[i].flx > max) max = data[i].flx;
    }
  }

  if ( min == max )
  {
    warn << "WARNING spa::prob_hist minimal and maximal value in the data are equal; probalby due to too few pulses found ( found " << data.size () << " pulses)\n The range of histogram is being artificially adjusted..." << endl;
    min = 0.9 * min;
    max = 1.1 * max; 
  }

  float bwid = (max - min) / float(nbin);
  float a = min, b = min + bwid;

  hbin newbin;

  for(unsigned i = 0; i < nbin; i++) {

    newbin.y = 0;
    
    for(unsigned j = 0; j < data.size(); j++)
      if ((data[j].flx > a) && (data[j].flx < b)) newbin.y++;
    
    if (newbin.y > 0) {
      newbin.e = sqrt(newbin.y); // set error due to counting statistics
      newbin.x = a + bwid/2.0;
      hist.push_back(newbin);
    }
    
    a += bwid;
    b += bwid;
  }
  
  // Convert to a discrete probability distribution
  for (unsigned i = 0; i < hist.size(); i++) {
    hist[i].y /= data.size();
    hist[i].e /= data.size();
  }
}

//////////////////////////////////////////////////////////////////////////////

void fit_gauss(vector<hbin>& data, MEAL::Gaussian& gm) {

  // Instantiate all the machinery required for curve fitting

  float cutoff = 0.001;
  float chisq  = 1.0;
  float nchisq = 1.0;

  unsigned iter = 1;

  unsigned not_improving = 0;

  MEAL::Axis<double> argument;

  vector< MEAL::Axis<double>::Value > data_x;   // x-ordinate of data
  vector< Estimate<double> > data_y; // y-ordinate of data with error

  MEAL::LevenbergMarquardt<double> fit;
  
  double xmin, xmax, ymin, ymax;

  xmin = xmax = data[0].x;
  ymin = ymax = data[0].y;

  // Find the extremes of the data set

  for (unsigned i = 1; i < data.size(); i++) {
    if (data[i].x > xmax) xmax = data[i].x;
    if (data[i].x < xmin) xmin = data[i].x;
    if (data[i].y > ymax) ymax = data[i].y;
    if (data[i].y < ymin) ymin = data[i].y;
  }

  // Fit a Gaussian function to the distribution
  
  gm.set_centre((xmax + xmin)/2.0);
  gm.set_width(xmax - xmin);
  gm.set_height(ymax);
  
  gm.set_argument (0, &argument);
  
  for (unsigned i = 0; i < data.size(); i++) {
    data_x.push_back ( argument.get_Value(data[i].x) );
    data_y.push_back( Estimate<double>(data[i].y, data[i].e) );
  }
  
  chisq = fit.init (data_x, data_y, gm);
  
  iter = 1;
  not_improving = 0;
  while (not_improving < 25) {
    nchisq = fit.iter (data_x, data_y, gm);
    
    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < cutoff && diffchisq > 0) {
	break;
      }
    }
    else
      not_improving ++;
    
    iter ++;
  }
  
  cerr << "Fitted Gaussian has Chi-Squared = " << nchisq << endl;
  
}

//////////////////////////////////////////////////////////////////////////////

void display_giants(vector<pulse>& giants)
{
  /* Displays info about giant pulses on screen */
  
  cout << "---------------------------------------------------------------" 
       << endl;
  cout << "Integration   Flux          Phase     Filename" 
       << endl;
  cout << "  Number" << endl;
  cout << "---------------------------------------------------------------" 
       << endl;
  
  for(unsigned i = 0; i < giants.size(); i++)
    cout << setw(11) << giants[i].intg << setw(10) 
	 << giants[i].flx << setw(11) << giants[i].phs 
	 << "      " << giants[i].file << endl;

  cout << endl;
  
  return;
}

//////////////////////////////////////////////////////////////////////////////

// END FUNCTION DEFINITIONS
//////////////////////////////////////////////////////////////////////////////
