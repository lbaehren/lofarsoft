/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: tStatisticsFilter.cc,v 1.1 2006/03/28 13:33:57 bahren Exp $*/

#include <Functionals/StatisticsFilter.h>

using std::cerr;
using std::cout;
using std::endl;
using LOPES::StatisticsFilter;
using LOPES::FilterType;

/*!
  \file tStatisticsFilter.cc
  
  \brief A collection of test routines for StatisticsFilter
  
  \author Lars B&auml;hren
  
  \date 2005/12/21
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StatisticsFilter object
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_StatisticsFilter ()
{
  cout << "\n[test_StatisticsFilter]\n" << endl;

  int nofFailedTests (0);
  unsigned int filterStrength (5);

  // Test the default constructor
  cout << "[1] Testing default constructor ..." << endl;
  try {
    StatisticsFilter<float> filter_float;
    StatisticsFilter<double> filter_double;
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Test the argumented constructor
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    StatisticsFilter<float> filter_float(filterStrength);
    StatisticsFilter<double> filter_double(filterStrength);
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test the argumented constructor
  cout << "[3] Testing argumented constructor ..." << endl;
  try {
    StatisticsFilter<float> mf (filterStrength,FilterType::MEAN);
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test the copy constructor
  cout << "[4] Testing copy constructor ..." << endl;
  try {
    StatisticsFilter<float> mf1 (filterStrength,FilterType::MEAN);
    StatisticsFilter<float> mf2 (mf1);
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << " -> " << nofFailedTests << " failed tests." << endl;
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test application of the filter to data

  This essentially just checks the internal operation (handling of arrays,
  slices, etc. It might be nice to actually provide the test program with some
  example data to work on, to demonstrate the effect of the filter.
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_filter ()
{
  cout << "\n[test_filter]\n" << endl;

  int nofFailedTests (0);
  int nofAntennas (10);
  int nofChannels (1024);
  unsigned int strength (8);

  cout << "[1] Testing filter with vector input ..." << endl;
  try {
    StatisticsFilter<float> mf (strength,FilterType::MEAN);
    casa::Vector<float> input (nofChannels,1.0);
    casa::Vector<float> output (mf.filter(input));
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing filter with Matrix input ..." << endl;
  try {
    StatisticsFilter<float> mf (strength,FilterType::MEAN);
    casa::Matrix<float> input (nofChannels,nofAntennas,1.0);
    casa::Matrix<float> output (mf.filter(input));
  } catch (casa::AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << " -> " << nofFailedTests << " failed tests." << endl;
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_StatisticsFilter ();
  }

  {
    nofFailedTests += test_filter ();
  }

  return nofFailedTests;
}
