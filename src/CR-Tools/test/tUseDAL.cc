/*-------------------------------------------------------------------------*
 | $Id:: tUseCFITSIO.cc 1059 2007-11-19 10:43:30Z baehren                $ |
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

#include <dal/dalDataset.h>

using DAL::dalDataset;
using DAL::dalGroup;

#include <crtools.h>

// -----------------------------------------------------------------------------

/*!
  \file tUseDAL.cc

  \ingroup CR_test

  \brief A number of tests to work with the Data Access Library (DAL)

  \author Lars B&auml;hren

  \date 2008/04/04
*/

// -----------------------------------------------------------------------------

char* string2char (std::string const &mystring)
{
  int x (0);
  char* retChar;
  const char* tmpChar = mystring.c_str();
  
  retChar = new char [strlen(tmpChar)+1];
  
  do {
    retChar[x] = tmpChar[x];
  } while (tmpChar[x++] != '\0');
  
  return retChar;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test some basic operations using the Data Access Library

  This function runs a few basic tests on how to work with various objects of
  the Data Acces Library (DAL), such as dalDataset, dalGroup and dalArray.

  \param filename -- Test HDF5 time-series dataset to work with

  \return nofFailedTests -- The number of failed tests.
*/
int test_dalDataset (std::string const &filename)
{
  int nofFailedTests (0);

  int retval (0);
  std::string groupname = name_station;
  char *h5filename = string2char(filename);
  
  cout << "[1] Open dataset using DAL::Dataset object pointer" << endl;
  try {
    dalDataset *dataset;
    dataset = new dalDataset();
    retval = dataset->open(h5filename);
    cout << "-- Data type = " << dataset->getType() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Extract group from dalDataset" << endl;
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
    H5Eset_auto1(NULL, NULL);
    file_id = H5Fopen("testfile", H5F_ACC_RDWR, H5P_DEFAULT);
    cout << "-- file ID  = " << file_id << endl;
    file_id = H5Fopen(h5filename, H5F_ACC_RDWR, H5P_DEFAULT);
    cout << "-- file ID  = " << file_id << endl;

    cout << "-- opening station group via H5Gopen() ..." << endl;
    group_id = H5Gopen1 (file_id,"/Station001");
    cout << "-- group ID = " << group_id << endl;

    cout << "-- opening dataset via H5Fopen() ..." << endl;
    dataset_id[0] = H5Dopen1(file_id, "/Station001/001000000");
    cout << "-- dataset ID = " << dataset_id[0] << endl;
    dataset_id[1] = H5Dopen1(file_id, "/Station001/001000001");
    cout << "-- dataset ID = " << dataset_id[1] << endl;
    dataset_id[2] = H5Dopen1(file_id, "/Station001/001000002");
    cout << "-- dataset ID = " << dataset_id[2] << endl;
    dataset_id[3] = H5Dopen1(file_id, "/Station001/001000003");
    cout << "-- dataset ID = " << dataset_id[3] << endl;

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
    std::cerr << "[tUseDAL] Too few parameters!" << endl;
    std::cerr << "" << endl;
    std::cerr << "  tUseDAL <filename>" << endl;
    std::cerr << "" << endl;
    return -1;
  }

  std::string filename = argv[1];

  nofFailedTests += test_dalDataset (filename);

  return nofFailedTests;
}
