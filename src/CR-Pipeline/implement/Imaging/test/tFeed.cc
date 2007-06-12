/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

/* $Id: template-tclass.cc,v 1.8 2007/06/05 14:47:41 bahren Exp $*/

#include <Imaging/Feed.h>

using LOPES::Feed;

/*!
  \file tFeed.cc

  \ingroup CR
  \ingroup Imaging

  \brief A collection of test routines for Feed
 
  \author Lars Baehren
 
  \date 2007/06/12
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Feed object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Feed ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_Feed]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    Feed newObject;
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
    nofFailedTests += test_Feed ();
  }

  return nofFailedTests;
}
