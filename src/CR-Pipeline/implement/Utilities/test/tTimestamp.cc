/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (lbaehren@yahoo.de)                                      *
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
#include <lopes/Utilities/Timestamp.h>

/*!
  \file tTimestamp.cc

  \ingroup Utilities

  \brief A collection of test routines for Timestamp
 
  \author Lars B&auml;hren
 
  \date 2006/07/16
*/

// -----------------------------------------------------------------------------

void showTime (LOPES::Timestamp &nb)
{
  std::cout << " -- ymd         = " << nb.ymd()         << std::endl;
  std::cout << " -- hms         = " << nb.hms()         << std::endl;
  std::cout << " -- iso8601     = " << nb.iso8601()     << std::endl;
  std::cout << " -- Unix time   = " << nb.timer()       << std::endl;
  std::cout << " -- Nanoblogger = " << nb.nanoblogger() << std::endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with timer structure

  \return nofFailedTests -- The number of failed tests.
*/
int test_time ()
{
  int nofFailedTests (0);

  {
    time_t rawtime;
    time (&rawtime);

    std::cout << " - rawtime = " << rawtime         << std::endl;
    std::cout << " - ctime   = " << ctime(&rawtime) << std::endl;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Timestamp object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Timestamp ()
{
  std::cout << "\n[test_Timestamp]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    LOPES::Timestamp nb;
    //
    showTime (nb);
  }
  
  std::cout << "[2] Testing copy constructor ..." << std::endl;
  {
    LOPES::Timestamp nb;
    //
    std::cout << " - Original object:" << std::endl;
    showTime (nb);
    //
    LOPES::Timestamp nb2 (nb);
    //
    std::cout << " - Copied object:" << std::endl;
    showTime (nb2);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_setTime ()
{
  std::cout << "\n[test_setTime]\n" << std::endl;

  int nofFailedTests (0);
  
  LOPES::Timestamp nb;
  //
  std::cout << " - Object from default constructor:" << std::endl;
  showTime (nb);
  
  return nofFailedTests;
}
// -----------------------------------------------------------------------------

/*!
  \brief Main function
*/
int main ()
{
  int nofFailedTests (0);

  // Test working with the timer
//   {
//     nofFailedTests += test_time ();
//   }

  // Test for the constructor(s)
  {
    nofFailedTests += test_Timestamp ();
  }

  // Test assignment of time
  {
    nofFailedTests += test_setTime ();
  }

  return nofFailedTests;
}
