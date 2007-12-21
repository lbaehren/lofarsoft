/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

/* $Id$*/

#include <Data/TIM40Header.h>

using CR::TIM40Header;

/*!
  \file tTIM40Header.cc

  \ingroup CR_Data
  
  \brief A collection of test routines for TIM40Header
 
  \author Lars B&auml;hren
 
  \date 2006/01/11
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TIM40Header object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TIM40Header ()
{
  int nofFailedTests (0);

  try {
    TIM40Header header;
    header.show();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
 
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion of frequencies using measures

  Since we are going to internally store the sampling information as measures,
  we should know who to deal with them.
*/
int test_frequencyConversion () 
{
  int nofFailedTests (0);
  double samplerateValue (80);
  String samplerateUnit ("MHz");

  MVFrequency samplerate (Quantity(samplerateValue,samplerateUnit));

  cout << "[test_frequencyConversion]" << endl;
  cout << " - numerical value         : " << samplerate << endl;
  cout << " - value in base units     : " << samplerate.get() << endl;
  cout << " - value in specific units : " << samplerate.get(Unit("MHz")) << endl;

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  
  // Test for the constructor(s)
  nofFailedTests += test_TIM40Header ();
  // Test conversion of frequencies using measures
  nofFailedTests += test_frequencyConversion ();
  
  return nofFailedTests;
}
