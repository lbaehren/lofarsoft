#ifndef MATHCLASSES_HH
#define MATHCLASSES_HH

#include "azelvec.hh"
#include "xyzvec.hh"

double rad2deg(double rad);
double deg2rad(double deg);

azelvec xyz2azel(double x, double y, double z);
xyzvec azel2xyz(double az, double ze, double d);

azelvec xyz2azel(xyzvec );
xyzvec azel2xyz( azelvec );

double pi( void ) ;
double c( void );

double distance(double, double, double, double, double, double);
double length(double, double, double) ;
double diffsq(double, double, double, double, double, double);
double diff(double, double) ;



#endif
