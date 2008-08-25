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
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MeasFrame.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasData.h>
#include <measures/Measures/MeasFrame.h>

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
  \brief Test of class to handle angle type conversions and I/O

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
  \brief Test of class for high precision time

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
  \brief Test of measure for an instance in time

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
  \brief Test of measure storing wave characteristics.

  \return nofFailedTests -- The number of failed tests
*/
int test_MFrequency ()
{
  cout << "\n[test_MFrequency]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] An oberved HI frequency of 1380 MHz ..." << endl;
  try {
    casa::MFrequency freq ( casa::Quantity(1380., "MHz"),
			    casa::MFrequency::TOPO);
    //
    cout << "-- MFrequency          = " << freq << endl;
    cout << "-- Type as register    = " << freq.myType() << endl;
    cout << "-- Type reference code = " << freq.showType (freq.myType()) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Construct from Quantity ..." << endl;
  try {
    double freqValue            = 1380;
    casa::String freqUnit       = "MHz";
    casa::Quantity freqQuantity = casa::Quantity (freqValue,freqUnit);
    casa::MFrequency freq       = casa::MFrequency (freqQuantity,
						    casa::MFrequency::TOPO);
    //
    cout << "-- Quantity            = " << freqQuantity << endl;
    cout << "-- MFrequency          = " << freq         << endl;
    cout << "-- Type as register    = " << freq.myType() << endl;
    cout << "-- Type reference code = " << freq.showType (freq.myType()) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test of measure for a position on Earth

  - Add two positions given in the reference frame.
  - Add two positions given in different reference frames.

  \return nofFailedTests -- The number of failed tests
*/
int test_MPosition ()
{
  cout << "\n[test_MPosition]\n" << endl;

  int nofFailedTests (0);
  bool status (true);

  cout << "[1] Conversion of ref. code to type ..." << endl;
  try {
    casa::String refcode ("ITRF");
    casa::MPosition::Types tp;
    //
    status = casa::MPosition::getType (tp,refcode);
    if (status) {
      cout << "-- Ref. code = " << refcode << endl;
      cout << "-- Type      = " << tp      << endl;
    }
    //
    refcode = "WGS84";
    status = casa::MPosition::getType (tp,refcode);
    if (status) {
      cout << "-- Ref. code = " << refcode << endl;
      cout << "-- Type      = " << tp      << endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

#ifndef AIPS_GCC3
  cout << "[2] Basic example from API documentation ..." << endl;
  try {
    MVPosition mvpos ( Quantity( 10, "m"),
		       Quantity( -6, "deg"),
		       Quantity( 50, "deg"));
    MPosition obs( mvpos,
		   MPosition::Ref(MPosition::WGS84));
    //
    cout << "-- Reference string = " << obs.getRefString() << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Construction of MPosition as in DAL::HDF5Common ..." << endl;
  try {
    casa::Vector<double> values (3);
    casa::Vector<casa::String> units (3);
    casa::MPosition::Types tp;
    string refcode;
    //
    values(0) = 10;
    values(1) = -6;
    values(2) = 50;
    units(0)  = "m";
    units(1)  = "deg";
    units(2)  = "deg";
    refcode   = "WGS84";
    //
    status = casa::MPosition::getType (tp,refcode);
    //
    MVPosition mvpos ( Quantity( values(0), units(0)),
		       Quantity( values(1), units(1)),
		       Quantity( values(2), units(2)));
    MPosition obs( mvpos,
		   MPosition::Ref(tp));
    //
    cout << "-- Values    = " << values  << endl;
    cout << "-- Units     = " << units   << endl;
    cout << "-- Ref. code = " << refcode << endl;
    cout << "-- Ref. type = " << tp      << endl;
    cout << "-- MPosition = " << obs     << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test of measure for a direction in the sky

  \return nofFailedTests -- The number of failed tests
*/
int test_MDirection ()
{
  cout << "\n[test_MDirection]\n" << endl;
  
  int nofFailedTests (0);
  bool status (true);

  cout << "[1] Conversion of ref. code to type ..." << endl;
  try {
    casa::String refcode ("J2000");
    casa::MDirection::Types tp;
    //
    status = casa::MDirection::getType (tp,refcode);
    if (status) {
      cout << "-- " << refcode << "\t= " << tp << endl;
    }
    //
    refcode = "B1950";
    status = casa::MDirection::getType (tp,refcode);
    if (status) {
      cout << "-- " << refcode << "\t= " << tp << endl;
    }
    //
    refcode = "GALACTIC";
    status = casa::MDirection::getType (tp,refcode);
    if (status) {
      cout << "-- " << refcode << "\t= " << tp << endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[2] Basic example from API documentation ..." << endl;
  try {
    MDirection direction ( Quantity( 20, "deg"),
			   Quantity(-10, "deg"),
			   MDirection::Ref( MDirection::B1950));
    cout << "-- MDirection = " << direction << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

#ifndef AIPS_GCC3
  cout << "[3] Construction of MDirection as in HDF5Common ..." << endl;
  try {
    casa::Vector<double> values (2);
    casa::Vector<casa::String> units (2);
    casa::MDirection::Types tp;
    string refcode;
    //
    values(0) = 20;
    values(1) = -10;
    units(0)  = "deg";
    units(1)  = "deg";
    refcode   = "B1950";
    //
    status = casa::MDirection::getType (tp,refcode);
    //
    MDirection direction ( Quantity( values(0), units(0)),
			   Quantity( values(1), units(1)),
			   MDirection::Ref(tp));
    //
    cout << "-- Values     = " << values  << endl;
    cout << "-- Units      = " << units   << endl;
    cout << "-- Ref. code  = " << refcode << endl;
    cout << "-- Ref. type  = " << tp      << endl;
    cout << "-- MDirection = " << direction << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test the coordinate operation required by the imaging module

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_imagingCoordinates ()
{
  int nofFailedTests (0);

  cout << "[1] Set up the frame with the observatory position ... " << endl;
  casa::MPosition obs ( casa::MVPosition( casa::Quantity( 10, "m"),
					  casa::Quantity( -6, "deg"),
					  casa::Quantity( 50, "deg")),
			casa::MPosition::Ref(casa::MPosition::WGS84));
  casa::MeasFrame frame( obs);

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
  nofFailedTests += test_MFrequency ();
  nofFailedTests += test_MPosition ();
  nofFailedTests += test_MDirection ();
  
  return nofFailedTests;
}
