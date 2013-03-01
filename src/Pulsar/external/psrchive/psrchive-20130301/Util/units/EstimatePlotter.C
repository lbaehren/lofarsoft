/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "EstimatePlotter.h"
#include "Error.h"

#include <cpgplot.h>
#include <assert.h>

using namespace std;

bool EstimatePlotter::report_mean = false;

EstimatePlotter::EstimatePlotter ()
{
  x_border = 0.02;
  y_border = 0.08;
  xrange_min = x_min = y_min = 0.0;
  xrange_max = x_max = y_max = 1.0;

  i_min = i_max = 0;

  minimum_error = maximum_error = -1.0;

  graph_marker = -1;

  xrange_set = yrange_set = false;
  viewports_set = false;
  control_viewport = true;

  report_mean_on_single_line = false;
}

//! Set the border used when setting the world coordinates of the viewport
void EstimatePlotter::set_border (float fraction_x, float fraction_y)
{
  x_border = fraction_x;
  y_border = fraction_y;
}

void EstimatePlotter::set_control_viewport (bool flag)
{
  control_viewport = flag;
}

void EstimatePlotter::set_xrange (float xmin, float xmax)
{
  xrange_min = xmin;
  xrange_max = xmax;
}

//! Set the minimum value of error to plot
void EstimatePlotter::set_minimum_error (float error)
{
  minimum_error = error;
}

//! Set the maximum value of error to plot
void EstimatePlotter::set_maximum_error (float error)
{
  maximum_error = error;
}

/*! See <a href="http://www.astro.caltech.edu/~tjp/pgplot/chapter4.html>
Section 4.4</a> of the PGPLOT  Graphics Subroutine Library User's Manual. */
void EstimatePlotter::set_graph_marker (int symbol)
{
  graph_marker = symbol;
}

//! Set flag to plot data in separate viewports
void EstimatePlotter::separate_viewports (bool scaled, bool vertical)
{
  data_xmin.resize (xval.size());
  data_xmax.resize (xval.size());
  data_ymin.resize (xval.size());
  data_ymax.resize (xval.size());

  bool xrange = false;
  bool yrange = false;

  unsigned index;

  // complex data, for example, have ndim = 2
  unsigned ndim = yval.size() / xval.size();

  for (index=0; index<xval.size(); index++)
  {
    float& xmin = data_xmin[index];
    float& xmax = data_xmax[index];
    float& ymin = data_ymin[index];
    float& ymax = data_ymax[index];

    if (scaled)
    {
      if (vertical)
      {
	xmin = x_min;
	xmax = y_max;
      }
      else
      {
	ymin = y_min;
	ymax = y_max;
      }
    }

    for (unsigned idim=0; idim < ndim; idim++)
    {
      unsigned yndex = index*ndim+idim;
      minmax (xrange, xmin, xmax, yrange, ymin, ymax,
	      xval[index], yval[yndex], yerr[yndex]);
    }

    // cerr << "index=" << index << " xmin=" << xmin << " xmax=" << xmax
	 // << " ymin=" << ymin << " ymax=" << ymax << endl;

    if (scaled)
    {
      if (vertical)
	yrange = false;
      else
	xrange = false;
    }
  }

  if (scaled)
  {
    if (vertical)
      for (index=0; index<xval.size(); index++)
      {
	data_xmin[index] = x_min;
	data_xmax[index] = x_max;
      }
    else
      for (index=0; index<xval.size(); index++)
      {
	data_ymin[index] = y_min;
	data_ymax[index] = y_max;
      }
  }

  // query the current size of the viewport in normalized device coordinates
  cpgqvp (0, &vp_x1, &vp_x2, &vp_y1, &vp_y2);
 
  // cerr << "viewport: vp_x1=" << vp_x1 << " vp_x2=" << vp_x2
       // << " vp_y1=" << vp_y1 << " vp_y2=" << vp_y2 << endl;

  viewports_set = true;
  viewports_vertical = vertical;
  viewports_scaled = scaled;
}

//! Set the viewport to plot the specified member of the current data set
void EstimatePlotter::set_viewport (unsigned index)
{
  if (!viewports_set)
    throw Error (InvalidState, "EstimatePlotter::set_viewport",
		 "must first call EstimatePlotter::separate_viewports");

  if (!viewports_scaled)
    return;

  double start_size = 0.0;
  double end_size = 0.0;
  double total_size = 0.0;

  double max_size = 0.0;

  for (unsigned iplot=0; iplot < xval.size(); iplot++)
  {
    double size = 0.0;
    
    if (viewports_vertical)
      size = data_ymax[iplot] - data_ymin[iplot];
    else
      size = data_xmax[iplot] - data_xmin[iplot];

    if (size == 0.0)
      size = 1.0;

    if (iplot <= index)
    {
      start_size = end_size;
      end_size += size;
    }

    total_size += size;
    
    if (size > max_size)
      max_size = size;
  }

  double border = 0.0;

  if (viewports_vertical)
    border = y_border;
  else
    border = x_border;
  
  double buffer = border * max_size;
  total_size += 2.0 * buffer * xval.size();
  start_size += 2.0 * buffer * index;
  end_size   += 2.0 * buffer * (index+1);
  
  if (viewports_vertical)
  {
    double scale = (vp_y2 - vp_y1) / total_size;
    cpgsvp (vp_x1, vp_x2, vp_y1+scale*start_size, vp_y1+scale*end_size);
    float xbuf = x_border * (data_xmax[index]-data_xmin[index]);
    if (xbuf == 0)
      xbuf = data_xmin[index] * 0.25;
    if (xbuf == 0)
      xbuf = 1.0;

    // cerr << "xswin " << data_xmin[index]-xbuf << " " << data_xmax[index]+xbuf << " " << data_ymin[index]-buffer << " " << data_ymax[index]+buffer << endl;
    
    cpgswin (data_xmin[index]-xbuf, data_xmax[index]+xbuf, 
	     data_ymin[index]-buffer, data_ymax[index]+buffer);
  }
  else
  {
    double scale = (vp_x2 - vp_x1) / total_size;
    cpgsvp (vp_x1+scale*start_size, vp_x1+scale*end_size, vp_y1, vp_y2);
    float ybuf = y_border * (data_ymax[index]-data_ymin[index]);
    if (ybuf == 0)
      ybuf = data_ymin[index] * 0.25;
    
    // cerr << "xswin " << data_xmin[index]-buffer << " " << data_xmax[index]+buffer << " " << data_ymin[index]-ybuf << " " << data_ymax[index]+ybuf << endl;
    
    cpgswin (data_xmin[index]-buffer, data_xmax[index]+buffer, 
	     data_ymin[index]-ybuf, data_ymax[index]+ybuf);
  }
}

//! Set the viewport to plot the specified member of the current data set
void EstimatePlotter::restore_viewport ()
{
  if (!viewports_set)
    throw Error (InvalidState, "EstimatePlotter::restore_viewport",
		 "must first call EstimatePlotter::separate_viewports");

  cpgsvp (vp_x1, vp_x2, vp_y1, vp_y2);

}


void EstimatePlotter::set_world (float x1, float x2, float y1, float y2)
{
  float xbuf = x_border * (x2-x1);
  if (xbuf == 0.0)
    xbuf = 0.25 * x1;
  if (xbuf == 0.0)
    xbuf = 1.0;

  float ybuf = y_border * (y2-y1);
  if (ybuf == 0.0)
    ybuf = 0.25 * y1;
  if (ybuf == 0.0)
    ybuf = 1.0;

  // cerr << "x1=" << x1 << " x2=" << x2 << endl;
  // cerr << "swin " << x1-xbuf << " " <<  x2+xbuf << " " << y1-ybuf << " " << y2+ybuf << endl;

  cpgswin (x1-xbuf, x2+xbuf, y1-ybuf, y2+ybuf);
}

unsigned EstimatePlotter::plot ()
{
  unsigned plotted = 0;

  for (unsigned index=0; index<xval.size(); index++)
    plotted += plot (index);

  return plotted;
}

unsigned EstimatePlotter::plot (unsigned index)
{
  if (index >= xval.size())
    throw Error (InvalidRange, "EstimatePlotter::plot",
		 "iplot=%d >= nplot=%d", index, xval.size());

  if (control_viewport)
  {
    if (!viewports_set)
      set_world (x_min, x_max, y_min, y_max);
    else
      set_viewport (index);
  }

  unsigned npt = xval[index].size();
  unsigned plotted = 0;

  const unsigned ndim = yval.size() / xval.size();

  for (unsigned idim=0; idim < ndim; idim++)
  {
    if (ndim > 1)
      cpgsci (idim+1);

    unsigned yndex = index*ndim + idim;

    for (unsigned ipt=0; ipt<npt; ipt++)
    {
      if (minimum_error >= 0.0 && yerr[yndex][ipt] <= minimum_error)
	continue;

      if (maximum_error >= 0.0 && yerr[yndex][ipt] >= maximum_error)
	continue;

      cpgerr1 (6, xval[index][ipt], yval[yndex][ipt], yerr[yndex][ipt], 1.0);
      cpgpt1 (xval[index][ipt], yval[yndex][ipt], graph_marker);

      plotted ++;
    }
  }

  cpgsci (1);
  return plotted;
}

//! Set the range of indeces to be considered on next call to minmax
void EstimatePlotter::set_minmax_range (unsigned imin, unsigned imax)
{
  i_min = imin;
  i_max = imax;
}

void EstimatePlotter::minmax (bool& xrange, float& xmin, float& xmax,
			      bool& yrange, float& ymin, float& ymax,
			      const vector<float>& x,
			      const vector<float>& y,
			      const vector<float>& ye)
{
  unsigned npt = x.size();

  assert (y.size() == npt);
  assert (ye.size() == npt);

  unsigned i_start = 0;
  unsigned i_end = npt;

  if (i_max > 0)
  {
    i_start = i_min;
    i_end = i_max;
  }

  for (unsigned ipt=i_start; ipt<i_end; ipt++)
  {
    if (minimum_error >= 0.0 && ye[ipt] <= minimum_error)
      continue;

    if (maximum_error >= 0.0 && ye[ipt] >= maximum_error)
      continue;

    if (!yrange)
    {
      ymin = y[ipt] - ye[ipt];
      ymax = y[ipt] + ye[ipt];
      yrange = true;
    }

    if (!xrange)
    {
      xmin = x[ipt];
      xmax = x[ipt];
      xrange = true;
    }

    float yval = y[ipt] - ye[ipt];
    if (yval < ymin)
      ymin = yval;

    yval = y[ipt] + ye[ipt];
    if (yval > ymax)
      ymax = yval;
    
    if (x[ipt] < xmin)
      xmin = x[ipt];
    
    if (x[ipt] > xmax)
      xmax = x[ipt];
  }

  // cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

}

void EstimatePlotter::clear ()
{
  xval.resize(0);
  yval.resize(0);
  yerr.resize(0);
  xrange_set = yrange_set = false;
  viewports_set = false;
}
