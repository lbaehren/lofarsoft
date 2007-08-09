/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <iostream>
#include <blitz/Array.h>

using std::cerr;
using std::cout;
using std::endl;
using blitz::Array;
using blitz::shape;

/*!
  \file tBlitz.cc

  \ingroup Imaging

  \brief A number of tests for working with the Blitz++ library

  \author Lars B&auml;hren

  \date 2007/04/10
*/

// ------------------------------------------------------------------------------

/*!
  \brief Test the various ways to construct a Blitz++ array

  Besides the more commonly used constructors -- such as the default and
  copy constructors -- there is also the question how to handle the construction
  of an array from pre-existing data.

  \return nofFailedTest -- The number of failed tests
*/
int test_construction ()
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  uint nelem (10);

  std::cout << "[1] Construction with shape information..." << std::endl;
  try {
    Array<int,1> arr1int (nelem);
    Array<int,2> arr2int (nelem,nelem);
    Array<int,3> arr3int (nelem,nelem,nelem);

    std::cout << "-- shape(arr1) = " << arr1int.shape() << std::endl;
    std::cout << "-- shape(arr2) = " << arr2int.shape() << std::endl;
    std::cout << "-- shape(arr3) = " << arr3int.shape() << std::endl;

    Array<float,1> arr1float (nelem);
    Array<float,2> arr2float (nelem,nelem);
    Array<float,3> arr3float (nelem,nelem,nelem);
  } catch (std::string message) {
    std::cerr << "[test_construction] " << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Construction from pre-existing data..." << std::endl;
  try {
    uint n(0);
    double data1[] = {0,1,2,3,4,5,6,7,8,9};
    double *data2;

    data2 = new double [nelem];

    for (n=0; n<nelem; n++) {
      data2[n] = 0.5*data1[n];
    }

    Array<double,2> arr1 (data1,shape(3,3),blitz::neverDeleteData);

    Array<double,2> arr2 (data2,shape(3,3),blitz::neverDeleteData);
    delete [] data2;

    std::cout << arr1 << std::endl;
    std::cout << arr2 << std::endl;

  } catch (std::string message) {
    std::cerr << "[test_construction] " << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test math function for atomic variables

  \return nofFailedTest -- The number of failed tests
*/
template <class T> int test_mathFunctions (T const &a,
					   T const &b)
{
  int nofFailedTests (0);
  T result;

  result = a+b;
  result = a-b;
  result = a*b;
  result = a/b;

  result = sin(a) + cos (b);
  result = sin(a)/cos(b);
  result = tan(a);

  result = exp(a);

  return nofFailedTests;
}

/*
  Template instantiations
*/
template int test_mathFunctions (int const &a, int const &b);
template int test_mathFunctions (short const &a, short const &b);
template int test_mathFunctions (long const &a, long const &b);
template int test_mathFunctions (float const &a, float const &b);
template int test_mathFunctions (double const &a, double const &b);

//! Test math functions for Blitz array variables
template <class T> int test_mathFunctions (Array<T,1> const &a,
					   Array<T,1> const &b)
{
  int nofFailedTests (0);
  int nelem = a.numElements();

  // check if array shapes match
  if (nelem != b.numElements()) {
    return 0;
  }

  // test elememt-by-element operation first
  {
    T result;

    for (int n(0); n<nelem; n++) {
      result = a(n)+b(n);
      result = a(n)-b(n);
      result = a(n)*b(n);
      result = a(n)/b(n);

      result = sin(a(n)) + cos (b(n));
      result = sin(a(n))/cos(b(n));
      result = tan(a(n));
      
      result = exp(a(n));
    }
  }

  // test array-based operations
  {
    Array<T,1> result (nelem);
    
    result = a+b;
    result = a-b;
    result = a*b;
    result = a/b;
    
    result = sin(a) + cos (b);
    result = sin(a)/cos(b);
    result = tan(a);

    result = exp(a);
  }
  
  return nofFailedTests;
}

/*
  Template instantiations
*/
template int test_mathFunctions (Array<int,1> const &a, Array<int,1> const &b);
template int test_mathFunctions (Array<short,1> const &a, Array<short,1> const &b);
template int test_mathFunctions (Array<long,1> const &a, Array<long,1> const &b);
template int test_mathFunctions (Array<float,1> const &a, Array<float,1> const &b);
template int test_mathFunctions (Array<double,1> const &a, Array<double,1> const &b);

/*!
  \brief Test working with mathematical operations
*/
int test_mathematics ()
{
  cout << "\n[test_mathematics]\n" << endl;

  int nofFailedTests (0);
  int nelem (10);

  cout << "[1] Testing with atomic variables..." << endl;

  cout << "-- int" << endl;
  {
    int a(1);
    int b(2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- short" << endl;
  {
    short a(1);
    short b(2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- long" << endl;
  {
    long a(1);
    long b(2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- float" << endl;
  {
    float a(1);
    float b(2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- double" << endl;
  {
    double a(1);
    double b(2);
    nofFailedTests += test_mathFunctions (a,b);
  }

  cout << "[2] Testing with Blitz++ arrays" << endl;

  cout << "-- int" << endl;
  {
    Array<int,1> a (nelem,1);
    Array<int,1> b (nelem,2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- short" << endl;
  {
    Array<short,1> a (nelem,1);
    Array<short,1> b (nelem,2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- long" << endl;
  {
    Array<long,1> a (nelem,1);
    Array<long,1> b (nelem,2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- float" << endl;
  {
    Array<float,1> a (nelem,1);
    Array<float,1> b (nelem,2);
    nofFailedTests += test_mathFunctions (a,b);
  }
  cout << "-- double" << endl;
  {
    Array<double,1> a (nelem,1);
    Array<double,1> b (nelem,2);
    nofFailedTests += test_mathFunctions (a,b);
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Main routine

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_construction();
  nofFailedTests += test_mathematics();

  return nofFailedTests;
}
