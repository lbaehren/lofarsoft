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

#include <cmath>
#include <iostream>
#include <string>
#include <Math/VectorNorms.h>

using std::cerr;
using std::cout;
using std::endl;

/*!
  \file tVectorNorms.cc
  
  \ingroup CR_Math

  \brief Collection of tests for the methods in VectorNorms

  \author Lars B&auml;hren

  \date 2007/01/22
 */

// ==============================================================================
//
//  Test functions working with CASA array classes
//
// ==============================================================================

#ifdef HAVE_CASA

int test_casa_functions ()
{
  cout << "\n[test_casa_functions]\n" << endl;

  int nofFailedTests (0);
  int nelem (10);

  cout << "[1] Sign of vector elements..." << endl;
  try {
    casa::Vector<int> vecInt (nelem,1);
    casa::Vector<float> vecFloat (nelem,1);
    casa::Vector<double> vecDouble (nelem,1);

    casa::Vector<int> signVecInt = CR::sign (vecInt);
    casa::Vector<float> signVecFloat = CR::sign (vecFloat);
    casa::Vector<double> signVecDouble = CR::sign (vecDouble);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }

  cout << "[2] Inversion of vector elements order..." << endl;
  try {
    casa::Vector<int> vecInt (nelem,1);
    casa::Vector<float> vecFloat (nelem,1);
    casa::Vector<double> vecDouble (nelem,1);

    casa::Vector<int> invVecInt = CR::invertOrder (vecInt);
    casa::Vector<float> invVecFloat = CR::invertOrder (vecFloat);
    casa::Vector<double> invVecDouble = CR::invertOrder (vecDouble);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

#endif

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

#ifdef HAVE_CASA
    nofFailedTests += test_casa_functions();
#endif

  return nofFailedTests;  
}
