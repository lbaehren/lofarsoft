/*-------------------------------------------------------------------------*
 | $Id:: tUseFFTW3.cc 613 2007-08-23 09:22:15Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <iostream>
#include <string>

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_sf_exp.h>

/*!
  \file tUseGSL.cc

  \ingroup CR-Pipeline

  \brief A simple test for working with the GNU Scientific Library (GSL)

  \author Lars B&auml;hren

  \date 2007/09/06
*/

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  
  double x (0.);
  int nofSamplePoints (20);
  double incr (0.25);
  
  // --------------------------------------------------------------------- Test 1
  
  std::cout << "[1] GSL :: Special functions :: Bessel" << std::endl;
  try {
    double y0 (0.);
    double y1 (0.);

    for (int n(0); n<nofSamplePoints; n++) {
      x = n*incr;
      y0 = gsl_sf_bessel_J0 (x);
      y1 = gsl_sf_bessel_J1 (x);
      std::cout << "\t" << x << "\t" << y0 << "\t" << y1 << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  // --------------------------------------------------------------------- Test 2
  
  std::cout << "[2] GSL :: Special functions :: Exponential" << std::endl;
  try {
    double y0 (0.);
    double y1 (0.);

    for (int n(0); n<nofSamplePoints; n++) {
      x = n*incr;
      y0 = gsl_sf_exp (x);
      y1 = gsl_sf_expm1 (x);
      std::cout << "\t" << x << "\t" << y0 << "\t" << y1 << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  // --------------------------------------------------------------------- Test 3
  
//   try {
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }
  
  return nofFailedTests;
}
