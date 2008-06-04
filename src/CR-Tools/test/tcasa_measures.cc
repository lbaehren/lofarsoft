/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <iostream>
#include <string>

#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <measures/Measures.h>
#include <casa/Quanta/MVAngle.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/MVPosition.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasFrame.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasData.h>

/*!
  \file tcasa_measures.cc
  
  \ingroup CR_test

  \brief A simple test for external build against libmeasures

  \author Lars B&auml;hren

  \date 2007/01/26
*/

using std::cout;
using std::endl;

using casa::MVAngle;
using casa::Quantity;
using casa::MVTime;
using casa::MeasFrame;
using casa::MDirection;
using casa::MeasData;
using casa::MPosition;
using casa::MVPosition;

// ------------------------------------------------------------------------------

/*!
  \brief Test working with a MVAngle measure

  \return nofFailedTest -- The number of failed tests within this function
*/
int test_MVAngle ()
{
  cout << "\n[test_MVAngle]\n" << endl;

  int nofFailedTests (0);
  
  try {
    MVAngle mva1 = Quantity(190,"deg") + Quantity(59,"'") + Quantity(59.95,"\"");
    cout << " - Degrees:  " << mva1.degree()   << std::endl;
    cout << " - Radians:  " << mva1.radian()   << std::endl;
    cout << " - Fraction: " << mva1.circle()   << std::endl;
    cout << " - Degrees:  " << mva1().degree() << std::endl;
    cout << " - Radians:  " << mva1().radian() << std::endl;
    cout << " - Fraction: " << mva1().circle() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \return nofFailedTests -- The number of failed tests
*/
int test_MVTime ()
{
  cout << "\n[test_MVTime]\n" << endl;

  int nofFailedTests (0);

  try {
    MVTime mtim = 44362.6;
    
    cout << " - Days:       " << mtim.day()    << std::endl;    
    cout << " - Hours:      " << mtim.hour()   << std::endl;    
    cout << " - Seconds:    " << mtim.second() << std::endl;    
    cout << " - Minutes:    " << mtim.minute() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \return nofFailedTests -- The number of failed tests
*/
int test_MEpoch ()
{
  cout << "\n[test_MEpoch]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::MEpoch today;
    casa::MEpoch ep (Quantity(50083.,"d"));
    cout << "-- Default epoch        = " << today << endl;
    cout << "-- User specified epoch = " << ep    << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing MDirection..." << std::endl;
  try {
    // Zenith in J2000 coordinates
    MDirection j2000 (Quantity(0,"deg"),
		      Quantity(90,"deg"),
		      MDirection::J2000);
    // Zenith in GALACTIC coordinates
    MDirection galactic (Quantity(0,"deg"),
			 Quantity(90,"deg"),
			 MDirection::GALACTIC);
    // Zenith in AZEL coordinates
    MDirection azel (Quantity(0,"deg"),
		     Quantity(90,"deg"),
		     MDirection::AZEL);
    
    cout << "-- J2000 zenith    = " << j2000    << endl;
    cout << "-- Galactic zenith = " << galactic << endl;
    cout << "-- AZEL zenith     = " << azel     << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \return nofFailedTests -- The number of failed tests
*/
int test_MPosition ()
{
  cout << "\n[test_MPosition]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Basic example from API documentation ..." << endl;
  try {
    MPosition obs( MVPosition( Quantity( 10, "m"),
			       Quantity( -6, "deg"),
			       Quantity( 50, "deg")),
		   MPosition::Ref(MPosition::WGS84));
    //
    cout << "-- Reference string = " << obs.getRefString() << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  nofFailedTests += test_MVAngle ();
  nofFailedTests += test_MVTime ();
  nofFailedTests += test_MEpoch ();
  nofFailedTests += test_MPosition ();
  
  return nofFailedTests;
}
