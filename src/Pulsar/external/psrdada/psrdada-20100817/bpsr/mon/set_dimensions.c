/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <cpgplot.h>
#include <math.h>

/* UNITS: 0 => answer in normalized device coordinates
          1 => answer in inches
	  2 => answer in mm
	  3 => answer in absolute device coordinates (dots)
	  4 => answer in world coordinates
	  5 => answer as a fraction of the current viewport size
*/

const int Device = 0;
const int Inches = 1;
const int Millimetres = 2;
const int Pixels = 3;
const int World = 4;
const int Viewport = 5;

void get_scale (int from, int to, float* width, float* height)
{
  float j = 0;
  float fx, fy;
  cpgqvsz (from, &j, &fx, &j, &fy);

  float tx, ty;
  cpgqvsz (to, &j, &tx, &j, &ty);

  if (width)
    *width = tx / fx;
  if (height)
    *height = ty / fy;
}

void set_dimensions (unsigned width_pixels, unsigned height_pixels)
{
  float width_scale, height_scale;
 
  get_scale (Pixels, Inches, &width_scale, &height_scale);

  float width_inches = width_pixels * width_scale;
  float aspect_ratio = height_pixels * height_scale / width_inches;

  cpgpap( width_inches, aspect_ratio );

  float x1, x2, y1, y2;
  cpgqvsz (Pixels, &x1, &x2, &y1, &y2);
}
