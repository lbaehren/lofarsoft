/*-------------------------------------------------------------------------*
 | $Id:: TimeFreqSkymap.cc 1967 2008-09-09 13:16:22Z baehren             $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

/*!
  \file casacoreArrayAccess.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Array<T>

  \autor Lars B&auml;hren
*/

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include <casa/Arrays.h>

using casa::Array;
using casa::IPosition;

//_______________________________________________________________________________
//                                                                access_array_2d

/*!
  \brief Access to and manipulation of a two-dimensional array
  
  \return nofFailedTests -- The number of failed tests encountered within this 
          function.

  Test inserting of values through slicing operation:
  - fill array per row
  - fill array per column
*/
int access_array_2d (uint const &nelem=5)
{
  std::cout << "\n[casacoreArrayAccess::access_array_2d]\n" << std::endl;

  int nofFailedTests (0);
  uint nofAxes (2);
  IPosition naxis (nofAxes,nelem);
  Array<double> arr (naxis);

  std::cout << "[1] Fill matrix through reference to rows ..." << std::endl;
  try {
    casa::Matrix<double> vec;
    IPosition start (nofAxes,0,0);
    IPosition end (arr.shape()-1);

    for (uint n(0); n<naxis(0); n++) {
      start(0) = end(0) = n;
      vec.reference(arr(start,end));
      vec = double(n);
    }
    
    std::cout << arr << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Fill matrix through reference to columns ..." << std::endl;
  try {
    casa::Matrix<double> vec;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);

    for (uint n(0); n<naxis(1); n++) {
      start(1) = end(1) = n;
      vec.reference(arr(start,end));
      vec = double(n);
    }
    
    std::cout << arr << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                access_array_3d

/*!
  \brief Access to and manipulation of a three-dimensional array
  
  \return nofFailedTests -- The number of failed tests encountered within this 
          function.

  Test inserting of values through slicing operation:
  - fill in values perpendicular to a 2D-plane, i.e. parallel to an axis
  - fill in values per plane, i.e. parallel to an array axis; an example for this
    would be assigning a single value to points in a (x,y) plane.
*/
int access_array_3d (uint const &nelem=5)
{
  std::cout << "\n[casacoreArrayAccess::access_array_3d]\n" << std::endl;

  int nofFailedTests (0);
  uint nofAxes (3);
  IPosition naxis (nofAxes,nelem);
  Array<double> arr (naxis);

  std::cout << "[1] Insert values along the z-axis ..." << std::endl;
  try {
    casa::Cube<double> mat;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);
    double n (0);
    
    for (int ny(0); ny<naxis(1); ny++) {
      start(1) = end(1) = ny;
      for (int nx(0); nx<naxis(0); nx++) {
	start(0) = end(0) = nx;
	mat.reference(arr(start,end));
	mat = n;
	n+=1.0;
      }
    }
    
    std::cout << arr << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Insert values per (x,y)-plane ..." << std::endl;
  try {
    casa::Cube<double> mat;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);

    for (int nz(0); nz<naxis(2); nz++) {
      start(2) = end(2) = nz;
      mat.reference(arr(start,end));
      mat = double(nz);
    }

    std::cout << arr << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Insert values per (y,z)-plane ..." << std::endl;
  try {
    casa::Cube<double> mat;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);

    for (int nx(0); nx<naxis(0); nx++) {
      start(0) = end(0) = nx;
      mat.reference(arr(start,end));
      mat = double(nx);
    }

    std::cout << arr << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                access_array_4d

/*!
  \brief Access to and manipulation of a two-dimensional array
  
  \return nofFailedTests -- The number of failed tests encountered within this 
          function.

  Test inserting of values into an array of rank N through slicing operation:
  - iterate along one of the array axes and insert a subarray rank (N-1)
  - loop over two selected axes and insert a two-dimensional subarray (Matrix)
  - for a combination three array axes insert a vector of values along the
    remaining axis
*/
int access_array_4d (uint const &nelem=5)
{
  std::cout << "\n[casacoreArrayAccess::access_array_4d]\n" << std::endl;

  int nofFailedTests (0);
  uint nofAxes (4);
  IPosition naxis (nofAxes,nelem);
  Array<double> arr (naxis);

  std::cout << "[1] Insert values into 3D subarray perp. to x-axis ..."
	    << std::endl;
  try {
    Array<double> buffer;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);
    
    for (int nx(0); nx<naxis(0); nx++) {
      start(0) = end(0) = nx;
      buffer.reference(arr(start,end));
      buffer = double(nx);
    }
    
    std::cout << arr << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Insert values into 3D subarray perp. to y-axis ..." << std::endl;
  try {
    Array<double> buffer;
    IPosition start (nofAxes,0);
    IPosition end (arr.shape()-1);
    
    for (int ny(0); ny<naxis(1); ny++) {
      start(1) = end(1) = ny;
      buffer.reference(arr(start,end));
      buffer = double(ny);
    }
    
    std::cout << arr << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main ()
{
  int nofFailedTests = 0;

  nofFailedTests += access_array_2d ();
  nofFailedTests += access_array_3d ();
  nofFailedTests += access_array_4d ();

  return nofFailedTests;
}
