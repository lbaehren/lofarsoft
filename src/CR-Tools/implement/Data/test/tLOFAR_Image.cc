/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1159 2007-12-21 15:40:14Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

#include <Data/LOFAR_Image.h>

using LOFAR::LOFAR_Image;  // Namespace usage

/*!
  \file tLOFAR_Image.cc

  \ingroup CR_Data

  \brief A collection of test routines for the LOFAR_Image class
 
  \author Lars Baehren
 
  \date 2008/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_Image object

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_Image ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_LOFAR_Image]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LOFAR_Image newLOFAR_Image;
    //
    newLOFAR_Image.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_LOFAR_Image ();
  }

  return nofFailedTests;
}
