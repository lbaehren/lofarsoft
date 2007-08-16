/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id: tRaisedCosineFilter.cc,v 1.1 2005/11/23 18:11:54 bahren Exp $*/

#include <Math/RaisedCosineFilter.h>

/*!
  \file tRaisedCosineFilter.cc
  
  \ingroup Math

  \brief A collection of test routines for RaisedCosineFilter
 
  \author Lars Bahren
 
  \date 2005/11/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new RaisedCosineFilter object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_RaisedCosineFilter ()
{
  Int nofFailedTests (0);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_RaisedCosineFilter ();
  }

  return nofFailedTests;
}
