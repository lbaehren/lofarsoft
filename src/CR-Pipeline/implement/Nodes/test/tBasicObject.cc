/***************************************************************************
 *   Copyright (C) 2005                                                  *
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
/* $Id: tBasicObject.cc,v 1.2 2005/07/22 10:02:31 sven Exp $*/

#include <lopes/LopesBase/BasicObject.h>

/*!
  \file tBasicObject.cc

  \brief A collection of test routines for BasicObject
 
  \author Lars Baehren
 
  \date 2005/07/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new BasicObject object
*/
Bool test_BasicObject ()
{
  Bool ok (True);

  BasicObject b;
  
  return ok;
}

// -----------------------------------------------------------------------------

int main ()
{
  Bool ok (True);

  // Test for the constructor(s)
  {
    ok = test_BasicObject ();
  }

  if (ok) {
    return 0;
  } else {
    return 1;
  }
}
