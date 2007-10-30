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
  std::cout << "[" << a1 << "," << a2 << "," << a3 << "]";
  std::cout << "  ->  ";
  std::cout << "[" << b1 << "," << b2 << "," << b3 << "]";
  std::cout << std::endl;
}

void show_conversion (std::vector<double> const &a,
		      std::vector<double> const &b)
{
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
  \brief Test conversion from cartesian to cylindrical coordinates

  \return nofFailedTests -- Number of failed tests within this function
*/
int test_cartesian2cylindrical ()
{
  std::cout << "\n[test_cartesian2cylindrical]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);

  std::cout << "-- Passing of atomic parameters ..." << std::endl;
  try {
    double x (1.0);
    double y (1.0);
    double z (1.0);
    double cyl_rho;
    double cyl_phi;
    double cyl_z;
    // angles in radian
    status = CR::cartesian2cylindrical (cyl_rho,
					cyl_phi,
					cyl_z,
					x,
					y,
					z,
					false);
    if (status) {
      show_conversion (x,
		       y,
		       z,
		       cyl_rho,
		       cyl_phi,
		       cyl_z);
    }
    // angles in degrees
    status = CR::cartesian2cylindrical (cyl_rho,
					cyl_phi,
					cyl_z,
					x,
					y,
					z,
					true);
    if (status) {
      show_conversion (x,
		       y,
		       z,
		       cyl_rho,
		       cyl_phi,
		       cyl_z);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "-- Passing of STDL vectors ..." << std::endl;
  try {
    std::vector<double> cartesian(3);
    std::vector<double> cylindrical (3);
    //
    cartesian[0] = 1.0;
    cartesian[1] = 1.0;
    cartesian[2] = 1.0;
    // angles in radian
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					false);
    if (status) {
      show_conversion (cartesian,
		       cylindrical);
    }
    // angles in degrees
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					true);
    if (status) {
      show_conversion (cartesian,
		       cylindrical);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

#ifdef HAVE_CASA
  std::cout << "-- Passing of CASA vectors ..." << std::endl;
  try {
    casa::Vector<double> cartesian(3);
    casa::Vector<double> cylindrical (3);
    //
    cartesian = 1.0;
    // angles in radian
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					false);
    if (status) {
      std::cout << cartesian << "  ->  " << cylindrical << std::endl;
    }
    // angles in degrees
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					true);
    if (status) {
      std::cout << cartesian << "  ->  " << cylindrical << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif

#ifdef HAVE_BLITZ
  std::cout << "-- Passing of Blitz++ vectors ..." << std::endl;
  try {
    blitz::Array<double,1> cartesian(3);
    blitz::Array<double,1> cylindrical (3);
    //
    cartesian = 1.0,1.0,1.0;
    // angles in radian
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					false);
    if (status) {
      show_conversion (cartesian,
		       cylindrical);
    }
    // angles in degrees
    status = CR::cartesian2cylindrical (cylindrical,
					cartesian,
					true);
    if (status) {
      show_conversion (cartesian,
		       cylindrical);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from cartesian to spherical coordinates

  \return nofFailedTests -- Number of failed tests within this function
*/
int test_cartesian2spherical ()
{
  std::cout << "\n[test_cartesian2spherical]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);

  std::cout << "-- Passing of atomic parameters ..." << std::endl;
  try {
    double x (1.0);
    double y (1.0);
    double z (1.0);
    double r;
    double phi;
    double theta;
    // angles in radian
    status = CR::cartesian2spherical (r,
				      phi,
				      theta,
				      x,
				      y,
				      z,
				      false);
    if (status) {
      show_conversion (x,
		       y,
		       z,
		       r,
		       phi,
		       theta);
    }
    // angles in degrees
    status = CR::cartesian2spherical (r,
				      phi,
				      theta,
				      x,
				      y,
				      z,
				      true);
    if (status) {
      show_conversion (x,
		       y,
		       z,
		       r,
		       phi,
		       theta);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "-- Passing of STDL vectors ..." << std::endl;
  try {
    std::vector<double> cartesian(3);
    std::vector<double> spherical (3);
    //
    cartesian[0] = 1.0;
    cartesian[1] = 1.0;
    cartesian[2] = 1.0;
    // angles in radian
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      false);
    if (status) {
      show_conversion (cartesian,
		       spherical);
    }
    // angles in degrees
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      true);
    if (status) {
      show_conversion (cartesian,
		       spherical);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
#ifdef HAVE_CASA
  std::cout << "-- Passing of CASA vectors ..." << std::endl;
  try {
    casa::Vector<double> cartesian(3);
    casa::Vector<double> spherical (3);
    //
    cartesian = 1.0;
    // angles in radian
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      false);
    if (status) {
      std::cout << cartesian << "  ->  " << spherical << std::endl;
    }
    // angles in degrees
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      true);
    if (status) {
      std::cout << cartesian << "  ->  " << spherical << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
#ifdef HAVE_BLITZ
  std::cout << "-- Passing of Blitz++ vectors ..." << std::endl;
  try {
    blitz::Array<double,1> cartesian(3);
    blitz::Array<double,1> spherical (3);
    //
    cartesian = 1.0,1.0,1.0;
    // angles in radian
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      false);
    if (status) {
      show_conversion (cartesian,
		       spherical);
    }
    // angles in degrees
    status = CR::cartesian2spherical (spherical,
				      cartesian,
				      true);
    if (status) {
      show_conversion (cartesian,
		       spherical);
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_cylindrical2cartesian ()
{
  std::cout << "\n[test_cylindrical2cartesian]\n" << std::endl;

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
    status = CR::cylindrical2cartesian (x,
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
    status = CR::cylindrical2cartesian (x,
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
    status = CR::cylindrical2cartesian (cartesian,
					cylindrical,
					false);
    if (status) {
      show_conversion (cylindrical,
		       cartesian);
    }
    //
    status = CR::cylindrical2cartesian (cartesian,
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

int test_cylindrical2spherical ()
{
  int nofFailedTests (0);

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_angleConversion ();

  nofFailedTests += test_cartesian2cylindrical ();
  nofFailedTests += test_cartesian2spherical ();

  nofFailedTests += test_cylindrical2cartesian ();
  nofFailedTests += test_cylindrical2spherical ();

  return nofFailedTests;
}
