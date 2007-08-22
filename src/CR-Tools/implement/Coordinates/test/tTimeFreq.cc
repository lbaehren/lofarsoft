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

#include <Coordinates/TimeFreq.h>

/*!
  \file tTimeFreq.cc

  \ingroup Coordinates

  \brief A collection of test routines for TimeFreq
 
  \author Lars B&auml;hren
 
  \date 2007/03/06
*/

using CR::TimeFreq;

// -----------------------------------------------------------------------------

/*!
  \brief Show the frequency values
*/
void show_frequencies (vector<double> const &freq,
		       uint const &nyquistZone,
		       double const &incr)
{
  uint nofChannels (freq.size());

  // how many frequency channels do we have?
  std::cout << std::endl;
  std::cout << " Nyquist zone    = " << nyquistZone << std::endl;
  std::cout << " nof channels    = " << nofChannels << std::endl;
  std::cout << " freq. increment = " << incr             << std::endl;
  std::cout << " freq[1]-freq[0] = " << freq[1]-freq[0]  << std::endl;

  // print a subset of the frequency values
  std::cout << std::endl;
  std::cout << " [ "
	    << freq[0] << " "
	    << freq[1] << " "
	    << freq[2] << " "
	    << freq[3] << " "
	    << freq[4] << " "
	    << freq[5] << "\n   ... \n   "
	    << freq[nofChannels-3] << " "
	    << freq[nofChannels-2] << " "
	    << freq[nofChannels-1] << " ]"
	    << std:: endl;
  std::cout << std::endl;
  
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TimeFreq object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TimeFreq ()
{
  std::cout << "\n[test_TimeFreq]\n" << std::endl;

  int nofFailedTests (0);
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    TimeFreq data;
    
    data.summary();
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructors ..." << std::endl;
  try {
    TimeFreq data1 (blocksize);
    data1.summary();
    //
    TimeFreq data2 (blocksize,sampleFrequency,nyquistZone);
    data2.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    TimeFreq data1 (blocksize,sampleFrequency,nyquistZone);
    TimeFreq data2 (data1);
    
    std::cout << "-- Properties of the original object" << std::endl;
    data1.summary();
    std::cout << "-- Properties of the copied object" << std::endl;
    data2.summary();
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test access to and manipulation of the sample frequency

  This function will we testing the various methods, by which the sample
  frequency can be received or set:
  <ul>
    <li>Retrieve \f$ \nu_{\rm Sample} \f$ in natural frequency units, i.e. in [Hz]
    \verbatim
    sample frequency [Hz] = 8e+07
    \endverbatim
    <li>Retrieve \f$ \nu_{\rm Sample} \f$ in user-defined frequency units; this
    is a simple wrapper to e.g. retrieve \f$ \nu_{\rm Sample} \f$ in units of
    [MHz], which internally utilizes casa::Quanta for conversion.
    \verbatim
    sample frequency [GHz] = 0.08
    sample frequency [MHz] = 80
    sample frequency [kHz] = 80000
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ in natural frequency units, i.e. in [Hz]
    \verbatim
    sample frequency [Hz] = 1e+08
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ in user-defined frequency units
    \verbatim
    sample frequency [Hz] = 80
    sample frequency [Hz] = 80000
    sample frequency [Hz] = 8e+07
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ using a casa::Quantity
    \verbatim
    sample frequency [Hz] = 80
    sample frequency [Hz] = 80000
    sample frequency [Hz] = 8e+07
    \endverbatim
  </ul>
    
  \return nofFailedTests -- The number of failed tests.
*/
int test_sampleFrequency ()
{
  std::cout << "\n[test_sampleFrequency]\n" << std::endl;

  int nofFailedTests (0);
  double sampleFrequency (0.0);
  TimeFreq data;

  std::cout << "[1] Get the natural value of sample frequency" << std::endl;
  try {
    sampleFrequency = data.sampleFrequency();

    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Retrieve sample frequency in user defined unit" << std::endl;
  try {
    sampleFrequency = data.sampleFrequency("GHz");
    std::cout << " sample frequency [GHz] = " << sampleFrequency << std::endl;
    //
    sampleFrequency = data.sampleFrequency("MHz");
    std::cout << " sample frequency [MHz] = " << sampleFrequency << std::endl;
    //
    sampleFrequency = data.sampleFrequency("kHz");
    std::cout << " sample frequency [kHz] = " << sampleFrequency << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Set sample frequency in natural units" << std::endl;
  try {
    sampleFrequency = 1e08;

    data.setSampleFrequency (sampleFrequency);

    sampleFrequency = data.sampleFrequency();

    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Set sample frequency user defined units" << std::endl;
  try {
    data.setSampleFrequency (80,"Hz");
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
    //
    data.setSampleFrequency (80,"kHz");
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
    //
    data.setSampleFrequency (80,"MHz");
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[5] Set sample frequency from Quantity" << std::endl;
  try {
    casa::Quantity rate1 (80,"Hz");
    data.setSampleFrequency (rate1);
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
    //
    casa::Quantity rate2 (80,"kHz");
    data.setSampleFrequency (rate2);
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
    //
    casa::Quantity rate3 (80,"MHz");
    data.setSampleFrequency (rate3);
    sampleFrequency = data.sampleFrequency();
    std::cout << " sample frequency [Hz] = " << sampleFrequency << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrival of the values along the frequency axis

  \verbatim
  freq[0]   = 4e+07
  freq[1]   = 4.00391e+07
  freq[2]   = 4.00781e+07
  freq[N-2] = 7.99609e+07
  freq[N-1] = 8e+07
  
  freq. increment = 39062.5
  freq[1]-freq[0] = 39062.5
  \endverbatim
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_frequencyValues ()
{
  std::cout << "\n[test_frequencyValues]\n" << std::endl;

  int nofFailedTests (0);
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  TimeFreq data (blocksize,sampleFrequency,nyquistZone);
  vector<double> freq;

  std::cout << "[1] Frequency values for different Nyquist zones..." << std::endl;
  try {
    freq = data.frequencyValues();
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.frequencyIncrement());
    
    data.setNyquistZone (3);
    
    freq = data.frequencyValues();
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.frequencyIncrement());
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Frequency channels selection by mask..." << std::endl;
  try {
    vector<bool> selection (data.fftLength(),true);

    selection[0] = selection[1] = selection[selection.size()-1] = false;

    freq = data.frequencyValues(selection);

    show_frequencies (freq,
		      data.nyquistZone(),
		      data.frequencyIncrement());
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Frequency channels selection by range..." << std::endl;
  try {
    vector<double> range (2);

    range[0] = 85e+06;
    range[1] = 95e+06;

    // provide range as vector

    freq = data.frequencyValues(range);

    show_frequencies (freq,
		      data.nyquistZone(),
		      data.frequencyIncrement());

    // provide range as limits

    freq = data.frequencyValues(range[1],range[0]);
    
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.frequencyIncrement());

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test creation go coordinate axes from the time-frequency parameters

  \return nofFailedTests -- The number of failed tests.
*/
int test_coordinateAxes ()
{
  std::cout << "\n[test_coordinateAxes]\n" << std::endl;

  int nofFailedTests (0);

  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  TimeFreq data (blocksize,sampleFrequency,nyquistZone);

  std::cout << "[1] Coordinates axes using the default methods" << std::endl;
  try {
    LinearCoordinate timeAxis   = data.timeAxis();

    std::cout << "Time axis" << std::endl;
    std::cout << "-- nof. pixel axes  = " << timeAxis.nPixelAxes() << std::endl;
    std::cout << "-- nof. world axes  = " << timeAxis.nWorldAxes() << std::endl;
    std::cout << "-- World axis names = " << timeAxis.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << timeAxis.worldAxisUnits() << std::endl;
    std::cout << "-- Reference value  = " << timeAxis.referenceValue() << std::endl;
    std::cout << "-- Reference pixel  = " << timeAxis.referencePixel() << std::endl;
    std::cout << "-- Increment        = " << timeAxis.increment() << std::endl;

    SpectralCoordinate freqAxis = data.frequencyAxis();

    std::cout << "Frequency axis" << std::endl;
    std::cout << "-- nof. pixel axes  = " << freqAxis.nPixelAxes() << std::endl;
    std::cout << "-- nof. world axes  = " << freqAxis.nWorldAxes() << std::endl;
    std::cout << "-- World axis names = " << freqAxis.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << freqAxis.worldAxisUnits() << std::endl;
    std::cout << "-- Reference value  = " << freqAxis.referenceValue() << std::endl;
    std::cout << "-- Reference pixel  = " << freqAxis.referencePixel() << std::endl;
    std::cout << "-- Increment        = " << freqAxis.increment() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Coordinates axes using the default methods" << std::endl;
  try {
    double refValue (100);
    double increment (0.1);
    double refPixel (1);

    LinearCoordinate timeAxis   = data.timeAxis(refValue,
						increment,
						refPixel);

    std::cout << "Time axis" << std::endl;
    std::cout << "-- nof. pixel axes  = " << timeAxis.nPixelAxes() << std::endl;
    std::cout << "-- nof. world axes  = " << timeAxis.nWorldAxes() << std::endl;
    std::cout << "-- World axis names = " << timeAxis.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << timeAxis.worldAxisUnits() << std::endl;
    std::cout << "-- Reference value  = " << timeAxis.referenceValue() << std::endl;
    std::cout << "-- Reference pixel  = " << timeAxis.referencePixel() << std::endl;
    std::cout << "-- Increment        = " << timeAxis.increment() << std::endl;

    SpectralCoordinate freqAxis = data.frequencyAxis(refValue,
						     increment,
						     refPixel);
    
    std::cout << "Frequency axis" << std::endl;
    std::cout << "-- nof. pixel axes  = " << freqAxis.nPixelAxes() << std::endl;
    std::cout << "-- nof. world axes  = " << freqAxis.nWorldAxes() << std::endl;
    std::cout << "-- World axis names = " << freqAxis.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << freqAxis.worldAxisUnits() << std::endl;
    std::cout << "-- Reference value  = " << freqAxis.referenceValue() << std::endl;
    std::cout << "-- Reference pixel  = " << freqAxis.referencePixel() << std::endl;
    std::cout << "-- Increment        = " << freqAxis.increment() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  
  // Test for the constructor(s)
  nofFailedTests += test_TimeFreq ();
  // Test accesss to the sample frequency in the ADC
  nofFailedTests += test_sampleFrequency ();
  // Test retrival of the frequency values
  nofFailedTests += test_frequencyValues ();
  // Test conversion of parameters to coordinate axes
  nofFailedTests += test_coordinateAxes ();
  
  return nofFailedTests;
}
