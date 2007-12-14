/*-------------------------------------------------------------------------*
 | $Id::                                                                   |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (<mail>)                                                 *
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

#include <Data/LOFAR_TBB.h>
#include <Utilities/StringTools.h>

/* Namespace usage */
using std::cout;
using std::endl;
using CR::LOFAR_TBB;

/*!
  \file tLOFAR_TBB.cc

  \ingroup CR_Data

  \brief A collection of test routines for the LOFAR_TBB class
 
  \author Lars B&auml;hren
 
  \date 2007/12/07
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test some basic operations using the Data Access Library

  This function runs a few basic tests on how to work with various objects of
  the Data Acces Library (DAL), such as dalDataset, dalGroup and dalArray.

  \return nofFailedTests -- The number of failed tests.
*/
int test_dal (std::string const &filename)
{
  cout << "\n[test_dal]\n" << endl;

  int nofFailedTests (0);
  int retval (0);
  std::string groupname ("Station001");
  char *h5filename = CR::string2char(filename);
  
  cout << "[1] Open dataset using DAL::Dataset object" << endl;
  try {
    dalDataset dataset;
    retval = dataset.open(h5filename);
    cout << "-- dalDataset type   = " << dataset.getType() << endl;
    cout << "-- dalDataset name   = " << dataset.getName() << endl;
    cout << "-- dalDataset tables = "; dataset.listTables();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Open dataset using DAL::Dataset object pointer" << endl;
  try {
    dalDataset *dataset;
    dataset = new dalDataset();
    retval = dataset->open(h5filename);
    cout << "-- Data type = " << dataset->getType() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Extract group from dalDataset" << endl;
  try {
    dalDataset dataset;
    retval = dataset.open(h5filename);

    dalGroup *group;
    group = dataset.openGroup(groupname);

    cout << "-- Data type  = " << dataset.getType() << endl;
    cout << "-- Group name = " << groupname         << endl;
    cout << "              = " << group->getName()  << endl;
    cout << "-- Group ID   = " << group->getId()    << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Extract times-series data for individual antennas" << endl;
  try {
    cout << "-- opening data set via dalDataset object ..." << endl;
    dalDataset dataset;
    retval = dataset.open(h5filename);
    cout << "-- Extract block of data for single signal channels ..." << endl;
    uint blocksize (1024);
    short buffer[blocksize];
    dataset.read_tbb("001000000",
		     0,
		     blocksize,
		     buffer);
    cout << "[" << buffer[0] << " " << buffer[1]
	 << " " << buffer[2] << " " << buffer[3]
	 << " " << buffer[4] << " " << buffer[5]
	 << " " << buffer[6] << " " << buffer[7]
	 << " ...]" << endl;
    dataset.read_tbb("001000001",
		     0,
		     blocksize,
		     buffer);
    cout << "[" << buffer[0] << " " << buffer[1]
	 << " " << buffer[2] << " " << buffer[3]
	 << " " << buffer[4] << " " << buffer[5]
	 << " " << buffer[6] << " " << buffer[7]
	 << " ...]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }  

  cout << "[5] Low-level test to directly interact with the file" << endl;
  try {
    hid_t file_id;
    hid_t group_id;
    std::vector<hid_t> dataset_id(4);

    cout << "-- opening file via H5Fopen() ..." << endl;
    H5Eset_auto(NULL, NULL);
    file_id = H5Fopen("testfile", H5F_ACC_RDWR, H5P_DEFAULT);
    cout << "-- file ID  = " << file_id << endl;
    file_id = H5Fopen(h5filename, H5F_ACC_RDWR, H5P_DEFAULT);
    cout << "-- file ID  = " << file_id << endl;

    cout << "-- opening station group via H5Gopen() ..." << endl;
    group_id = H5Gopen (file_id,"/Station001");
    cout << "-- group ID = " << group_id << endl;

    cout << "-- opening dataset via H5Fopen() ..." << endl;
    dataset_id[0] = H5Dopen(file_id, "/Station001/001000000");
    cout << "-- dataset ID = " << dataset_id[0] << endl;
    dataset_id[1] = H5Dopen(file_id, "/Station001/001000001");
    cout << "-- dataset ID = " << dataset_id[1] << endl;
    dataset_id[2] = H5Dopen(file_id, "/Station001/001000002");
    cout << "-- dataset ID = " << dataset_id[2] << endl;
    dataset_id[3] = H5Dopen(file_id, "/Station001/001000003");
    cout << "-- dataset ID = " << dataset_id[3] << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_TBB object

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_TBB (std::string const &filename)
{
  int nofFailedTests (0);
  
  cout << "\n[test_LOFAR_TBB]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  try {
    LOFAR_TBB data (filename);
    //
    data.summary(true); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
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
    std::cerr << "[tLOFAR_TBB] Too few parameters!" << endl;
    std::cerr << "" << endl;
    std::cerr << "  tLOFAR_TBB <filename>" << endl;
    std::cerr << "" << endl;
    return -1;
  }

  std::string filename = argv[1];

  // Perform some basic tests using the DAL
  nofFailedTests += test_dal(filename);

  // Test for the constructor(s)
  nofFailedTests += test_LOFAR_TBB (filename);
  
  return nofFailedTests;
}
