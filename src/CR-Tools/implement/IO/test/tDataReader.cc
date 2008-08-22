/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

// CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>

// LOPES header files
#include <IO/DataReader.h>

using std::cout;
using std::endl;

using casa::DComplex;
using CR::DataReader;

/*!
  \file tDataReader.cc

  \ingroup IO

  \brief A collection of test routines for DataReader
 
  \author Lars B&auml;hren
 
  \date 2005/05/15
*/

// ----------------------------------------------------------------- test_Record

/*!
  \brief Tests for working with the Record container
*/
void test_Record ()
{
  cout << "\n[test_Record]\n" << std::endl;

  // Set up description of the record fields
  casa::RecordDesc ITSMetadataDesc;

  ITSMetadataDesc.addField ("description", TpString);
  ITSMetadataDesc.addField ("current_status", TpString);
  ITSMetadataDesc.addField ("observation_owner", TpString);
  ITSMetadataDesc.addField ("interval", TpInt);
  ITSMetadataDesc.addField ("current_iteration", TpInt);

  // create the record from the record description
  casa::Record ITSMetadataRec (ITSMetadataDesc);

}

// -------------------------------------------------------------- test_DataReader

/*!
  \brief Test constructors for a new DataReader object

  \return nofFailedTests -- The number of failed tests.

  We should provide a test for all the possible constructors here.
*/
int test_DataReader ()
{
  cout << "\n[test_DataReader]\n" << std::endl;

  int nofFailedTests (0);
  
  uint blocksize (128);
  uint fftLength (blocksize/2+1);
  uint nofFiles (5);
  Vector<Double> adc2voltage (nofFiles,0.25);
  Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);

  cout << "[1] Testing default constructor ..." << std::endl;
  try {
    DataReader dr;
    //
    dr.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << std::endl;
  {
    cout << " --> blocksize = " << blocksize << std::endl;
    //
    DataReader dr (blocksize);
    //
    dr.summary();
  }
  
  cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    //
    dr.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
//   cout << "[4] Testing copy constructor ..." << std::endl;
//   try {
//     DataReader dr1 (blocksize,
// 		    adc2voltage,
// 		    fft2calfft);
//     //
//     DataReader dr2 (dr1);
//     //
//     dr2.summary();
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }
  
  return nofFailedTests;
}

// ------------------------------------------------------------ test_headerRecord

/*!
  \brief Test working with the header record storing metadata

  \return nofFailedTests -- The number of failed tests.
*/
int test_headerRecord ()
{
  cout << "\n[test_headerRecord]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Retrieve the header record from the LOFAR_TBB object ..." << endl;
  try {
    DataReader dr;
    casa::Record rec = dr.headerRecord();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Set header record from external values ..." << endl;
  try {
    bool status (true);
    // new DataReader object to test
    DataReader dr;
    // set up record
    casa::Record rec;
    rec.define("Date",0);
    rec.define("AntennaIDs",casa::Vector<int>(1,1));
    rec.define("Observatory","LOFAR");
    rec.define("Filesize",1);
    // try writing the record back into the DataReader object
    status = dr.setHeaderRecord (rec);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------- test_dataStreams

/*!
  \brief Test working with file streams as they are used for reading of data

  \return nofFailedTests -- The number of failed tests.
*/
int test_dataStreams ()
{
  cout << "\n[test_dataStreams]\n" << std::endl;

  int nofFailedTests (0);
  uint blocksize (128);
  uint nofFiles (5);
  DataIterator *iterators;

  try {
    iterators = new DataIterator[nofFiles];
    
    for (uint file (0); file<nofFiles; file++) {
      iterators[file].setBlocksize(blocksize);
      cout << file << "\t" << iterators[file].blocksize() << endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
    
  return nofFailedTests;
}

// --------------------------------------------------------------- test_selection

/*!
  \brief Test (De-)Selection of antennas in a dataset

  \return nofFailedTests -- The number of failed tests.
*/
int test_selection ()
{  
  cout << "\n[test_selection]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);

  uint blocksize (20);
  uint fftLength (blocksize/2+1);
  uint nofFiles (5);
  Vector<Double> adc2voltage (nofFiles,0.25);
  Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);

  cout << "[1] No selection of antennas or frequencies ..." << std::endl;

  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    dr.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Testing antenna selection ..." << std::endl;
  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    Vector<Bool> selection (nofFiles,True);
    selection(0) = selection(1) = False;
    //
    cout << " - Antenna selection = " << selection << endl;
    //
    status = dr.setSelectedAntennas (selection);
    //
    if (status) {
      dr.summary();
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing frequency channel selection ..." << std::endl;
  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    Vector<Bool> selection (fftLength,True);
    selection(0) = selection(1) = False;
    //
    cout << " - Channel selection = " << selection << endl;
    //
    status = dr.setSelectedChannels (selection);
    //
    if (status) {
      dr.summary();
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -------------------------------------------------------- test_conversionArrays

/*!
  \brief Test assignment of the conversion array

  Test assignment and application of the arrays <tt>adc2voltage</tt> and 
  <tt>fft2calfft</tt> used within the processing chain to perform the required
  conversion of values.

  \param blocksize -- The number of samples per block of data per antenna.

  \return nofFailedTests -- The number of failed tests encountered.
*/
int test_conversionArrays (uint const &blocksize)
{  
  cout << "\n[test_conversionArrays]\n" << std::endl;

  int nofFailedTests (0);

  uint fftLength (blocksize/2+1);
  uint nofAntennas (5);
  
  cout << "[1] Use correct array shapes" << endl;
  try {
    Vector<Double> adc2voltage (nofAntennas,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofAntennas,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    dr.summary();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Using incorrect shape for adc2voltage (must throw warning)" 
       << endl;
  try {
    Vector<Double> adc2voltage (nofAntennas/2,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofAntennas,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    dr.summary();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Try to assign wrong array after construction (must throw warning)"
       << endl;
  try {
    Vector<Double> adc2voltage (nofAntennas,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofAntennas,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);

    dr.summary();

    adc2voltage.resize(nofAntennas/2);
    adc2voltage = 0.25;

    dr.setADC2Voltage (adc2voltage);
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Go through sequence of the parameter adjustments ..." << endl;
  try {
    Matrix<double> adc2voltage (blocksize,nofAntennas,1.0);
    Matrix<DComplex> fft2calfft (fftLength,nofAntennas,1.0);
    
    cout << " -- creating new DataReader object ..." << endl;
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    cout << " --> blocksize ............ = " << dr.blocksize() << endl;
    cout << " --> FFT length ........... = " << dr.fftLength() << endl;
    cout << " --> nof. selected channels = " << dr.nofSelectedChannels() << endl;
    cout << " --> nof. antennas ........ = " << dr.nofAntennas() << endl;
    
    {
      Vector<double> adc2voltageVector (dr.nofAntennas(),1.0);
      cout << " -- setting new values for adc2voltage (Vector<double>) ..." << endl;
      cout << " --> shape(adc2voltage) = " << adc2voltageVector.shape() << endl;
      dr.setADC2Voltage (adc2voltageVector);
    }
    
    {
      Matrix<double> adc2voltageMatrix (dr.blocksize(),dr.nofAntennas(),1.0);
      cout << " -- setting new values for adc2voltage (Matrix<double>) ..." << endl;
      cout << " --> shape(adc2voltage) = " << adc2voltageMatrix.shape() << endl;
      dr.setADC2Voltage (adc2voltageMatrix);
    }

    {
      Matrix<DComplex> fft2calfftMatrix (dr.fftLength(),dr.nofAntennas(),1.0);
      cout << " -- setting new values for fft2calfft (Matrix<DComplex>) ..." << endl;
      cout << " --> shape(fft2calfft) = " << fft2calfftMatrix.shape() << endl;
      dr.setFFT2calFFT (fft2calfftMatrix);
    }
    
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_frequency

/*!
  \brief Test access to the data building up the frequency axis

  \param blocksize -- The number of samples per block of data per antenna.

  \return nofFailedTests -- The number of failed tests encountered.
*/
int test_frequency (uint const &blocksize)
{  
  cout << "\n[test_frequency]" << std::endl;

  int nofFailedTests (0);

  cout << "\n[1] Default frequency settings" << endl;
  try {
    DataReader dr (blocksize);
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "\n[2] Explicit setting of the frequency parameters" << endl;
  try {
    uint nyquistZone   (1);
    Double samplingRate (40e06);
    //
    DataReader dr (blocksize, nyquistZone, samplingRate);
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "\n[3] Adjust frequency domain settings after construction" << endl;
  try {
    DataReader dr (blocksize);
    //
    dr.setSampleFrequency (30e06);
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
    cout << " - Frequencies   [ Hz] = " << dr.frequencyValues()   << endl;
    //
    dr.setSampleFrequency (Quantity(20,"MHz"));
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
    cout << " - Frequencies   [ Hz] = " << dr.frequencyValues()   << endl;
    //
    dr.setNyquistZone(2);
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
    cout << " - Frequencies   [ Hz] = " << dr.frequencyValues()   << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -------------------------------------------------------------------- test_time

/*!
  \brief Test retrival of the values along the time axis
  
  \param blocksize -- The number of samples per block of data per antenna.

  \return nofFailedTests -- The number of failed tests.
 */
int test_time (uint const &blocksize)
{
  cout << "\n[test_time]\n" << endl;

  int nofFailedTests (0);
  DataReader dr (blocksize);

  cout << "-- Blocksize    = " << dr.blocksize() << endl;
  cout << "-- Block number = " << dr.block()     << endl;
  cout << endl;

#ifdef HAVE_CASA
  cout << "[1] Default method for retrival of time values ..." << std::endl;
  try {
    casa::Vector<double> timeValues = dr.timeValues();
    cout << " [" << timeValues(0) << " " << timeValues(1) << " ...]" << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
#endif

  return nofFailedTests;
}

// -------------------------------------------------------------- test_processing

/*!
  \brief Test manipulations of DataReader object as performed in applications

  \return nofFailedTests -- The number of failed tests encountered in this
                            function.
*/
int test_processing ()
{
  cout << "\n[test_processing]" << std::endl;

  int nofFailedTests (0);
  uint blocksize (8192);

  cout << "[1] Testing call sequence as in dstb ..." << endl;
  try {
    cout << " --> creating DataReader object ..." << endl;
    DataReader dr (blocksize);
    dr.summary();
    cout << " --> adjusting blocksize ..." << endl;
    blocksize = 1024;
    dr.setBlocksize(blocksize);
    dr.summary();
    cout << " --> adjusting adc2voltage conversion factors ..." << endl;
    Matrix<double> adc2voltage (dr.blocksize(),dr.nofSelectedAntennas(),1.0);
    dr.setADC2Voltage(adc2voltage);
    dr.summary();
    cout << " --> adjusting ffft2calfft conversion factors ..." << endl;
    Matrix<DComplex> fft2calfft (dr.fftLength(),dr.nofSelectedAntennas(),1.0);
    dr.setFFT2calFFT(fft2calfft);
    dr.summary();
//     cout << " --> adjusting start block ..." << endl;
//     uint startblock (1);
//     dr.setStartBlock(startblock);
//     cout << " --> adjusting stride ..." << endl;
//     dr.setStride(0);
//     cout << " --> adjusting offset ..." << endl;
//     int offset (100);
//     dr.setShift(offset);
    cout << " --> adjusting frequency channel selection ..." << endl;
    Vector<bool> frequencySelection (dr.fftLength(),true);
    dr.setSelectedChannels(frequencySelection);
    dr.summary();
//     cout << " --> adjusting Hanning filter ..." << endl;
//     double alpha (0.54);
//     dr.setHanningFilter(alpha);
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ----------------------------------------------------------------- main routine

int main (int argc,
          char *argv[])
{
  int nofFailedTests (0);
  uint blocksize (100);
  
  // Test for the various constructors
  nofFailedTests += test_DataReader ();


  if (nofFailedTests == 0) {
    nofFailedTests += test_conversionArrays (blocksize);
//     nofFailedTests += test_dataStreams ();
    nofFailedTests += test_headerRecord ();
    nofFailedTests += test_selection ();
    nofFailedTests += test_time(blocksize);
    nofFailedTests += test_frequency (blocksize);
    nofFailedTests += test_processing();
  } else {
    std::cerr << "[tDataReader]"
      "Little sense trying to test object methods without valid object..."
	      << std::endl;
  }
  
  {
    test_Record ();
  }

  return nofFailedTests;
}
