/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"

using namespace Pulsar;

float width (const Profile* profile, float& error, float pc, float dc)
{

  float min_phs = profile->find_min_phase(dc);
  int   min_bin = int(min_phs * float(profile->get_nbin()));

  double min_mean = 0.0;
  double min_var  = 0.0;
  double var_var  = 0.0;

  int start = int(float(profile->get_nbin()) * (min_phs - dc/2.0));
  int end   = int(float(profile->get_nbin()) * (min_phs + dc/2.0));

  profile->stats(&min_mean, &min_var, &var_var, start, end);

  float stdev = sqrt(min_var);

  float level = profile->max() * pc/100.0 - stdev;

  std::vector<float> results;

  for (unsigned tries = 0; tries < 3; tries++)
  {

    if (level < 5.0*min_var)
    {
      return 0.0;
    }
    float c = 0.0;
    float m = 0.0;
    float lo_edge = 0.0;
    float hi_edge = 0.0;

    int bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linerly interpolate between both sides of the edge
        int x1 = bc;
        int x2 = bc+1;

        float y1 = profile->get_amps()[x1];
        float y2 = profile->get_amps()[x2];

        m = (y2 - y1) / (float(x2) - float(x1));
        c = y1 - m*x1;

        hi_edge = (level - c)/m;

        break;
      }
      bc--;
      if (bc < 0)
        bc += profile->get_nbin();
      if (bc == min_bin)
        break;
    }

    bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linerly interpolate between both sides of the edge
        int x1 = bc;
        int x2 = bc-1;

        float y1 = profile->get_amps()[x1];
        float y2 = profile->get_amps()[x2];

        m = (y2 - y1) / (float(x2) - float(x1));
        c = y1 - m*x1;

        lo_edge = (level-c)/m;

        break;
      }
      bc++;
      if (bc > int(profile->get_nbin()-1))
        bc -= profile->get_nbin();
      if (bc == min_bin)
        break;
    }

    float bin_dist = hi_edge - lo_edge;
    if(bin_dist < 0)bin_dist += float(profile->get_nbin());

    results.push_back(bin_dist / float(profile->get_nbin()));
    level += stdev;

  }

  error = fabs(results.front()-results.back())/2.0;
  return results[1];
}
