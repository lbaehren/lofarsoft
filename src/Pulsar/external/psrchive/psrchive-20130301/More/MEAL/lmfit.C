/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#include "EstimatePlotter.h"
#endif

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

using namespace std;

void usage ()
{
  cerr <<
    "lmfit - fits algebraic scalar models to data\n"
    "\n"
    "USAGE: lmfit [options] -m model.file data.file \n"
    "\n"
    "  -d display    set the PGPLOT device name \n"
    "  -e efac       multiply all error estimates by efac \n"
    "  -t threshold  call it a fit when delta_chi/chi < threshold \n"
    "  -x label      label the x-axis \n"
    "  -y label      label teh y-axis \n"
    "  -v            verbose \n"
    "  -V            very verbose \n"
       << endl;
}

void plot_model (MEAL::Axis<double>& argument,
		 MEAL::Scalar* scalar,
		 unsigned npts, double xmin, double xmax);

int main (int argc, char** argv) try {

  float threshold = 0.001;
  bool verbose = false;

  string display = "?";
  char*  model_filename = NULL;

  char* xlab = NULL;
  char* ylab = NULL;

  float efac = 1.0;

  int c;
  while ((c = getopt(argc, argv, "d:e:hm:t:x:y:vV")) != -1)  {
    switch (c)  {
    case 'd':
      display = optarg;
      break;
    case 'e':
      efac = atof (optarg);
      cerr << "efac:" << efac << endl;
      break;
    case 'h':
      usage();
      return -1;
    case 'm':
      model_filename = optarg;
      break;
    case 't':
      threshold = atof (optarg);
      break;
    case 'x':
      xlab = optarg;
      break;
    case 'y':
      ylab = optarg;
      break;

    case 'V':
      MEAL::Function::verbose = true;
    case 'v':
      verbose = true;
      break;
      
      break;
    }
  }

  if (!model_filename) {
    cerr << "lmfit: requires an input model (use -m)" << endl;
    return -1;
  }

  if (verbose)
    cerr << "lmfit: loading model from '" << model_filename << "'" << endl;

  MEAL::Scalar* scalar = MEAL::Function::load<MEAL::Scalar> (model_filename);

  string model_text;

  scalar->print (model_text);
  cout << "INPUT:\n" << model_text << endl;

  MEAL::Axis<double> argument; 
  scalar->set_argument (0, &argument);

  if (optind >= argc) {
    cerr << "lmfit: requires a data file as the last argument" << endl;
    return -1;
  }
    
  if (verbose)
    cerr << "lmfit: loading data from '" << argv[optind] << "'" << endl;

  FILE* fptr = fopen (argv[optind], "r");
  if (!fptr) {
    cerr << "lmfit: could not open '" << argv[optind] << "'" << endl;
    perror ("");
    return -1;
  }

  std::vector< MEAL::Axis<double>::Value > data_x;  // x-ordinate of data
  std::vector< Estimate<double> > data_y;       // y-ordinate of data with error

  double x, y, e;
  while (fscanf (fptr, "%lf %lf %lf", &x, &y, &e) == 3) {

    data_x.push_back ( argument.get_Value(x) );
    e *= efac;
    data_y.push_back ( Estimate<double>(y,e*e) );

  }

  unsigned npts = 500;

  double xmin = *min_element(data_x.begin(), data_x.end());
  double xmax = *max_element(data_x.begin(), data_x.end());

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

#ifdef HAVE_PGPLOT

  //
  // Plot the data
  //

  EstimatePlotter plot;
  plot.add_plot (data_x, data_y);

  cpgopen (display.c_str());
  cpgsch (1.5);

  plot.plot();

  cpgbox ("bcinst", 0,0, "bcinst", 0,0);


  //
  // Plot the input model
  //

  cpgsci(2);
  plot_model (argument, scalar, npts, xmin, xmax);

#endif

  MEAL::LevenbergMarquardt<double> fit;
  fit.verbose = MEAL::Function::verbose;
  
  float chisq = fit.init (data_x, data_y, *scalar);
  cerr << "initial chisq = " << chisq << endl;
  
  unsigned iter = 1;
  unsigned not_improving = 0;
  while (not_improving < 25) {
    cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (data_x, data_y, *scalar);
    cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < threshold && diffchisq > 0) {
	cerr << "no big diff in chisq = " << diffchisq << endl;
	break;
      }
    }
    else
      not_improving ++;

    iter ++;
  }

  double free_parms = data_x.size() + scalar->get_nparam();

  cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
       << chisq / free_parms << endl;

  std::vector<std::vector<double> > covariance;
  fit.result (*scalar, covariance);

  for (unsigned iparm=0; iparm < scalar->get_nparam(); iparm++) {
    // Bi Qing has uncovered an error in our estimation of parameter error
    // most likely due to an error in Numerical Recipes
    scalar->set_variance (iparm, 2.0*covariance[iparm][iparm]);
  }

#ifdef HAVE_PGPLOT
  cpgsci(3);
  plot_model (argument, scalar, npts, xmin, xmax);
  cpgend();
#endif

  scalar->print (model_text);
  cout << "ANSWER:\n" << model_text << endl;

  return 0;
}
catch (const Error& error) {
  cerr << error << endl;
  return -1;
}
catch (...) {
  cerr << "Unhandled exception" << endl;
  return -1;
}

#ifdef HAVE_PGPLOT

void plot_model (MEAL::Axis<double>& argument,
		 MEAL::Scalar* scalar,
		 unsigned npts, double xmin, double xmax)
{
  double xdel = (xmax-xmin)/(npts-1);

  for (unsigned i=0; i<npts; i++) {
 
    double x = xmin + xdel * double(i);

    argument.set_value (x);

    double y = scalar->evaluate ();

    if (i==0)
      cpgmove (x,y);
    else
      cpgdraw (x,y);
  }

}

#endif
