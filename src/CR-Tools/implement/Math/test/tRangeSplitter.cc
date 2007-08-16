/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: tRangeSplitter.cc,v 1.1 2006/03/28 12:01:44 bahren Exp $*/

#include <iostream>
#include <string>
#include <Math/RangeSplitter.h>

/*!
  \file tRangeSplitter.cc
  
  \ingroup Math

  \brief A collection of test routines for the RangeSplitter class
 
  \author Lars B&auml;hren
 
  \date 2006/03/27
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the internal parameters of the RangeSplitter object 
*/
template <class T>
void show_parameters (RangeSplitter<T>& splitter)
{
  cout << " - Segmentation mode : " << splitter.method() << endl;
  cout << " - nof segments      : " << splitter.nofSegments() << endl;
  cout << " - range segments    : " << splitter.rangeLimits() << endl;
}

template <class T> void show_parameters (RangeSplitter<int>& splitter);
template <class T> void show_parameters (RangeSplitter<float>& splitter);
template <class T> void show_parameters (RangeSplitter<double>& splitter);

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new RangeSplitter object

  \return nofFailedTests -- The number of failed tests.
*/
int test_RangeSplitter ()
{
  int nofFailedTests (0);
  int nofSegments (5);
  
  cout << "\n[test_RangeSplitter]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  try {
    RangeSplitter<int> splitter_int;
    show_parameters (splitter_int);
    //
    RangeSplitter<float> splitter_float;
    show_parameters (splitter_float);
    //
    RangeSplitter<double> splitter_double;
    show_parameters (splitter_double);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    RangeSplitter<int> splitter_lin (nofSegments);
    show_parameters (splitter_lin);
    //
    RangeSplitter<int> splitter_log (nofSegments);
    show_parameters (splitter_log);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing argumented constructor ..." << endl;
  try {
    RangeSplitter<int> splitter_lin ("lin");
    show_parameters (splitter_lin);
    //
    RangeSplitter<int> splitter_log ("log");
    show_parameters (splitter_log);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing argumented constructor ..." << endl;
  try {
    RangeSplitter<int> splitter_lin (nofSegments, "lin");
    show_parameters (splitter_lin);
    //
    RangeSplitter<int> splitter_log (nofSegments, "log");
    show_parameters (splitter_log);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the actual splitting of ranges

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_split ()
{
  int nofFailedTests (0);
  int nofSegments (15);
  bool status (true);
  
  cout << "\n[test_split]\n" << endl;

  cout << "[1] Linear splitting ..." << endl;
  try {
    RangeSplitter<Double> splitter (nofSegments,"lin");
    //
    status = splitter.split(40,80);
    show_parameters (splitter);
    //
    status = splitter.split(0,100);
    show_parameters (splitter);
    //
    status = splitter.split(100,200);
    show_parameters (splitter);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[2] Logarithmic splitting ..." << endl;
  try {
    RangeSplitter<Double> splitter (nofSegments,"log");
    //
    status = splitter.split(40,80);
    show_parameters (splitter);
    //
    status = splitter.split(0,100);
    show_parameters (splitter);
    //
    status = splitter.split(100,200);
    show_parameters (splitter);
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
    nofFailedTests += test_RangeSplitter ();
  }
  
  if (nofFailedTests == 0) {
    {
      nofFailedTests += test_split ();
    }    
  }

  return nofFailedTests;
}
