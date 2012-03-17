/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include <cmath>

#include <cpgplot.h>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/SyntheticProfile.h"
#include "Pulsar/box-muller.h"
#include "toa.h"
#include "Error.h"
#include "minmax.h"

#define ZOOM 4

int main(int argc, char** argv) {
  
  bool display = false;
  bool verbose = false;

  bool stdflag   = false;
  bool synthetic = false;

  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> stdprof;

  int gotc = 0;
  
  vector<string> archives;
  
  while ((gotc = getopt(argc, argv, "hvVDs:S")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for testing the TimeDomain TOA algorithm" << endl;
      cout << "Usage: rmfit [options] filenames"                   << endl;
      cout << "  -h               This help page"                  << endl;
      cout << "  -v               Verbose mode"                    << endl;
      cout << "  -V               Very verbose mode"               << endl;
      cout << "  -D               Display results"                 << endl;
      cout << "  -s               Standard profile"                << endl;
      cout << "  -S               Use synthetic profiles"          << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'D':
      display = true;
      break;
    case 's':
      try {
	stdarch = Pulsar::Archive::load(optarg);
	stdarch->centre();
	stdprof = new Pulsar::Profile(stdarch->total()->get_Profile(0,0,0));
	stdflag = true;
      }
      catch (Error& error) {
	cerr << error << endl;
	return (-1);
      }
      break;
    case 'S':
      synthetic = true;
      stdflag   = true;
      break;
      
    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  if (!stdflag) {
    cerr << "This program requires a standard profile!" << endl;
    return(-1);
  }

  // Parse the list of archives
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty() && !synthetic) {
    cerr << "No archives were specified" << endl;
    exit(-1);
  }

  if (display)
    cpgopen("?");
  
  vector<Reference::To<Pulsar::Profile> > profs;

  if (!synthetic) {
    
    Reference::To<Pulsar::Archive> data;
    
    for (unsigned i = 0; i < archives.size(); i++) {
      // Load in a file
      
      try {
	
	data = Pulsar::Archive::load(archives[i]);
	data->centre();
	
      }
      catch (Error& error) {
	cerr << error << endl;
	continue;
      }
      
      profs.push_back(new Pulsar::Profile(data->total()->get_Profile(0,0,0)));
    }
  }
  else {
    bmrng mygen;
    SyntheticProfile fakep(1024, 5.0, 40.0, 512 + (100.0*mygen.rand()));
    SyntheticProfile fakes(1024, 10.0, 40.0, 512 + (100.0*mygen.rand()));

    fakep.build();
    fakep.add_noise();
    profs.push_back(new Pulsar::Profile(fakep.get_Profile()));
    
    fakes.build();
    fakes.add_noise();
    stdprof = fakes.get_Profile().clone();
  }
    
  for (unsigned i = 0; i < profs.size(); i++) {
    
    float* corr = new float[profs[i]->get_nbin()];
    float* bins = new float[profs[i]->get_nbin()];
    float* parb = new float[profs[i]->get_nbin()];
    float* fn   = new float[3];

    double shift = 0.0;
    float   error = 0.0;
    
    shift = profs[i]->ParIntShift(*stdprof, error, corr, fn);

    unsigned nbin = profs[i]->get_nbin();

    for (unsigned j = 0; j < nbin; j++) {
      bins[j] = float(j)/float(nbin);
      parb[j] = fn[0] - fn[1]*(bins[j]-fn[2])*(bins[j]-fn[2]);
    }
    
    if (display) {

      cpgsch(1.5);

      cpgsubp(1,2);
      cpgpanl(1,1);

      cpgsvp(0.1,0.9,0.1,0.9);
      
      float ymin = 0.0;
      float ymax = 0.0;

      findminmax(corr, corr+nbin-1, ymin, ymax);

      ymax += (ymax-ymin)/10.0;

      cpgswin(0.0,1.0,ymin,ymax);
      cpgbox ("BCNST", 0.0, 0, "BCNST", 0.0, 0);
      cpglab("", "", "Cross Correlation");

      cpgline(nbin, bins, corr);
      cpgsci(2);
      cpgline(nbin, bins, parb);
      cpgsci(1);

      cpgpanl(1,2);
      cpgsvp(0.1,0.5,0.1,0.9);

      float maxphs = 0.0;
      int   maxbin = 0;
      ymax = 0.0;

      for (unsigned j = 0; j < nbin; j++) {
	if (corr[j] > ymax) {
	  ymax = corr[j];
	  maxbin = j;
	  maxphs = float(j)/float(nbin);
	}
      }

      int binmin = maxbin - ZOOM;
      int binmax = maxbin + ZOOM;

      if (binmin < 0)
	binmin = 0;

      if (binmax > int(nbin)-1)
	binmax = nbin-1;

      findminmax(&corr[binmin], &corr[binmax], ymin, ymax);

      ymax += (ymax-ymin)/10.0;

      float phsmin = bins[binmin];
      float phsmax = bins[binmax];

      cpgswin(phsmin, phsmax, ymin, ymax);
      cpgbox ("BCNST", 0.0, 0, "BCNST", 0.0, 0);
      cpglab("", "", "Interpolation Region");
      
      for (int j = binmin; j < binmax; j++) {
	cpgpt1(bins[j], corr[j], 0);
      }

      float stepsize = (phsmax-phsmin) / float(nbin);
      
      for (unsigned j = 0; j < nbin; j++) {
	bins[j] = phsmin + float(j)*stepsize;
	parb[j] = fn[0] - fn[1]*(bins[j]-fn[2])*(bins[j]-fn[2]);
      }

      cpgsci(2);
      cpgline(nbin, bins, parb);
      cpgsci(3);
      cpgsls(2);
      cpgmove(fn[2], ymin);
      cpgdraw(fn[2], ymax);

      cpgsci(2);
      cpgsls(1);
      cpgerr1(5, fn[2], (ymin+ymax)/2.0, error, 2.0);

      // Draw the profiles

      cpgsci(3);

      Pulsar::Plotter plotter;

      cpgsvp(0.525,0.7,0.5,0.9);
      cpgsci(1);
      cpgbox("BCT", 0.0, 0, "BCT", 0.0, 0);
      cpglab("", "", "Observed Profile");
      cpgsci(7);
      plotter.plot(profs[i]);

      cpgsvp(0.725,0.9,0.5,0.9);
      cpgsci(1);
      cpgbox("BCT", 0.0, 0, "BCT", 0.0, 0);
      cpglab("", "", "Standard Profile");
      cpgsci(7);
      plotter.plot(stdprof);

      cpgsvp(0.525,0.9,0.1,0.4);
      cpgsci(1);
      cpgsch(2.0);

      char* useful = new char[64];
      sprintf(useful, "%f", shift);

      cpgmtxt("T", -2.0, 0.1, 0.0, "Shift = ");
      cpgmtxt("T", -2.0, 0.3, 0.0, useful);
      cpgmtxt("T", -2.0, 0.6, 0.0, "Phase Units");

      sprintf(useful, "%f", error);
      
      cpgmtxt("T", -3.5, 0.1, 0.0, "Error = ");
      cpgmtxt("T", -3.5, 0.3, 0.0, useful);
      cpgmtxt("T", -3.5, 0.6, 0.0, "Phase Units");
    }
  }
}



