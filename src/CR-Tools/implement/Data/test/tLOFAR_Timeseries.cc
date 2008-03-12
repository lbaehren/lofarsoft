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

#include <Data/LOFAR_Timeseries.h>

using DAL::LOFAR_Timeseries;  // Namespace usage

/*!
  \file tLOFAR_Timeseries.cc

  \ingroup CR_Data

  \brief A collection of test routines for the LOFAR_Timeseries class
 
  \author Lars B&auml;hren
 
  \date 2008/02/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LOFAR_Timeseries object

  \param filename -- Name of the HDF5 file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_construction (std::string const &filename)
{
  std::cout << "\n[test_construction]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LOFAR_Timeseries newLOFAR_Timeseries;
    //
    newLOFAR_Timeseries.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    LOFAR_Timeseries newLOFAR_Timeseries (filename);
    //
    newLOFAR_Timeseries.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    std::cout << "--> creating original object ..." << std::endl;
    LOFAR_Timeseries timeseries (filename);
    timeseries.summary(); 
    //
    std::cout << "--> creating new object by copy ..." << std::endl;
    LOFAR_Timeseries timeseriesCopy (timeseries);
    timeseriesCopy.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various methods of the class

  \param filename -- Name of the HDF5 file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_methods (std::string const &filename)
{
  std::cout << "\n[test_methods]\n" << std::endl;

  int nofFailedTests = 0;
  LOFAR_Timeseries timeseries (filename);

  cout << "[1] Retrieving sample frequencies ..." << endl;
  try {
#ifdef HAVE_CASA
    casa::Vector<double> sampleFreq = timeseries.sample_frequencies();
    cout << "-- nof. values        = " << sampleFreq.nelements() << endl;
    cout << "-- Sample frequencies = " << sampleFreq << endl;
#else 
    std::vector<double> sampleFreq = timeseries.sample_frequencies();
    cout << "-- nof. values        = " << sampleFreq.size() << endl;
#endif
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Retrieve recording times ..." << endl;
  try {
#ifdef HAVE_CASA
    casa::Vector<uint> times = timeseries.times();
    cout << "-- nof. values        = " << times.nelements() << endl;
    cout << "-- Sample frequencies = " << times << endl;
#else 
    std::vector<uint> times = timeseries.times();
    cout << "-- nof. values        = " << times.size() << endl;
#endif
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrieval of TBB data

  \param filename -- Name of the HDF5 file used for testing

  \return nofFailedTests -- The number of failed tests.
*/
int test_data (std::string const &filename)
{
  std::cout << "\n[test_data]\n" << std::endl;

  int nofFailedTests = 0;
  int start          = 0;
  int nofSamples     = 1024;
  LOFAR_Timeseries timeseries (filename);

  std::cout << "[1] Retrieve time-series data without channel selection"
	    << std::endl;
  try {
    casa::Matrix<double> data = timeseries.fx (start,
					       nofSamples);
    // feedback 
    std::cout << "-- Data start     = " << start        << std::endl;
    std::cout << "-- Data blocksize = " << nofSamples   << std::endl;
    std::cout << "-- Data array     = " << data.shape() << std::endl;
    std::cout << "-- Data [0,]      = " << data.row(0)  << std::endl;
    std::cout << "-- Data [1,]      = " << data.row(1)  << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
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

  // Test for the constructor(s)
  nofFailedTests += test_construction (filename);

  if (nofFailedTests == 0) {
    nofFailedTests += test_methods (filename);
    nofFailedTests += test_data (filename);
  } else {
    std::cerr << "[tLOFAR_StationGroup]"
	      << " Skipping tests after testing constructors returned errors!"
	      << std::endl;
  }
  
  return nofFailedTests;
}
