/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (<bahren@astron.nl>)                                     *
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

/* $Id: tBasicFilter.cc,v 1.2 2006/02/06 16:12:43 bahren Exp $*/

#include <Math/BasicFilter.h>

/*!
  \file tBasicFilter.cc
  
  \ingourp Math

  \brief A collection of test routines for BasicFilter
 
  \author Lars B&auml;hren
 
  \date 2005/11/16
*/

/*!
  \brief Test constructors for a new BasicFilter object

  Test the various constructors for a new BasicFilter object; as the class
  is templated we therefore should check all type for which an object can
  be created.

  \return nofFailedTests -- The number of failed tests.
*/
Int test_BasicFilter ();
/*!
  \brief Test manipulation and retrival of the filter weights

  \return nofFailedTests -- The number of failed tests.
*/
Int test_weights ();
/*!
  \brief Test operator overloading

  \return nofFailedTests -- The number of failed tests.
*/
Int test_operators ();

// ------------------------------------------------------------- test_BasicFilter

Int test_BasicFilter ()
{
  cout << "\n[test_BasicFilter]\n" << endl;

  Int nofFailedTests (0);
  //
  unsigned int blocksize (1024);
  Vector<Float> weights_float (blocksize,1);
  Vector<Double> weights_double (blocksize,1);
  Vector<Complex> weights_complex (blocksize,1);

  // default constructor
  cout << "[1] BasicFilter<T> ()" << endl;
  {
    cout << " - float" << endl;
    BasicFilter<Float> filter_float();
    cout << " - double" << endl;
    BasicFilter<Double> filter_double();
    cout << " - complex" << endl;
    BasicFilter<Complex> filter_complex();
  }

  // simplest argumented constructor
  cout << "[2] BasicFilter<T> (const Int&blocksize)" << endl;
  {
    cout << " - float" << endl;
    BasicFilter<Float> filter_float(blocksize);
    cout << " - double" << endl;
    BasicFilter<Double> filter_double(blocksize);
    cout << " - complex" << endl;
    BasicFilter<Complex> filter_complex(blocksize);
  }

  // Construction using predefined weights
  cout << "[3] BasicFilter<T> (const Vector<T>& weights)" << endl;
  {
    cout << " - float" << endl;
    BasicFilter<Float> filter_float(weights_float);
    //
    cout << " - double" << endl;
    BasicFilter<Double> filter_double(weights_double);
    //
    cout << " - complex" << endl;
    BasicFilter<Complex> filter_complex(weights_complex);
  }
    
  // copy constructor
  cout << "[4] BasicFilter<T> (const BasicFilter<T>& other)" << endl;
  {
    cout << " - float ... " << flush;
    BasicFilter<Float> filter_float1 (blocksize);
    BasicFilter<Float> filter_float2 (filter_float1);
    cout << "ok" << endl;
    //
    cout << " - double ... " << flush;
    BasicFilter<Double> filter_double1 (blocksize);
    BasicFilter<Double> filter_double2 (filter_double1);
    cout << "ok" << endl;
    //
    cout << " - complex ... " << flush;
    BasicFilter<Complex> filter_complex1 (blocksize);
    BasicFilter<Complex> filter_complex2 (filter_complex1);
    cout << "ok" << endl;
  }

  return nofFailedTests;
}

// ----------------------------------------------------------------- test_weights

Int test_weights ()
{
  cout << "\n[test_weights]\n" << endl;

  Int nofFailedTests (0);
  //
  unsigned int blocksize (1024);

  cout << "[1] float" << endl;
  {
    Vector<Float> weights (blocksize,2);
    BasicFilter<Float> filter (blocksize);
    //
    cout << sum(filter.weights()) << " -> ";
    //
    filter.setWeights (weights);
    //
    cout << sum(filter.weights()) << endl;
  }

  cout << "[2] double" << endl;
  {
    Vector<Double> weights (blocksize,2);
    BasicFilter<Double> filter (blocksize);
    //
    cout << sum(filter.weights()) << " -> ";
    //
    filter.setWeights (weights);
    //
    cout << sum(filter.weights()) << endl;
  }

  cout << "[3] complex" << endl;
  {
    Vector<Complex> weights (blocksize,2);
    BasicFilter<Complex> filter (blocksize);
    //
    cout << sum(filter.weights()) << " -> ";
    //
    filter.setWeights (weights);
    //
    cout << sum(filter.weights()) << endl;
  }

  return nofFailedTests;
}

// --------------------------------------------------------------- test_operators

Int test_operators ()
{
  cout << "\n[test_operators]\n" << endl;

  Int nofFailedTests (0);
  //
  unsigned int blocksize (512);
  Vector<Float> weights (1024,1.5);

  BasicFilter<Float> filter1 (blocksize);
  BasicFilter<Float> filter2 (filter1);

  cout << " - copy constructor" << endl;
  cout << " -- Filter 1 : " << sum(filter1.weights()) << endl;
  cout << " -- Filter 2 : " << sum(filter2.weights()) << endl;
  
  filter1.setWeights (weights);
  filter2 = filter1;

  cout << " - copy operator" << endl;
  cout << " -- Filter 1 : " << sum(filter1.weights()) << endl;
  cout << " -- Filter 2 : " << sum(filter2.weights()) << endl;

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_BasicFilter ();
  }

  // Test for computation of the filter weights
  {
    nofFailedTests += test_weights ();
  }

  // Test for the operators
  {
    nofFailedTests += test_operators ();
  }

  return nofFailedTests;
}
