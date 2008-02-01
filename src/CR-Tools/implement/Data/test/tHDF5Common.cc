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

using std::cerr;
using std::cout;
using std::endl;

/*!
  \file tHDF5Common.cc

  \ingroup CR_Data

  \brief A collection of test routines for the HDF5Common class
 
  \author Lars B&auml;hren
 
  \date 2008/01/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test the additional methods to go along with the HDF5 functionality

  \return nofFailedTests -- The number of failed tests in this function.
*/
int test_support_methods ()
{
  cout << "\n[test_support_methods]\n" << endl;

  int nofFailedTests (0);
  uint nofElements (10);

  try {
    std::vector<int> vec (nofElements);
    //
    for (uint n(0); n<vec.size(); n++) {
      vec[n] = int(n);
    }
    //
    LOFAR::show_vector(std::cout,vec);
    cout << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  try {
    std::vector<double> vec (nofElements);
    //
    for (uint n(0); n<vec.size(); n++) {
      vec[n] = 0.5*n;
    }
    //
    LOFAR::show_vector(std::cout,vec);
    cout << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*
  \brief Test retrieval of an attribute's identifier within the HDF5 file

  \param file_id --
  
  \return nofFailedTests -- The number of failed tests within this function 
*/
int get_attribute_id (hid_t const &file_id)
{
  std::cout << "\n[get_attribute_id]\n" << endl;

  int nofFailedTests (0);
  hid_t attribute_id (0);

  /*
   * Get the ID of the station group
   */
  hid_t group_id = H5Gopen (file_id,
			    "Station001");
  
  if (group_id > 0) {

    std::cout << "[1] Retrieving IDs of group attributes..." << endl;

    attribute_id = H5Aopen_name(group_id,"TELESCOPE");
    std::cout << "-- TELESCOPE = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"OBSERVER");
    std::cout << "-- OBSERVER  = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"PROJECT");
    std::cout << "-- PROJECT   = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"OBS_ID");
    std::cout << "-- OBS_ID    = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"OBS_MODE");
    std::cout << "-- OBS_MODE  = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_TYPE");
    std::cout << "-- TRIG_TYPE = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_OFST");
    std::cout << "-- TRIG_OFST = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"TRIG_ANTS");
    std::cout << "-- TRIG_ANTS = " << attribute_id << endl;

    attribute_id = H5Aopen_name(group_id,"BEAM_DIR");
    std::cout << "-- BEAM_DIR  = " << attribute_id << endl;
  }

  /*
   * Get ID of the first dataset in the first station group
   */
  hid_t dataset_id = H5Dopen (file_id,
			      "Station001/001000000");

  if (dataset_id > 0) {

    std::cout << "[2] Retrieving IDs of dataset attributes..." << endl;

    attribute_id = H5Aopen_name(dataset_id,"STATION_ID");
    std::cout << "-- STATION_ID        = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"RSP_ID");
    std::cout << "-- RSP_ID            = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"RCU_ID");
    std::cout << "-- RCU_ID            = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLE_FREQ");
    std::cout << "-- SAMPLE_FREQ       = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"TIME");
    std::cout << "-- TIME              = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLE_NR");
    std::cout << "-- SAMPLE_NR         = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"SAMPLES_PER_FRAME");
    std::cout << "-- SAMPLES_PER_FRAME = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"DATA_LENGTH");
    std::cout << "-- DATA_LENGTH       = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"NYQUIST_ZONE");
    std::cout << "-- NYQUIST_ZONE      = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"FEED");
    std::cout << "-- FEED              = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"ANT_POSITION");
    std::cout << "-- ANT_POSITION      = " << attribute_id << endl;
    
    attribute_id = H5Aopen_name(dataset_id,"ANT_ORIENTATION");
    std::cout << "-- ANT_ORIENTATION   = " << attribute_id << endl;
    
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test extracting the values of attributes attached to a group or dataset

  \param file_id -- Identifier for the HDF5 file

  \return nofFailedTests -- The number of failed tests within this function.
*/
int get_attributes (hid_t const &file_id)
{
  std::cout << "\n[get_attributes]\n" << endl;
  
  int nofFailedTests (0);

  bool status (true);

  /*
   * Get the ID of the station group
   */
  hid_t group_id = H5Gopen (file_id,
			    "Station001");
  hid_t dataset_id = H5Dopen (file_id,
			      "Station001/001002021");
  
  if (group_id > 0) {
    std::cout << "[1] Retrieve attributes of type std::string" << endl;

    std::string telescope ("UNDEFINED");
    std::string observer ("UNDEFINED");
    std::string project ("UNDEFINED");
    std::string observation_id ("UNDEFINED");
    std::string observation_mode ("UNDEFINED");
    std::string trigger_type ("UNDEFINED");
    double trigger_offset (0);
    std::vector<int> triggered_antennas;
    std::vector<double> beam_direction;
    
    try {
      status = LOFAR::h5get_attribute (telescope,"TELESCOPE",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (observer,"OBSERVER",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (project,"PROJECT",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (observation_id,"OBS_ID",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (observation_mode,"OBS_MODE",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (trigger_type,"TRIG_TYPE",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (trigger_offset,"TRIG_OFST",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (triggered_antennas,"TRIG_ANTS",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (beam_direction,"BEAM_DIR",group_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    std::cout << "-- TELESCOPE          = " << telescope        << endl;
    std::cout << "-- OBSERVER           = " << observer         << endl;
    std::cout << "-- PROJECT            = " << project          << endl;
    std::cout << "-- OBSERVATION_ID     = " << observation_id   << endl;
    std::cout << "-- OBSERVATION_MODE   = " << observation_mode << endl;
    std::cout << "-- TRIGGER_TYPE       = " << trigger_type     << endl;
    std::cout << "-- TRIGGER_OFFSET     = " << trigger_offset   << endl;
    std::cout << "-- TRIGGERED_ANTENNAS = "; LOFAR::show_vector(std::cout,
							     triggered_antennas);
    std::cout << std::endl;
    std::cout << "-- BEAM_DIRECTION     = "; LOFAR::show_vector(std::cout,
							   beam_direction);
    std::cout << std::endl;

  } else {
    cerr << "[get_attributes] Unable to open station group!" << endl;
    nofFailedTests++;
  }

  if (dataset_id > 0) {
    std::cout << "[2] Retrieving attributes from dipole dataset ..." << endl;

    uint station_id (0);
    uint rsp_id (0);
    uint rcu_id (0);
    double sample_frequency (0);
    uint time (0);
    uint sample_number (0);
    uint samples_per_frame (0);
    uint data_length (0);
    uint nyquist_zone (0);
    std::vector<double> antenna_position;
    std::vector<double> antenna_orientation;

    try {
      status = LOFAR::h5get_attribute (station_id,"STATION_ID",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (rsp_id,"RSP_ID",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (rcu_id,"RCU_ID",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (sample_frequency,"SAMPLE_FREQ",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (time,"TIME",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (sample_number,"SAMPLE_NR",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (samples_per_frame,"SAMPLES_PER_FRAME",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (data_length,"DATA_LENGTH",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (nyquist_zone,"NYQUIST_ZONE",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (antenna_position,"ANT_POSITION",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    try {
      status = LOFAR::h5get_attribute (antenna_orientation,"ANT_ORIENTATION",dataset_id);
    } catch (std::string message) {
      cerr << message << endl;
      nofFailedTests++;
    }
    
    std::cout << "-- STATION_ID        = " << station_id        << endl;
    std::cout << "-- RSP_ID            = " << rsp_id            << endl;
    std::cout << "-- RCU_ID            = " << rcu_id            << endl;
    std::cout << "-- SAMPLE_FREQ       = " << sample_frequency  << endl;
    std::cout << "-- TIME              = " << time              << endl;
    std::cout << "-- SAMPLE_NUMBER     = " << sample_number     << endl;
    std::cout << "-- SAMPLES_PER_FRAME = " << samples_per_frame << endl;
    std::cout << "-- DATA_LENGTH       = " << data_length       << endl;
    std::cout << "-- NYQUIST_ZONE      = " << nyquist_zone      << endl;
    
  } else {
    cerr << "[get_attributes] Unable to open dipole dataset!" << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*
  \brief Test printing of a summary of an attribute's properties

  \param file_id -- Identifier for the HDF5 file.

  \return nofFailedTests -- The number of failed tests in this function.
*/
int attribute_summary (hid_t const &file_id)
{
  int nofFailedTests (0);

  // Open group containing the attributes we want to test
  hid_t group_id = H5Gopen (file_id,
			    "Station001");
  
  if (group_id > 0) {

    hid_t attribute_id;
    
  } else {
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
    Some tests do not require any external data input
  */
  nofFailedTests += test_support_methods ();

  /*
    Check if filename of the dataset is provided on the command line; if not
    exit the program.
  */
  if (argc < 2) {
    cerr << "[tHDF5Common] Too few parameters!" << endl;
    cerr << "" << endl;
    cerr << "  tHDF5Common <filename>" << endl;
    cerr << "" << endl;
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
    nofFailedTests += get_attribute_id (file_id);
    nofFailedTests += get_attributes (file_id);
    nofFailedTests += attribute_summary (file_id);
  } else {
    cerr << "[tHDF5Common] Error opening HDF5 file!" << endl;
    return -1;
  }

  return nofFailedTests;
}
