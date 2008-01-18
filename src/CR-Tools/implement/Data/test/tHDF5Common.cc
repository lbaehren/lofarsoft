/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Data/HDF5Common.h>

/*!
  \file tHDF5Common.cc

  \ingroup CR_Data

  \brief A collection of test routines for the HDF5Common class
 
  \author Lars B&auml;hren
 
  \date 2008/01/17
*/

// -----------------------------------------------------------------------------

/*
  \brief Test accessing the HDF5 file using the Data Access Library (DAL)

  \return nofFailedTests -- The number of failed tests.
*/
int test_dal ()
{
  int nofFailedTests (0);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int get_attribute_id (hid_t const &file_id)
{
  int nofFailedTests (0);
   hid_t attribute_id (0);

  /*
   * Get the ID of the station group
   */
  hid_t group_id = H5Gopen (file_id,
			    "Station001");
  
  if (group_id > 0) {

    std::cout << "-- Retrieving IDs of group attributes..." << std::endl;

    attribute_id = H5Aopen_name(group_id,"TELESCOPE");
    std::cout << "--> TELESCOPE = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"OBSERVER");
    std::cout << "--> OBSERVER  = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"PROJECT");
    std::cout << "--> PROJECT   = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"OBS_ID");
    std::cout << "--> OBS_ID    = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"OBS_MODE");
    std::cout << "--> OBS_MODE  = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_TYPE");
    std::cout << "--> TRIG_TYPE = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_OFST");
    std::cout << "--> TRIG_OFST = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_ANTS");
    std::cout << "--> TRIG_ANTS = " << attribute_id << std::endl;

    attribute_id = H5Aopen_name(group_id,"BEAM_DIR");
    std::cout << "--> BEAM_DIR  = " << attribute_id << std::endl;
  }

  /*
   * Get ID of the first dataset in the first station group
   */
  hid_t dataset_id = H5Dopen (file_id,
			      "Station001/001000000");

  if (dataset_id > 0) {

    std::cout << "-- Retrieving IDs of dataset attributes..." << std::endl;

    attribute_id = H5Aopen_name(dataset_id,"STATION_ID");
    std::cout << "--> STATION_ID        = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"RSP_ID");
    std::cout << "--> RSP_ID            = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"RCU_ID");
    std::cout << "--> RCU_ID            = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLE_FREQ");
    std::cout << "--> SAMPLE_FREQ       = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"TIME");
    std::cout << "--> TIME              = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLE_NR");
    std::cout << "--> SAMPLE_NR         = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLES_PER_FRAME");
    std::cout << "--> SAMPLES_PER_FRAME = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"DATA_LENGTH");
    std::cout << "--> DATA_LENGTH       = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"NYQUIST_ZONE");
    std::cout << "--> NYQUIST_ZONE      = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"FEED");
    std::cout << "--> FEED              = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"ANT_POSITION");
    std::cout << "--> ANT_POSITION      = " << attribute_id << std::endl;
    
    attribute_id = H5Aopen_name(dataset_id,"ANT_ORIENTATION");
    std::cout << "--> ANT_ORIENTATION   = " << attribute_id << std::endl;
    
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test extracting the values of attributes attached to a group or dataset

  \param file_id -- Identifier for the HDF5 file

  \return nofFailedTests -- The number of failed tests.
*/
int get_attributes (hid_t const &file_id)
{
  int nofFailedTests (0);

  bool status (true);
  std::string telescope ("UNDEFINED");
  std::string observer ("UNDEFINED");
  std::string project ("UNDEFINED");

  /*
   * Get the ID of the station group
   */
  hid_t group_id = H5Gopen (file_id,
			    "Station001");
  
  if (group_id > 0) {
    std::cout << "[get_attributes] Successfully opened station group." << std::endl;
  } else {
    std::cerr << "[get_attributes] Unable to open station group!" << std::endl;
    return 1;
  }

  /* [1] Retrieve attributes of type std::string */
  
  try {
    status = CR::h5get_attribute (telescope,"TELESCOPE",group_id);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  try {
    status = CR::h5get_attribute (telescope,"OBSERVER",group_id);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  try {
    status = CR::h5get_attribute (telescope,"PROJECT",group_id);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "-- TELESCOPE = " << telescope << std::endl;
  std::cout << "-- OBSERVER  = " << observer  << std::endl;
  std::cout << "-- PROJECT   = " << project   << std::endl;
  
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
    std::cerr << "[tHDF5Common] Too few parameters!" << endl;
    std::cerr << "" << endl;
    std::cerr << "  tHDF5Common <filename>" << endl;
    std::cerr << "" << endl;
    return -1;
  }

  /*
   * Open the HDF5 file; the library function returns a file handler which 
   * is to be used in subsequent interactions with the data.
   */
  hid_t file_id = H5Fopen (argv[1],
			   H5F_ACC_RDONLY,
			   H5P_DEFAULT);
  
  if (file_id > 0) {
    std::cout << "[tHDF5Common] Successfully opened HDF5 file." << std::endl;
    nofFailedTests += get_attribute_id (file_id);
    nofFailedTests += get_attributes (file_id);
  } else {
    std::cerr << "[tHDF5Common] Error opening HDF5 file!" << std::endl;
    return -1;
  }

  return nofFailedTests;
}
