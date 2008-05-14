/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: tTimestamp.cc,v 1.2 2006/10/31 18:24:08 bahren Exp $*/

#include <iostream>
#include <casa/OS/Time.h>
#include <Utilities/Timestamp.h>

using std::endl;

using CR::Timestamp;

/*!
  \file tTimestamp.cc

  \ingroup CR_Utilities

  \brief A collection of test routines for tne Timestamp class
 
  \author Lars B&auml;hren
 
  \date 2006/07/16
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test working with timer structure

  \return nofFailedTests -- The number of failed tests.
*/
int test_time ()
{
  std::cout << "\n[test_time]\n" << endl;
  
  int nofFailedTests (0);


  std::cout << "[1] New time construct from scratch ..." << endl;
  try {
    time_t rawtime;
    time (&rawtime);

    std::cout << " - rawtime = " << rawtime         << endl;
    std::cout << " - ctime   = " << ctime(&rawtime) << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with casa/OS/Time class

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_Time ()
{
  std::cout << "\n[test_Time]\n" << endl;
  
  int nofFailedTests (0);

  try {
    casa::Time casaTime;
    std::cout << "-- Current time, GMT     = " << casaTime.toString(false) << endl;
    std::cout << "-- Current time, ISO8601 = " << casaTime.toString(true) << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Timestamp object

  \return nofFailedTests -- The number of failed tests in this function.
*/
int test_Timestamp ()
{
  std::cout << "\n[test_Timestamp]\n" << endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << endl;
  {
    Timestamp nb;
    nb.summary();
  }
  
  std::cout << "[2] Testing copy constructor ..." << endl;
  {
    Timestamp nb;
    //
    std::cout << " - Original object:" << endl;
    nb.summary();
    //
    Timestamp nb2 (nb);
    //
    std::cout << " - Copied object:" << endl;
    nb2.summary();
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_setTime ()
{
  std::cout << "\n[test_setTime]\n" << endl;

  int nofFailedTests (0);
  
  Timestamp nb;
  nb.summary();
  
  return nofFailedTests;
}
// -----------------------------------------------------------------------------

/*!
  \brief Main function

  \return nofFailedTests -- The number of failed test encountered throughout this
          program.
*/
int main ()
{
  int nofFailedTests (0);

  // Test working with the timer
  nofFailedTests += test_time ();
  nofFailedTests += test_Time ();

  // Test for the constructor(s)
  nofFailedTests += test_Timestamp ();
  // Test assignment of time
  nofFailedTests += test_setTime ();
  
  return nofFailedTests;
}
