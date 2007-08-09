#ifndef MATHCLASSES_HH
#define MATHCLASSES_HH

#include "azelvec.h"
#include "xyzvec.h"

//! Convert angle from radian to degree
double rad2deg(double rad);
//! Convert angle from degree to radian
double deg2rad(double deg);

/*!
  \brief Convert cartesian to (az,el,distance) coordinates

  \param x -- x-component of the cartesian coordinate
  \param y -- y-component of the cartesian coordinate
  \param z -- z-component of the cartesian coordinate

  \return azelvec -- Vector of Azimuth-Elevation coordinates
*/
azelvec xyz2azel(double x, double y, double z);
/*!
  \brief Convert (az,el,distance) to cartesian coordinates

  \param az
  \param ze
  \param d

  \return xyzvec
*/
xyzvec azel2xyz(double az, double ze, double d);

azelvec xyz2azel(xyzvec );
xyzvec azel2xyz( azelvec );

/*!
  \brief Numerical constant: \f$ \pi \f$

  \return pi
*/
double pi( void ) ;
/*!
  \brief Numerical constant: speed of light \f$ c \f$
*/
double c( void );
/*!
  \brief Compute the distance between two points in 3-dim spacce

  \param x
  \param y
  \param z
  \param x1
  \param y1
  \param z1

  \return distance 
*/
double distance(double x, double y, double z, double x1, double y1, double z1);
/*!
  \brief Get the length, \f$ \sqrt{x^2+y^2+z^2} \f$, of the vector

  \param x
  \param y
  \param z

  \return length 
*/
double length(double x, double y, double z) ;
double diffsq(double, double, double, double, double, double);
double diff(double, double) ;

#endif
