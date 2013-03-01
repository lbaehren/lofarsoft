/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/GaussianShift.h"
#include "Pulsar/Profile.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

using namespace std;

void wrap (int& binval, int nbin) {
  if (binval < 0)
    binval += nbin;
  else if (binval > nbin-1)
    binval -= nbin;
}

/*! The WINDOW size is used to set the region around the point of
 maximum correlation that will be used in the interpolation.  The
 find_peak_edges routine seems to get it wrong when the S/N is low,
 this simple method is more robust.

 Experimentation with test_gtd.C suggests that the following value
 works best in most situations:
*/
#define WINDOW 0.0125

Estimate<double> Pulsar::GaussianShift::get_shift () const
{
  // This algorithm interpolates the time domain cross correlation
  // function by fitting a Gaussian.

  bool verbose = false;

  // First compute the standard cross correlation function:

  Reference::To<Pulsar::Profile> ptr = observation->clone();
  Reference::To<Pulsar::Profile> stp = standard->clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate (standard);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  // Ensure the amps aren't tiny lest the fitter be unhappy
  while (ptr->max() < 1) {
    *ptr *= 100;
  }
  
  // Find the peak (can be done a number of ways, this is the simplest)
  
  int brise = 0;
  int bfall = 0;
  
  int binmax = ptr->find_max_bin();
  int offset = 0;

  brise = binmax - int(WINDOW*float(ptr->get_nbin()));
  bfall = binmax + int(WINDOW*float(ptr->get_nbin()));

  if (brise < bfall) {
    offset = int((ptr->get_nbin())/2.0) - binmax;
  }
  else {
    offset = binmax - int((ptr->get_nbin())/2.0);
  }
  
  brise  += offset;
  bfall  += offset;
  binmax += offset;
    
  wrap(brise,  ptr->get_nbin());
  wrap(bfall,  ptr->get_nbin());
  wrap(binmax, ptr->get_nbin());

  try{
    
    float threshold = 0.001;
    
    MEAL::Gaussian gm;
    
    gm.set_centre(binmax);
    gm.set_width(abs(bfall - brise));
    gm.set_height(ptr->max());
    
    MEAL::Axis<double> argument;
    gm.set_argument (0, &argument);
    
    vector< MEAL::Axis<double>::Value > data_x;  // x-ordinate of data
    vector< Estimate<double> > data_y;       // y-ordinate of data with error
    
    int index = 0;

    for (int i = brise; i < bfall; i++) {
      data_x.push_back ( argument.get_Value(double(i)) );
      index = i;
      index -= offset;
      wrap(index, ptr->get_nbin());

      float phs1 = ptr->find_min_phase();

      double mean    = 0.0;
      double var     = 0.0;
      double varmean = 0.0;
     
      ptr->stats(phs1,&mean,&var,&varmean); 

      data_y.push_back( Estimate<double>(ptr->get_amps()[index], 
					 sqrt(var)) );
    }
    
    MEAL::LevenbergMarquardt<double> fit;
    fit.verbose = MEAL::Function::verbose;
    
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

    if (verbose)
      cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
	   << chisq / free_parms << endl;
    
    double shift = (gm.get_centre() - double(offset)) / 
      double(ptr->get_nbin());

    // Wrap the result so it is always withing +/- 0.5 phase units

    if (shift < -0.5)
      shift += 1.0;
    else if (shift > 0.5)
      shift -= 1.0;

    // Compute the error based on how far you have to move the
    // centre of the Gaussian before the chi-sq doubles

    float aim = chisq * 2.0;
    float cst = chisq;

    double raw = gm.get_centre();
    double itr = 0.0;

    while (cst < aim) {
      gm.set_centre(raw + itr);
      cst = fit.init(data_x, data_y, gm);
      itr += 0.01;
    }

    double ephase = (itr * 2.0) / double(ptr->get_nbin());

    return Estimate<double>(shift,ephase*ephase);
  }
  catch (Error& error) {
    cerr << error << endl;
    return Estimate<double>(0.0,0.25);
  }
}
