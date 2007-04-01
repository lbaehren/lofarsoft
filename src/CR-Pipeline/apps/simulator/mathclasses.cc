
#include <vector>
#include <cmath>

using namespace std;

// Coordinate transformations have been tested and are ok at last!!!!

//... Private header file with prototype for mathclasses
#include "mathclasses.hh"


azelvec xyz2azel (double x, double y, double z) { 
  azelvec azel ;
  azel[2] = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  azel[1] = rad2deg( acos( z / azel[2] ) );
  azel[0] = -(rad2deg( atan2( y , x ) ) + 90);
  return azel ;
}

azelvec xyz2azel( xyzvec xyz) {
  azelvec azel ;
  azel[2] = sqrt(pow(xyz[0],2)+pow(xyz[1],2)+pow(xyz[2],2));
  azel[1] = rad2deg( acos( xyz[2] / azel[2] ) );
  azel[0] = -(rad2deg( atan2( xyz[1] , xyz[0] ) ) + 90);
  if(azel[0]<0) azel[0]+=360 ;
  return azel ;
}


xyzvec azel2xyz(double az, double ze, double d){
  xyzvec cart ;
  cart[0] = -sin( deg2rad( az ) ) * d * sin( deg2rad( ze ) );
  cart[1] = -cos( deg2rad( az ) ) * d * sin( deg2rad( ze ) );
  cart[2] = d * cos( deg2rad( ze ) ) ;
  return cart ;
}

xyzvec azel2xyz( azelvec azel){
  xyzvec cart ;
  cart[0] = -sin( deg2rad( azel[0]  ) ) * azel[2] * sin( deg2rad( azel[1] ) );
  cart[1] = -cos( deg2rad( azel[0]  ) ) * azel[2] * sin( deg2rad( azel[1] ) );
  cart[2] = azel[2] * cos( deg2rad( azel[1] ) ) ;
  return cart ;
}


double rad2deg(double rad){
  double deg;
  deg = rad * 180 / pi() ;
  return deg ;
}

double deg2rad(double deg){
  double rad ;
  rad = deg * pi() / 180 ;
  return rad;
}

double pi( void ) {

  return acos(-1);
}

double c( void ) {
  return 299792458.0 ;
}


double distance(double x, double y, double z, double x1, double y1, double z1) {
  double r;
  r = sqrt(pow(x-x1,2) + pow(y-y1,2) + pow(z-z1,2)) ;
  return r;
}

double length (double x, double y, double z) {
  double r;
  r = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  return r;
}

double diffsq(double x, double y, double z, double x1, double y1, double z1) {
  double r ;
  r = pow(length(x,y,z),2) - pow(length(x1, y1, z1),2);
  return r;
    
}

double diff(double x ,double y){
  return x - y;
}

