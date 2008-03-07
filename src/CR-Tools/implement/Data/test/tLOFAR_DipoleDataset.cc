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

#include <casa/Arrays/ArrayIO.h>
#include <dal/Enumerations.h>
#include <Data/LOFAR_DipoleDataset.h>

using std::cerr;
using std::cout;
using std::endl;

using DAL::LOFAR_DipoleDataset;

/*!
  \file tLOFAR_DipoleDataset.cc

  \ingroup CR_Data

  \brief A collection of test routines for the LOFAR_DipoleDataset class
 
  \author Lars B&auml;hren
 
  \date 2008/01/10
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_DipoleDataset object

  This function should provide tests for all the available constructors to a 
  new DAL::LOFAR_DipoleDataset object:
  \code
    LOFAR_DipoleDataset ();

    LOFAR_DipoleDataset (std::string const &filename,
			 std::string const &dataset);

    LOFAR_DipoleDataset (hid_t const &location,
			 std::string const &dataset);

    LOFAR_DipoleDataset (hid_t const &dataset_id);

    LOFAR_DipoleDataset (LOFAR_DipoleDataset const &other);
  \endcode
  
  \param filename -- Name of the HDF5 file, within which the dataset is located

  \return nofFailedTests -- The number of failed tests.
*/
int test_construction (std::string const &filename)
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  herr_t h5error;

  /*
   * Test for the default constructor taking no argument at all.
   */
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    LOFAR_DipoleDataset dataset;
    //
    dataset.summary();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * Test for the constructor taking file name and name of the dataset as arguments.
   */
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    LOFAR_DipoleDataset dataset (filename,
				 "Station001/001000000");
    //
    dataset.summary(); 
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();
  
  /*
   * Test for the constructor taking file identifier and name of the dataset as
   * arguments.
   */
  
  cout << "[3] Testing argumented constructor with file as anchor ..." << endl;
  try {
    hid_t file_id = H5Fopen (filename.c_str(),
			     H5F_ACC_RDONLY,
			     H5P_DEFAULT);

    if (file_id > 0) {
      LOFAR_DipoleDataset dataset (file_id,
				   "Station001/001000000");
      dataset.summary();
      h5error = H5Fclose (file_id);
    } else {
      cerr << "-- Error opening HDF5 file " << filename << endl;
      nofFailedTests++;
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * Instead of using the base of the file as reference location, also the group
   * within which the dataset is contained can serve as anchor point.
   */
  
  cout << "[4] Testing argumented constructor with group as anchor ..." << endl;
  try {
    // open file first
    hid_t file_id = H5Fopen (filename.c_str(),
			     H5F_ACC_RDONLY,
			     H5P_DEFAULT);
    
    if (file_id > 0) {
      // open group within which the dataset is located
      hid_t group_id = H5Gopen (file_id,
				"Station001");
      
      if (group_id > 0) {
	// create new object
	LOFAR_DipoleDataset dataset (group_id,
				     "001000000");
	// provide summary of object's properties
	dataset.summary(); 
	// release group ID
	h5error = H5Gclose (group_id);
      } else {
	cerr << "-- Error opening station group" << endl;
	nofFailedTests++;
      }

      // release file ID
      h5error = H5Fclose (file_id);
    } else {
      cerr << "-- Error opening HDF5 file " << filename << endl;
      nofFailedTests++;
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * Instead of using a ID/name conbination, we should be able to construct a
   * new object directly from a provided identifier for the dataset within the HDF5
   * file.
   */
  
  cout << "[5] Testing argumented constructor with dataset identifier ..." << endl;
  try {
    // open file first
    hid_t file_id = H5Fopen (filename.c_str(),
			     H5F_ACC_RDONLY,
			     H5P_DEFAULT);
    
    if (file_id > 0) {
      // open dataset within which the dataset is located
      hid_t dataset_id = H5Dopen (file_id,
				"Station001/001000000");
      
      if (dataset_id > 0) {
	// create new object
	LOFAR_DipoleDataset dataset (dataset_id);
	// provide summary of object's properties
	dataset.summary(); 
	// release dataset ID
	h5error = H5Dclose (dataset_id);
      } else {
	cerr << "-- Error opening dipole dataaset!" << endl;
	nofFailedTests++;
      }

      // release file ID
      h5error = H5Fclose (file_id);
    } else {
      cerr << "-- Error opening HDF5 file " << filename << endl;
      nofFailedTests++;
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * Test copy constructor, as required to directly construct a new object from an
   * existing one.
   */

  cout << "[6] Testing copy constructor ..." << endl;
  try {
    cout << "-- creating original object ..." << endl;
    LOFAR_DipoleDataset dataset (filename,
 				 "Station001/001000000");
    dataset.summary();
    //
    cout << "-- creating new object as copy ..." << endl;
    LOFAR_DipoleDataset datasetCopy (dataset);
    datasetCopy.summary();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();
  
  return nofFailedTests;
  }

// -----------------------------------------------------------------------------

/*!
  \brief Test handling of multiple LOFAR_DipoleDataset objects

  One of the essentional issues for mapping the interal structure of a HDF5
  TBB time-series file onto a set up C++ objects is the possibility for
  higher-level structure elements to contain a collection of lower-level ones.
  The prefered way to do this is to use a construct such as 
  \code 
  std::vector<DAL::LOFAR_DipoleDataset> datasets;
  \endcode
  so we should be testing well in advance that such a construct behaves as
  expected.

  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_datasets (std::string const &filename)
{
  cout << "\n[test_datasets]\n" << endl;

  int nofFailedTests (0);
  hid_t file_id (0);
  hid_t dataset_id (0);
  herr_t h5error;
  std::vector<std::string> dataset_names;
  uint nofDatasets (0);

  dataset_names.push_back ("Station001/001000000");
  dataset_names.push_back ("Station001/001000001");
  dataset_names.push_back ("Station001/001000002");
  dataset_names.push_back ("Station001/001000003");
  dataset_names.push_back ("Station001/001000004");

  nofDatasets = dataset_names.size();

  /*
   * TEST:
   * Open the HDF5 file for further reading
   */

  cout << "[1] Opening the HDF5 file containing the data ..." << endl;
  file_id = H5Fopen (filename.c_str(),
		     H5F_ACC_RDONLY,
		     H5P_DEFAULT);

  if (file_id > 0) {
    cout << "--> File name = " << filename << endl;
    cout << "--> File ID   = " << file_id  << endl;
  } else {
    return 0;
  }

  /* 
   * TEST: Use the basic methods provided by the HDF5 library to open a number
   *       datasets contained within the file. The object identifiers are
   *       collected in a STD vector.
   */

  cout << "[2] Opening dipole datasets ..." << endl;
  try {
    // additional local variables
    std::vector<hid_t> dataset_ids;
    uint numDataset (0);

    cout << "-- start opening datasets within the file ..." << endl;
    for (uint n(0); n<dataset_names.size(); n++) {
      dataset_id = H5Dopen (file_id,
			    dataset_names[n].c_str());
      // check if opening of dataset was successful
      if (dataset_id > 0) {
	dataset_ids.push_back (dataset_id);
	cout << "--> " << dataset_names[n] << " = " << dataset_ids[numDataset] << endl;
	numDataset++;
      }
    }

    cout << "-- cleaning up the list of dataset identifiers ..." << endl;
    for (uint n(0); n<dataset_ids.size(); n++) {
      h5error = H5Dclose (dataset_ids[n]);
      cout << "--> " << dataset_ids[n] << " : " << h5error << endl;
    }

  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * TEST: While before we have been collecting the IDs for a set of non-identical
   *       objects in the file, this time we attempt to open up the same object in
   *       HDF5 file multiple time. Though this might not be advisable in terms of
   *       consistency, this at least should be ok in terms of simply applying the
   *       open() operation multiple times.
   */
  
  cout << "[3] Attempt opening dataset multiple times ..." << endl;
  try {
    // additional local variables
    int numDataset (0);
    std::vector<hid_t> dataset_ids;

    for (uint n(0); n<dataset_names.size(); n++) {
      // try to open the dataset
      dataset_id = H5Dopen (file_id,
			    dataset_names[numDataset].c_str());
      // check if opening of dataset was successful
      if (dataset_id > 0) {
	dataset_ids.push_back (dataset_id);
	//
	cout << "--> " << dataset_names[numDataset] << " = " << dataset_id << endl;
      }
    }

    // clean up the list of identifiers
    for (uint n(0); n<dataset_ids.size(); n++) {
      h5error = H5Dclose (dataset_ids[n]);
    }

  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * TEST: Use std::vector as container for keeping a set of multiple
   *       LOFAR_DipoleDataset objects. This will be required by classes
   *       representing higher-level structures of the HDF5 file, such as e.g.
   *       LOFAR_StationGroup.
   */

  cout << "[5] Creating a collection of dipole dataset objects ..." << endl;
  try {
    std::vector<DAL::LOFAR_DipoleDataset> datasets;

    for (uint n(0); n<nofDatasets; n++) {
      // try to open the dataset
      dataset_id = H5Dopen (file_id,
			    dataset_names[n].c_str());
      h5error = H5Eclear();
      // if HDF5 object exists, create LOFAR_DipoleDataset object for it
      if (dataset_id > 0) {
 	cout << "-- " << dataset_names[n] << endl;
	// create LOFAR_DipoleDataset object new to the list
	LOFAR_DipoleDataset dataset (dataset_id);
  	datasets.push_back (LOFAR_DipoleDataset(dataset_id));
	// feedback
	cout << "--> Dateset ID      = " << dataset.dataset_id()    << endl;
	cout << "--> nof. attributes = " << dataset.nofAttributes() << endl;
      }
      // release the identifier for the located dataset object
      h5error = H5Dclose (dataset_id);
      h5error = H5Eclear();
    }
    cout << "--> Located and assigned " << datasets.size() << " datasets." << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  // release the opened data file
  if (file_id > 0) {
    h5error = H5Fclose (file_id);
    h5error = H5Eclear();
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the methods used for retrieval of parameters/private data
  
  \param filename -- Name of the HDF5 file, within which the dataset is located

  \return nofFailedTests -- The number of failed tests.
*/
int test_parameters (std::string const &filename)
{
  cout << "\n[test_parameters]\n" << endl;

  int nofFailedTests (0);
  herr_t h5error;

  // create object to use for the subsequent tests
  LOFAR_DipoleDataset dataset (filename,
			       "Station001/001000000");

  cout << "[1] Retrieve object parameters ..." << endl;
  try {
    cout << "-- Class name = " << dataset.className()  << endl;
    cout << "-- Dataset ID = " << dataset.dataset_id() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  cout << "[2] Retrieve atomic valued attributes ..." << endl;
  try {
    uint station_id         = dataset.station_id();
    uint rsp_id             = dataset.rsp_id();
    uint rcu_id             = dataset.rcu_id();
    double sample_frequency = dataset.sample_frequency();
    uint nyquist_zone       = dataset.nyquist_zone();
    uint time               = dataset.time();
    uint sample_number      = dataset.sample_number();
    uint samples_per_frame  = dataset.samples_per_frame();
    // 
    cout << "-- STATION_ID        = " << station_id        << endl;
    cout << "-- RSP_ID            = " << rsp_id            << endl;
    cout << "-- RCU_ID            = " << rcu_id            << endl;
    cout << "-- SAMPLE_FREQUENCY  = " << sample_frequency  << endl;
    cout << "-- NYQUIST_ZONE      = " << nyquist_zone      << endl;
    cout << "-- TIME              = " << time              << endl;
    cout << "-- SAMPLE_NUMBER     = " << sample_number     << endl;
    cout << "-- SAMPLES_PER_FRAME = " << samples_per_frame << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  cout << "[3] Retrieve vector valued attributes ..." << endl;
  try {
    casa::Vector<double> antenna_position    = dataset.antenna_position();
    casa::Vector<double> antenna_orientation = dataset.antenna_orientation();
    //
    cout << "-- ANTENNA_POSITION    = " << antenna_position    << endl;
    cout << "-- ANTENNA_ORIENTATION = " << antenna_orientation << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the retrieval of the actual TBB data contained within the dataset
  
  \param filename -- Name of the HDF5 file, within which the dataset is located

  \return nofFailedTests -- The number of failed tests.
*/
int test_data (std::string const &filename)
{
  cout << "\n[test_data]\n" << endl;

  int nofFailedTests (0);
  bool status (true);
  int start (0);
  int blocksize (10);

  // open dataset
  LOFAR_DipoleDataset dataset (filename,
			       "Station001/001000001");
  
  std::cout << "[1] Retrieve data via pointer to array ..." << std::endl;
  try {
    short *dataBuffer;

    dataBuffer = new short [blocksize];

    status = dataset.fx(start,
			blocksize,
			dataBuffer);
    
    if (status) {
      std::cout << "-- Channel name = " << dataset.channelName() << std::endl;
      std::cout << "-- Data start   = " << start     << std::endl;
      std::cout << "-- Blocksize    = " << blocksize << std::endl;
      std::cout << "-- Channel data = ["
		<< dataBuffer[0] << ", "
		<< dataBuffer[1] << ", "
		<< dataBuffer[2] << ", "
		<< dataBuffer[3] << ", .. "
		<< dataBuffer[blocksize-1] << " ]"
		<< std::endl;
    } else {
      std::cerr << "--> Error retrieving time-series data!" << std::endl;
      nofFailedTests++;
    }

  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Retrieve data into casa::Vector ..." << std::endl;
  try {
    casa::Vector<double> data = dataset.fx (start,blocksize);

    if (data.size() == blocksize) {
      std::cout << "-- Channel name = " << dataset.channelName() << std::endl;
      std::cout << "-- Data start   = " << start                 << std::endl;
      std::cout << "-- Blocksize    = " << blocksize             << std::endl;
      std::cout << "-- Channel data = " << data                  << std::endl;
    } else {
      std::cerr << "--> Error retrieving time-series data!" << std::endl;
      nofFailedTests++;
    }

  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test export of the attribute values to a casa::Record object
  
  \param filename -- Name of the HDF5 file, within which the dataset is located

  \return nofFailedTests -- The number of failed tests.
*/
int test_export2record (std::string const &filename)
{
  cout << "\n[test_export2record]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Retrieve attributes of dataset into record ..." << endl;
  try {
    // open dataset
    LOFAR_DipoleDataset dataset (filename,
				 "Station001/001000001");
    // retrieve attributes into record
    casa::Record record = dataset.attributes2record ();
    // feedback
    cout << "-- nof. attributes    = " << dataset.nofAttributes() << endl;
    cout << "-- nof. record fields = " << record.nfields() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Combine records from multiple dipole datasets ..." << endl;
  try {
    // open datasets from which to extract records
    LOFAR_DipoleDataset dataset1 (filename,
				  "Station001/001000001");
    LOFAR_DipoleDataset dataset2 (filename,
				  "Station001/001000002");
    // description of the record's structure
    casa::RecordDesc descriptionRecord;
    casa::RecordDesc descriptionSubRecord = dataset1.recordDescription();
    descriptionRecord.addField ("001000001",descriptionSubRecord);
    descriptionRecord.addField ("001000002",descriptionSubRecord);
    // create the record
    casa::Record record(descriptionRecord);
    // feedback
    cout << "-- nof. record fields = " << record.nfields() << endl;
  } catch (std::string message) {
    cerr << message << endl;
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
  nofFailedTests += test_construction (filename);
  // Test working with collection of multiple objects
  nofFailedTests += test_datasets (filename);
  // Test retrieval of parameters/attributes attached to the dataset
  nofFailedTests += test_parameters (filename);
  // Test retrieval of the actual TBB time-series values
  nofFailedTests += test_data (filename);
  // Test exporting the attributes to a casa::Record
  nofFailedTests += test_export2record (filename);

  return nofFailedTests;
}
