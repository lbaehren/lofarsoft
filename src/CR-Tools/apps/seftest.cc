/*-------------------------------------------------------------------------*
 | $Id:: ttbbctlIn.cc 1908 2008-08-22 12:23:04Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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
#include <Data/tbbctlIn.h>
#include <IO/LOFAR_TBB.h>

using namespace std;
using CR::tbbctlIn;  // Namespace usage
using CR::LOFAR_TBB;

/*
  \file ttbbctlIn.cc

  \ingroup CR_Applications

  \brief A collection of test routines for the tbbctlIn class
 
  \author Andreas Horneffer
 
  \date 2007/11/07
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,0) << " "
	    << data(2,0) << " "
	    << data(3,0)
	    << std::endl;
}

/*!
  \brief Test constructors for a new tbbctlIn object

  \return nofFailedTests -- The number of failed tests.
*/
int test_LOFAR_TBB ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_LOFAR_TBB]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
  
	string filename =
"/misc/usersb/student/swelles/usg/solburst-lopes30/2005.07.14.07:22:55.266.event";
	 //"/misc/lofar/CS1_tbb/big_2008_03_06/big2008_03_06_1093.h5";
	
	cout<<"Everything's oke until now... 1\n";
	
	LOFAR_TBB newI(filename,1024);	// Reads in the datafile in filename into
								// the structure(?) newI. newI then contains
								// the methods defined in LOFAR_TBB. An
								// example of a method is fx().

	cout<<"Everything's oke until now... 2\n";

    std::cout << "[2] Testing attaching a file ..." << std::endl;
	

    std::cout << "[3] Testing retrieval of fx() data ..." << std::endl;
    Matrix<Double> data;
    data = newI.fx();
    show_data (data);
	
    std::cout << "[4] Testing retrieval of Voltage data ..." << std::endl;
    data = newI.voltage();
    show_data(data);
	
    std::cout << "[5] Testing header record ..." << std::endl;
    for (uint i=0; i< newI.headerRecord().nfields(); i++) {
      std::cout << "  Field No: " << i << " Name: " << newI.headerRecord().name(i) << std::endl;
    };
    std::cout << "  Values:\n    Date: " << newI.headerRecord().asuInt("Date") << std::endl;
    std::cout << "    Observatory: " << newI.headerRecord().asString("Observatory") << std::endl;
    std::cout << "    IDs: " << newI.headerRecord().asArrayInt("AntennaIDs") << std::endl;
	std::cout << "    Filesize: " << newI.headerRecord().asArrayInt("Filesize") << std::endl;
//	std::cout << "    dDate: " << newtbbctlIn.headerRecord().asArrayInt("dDate") << std::endl;

	
    newI.summary(); 
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
  {
    nofFailedTests += test_LOFAR_TBB ();
  }
  
  

  return nofFailedTests;
}
