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

#define ZOOM 12

void gtd_wrap(int& index, int nbin) {
  if (index < 0)
    index += nbin;
  if (index > int(nbin))
    index -= nbin;
}

int main(int argc, char** argv) {
  
  bool display = false;
  bool verbose = false;

  bool stdflag   = false;
  bool synthetic = false;

  float dc = 0.05;
  int nbin = 1024;

  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> stdprof;

  int gotc = 0;
  
  vector<string> archives;
  
  while ((gotc = getopt(argc, argv, "hvVDs:Sd:")) != -1) {
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
      cout << "  -d               Duty cycle of synthetic profs"   << endl;
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
    case 'd':
      dc = atof(optarg);
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

    for (unsigned i = 0; i < 1; i++) {
      SyntheticProfile fakep(nbin, 2.0, 
			     dc*nbin, nbin/2 + (100.0*mygen.rand()));
      
      fakep.build();
      fakep.add_noise();
      profs.push_back(new Pulsar::Profile(fakep.get_Profile())); 
    }
    
    SyntheticProfile fakes(nbin, 50.0, dc*nbin, nbin/2 + (100.0*mygen.rand()));
    fakes.build();
    fakes.add_noise();
    stdprof = fakes.get_Profile().clone();
  }
  
  for (unsigned i = 0; i < profs.size(); i++) {
    
    vector<float> corr;
    MEAL::Gaussian model;

    int rise = 0;
    int fall = 0;
    int ofs  = 0;

    double shift = 0.0;
    float  error = 0.0;
    
    shift = profs[i]->GaussianShift(*stdprof, error, corr, model, 
				    rise, fall, ofs, true);
    
    unsigned nbin = profs[i]->get_nbin();
    
    if (display) {
      cpgeras();
      cpgsch(1.5);

      cpgsubp(1,2);
      cpgpanl(1,1);
      cpgeras();
      cpgsvp(0.1,0.9,0.1,0.9);
      
      float ymin = 0.0;
      float ymax = 0.0;

      findminmax(&(corr.front()), &(corr.back()), ymin, ymax);

      ymax += (ymax-ymin)/10.0;

      cpgswin(0.0,1.0,ymin,ymax);
      cpgbox ("BCINST", 0.0, 0, "BCINST", 0.0, 0);
      cpglab("", "Amplitude (Arbitrary)", "Pulse Phase");
      
      cpgmove(0.0, corr[0]);
      for (unsigned j = 1; j < nbin; j++) {
	cpgdraw(float(j)/float(nbin), corr[j]);
      }

      cpgsci(2);
      
      model.set_abscissa(double(rise));

      // Remember that RISE, FALL and the model GM have all been
      // offset by the amount required to centre them.

      for (int j = rise; j < fall; j++) {
	model.set_abscissa(double(j));
	int index = int(j) - ofs;
	gtd_wrap(index, nbin);
	cpgpt1(float(index)/float(nbin), model.evaluate(), 1);
      }

      cpgsci(5);
      cpgsls(2);

      int _rise = (rise-ofs);
      gtd_wrap(_rise, nbin);
      
      int _fall = (fall-ofs);
      gtd_wrap(_fall, nbin);

      float _r = float(_rise)/float(nbin);
      float _f = float(_fall)/float(nbin);

      cpgmove(_r, ymin);
      cpgdraw(_r, ymax);
      
      cpgmove(_f, ymin);
      cpgdraw(_f, ymax);

      cpgsci(1);
      cpgsls(1);
      
      cpgpanl(1,2);
      cpgsvp(0.1,0.5,0.15,0.9);
      
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

      ymax += (ymax-ymin)/2.0;

      float phsmin = binmin/float(nbin);
      float phsmax = binmax/float(nbin);

      cpgswin(phsmin, phsmax, ymin, ymax);
      cpgbox ("BCINST", 0.0, 0, "BCINST", 0.0, 0);
      cpglab("Pulse Phase", "Amplitude (Arbitrary)", "Interpolation Region");
      
      for (int j = binmin; j < binmax; j++) {
	cpgpt1(float(j)/float(nbin), corr[j], 0);
      }

      cpgsci(2);
      
      for (float j = float(binmin); j < float(binmax); j+= 0.01) {
	float index = j+float(ofs);
	if (index < 0.0)
	  index += float(nbin);
	if(index > float(nbin))
	  index -= float(nbin);
	model.set_abscissa(index);
	cpgpt1(j/float(nbin), model.evaluate(), 1);
      }
      
      cpgsci(3);
      cpgsls(2);
      
      float cent = model.get_centre()-float(ofs);
      if (cent < 0.0)
	cent += float(nbin);
      if (cent > float(nbin))
	cent -= float(nbin);
      
      cpgmove(cent/float(nbin), ymin);
      cpgdraw(cent/float(nbin), ymax);
      
      cpgsci(2);
      cpgsls(1);
      cpgerr1(5, (cent)/float(nbin), 
	      (ymin+ymax)/2.0, error, 2.0);
      
      // Draw the profiles
      
      cpgsci(3);

      Pulsar::Plotter plotter;

      cpgsvp(0.525,0.7,0.5,0.9);
      cpgsci(1);
      cpgbox("BCIT", 0.0, 0, "BCIT", 0.0, 0);
      cpglab("", "", "Observed Profile");
      cpgsci(7);
      plotter.plot(profs[i]);

      cpgsvp(0.725,0.9,0.5,0.9);
      cpgsci(1);
      cpgbox("BCIT", 0.0, 0, "BCIT", 0.0, 0);
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



