/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

/* $Id$ */

#include <iostream>
#include <sstream>

#include <Data/Data.h>

using namespace std;

using CR::Data;

/*!
  \file tData.cc

  \ingroup CR_Data
  
  \brief A collection of test routines for Data

  \author Sven Lafebre, Lars B&auml;hren
*/

// -------------------------------------------------------------------- test_Data

/*!
  \brief Test constructors for a new Data object
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_Data ()
{
  std::cout << "\n[test_Data]\n" << std::endl;
  
  int nofFailedTests (0);
  int max (10);

  std::cout << "[1] Creating new Data object via default constructor ..." << std::endl;
  try {
    Data data1;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Creating single new Data object ..." << std::endl;
  try {
    Data data2 (50, 1000);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Creating multiple new Data objects ..." << std::endl;
  try {
    uint maxLength (20);
    uint nofPoints (0);

    for (uint n (1); n<maxLength; n++) {
      nofPoints = n*1e6;
      std::cout << " --> length(data) = " << n
		<< " = " << nofPoints*sizeof(short) << " bytes" << std::endl;
      // create a new object
      Data d (nofPoints, 1100);
      // write a value into the first element
      d[0] = 1;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
//   std::cout << "[4] Creating multiple new Data objects for manipulation ..." << std::endl;
//   try {
//     Data d(50, 1200);
//     Data e(50, 1201);
//     Data f((uint)1e8, 1);

//     for (int a(0); a<max; a++) {
//       d[2] = 10;
//       e[2] = 12;
//       f = d;
//       cout << "d: " << d.id() << ", e: " << e.id() << ", f: " << f.id() << endl;
//     }
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }
  
  return nofFailedTests;
}

// ----------------------------------------------------------------- main routine

int main ()
{
  int nofFailedTests (0);
  
  nofFailedTests += test_Data ();
  
  return nofFailedTests;
}
