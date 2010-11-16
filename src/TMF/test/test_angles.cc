#include <iostream>
#include <cmath>
#include "../src/tmf.h"

using namespace std;
using namespace tmf;

int main()
{
  int h = 12;
  int m = 10;
  double s = 11.3;

  cout << h << " " << m << " " << s << endl;

  double d = hms2deg(h, m, s);

  cout << "deg = " << d << endl;

  deg2hms(h, m, s, d);

  cout << h << " " << m << " " << s << endl;

  // Test angles > 360
  d = 360 + 180;

  deg2hms(h, m, s, d);

  cout << h << " " << m << " " << s << endl;

  // Test angles < 0
  d = -180;

  deg2hms(h, m, s, d);

  cout << h << " " << m << " " << s << endl;

  // Test angles to deg, m, s
  int D = -110;
  m = 10.;
  s = 12.;

  cout << D << " " << m << " " << s << endl;

  double deg = dms2deg(D, m, s);

  cout << deg << endl;

  deg2dms(D, m, s, deg);

  cout << D << " " << m << " " << s << endl;

}

