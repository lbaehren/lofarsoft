/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*!
 * Plots P.A. as a function of phi using two different equations
 */

#include <iostream>
#include <math.h>
#include "Cartesian.h"

using namespace std;

int main ()
{
  double alpha = 5;
  double zeta = 5.5;
  double phi0 = 180;
  //double PA0 = 0;

  double a = alpha * M_PI/180;
  double z = zeta * M_PI/180;

#if 0
  for (double psi=-90; psi < 90; psi += 1)
  {
    double p = psi * M_PI/180;
    double x = cos(p);
    double y = sin(p);

    double Q = x*x - y*y;

    double xu = x/sqrt(2) + y/sqrt(2);
    double yu = x/sqrt(2) - y/sqrt(2);

    double U = xu*xu - yu*yu;

    double p0 = 0.5 * atan2 (U,Q);

    cerr << p << " " << p0 << endl;
  }
  return 0;
#endif

  for (double phi=0; phi < 360; phi += 1)
  {
    double p = (phi - phi0) * M_PI/180;

    double tan_psi = sin(a)*sin(p) / (sin(z)*cos(a) - cos(z)*sin(a)*cos(p));
    double psi = atan (tan_psi);

    Cartesian L (sin(z), 0, cos(z));  // Line of sight
    Cartesian N (-cos(z), 0, sin(z)); // North

    // note that the RVM equation measures P.A. clockwise (toward West)
    // the observer's convention vector should point to East (0, -1, 0)
    Cartesian E (0, 1, 0); // West

    Cartesian M (cos(p)*sin(a), sin(p)*sin(a), cos(a));

    Cartesian vector = M - L;

    double x = vector * N;
    double y = vector * E;

    double psi1 = atan(y/x);

    // now, to predict Stokes Q and U without having to compute P.A.
    double Q = x*x - y*y;
    double U = 2*x*y;

    double psi2 = 0.5*atan2(U,Q);

    cout << phi << " " << psi*180/M_PI << " " << psi1*180/M_PI
	 << " " << psi2*180/M_PI << endl;
  }

  return 0;
}
