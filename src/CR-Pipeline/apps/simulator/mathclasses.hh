#ifndef MATHCLASSES_HH
#define MATHCLASSES_HH

#include "azelvec.h"
#include "xyzvec.h"

//! Convert angle from radian to degree
double rad2deg(double rad);
//! Convert angle from degree to radian
double deg2rad(double deg);

//! Convert cartesian to (az,el,distance) coordinates
azelvec xyz2azel(double x, double y, double z);
//! Convert (az,el,distance) to cartesian coordinates
xyzvec azel2xyz(double az, double ze, double d);

azelvec xyz2azel(xyzvec );
xyzvec azel2xyz( azelvec );

double pi( void ) ;
double c( void );

double distance(double x, double y, double z, double x1, double y1, double z1);
//! Get the length, \f$ \sqrt{x^2+y^2+z^2} \f$, of the vector
double length(double x, double y, double z) ;
double diffsq(double, double, double, double, double, double);
double diff(double, double) ;



#endif
