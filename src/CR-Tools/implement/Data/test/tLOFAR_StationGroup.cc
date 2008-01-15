/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_StationGroup.h>
#include <Utilities/StringTools.h>

// Namespace usage
using std::cout;
using std::endl;
using CR::LOFAR_StationGroup;

/*!
  \file tLOFAR_StationGroup.cc

  \ingroup Data

  \brief A collection of test routines for the LOFAR_StationGroup class
 
  \author Lars B&auml;hren
 
  \date 2007/12/10

  <h3>Usage</h3>
  
  To run the test program use:
  \verbatim
  tLOFAR_TBB <filename>
  \endverbatim
  where the <i>filename</i> points to an existing HDF5 time-series dataset.
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_StationGroup object

  \param filename -- Data file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_StationGroup (std::string const &filename)
{
  cout << "\n[test_LOFAR_StationGroup]\n" << endl;

  int nofFailedTests (0);
  
  // DAL Dataset object to handle the basic I/O
  dalDataset dataset;
  
  if (dataset.open(CR::string2char(filename))) {
    std::cerr << "Error opening datafile." << endl;
    nofFailedTests++;
  } else {
    
    cout << "[1] Testing default constructor ..." << endl;
    try {
      LOFAR_StationGroup group;
      //
      group.summary(); 
    } catch (std::string message) {
      std::cerr << message << endl;
      nofFailedTests++;
    }
    
    cout << "[2] Testing argumented constructor ..." << endl;
    try {
      LOFAR_StationGroup group (dataset,"Station001");
      //
      group.summary(); 
    } catch (std::string message) {
      std::cerr << message << endl;
      nofFailedTests++;
    }
    
  }
  
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
  LOFAR_StationGroup group (dataset,"Station001");

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
  
  cout << "[2] Retrieve the list of channels IDs" << endl;
  try {
    std::vector<std::string> channelIDs = group.channelIDs();
    uint nofChannels = channelIDs.size();

    cout << "-- nof. channel IDs = " << nofChannels    << endl;
    cout << "-- Channel IDs      = [";
    for (uint channel(0); channel<nofChannels; channel++) {
      cout << channelIDs[channel] << " ";
    }
    cout << "]" << endl;
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

  // Test for the constructor(s)
  nofFailedTests += test_LOFAR_StationGroup (filename);

  if (nofFailedTests == 0) {
    nofFailedTests += test_methods (filename);
  }

  return nofFailedTests;
}
