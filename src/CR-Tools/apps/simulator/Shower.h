#ifndef SHOWER_HH
#define SHOWER_HH

#include<math.h>
#include<vector>
#include "xyzvec.h"

#include "mathclasses.h"

using namespace std;

/*!
  \ingroup CR_Applications
*/
class Shower {

public:

  Shower(double tnul, double AZ, double ZE, double dist):_tnul(tnul),_AZ(AZ), _ZE(ZE),_dist(dist) { }


  double get_toa(double x, double y){
    xyzvec cart;
    cart=azel2xyz(_AZ, _ZE, _dist);
    
    double disttrav, toa;
    disttrav = sqrt(pow(cart[0]-x,2) + pow(cart[1]-y,2) + pow(cart[2],2)) ;
    return toa;
  }

 
  
private:

  double _tnul, _AZ, _ZE, _dist ;
} ;



#endif
