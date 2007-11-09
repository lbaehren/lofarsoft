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

/* $Id: template-tclass.cc,v 1.6 2006/09/20 09:56:53 bahren Exp $*/

#include <Math/VectorConversion.h>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

#ifdef HAVE_CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#endif

/*!
  \file tVectorConversion.cc

  \ingroup Math

  \brief A collection of test routines for VectorConversion
 
  \author Lars B&auml;hren
 
  \date 2007/05/29
*/

// -----------------------------------------------------------------------------

void show_conversion (double const &a1,
		      double const &a2,
		      double const &a3,
		      double const &b1,
		      double const &b2,
		      double const &b3)
{
  std::cout << "\t";
  std::cout << "[" << a1 << "," << a2 << "," << a3 << "]";
  std::cout << "  ->  ";
  std::cout << "[" << b1 << "," << b2 << "," << b3 << "]";
  std::cout << std::endl;
}

void show_conversion (std::vector<double> const &a,
		      std::vector<double> const &b)
{
  std::cout << "\t";
  std::cout << "[" << a[0] << "," << a[1] << "," << a[2] << "]";
  std::cout << "  ->  ";
  std::cout << "[" << b[0] << "," << b[1] << "," << b[2] << "]";
  std::cout << std::endl;
}

#ifdef HAVE_BLITZ
void show_conversion (blitz::Array<double,1> const &a,
		      blitz::Array<double,1> const &b)
{
  std::cout << "[" << a(0) << "," << a(1) << "," << a(2) << "]";
  std::cout << "  ->  ";
  std::cout << "[" << b(0) << "," << b(1) << "," << b(2) << "]";
  std::cout << std::endl;
}
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test routines for conversion of angles

  \return nofFailedTests -- The number of failed tests.
*/
int test_angleConversion ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_angleConversion]\n" << std::endl;

  std::cout << "[1] double deg2rad (double const &deg)" << std::endl;
  try {
    double degInput (0);
    double degOutput (0);
    double radian (0);

    for (int n(0); n<=90; n++) {
      degInput = double(n);
      radian = CR::deg2rad (degInput);
      degOutput = CR::rad2deg (radian);
    }

  } catch (std::string err) {
    std::cerr << err << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] void deg2rad (double &rad,double const &deg)" << std::endl;
  try {
    double degInput (0);
    double degOutput (0);
    double radian (0);

    for (int n(0); n<=90; n++) {
      degInput = double(n);
      CR::deg2rad (radian,degInput);
      CR::rad2deg (degOutput,radian);
    }

  } catch (std::string err) {
    std::cerr << err << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the generic interface to the vector conversion functions

  \return nofFailedTests -- Number of failed tests within this function
*/
int test_vectorConversion ()
{
  std::cout << "\n[test_vectorConversion]\n" << std::endl;

  int nofFailedTests (0);

  double xSource (0.0);
  double ySource (0.0);
  double zSource (0.0);
  double xTarget (0.0);
  double yTarget (0.0);
  double zTarget (0.0);
  bool status (true);

  std::cout << "[1] Conversion from cartesian to other" << std::endl;
  try {
    xSource = 1.0;
    ySource = 1.0;
    zSource = 1.0;
    
    std::cout << "-- cartesian -> cylindrical" << std::endl;
    status = CR::convertVector (xTarget,
				yTarget,
				zTarget,
				CR::Cylindrical,
				xSource,
				ySource,
				zSource,
				CR::Cartesian,
				true);
    show_conversion(xSource,ySource,zSource,xTarget,yTarget,zTarget);
    
    std::cout << "-- cartesian -> spherical" << std::endl;
    status = CR::convertVector (xTarget,
				yTarget,
				zTarget,
				CR::Spherical,
				xSource,
				ySource,
				zSource,
				CR::Cartesian,
				true);
    show_conversion(xSource,ySource,zSource,xTarget,yTarget,zTarget);
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Conversion from cylindrical to other" << std::endl;
  try {
    xSource =  1.0;  // rho
    ySource = 40.0;  // phi
    zSource =  1.0;  // z
    
    std::cout << "-- cylindrical -> cartesian" << std::endl;
    status = CR::convertVector (xTarget,
				yTarget,
				zTarget,
				CR::Cartesian,
				xSource,
				ySource,
				zSource,
				CR::Cylindrical,
				true);
    show_conversion(xSource,ySource,zSource,xTarget,yTarget,zTarget);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from cartesian to other coordinates

  This will run a number of very basic tests for the following routines:
  - CR::Cartesian2Spherical
  - CR::Cartesian2Cylindrical
  - CR::Cartesian2AzElHeight
  - CR::Cartesian2AzElRadius

  \return nofFailedTests -- Number of failed tests within this function
*/
int test_Cartesian2Other ()
{
  std::cout << "\n[test_Cartesian2Other]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);
  vector<double> cartesian (3);
  vector<double> other (3);

  cartesian[2] = 1.0;

  std::cout << "[1] Cartesian (x,y,z) -> Spherical (r,phi,theta)" << std::endl;
  try {
    cartesian[0] = 1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2Spherical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = 0.0;
    cartesian[1] = 1.0; 
    status = CR::Cartesian2Spherical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = -1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2Spherical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
    //
    cartesian[0] = 0.0;
    cartesian[1] = -1.0; 
    status = CR::Cartesian2Spherical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Cartesian (x,y,z) -> Cylindrical (r,phi,z)" << std::endl;
  try {
    cartesian[0] = 1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2Cylindrical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = 0.0;
    cartesian[1] = 1.0; 
    status = CR::Cartesian2Cylindrical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = -1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2Cylindrical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
    //
    cartesian[0] = 0.0;
    cartesian[1] = -1.0; 
    status = CR::Cartesian2Cylindrical (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Cartesian (x,y,z) -> AzElHeight (Az,El,H)" << std::endl;
  try {
    cartesian[0] = 1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2AzElHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = 0.0;
    cartesian[1] = 1.0; 
    status = CR::Cartesian2AzElHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = -1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2AzElHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
    //
    cartesian[0] = 0.0;
    cartesian[1] = -1.0; 
    status = CR::Cartesian2AzElHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] Cartesian (x,y,z) -> AzElRadius (Az,El,R)" << std::endl;
  try {
    cartesian[0] = 1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2AzElRadius (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = 0.0;
    cartesian[1] = 1.0; 
    status = CR::Cartesian2AzElRadius (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = -1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2AzElRadius (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
    //
    cartesian[0] = 0.0;
    cartesian[1] = -1.0; 
    status = CR::Cartesian2AzElRadius (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[5] Cartesian (x,y,z) -> North-East-Height" << std::endl;
  try {
    cartesian[0] = 1.0;
    cartesian[1] = 0.0;
    status = CR::Cartesian2NorthEastHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = 0.0;
    cartesian[1] = 1.0; 
    status = CR::Cartesian2NorthEastHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);

    cartesian[0] = -1.0;
    cartesian[1] = 0.0; 
    status = CR::Cartesian2NorthEastHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
    //
    cartesian[0] = 0.0;
    cartesian[1] = -1.0; 
    status = CR::Cartesian2NorthEastHeight (other,
				      cartesian,
				      true);
    show_conversion (cartesian,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from cylindrical to other coordinates

  This will run a number of very basic tests for the following routines:
  - CR::Cylindrical2Cartesian
  - CR::Cylindrical2Spherical
  - CR::Cylindrical2AzElHeight
  - CR::Cylindrical2AzElRadius

  \return nofFailedTests -- Number of failed tests within this function
*/
int test_Cylindrical2Other ()
{
  std::cout << "\n[test_Cylindrical2Other]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);
  vector<double> cylindrical (3);
  vector<double> other (3);

  cylindrical[0] = 1.0;
  cylindrical[2] = 0.0;

  std::cout << "[1] Cylindrical (rho,phi,h) -> Cartesian (x,y,z)" << std::endl;
  try {
    cylindrical[1] = 0.0;
    status = CR::Cylindrical2Cartesian (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 90.0;
    status = CR::Cylindrical2Cartesian (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 180;
    status = CR::Cylindrical2Cartesian (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);
    //
    cylindrical[1] = 270;
    status = CR::Cylindrical2Cartesian (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Cylindrical (rho,phi,h) -> Spherical (r,phi,theta)" << std::endl;
  try {
    cylindrical[1] = 0.0;
    status = CR::Cylindrical2Spherical (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 90.0;
    status = CR::Cylindrical2Spherical (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 180.0;
    status = CR::Cylindrical2Spherical (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);
    //
    cylindrical[1] = 270.0;
    status = CR::Cylindrical2Spherical (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Cylindrical (rho,phi,h) -> Az-El-Height" << std::endl;
  try {
    cylindrical[1] = 0.0;
    status = CR::Cylindrical2AzElHeight (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 90.0;
    status = CR::Cylindrical2AzElHeight (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 180;
    status = CR::Cylindrical2AzElHeight (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);
    //
    cylindrical[1] = 270;
    status = CR::Cylindrical2AzElHeight (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] Cylindrical (rho,phi,h) -> Az-El-Radius" << std::endl;
  try {
    cylindrical[1] = 0.0;
    status = CR::Cylindrical2AzElRadius (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 90.0;
    status = CR::Cylindrical2AzElRadius (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);

    cylindrical[1] = 180;
    status = CR::Cylindrical2AzElRadius (other,
				      cylindrical,
				      true);
    show_conversion (cylindrical,other);
    //
    cylindrical[1] = 270;
    status = CR::Cylindrical2AzElRadius (other,
					cylindrical,
					true);
    show_conversion (cylindrical,other);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_Cylindrical2Cartesian ()
{
  std::cout << "\n[test_Cylindrical2Cartesian]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);

  std::cout << "-- Passing of atomic parameters ..." << std::endl;
  try {
    double rho (1.0);
    double phi (0.0);
    double z_cyl (1.0);
    double x;
    double y;
    double z;
    // angles in radian
    status = CR::Cylindrical2Cartesian (x,
					y,
					z,
					rho,
					phi,
					z_cyl,
					false);
    if (status) {
      show_conversion (rho,
		       phi,
		       z_cyl,
		       x,
		       y,
		       z);
    }
    // angles in degrees
    status = CR::Cylindrical2Cartesian (x,
					y,
					z,
					rho,
					phi,
					z_cyl,
					true);
    if (status) {
      show_conversion (rho,
		       phi,
		       z_cyl,
		       x,
		       y,
		       z);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "-- Passing of STDL vectors ..." << std::endl;
  try {
    std::vector<double> cylindrical (3);
    std::vector<double> cartesian (3);
    //
    cylindrical[0] = 1.0;
    cylindrical[1] = 0.0;
    cylindrical[2] = 1.0;
    //
    status = CR::Cylindrical2Cartesian (cartesian,
					cylindrical,
					false);
    if (status) {
      show_conversion (cylindrical,
		       cartesian);
    }
    //
    status = CR::Cylindrical2Cartesian (cartesian,
					cylindrical,
					true);
    if (status) {
      show_conversion (cylindrical,
		       cartesian);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_Cylindrical2Spherical ()
{
  int nofFailedTests (0);

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_angleConversion ();

  nofFailedTests += test_Cartesian2Other ();
  nofFailedTests += test_Cylindrical2Other ();

  nofFailedTests += test_vectorConversion ();

  nofFailedTests += test_Cylindrical2Cartesian ();
  nofFailedTests += test_Cylindrical2Spherical ();
  
  return nofFailedTests;
}
