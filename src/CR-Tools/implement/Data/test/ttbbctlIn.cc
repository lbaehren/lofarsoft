/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
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

#include <Data/tbbctlIn.h>

using CR::tbbctlIn;  // Namespace usage

/*!
  \file ttbbctlIn.cc

  \ingroup CR_Data

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
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
}

/*!
  \brief Test constructors for a new tbbctlIn object

  \return nofFailedTests -- The number of failed tests.
*/
int test_tbbctlIn ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_tbbctlIn]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    tbbctlIn newtbbctlIn;
    //

    Vector<String> filenames(2);
    filenames(0) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0100.dat";
    filenames(1) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0101.dat";
    //    filenames(2) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0106.dat";
    //    filenames(3) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0107.dat";
    //    filenames(4) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0108.dat";

    std::cout << "[2] Testing attaching a file ..." << std::endl;
    if (! newtbbctlIn.attachFile(filenames) ){
      std::cout << "  Failed to attach file(s)!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    

    std::cout << "[3] Testing retrieval of fx() data ..." << std::endl;
    Matrix<Double> data;
    data = newtbbctlIn.fx();
    show_data (data);
    
    std::cout << "[4] Testing retrieval of Voltage data ..." << std::endl;
    data = newtbbctlIn.voltage();
    show_data(data);


    std::cout << "[5] Testing header record ..." << std::endl;
    for (uint i=0; i<newtbbctlIn.headerRecord().nfields(); i++) {
      std::cout << "  Field No: " << i << " Name: " << newtbbctlIn.headerRecord().name(i) << std::endl;
    };
    std::cout << "  Values:\n    Date: " << newtbbctlIn.headerRecord().asuInt("Date") << std::endl;
    std::cout << "    Observatory: " << newtbbctlIn.headerRecord().asString("Observatory") << std::endl;
    std::cout << "    IDs: " << newtbbctlIn.headerRecord().asArrayInt("AntennaIDs") << std::endl;

    newtbbctlIn.summary(); 
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
    nofFailedTests += test_tbbctlIn ();
  }

  return nofFailedTests;
}
