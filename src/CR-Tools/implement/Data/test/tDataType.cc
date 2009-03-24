/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#include <Data/DataType.h>

// Namespace usage
using CR::DataType;

/*!
  \file tDataType.cc

  \ingroup CR_Data

  \brief A collection of test routines for the DataType class
 
  \author Lars B&auml;hren
 
  \date 2009/03/24
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new DataType object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tDataType::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing argumented constructor ..." << std::endl;
  try {
    DataType casa_image (DataType::CASA_IMAGE);
    casa_image.summary();
    //
    DataType casa_ms    (DataType::CASA_MS);
    casa_ms.summary();
    //
    DataType fits       (DataType::FITS);
    fits.summary();
    //
    DataType hdf5       (DataType::HDF5);
    hdf5.summary();
    //
    DataType lofar_bf   (DataType::LOFAR_BF);
    lofar_bf.summary();
    //
    DataType lofar_tbb  (DataType::LOFAR_TBB);
    lofar_tbb.summary();
    //
    DataType tim40      (DataType::TIM40);
    tim40.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Testing copy constructor ..." << std::endl;
  try {
    DataType t1 (DataType::FITS);
    DataType t2 (t1);
    //
    std::cout << "--> original object:" << std::endl;
    t1.summary();
    //
    std::cout << "--> object created as copy:" << std::endl;
    t2.summary();
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
  nofFailedTests += test_constructors ();

  return nofFailedTests;
}
