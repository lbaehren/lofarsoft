/*-------------------------------------------------------------------------*
 | $Id:: TimeFreqSkymap.cc 1967 2008-09-09 13:16:22Z baehren             $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::MDirection;
using casa::MPosition;
using casa::Quantity;

/*!
  \file casacoreMeasuresConversion.cc

  \ingroup contrib

  \brief Test conversion between Measures objects

  \author Lars B&auml;hren
*/

//_______________________________________________________________________________
//                                                              test_constructors

/*!
  \brief Test the various ways to construct a casa::Measure object

  \return nofFailedTests -- The number of failed test encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[casacoreMeasuresConversion::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Default constructors ..." << endl;
  try {
    casa::MPosition pos;
    casa::MDirection dir;
    //
    cout << "-- MPosition()  = " << pos << endl;
    cout << "-- MDirection() = " << dir << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Argumented constructor for MDirection ..." << endl;
  try {
    MDirection azel (MDirection(Quantity(0,"deg"),
				Quantity(90,"deg"),
				MDirection::Ref(MDirection::AZEL)));
    MDirection b1950 (MDirection(Quantity(0,"deg"),
				 Quantity(90,"deg"),
				 MDirection::Ref(MDirection::B1950)));
    MDirection j2000 (MDirection(Quantity(0,"deg"),
				 Quantity(90,"deg"),
				 MDirection::Ref(MDirection::J2000)));
    //
    cout << "-- MDirection ( AZEL) = " << azel << endl;
    cout << "-- MDirection (B1950) = " << b1950 << endl;
    cout << "-- MDirection (J2000) = " << j2000 << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   main routine

int main (int argc, char* argv[])
{
  int nofFailedTests (0);

  nofFailedTests += test_constructors();

  return nofFailedTests;
}
