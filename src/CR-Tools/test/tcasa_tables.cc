/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <iostream>
#include <string>

using std::cout;
using std::endl;

#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Slice.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/RegularFile.h>
#include <casa/Utilities/Sort.h>
#include <casa/Utilities/Assert.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableLock.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/StManAipsIO.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/ExprNodeSet.h>

using casa::Int;
using casa::uInt;
using casa::DComplex;
using casa::IPosition;
using casa::ColumnDesc;
using casa::TableDesc;
using casa::ScalarColumnDesc;
using casa::ArrayColumnDesc;
using casa::MVTime;

/*!
  \file tcasa_tables.cc

  \brief A number of tests for clases in the casacore tables module

  \ingroup CR_test

  \author Lars B&auml;hren

  <u>Note:</u> A number of the tests performed will not work with the original CASA
  libraries.
*/

// ------------------------------------------------------------------------------

int test_Tables ()
{
  int nofFailedTests (0);

  std::cout << "[1] Build the table description..." << std::endl;
  try {
    TableDesc td("", "1", casa::TableDesc::Scratch);
    td.comment() = "A test of class Table";
    td.addColumn (ScalarColumnDesc<Int>("ab","Comment for column ab"));
    td.addColumn (ScalarColumnDesc<uInt>("ad","comment for ad"));
    td.addColumn (ScalarColumnDesc<DComplex>("ag"));
    td.addColumn (ArrayColumnDesc<float>("arr1",IPosition(3,2,3,4),
					 ColumnDesc::Direct));
    td.addColumn (ArrayColumnDesc<float>("arr2",0));
    td.addColumn (ArrayColumnDesc<float>("arr3",0,ColumnDesc::Direct));
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Test deletion of rows, array of Strings, and some more..." << std::endl;
  try {
    casa::TableDesc td("", "1", TableDesc::Scratch);
    td.addColumn (ScalarColumnDesc<Int>("ab","Comment for column ab"));
    td.addColumn (ScalarColumnDesc<Int>("ac"));
    td.addColumn (ScalarColumnDesc<uInt>("ad","comment for ad"));
    td.addColumn (ScalarColumnDesc<float>("ae"));
    td.addColumn (ScalarColumnDesc<casa::String>("af"));
    td.addColumn (ScalarColumnDesc<DComplex>("ag"));
    td.addColumn (ArrayColumnDesc<float>("arr1",3,ColumnDesc::Direct));
    td.addColumn (ArrayColumnDesc<float>("arr2",0));
    td.addColumn (ArrayColumnDesc<casa::String>("arr3",0,ColumnDesc::Direct));
    // Now create a new table from the description.
    casa::SetupNewTable newtab("tTable_tmp.data1", td, casa::Table::New);
    // Create a storage manager for it.
    casa::StManAipsIO sm1;
    casa::StManAipsIO sm2;
    newtab.bindAll (sm1);
    newtab.bindColumn ("ab",sm2);
    newtab.bindColumn ("ac",sm2);
    newtab.setShapeColumn("arr1",IPosition(3,2,3,4));
    newtab.setShapeColumn("arr3",IPosition(1,2));
    casa::Table tab(newtab);
    tab.rename ("tTable_tmp.data2",  casa::Table::New);
    tab.rename ("tTable_tmp.data2",  casa::Table::New);
    tab.rename ("tTable_tmp.data2",  casa::Table::Scratch);
    tab.rename ("tTable_tmp.data2a", casa::Table::Scratch);
    tab.rename ("tTable_tmp.data2a", casa::Table::Scratch);
    tab.rename ("tTable_tmp.data2a", casa::Table::New);
    tab.rename ("tTable_tmp.data2",  casa::Table::Scratch);
    tab.rename ("tTable_tmp.data2a", casa::Table::New);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  std::cout << "[1] Testing classes in casa/LogTables ..." << std::endl;
  try {
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  nofFailedTests += test_Tables();
  
  return nofFailedTests;
}
