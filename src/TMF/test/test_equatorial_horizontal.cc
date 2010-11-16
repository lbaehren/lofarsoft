#include <iostream>
#include <cmath>
#include "../src/tmf.h"

using namespace std;
using namespace tmf;

int main()
{
  double alpha = hms2rad(23, 9, 16.641);
  double delta = dms2rad(-6, 43, 11.61);
  double L = dms2rad(77, 3, 56);
  double phi = dms2rad(38, 55, 17);
  int d, h, m;
  double s, Dphi, Depsilon, epsilon;

  cout << "alpha " << rad2deg(alpha) << " delta " << rad2deg(delta) << endl;
  cout << "L " << rad2deg(L) << " phi " << rad2deg(phi) << endl;

  // Calculate JD(UT1)
  double ut = gregoriandate2jd(1987, 4, 10. + ((19. + 21. / 60.) / 24.));

  cout.precision(9);
  cout << "JD(UT1) " << ut << endl;

  // Calculate GMST
  double theta = rad2circle(gmst(ut));

  rad2hms(h, m, s, theta);

  cout << "GMST " << theta;
  cout.precision(2);
  cout << " = " << h << " " << m << " ";
  cout.precision(6);
  cout << s <<endl;

  // Calculate precession and nutation of the ecliptic
  double dtt = tt_utc(date2jd(1987, 4, 10. + ((19. + 21. / 60.) / 24.)));
  double tt = gregoriandate2jd(1987, 4, 10. + ((19. + 21. / 60. + dtt / 3600.) / 24.));

  cout.precision(9);
  cout << "JD(TT) " << tt << endl;

  nutation(Dphi, Depsilon, tt);

  epsilon = meanobliquity(ut) + Depsilon;

  rad2dms(d, m, s, epsilon);

  cout.precision(4);
  cout << "Dphi " << rad2deg(Dphi) * 3600 << " epsilon ";
  cout.precision(2);
  cout << d << " " << m << " ";
  cout.precision(4);
  cout << s << endl;

  // Calculate Greenwich Apparent Siderial Time
  double theta_0 = rad2circle(gast(ut, tt));
  rad2hms(h, m, s, theta_0);

  cout.precision(2);
  cout << "GAST " << theta_0 << " = " << h << " " << m << " ";
  cout.precision(5);
  cout << s << endl;

  // Calculate Local Apparant Sidereal Time
  double theta_L = rad2circle(last(ut, tt, L));

  rad2hms(h, m, s, theta_L);
  cout << "LAST " << h << " " << m << " " << s << endl;

  rad2hms(h, m, s, alpha);
  cout << h << " " << m << " " << s << endl;

  // Calculate hour angle
  double H = rad2circle(theta_L - alpha);

  cout.precision(9);
  cout << "H " << rad2deg(H) << endl;

  // Calculate Altitude and Azimuth
  double A = 0.;
  double h2 = 0.;
  equatorial2horizontal(A, h2, H, delta, phi);

  cout.precision(5);
  cout << "A " << rad2deg(A) << " h " << rad2deg(h2) << endl;

  // Test the inverse transformations
  horizontal2equatorial(H, delta, A, h2, phi);

  cout.precision(9);
  cout << "H " << rad2deg(H) << endl;

  alpha = deg2circle(rad2deg(theta_L - H));

  delta = rad2deg(delta);

  cout << "alpha " << alpha << " delta " << delta << endl;
}

