/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Coordinates/GeodeticEllipsoid.h>

namespace CR { // Namespace CR -- begin

// ---  Construction -----------------------------------------------------------

GeodeticEllipsoid::GeodeticEllipsoid ()
{
  String name ("Undefined");           // identifier
  double a (0.0);                      // major axis [m]
  double b (0.0);                      // minor axis [m]
  Vector<double> translation(3);       // translation parameters [m]
  Vector<double> rotation(3);          // rotation parameters
  double scale (1.0);                  // Scale parameter

  // initialize the data fields ...
  GeodeticEllipsoid::init(name,
			  a,
			  b,
			  translation,
			  rotation,
			  scale);

  // ... but set the control flag to false, since the values are not yet set 
  // up correctly;
  flag_p = false;
}

GeodeticEllipsoid::GeodeticEllipsoid (const String& name,
				      double major,
				      double minor,
				      Vector<double> translation,
				      Vector<double> rotation,
				      double scale)
{
  GeodeticEllipsoid::init(name,
			  major,
			  minor,
			  translation,
			  rotation,
			  scale);
}

// --- Deconstruction ----------------------------------------------------------

GeodeticEllipsoid::~GeodeticEllipsoid ()
{;}

// =============================================================================
//
// Initialization
//
// =============================================================================

void GeodeticEllipsoid::init(String name,
			     double major,
			     double minor,
			     Vector<double> translation,
			     Vector<double> rotation,
			     double scale)
{
  // flattnes parameter
  flattening_p = 0.0;

  // major and minor axis of the ellipsoid
  majorAxis_p = major;
  minorAxis_p = minor;

  GeodeticEllipsoid::setName(name);
  GeodeticEllipsoid::setScale(scale);
  
  /* do not accept the input vetors with translation and rotation paramater 
     values without checking; make sure that the number of elements is
     correct, i.e. = 3. This is done within the related routines. */
  GeodeticEllipsoid::setTranslation(translation);
  GeodeticEllipsoid::setRotation(rotation);

}

// =============================================================================
//
// Set variable values
//
// =============================================================================

void GeodeticEllipsoid::setName (const String& name)
{
  name_p = name;
}

void GeodeticEllipsoid::setMajorAxis (const double& a)
{
  majorAxis_p = a;
}

void GeodeticEllipsoid::setMinorAxis(const double& b)
{
  minorAxis_p = b;
}

void GeodeticEllipsoid::setMinorAxis()
{
  double a,f;
  Complex b;

  a = majorAxis_p;
  f = flattening_p;

  /* Compute the value of the minor axis from the value of the major axis, a,
     and the value flattening, f. */
  if (f != 0 && f != 0) {
    b = a*sqrt(1 - 2*f + f*f);
  }

  /* Check if the value is real; otherwise the minor axis value to zero and mark
     the data settings invalid (using the flag). */
  if (imag(b) == 0) {
    minorAxis_p = real(b);
  } else {
    minorAxis_p = 0.0;
    flag_p = false;
  }
}

void GeodeticEllipsoid::setScale (const double& scale)
{
  scale_p = scale;
}

void GeodeticEllipsoid::setTranslation (const Vector<double>& translation)
{
  translation_p.resize(3);

  if (translation.nelements() == 3) {
    translation_p = translation;
  } else {
    cerr << "[GeodeticEllipsoid::setTranslation] Invalid number of elements."
	 << endl;
    translation_p = 0.0;
    flag_p = false;
  }
}

void GeodeticEllipsoid::setRotation (const Vector<double>& rotation)
{
  rotation_p.resize(3);

  if (rotation.nelements() == 3) {
    rotation_p = rotation;
  } else {
    cerr << "[GeodeticEllipsoid::setRotation] Invalid number of elements."
	 << endl;
    rotation_p = 0.0;
    flag_p = false;
  }
}
void GeodeticEllipsoid::setFlattening()
{
  GeodeticEllipsoid::setFlattening(majorAxis_p,
				    minorAxis_p);
}

void GeodeticEllipsoid::setFlattening(const double& flattening) {

  double b (minorAxis_p);
  double fTemp;
  
  /* Commonly coordinate frames are defined in terms of (a,1/f); in this case
     the value of the minor axis, b, still may be unset - if this is indeed the 
     case we compute it now. */
  if (b == 0) {
    flattening_p = flattening;
    GeodeticEllipsoid::setMinorAxis();
  } else {
    fTemp = GeodeticEllipsoid::get_flattening();
    if (fTemp != flattening) {
      cerr << "GeodeticEllipsoid::setFlattening :";
      cerr << " The provided flattening is inconsistent with the value derived";
      cerr << " from major and minor axis values.";
      cerr << endl;
    }
  }

}

void GeodeticEllipsoid::setFlattening (const double& a,
				       const double& b)
{
  double f1 (0.0);
  double f2 (0.0);
  double ecc (GeodeticEllipsoid::eccentricity(a,b));
  complex<double> root;

  root = sqrt(1.0 - ecc);
  
  if (imag(root) == 0) { // the root is real
    f1 = 1.0 + real(root);
    f2 = 1.0 - real(root);
  } else { // the root is imaginary
    f1 = f2 = -1;
  }

  // Convention according to NIMA TR8350.2: use the negative sign for the root
  flattening_p = f2;
}

// =============================================================================
//
// Get/print variable values
//
// =============================================================================

void GeodeticEllipsoid::print_variables() {
  
  cout << endl;
  cout << "------------------------------------------------------------" << endl;
  cout << " GeodeticEllipsoid" << endl;
  cout << "------------------------------------------------------------" << endl;
  cout << " flag_p ...... = " << flag_p << endl;
  cout << " name_p ...... = " << name_p << endl;
  cout << " translation_p = " << translation_p << endl;
  cout << " rotation_p .. = " << rotation_p << endl;
  cout << " scale_p ..... = " << scale_p << endl;
  cout << " majorAxis_p . = " << majorAxis_p << endl;
  cout << " minorAxis_p . = " << minorAxis_p << endl;
  cout << " flattening_p  = " << flattening_p << endl;
  cout << "------------------------------------------------------------" << endl;
  cout << endl;

}

double GeodeticEllipsoid::get_flattening()
{
  /* Check first if the flattening value is already stored; if this is the case we 
   can read from storage, otherwise we have to compute first. */
  if (flattening_p == 0) {
    GeodeticEllipsoid::setFlattening();
  }

  /* Since the value is available now, we can return it. */
  return flattening_p;
}

// =============================================================================
//
// Computation routines
//
// =============================================================================

/*
  NAME
   GeodeticEllipsoid::eccentricity
  TYPE
  DESCRIPTION
   Compute the eccentricity of the ellipsoid with major semi-axis a and minor 
   semi-axis b.
  INPUT
   a,b  = Major and minor semi-axis of the ellipsoid.
  RETURN
   ecc  = Eccentricity of the ellipsoid
 */

double GeodeticEllipsoid::eccentricity ()
{
  return GeodeticEllipsoid::eccentricity(majorAxis_p,
					 minorAxis_p);
}

double GeodeticEllipsoid::eccentricity (const double& a,
					const double& b)
{
  return sqrt((a*a-b*b)/(a*a));
} 

// ===================================================================

/*
  NAME
   Geo_prime_vertical_radius
  TYPE
   C++ / Computation routine
  DESCRIPTION
   Compute the prime vertical radius of curvature at latitude phi.
  INPUT
   double a   = Major semi-axis of the ellipsoid.    [  m]
   double b   = Minor semi-axis of the ellipsoid.    [  m]
   double lat = Geographic latitude of the position. [deg]
  RETURN
*/
double GeodeticEllipsoid::primeVerticalRadius (const double& a,
					       const double& b,
					       const double& lat)
{
  double ecc (GeodeticEllipsoid::eccentricity(a,b));

  return a/sqrt(1-ecc*ecc*sin(lat)*sin(lat));
}

// ===================================================================

void GeodeticEllipsoid::geographic_to_geocentric (const double& a,
						  const double& b,
						  Vector<double> geogra,
						  Vector<double>& geocen)
{
  // Geocentric coordinates
  double x (0.0);
  double y (0.0);
  double z (0.0);
  
  // cal the computation routine
  GeodeticEllipsoid::geographic_to_geocentric(a,
					      b,
					      geogra(0),
					      geogra(1),
					      geogra(2),
					      x,
					      y,
					      z);

  // copy the coordinate values to the vector returned
  geocen.resize(3);
  geocen(0) = x;
  geocen(1) = y;
  geocen(2) = z;
}

void GeodeticEllipsoid::geographic_to_geocentric (const double& a,
						  const double& b,
						  double lat,
						  double lon,
						  double h,
						  double& x,
						  double& y,
						  double& z)
{
  // compute the eccentricity of the ellipsoid
  double ecc (GeodeticEllipsoid::eccentricity (a,b));

  // compute the prime vertical radius at latitude 'lat'
  double N (GeodeticEllipsoid::primeVerticalRadius(a,b,lat));

  x = (N+h)*cos(lat)*cos(lon);      
  y = (N+h)*cos(lat)*sin(lon);
  z = ((1-ecc*ecc)*N+h)*sin(lat);
}

Vector<double> GeodeticEllipsoid::helmertTransform (const Vector<double>& from,
						    const bool inverse)
{
  Vector<double> to (3);

  if (inverse) {
    // rotation matrix for forward tranformation 
    double matR[3][3];
    double detR;
    double invR[3][3];
    double dx,dy,dz;
    
    matR[0][0] = 1;
    matR[0][1] = rotation_p(2);
    matR[0][2] = -rotation_p(1);
    matR[1][0] = -rotation_p(2);
    matR[1][1] = 1;
    matR[1][2] = rotation_p(0);
    matR[2][0] = rotation_p(1);
    matR[2][1] = -rotation_p(0);
    matR[2][2] = 1;
    
    /* compute the determinante of the rotation matrix for the forward
       transformation */
    detR  = matR[0][0]*matR[1][1]*matR[2][2];
    detR -= matR[0][0]*matR[1][2]*matR[2][1];
    detR -= matR[1][0]*matR[0][1]*matR[2][2];
    detR += matR[1][0]*matR[0][2]*matR[2][1];
    detR += matR[2][0]*matR[0][1]*matR[1][2];
    detR -= matR[2][0]*matR[0][2]*matR[1][1];
    
    // set up the rotation matrix for the inverse transformation
    invR[0][0] =  ( matR[1][1]*matR[2][2] - matR[1][2]*matR[2][1])/detR;
    invR[0][1] = -( matR[0][1]*matR[2][2] - matR[0][2]*matR[2][1])/detR;
    invR[0][2] =  ( matR[0][1]*matR[1][2] - matR[0][2]*matR[1][1])/detR;
    invR[1][0] = -( matR[1][0]*matR[2][2] - matR[1][2]*matR[2][0])/detR;
    invR[1][1] =  ( matR[0][0]*matR[2][2] - matR[0][2]*matR[2][0])/detR;
    invR[1][2] = -( matR[0][0]*matR[1][2] - matR[0][2]*matR[1][0])/detR;
    invR[2][0] = -(-matR[1][0]*matR[2][1] + matR[1][1]*matR[2][0])/detR;
    invR[2][1] =  (-matR[0][0]*matR[2][1] + matR[0][1]*matR[2][0])/detR;
    invR[2][2] =  ( matR[0][0]*matR[1][1] - matR[0][1]*matR[1][0])/detR;
    
    // substract the translation vector from the position vector
    dx = from(0)-translation_p(0);
    dy = from(1)-translation_p(1);
    dz = from(2)-translation_p(2);
    
    // perform the coordinate transformation
    to(0)  = (dx*invR[0][0] + dy*invR[0][1] + dz*invR[0][2])/scale_p;
    to(1)  = (dx*invR[1][0] + dy*invR[1][1] + dz*invR[1][2])/scale_p;
    to(2)  = (dx*invR[2][0] + dy*invR[2][1] + dz*invR[2][2])/scale_p;
  } 
  else {
    to(0) = translation_p(0);
    to(0) += (scale_p*(1*from(0) + rotation_p(2)*from(1) - rotation_p(1)*from(2)));
    
    to(1) = translation_p(1);
    to(1) += (scale_p*(-rotation_p(2)*from(0) + 1*from(1) + rotation_p(0)*from(2)));
    
    to(2) = translation_p(2);
    to(2) += (scale_p*(rotation_p(1)*from(0) - rotation_p(0)*from(1) + 1*from(2)));
  }

  return to;
}

} // Namespace CR -- end
