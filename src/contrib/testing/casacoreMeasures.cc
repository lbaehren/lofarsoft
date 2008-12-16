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
using casa::MVDirection;
using casa::MVPosition;
using casa::Quantity;

/*!
  \file casacoreMeasures.cc

  \ingroup contrib

  \brief A number of tests and exercises for working with Measures objects

  \author Lars B&auml;hren
*/

//_______________________________________________________________________________
//                                                                test_MVPosition

/*!
  \brief Test the various ways to construct a Measure of type MVPosition

  \return nofFailedTests -- The number of failed test encountered within this
          function.
*/
int test_MVPosition ()
{
  cout << "\n[casacoreMeasures::test_MVPosition]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Default constructor ..." << endl;
  try {
    casa::MVPosition pos;
    //
    cout << "-- MVPosition() = " << pos << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Simple argumented constructor ..." << endl;
  try {
    casa::MVPosition pos1 (1,2,3);
    casa::MVPosition pos2 (pos1);
    casa::MVPosition pos3 (casa::Quantity(1,"m"),
			   casa::Quantity(45,"deg"),
			   casa::Quantity(45,"deg"));
    casa::MVPosition pos_x (casa::Quantity(1,"m"),
			    casa::Quantity(0,"deg"),
			    casa::Quantity(0,"deg"));
    casa::MVPosition pos_y (casa::Quantity(1,"m"),
			    casa::Quantity(90,"deg"),
			    casa::Quantity(0,"deg"));
    casa::MVPosition pos_z (casa::Quantity(1,"m"),
			    casa::Quantity(0,"deg"),
			    casa::Quantity(90,"deg"));
    //
    cout << "-- MVPosition(double,double,double)    = " << pos1  << endl;
    cout << "-- MVPosition(MVPosition)              = " << pos2  << endl;
    cout << "-- MVPosition([1,m],[45,deg],[45,deg]) = " << pos3  << endl;
    cout << "-- MVPosition([1,m],[0,deg],[0,deg])   = " << pos_x << endl;
    cout << "-- MVPosition([1,m],[90,deg],[0,deg])  = " << pos_y << endl;
    cout << "-- MVPosition([1,m],[0,deg],[90,deg])  = " << pos_z << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Mathematical operation on/using MVPosition ..." << endl;
  try {
    casa::MVPosition pos1 (1,1,1);
    casa::MVPosition pos2 (1,2,3);
    //
    casa::MVPosition posSum = pos1+pos2;
    //
    cout << "-- pos1      = " << pos1   << endl;
    cout << "-- pos2      = " << pos2   << endl;
    cout << "-- pos1+pos2 = " << posSum << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Test conversion between units ..." << endl;
  try {
    casa::MVPosition pos1 (1,1,1);
    casa::MVPosition pos2 (1,1,0);
    casa::MVPosition pos3 (1,0,1);
    casa::MVPosition pos4 (0,1,1);
    //
    cout << "-- pos1 = " << pos1 << " = " << pos1.getAngle() << endl;
    cout << "-- pos2 = " << pos2 << " = " << pos2.getAngle() << endl;
    cout << "-- pos3 = " << pos3 << " = " << pos3.getAngle() << endl;
    cout << "-- pos4 = " << pos4 << " = " << pos4.getAngle() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}
  
//_______________________________________________________________________________
//                                                                 test_MPosition

/*!
  \brief Test the various ways to construct a Measure of type MPosition

  \return nofFailedTests -- The number of failed test encountered within this
          function.
*/
int test_MPosition ()
{
  cout << "\n[casacoreMeasures::test_MPosition]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Default constructor..." << endl;
  try {
    casa::MPosition pos;
    //
    cout << "-- MPosition()  = " << pos << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                test_MDirection

/*!
  \brief Test the various ways to construct a Measure of type MDirection

  \return nofFailedTests -- The number of failed test encountered within this
          function.
*/
int test_MDirection ()
{
  cout << "\n[casacoreMeasures::test_MDirection]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Default constructor ..." << endl;
  try {
    casa::MDirection dir;
    //
    cout << "-- MDirection() = " << dir << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Argumented constructor ..." << endl;
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

  nofFailedTests += test_MVPosition();

  nofFailedTests += test_MPosition();
  nofFailedTests += test_MDirection();

  return nofFailedTests;
}
