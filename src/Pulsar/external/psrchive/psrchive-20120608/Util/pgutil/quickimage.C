#include "quickplot.h"

#include <iostream>
using namespace std;

void quickimage (unsigned xdim, const std::vector<float>& vals)
{
  float min = *min_element(vals.begin(), vals.end());
  float max = *max_element(vals.begin(), vals.end());

  unsigned ydim = vals.size() / xdim;

  cerr << "min=" << min << " max=" << max << endl;
  cerr << "xdim=" << xdim << " ydim=" << ydim << endl;

  float trf[6] = { -0.5, 1.0, 0.0,
                   -0.5, 0.0, 1.0 };

  cpgswin (0, xdim, 0, ydim);
  cpgimag (&vals[0], xdim, ydim, 1, xdim, 1, ydim, min, max*2, trf);
}

