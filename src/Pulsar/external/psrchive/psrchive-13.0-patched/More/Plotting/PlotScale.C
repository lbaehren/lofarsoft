/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotLabel.h"
#include "pairutil.h"

#include <math.h>

using namespace std;

std::pair<float,float> unset_range_norm (0.0, 1.0);
std::pair<float,float> unset_world (0.0, 0.0);

Pulsar::PlotScale::PlotScale () :
  world (unset_world),
  range_norm (unset_range_norm),
  world_external (unset_world)
{
  buf_norm = 0.0;
  minval = 0.0;
  maxval = 1.0;
}

void Pulsar::PlotScale::init (const Archive*)
{
}

//! Set the minimum and maximum value in the data
void Pulsar::PlotScale::set_minmax (float min, float max)
{
  minval = min;
  maxval = max;
}

//! Get the minimum and maximum value in the data
void Pulsar::PlotScale::get_minmax (float& min, float& max) const
{
  min = minval;
  max = maxval;
}

void Pulsar::PlotScale::set_range_norm (const std::pair<float,float>& f)
{
  range_norm = f;
}

void Pulsar::PlotScale::set_world (const std::pair<float,float>& f)
{
  world = f;
}

void Pulsar::PlotScale::set_world_external (const std::pair<float,float>& f)
{
  world_external = f;
}

std::pair<float,float> Pulsar::PlotScale::get_world_external () const
{
  return world_external;
}

void Pulsar::PlotScale::get_range (float& min, float& max) const
{
  get_minmax (min, max);

  if (world != unset_world)
  {
    min = world.first;
    max = world.second;
  }

  if (range_norm != unset_range_norm)
    stretch (range_norm, min, max);

  if (buf_norm > 0)
  {
    float space = (max - min) * buf_norm;
    min -= space;
    max += space;
  }

}

//! Return min and max scaled according to zoom attributes
void Pulsar::PlotScale::get_range_external (float& min, float& max) const
{
  if (world_external == unset_world)
    get_range (min, max);
  else
  {
    min = world_external.first;
    max = world_external.second;
  }
}

void Pulsar::PlotScale::get_indeces (unsigned n,
				     unsigned& imin, unsigned& imax,
				     bool cyclic) const
{
  float min = 0.0;
  float max = 1.0;

  if (world != unset_world)
  {
    float wmin = 0.0;
    float wmax = 1.0;

    get_minmax (wmin, wmax);
    float length = wmax - wmin;

    std::pair<float,float> zoom;

    zoom.first = (world.first - wmin) / length;
    zoom.second = (world.second - wmin) / length;

    stretch (zoom, min, max);
  }

  stretch (range_norm, min, max);

  if (cyclic)
  {
    double diff = max - min;

    // fold onto 0->1
    min -= floor (min);
    max -= floor (max);

    // and ensure that max > min
    if (max <= min)
      max += 1.0;

    // and ensure that the difference is preserved
    if (diff > 0 && (max - min) < diff)
      max += 1.0;
  }
  else
  {
    if (min < 0)
      min = 0;
    if (max < 0)
      max = 0;
  }

  imin = unsigned( min * n );
  imax = unsigned( max * n );

  if (!cyclic)
  {
    if (imin > n)
      imin = n;

    if (imax > n)
      imax = n;
  }
}
