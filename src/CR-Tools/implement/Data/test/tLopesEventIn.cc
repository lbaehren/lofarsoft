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
#include <Data/LopesEventIn.h>

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
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
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
  std::cout << "\n[tLopesEventIn::test_arrays]\n" << std::endl;

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
  std::cout << "\n[test_construction]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    CR::LopesEventIn event;
    event.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  std::cout << "[2] Testing construction with filename ..." << std::endl;
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
  std::cout << "\n[tLopesEventIn::test_data]\n" << std::endl;

  int nofFailedTests (0);
  CR::LopesEventIn newObject (filename);
  
  std::cout << "[1] Testing retrieval of fx() data ..." << std::endl;
  try {
    Matrix<Double> data;
    //
    std::cout << "--> LopesEventIn::fx()" << std::endl;
    data = newObject.fx();
    show_data (data);
    //
    std::cout << "--> LopesEventIn::fx(Matrix<double>&)" << std::endl;
    data = 0.0;
    newObject.fx(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  std::cout << "[2] Testing retrieval of voltage() data ..." << std::endl;
  try {
    Matrix<Double> data;
    //
    std::cout << "--> LopesEventIn::voltage()" << std::endl;
    data = newObject.voltage();
    show_data (data);
    //
    std::cout << "--> LopesEventIn::voltage(Matrix<double>&)" << std::endl;
    data = 0.0;
    newObject.voltage(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  std::cout << "[3] Testing retrieval of fft() data ..." << std::endl;
  try {
    Matrix<DComplex> data;
    //
    std::cout << "--> LopesEventIn::fft()" << std::endl;
    data = newObject.fft();
    show_data (data);
    //
    std::cout << "--> LopesEventIn::fft(Matrix<DComplex>&)" << std::endl;
    data = 0.0;
    newObject.fft(data);
    show_data (data);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };

  std::cout << "[4] Testing retrieval of calfft() data ..." << std::endl;
  try {
    Matrix<DComplex> data;
    //
    std::cout << "--> LopesEventIn::calfft()" << std::endl;
    data = newObject.calfft();
    show_data (data);
    //
    std::cout << "--> LopesEventIn::calfft(Matrix<DComplex>&)" << std::endl;
    data = 0.0;
    newObject.calfft(data);
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
  std::cout << "\n[tLopesEventIn::test_parameters]\n" << std::endl;

  int nofFailedTests (0);
  CR::LopesEventIn event (filename);
  unsigned int blocksize = event.blocksize();
  
  std::cout << "[1] Adjust blocksize ..." << std::endl;
  try {
    std::cout << "-- blocksize  = " << event.blocksize()   << std::endl;
    std::cout << "-- shape(fx)  = " << event.fx().shape()  << std::endl;
    std::cout << "-- shape(fft) = " << event.fft().shape() << std::endl;
    //
    blocksize /= 2;
    event.setBlocksize (blocksize);
    std::cout << "-- blocksize  = " << event.blocksize()   << std::endl;
    std::cout << "-- shape(fx)  = " << event.fx().shape()  << std::endl;
    std::cout << "-- shape(fft) = " << event.fft().shape() << std::endl;
    //
    blocksize /= 2;
    event.setBlocksize (blocksize);
    std::cout << "-- blocksize  = " << event.blocksize()   << std::endl;
    std::cout << "-- shape(fx)  = " << event.fx().shape()  << std::endl;
    std::cout << "-- shape(fft) = " << event.fft().shape() << std::endl;
    //
    blocksize /= 2;
    event.setBlocksize (blocksize);
    std::cout << "-- blocksize  = " << event.blocksize()   << std::endl;
    std::cout << "-- shape(fx)  = " << event.fx().shape()  << std::endl;
    std::cout << "-- shape(fft) = " << event.fft().shape() << std::endl;
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
  std::cout << "\n[tLopesEventIn::test_header]\n" << std::endl;

  int nofFailedTests (0);
  CR::LopesEventIn event (filename);
  
  std::cout << "[1] Testing header record ..." << std::endl;
  try {
    for (uint i=0; i<event.headerRecord().nfields(); i++) {
      std::cout << "  Field No: " << i
		<< " Name: " << event.headerRecord().name(i)
		<< std::endl;
    };
    std::cout << "  Values:\n    Date: "
	      << event.headerRecord().asuInt("Date") << std::endl;
    std::cout << "    Observatory: "
	      << event.headerRecord().asString("Observatory") << std::endl;
    std::cout << "    IDs: "
	      << event.headerRecord().asArrayInt("AntennaIDs") << std::endl;
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
  std::string filename ("example.event");

  /* Check if a filename has been provided on the command line */
  if (argc > 1) {
    std::string tmp = argv[1];
    filename        = tmp;
  }

  // Timing tests for different array access schemes
  nofFailedTests += test_arrays ();

  // Test for the constructor(s)
  nofFailedTests += test_construction (filename);
  // Test access to the data
  nofFailedTests += test_data (filename);
  // Test settings for the control parameters
  nofFailedTests += test_parameters (filename);
  // Test working with the header record
  nofFailedTests += test_header (filename);

  return nofFailedTests;
}
