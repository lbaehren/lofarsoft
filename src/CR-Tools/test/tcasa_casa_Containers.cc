/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1159 2007-12-21 15:40:14Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include <assert.h>
#include <casa/aips.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/List.h>
#include <casa/Containers/ListIO.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

#include "tests_common.h"

using std::cout;
using std::endl;

/*!
  \file tcasa_casa_Containers.cc

  \ingroup CR_test

  \brief A number of tests for clases in the casacore casa/Containers module

  \author Lars B&auml;hren

  <h3>Notes</h3>

  <ol>
    <li>Due to the split of the code base and the discontinued developement of
    the original CASA libraries, some of the test might work with <i>casacore</i>
    only; this needs to be handled by precompiler statements.
  </ol>
*/

// ------------------------------------------------------------------------------

/*!
  \brief Tests for classes in casa/Containers

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Block ()
{
  cout << "\n[test_Block]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::Block<int> bi1;                   // Block::Block()
    assert(bi1.nelements() == 0);     // Block::nelements()
    casa::Block<int> bi2(100);              // Block::Block(uInt)
    assert(bi2.nelements() == 100);
    casa::Block<int> bi7(0);
    assert(bi7.nelements() == 0);
    casa::Block<int> bi3(200,5);            // Block::Block(uInt, T)
    assert(bi3.nelements() == 200);
    casa::Block<int> bi6(0, 5);
    assert(bi6.nelements() == 0);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Testing casa/Containers/OrderedMap ..." << endl;
  try {
    casa::OrderedMap<int,int> map(-1);
    
    map(2)  = 90;
    map(8)  = 82;
    map(7)  = 72;
    map(3)  = 51;
    map(10) = 34;
    map(15) = 79;
    map(21) = map(27) = map(24) = 104;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_Record ()
{
  cout << "\n[test_Record]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::RecordDesc rd;
    rd.addField ("TpBool", casa::TpBool);
    rd.setComment (0, "comment for field TpBool");
    rd.addField ("TpUChar", casa::TpUChar);
    rd.addField ("TpShort", casa::TpShort);
    rd.addField ("TpInt", casa::TpInt);
    rd.addField ("TpUInt", casa::TpUInt);
    rd.addField ("TpFloat", casa::TpFloat);
    rd.addField ("TpDouble", casa::TpDouble);
    rd.addField ("TpComplex", casa::TpComplex);
    rd.addField ("TpDComplex", casa::TpDComplex);
    rd.addField ("TpString", casa::TpString);
    rd.addField ("TpArrayBool", casa::TpArrayBool, casa::IPosition(1,1));
    rd.addField ("TpArrayUChar", casa::TpArrayUChar, casa::IPosition(1,1));
    rd.addField ("TpArrayShort", casa::TpArrayShort, casa::IPosition(1,1));
    rd.addField ("TpArrayInt", casa::TpArrayInt, casa::IPosition(1,1));
    rd.addField ("TpArrayUInt", casa::TpArrayUInt, casa::IPosition(1,1));
    rd.addField ("TpArrayFloat", casa::TpArrayFloat, casa::IPosition(1,1));
    rd.addField ("TpArrayDouble", casa::TpArrayDouble, casa::IPosition(1,1));
    rd.addField ("TpArrayComplex", casa::TpArrayComplex, casa::IPosition(1,1));
    rd.addField ("TpArrayDComplex", casa::TpArrayDComplex, casa::IPosition(1,1));
    rd.addField ("TpArrayString", casa::TpArrayString);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_List ()
{
  cout << "\n[test_List]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::List<int> one;
    casa::ListIter<int> onePa(one);
    casa::ListIter<int> onePb(one);
    //
    onePa.addRight(3);
    onePa.addRight(72);
    onePa.addRight(16);
    onePa.addRight(7);
    onePa.addRight(31);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Main method

  \return nofFailedTests -- The number of failed tests we have encountered
*/
int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_Block();
  nofFailedTests += test_Record();
  nofFailedTests += test_List();

  return nofFailedTests;
}
