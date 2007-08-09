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
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasFrame.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MeasData.h>

/*!
  \file tUseMeasures.cc
  
  \ingroup CR-Pipeline

  \brief A simple test for external build against libmeasures

  \author Lars B&auml;hren

  \date 2007/01/26
*/

using std::cout;
using std::endl;

using casa::MEpoch;
using casa::MVAngle;
using casa::Quantity;
using casa::MVTime;
using casa::MeasFrame;
using casa::MDirection;
using casa::MeasData;

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  
  cout << "[1] Testing MVAngle..." << std::endl;
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
  
  cout << "[2] Testing MVTime..." << std::endl;
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
  
  cout << "[3] Testing MEpoch..." << std::endl;
  try {
    MEpoch today;
    MEpoch ep (Quantity(50083.,"d"));
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

  cout << "[5] Testing conversion of direction between frames..." << endl;
  try {
    casa::MDirection::Ref refB1950 (casa::MDirection::B1950);
    casa::MDirection::Ref refJ2000 (casa::MDirection::J2000);

    casa::MDirection::Convert conv (refB1950,
				    refJ2000);
    
    cout << "-- Reference for source frame  = " << refB1950    << endl;
    cout << "-- Reference for target frame  = " << refJ2000    << endl;
    cout << "-- Coordinate conversion       = " << conv        << endl;

    // testwise conversion of some coordinates
    casa::MVDirection coordB1950;

    for (int elevation (0); elevation<90; elevation+=10) {
      coordB1950 = casa::MVDirection (casa::Quantity(0,"deg"),
				      casa::Quantity(elevation,"deg"));
      cout << "\t" << elevation
	   << "\t" << coordB1950
	   << "\t" << conv(coordB1950).getValue().getRecordValue()
	   << endl;
    }

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
