/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Kalpana Singh (<mail>)                                                     *
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

/* $Id: tppfinversion.cc,v 1.2 2007/08/08 15:30:04 singh Exp $*/

#include <Analysis/ppfinversion.h>

/*!
  \file tppfinversion.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfinversion
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/
using CR::ppfinversion ;


uint dataBlockSize (1024);
uint nofsegmentation(16*6) ;
Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
 
// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfinversion object

  \return nofFailedTests -- The number of failed tests.
*/
int test_ppfinversion ()
{
  int nofFailedTests (0);
  
  ppfinversion ppf_inv ;
  
  std::cout << "\n[test_ppfinversion]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    ppfinversion newObject;
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
    nofFailedTests += test_ppfinversion ();
  }

  return nofFailedTests;
}
