/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1159 2007-12-21 15:40:14Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Data/LOFAR_DipoleDataset.h>

using std::cerr;
using std::endl;

using LOFAR::LOFAR_DipoleDataset;  // Namespace usage

/*!
  \file tLOFAR_DipoleDataset.cc

  \ingroup LOFAR

  \brief A collection of test routines for the LOFAR_DipoleDataset class
 
  \author Lars Baehren
 
  \date 2008/01/10
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_DipoleDataset object
  
  \param filename -- Name of the HDF5 file, within which the dataset is located

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_DipoleDataset (std::string const &filename)
{
  std::cout << "\n[test_LOFAR_DipoleDataset]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LOFAR_DipoleDataset dataset;
    //
    dataset.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    LOFAR_DipoleDataset dataset (filename,
				 "Station001/001000000");
    //
    dataset.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    hid_t file_id = H5Fopen (filename.c_str(),
			     H5F_ACC_RDONLY,
			     H5P_DEFAULT);

    if (file_id > 0) {
      LOFAR_DipoleDataset dataset (file_id,
				 "Station001/001000000");
      dataset.summary(); 
      herr_t h5error = H5Fclose (file_id);
    } else {
      std::cerr << "-- Error opening HDF5 file " << filename << std::endl;
      nofFailedTests++;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);

  if (argc < 2) {
    cerr << "[tHDF5Common] Too few parameters!" << endl;
    cerr << "" << endl;
    cerr << "  tHDF5Common <filename>" << endl;
    cerr << "" << endl;
    return -1;
  }

  std::string filename (argv[1]);
  
  // Test for the constructor(s)
  nofFailedTests += test_LOFAR_DipoleDataset (filename);
  
  return nofFailedTests;
}
