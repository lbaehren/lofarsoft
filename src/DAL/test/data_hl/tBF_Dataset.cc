/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 3179 2009-10-14 13:20:10Z baehren                  $ |
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

#include <CommonAttributes.h>
#include <BF_Dataset.h>

// Namespace usage
using DAL::CommonAttributes;
using DAL::Filename;
using DAL::BF_Dataset;

/*!
  \file tBF_Dataset.cc

  \ingroup DAL

  \brief A collection of test routines for the BF_Dataset class
 
  \author Lars B&auml;hren
 
  \date 2009/10/28
*/

//_______________________________________________________________________________
//                                                                stationBeamName

//! Convert StationBeam index to name of the HDF5 group
std::string stationBeamName (unsigned int const &index)
{
  char uid[10];
  sprintf(uid,
	  "%03d",
	  index);
  
  std::string name (uid);
  
  name = "StationBeam" + name;
  
  return name;
}

//_______________________________________________________________________________
//                                                              test_constructors

/*!
  \brief Test constructors for a new BF_Dataset object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tBF_Dataset::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  Filename file ("123456789",
		 "test",
		 Filename::bf,
		 Filename::h5);
  
  std::cout << "[1] Testing construction with Filename ..." << std::endl;
  try {
    BF_Dataset dataset (file);
    //
    dataset.summary();
    //
    std::cout << "-- attribute[1] " << dataset.attribute(1) << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing construction with CommonAttributes ..." << std::endl;
  try {
    CommonAttributes commonAttr;
    commonAttr.setFilename (file);
    //
    BF_Dataset dataset (commonAttr);
    //
    dataset.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                 test_subGroups

/*!
  \brief Test working with the embedded groups

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_subGroups ()
{
  std::cout << "\n[tBF_Dataset::test_subGroups]\n" << std::endl;

  int nofFailedTests (0);
  Filename file ("123456789",
		 "test",
		 Filename::bf,
		 Filename::h5);
  BF_Dataset dataset (file);
  
  std::cout << "[1] Get name of StationBeam group from index ..." << std::endl;
  try {
    unsigned int nofStations (20);
    for (unsigned int n(0); n<nofStations; ++n) {
      std::cout << "\t" << n << "\t" << stationBeamName(n) << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Open SysLog group ..." << std::endl;
  try {
    dataset.openSysLog();
    dataset.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Open StationBeam groups ..." << std::endl;
  try {
    dataset.openStationBeam(0,true);
    dataset.openStationBeam(1,true);
    dataset.openStationBeam(2,true);
    dataset.openStationBeam(3,true);
    //
    dataset.openStationBeam(0,true);
    //
    dataset.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors ();
  // Test working with the embedded groups
  nofFailedTests += test_subGroups ();

  return nofFailedTests;
}
