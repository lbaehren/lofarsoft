/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "pgutil.h"

#include <cpgplot.h>
#include <math.h>

using namespace std;

float pgplot::get_aspect_ratio ()
{
  float x1, x2, y1, y2;
  cpgqvsz (Inches, &x1, &x2, &y1, &y2);
  return y2 / x2;
}

float pgplot::get_viewport_aspect_ratio ()
{
  float x1, x2, y1, y2;
  cpgqvp (Inches, &x1, &x2, &y1, &y2);

  return (y2-y1) / (x2-x1);
}

void pgplot::get_scale (Units from, Units to, float& width, float& height)
{
  float j = 0;
  float fx, fy;
  cpgqvsz (from, &j, &fx, &j, &fy);

  // cerr << "from x=" << fx << " y=" << fy << endl;

  float tx, ty;
  cpgqvsz (to, &j, &tx, &j, &ty);

  // cerr << "to x=" << tx << " y=" << ty << endl;

  width = tx / fx;
  height = ty / fy;
}

void pgplot::set_paper_size (float width_cm, float aspect_ratio)
{
  if (width_cm == 0 && aspect_ratio == 0)
    return;

  if (aspect_ratio == 0)
    aspect_ratio = get_aspect_ratio ();

  float cm_per_inch = 2.54;

  cpgpap( width_cm/cm_per_inch, aspect_ratio );
}

void pgplot::set_dimensions (unsigned width_pixels, unsigned height_pixels)
{
  float width_scale, height_scale;
 
  get_scale (Pixels, Inches, width_scale, height_scale);

  float width_inches = width_pixels * width_scale;
  float aspect_ratio = height_pixels * height_scale / width_inches;

  cpgpap( width_inches, aspect_ratio );

  float x1, x2, y1, y2;
  cpgqvsz (Pixels, &x1, &x2, &y1, &y2);

  if (round(x2) != width_pixels)
    cerr << "pgplot::set_dimensions width=" << x2
	 << " != request=" << width_pixels << endl;

  if (round(y2) != height_pixels)
    cerr << "pgplot::set_dimensions height=" << y2
	 << " != request=" << height_pixels << endl;
}
