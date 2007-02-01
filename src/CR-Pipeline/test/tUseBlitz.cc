/***************************************************************************
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
#include <cmath>

#include <blitz/array.h>

using blitz::Array;
using blitz::Range;

/*!
  \file tUseBlitz.cc

  \brief A simple test for external build against libfits

  \author Lars B&auml;hren

  \date 2007/01/29

  Check if we can properly build against the Blitz library
*/

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  std::cout << "[1] Create 1-dimensional arrays..." <<std::endl;
  try {
    int nelem (5);
    Array<int,1> arrInt (nelem);
    Array<float,1> arrFloat (nelem);
    Array<double,1> arrDouble (nelem);
    
    std::cout << arrInt    << std::endl;
    std::cout << arrFloat  << std::endl;
    std::cout << arrDouble << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  std::cout << "[2] Create 2-dimensional arrays..." <<std::endl;
  try {
    Array<int,2> arrInt (10,20);
    Array<float,2> arrFloat (10,20);
    Array<double,2> arrDouble (10,20);

    std::cout << arrInt    << std::endl;
    std::cout << arrFloat  << std::endl;
    std::cout << arrDouble << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  std::cout << "[3] Mathematical operations on arrays..." <<std::endl;
  try {
    int nelem (20);
    Array<double,1> arr (nelem);
    // fill the array
    for (int i(0); i<nelem; i++) {
      arr(i) = sin(0.5*i);
    }
    // show the contents of the array
    std::cout << arr << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  std::cout << "[4] Multi-element operations" <<std::endl;
  try {
    int nelem (10);
    Array<double,2> cartesian (nelem,3);
    Array<double,2> spherical (nelem,3);
    // fill the input array
    spherical(Range(Range::all()),0) = 1.0;
    spherical(Range(Range::all()),1) = 2.0;
    spherical(Range(Range::all()),2) = 3.0;
    //
    cartesian(Range(Range::all()),0) = spherical(Range(Range::all()),0)*cos(spherical(Range(Range::all()),1))*sin(spherical(Range(Range::all()),2));
    cartesian(Range(Range::all()),1) = spherical(Range(Range::all()),0)*sin(spherical(Range(Range::all()),1))*sin(spherical(Range(Range::all()),2));
    cartesian(Range(Range::all()),2) = spherical(Range(Range::all()),0)*cos(spherical(Range(Range::all()),2));
    // show the contents of the arrays
    std::cout << spherical << std::endl;
    std::cout << cartesian << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  return nofFailedTests;
}
