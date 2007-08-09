#ifndef SHOWER_HH
#define SHOWER_HH

#include<math.h>
#include<vector>
#include "xyzvec.h"

#include "mathclasses.h"

using namespace std;
class Shower {

public:

  Shower(double tnul, double x, double y, double z):_x(x),_y(y), _z(z),_tnul(tnul) { }


  double get_toa(double x, double y){

    double disttrav, toa;
    disttrav = sqrt(pow((_x-x),2) + pow((_y-y),2) + pow(_z,2)) ;
    toa = (disttrav /c()) + _tnul ;

    return toa;
  }

 
  
private:

double  _x, _y, _z, _tnul ;
} ;



#endif
