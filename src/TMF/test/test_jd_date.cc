#include <iostream>
#include "../src/tmf.h"

using namespace std;
using namespace tmf;

int main()
{
  double jd, d;
  int y, m;

  jd = 0;
  y = -4712;
  m = 1;
  d = 1.5;

  std::cout<<"Y "<<y<<" M "<<m<<" D "<<d<<std::endl;

  jd = date2jd(y, m, d);

  std::cout.precision(9);

  std::cout<<jd<<std::endl;

  int Y = 0;
  int M = 0;
  double D = 0;

  jd2date(Y, M, D, jd);

  std::cout<<"Y "<<Y<<" M "<<M<<" D "<<D<<std::endl;
}

