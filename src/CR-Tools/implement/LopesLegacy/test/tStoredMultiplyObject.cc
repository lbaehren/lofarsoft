/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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
/* $Id: tStoredMultiplyObject.cc,v 1.1 2005/07/22 13:40:36 horneff Exp $*/

#include <LopesLegacy/StoredMultiplyObject.h>

using CR::StoredMultiplyObject;

/*!
  \file tStoredMultiplyObject.cc

  \brief A collection of test routines for StoredMultiplyObject
 
  \author Andreas Horneffer
 
  \date 2005/07/21
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StoredMultiplyObject object
*/
bool test_StoredMultiplyObject ()
{
  bool ok (true);
  
  return ok;
}

// -----------------------------------------------------------------------------

int main ()
{
  bool ok (true);

  // Test for the constructor(s)
  {
    ok = test_StoredMultiplyObject ();
  }

  if (ok) {
    return 0;
  } else {
    return 1;
  }
}
