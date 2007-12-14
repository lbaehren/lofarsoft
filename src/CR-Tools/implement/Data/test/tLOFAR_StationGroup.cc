/*-------------------------------------------------------------------------*
 | $Id::                                                                   |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_StationGroup.h>
#include <Utilities/StringTools.h>

using CR::LOFAR_StationGroup;  // Namespace usage

/*!
  \file tLOFAR_StationGroup.cc

  \ingroup Data

  \brief A collection of test routines for the LOFAR_StationGroup class
 
  \author Lars B&auml;hren
 
  \date 2007/12/10
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_StationGroup object

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_StationGroup (std::string const &filename)
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_LOFAR_StationGroup]\n" << std::endl;

  // DAL Dataset object to handle the basic I/O
  dalDataset dataset;
  
  if (dataset.open(CR::string2char(filename))) {
    std::cerr << "Error opening datafile." << endl;
    nofFailedTests++;
  } else {
    
    std::cout << "[1] Testing default constructor ..." << std::endl;
    try {
      LOFAR_StationGroup group;
      //
      group.summary(); 
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }
    
    std::cout << "[2] Testing argumented constructor ..." << std::endl;
    try {
      LOFAR_StationGroup group (dataset,"Station001");
      //
      group.summary(); 
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }
    
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);

  /*
    Check if filename of the dataset is provided on the command line; if not
    exit the program.
  */
  if (argc < 2) {
    std::cerr << "[tLOFAR_TBB] Too few parameters!" << std::endl;
    std::cerr << "" << std::endl;
    std::cerr << "  tLOFAR_TBB <filename>" << std::endl;
    std::cerr << "" << std::endl;
    return -1;
  }

  std::string filename = argv[1];

  // Test for the constructor(s)
  nofFailedTests += test_LOFAR_StationGroup (filename);

  return nofFailedTests;
}
