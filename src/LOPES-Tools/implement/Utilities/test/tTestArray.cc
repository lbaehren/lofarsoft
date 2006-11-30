/***************************************************************************
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

/*!
  \file tTestArray.cc

  \brief A number of tests for the TestArray class
  
  \author Lars B&auml;hren

  \date 2006/11/21
*/

// Header files from standard library
#include <stdlib.h>

// LOPES-Tools header files
#include <Utilities/TestArray.h>

using casa::Array;
using casa::IPosition;
using LOPES::TestArray;

int main ()
{
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    TestArray<double> obj;
    //
    obj.summary();
  }
  
  std::cout << "[2] Testing default constructor (int) ..." << std::endl;
  {
    TestArray<int> obj;
    //
    obj.summary();
  }
  
  std::cout << "[3] Testing argumented constructor (double) ..." << std::endl;
  {
    IPosition shape (2,10,10);
    Array<double> arr (shape);
    arr = 1.0;
    //
    TestArray<double> obj (arr);
    //
    obj.summary();
  }
  
  std::cout << "[4] Testing argumented constructor (int) ..." << std::endl;
  {
    IPosition shape (2,15,15);
    Array<int> arr (shape);
    arr = 1;
    //
    TestArray<int> obj (arr);
    //
    obj.summary();
  }
  
  return 0;
}
