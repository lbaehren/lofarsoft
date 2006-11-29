/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id: tBeamformer.cc,v 1.3 2006/10/31 19:27:36 bahren Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <lopes/Beamforming/Beamformer.h>

/*!
  \file tBeamformer.cc

  \ingroup Imaging

  \brief A collection of test routines for Beamformer
 
  \author Lars Bahren
 
  \date 2006/09/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Beamformer object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Beamformer ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_Beamformer]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    Beamformer newObject;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_Beamformer ();
  }

  return nofFailedTests;
}
