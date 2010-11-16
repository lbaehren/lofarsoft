#include <iostream>
#include <cmath>
#include "../src/tmf.h"

using namespace std;
using namespace tmf;

int main()
{
  double alpha = hms2rad(17, 48, 59.74);
  double delta = dms2rad(-14, 43, 8.2);
  double l = 0.0;
  double b = 0.0;

  /************************************************************************
   * Test of B1950 coordinate conversion                                  *
   ************************************************************************/

  cout << "alpha = " << rad2deg(alpha) << " delta = " << rad2deg(delta) << " (degrees)" << endl;

  cout << "alpha = " << alpha << " delta = " << delta << " (radians)" << endl;

  equatorial2galactic(l, b, alpha, delta);

  cout << "l = " << rad2deg(l) << " b = " << rad2deg(b) << endl;

  galactic2equatorial(alpha, delta, l, b);

  cout << "alpha = " << alpha << " delta = " << delta << " (radians)" << endl;

  cout << "alpha = " << rad2deg(alpha) << " delta = " << rad2deg(delta) << " (degrees)" << endl;

  /************************************************************************
   * Test of J2000 coordinate conversion                                  *
   ************************************************************************/

  double alpha_J = 0.0;
  double delta_J = 0.0;

  cout << "(B1950) alpha = " << rad2deg(alpha) << " delta = " << rad2deg(delta) << " (degrees)" << endl;

  b19502j2000(alpha_J, delta_J, alpha, delta);

  cout << "(J2000) alpha = " << rad2deg(alpha_J) << " delta = " << rad2deg(delta_J) << " (degrees)" << endl;

  j20002b1950(alpha, delta, alpha_J, delta_J);

  cout << "(B1950) alpha = " << rad2deg(alpha) << " delta = " << rad2deg(delta) << " (degrees)" << endl;

  cout << "alpha = " << rad2hmsrepr(alpha_J) << " delta = " << rad2dmsrepr(delta_J) << " (degrees)" << endl;
  cout << "l = " << rad2dmsrepr(l) << " b = " << rad2dmsrepr(b) << endl;
}

