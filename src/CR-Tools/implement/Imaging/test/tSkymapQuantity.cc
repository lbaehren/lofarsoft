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

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Skymap/SkymapQuantity.h>

/*!
  \file tSkymapQuantity.cc

  \ingroup Imaging

  \brief A collection of test routines for SkymapQuantity
 
  \author Lars B&auml;hren
 
  \date 2006/09/18
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the internal parameters of the created object

  \param qnt -- SkymapQuantity to be displayed
*/
void show (SkymapQuantity &qnt)
{
  std::cout << " - class name           = " << qnt.className()      << std::endl;
  std::cout << " - SkymapQuantity::Type = " << qnt.skymapQuantity() << std::endl;
  std::cout << " - signal domain        = " << qnt.domain()         << std::endl;
  std::cout << " - signal quantity      = " << qnt.quantity()       << std::endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SkymapQuantity object

  \return nofFailedTests -- The number of failed tests.
*/
int test_SkymapQuantity ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_SkymapQuantity]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    SkymapQuantity qnt;
    //
    show (qnt);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  } 
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    SkymapQuantity qnt1 = SkymapQuantity (SkymapQuantity::TIME_FIELD);
    SkymapQuantity qnt2 = SkymapQuantity (SkymapQuantity::TIME_POWER);
    SkymapQuantity qnt3 = SkymapQuantity (SkymapQuantity::TIME_CC);
    SkymapQuantity qnt4 = SkymapQuantity (SkymapQuantity::TIME_X);
    //
    show (qnt1);
    show (qnt2);
    show (qnt3);
    show (qnt4);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  } 
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    SkymapQuantity qnt1 = SkymapQuantity (SkymapQuantity::TIME_FIELD);
    SkymapQuantity qnt2 = SkymapQuantity (qnt1);
    //
    std::cout << "original object:" << std::endl;
    show (qnt1);
    std::cout << "copied object:" << std::endl;
    show (qnt2);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
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
    nofFailedTests += test_SkymapQuantity ();
  }

  return nofFailedTests;
}
