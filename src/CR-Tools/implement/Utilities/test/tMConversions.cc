/*-------------------------------------------------------------------------*
 | $Id                                                                     |
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

#include <Utilities/MConversions.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::MDirection;
using casa::MVDirection;
using casa::MPosition;
using casa::MVPosition;
using casa::Quantity;

/*!
  \file tMConversions.cc

  \ingroup CR_Utilities

  \brief A collection of tests for the routines implemented in MConversions.cc
 
  \author Lars B&auml;hren
 
  \date 2007/09/07
*/

//_______________________________________________________________________________
//                                                                 test_MeasValue

/*!
  \brief Test working with Measures of type MVPosition and MVDirection

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MeasValue ()
{
  std::cout << "\n[tMConversions::test_MeasValue]\n" << endl;
  
  int nofFailedTests (0);

  try {
    MVDirection dir1;
    MVDirection dir2 (1,2,3);
    MVDirection dir3 (Quantity(0,"deg"),Quantity(90,"deg"));

    cout << "-- MVDirection()      = " << dir1 << endl;
    cout << "                      = " << dir1.get() << endl;
    cout << "-- MVDirection(1,2,3) = " << dir2 << endl;
    cout << "                      = " << dir2.get() << endl;
    cout << "-- MVDirection(0,90)  = " << dir3 << endl;
    cout << "                      = " << dir3.get() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}
  
//_______________________________________________________________________________
//                                                                test_MDirection

/*!
  \brief Test working with Measures of type MDirection

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MDirection ()
{
  std::cout << "\n[tMConversions::test_MDirection]\n" << endl;
  
  int nofFailedTests (0);
  
  std::cout << "[1] Default constructors ..." << endl;
  try {
    MDirection dir;
    //
    std::cout << "-- MDirection() = " << dir << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Argumented constructor for MDirection ..." << endl;
  try {
    MDirection dir;
    //
    dir = MDirection(Quantity(0,"deg"),
		     Quantity(90,"deg"),
		     MDirection::Ref(MDirection::AZEL));
    MDirection azel (dir);
    //
    dir = MDirection(Quantity(0,"deg"),
		     Quantity(90,"deg"),
		     MDirection::Ref(MDirection::B1950));
    MDirection b1950 (dir);
    //
    dir = MDirection(Quantity(0,"deg"),
		     Quantity(90,"deg"),
		     MDirection::Ref(MDirection::J2000));
    MDirection j2000 (dir);
    //
    dir = MDirection(Quantity(0,"deg"),
		     Quantity(90,"deg"),
		     MDirection::Ref(MDirection::ITRF));
    MDirection itrf (dir);
    //
    std::cout << "-- MDirection ( AZEL) = " << azel  << endl;
    std::cout << "-- MDirection (B1950) = " << b1950 << endl;
    std::cout << "-- MDirection (J2000) = " << j2000 << endl;
    std::cout << "-- MDirection ( ITRF) = " << itrf  << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Get MDirection type from the reference code ..." << endl;
  try {
    std::cout << "-- AZEL  -> " << CR::MDirectionType ("AZEL") << endl;
    std::cout << "-- J2000 -> " << CR::MDirectionType ("J2000") << endl;
    std::cout << "-- ITRF  -> " << CR::MDirectionType ("ITRF") << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[4] Get MDirection reference code from the type ..." << endl;
  try {
    std::cout << "-- AZEL  -> " << CR::MDirectionName (MDirection::AZEL) << endl;
    std::cout << "-- J2000 -> " << CR::MDirectionName (MDirection::J2000) << endl;
    std::cout << "-- JTRUE -> " << CR::MDirectionName (MDirection::JTRUE) << endl;
    std::cout << "-- JMEAN -> " << CR::MDirectionName (MDirection::JMEAN) << endl;
    std::cout << "-- ITRF  -> " << CR::MDirectionName (MDirection::ITRF) << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[5] Conversion of MDirection to other  ..." << endl;
  try {
    MDirection dir;
    double ra (0);
    double dec (90);
    //
    dir = MDirection(Quantity(ra,"deg"),
		     Quantity(dec,"deg"),
		     MDirection::Ref(MDirection::J2000));
    MDirection j2000 (dir);
    //
    cout << j2000 << endl;
    //
    cout << casa::MDirection::Convert (j2000,
				       MDirection::Ref(MDirection::B1950))()
	 << endl;
    //
    cout << casa::MDirection::Convert (j2000,
				       MDirection::Ref(MDirection::GALACTIC))()
	 << endl;
    // Set up the model for the input (default reference is UTC)
    casa::MEpoch epoch (  Quantity(0., "d"));
    // Set the telescope position
    MPosition obs(  MVPosition(     Quantity( 10, "m"),
				    Quantity( -6, "deg"),
				    Quantity( 50, "deg")),
		    MPosition::Ref(MPosition::WGS84));
    // Create refrence frame
    casa::MeasFrame frame(epoch,obs);
    //  Create conversion engine
    MDirection::Convert conv (MDirection(MDirection::J2000),
			      MDirection::Ref(MDirection::AZEL,frame));
    // Convert direction from J2000 to AZEL
    cout << "AZEL = " << conv(MVDirection (Quantity(ra,"deg"),
			      Quantity(dec,"deg"))).getValue().getRecordValue() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                 test_MPosition

/*!
  \brief Test working with Measures of type MFrequency
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MFrequency () 
{
  std::cout << "\n[tMConversions::test_MFrequency]\n" << endl;

  int nofFailedTests (0);

  std::cout << "[1] Default constructor ..." << endl;
  try {
    casa::MFrequency freq;
    //
    std::cout << "-- MFrequency()  = " << freq                 << endl;
    std::cout << "                 = " << freq.get("Hz")       << endl;
    std::cout << "                 = " << freq.get("Hz").get() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Construction from Quantity ..." << endl;
  try {
    casa::MFrequency freq (casa::Quantity(200,"MHz"));
    //
    std::cout << "-- MFrequency(Quantity) = " << freq           << endl;
    std::cout << "                        = " << freq.get("Hz") << endl;
    std::cout << "                        = " << freq.get("Hz").get() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }


  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                 test_MPosition

/*!
  \brief Test working with Measures of type MPosition

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MPosition ()
{
  std::cout << "\n[tMConversions::test_MPosition]\n" << endl;

  int nofFailedTests (0);

  std::cout << "[1] Default constructors ..." << endl;
  try {
    casa::MPosition pos;
    //
    std::cout << "-- MPosition()  = " << pos << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Argumented constructor for MPosition ..." << endl;
  try {
    casa::MVPosition mvp;
    //
    mvp = MVPosition(Quantity(25, "m"),       // Height
		     Quantity(20, "deg"),     // East longitude
		     Quantity(53, "deg"));    // Lattitude
    MPosition pos_wgs84 (mvp,
			 MPosition::WGS84);   // Reference type
    // 45 foot diameter antenna, Green Bank, West Virginia
    mvp = MVPosition(884084.2636,             // X
		     -4924578.7481,           // Y
		     3943734.3354);           // Z
    MPosition pos_itrf (mvp,
			MPosition::ITRF);     // Reference type
    //
    std::cout << "-- MPosition (WGS84) = " << pos_wgs84 << endl;
    std::cout << "-- MPosition (ITRF)  = " << pos_itrf  << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                test_operations

/*!
  \brief Test application of mathematical operations to Measures

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_operations ()
{
  std::cout << "\n[tMConversions::test_operations]\n" << endl;

  int nofFailedTests (0);

  std::cout << "[1] Addition of MVPosition (m,deg,deg) ..." << endl;
  try {
    MVPosition pos1 (Quantity(25, "m"),
		     Quantity(20, "deg"),
		     Quantity(53, "deg"));
    MVPosition shift (Quantity(1, "m"),
		     Quantity(1, "deg"),
		     Quantity(1, "deg"));
    MVPosition pos2 = pos1+shift;
    //
    std::cout << "-- Position 1     = " << pos1  << endl;
    std::cout << "-- Position shift = " << shift << endl;
    std::cout << "-- Position 2     = " << pos2  << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[1] Addition of MVPosition (m,m,m) ..." << endl;
  try {
    MVPosition pos1 (25,25,1);
    MVPosition shift (1,1,1);
    MVPosition pos2 = pos1+shift;
    //
    std::cout << "-- Position 1       = " << pos1  << endl;
    std::cout << "-- Position shift   = " << shift << endl;
    std::cout << "-- Position 2       = " << pos2  << endl;
    std::cout << "                    = " << pos2.get() << endl;
    std::cout << "-- (lon,lat,length) = ("
	      << pos2.getLong() << ","
	      << pos2.getLat() << ","
	      << pos2.getLength() << ")"
	      << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                               test_conversions

int test_conversions ()
{
  std::cout << "\n[tMConversions::test_conversions]\n" << endl;

  int nofFailedTests (0);

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   main routine

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_MeasValue();
  nofFailedTests += test_MFrequency();
  nofFailedTests += test_MDirection();
  nofFailedTests += test_MPosition();
  nofFailedTests += test_operations ();
  nofFailedTests += test_conversions ();

  return nofFailedTests;
}
