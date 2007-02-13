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
/* $Id: tStoredObject.cc,v 1.2 2005/07/21 12:22:54 horneff Exp $*/

#include <Nodes/StoredObject.h>

/*!
  \file tStoredObject.cc

  \brief A collection of test routines for StoredObject
 
  \author Andreas Horneffer
 
  \date 2005/07/20
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StoredObject object
*/
bool test_StoredObject ()
{
  bool ok (true);
  CR::StoredObject<casa::Double> so;
  casa::Vector<casa::Double> vec;
  
  try {
    ok = so.get(&vec);
    std::cout << "laber!a \n";
  } catch (AipsError x) {
    std::cerr << x.getMesg() << std::endl;
    return false;
  };
  
  return ok;
}

// -----------------------------------------------------------------------------

int main ()
{
  bool ok (true);

  std::cout << "laber! \n";

  // Test for the constructor(s)
  {
    ok = test_StoredObject ();
  }

  std::cout << "laber!! \n";

  
  if (ok) {
    return 0;
  } else {
    return 1;
  }
}
