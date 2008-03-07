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

#include <iostream>

#include <casa/Arrays/Vector.h>

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_StationGroup.h>
#include <Utilities/StringTools.h>

// Namespace usage
using std::cout;
using std::endl;
using DAL::LOFAR_StationGroup;

/*!
  \file tLOFAR_StationGroup.cc

  \ingroup CR_Data

  \brief A collection of test routines for the LOFAR_StationGroup class
 
  \author Lars B&auml;hren
 
  \date 2007/12/10

  <h3>Usage</h3>
  
  To run the test program use:
  \verbatim
  tLOFAR_StationGroup <filename>
  \endverbatim
  where the <i>filename</i> points to an existing HDF5 time-series dataset.
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test localization and handling of datasets inside the group

  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_datasets (std::string const &filename)
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  herr_t h5error (0);
  hid_t fileID (0);
  hid_t groupID (0);

  try {
    cout << "-- opening HDF5 file ..." << endl;
    fileID = H5Fopen (filename.c_str(),
		      H5F_ACC_RDONLY,
		      H5P_DEFAULT);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  if (fileID > 0) {
    
    cout << "-- opening HDF5 group ..." << endl;
    groupID = H5Gopen (fileID,
		       "Station001");
    
    if (groupID > 0) {
      hsize_t nofObjects;
      // feedback
      cout << "--> successfully opened HDF5 group" << endl;
      // Number of objects in the group specified by its identifier
      h5error = H5Gget_num_objs(groupID,
				&nofObjects);
      cout << "--> " << nofObjects << " objects found in group." << endl;
      // go through the list of objects and extract the datasets
      for (hsize_t n(0); n<nofObjects; n++) {
	if (H5G_DATASET == H5Gget_objtype_by_idx (groupID,n)) {
	  cout << "\tObject " << n << " is a dataset" << endl;
	}
      }
      // close HDF5 group
      h5error = H5Gclose (groupID);
    } else {
      std::cerr << "--> Unable to perform test; invalid group ID!" << endl;
    }
    
    // close file
    h5error = H5Fclose (fileID);
    
  } else {
    std::cerr << "--> Unable to perform test; invalid file ID!" << endl;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_StationGroup object

  This function should provide tests for all the available constructors to a 
  new DAL::LOFAR_StationGroup object:
  \code
    LOFAR_StationGroup ();

    LOFAR_StationGroup (std::string const &filename,
			std::string const &group);

    LOFAR_StationGroup (hid_t const &location,
			std::string const &group);

    LOFAR_StationGroup (hid_t const &group_id);

    LOFAR_StationGroup (LOFAR_StationGroup const &other);
  \endcode
  
  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_construction (std::string const &filename)
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  herr_t h5error (0);
  
  // open the HDF5 for further access
  hid_t file_id = H5Fopen (filename.c_str(),
			   H5F_ACC_RDONLY,
			   H5P_DEFAULT);

  /*
   * TEST: Default constructor
   */
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    LOFAR_StationGroup group;
    //
    group.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * TEST: Argumented constructor using filename and name of the group as
   *       input parameters.
   */
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    LOFAR_StationGroup group (filename,
			      "Station001");
    //
    group.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();
  
  /*
   * TEST: Argumented constructor using file identifier (as obtained from 
   *       previous call to HDF5 library) and group name as input parameters.
   */  
  
  cout << "[3] Testing argumented constructor ..." << endl;
  try {    
    if (file_id > 0) {
      LOFAR_StationGroup group (file_id,
				"Station001");
      group.summary(); 
    } else {
      std::cerr << "--> Unable to perform test; invalid file ID!" << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * TEST: Argumented constructor using object identifier for the group as
   *       input parameter.
   */

  cout << "[4] Testing argumented constructor ..." << endl;
  try {
    if (file_id > 0) {
      // retrieve the group ID 
      hid_t groupID = H5Gopen (file_id,
			       "Station001");
      // contiue if group successfully opened
      if (groupID > 0) {
	// feedback
	cout << "--> Passed group ID = " << groupID << endl;
	// create new object
	LOFAR_StationGroup group (groupID);
	group.summary(); 
      } else {
	std::cerr << "--> Unable to perform test; invalid group ID!" << endl;
      }
    } else {
      std::cerr << "--> Unable to perform test; invalid file ID!" << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  /*
   * TEST: 
   */

  cout << "[5] Testing copy constructor ..." << endl;
  try {
    cout << "--> creating original object ..." << endl;
    LOFAR_StationGroup group (filename,
			      "Station001");
    group.summary();
    //
    cout << "--> creating new object as copy ..." << endl;
    LOFAR_StationGroup groupCopy (group);
    groupCopy.summary();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  h5error = H5Eclear();

  // release file identifier
  h5error = H5Fclose (file_id);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test identification and access of groups a root level of the HDF5 file
  
  \param filename -- Data file used for testing
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_groups (std::string const &filename)
{
  cout << "\n[test_groups]\n" << endl;

  bool status        = true;
  int nofFailedTests = 0;
  hsize_t nofObjects = 0;
  int objectType     = 0;
  std::string name;
  herr_t h5error     = 0;
  
  // open the HDF5 for further access
  hid_t fileID = H5Fopen (filename.c_str(),
			  H5F_ACC_RDONLY,
			  H5P_DEFAULT);
  
  h5error = H5Gget_num_objs(fileID,
			    &nofObjects);

  std::cout << "-- File ID      = " << fileID     << std::endl;
  std::cout << "-- nof. objects = " << nofObjects << std::endl;

  for (hsize_t n(0); n<nofObjects; n++) {
    // retrieve the object type
    objectType = H5Gget_objtype_by_idx (fileID,
					n);
    // ... and report the finding
    switch (objectType) {
    case H5G_DATASET:
      std::cout << "--> Object " << n << " is of type H5G_DATASET." << std::endl;
      break;
    case H5G_GROUP:
      std::cout << "--> Object " << n << " is of type H5G_GROUP." << std::endl;
      // get the name of the group
      status = DAL::h5get_name (name,
				fileID,
				n);
      if (status) {
	std::cout << "--> Group name = " << name << std::endl;
      }
      break;
    case H5G_LINK:
      std::cout << "--> Object " << n << " is of type H5G_LINK." << std::endl;
      break;
    }
  }
  
  // release the file ID
  h5error = H5Fclose (fileID);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various methods provided by the class

  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_methods (std::string const &filename)
{
  cout << "\n[test_methods]\n" << endl;

  int nofFailedTests (0);
  dalDataset dataset;

  // open file into dataset object
  cout << "-- opening dataset via DAL ... " << std::flush;
  dataset.open(CR::string2char(filename));
  cout << "OK" << endl;
  // create LOFAR_StationGroup object to continue working with
  LOFAR_StationGroup group (filename,"Station001");

  cout << "[1] Convert individual ID numbers to channel ID" << endl;
  try {
    uint station_id (0);
    uint rsp_id (0);
    uint rcu_id (0);
    
    for (rcu_id=0; rcu_id<12; rcu_id++) {
      cout << "\t[" << station_id << "," << rsp_id << "," << rcu_id
		<< "] \t->\t"
		<< group.channelID(station_id,rsp_id,rcu_id)
		<< endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Retrieve the list of channels names and IDs" << endl;
  try {
    std::vector<std::string> channelNames = group.channelNames();
    std::vector<int> channelIDs           = group.channelIDs();
    uint nofChannels = channelNames.size();
    
    cout << "-- nof. channels = " << nofChannels    << endl;
    cout << "-- Channel names = [";
    for (uint channel(0); channel<nofChannels; channel++) {
      cout << channelNames[channel] << " ";
    }
    cout << "]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrieval of the actual time-series data form the dipoles

  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_data (std::string const &filename)
{
  cout << "\n[test_data]\n" << endl;

  int nofFailedTests (0);
  LOFAR_StationGroup group (filename,"Station001");
  int start (0);
  int blocksize (1024);

  std::cout << "[1] Retrieve data for all dipoles ..." << std::endl;
  try {
    casa::Matrix<double> data = group.fx (start,
					  blocksize);
    // feedback 
    std::cout << "-- Data start     = " << start        << std::endl;
    std::cout << "-- Data blocksize = " << blocksize    << std::endl;
    std::cout << "-- Data array     = " << data.shape() << std::endl;
    std::cout << "-- Data [0,]      = " << data.row(0)  << std::endl;
    std::cout << "-- Data [1,]      = " << data.row(1)  << std::endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Retrieve data for selected dipoles ..." << std::endl;
  try {
    // set up the dipole selection
    std::vector<uint> selection;
    selection.push_back(0);
    selection.push_back(1);
    selection.push_back(2);
    casa::Matrix<double> data = group.fx (start,
					  blocksize,
					  selection);
    // feedback 
    std::cout << "-- Data start     = " << start        << std::endl;
    std::cout << "-- Data blocksize = " << blocksize    << std::endl;
    std::cout << "-- Data array     = " << data.shape() << std::endl;
    std::cout << "-- Data [0,]      = " << data.row(0)  << std::endl;
    std::cout << "-- Data [1,]      = " << data.row(1)  << std::endl;
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
    std::cerr << "[tLOFAR_StationGroup] Too few parameters!" << endl;
    std::cerr << "" << endl;
    std::cerr << "  tLOFAR_StationGroup <filename>" << endl;
    std::cerr << "" << endl;
    return -1;
  }

  std::string filename = argv[1];

  // Test localization and handling of datasets inside the group
  nofFailedTests += test_datasets (filename);

  // Test for the constructor(s)
  nofFailedTests += test_construction (filename);
  
  if (nofFailedTests == 0) {
    nofFailedTests += test_groups(filename);
//     nofFailedTests += test_methods (filename);
    nofFailedTests += test_data(filename);
   }

  return nofFailedTests;
}
