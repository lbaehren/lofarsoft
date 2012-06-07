#include "sky_coord.h"
#include <stdlib.h>

using namespace std;

string hms = "19:01:31";
string dms = "-33:45:13";

void test (sky_coord& coord)
{
  AnglePair radec = coord.getRaDec();

  string HMS = radec.angle1.getHMS(0);
  string DMS = radec.angle2.getDMS(0);

  if (HMS != hms) {
    cerr << "sky_coord fail:\n"
      "  input hms='" << hms << "'\n"
      " output hms='" << HMS << "'\n";
    exit (-1);
  }

  if (DMS != dms) {
    cerr << "sky_coord fail:\n"
      "  input dms='" << dms << "'\n"
      " output dms='" << DMS << "'\n";
    exit (-1);
  }
}

int main ()
{
  cerr << "sky_coord test constructor" << endl;
  sky_coord coord (hms + dms);
  test (coord);

  cerr << "sky_coord test setHMSDMS" << endl;
  coord.setHMSDMS (hms, dms);
  test (coord);

  cerr << "sky_coord test copy constructor" << endl;
  sky_coord copy = coord;
  test (copy);

  cerr << "sky_coord class passes all tests" << endl;
  return 0;
}
