/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <IO/H5ImportConfig.h>

using CR::H5ImportConfig;  // Namespace usage

/*!
  \file tH5ImportConfig.cc

  \ingroup IO

  \brief A collection of test routines for the H5ImportConfig class
 
  \author Lars Baehren
 
  \date 2007/09/18
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new H5ImportConfig object

  \return nofFailedTests -- The number of failed tests.
*/
int test_H5ImportConfig ()
{
  int nofFailedTests (0);
  int rank (3);
  int *dimensions;
  
  std::cout << "\n[test_H5ImportConfig]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    H5ImportConfig newH5ImportConfig;
    //
    newH5ImportConfig.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simple argumented constructor ..." << std::endl;
  try {

    dimensions = new int [rank];
    for (int n(0); n<rank; n++) {
      dimensions[n] = (n+1)*10;
    }

    H5ImportConfig newH5ImportConfig (rank,dimensions);
    //
    newH5ImportConfig.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
    nofFailedTests += test_H5ImportConfig ();
  }

  return nofFailedTests;
}
