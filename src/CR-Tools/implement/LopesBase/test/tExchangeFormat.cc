/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Sven Lafebre (<mail>)                                                     *
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

/* $Id: tExchangeFormat.cc,v 1.2 2006/07/05 12:59:12 bahren Exp $*/

#include <iostream>
#include <LopesBase/ExchangeFormat.h>

using namespace std;

using CR::EF_isComplex;
using CR::EF_isScalar;
using CR::ExchangeFormat;

/*!
  \file tExchangeFormat.cc

  \brief A collection of test routines for ExchangeFormat
 
  \author Sven Lafebre
 
  \date 2005/07/21
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ExchangeFormat object
*/
bool test_ExchangeFormat ()
{
  bool ok (true);
  ExchangeFormat a;

  a.setFlag(EF_isComplex | EF_isScalar);
  cout << a.flag(EF_isComplex) << endl;
  cout << a.flag(EF_isScalar) << endl;
  a.clearFlag(EF_isScalar);
  cout << a.flag(EF_isScalar | EF_isComplex) << endl;

  ExchangeFormat b = a;
  
  b.setFlag(EF_isScalar);
  cout << b.flag(EF_isComplex)   << " & "
       << b.flag(EF_isScalar) << " = "
       << b.flag(EF_isScalar & EF_isComplex) 
       << endl;

  return ok;
}

// -----------------------------------------------------------------------------

int main ()
{
  bool ok (true);

  // Test for the constructor(s)
  {
    ok = test_ExchangeFormat ();
  }

  if (ok) {
    return 0;
  } else {
    return 1;
  }
}
