//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/EstimatePlotter.h,v $
   $Revision: 1.20 $
   $Date: 2009/08/14 06:21:40 $
   $Author: straten $ */

#ifndef __EstimatePlotter_h
#define __EstimatePlotter_h

#include "Estimate.h"
#include "Error.h"

#include <vector>
#include <iostream>
#include <cmath>

class EstimatePlotter {

 public:

  //! Flag controls verbosity
  static bool report_mean;

  bool report_mean_on_single_line;

  //! Default constructor
  EstimatePlotter ();

  //! Clear all data
  void clear ();

  //! Set the viewport and/or world coordinates before plotting
  void set_control_viewport (bool flag);

  //! Set the border used when setting the world coordinates of the viewport
  void set_border (float fraction_x, float fraction_y);

  //! Set the range of x values
  void set_xrange (float x_min, float x_max);

  //! Set the minimum value of error to plot
  void set_minimum_error (float error);

  //! Set the maximum value of error to plot
  void set_maximum_error (float error);

  //! Set the PGPLOT standard graph marker
  void set_graph_marker (int symbol);

  //! Set flag to plot data in separate viewports
  void separate_viewports (bool scaled = true, bool vertical = true);

  //! Set the viewport to plot the specified member of the current data set
  void set_viewport (unsigned index);

  //! Set the world coordinates of the viewport with some buffer space
  void set_world (float x1, float x2, float y1, float y2);

  //! Get the minimum x value in plot
  float get_x_min () const { return x_min; }
  //! Get the maximum x value in plot
  float get_x_max () const { return x_max; }

  //! Get the minimum y value in plot
  float get_y_min () const { return y_min; }
  //! Get the maximum y value in plot
  float get_y_max () const { return y_max; }

  //! Set the viewport to that when separate_viewports was called
  void restore_viewport ();

  //! Add a vector of Estimates to the current data set
  template<class T> void add_plot (const std::vector< Estimate<T> >& data);

  template<class Xt, class Yt>
  void add_plot (const std::vector<Xt>&, const std::vector< Estimate<Yt> >&);

  template<class Xt, class Yt> 
  void add_plot (const std::vector<Xt>& xdata,
		 const std::vector< std::complex< Estimate<Yt> > >& ydata);

  //! Plot the specified data set
  template<class T> void plot (const std::vector< Estimate<T> >& data);

  //! Plot the specified member of the current data set
  unsigned plot (unsigned index);

  //! Plot the current data set in one window
  unsigned plot ();

  //! Set the range of indeces to be considered on next call to minmax
  void set_minmax_range (unsigned i_min, unsigned i_max);

  void minmax (bool& xrange_set, float& xmin, float& xmax,
	       bool& yrange_set, float& ymin, float& ymax,
	       const std::vector<float>& x,
	       const std::vector<float>& y,
	       const std::vector<float>& yerr);

 protected:

  //! Set the viewport and/or world coordinates before plotting
  bool control_viewport;

  //! Borders
  float x_border, y_border;

  //! add_plot X range
  float xrange_min, xrange_max;

  //! total X range
  float x_min, x_max;

  //! total Y range
  float y_min, y_max;

  //! range of indeces to be added on next call to add_plot
  unsigned i_min, i_max;

  //! range set
  bool xrange_set;
  bool yrange_set;

  //! The minimum error in plot
  float minimum_error;

  //! The maximum error in plot
  float maximum_error;

  //! PGPLOT Standard Graph Marker
  int graph_marker;

  std::vector< std::vector<float> > xval;
  std::vector< std::vector<float> > yval;
  std::vector< std::vector<float> > yerr;
  
 private:

  std::vector<float> data_xmin;
  std::vector<float> data_xmax;
  std::vector<float> data_ymin;
  std::vector<float> data_ymax;

  float vp_x1, vp_x2, vp_y1, vp_y2;

  bool viewports_set;
  bool viewports_vertical;
  bool viewports_scaled;

};

template<class T> 
void EstimatePlotter::plot (const std::vector< Estimate<T> >& data)
{
  clear ();
  add_plot (data);
  plot (0);
}

template<class T> 
void EstimatePlotter::add_plot (const std::vector< Estimate<T> >& data)
{
  unsigned ipt = 0, npt = data.size();
  if (npt == 0)
    return;
  
  xval.push_back ( std::vector<float>(npt) );
  yval.push_back ( std::vector<float>(npt) );
  yerr.push_back ( std::vector<float>(npt) );

  std::vector<float>& x = xval.back();
  std::vector<float>& y = yval.back();
  std::vector<float>& ye = yerr.back();

  double xscale = 0.0;
  if (npt > 1)
    xscale = double(xrange_max - xrange_min) / double(npt-1);

  MeanEstimate<T> mean;

  for (ipt=0; ipt<npt; ipt++)
  {
    x[ipt] = xrange_min + xscale * double(ipt);

    if (!std::isfinite( data[ipt].val ))
      ye[ipt] = 0;
    else 
    {
      y[ipt] = data[ipt].val;
      ye[ipt] = sqrt (data[ipt].var);

      if (data[ipt].var)
	mean += data[ipt];
    }

  }

  // useful at times, excessive at others ...
  if (report_mean)
  {
    std::cerr << "Mean = " << mean << std::endl;
  }
  else if (report_mean_on_single_line)
  {
    std::cout << mean.get_Estimate().get_value() << " ";
    std::cout << mean.get_Estimate().get_error() << " ";
  }

  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, y, ye);
}

template<class Xt, class Yt> 
void EstimatePlotter::add_plot (const std::vector<Xt>& xdata,
				const std::vector< Estimate<Yt> >& ydata)
{
  unsigned ipt = 0, npt = xdata.size();
  if (npt == 0)
    return;
  
  if (ydata.size() != npt)
    throw Error (InvalidParam, "EstimatePlotter::add_plot (Xt, Yt)",
		 "xdata.size=%d != ydata.size=%d", npt, ydata.size());

  xval.push_back ( std::vector<float>(npt) );
  yval.push_back ( std::vector<float>(npt) );
  yerr.push_back ( std::vector<float>(npt) );

  std::vector<float>& x = xval.back();
  std::vector<float>& y = yval.back();
  std::vector<float>& ye = yerr.back();

  for (ipt=0; ipt<npt; ipt++) {
    ye[ipt] = sqrt (ydata[ipt].var);
    x[ipt] = xdata[ipt];
    y[ipt] = ydata[ipt].val;
  }

  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, y, ye);
}

template<class Xt, class Yt> 
void EstimatePlotter::add_plot
(const std::vector<Xt>& xdata,
 const std::vector< std::complex< Estimate<Yt> > >& ydata)
{
  unsigned ipt = 0, npt = xdata.size();
  if (npt == 0)
    return;
  
  if (ydata.size() != npt)
    throw Error (InvalidParam, "EstimatePlotter::add_plot (Xt, Yt)",
		 "xdata.size=%d != ydata.size=%d", npt, ydata.size());

  xval.push_back ( std::vector<float>(npt) );
  std::vector<float>& x = xval.back();

  std::vector<float> re_y (npt);
  std::vector<float> re_ye (npt);

  std::vector<float> im_y (npt);
  std::vector<float> im_ye (npt);

  for (ipt=0; ipt<npt; ipt++)
  {
    x[ipt] = xdata[ipt];

    re_y[ipt] = ydata[ipt].real().val;
    re_ye[ipt] = sqrt (ydata[ipt].real().var);

    im_y[ipt] = ydata[ipt].imag().val;
    im_ye[ipt] = sqrt (ydata[ipt].imag().var);
  }

  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, re_y, re_ye);
  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, im_y, im_ye);

  yval.push_back ( re_y );
  yval.push_back ( im_y );

  yerr.push_back ( re_ye );
  yerr.push_back ( im_ye );
}

#endif
