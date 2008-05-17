/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

  <h3>Usage</h3>

  \verbatim
  tLOFAR_TBB <filename>
  \endverbatim
  where <tt>filename</tt> points to a existing LOFAR TBB data set in HDF5
  format.
*/

// -----------------------------------------------------------------------------

/*!
  \brief Access contents of the time-series datset directly via the HDF5 library

  While typically the interaction to the HDF5-based LOFAR time-series datasets
  is to take place via the DAL, once the data format is known this operation
  also can be carried out directly using the functionality of the HDF5 library
  routines. Since not all the required functionality is provided by the DAL (at
  least not at this point in time), we need to be able to access some of the 
  datasets contents directly; this function collects a number of calls to
  explore how this is done and what might need to be added to the DAL at some
  point to properly map the dataset structure onto a set of C++ objects.

  \param filename -- Test HDF5 time-series dataset to work with

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_hdf5 (std::string const &filename)
{
  cout << "\n[test_hdf5]\n" << endl;

  int nofFailedTests (0);
  std::string dataset_name ("001002021");

  herr_t h5error;
  hid_t file_id;
  hid_t attribute_id;
  hid_t stationGroup_id;
  hid_t channelDataset_id;
  int nofDatasetAttributes (0);

  /*
   * Open the HDF5 file; the library function returns a file handler which 
   * is to be used in subsequent interactions with the data.
   */
  cout << "--> Opening file ..." << endl;
  file_id = H5Fopen (filename.c_str(),
		     H5F_ACC_RDONLY,
		     H5P_DEFAULT);

  /*
   * Open station group; is this was successful, retrieve the number of 
   * attributes attached to the group.
   */
  if (file_id > 0) {
    cout << "--> Opening StationGroup ..." << endl;
    stationGroup_id = H5Gopen (file_id,
			       "Station001");
  } else {
    nofFailedTests++;
  }

  /*
   * Retrieve Attributes attached to the station group
   */

  std::string telescope ("UNDEFINED");
  std::string observer ("UNDEFINED");
  std::string project ("UNDEFINED");

  if (stationGroup_id > 0) {

    cout << "--> Reading in TELESCOPE attribute ..." << endl;
    try {
      // get the identifier for the attribute
      attribute_id = H5Aopen_name(stationGroup_id,
				  DAL::attribute_name(DAL::TELESCOPE).c_str());
      if (attribute_id > 0) {
	// dataspace of the attribute
	hid_t dataspace_id       = H5Aget_space (attribute_id);
	bool dataspace_is_simple = H5Sis_simple(dataspace_id);
	// datatype of the attribute
	hid_t datatype_id        = H5Aget_type (attribute_id);
	hsize_t datatype_size    = H5Tget_size (datatype_id);
	bool datatype_is_integer = H5Tdetect_class (datatype_id,H5T_INTEGER);
	bool datatype_is_float   = H5Tdetect_class (datatype_id,H5T_FLOAT);
	bool datatype_is_string  = H5Tdetect_class (datatype_id,H5T_STRING);
	// read the value of the attribute
	if (H5Tget_class (datatype_id) == H5T_STRING) {
	  char* data;
	  data = new char[datatype_size];
	  h5error = H5Aread(attribute_id,
			    datatype_id,
			    data);
	  telescope = data;
	}
// 	herr_t H5Aread(hid_t attr_id, hid_t mem_type_id, void *buf );
	// display atrribute properties
	cout << "\t-- Attribute value         = " << telescope << endl;
	cout << "\t-- Attribute ID            = " << attribute_id << endl;
	cout << "\t-- Dataspace ID            = " << dataspace_id << endl;
	cout << "\t-- Dataspace is simple?    = " << dataspace_is_simple << endl;
	cout << "\t-- Datatype ID             = " << datatype_id  << endl;
	cout << "\t-- Datatype size [Bytes]   = " << datatype_size << endl;
	cout << "\t-- Datatype is H5T_INTEGER = " << datatype_is_integer << endl;
	cout << "\t-- Datatype is H5T_FLOAT   = " << datatype_is_float << endl;
	cout << "\t-- Datatype is H5T_STRING  = " << datatype_is_string << endl;
	// release identifiers
	H5Sclose (dataspace_id);
	H5Tclose (datatype_id);
      }
      // Release identifiers
      H5Aclose (attribute_id);
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }

  }

  /*
   * Open dataset for an individual signal channel (dipole); f this was 
   * successful, retrieve the number of attributes attached to the dataset
   */
  if (stationGroup_id > 0) {
    cout << "--> Opening dipole dataset ..." << endl;
    channelDataset_id = H5Dopen (stationGroup_id,
				 dataset_name.c_str());
  } else {
    nofFailedTests++;
  }
  
  if (channelDataset_id > 0) {
    nofDatasetAttributes = H5Aget_num_attrs (channelDataset_id);
  } else {
    nofFailedTests++;
  }

  /*
   * Retrieve the attributes attached to the dataset
   */
  
  bool status (true);
  uint station_id (0);
  uint rsp_id (0);
  uint rcu_id (0);
  std::string feed ("UNDEFINED");
//   double *antenna_position;
//   double *antenna_orientation;
  
  if (channelDataset_id > 0) {

    cout << "--> Reading in STATION_ID attribute ..." << endl;
    try {
      status = DAL::h5get_attribute (station_id,
				       DAL::attribute_name(DAL::STATION_ID),
				       channelDataset_id);
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }

    cout << "--> Reading in RSP_ID attribute ..." << endl;
    try {
      status = DAL::h5get_attribute (rsp_id,
				       DAL::attribute_name(DAL::RSP_ID),
				       channelDataset_id);
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }

    cout << "--> Reading in RCU_ID attribute ..." << endl;
    try {
      status = DAL::h5get_attribute (rcu_id,
				       DAL::attribute_name(DAL::RCU_ID),
				       channelDataset_id);
    } catch (std::string message) {
      std::cerr << message << std::endl;
      nofFailedTests++;
    }
   
  }
  
  /*
   * Summary of the file properties/contents 
   */
  cout << "Summary of the file properties/contents:" << endl;
  cout << "-- File name            = " << filename                  << endl;
  cout << "-- File ID              = " << file_id                   << endl;
  cout << "-- Station group ID     = " << stationGroup_id           << endl;
  cout << "-- Dipole data channel:   " << endl;
  cout << "\t-- Dataset name       = " << dataset_name              << endl;
  cout << "\t-- Dataset ID         = " << channelDataset_id         << endl;
  cout << "\t-- nof. attributes    = " << nofDatasetAttributes      << endl;
  cout << "\t-- Station ID         = " << station_id                << endl;
  cout << "\t-- RSP ID             = " << rsp_id                    << endl;
  cout << "\t-- RCU ID             = " << rcu_id                    << endl;
  cout << "\t-- Feed type          = " << feed                      << endl;
  
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

  int nofFailedTests = 0;
  uint blocksize     = 1024;
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    LOFAR_TBB tbb;
    //
    tbb.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    LOFAR_TBB data (filename);
    //
    data.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing argumented constructor ..." << endl;
  try {
    LOFAR_TBB data (filename,
		    blocksize);
    //
    data.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing copy constructor ..." << endl;
  try {
    std::cout << "--> creating original object ..." << std::endl;
    LOFAR_TBB data (filename,
		    blocksize);
    data.summary();
    //
    std::cout << "--> creating new object as copy ..." << std::endl;
    LOFAR_TBB dataCopy (data);
    dataCopy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Test construction using pointer to DataReader object ..." << endl;
  try {
    cout << "-- creating pointer to DataReader object" << endl;
    CR::DataReader *dr;
    cout << "-- creating pointer to LOFAR_TBB object" << endl;
    CR::LOFAR_TBB *tbb = new CR::LOFAR_TBB (filename,
					    blocksize);
    cout << "-- assigning pointers from one object to the other" << endl;
    dr = tbb;
    // display summary
    tbb->summary();
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
int test_data (std::string const &filename)
{
  cout << "\n[test_data]\n" << endl;
  
  int nofFailedTests (0);
  uint blocksize (1024);

  // create object to handle the data
  LOFAR_TBB data (filename,
		  blocksize);
  
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
    casa::Matrix<casa::DComplex> fft = data.fft();

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

  cout << "[5] Read multiple blocks of ADC values ..." << endl;
  try {
    uint nofBlocks (10);
    uint nofDipoles = data.nofDipoleDatasets();
    casa::Matrix<double> fx (data.blocksize(),
			     nofDipoles);

    cout << "-- blocksize    = " << blocksize  << endl;
    cout << "-- nof. dipoles = " << nofDipoles << endl;
    cout << "-- shape(fx)    = " << fx.shape() << endl;

    for (uint block(0); block<nofBlocks; block++) {
      // get the data for the current block
      fx = data.fx();
      // feedback
      cout << "\t" << block << "/" << nofBlocks
	   << "\t" << min(fx)
	   << "\t" << mean(fx)
	   << "\t" << max(fx)
	   << endl;
      // increment the block-counter
      data.nextBlock();
    }

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

  // Perform some basic tests using the HDF5 library directly
//   nofFailedTests += test_hdf5(filename);

  // Perform some basic tests using the DAL

  // Test for the constructor(s)
  nofFailedTests += test_construction (filename);
  
  if (nofFailedTests == 0) {
    // Test reading in the data from a file
    nofFailedTests += test_data (filename);
  }
  
  return nofFailedTests;
}
