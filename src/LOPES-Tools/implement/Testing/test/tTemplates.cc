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

#include <iostream>
#include <string>
#include <Testing/Templates.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using LOPES::TemplatesBase;

/*!
  \brief Test constructors for a TemplatesBase object
*/
void test_TemplatesBase ()
{
  cout << "[1] Testing default constructor ..." << endl;
  try {
    TemplatesBase<short> tShort;
    TemplatesBase<int> tInt;
    TemplatesBase<float> tFloat;
    TemplatesBase<double> tDouble;
    //
    tShort.summary();
    tInt.summary();
    tFloat.summary();
    tDouble.summary();
  } catch (string message) {
    cerr << " - error: " << message << endl;
  }

  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    short  dataShort  (1);
    int    dataInt    (1);
    float  dataFloat  (1.0);
    double dataDouble (1.0);
    //
    TemplatesBase<short> tShort (dataShort);
    TemplatesBase<int> tInt (dataInt);
    TemplatesBase<float> tFloat (dataFloat);
    TemplatesBase<double> tDouble (dataDouble);
    //
    tShort.summary();
    tInt.summary();
    tFloat.summary();
    tDouble.summary();
  } catch (string message) {
    cerr << " - error: " << message << endl;
  }
}

int main () {

  test_TemplatesBase ();

  return 0;

}
