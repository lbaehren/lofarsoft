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
  
  std::cout << "[1] Testing MVAngle..." << std::endl;
  try {
    MVAngle mva1 = Quantity(190,"deg") + Quantity(59,"'") + Quantity(59.95,"\"");
    std::cout << " - Degrees:  " << mva1.degree()   << std::endl;
    std::cout << " - Radians:  " << mva1.radian()   << std::endl;
    std::cout << " - Fraction: " << mva1.circle()   << std::endl;
    std::cout << " - Degrees:  " << mva1().degree() << std::endl;
    std::cout << " - Radians:  " << mva1().radian() << std::endl;
    std::cout << " - Fraction: " << mva1().circle() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing MVTime..." << std::endl;
  try {
    MVTime mtim = 44362.6;
    
    std::cout << " - Days:       " << mtim.day()    << std::endl;    
    std::cout << " - Hours:      " << mtim.hour()   << std::endl;    
    std::cout << " - Seconds:    " << mtim.second() << std::endl;    
    std::cout << " - Minutes:    " << mtim.minute() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing MEpoch..." << std::endl;
  try {
    MEpoch ep(Quantity(50083.,"d"));
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing MDirection..." << std::endl;
  try {
    // Direction in J2000 coordinate-frame
    MDirection j2000 (Quantity(0,"deg"),
		      Quantity(30,"deg"),
		      MDirection::J2000);
    // Direction in B1900 coordinate-frame
    MeasFrame b1900((MEpoch(Quantity(MeasData::MJDB1900,"d"))));
    MDirection lsr1900(Quantity(270,"deg"),
		       Quantity(30,"deg"),
		       MDirection::Ref(MDirection::BMEAN,
				       b1900));
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
