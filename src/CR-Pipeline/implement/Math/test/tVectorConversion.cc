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

/*!
  \file tVectorConversion.cc

  \ingroup Math

  \brief A collection of test routines for VectorConversion
 
  \author Lars B&auml;hren
 
  \date 2007/05/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test routines for conversion of angles

  \return nofFailedTests -- The number of failed tests.
*/
int test_angleConversion ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_VectorConversion]\n" << std::endl;

  std::cout << "[1] Testing conversion of individual values ..." << std::endl;
  try {
    double angleInDegree (0.0);
    double angleInRadian (0);

    for (unsigned int n(0); n<18; n++) {
      angleInDegree = n*10.0;
      angleInRadian = CR::deg2rad (angleInDegree);
      std::cout << "\t" << angleInDegree
		<< "\t" << angleInRadian
		<< "\t" << CR::rad2deg (angleInRadian)
		<< std::endl;
    }
  } catch (std::string err) {
    std::cerr << err << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_angleConversion ();
  }

  return nofFailedTests;
}
