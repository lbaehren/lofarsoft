/***************************************************************************
 *
 *   Copyright (C) 2003 by Na Wang
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
//
// $Id: pas.C, v 1.00 2003/05/05 nwang 
//
// The Pulsar Archive Standard profile
//
// A program for aligning standard profiles in Pulsar::Archive objects
//

#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProfilePlot.h"

#include "Error.h"

#include "dirutil.h"
#include "strutil.h"

#include <cpgplot.h>

#include <fstream>
#include <unistd.h>
#include <math.h>
#include <string.h>

using namespace std;

void plot_it(Reference::To<Pulsar::Archive>, Reference::To<Pulsar::Archive>, int, int, int, int, char *, char *, bool, float, float);
void coef (float *, float *, int, int, double *, int *, double *, bool);
void cross_correlation(Reference::To<Pulsar::Profile>, Reference::To<Pulsar::Profile>, double *, int *, double *, bool);
void smooth (Reference::To<Pulsar::Profile>, bool);
double convt(Reference::To<Pulsar::Archive>, float, bool);
void cross(Reference::To<Pulsar::Archive>,Reference::To<Pulsar::Archive>,bool,bool,char*);


void usage ()
{
  cout << "A program for aligning standard profiles\n"
    "Usage: pas [-r reference_profile] [-v] [-V] standard_profile\n"
    "Other possible options are as follows:\n"
    " -h        This help page\n"
    " -i        Show revision information\n"
    " -a        auto-align all files to first file (overwrites originals) \n"
    " -I        auto-align using invariant profile to estimate phase shift \n"
    " -r ref    Reference profile\n"
    " -v        Verbose output\n"
    " -V        Very verbose output \n\n"

    "This program works via keyboard interaction. To set baseline zero\n"
    "you type '0' to set the start point, move the cursor and type '0'\n"
    "again to accept the region\n\n"
    "See " PSRCHIVE_HTTP "/manuals/pas for more details\n"
       << endl;
}

uint64_t power_of_two (uint64_t number)
{
  uint64_t twos = 1;
  while (twos < number)
    twos *= 2;
  if (twos != number)
    return 0;
  return 1;
}

bool auto_align = false;
bool auto_align_invariant = false;
void auto_align_files (std::vector<std::string>& filename);

int main (int argc, char** argv) 
{
  string refname, the_old, the_new;
  vector<string> stdname;
  char line[100];
  char plotdev[5];
  char opts;
  float curs_x, curs_y, x, y, meantmp =0, curs_x0;
  float stdphase=0;
  float ephase;    
  unsigned int i;
  bool verbose = false;
  bool vverbose = false;
  bool refflag = false;
  int c = 0;
  float fmax;
  bool zflag = false;
  //color of lines
  int ci_ref = 7;  //yellow  
  int ci_std = 5;  //grey
  int ci_tex = 1;  //white
  int ci_dis = 15; //dark 
  float xmin=0.0, xmax=1.0;
  const char* args = "ahiIr:vV";

  while ((c = getopt(argc, argv, args)) != -1) {
    switch (c) {

    case 'a':
      auto_align = true;
      break;

    case 'h':
      usage ();
      return 0;

    case 'i':
      cout << "$Id: pas.C,v 1.30 2009/11/22 19:47:28 straten Exp $" << endl;
      return 0;

    case 'I':
      auto_align_invariant = true;
      break;

    case 'r':
      refflag = true;
      refname = optarg;
     break;

    case 'v':
      verbose = true;
      Error::verbose = true;
      break;

    case 'V':
      vverbose = true;
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;

    default:
      cerr << "invalid param '" << c << "'" << endl;
    }
  }

  if (refflag==true && refname == "") {
    cerr << "No specified reference profile" <<endl;
    usage();
    return 0;
  }

  if(argv[optind]==NULL) {
    cerr << "No specified standard profile " << endl <<endl;
    usage();
    exit(-1);    
  }

  if (auto_align)
  {
    vector <string> filename;
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filename, argv[ai]);

    auto_align_files (filename);

    return 0;
  }

  dirglob (&stdname, argv[optind]);

  if (stdname.empty()) {
    cerr << "No specified standard profile " << endl <<endl;
    usage();
    exit(-1);
  } 

  //open new standard profile
  Reference::To<Pulsar::Archive> stdarch = Pulsar::Archive::load(stdname[0]);
  stdarch->fscrunch();
  stdarch->pscrunch();
  stdarch->tscrunch();
  stdarch->remove_baseline();
  Reference::To<Pulsar::Profile> stdprof=stdarch->get_Profile(0, 0, 0);

  if(verbose) cout << "new standard archives loaded" << endl;
  stdphase=stdarch->find_max_phase();
  if(verbose) cout <<"   original stdphase " <<stdphase <<endl;
  //rotate 0.5 phase period
  stdarch->rotate(convt(stdarch, -0.5*stdarch->get_nbin(), verbose));
  //keep the original new standard profile for reload
  Reference::To<Pulsar::Archive> stdarch0 (stdarch->clone());
  //profile to correlate
  Reference::To<Pulsar::Archive> stdcorr (stdarch->clone());
  //check profile bin number
  if( verbose ) {
    cout << "Processing profile:" << stdarch->get_filename() << endl;
    cout <<"  Profile has " << stdarch->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(stdarch->get_Profile(0, 0, 0)->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		stdarch->get_filename().c_str(),stdarch->get_nbin());

  //open original ref. archive
  if(refflag==false) refname = stdname[0].c_str();//open a dummy ref profile
  Reference::To<Pulsar::Archive> refarch = Pulsar::Archive::load(refname);
  if(verbose) cout << "ref. archive loaded" << endl;
  refarch->rotate(convt(refarch, -0.5*refarch->get_nbin(), verbose));
  //keep the original ref. archive
  Reference::To<Pulsar::Archive> refarch0 (refarch->clone());
  //profile to correlate
  Reference::To<Pulsar::Archive> refcorr (refarch->clone()); 
  //check reference profile
  if( verbose ){
    cout << "Reference profile: " << refarch->get_filename() << endl;
    cout << "  Profile has " << refarch->get_Profile(0, 0, 0)->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(refarch->get_Profile(0, 0, 0)->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		refarch->get_filename().c_str(), refarch->get_Profile(0, 0, 0)->get_nbin());

  if(refflag==true)
    cross(refcorr, stdcorr, verbose, vverbose, line);

  cout << "Input plot device : ";
  cin >> plotdev;
  //plot profiles
  plot_it(refarch, stdarch, ci_ref, ci_std, ci_tex, ci_dis, line, 
	  plotdev, refflag, xmin, xmax);

  opts = ' ';
  if (cpgcurs(&curs_x, &curs_y, &opts) == 1) {
    while (opts != 'q') {  
      try {
	switch(opts) {
	case 'a':   //Align
	  if(refflag==true) {  
	    stdprof=stdarch->get_Profile(0, 0, 0);
            Estimate<double> Ephase = stdprof->shift(refarch->get_Profile(0, 0, 0));
	    stdphase=Ephase.val;
            ephase=sqrt(Ephase.var);
	    fmax=stdphase*stdarch->get_Profile(0,0,0)->get_nbin();
	    stdarch->rotate(convt(stdarch, fmax, verbose));
	    if(verbose) cout << "Align: rotated " <<stdphase << "phase, " 
			     << fmax <<" bins" << endl;
	  }
	  else cout << "No specified ref. profile" << endl;
	  break;

	case 'b':  //Bscrunch
	  stdarch->bscrunch(2);
	  if( verbose ) cout << "Bscrunch: Profile is smoothed by factor 2" 
			     << endl;
	  break;
	 
	case 'c': {
	  //Centre Profile, not work properly according to find_max_phase()
	  stdarch->centre_max_bin();
	  if( verbose ) cout << "Centre: Centre the profile, done" <<endl;
	}
	break;
	
	case 'f':
	  stdarch->rotate (convt(stdarch, 0.05, verbose));
	  if( verbose ) cout << "Delicate rotate profile: 0.05 bin to the left" << endl;
	  break;

	case 'g':
	  stdarch->rotate (convt(stdarch, -0.05, verbose));
	  if( verbose ) cout << "Delicate rotate profile: 0.05 bin to the right" << endl;
	  break;

	case 'l':  //Left rotate
	  stdarch->rotate (convt(stdarch, 1.0, verbose));
	  if( verbose ) cout << "Rotate profile: 1 bin to the left" << endl;
	  break;

	case 'L': //Fast rotate, to the left
	  stdarch->rotate (convt(stdarch, 20.0, verbose));
	  if( verbose ) cout << "Fast rotate profile: 20 bins to the left" <<endl;
	  break;
	
	case 'm':   //smooth the profile
	  smooth(stdarch->get_Profile(0, 0, 0), vverbose);
	  if( verbose ) cout << "Smooth: profile smoothed" << endl;
	  break;

	case 'o':  //Back to Original
	  *stdarch = *stdarch0;
	  *stdcorr = *stdarch;
 	  if( verbose ) cout << "Back to original" << endl;
	  break;

	case 'q':
	  return 0;

	case 'r':  //Right rotate
	  stdarch->rotate (convt(stdarch, -1.0, verbose));
	  if( verbose ) cout << "Rotate profile: 1 bin to the right" << endl;
	  break;

	case 'R': //Fast rotate, to the right
	  stdarch->rotate (convt(stdarch, -20.0, verbose));
	  if( verbose ) cout << "Fast rotate profile: 20 bins to the right" <<endl;
	  break;
	  
	case 's':  //Save
	  the_old = stdarch->get_filename().c_str();
	  the_new = " ";
	  if(verbose) cout << "Save: present name is " << the_old <<endl;
	  cout << "Save: the center freq is " << stdarch->get_centre_frequency() << "MHz, "<< (30/(1e-3* stdarch->get_centre_frequency()))<< "cm" <<endl;
	  cout << "      new file name: ";
	  cin >> the_new;
	  stdarch->rotate(convt(stdarch, -0.5*stdarch->get_nbin(), verbose));
	  stdarch->unload(the_new);
	  stdarch->rotate(convt(stdarch, 0.5*stdarch->get_nbin(), verbose));
	  cout << "Save: new standard profile " << the_new << " written to disk" << endl;
	  break;	 
	  
        case 'i':  // Interpolate
          opts = ' ';
	  cpgband(2, 0, curs_x, curs_y, &x, &y, &opts);
	  if (opts == 'i') {
	    float * tmpdata=stdarch->get_Profile(0, 0, 0)->get_amps();
	    int istart, iend;
	    if (curs_x > x) {float tmp=x; x=curs_x; curs_x=tmp;}
	    istart = int(curs_x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if (istart<0) istart=0;
	    fprintf(stderr, "istart = %d\n", istart);
	    iend = int(x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if(unsigned(iend)>stdarch->get_Profile(0, 0, 0)->get_nbin()) 
	      iend=stdarch->get_Profile(0, 0, 0)->get_nbin();
	    if(verbose) cout << "Interpolate: start & end bin number: " <<istart << ", " <<iend<<endl;

	    float firstVal,lastVal;
	    firstVal = tmpdata[istart];
	    lastVal = tmpdata[iend-1];
	  
	    for(i=unsigned(istart); i<unsigned(iend); i++) 
	      tmpdata[i]=firstVal+(lastVal-firstVal)/(float)(iend-1-istart)*(i-istart);
	    
	    if (vverbose) 
	      for(i=0; i<stdarch->get_Profile(0, 0, 0)->get_nbin(); i++) 
		cout << "  " << i << ": " << tmpdata[i]<<"  ";
	    
	    stdarch->get_Profile(0, 0, 0)->set_amps(tmpdata);
	    if (verbose) cout << "Interpolate: done" << endl;
	  }
	  break;

	  case '0':   //Zero base line
	    cout << "Zero baseline: start point set at:" << curs_x << endl;
	  cout << "Zero baseline: move cursor and set the range" <<endl;
	  opts = ' ';
	  cpgband(2, 0, curs_x, curs_y, &x, &y, &opts);
	  if (opts == '0') {
	    cout << "Zero baseline: end point set at:" << x << endl;
	    if(verbose) cout << "Zero baseline: zero the profile base line" << endl;
	    float * tmpdata=stdarch->get_Profile(0, 0, 0)->get_amps();
	    int istart, iend;
	    if (curs_x > x) {float tmp=x; x=curs_x; curs_x=tmp;}
	    istart = int(curs_x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if (istart<0) istart=0;
	    fprintf(stderr, "istart = %d\n", istart);
	    iend = int(x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if(unsigned(iend)>stdarch->get_Profile(0, 0, 0)->get_nbin()) 
	      iend=stdarch->get_Profile(0, 0, 0)->get_nbin();
	    if(verbose) cout << "Zero baseline: start & end bin number: " <<istart << ", " <<iend<<endl;
	    if(zflag==false) {
	      meantmp=0;
	      for(i=unsigned(istart); i<unsigned(iend); i++) {
		meantmp+=tmpdata[i];
	      }
	      meantmp/=(iend-istart); 
	      zflag=true;
	    }
	    
	    if (verbose) cout << "Zero baseline: baseline value = " << meantmp << endl;
	    for(i=unsigned(istart); i<unsigned(iend); i++) 
	      tmpdata[i]=meantmp;

	    if (vverbose) 
	      for(i=0; i<stdarch->get_Profile(0, 0, 0)->get_nbin(); i++) 
		cout << "  " << i << ": " << tmpdata[i]<<"  ";
	      
	    stdarch->get_Profile(0, 0, 0)->set_amps(tmpdata);
	    if (verbose) cout << "Zero baseline: done" << endl;
	  }
	  else cout << "Zero baseline: range not set" << endl;
	  break;

	case 'z':   //Zero base line
	  curs_x0=curs_x;
	  if(curs_x0<0.0)    curs_x0=0.0;
	  if(curs_x0>1.0)    curs_x0=1.0;
	  xmin=curs_x0-0.9*(curs_x0-xmin);
	  if(xmin<0.0)   xmin=0.0;
	  cout << "curs_x=" <<curs_x0 << " xmin=" <<xmin <<endl;
	  xmax=curs_x0+0.9*(xmax-curs_x0);
	  if(xmax>1.0)   xmax=1.0;
	  cout << "curs_x=" <<curs_x0 << " xmax=" <<xmax <<endl;
	  break;
	  
	case 'u':
	  xmin=0.0;
	  xmax=1.0;
	  break;

	default:
	  cout << "Unrecognised option." << endl;
	  break;
	}

	*stdcorr=*stdarch;
	if(refflag==true) {
	  *refcorr=*refarch;
	  cross(refcorr, stdcorr, verbose, verbose, line);
	}
	plot_it(refarch, stdarch, ci_ref, ci_std, ci_tex, ci_dis, line, plotdev, refflag, xmin, xmax);
	cout << "Waiting for option ...." << endl;
	cpgcurs(&curs_x, &curs_y, &opts);
      }
      catch (Error& error) {
	cerr << error << endl;
      } 
      catch (...) {
	cerr << "unknown exception thrown." << endl;
      }
    }
  }
  else fprintf(stderr, "Sorry, plot device has no cursor.");
  cpgend();
  return 0;
}


void plot_it(Reference::To<Pulsar::Archive> refarch, Reference::To<Pulsar::Archive> stdarch, int ci_ref, int ci_std, int ci_tex, int ci_dis, char line[100], char plotdev[5], bool refflag, float xmin, float xmax) {
  float x, y;
  char str[50];
  unsigned i;
  //options
  cpgbeg(0, plotdev, 1, 1);
  
  cpgsvp (0.05, .95, 0.70, .98);
  cpgswin (0, 100, 0, 100);
  cpgsch (0.8);

  int step=10;
  for(i=0; i<10; i++) {
    cpgsci (ci_tex);
    x=5;
    y=100-i*step;
    if(i==0){
      if(refflag==false)
	cpgsci (ci_dis);
      cpgtext (x, y, "a:  align with reference profile");
    }
    else if(i==1)
      cpgtext (x, y, "b:  bscrunch by factor 2");
    else if(i==2)
      cpgtext (x, y, "c:  center the profile");
    else if(i==3)
      cpgtext (x, y, "m:  smooth the profile");
    else if(i==4)
      cpgtext (x, y, "o:  back to origin");
    else if(i==5)
      cpgtext (x, y, "q:  quit the program");
    else if(i==6)
      cpgtext (x, y, "0:  set the range and zero the base line"); 
    else if(i==7)
      cpgtext (x, y, "i:  set the range and interpolate"); 
    else if(i==8)
      cpgtext (x, y, "z:  zoom in"); 
    else    break;
  }
  if(refflag==true) {
    cpgsci (ci_ref);
    cpgmove (x+3, 22);
    cpgdraw (x+10, 22);
    strncpy(str, "", 50);
    sprintf(str, "Reference profile    %d bins", refarch->get_Profile(0, 0, 0)->get_nbin());
    cpgtext (x+13, 22, str);
  }

  cpgsci(ci_tex);
  for(i=0; i<10; i++) {
    x=60;
    y=100-i*step;
    if(i==0)
      cpgtext (x, y, "L:  fast rotate 20 bins to the Left");
    else if(i==1)
      cpgtext (x, y, "R:  fast rotate 20 bins to the Right");
    else if(i==2)
      cpgtext (x, y, "l:  left rotate profile by 1 bin");
    else if(i==3)
      cpgtext (x, y, "r:  right rotate profile by 1 bin  ");
    else if(i==4)
      cpgtext (x, y, "f:  fine rotate 0.05 bin to the left");
    else if(i==5)
      cpgtext (x, y, "g:  fine rotate 0.05 bin to the right");
    else if(i==6)
      cpgtext (x, y, "s:  save the standard profile");
    else if(i==7)
      cpgtext (x, y, "u:  unzoom");
    else  break;
  }
  cpgsci (ci_std);
  cpgmove (x+3, 22);
  cpgdraw (x+10, 22);
  strncpy(str, "", 50);
  sprintf(str, "New profile    %d bins", stdarch->get_Profile(0, 0, 0)->get_nbin());
  cpgtext (x+13, 22, str);

  //write the cross correlation message
  if(refflag == true) {
    cpgsci (ci_tex);
    cpgtext (10, 10, line);
  }

  //bottom scale
  cpgsvp (0.1, 0.9, 0.03, 0.05);
  cpgswin (xmin, xmax, 0, 1);
  cpgsci(ci_tex);

  if(xmax-xmin>0.2)
    cpgbox("BNTS", 0.1, 5, "", 0.0, 0);
  else
    cpgbox("BNTS", 0.05, 5, "", 0.0, 0);

  //profile window
  cpgsvp (0.1, 0.9, 0.05, 0.68);
  cpgswin (0, 1, 0, 1);
  
  //draw the centre line
  cpgsci (ci_tex); //grey
  cpgsls (2);
  cpgmove (0.5, 0.0);
  cpgdraw (0.5, 1.0);
  cpgtext(0.48, 1.02, "Ref.");

  //draw the profiles
  cpgsls (1);

  Pulsar::ProfilePlot plotter;
  plotter.get_frame()->get_x_scale()->set_range_norm (xmin, xmax);
  plotter.get_frame()->set_viewport (0.1, 0.9, 0.05, 0.68);

  if(refflag == true) {
    cpgsci (ci_ref);
    plotter.plot_profile( refarch->get_Profile(0, 0, 0) );
  }
  cpgsci (ci_std);
  plotter.plot_profile( stdarch->get_Profile(0, 0, 0) );
}


void coef (float *x, float *y, int maxdelay, int n, double *rmax, int *imax, double *pcoef, bool verbose) 
{
  float mx, my, sx, sy, denom, r, sxy;
  int i, delay, j;
  
  *rmax=-1e8;
  mx = 0;
  my = 0;   
  for (i=0;i<n;i++) {
    mx += x[i];
    my += y[i];
  }
  mx /= n;
  my /= n;
  
  /* Calculate the denominator */
  sx = 0;
  sy = 0;
  for (i=0;i<n;i++) {
    sx += (x[i] - mx) * (x[i] - mx);
    sy += (y[i] - my) * (y[i] - my);
  }
  denom = sqrt(sx*sy);
  
  /* Calculate the correlation coefficient on lag 'delay' */
  for (delay=-maxdelay;delay<maxdelay;delay++) {
    sxy = 0;
    for (i=0;i<n;i++) {
      j = i + delay;
      while (j < 0)
	j += n;
      j %= n;
      sxy += (x[i] - mx) * (y[j] - my);
    }
    r = sxy / denom;      /* r is the correlation coefficient at "delay" */
    if(r>*rmax) {
      *rmax=r;
      *imax=delay;
    }
    if(delay==0) {
      *pcoef=r;
    }
  }
}


void cross(Reference::To<Pulsar::Archive> refcorr, Reference::To<Pulsar::Archive> stdcorr, bool verbose, bool vverbose, char line[100]) 
{
  float fmax, ephase, stdphase;
  int imax; 
  double rmax, pcoef, ppcoef;
  Reference::To<Pulsar::Archive> stdclone (stdcorr->clone());
  Reference::To<Pulsar::Profile> stdprof;

  //present, clone
  cross_correlation(refcorr->get_Profile(0, 0, 0), stdclone->get_Profile(0, 0, 0), &rmax, &imax, &pcoef, vverbose);

  //  maximum, after rotate fractional phase bin
  stdprof=stdcorr->get_Profile(0, 0, 0);
  Estimate<double> Ephase = stdprof->shift(refcorr->get_Profile(0, 0, 0));
  stdphase=Ephase.val;
  ephase=sqrt(Ephase.var);
  fmax=float(stdphase)*stdcorr->get_Profile(0,0,0)->get_nbin();
  stdcorr->rotate(convt(stdcorr, fmax, verbose));
  cross_correlation(refcorr->get_Profile(0,0,0), stdcorr->get_Profile(0,0,0), 
		    &rmax, &imax, &ppcoef, vverbose);

  if(verbose) cout << "Maximum cross correlation coeffecient: " 
		   << rmax << " at bin lag " << fmax <<endl;
  if(verbose) cout << "Present cross correlation coeffecient: " 
		   << pcoef <<endl;
  strncmp(line, "", 100);
  sprintf(line, "Maximum cross correlation coeffecient = %f at bin lag %f   Present = %f", ppcoef, fmax, pcoef);
}


void cross_correlation(Reference::To<Pulsar::Profile> refprof, Reference::To<Pulsar::Profile> stdprof, double *rmax, int *imax, double *pcoef, bool vverbose)
{
  float fact_nbin;
  int corr_nbin=0;
  
  if((fact_nbin = 1.0*stdprof->get_nbin()/refprof->get_nbin())>1) 
    stdprof->bscrunch(int(fact_nbin));
  else if((fact_nbin = 1.0*stdprof->get_nbin()/refprof->get_nbin())<1)
    refprof->bscrunch(int(1/fact_nbin));

  corr_nbin=stdprof->get_nbin();
  if(vverbose) cout << "  cross correlation: " <<corr_nbin << " bins applied"<< endl;
  if(vverbose) cout << "  bin fact stdprof/refprof: " <<fact_nbin <<endl;

  //call coef
  float *ref = refprof->get_amps();
  float *std = stdprof->get_amps();
  coef(ref, std, corr_nbin/2, corr_nbin, rmax, imax, pcoef, vverbose);
  if (fact_nbin>=1) *imax = int(*imax*fact_nbin);
}


void smooth(Reference::To<Pulsar::Profile> prof, bool vverbose)
{
  float *temp=prof->get_amps();
  float *smth=prof->get_amps();
  int i, k1, k2;
  for(i=0; i<int(prof->get_nbin()); i++){
    k1=i-1;
    k2=i+1;
    if(k1<0)                   k1=prof->get_nbin()+k1;
    if(k2>=int(prof->get_nbin()))   k2=prof->get_nbin()-k2;
    smth[i]=0.25*(temp[k1]+temp[k2])+0.5*temp[i];
    //    if (vverbose)    fprintf(stderr, "  %d:%f %f  \n",i, temp[i], smth[i]);
    if(vverbose)  cout << "  smooth: "  << i << " " << temp[i]  << " " << smth[i] << endl;
  }

  prof->set_amps(smth);
}


double convt(Reference::To<Pulsar::Archive> arch, float nbin, bool verbose)
{
  double phase_in_time;
  phase_in_time=nbin*arch->get_Integration(0)->get_folding_period()/arch->get_nbin();
  if (verbose)  cout << "Convert: phase in time " << phase_in_time <<" (s)" << endl;
  return phase_in_time;
}

void auto_align_files (std::vector<std::string>& filename)
{
  if (filename.size() < 2)
  {
    cerr << "pas: auto align requires more than one filename" << endl;
    return;
  }

  Reference::To<Pulsar::Profile> standard;

  for (unsigned i=0; i < filename.size(); i++)
  {
    cerr << "pas: auto align loading " << filename[i] << endl;

    Reference::To<Pulsar::Archive> arch = Pulsar::Archive::load(filename[i]);
    Reference::To<Pulsar::Archive> clone = arch->clone();

    if (auto_align_invariant)
      clone->invint();
    else
      clone->pscrunch();

    clone->fscrunch();
    clone->tscrunch();
    
    if (i == 0)
    {
      standard = clone->get_Profile (0,0,0);
      continue;
    }
    
    Estimate<double> shift = clone->get_Profile(0,0,0)->shift (*standard);
      
    cerr << "pas: auto align shift = " << shift << endl;
    
    arch->rotate_phase (shift.get_value());
    
    cerr << "pas: auto align unloading " << filename[i] << endl;
    arch->unload ();
  }
}
