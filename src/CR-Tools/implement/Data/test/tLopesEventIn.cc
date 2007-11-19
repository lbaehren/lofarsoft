/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: tLopesEventIn.cc,v 1.2 2007/03/14 12:58:44 bahren Exp $*/

#include <sys/time.h>

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Data/LopesEventIn.h>

/*!
  \file tLopesEventIn.cc

  \ingroup CR_Data

  \brief A collection of test routines for LopesEventIn
 
  \author Andreas Horneffer
 
  \date 2006/12/14
*/

// -----------------------------------------------------------------------------

using namespace CR;

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
}

/*!
  \brief Test constructors for a new LopesEventIn object

  \return nofFailedTests -- The number of failed tests.
*/

int test_LopesEventIn ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_LopesEventIn]\n" << std::endl;

  try {
    std::cout << "[1] Testing default constructor ..." << std::endl;
    LopesEventIn newObject;

    std::cout << "[2] Testing attaching a file ..." << std::endl;
    if (! newObject.attachFile("example.event")){
      std::cout << "  Failed to attach file!" << std::endl;
      std::cout << "  (Maybe no file called \"example.event\" in the local directory. Copy one here.)" 
		<< std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[3] Testing retrieval of fx() data ..." << std::endl;
    Matrix<Double> data;
    data = newObject.fx();
    show_data (data);

    std::cout << "[4] Testing retrieval of Voltage data ..." << std::endl;
    data = newObject.voltage();
    show_data (data);

    std::cout << "[5] Testing header record ..." << std::endl;
    for (uint i=0; i<newObject.header().nfields(); i++) {
      std::cout << "  Field No: " << i << " Name: " << newObject.header().name(i) << std::endl;
    };
    std::cout << "  Values:\n    Date: " << newObject.header().asuInt("Date") << std::endl;
    std::cout << "    Observatory: " << newObject.header().asString("Observatory") << std::endl;
    std::cout << "    IDs: " << newObject.header().asArrayInt("AntennaIDs") << std::endl;

    std::cout << "[6] Testing destructor ..." << std::endl;
    //Is suppostd to happen when this bracket closes...
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  };
  
  std::cout << "\n[Tests done?]" << std::endl;
  
  return nofFailedTests;
}


int dummy_test() {
  int nofFailedTests=0;
  try {
    Matrix<Short> input(20048,2048);
    Matrix<Double> output(20048,2048);
    {
      int i,j;
      for (i=0;i<2048;i++){
	for (j=0;j<2048;j++){
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

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_LopesEventIn();
  }

  return nofFailedTests;
}
