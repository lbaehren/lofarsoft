/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "Pulsar/Faraday.h"

using namespace std;

void usage()
{
  cerr <<
    "rmrot - output change in position angle across observed band\n"
    "  Usage: rmrot [options]\n"
    "\n"
    "   -r RM      rotation measure in rad/m^2\n"
    "   -b BW      bandwidth in MHz\n"
    "   -f FREQ    centre frequency in MHz\n"
    "\n"
    "   -q         quiet (outputs only delta P.A.)\n"
    "   -v         verbose \n"
    "\n"
    "  Program returns the number of degrees over which the position angle \n"
    "  rotates across the band, as well as the differential group delay \n"
       << endl;
}

int main(int argc, char ** argv) try
{
  bool verbose = false;
  bool quiet = false;

  // these default parameters correspond to a delta PA = 0.129536 deg
  double rm = 1;
  double centrefreq = 430;
  double bw = 1;

  unsigned nchan = 1;

  int c;
  while ((c = getopt(argc, argv, "hb:f:n:r:qv")) != -1)
    switch (c) {

    case 'b':
      bw = atof (optarg);
      break;

    case 'r':
      rm = atof (optarg);
      break;

    case 'f':
      centrefreq = atof (optarg);
      break;

    case 'n':
      nchan = atoi (optarg);
      break;

    case 'h':
      usage ();
      return 0;

    case 'q':
      quiet = true;
      verbose = false;
      break;

    case 'v':
      Calibration::Faraday::verbose = true;
      verbose = true;
      quiet = false;
      break;

    default:
      cerr << "invalid param '" << c << "'" << endl;
    }

  Calibration::Faraday kernel;

  kernel.set_reference_frequency (centrefreq);
  kernel.set_rotation_measure (rm);

  if (!quiet)
    cout << "\nInput Parameters:\n"
      "Centre Frequency: " << kernel.get_reference_frequency () << " MHz\n"
      "Bandwidth:        " << bw << " MHz\n"
      "Rotation Measure: " << kernel.get_rotation_measure().val <<" rad/m^2\n";
  
  kernel.set_frequency (centrefreq + 0.5*bw);
  double pa_hi = kernel.get_rotation ();

  kernel.set_frequency (centrefreq - 0.5*bw);
  double pa_lo = kernel.get_rotation ();

  double delta_PA = (pa_hi - pa_lo) / (2.0*M_PI);
  
  if (!quiet)
  {
    // phase delay between RHC and LHS is twice the change in P.A.
    double dgd = 2.0 * fabs(delta_PA) / centrefreq;

    kernel.set_reference_wavelength (0.0);
    kernel.set_frequency (centrefreq);

    double abs_delta_PA = kernel.get_rotation () / (2.0*M_PI);
    double abs_dgd = 2.0 * fabs(abs_delta_PA) / centrefreq;

    cout << "\nOutput parameters:\n"
      "Position angle change:    " << delta_PA*360.0 << " deg\n"
      "Differential group delay: " << dgd*1e3 << " ns (across band)\n"
      "\n"
      "Absolute position angle change: " << abs_delta_PA*360.0 << " deg\n"
      "Absolute differential delay: " << abs_dgd*1e3 << " ns \n";
  }
  else
    cout << delta_PA*360.0 << endl;

  if (nchan < 2)
    return 0;

  kernel.set_rotation_measure (1.0);

  kernel.set_frequency (centrefreq + 0.5*bw/nchan);
  pa_hi = kernel.get_rotation ();

  kernel.set_frequency (centrefreq - 0.5*bw/nchan);
  pa_lo = kernel.get_rotation ();

  delta_PA = fabs(pa_hi - pa_lo);
  cout << "\nWith " << nchan << " frequency channels:\n"
      "maximum RM:    " << M_PI/delta_PA << endl;

}
catch (...)
{
  cerr << "exception thrown: " << endl;
  return -1;
}


