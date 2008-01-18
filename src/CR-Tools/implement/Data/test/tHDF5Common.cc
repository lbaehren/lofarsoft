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

/*!
  \brief Test extracting the values of attributes attached to a group or dataset

  \param file_id -- Identifier for the HDF5 file

  \return nofFailedTests -- The number of failed tests.
*/
int get_attributes (hid_t const &file_id)
{
  int nofFailedTests (0);

  bool status (true);
  std::string telescope;
  std::string observer;
  std::string project;

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
    nofFailedTests += get_attributes(file_id);
  } else {
    std::cerr << "[tHDF5Common] Error opening HDF5 file!" << std::endl;
    return -1;
  }

  return nofFailedTests;
}
