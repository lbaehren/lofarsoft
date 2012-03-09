/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Plot3D.h"

#include <math.h>
#include <cpgplot.h>

using namespace std;

bool pgplot::Plot3D::verbose = false;

void pgplot::Plot3D::where (float& x, float& y, const Cartesian& pt)
{
  x = pt * vx_axis;
  y = pt * vy_axis;
}

void pgplot::Plot3D::poly (const vector<Cartesian>& pts)
{
  if (pts.size() == 0)
    return;

  float* x = new float [pts.size()];
  float* y = new float [pts.size()];

  for (unsigned ipt=0; ipt<pts.size(); ipt++) {
    x[ipt] = float(pts[ipt] * vx_axis);
    y[ipt] = float(pts[ipt] * vy_axis);
  }

  if (verbose)
    cerr << "pgplot::Plot3D::poly call cpgpoly npt=" 
	 << pts.size() << endl;

  cpgpoly (pts.size(), x, y);

  delete [] x;
  delete [] y;
}

// primitives
void pgplot::Plot3D::move (const Cartesian& pt)
{
  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgmove (x, y);
}

void pgplot::Plot3D::draw (const Cartesian& pt)
{
  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgdraw (x, y);
}

void pgplot::Plot3D::set_hatch (const Cartesian& hp)
{
  float x = hp * vx_axis;
  float y = hp * vy_axis;
  float angle = 180/M_PI * atan (y/x);

  cpgshs (angle, 1, 0);
}

void pgplot::Plot3D::plot (const Cartesian& pt, int symbol)
{
  if (verbose)
    cerr << "pgplot::Plot3D::plot " << pt << endl;

  float x = pt * vx_axis;
  float y = pt * vy_axis;

  if (verbose)
    cerr << "pgplot::Plot3D::plot x=" << x << " y=" << y << endl;

  cpgpt1 (x, y, symbol);
}

void pgplot::Plot3D::text (const Cartesian& pt, 
				  const char* text, float align)
{
  if (verbose)
    cerr << "pgplot::Plot3D::text " << pt << endl;

  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgptxt (x, y, 0, align, text);

}

void pgplot::Plot3D::arrow (const Cartesian& from, const Cartesian& to)
{
  float fx = from * vx_axis;
  float fy = from * vy_axis;
  float tx = to * vx_axis;
  float ty = to * vy_axis;

  cpgarro (fx, fy, tx, ty);
}

// set camera position - phi and theta in degrees
void pgplot::Plot3D::set_camera (double theta, double phi)
{
  vx_axis = Cartesian (0, 1, 0);
  vy_axis = Cartesian (0, 0, 1);

  Angle t, p;
  t.setDegrees(theta);
  p.setDegrees(-phi);

  vy_axis.y_rot (p);
  vx_axis.z_rot (t); vy_axis.z_rot (t);
}

void pgplot::Plot3D::init()
{
  vx_axis = Cartesian (0, 1, 0);
  vy_axis = Cartesian (0, 0, 1);
}

