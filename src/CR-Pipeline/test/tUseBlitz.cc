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
#include <fstream>
#include <string>
#include <cmath>

#include <blitz/array.h>

using std::cout;
using blitz::Array;
using blitz::Range;

/*!
  \file tUseBlitz.cc

  \brief A simple test for external build against libblitz

  \author Lars B&auml;hren

  \date 2007/01/29

  Check if we can properly build against the Blitz++ library
*/

// ------------------------------------------------------------------------------

/*!
  \brief Test construction of Blitz arrays

  \return nofFailedTests -- The number of failed tests
*/
int test_arrays ()
{
  cout << "\n[test_arrays]\n" << std::endl;

  int nofFailedTests (0);
  int nelem (5);
  
  cout << "[1] Create 1-dimensional arrays..." <<std::endl;
  try {
    Array<int,1> arrInt (nelem);
    Array<float,1> arrFloat (nelem);
    Array<double,1> arrDouble (nelem);
    Array<std::complex<double>,1> arrComplex (nelem);
    
    cout << arrInt    << std::endl;
    cout << arrFloat  << std::endl;
    cout << arrDouble << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[2] Create 2-dimensional arrays..." <<std::endl;
  try {
    Array<int,2> arrInt (10,20);
    Array<float,2> arrFloat (10,20);
    Array<double,2> arrDouble (10,20);

    cout << arrInt    << std::endl;
    cout << arrFloat  << std::endl;
    cout << arrDouble << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[3] Construction from other array..." <<std::endl;
  try {
    Array<int,2> arrInt (10,20);
    Array<float,2> arrFloat (10,20);
    Array<double,2> arrDouble (10,20);

    Array<int,2> arrIntCopy (arrInt);
    Array<float,2> arrFloatCopy (arrFloat);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test usage of mathematical functions and operations

  Testing is carried out in a sequence of steps: (a) first operate on standard
  data types -- such as <tt>int</tt> or <tt>float</tt>, (b) operate on individual
  elements of an array and (c) operate on the array.

  \return nofFailedTests -- The number of failed tests
*/
int test_mathematics ()
{
  cout << "\n[test_mathematics]\n" << std::endl;

  int nofFailedTests (0);

  cout << "[1] Mathematical operations on individual numbers..." <<std::endl;
  try {
    double a (+1.0);
    double b (-2.0);
    std::complex<double> c (0.5,4.0);
    double result (0.0);

    cout << "M_PI = " << M_PI << std::endl;

    result = a+b;
    result = a-b;
    result = a*b;
    result = a/b;

    result = sqrt(a);
    result = fabs(b);

    // triangular functions
    result = sin(a);
    result = cos(b);
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  cout << "[2] Mathematical operations on array alements..." <<std::endl;
  try {
    int nelem (20);
    Array<double,1> arr (nelem);
    double result (0.0);

    arr = 2.5;

    for (int n(0); n<nelem; n++) {
      result = arr(n)+arr(n);
      result = arr(n)-arr(n);
      result = arr(n)*arr(n);
      result = arr(n)/arr(n);
      
      result = sqrt(arr(n));
      result = fabs(arr(n));
      
      result = sin(arr(n));
      result = cos(arr(n));
    }
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  cout << "[3] Mathematical operations on arrays..." <<std::endl;
  try {
    int nelem (20);
    Array<double,1> arr (nelem);
    // fill the array
    for (int i(0); i<nelem; i++) {
      arr(i) = sin(0.5*i);
    }
    // show the contents of the array
    cout << arr << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  cout << "[4] Multi-element operations" <<std::endl;
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
    cout << spherical << std::endl;
    cout << cartesian << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test array I/O

  Writing an array to a file is just the same as writing it out to standard
  output:
  \verbatim
  5 x 5
  [         1         2         3         4         5
            6         7         8         9        10
	    11        12        13        14        15
	    16        17        18        19        20
	    21        22        23        24        25 ]
  \endverbatim

  \return nofFailedTests -- The number of failed tests
*/  
int test_io ()
{
  cout << "\n[test_io]\n" << std::endl;

  int nofFailedTests (0);
  int nelem (5);
  std::ofstream outfile;
  std::ifstream infile;

  cout << "[1] Testing I/O of Array<double,1>..." << std::endl;
  try {
    Array<double,1> vect (nelem);
    
    for (int n(0); n<nelem; n++) {
      vect(n) = n+1;
    }

    // Write the data to a file on disk
    outfile.open("blitzdata1.data");
    outfile << vect << std::endl;
    outfile.close();

    // Read the data back in from disk ...
    Array<double,1> vectFromDisk;

    infile.open("blitzdata1.data");
    infile >> vectFromDisk;
    infile.close();
    
    // ... and compare the vectors
    cout << "-- Original data vector:" << std::endl;
    cout << vect         << std::endl;
    cout << "-- Data vector written to disk and read back in:" << std::endl;
    cout << vectFromDisk << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  cout << "[2] Testing I/O of Array<double,2>..." << std::endl;
  try {
    Array<double,2> matrix (nelem,nelem);
    int i,j,n(1);
    
    for (i=0; i<nelem; i++) {
      for (j=0; j<nelem; j++) {
	matrix(i,j) = n;
	n++;
      }
    }

    // Write the data to a file on disk
    outfile.open("blitzdata2.data");
    outfile << matrix << std::endl;
    outfile.close();

    // Read the data back in from disk ...
    Array<double,2> vectFromDisk;

    infile.open("blitzdata2.data");
    infile >> vectFromDisk;
    infile.close();
    
    // ... and compare the vectors
    cout << "-- Original data vector:" << std::endl;
    cout << matrix         << std::endl;
    cout << "-- Data vector written to disk and read back in:" << std::endl;
    cout << vectFromDisk << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  } 

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  nofFailedTests += test_arrays();
  
  nofFailedTests += test_mathematics();

  nofFailedTests += test_io();

  cout << "[tUseBlitz] Number of failed tests = "
	    << nofFailedTests << std::endl;

  return nofFailedTests;
}
