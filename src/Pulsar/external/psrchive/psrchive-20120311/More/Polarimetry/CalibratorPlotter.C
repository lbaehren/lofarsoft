/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/Calibrator.h"
#include "Pulsar/Archive.h"

#include "EstimatePlotter.h"

#include <cpgplot.h>

// #define _DEBUG 1

using namespace std;

bool Pulsar::CalibratorPlotter::verbose = false;

Pulsar::CalibratorPlotter::CalibratorPlotter ()
{
  npanel = 3;
  between_panels = 0.1;
  use_colour = true;
  print_titles = true;
  display_mean_single_line = false;
}

Pulsar::CalibratorPlotter::~CalibratorPlotter ()
{
}

//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::CalibratorPlotter::plot (const Calibrator* calibrator) try
{
  if (!calibrator)
    return;

  if (verbose)
    cerr << "Pulsar::CalibratorPlotter::plot" << endl;

  const bool display_mean_single_line = get_display_mean_single_line();

  if (display_mean_single_line) {
    const string filename = calibrator->get_Archive()->get_filename();
    const MJD epoch = calibrator->get_epoch();
    const double freq = calibrator->get_Archive()->get_centre_frequency();

    cout <<
      filename << " " <<
      tostring<MJD>(epoch, 3, ios::fixed) << " " <<
      tostring<double>(freq, 1, ios::fixed) << " ";
  }

  plot( calibrator->get_Info (), calibrator->get_nchan (),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );

  // Formatting: if the means have been displayed on on line, 
  // add a blank line after the plot.
  if (display_mean_single_line) {
    cout << endl;
  }
}
catch (Error& error)
{
  throw error += "Pulsar::CalibratorPlotter::plot(Calibrator*)";
}


// TODO: Add MJD as an argument here
void Pulsar::CalibratorPlotter::plot (const Calibrator::Info* info,
				      unsigned nchan, double cfreq, double bw)
try
{

  if (!info) {
    cerr << "Pulsar::CalibratorPlotter::plot no Calibrator::Info" << endl;
    return;
  }

  Reference::To<const Calibrator::Info> manage = info;


  if (nchan == 0)
  {
    cerr << "Pulsar::CalibratorPlotter::plot no points to plot" << endl;
    return;
  }

#ifdef _DEBUG
  cerr << "Pulsar::CalibratorPlotter::plot nchan=" << nchan << endl;
#endif

  unsigned nplot = info->get_nclass();

#ifndef _DEBUG
  if (verbose)
#endif
    cerr << "Pulsar::CalibratorPlotter::plot nchan=" << nchan 
	 << " nplot=" << nplot << " cfreq=" << cfreq << " bw=" << bw << endl;

  float xmin, xmax, ymin, ymax;
  cpgqvp (0, &xmin, &xmax, &ymin, &ymax);

  float ybottom = ymin;
  float yrange = ymax - ymin;
  float yspace = between_panels * yrange;
  float yheight = (yrange - yspace) / float(npanel);
  float ybetween = 0;
  if (npanel > 1)
    ybetween = yspace / float(npanel -1);

  cpgsci(1);
  cpgsch(1);

  // the plotting class
  EstimatePlotter plotter;

  const bool display_mean_single_line = get_display_mean_single_line();

  // If the mean is to be displayed on one line, prevent EstimatePlotter
  // from displaying it again (in its multi-line format).
  if (display_mean_single_line) {
    plotter.report_mean = false;
    plotter.report_mean_on_single_line = true;
  }

  //plotter.report_mean = display_mean_single_line

  plotter.set_xrange (cfreq-0.5*bw, cfreq+0.5*bw);

  // don't plot points with zero variance
  plotter.set_minimum_error (0.0);

  plotter.set_border (0.03, 0.045);

  // the data to be plotted
  vector< Estimate<float> > data (nchan);

  // ////////////////////////////////////////////////////////////////////

  string xaxis;

  unsigned ipanel = 0;

  for (unsigned iplot=0; iplot < info->get_nclass(); iplot++)
  {
#ifdef _DEBUG
    cerr << "Pulsar::CalibratorPlotter::plot " 
	 << iplot << "/" << info->get_nclass() << endl;
#endif

    if (ipanel % npanel == 0)
    {
      xaxis = "bcnst";

      if (ipanel)
      {
	if (print_titles)
	  plot_labels (info);
	cpgpage ();
      }

      ybottom = ymin;

      unsigned to_plot = info->get_nclass() - iplot;

      if (to_plot < npanel)
	ybottom += 0.5 * (ybetween + yheight) * float(npanel - to_plot);
    }
    else
      xaxis = "bcst";

    plotter.clear ();

    unsigned nparam = info->get_nparam( iplot );

#ifndef _DEBUG
    if (verbose)
#endif
      cerr << "Pulsar::CalibratorPlotter::plot iplot=" << iplot
	   << " nparam=" << nparam << endl;

    unsigned iparam = 0;

    for (iparam=0; iparam<nparam; iparam++)
    {
#ifndef _DEBUG
      if (verbose)
#endif
	cerr << "Pulsar::CalibratorPlotter::plot iplot=" << iplot
	     << " iparam=" << iparam << endl;

      for (unsigned ichan=0; ichan<nchan; ichan++)
	data[ichan] = info->get_param (ichan, iplot, iparam);


      plotter.add_plot (data);
    }

#ifndef _DEBUG
    if (verbose)
#endif
      cerr << "Pulsar::CalibratorPlotter::plot iplot=" << iplot
	   << " plots initialized" << endl;

    cpgsvp (xmin, xmax, ybottom, ybottom + yheight);

    unsigned plotted = 0;

    for (iparam=0; iparam<nparam; iparam++)
    {
      if (use_colour)
	cpgsci ( info->get_colour_index(iplot, iparam) );
      else
	plotter.set_graph_marker ( info->get_graph_marker(iplot, iparam) );

      plotted += plotter.plot (iparam);
    }

    if (plotted > 0)
    {
#ifndef _DEBUG
      if (verbose)
#endif
	cerr << "Pulsar::CalibratorPlotter::plot iplot=" << iplot
	     << " plot axis" << endl;

      cpgsci (1);

#ifdef _DEBUG
      cerr << "cpgbox (" << xaxis << ")" << endl;
#endif

      cpgbox(xaxis.c_str(),0,0,"bcvnst",0,2);

#ifdef _DEBUG
      cerr << "cpgmtxt (" << info->get_name(iplot) << ")" << endl;
#endif

      cpgmtxt("L",3.5,.5,.5, info->get_name(iplot).c_str());
      
      if (ipanel % npanel == 0)
	cpgmtxt("B",3.0,.5,.5, "Frequency (MHz)");

      ipanel ++;
      ybottom += ybetween + yheight;
    }

#ifdef _DEBUG
    cerr << "Pulsar::Calibrator::plot end " << iplot << "/" 
	 << info->get_nclass() << endl;
#endif
  }

  if (print_titles)
    plot_labels (info);

  // restore the viewport
  cpgsvp (xmin, xmax, ymin, ymax);

#ifndef _DEBUG
  if (verbose)
#endif
    cerr << "Pulsar::CalibratorPlotter::plot return" << endl;

}
catch (Error& error)
{
  throw error += "Pulsar::CalibratorPlotter::plot(Calibrator::Info*)";
}

void Pulsar::CalibratorPlotter::plot_labels (const Calibrator::Info* info) {
  float offset = 0.5;
  
  string plot_title = info->get_title ();
  
  if (!plot_title.empty())
  {
    cpgmtxt( "T", offset, .5,.5, plot_title.c_str());
    offset += 1.0;
  }

  if (!title.empty())
    cpgmtxt( "T", offset, .5,.5, title.c_str());
}

void Pulsar::CalibratorPlotter::set_display_mean_single_line(
    const bool _display_mean_single_line) {
  display_mean_single_line = _display_mean_single_line;
}

bool Pulsar::CalibratorPlotter::get_display_mean_single_line() const {
  return display_mean_single_line;
}
