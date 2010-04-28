/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <sys/time.h>
/* casacore header files */
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
/* CR-Tools header files */
#include <IO/LopesEventIn.h>

/* Namespace usage */
using std::cout;
using std::endl;

/*!
  \file tLopesEventIn.cc

  \ingroup CR_Data

  \brief A collection of test routines for LopesEventIn
 
  \author Andreas Horneffer, Lars B&auml;hren
 
  \date 2006/12/14
*/

//_______________________________________________________________________________
//                                                                      show_data

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
template <class T>
void show_data (Matrix<T> const &data)
{
  cout << "  Data shape : " << data.shape() << endl;
  cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << endl;
}

//_______________________________________________________________________________
//                                                                    test_arrays

/*!
  \brief Timing tests for different array access schemes

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_arrays()
{
  cout << "\n[tLopesEventIn::test_arrays]\n" << endl;

  int nofFailedTests (0);
  
  int nofColums (2048);
  int nofRows (20048);

  try {
    Matrix<Short> input(nofRows,nofColums);
    Matrix<Double> output(nofRows,nofColums);
    {
      int i,j;
      for (i=0;i<nofColums;i++){
	for (j=0;j<nofRows;j++){
	  input(j,i) = i+j;
	};
      };
    };
    struct timeval start1,start2,stop1,stop2;
    struct timezone tz;
    {
      int i,j;
      gettimeofday(&start1,&tz);
      for (i=0;i<2048;i++){
	for (j=0;j<20048;j++){
	  output(j,i) = input(j,(2047-i));
	};
      };
      gettimeofday(&stop1,&tz);
    }
    {
      Matrix<Short> zwisch(20048,2048);
      int i;
      gettimeofday(&start2,&tz);
      for (i=0;i<2048;i++){
	zwisch.column(i) = input.column(2047-i);
      };
      convertArray(output,zwisch);
      gettimeofday(&stop2,&tz);
    }
    Double time1,time2;
    time1 = stop1.tv_sec-start1.tv_sec+ (stop1.tv_usec-start1.tv_usec)*1e-6;
    time2 = stop2.tv_sec-start2.tv_sec+ (stop2.tv_usec-start2.tv_usec)*1e-6;
    // O.K. the second version is slightly faster!

    cout << "time1: "<<time1<< "  time2: " << time2 <<endl;


  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                              test_construction

/*!
  \brief Test constructors for a new LopesEventIn object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_construction (std::string const &filename)
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    CR::LopesEventIn event;
    event.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  cout << "[2] Testing construction with filename ..." << endl;
  try {
    CR::LopesEventIn event (filename);
    event.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                      test_data

/*!
  \brief Test access to the data
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_data (std::string const &filename)
{
  cout << "\n[tLopesEventIn::test_data]\n" << endl;

  int nofFailedTests (0);
  CR::LopesEventIn le (filename);
  
  cout << "[1] Testing retrieval of fx() data ..." << endl;
  try {
    Matrix<Double> data;
    //
    cout << "--> LopesEventIn::fx()" << endl;
    data = le.fx();
    show_data (data);
    //
    cout << "--> LopesEventIn::fx(Matrix<double>&)" << endl;
    data = 0.0;
    le.fx(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  cout << "[2] Testing retrieval of voltage() data ..." << endl;
  try {
    Matrix<Double> data;
    //
    cout << "--> LopesEventIn::voltage()" << endl;
    data = le.voltage();
    show_data (data);
    //
    cout << "--> LopesEventIn::voltage(Matrix<double>&)" << endl;
    data = 0.0;
    le.voltage(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  cout << "[3] Testing retrieval of fft() data ..." << endl;
  try {
    Matrix<DComplex> data;
    //
    cout << "--> LopesEventIn::fft()" << endl;
    data = le.fft();
    show_data (data);
    //
    cout << "--> LopesEventIn::fft(Matrix<DComplex>&)" << endl;
    data = 0.0;
    le.fft(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };

  cout << "[4] Testing retrieval of calfft() data ..." << endl;
  try {
    Matrix<DComplex> data;
    //
    cout << "--> LopesEventIn::calfft()" << endl;
    data = le.calfft();
    show_data (data);
    //
    cout << "--> LopesEventIn::calfft(Matrix<DComplex>&)" << endl;
    data = 0.0;
    le.calfft(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };

  return nofFailedTests;
}


//_______________________________________________________________________________
//                                                                test_parameters

/*!
  \brief Test settings for the control parameters
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_parameters (std::string const &filename)
{
  cout << "\n[tLopesEventIn::test_parameters]\n" << endl;

  int nofFailedTests (0);
  CR::LopesEventIn event (filename);
  unsigned int blocksize = event.blocksize();
  int steps (6);
  
  cout << "[1] Adjust blocksize ..." << endl;
  try {
    for (int n(0); n<steps; ++n) {
      // Display parameter values
      cout << "-- blocksize   = " << event.blocksize()   << endl;
      cout << "-- shape(fx)   = " << event.fx().shape()  << endl;
      cout << "-- shape(fft)  = " << event.fft().shape() << endl;
      cout << "-- shape(freq) = " << event.frequencyValues().shape() << endl;
      // Adjust the blocksize
      blocksize /= 2;
      event.setBlocksize (blocksize);
    }
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                    test_header

/*!
  \brief Test creation of header record
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_header (std::string const &filename)
{
  cout << "\n[tLopesEventIn::test_header]\n" << endl;
  
  int nofFailedTests (0);
  CR::LopesEventIn event (filename);
  
  cout << "[1] Testing header record ..." << endl;
  try {
    for (uint i=0; i<event.headerRecord().nfields(); i++) {
      cout << "  Field No: " << i
	   << " Name: " << event.headerRecord().name(i)
	   << endl;
    };
    cout << "  Values:\n    Date: "
	 << event.headerRecord().asuInt("Date") << endl;
    cout << "    Observatory: "
	 << event.headerRecord().asString("Observatory") << endl;
    cout << "    IDs: "
	 << event.headerRecord().asArrayInt("AntennaIDs") << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc, char *argv[])
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

  // Timing tests for different array access schemes
  nofFailedTests += test_arrays ();

  if (haveDataset) {
    // Test for the constructor(s)
    nofFailedTests += test_construction (filename);
    // Test access to the data
    nofFailedTests += test_data (filename);
    // Test settings for the control parameters
    nofFailedTests += test_parameters (filename);
    // Test working with the header record
    nofFailedTests += test_header (filename);
  }
  else {
    std::cerr << "[tLopesEventIn] Skipping tests which require input dataset!"
	      << std::endl;
  }
  
  return nofFailedTests;
}
