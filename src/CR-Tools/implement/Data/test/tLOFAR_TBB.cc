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

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>

/* Namespace usage */
using std::cout;
using std::endl;
using CR::LOFAR_TBB;

/*!
  \file tLOFAR_TBB.cc

  \ingroup CR_Data

  \brief A collection of test routines for the CR::LOFAR_TBB class
 
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
int test_construction (std::string const &filename)
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  bool listStationGroups (true);
  bool listChannelIDs (true);
  
  cout << "[1] Testing argumented constructor ..." << endl;
  try {
    LOFAR_TBB data (filename);
    //
    data.summary(listStationGroups,listChannelIDs); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    uint blocksize (1024);
    LOFAR_TBB data (filename,blocksize);
    //
    data.summary(listStationGroups,listChannelIDs); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test reading in the data from the data set on disk

  \return nofFailedTests -- The number of failed tests.
*/
int test_io (std::string const &filename)
{
  cout << "\n[test_io]\n" << endl;
  
  int nofFailedTests (0);
  uint blocksize (1024);
  LOFAR_TBB data (filename,blocksize);
  
  cout << "[1] Reading one block of ADC values ..." << endl;
  try {
    casa::Matrix<double> fx = data.fx();

    cout << "-- shape (fx)     = " << fx.shape() << endl;
    cout << "-- ADC values [0] = ["
	 << fx(0,0) << " "
	 << fx(1,0) << " "
	 << fx(2,0) << " "
	 << fx(3,0) << " .. ]" << endl;
    cout << "-- ADC values [1] = ["
	 << fx(0,1) << " "
	 << fx(1,1) << " "
	 << fx(2,1) << " "
	 << fx(3,1) << " .. ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Reading one block of voltage values ..." << endl;
  try {
    casa::Matrix<double> voltage = data.voltage();

    cout << "-- shape (voltage)    = " << voltage.shape() << endl;
    cout << "-- Voltage values [0] = ["
	 << voltage(0,0) << " "
	 << voltage(1,0) << " "
	 << voltage(2,0) << " "
	 << voltage(3,0) << " .. ]" << endl;
    cout << "-- Voltage values [1] = ["
	 << voltage(0,1) << " "
	 << voltage(1,1) << " "
	 << voltage(2,1) << " "
	 << voltage(3,1) << " .. ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Reading FFT for one block of data ..." << endl;
  try {
    casa::Matrix<DComplex> fft = data.fft();

    cout << "-- shape (fft)    = " << fft.shape() << endl;
    cout << "-- FFT values [0] = ["
	 << fft(0,0) << " "
	 << fft(1,0) << " "
	 << fft(2,0) << " "
	 << fft(3,0) << " .. ]" << endl;
    cout << "-- FFT values [1] = ["
	 << fft(0,1) << " "
	 << fft(1,1) << " "
	 << fft(2,1) << " "
	 << fft(3,1) << " .. ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Reading calibrated FFT for one block of data ..." << endl;
  try {
    casa::Matrix<DComplex> calfft = data.calfft();

    cout << "-- shape (calfft)      = " << calfft.shape() << endl;
    cout << "-- Cal. FFT values [0] = ["
	 << calfft(0,0) << " "
	 << calfft(1,0) << " "
	 << calfft(2,0) << " "
	 << calfft(3,0) << " .. ]" << endl;
    cout << "-- Cal. FFT values [1] = ["
	 << calfft(0,1) << " "
	 << calfft(1,1) << " "
	 << calfft(2,1) << " "
	 << calfft(3,1) << " .. ]" << endl;
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
  nofFailedTests += test_construction (filename);
  
  if (nofFailedTests == 0) {
    nofFailedTests += test_io (filename);
  }
  
  return nofFailedTests;
}
