/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: tDataReader.cc,v 1.15 2007/03/07 17:49:30 bahren Exp $*/

// CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>

// LOPES header files
#include <templates.h>
#include <IO/DataReader.h>

using casa::DComplex;

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
  std::cout << "\n[test_Record]\n" << std::endl;

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

// -------------------------------------------------------------------- show_data

/*!
  \brief Show the data array returned by the DataReader

  \verbatim
  - fx      = Axis Lengths: [1, 1]  (NB: Matrix in Row/Column order)
  [0]
  
  - voltage = Axis Lengths: [1, 1]  (NB: Matrix in Row/Column order)
  [0]
  
  - fft     = Axis Lengths: [1, 1]  (NB: Matrix in Row/Column order)
  [(0,0)]
  
  - calfft  = Axis Lengths: [1, 1]  (NB: Matrix in Row/Column order)
  [(0,0)]
  \endverbatim
*/
void show_data (DataReader &dr)
{
  cout << " - fx        = " << dr.fx() << endl;
  cout << " - voltage   = " << dr.voltage() << endl;
  cout << " - fft       = " << dr.fft() << endl;
  cout << " - calfft    = " << dr.calfft() << endl;
  cout << " - ccSpectra = " << dr.ccSpectra() << endl;
}

// -------------------------------------------------------------- show_parameters

/*!
  \brief Show the internal parameters as accessable through public functions
  
  \verbatim
  - blocksize   = 1
  - FFT length  = 1
  - nof. files  = 1
  - adc2voltage = [1]
  - fft2calfft  = Axis Lengths: [1, 1]  (NB: Matrix in Row/Column order)
  [(1,0)]
  \endverbatim
*/
void show_parameters (DataReader &dr)
{
  cout << " - nof. streams           = " << dr.nofStreams()       << endl;
  cout << " - blocksize              = " << dr.blocksize() << endl;
  //
  cout << " - FFT length             = " << dr.fftLength() << endl;
  cout << " - selected channels      = " << dr.selectedChannels() << endl;
  cout << " - nof. selected channels = " << dr.nofSelectedChannels() << endl;
  //
  cout << " - antennas               = " << dr.antennas()            << endl;
  cout << " - nof. antennas          = " << dr.nofAntennas()         << endl;
  cout << " - selected antennas      = " << dr.selectedAntennas()    << endl;
  cout << " - nof. selected antennas = " << dr.nofSelectedAntennas() << endl;
  //
  cout << " - shape(adc2voltage)     = " << dr.adc2voltage().shape() << endl;
  cout << " - shape(fft2calfft)      = " << dr.fft2calfft().shape()  << endl;
}

// -------------------------------------------------------------- test_DataReader

/*!
  \brief Test constructors for a new DataReader object

  \return nofFailedTests -- The number of failed tests.

  We should provide a test for all the possible constructors here.
*/
int test_DataReader ()
{
  std::cout << "\n[test_DataReader]\n" << std::endl;

  int nofFailedTests (0);
  
  uint blocksize (20);
  uint fftLength (blocksize/2+1);
  uint nofFiles (5);
  Vector<Double> adc2voltage (nofFiles,0.25);
  Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    DataReader dr;
    //
    show_parameters (dr);    
    show_data (dr);    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  {
    DataReader dr (blocksize);
    //
    show_parameters (dr);
    show_data (dr);    
  }
  
  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    //
    show_parameters (dr);
    show_data (dr);    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing copy constructor ..." << std::endl;
  try {
    DataReader dr1 (blocksize,
		    adc2voltage,
		    fft2calfft);
    //
    DataReader dr2 (dr1);
    //
    show_parameters (dr2);
    show_data (dr2);
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
  std::cout << "\n[test_dataStreams]\n" << std::endl;

  uint blocksize (128);
  uint nofFiles (5);
  DataIterator *iterators;

  iterators = new DataIterator[nofFiles](blocksize);

  for (uint file (0); file<nofFiles; file++) {
    cout << file << "\t" << iterators[file].blocksize() << endl;
  }
  
  int nofFailedTests (0);
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_selection

/*!
  \brief Test (De-)Selection of antennas in a dataset

  \return nofFailedTests -- The number of failed tests.
*/
int test_selection ()
{  
  std::cout << "\n[test_selection]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);

  uint blocksize (20);
  uint fftLength (blocksize/2+1);
  uint nofFiles (5);
  Vector<Double> adc2voltage (nofFiles,0.25);
  Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);

  std::cout << "[1] No selection of antennas or frequencies ..." << std::endl;

  try {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    show_parameters (dr);
    //
    Matrix<Double> voltage (dr.voltage());
    Matrix<DComplex> calfft (dr.calfft());
    std::cout << " - shape(voltage) = " << voltage.shape() << std::endl;
    std::cout << " - shape(calfft)  = " << calfft.shape() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Testing antenna selection ..." << std::endl;
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
    show_parameters (dr);
    //
    Matrix<Double> voltage (dr.voltage());
    Matrix<DComplex> calfft (dr.calfft());
    std::cout << " - shape(voltage) = " << voltage.shape() << std::endl;
    std::cout << " - shape(calfft)  = " << calfft.shape() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing frequency channel selection ..." << std::endl;
  {
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);
    Vector<Bool> selection (fftLength,True);
    selection(0) = selection(1) = False;
    //
    cout << " - Antenna selection = " << selection << endl;
    //
    status = dr.setSelectedChannels (selection);
    show_parameters (dr);
    //
    Matrix<Double> voltage (dr.voltage());
    Matrix<DComplex> calfft (dr.calfft());
    std::cout << " - shape(voltage) = " << voltage.shape() << std::endl;
    std::cout << " - shape(calfft)  = " << calfft.shape() << std::endl;
  }
  
  return nofFailedTests;
}

// -------------------------------------------------------- test_conversionArrays

/*!
  \brief Test assignment of the conversion array

  Test assignment and application of the arrays <tt>adc2voltage</tt> and 
  <tt>fft2calfft</tt> used within the processing chain to perform the required
  conversion of values.

  \return nofFailedTests -- The number of failed tests.
*/
int test_conversionArrays ()
{  
  std::cout << "\n[test_conversionArrays]\n" << std::endl;

  int nofFailedTests (0);

  uint blocksize (20);
  uint fftLength (blocksize/2+1);
  uint nofFiles (5);
  
  cout << "[1] Use correct array shapes" << endl;
  try {
    Vector<Double> adc2voltage (nofFiles,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);

    show_parameters (dr);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Using incorrect shape for adc2voltage (must throw warning)" 
       << endl;
  try {
    Vector<Double> adc2voltage (nofFiles/2,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);

    show_parameters (dr);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Try to assign wrong array after construction (must throw warning)"
       << endl;
  try {
    Vector<Double> adc2voltage (nofFiles,0.25);
    Matrix<DComplex> fft2calfft (fftLength,nofFiles,2.0);
    
    DataReader dr (blocksize,
		   adc2voltage,
		   fft2calfft);

    show_parameters (dr);

    adc2voltage.resize(nofFiles/2);
    adc2voltage = 0.25;

    dr.setADC2voltage (adc2voltage);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_frequency

/*!
  \brief Test access to the data building up the frequency axis
*/
int test_frequency (uint const &blocksize)
{  
  std::cout << "\n[test_frequency]" << std::endl;

  int nofFailedTests (0);

  cout << "\n[1] Default frequency settings" << endl;
  try {
    DataReader dr (blocksize);
    //
    cout << " - Nyquist zone        = " << dr.nyquistZone()       << endl;
    cout << " - Sampling rate [ Hz] = " << dr.sampleFrequency()      << endl;
    cout << " -               [MHz] = " << dr.sampleFrequency("MHz") << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
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
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
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
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ----------------------------------------------------------------- main routine

int main ()
{
  int nofFailedTests (0);
  uint blocksize (100);
  
  {
    nofFailedTests += test_DataReader ();
  }

  if (nofFailedTests == 0) {
    nofFailedTests += test_conversionArrays ();
    nofFailedTests += test_dataStreams ();
    nofFailedTests += test_selection ();
    nofFailedTests += test_frequency (blocksize);
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
