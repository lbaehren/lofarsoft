/***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <rmParallel.h>

/*!
  \file trmParallel.cpp
  \ingroup RM
  \brief A collection of tests for the RM::parallel class

  \author Lars B&auml;hren
  \date 2010-04-19
*/

//_______________________________________________________________________________
//                                                                    test_sysctl

int test_sysctl ()
{
  std::cout << "\n[trmParallel::test_sysctl]\n" << std::endl;

  int nofFailedTests = 0;

#ifdef HW_MACHINE
  std::cout << "-- HW_MACHINE (machine class) = " << HW_MACHINE << std::endl;
#endif

#ifdef HW_MODEL
  std::cout << "-- HW_MODEL (machine model)   = " << HW_MODEL << std::endl;
#endif

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                              test_construction

int test_construction ()
{
  std::cout << "\n[trmParallel::test_construction]\n" << std::endl;

  int nofFailedTests = 0;

  try {
    RM::parallel p;
    p.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    ++nofFailedTests;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

/*!
  \brief Main routine of the test program

  \return nofFailedTests -- The number of failed tests encountered within and
          identified by this test program.
*/
int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_sysctl ();
  nofFailedTests += test_construction ();

  return nofFailedTests;
}
