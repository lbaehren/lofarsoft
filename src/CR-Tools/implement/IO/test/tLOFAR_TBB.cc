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

#include <crtools.h>

#include <IO/LOFAR_TBB.h>
#include <Utilities/StringTools.h>

#include <casa/Arrays/Matrix.h>

/* Namespace usage */
using std::cout;
using std::endl;
using CR::LOFAR_TBB;

/*!
  \file tLOFAR_TBB.cc

  \ingroup IO

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

//_______________________________________________________________________________
//                                                                           show

/*!
  \param vec -- Vector of which to display the first \e nelem elements
  \param nelem -- Number of elements to display from the vector \e vec.
*/
template <class T> void show (std::vector<T> const &vec,
			      unsigned int nelem=4)
{
  cout << "\t[ ";
  
  for (unsigned int n(0); n<nelem; ++n) {
    cout << vec[n] << ", ";
  }

  cout << ".. ]" << endl;
}

//_______________________________________________________________________________
//                                                              test_construction

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
    cout << "--> creating original object ..." << std::endl;
    LOFAR_TBB data (filename,
		    blocksize);
    data.summary();
    //
    cout << "--> creating new object as copy ..." << std::endl;
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

//_______________________________________________________________________________
//                                                              test_sampleValues

/*!
  \brief Test retrieval of the values along the sample axis

  \return nofFailedTests -- The number of failed tests.
*/
int test_sampleValues ()
{
  cout << "\n[tLOFAR_TBB::test_sampleValues]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (1024);
  uint nyquistZone (1);
  double sampleFrequency (200e06);
  std::vector<uint> samples;
  uint maxOffset (5);

  //________________________________________________________
  // Create LOFAR_TBB object to perform the tests with

  LOFAR_TBB dr;
  dr.setBlocksize(blocksize);
  dr.setNyquistZone (nyquistZone);
  dr.setSampleFrequency (sampleFrequency);

  //________________________________________________________
  // Run the tests

  cout << "[1] sampleValues (samples) ..." << endl;
  {
    dr.sampleValues(samples);
    // 
    show(samples);
  }

  cout << "[2] sampleValues (samples,offset) ..." << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.sampleValues(samples,offset);
      // 
      show(samples);
    }
  }
  
  cout << "[3] sampleValues (samples,offset,false) ..." << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.sampleValues(samples,offset,false);
      // 
      show(samples);
    }
  }

  cout << "[4] setSampleOffset (offset) ..." << endl;
  {
    dr.setSampleOffset(0);
    
    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset);
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
    }
  }
  
  cout << "[5] setSampleOffset (offset,false) ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset,false);
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
    }
  }
  
  cout << "[6] nextBlock () ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
      // increment block counter
      dr.nextBlock();
    }
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                              test_headerRecord

/*!
  \brief Test working with the header record storing metadata

  \param filename -- Name of the HDF5 dataset to work with
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_headerRecord (std::string const &filename)
{
  cout << "\n[test_data]\n" << endl;
  
  int nofFailedTests (0);
  uint blocksize (1024);

  cout << "[1] Retrieve the header record from the LOFAR_TBB object ..." << endl;
  try {
    LOFAR_TBB data (filename,
		    blocksize);
    casa::Record rec = data.attributes2headerRecord();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Set header record from external values ..." << endl;
  try {
    bool status (true);
    // new LOFAR_TBB object to test
    LOFAR_TBB data (filename,
		    blocksize);
    // set up record
    casa::Record rec;
    rec.define("Date",0);
    rec.define("AntennaIDs",casa::Vector<int>(1,1));
    rec.define("Observatory","LOFAR");
    rec.define("Filesize",1);
    // try writing the record back into the LOFAR_TBB object
    status = data.setHeaderRecord (rec);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                      test_data

/*!
  \brief Test reading in the data from the data set on disk
  
  \param filename -- Name of the HDF5 dataset to work with

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
//    cout << "-- ADC values [1] = ["
//	 << fx(0,1) << " "
//	 << fx(1,1) << " "
//	 << fx(2,1) << " "
//	 << fx(3,1) << " .. ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

	// test changing the blocksize
	blocksize = 10240;
	cout << "[1a] changing blocksize from 1024 to " << blocksize << "...";
	data.setBlocksize(blocksize);
	cout << "changed." << endl;
		
  cout << "[2] Reading one block of voltage values ..." << endl;
  try {
    casa::Matrix<double> voltage = data.voltage();

    cout << "-- shape (voltage)    = " << voltage.shape() << endl;
    cout << "-- Voltage values [0] = ["
	 << voltage(0,0) << " "
	 << voltage(1,0) << " "
	 << voltage(2,0) << " "
	 << voltage(3,0) << " .. ]" << endl;
//    cout << "-- Voltage values [1] = ["
//	 << voltage(0,1) << " "
//	 << voltage(1,1) << " "
//	 << voltage(2,1) << " "
//	 << voltage(3,1) << " .. ]" << endl;
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
//    cout << "-- FFT values [1] = ["
//	 << fft(0,1) << " "
//	 << fft(1,1) << " "
//	 << fft(2,1) << " "
//	 << fft(3,1) << " .. ]" << endl;
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
//    cout << "-- Cal. FFT values [1] = ["
//	 << calfft(0,1) << " "
//	 << calfft(1,1) << " "
//	 << calfft(2,1) << " "
//	 << calfft(3,1) << " .. ]" << endl;
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

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  bool haveDataset (false);
  std::string filename;

  //________________________________________________________
  // Process parameters from the command line
  
  if (argc < 2) {
    haveDataset = false;
  } else {
    filename    = argv[1];
    haveDataset = true;
  }

  //________________________________________________________
  // Run the tests

  // Perform some basic tests using the HDF5 library directly
//   nofFailedTests += test_hdf5(filename);

  // Perform some basic tests using the DAL

  nofFailedTests += test_sampleValues ();
  
  if (haveDataset) {
    // Test for the constructor(s)
    nofFailedTests += test_construction (filename);
    // Test working with the header record storing metadata
    nofFailedTests += test_headerRecord (filename);
    // Test reading in the data from a file
    nofFailedTests += test_data (filename);
  } else {
    std::cout << "[tLOFAR_TBB] Skipping test needing test dataset." << endl;
  }
  
  return nofFailedTests;
}
