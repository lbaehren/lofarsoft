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
#include <Analysis/tbbTools.h>
#include <iostream>
#include <iomanip>
using CR::tbbctlIn;  // Namespace usage

Vector<String> filenames(1);
/*!
  \file ttbbctlIn.cc

  \ingroup CR_Data

  \brief A collection of test routines for the tbbctlIn class
 
  \author Andreas Horneffer
 
  \date 2007/11/07
 
  <h3>Synopsis</h3>
	
	Currently this routine also tests the TBBTools trigger algorithm. CRC checking from class CRCcheck is used in 
  the tbbctlIn datareader.
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
	Vector<double> dataVector = data.column(0);
	std::cout << " Vector data: " << dataVector(0) << " " << dataVector(1) << " " << dataVector(2) << " " << dataVector(3) << std::endl;
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
int test_tbbctlIn ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_tbbctlIn]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    tbbctlIn newtbbctlIn;
    //


	 // filenames(0) = "/Users/acorstanje/testing15apr/rw_20090415_102613_0124.dat";
    //filenames(0) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0100.dat";
    //filenames(1) = "/mnt/lofar/CS1_tbb/20071112_145210/rw_20071112_145210_0101.dat";
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
    
		// test tbbTools trigger algorithm
		std::cout << "[3a] Testing TBBTools trigger algorithm ..." << std::endl;
		tbbTools * myTbbTools = new tbbTools;
		Vector<double> dataVector = data.column(0);
		Vector<int> index, sum, width, peak, meanval, afterval; // output for trigger function
		
		// Trigger algorithm parameters as in tbbctl --setup=level, start, stop. Windowsize in samples; 0 for afterwindow.
	
		bool ok = myTbbTools->meanFPGAtrigger(dataVector, 6, 8, 5, 212144, 0, index, sum, width, peak, meanval, afterval, True);
		if (ok == False)
		{
			cerr << "Error doing TBBTools trigger algorithm!" << endl;
			nofFailedTests++;
		}
		
		std::cout << "pulse count: " << index.shape() << std::endl;
		std::cout << "pulse indices: " << index(0) << " " << index(1) << " " << index(2) << "..." << std::endl;
		
		std::cout << "no.         Index       sum         width       peak        mean" << std::endl;
		uint sp=8; // spacing
		for(uint i=0; i<index.shape(); i++)
		{ // show trigger results
			std::cout << setiosflags(ios::left);
			string empty = "    ";
			std::cout << setw(sp) << i << empty
								<< setw(sp) << index(i) << empty
								<< setw(sp) << sum(i) << empty 
								<< setw(sp) << width(i) << empty
								<< setw(sp) << peak(i) << empty
								<< setw(sp) << meanval(i) << std::endl;
		}
		
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
	  std::cout << "    SampleFreq: " << newtbbctlIn.headerRecord().asuInt("SampleFreq") << std::endl;
	  std::cout << "    StartSample: " << newtbbctlIn.headerRecord().asuInt("StartSample") << std::endl;
    newtbbctlIn.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  int nofFailedTests (0);
	
	if (argc <= 1) // argc == 1 when no arguments are given...
	{
		filenames(0) = "/Users/acorstanje/testing15apr/rw_20090415_102613_0124.dat"; // hardcoded file can be circumvented by giving parameter
	} else
	{
		filenames(0).assign(argv[1]);
	}
	// test for existence
	fstream ftest( (filenames(0)).c_str());
	if(!ftest.is_open()) {
		cerr << "Unable to open "<< filenames(0) << endl;
		cerr << "Give a correct filename as parameter" << endl;
		return 1;
	}
	ftest.close();
	
  // Test for the constructor(s)
  {
    nofFailedTests += test_tbbctlIn ();
  }

  return nofFailedTests;
}
