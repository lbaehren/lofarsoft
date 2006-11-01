/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: template-tclass.cc,v 1.6 2006/09/20 09:56:53 bahren Exp $*/

#include <iostream>
#include <Data/DynamicSpectrum.h>

/*!
  \file tDynamicSpectrum.cc

  \ingroup Data

  \brief A collection of test routines for DynamicSpectrum
 
  \author Lars B&auml;hren
 
  \date 2006/10/25
*/

// -----------------------------------------------------------------------------

int test_blitzArrays ()
{
  int nofFailedTests (0);
  int nofAxes (2);
  int shape[2] = {100,200};

  std::cout << "[1]" <<std::endl;
  {
    blitz::Array<int,1> data (shape[0]);
    //
    std::cout << " - dimensions    = " << data.dimensions()  << std::endl;
    std::cout << " - rank          = " << data.rank()        << std::endl;
    std::cout << " - shape         = " << data.shape()       << std::endl;
    std::cout << " - nof. elements = " << data.numElements() << std::endl;
  }

  std::cout << "[2]" <<std::endl;
  {
    blitz::Array<int,2> data (shape[0],shape[1]);
    //
    std::cout << " - dimensions    = " << data.dimensions()  << std::endl;
    std::cout << " - rank          = " << data.rank()        << std::endl;
    std::cout << " - shape         = " << data.shape()       << std::endl;
    std::cout << " - nof. elements = " << data.numElements() << std::endl;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new DynamicSpectrum object

  \return nofFailedTests -- The number of failed tests.
*/
int test_DynamicSpectrum ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_DynamicSpectrum]\n" << std::endl;
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    lopestools::DynamicSpectrum ds;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  {
    blitz::Array<int,1> shape (2);
    shape = 100,20;
    lopestools::DynamicSpectrum ds (shape);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test working with Blitz++ arrays
  {
    nofFailedTests += test_blitzArrays ();
  }

  // Test for the constructor(s)
  {
    nofFailedTests += test_DynamicSpectrum ();
  }

  return nofFailedTests;
}
