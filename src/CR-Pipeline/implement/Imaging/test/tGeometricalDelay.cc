/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <iostream>

#include <Imaging/GeometricalDelay.h>

using blitz::Array;
using blitz::Range;
using CR::GeometricalDelay;
using CR::L2Norm;

const double lightspeed = 2.99795e08;

/*!
  \file tGeometricalDelay.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalDelay
 
  \author Lars Baehren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Fundamental testing on the different formulae for delay computation

  \return nofFailedTests -- The number of failed tests.
*/
int test_formula ()
{
  std::cout << "\n[test_formula]\n" << std::endl;

  int nofFailedTests (0);

  int nofCoordinates (3);
  Array<double,1> skyPositions (nofCoordinates);
  Array<double,2> antPositions (2,nofCoordinates);
  double delay (.0);

  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 100.0, 100.0;

  std::cout << "sky positions = " << skyPositions  << std::endl;
  std::cout << "ant positions = " << antPositions  << std::endl;

  /*!
    Standard version for the computation of the delay
   */
  std::cout << "[1] Far-field geometry" << std::endl;
  try {
    double scalarProduct (.0);

    for (int n(0); n<nofCoordinates; n++) {
      scalarProduct += (antPositions(1,n)-antPositions(0,n))*skyPositions(n);
    }
    delay = scalarProduct/lightspeed;
    
    std::cout << "delay(FF)     = " << delay         << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  /*!
    Computation of the delay based on the full 3-dim geometry
  */
  std::cout << "\n[2] Compute delay for full geometry\n" << std::endl;
  try {
    Array<double,1> diff0 (nofCoordinates);
    Array<double,1> diff1 (nofCoordinates);
    
    for (int n(0); n<nofCoordinates; n++) {
      diff0(n) = skyPositions(n)-antPositions(0,n);
      diff1(n) = skyPositions(n)-antPositions(1,n);
    }
    delay = (L2Norm(diff1)-L2Norm(diff0))/lightspeed;
    
    std::cout << "delay(NF)     = " << delay         << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  /*
    Variation of the source position
  */
  std::cout << "\n[3] Variation of the source position\n" << std::endl;
  try {
    double zmin (100.0);
    double zmax (1000.0);
    double zstep (100.0);
    Array<double,1> diff0 (nofCoordinates);
    Array<double,1> diff1 (nofCoordinates);

    // go through the distance steps
    for (double z(zmin); z<=zmax; z+=zstep) {
      // set the z-coordinate of the sky position
      skyPositions(2) = z;
      // compute the geometrical delay
      for (int n(0); n<nofCoordinates; n++) {
	diff0(n) = skyPositions(n)-antPositions(0,n);
	diff1(n) = skyPositions(n)-antPositions(1,n);
      }
      delay = (L2Norm(diff1)-L2Norm(diff0))/lightspeed;
      // dispay the result
      std::cout << "\t" << z << "\t" << delay << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalDelay object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalDelay ()
{
  std::cout << "\n[test_GeometricalDelay]\n" << std::endl;

  int nofFailedTests (0);
  uint nofCoordinates (3);
  Array<double,2> skyPositions (1,nofCoordinates);
  Array<double,2> antPositions (2,nofCoordinates);
  
  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 100.0, 100.0;
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalDelay delay;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_formula ();
  }

  // Test for the constructor(s)
  {
    nofFailedTests += test_GeometricalDelay ();
  }

  return nofFailedTests;
}
