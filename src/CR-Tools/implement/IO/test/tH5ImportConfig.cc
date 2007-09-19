/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <IO/H5ImportConfig.h>
#include <fstream>

using CR::H5ImportConfig;  // Namespace usage

/*!
  \file tH5ImportConfig.cc

  \ingroup IO

  \brief A collection of test routines for the H5ImportConfig class
 
  \author Lars Baehren
 
  \date 2007/09/18
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new H5ImportConfig object

  \return nofFailedTests -- The number of failed tests.
*/
int test_H5ImportConfig ()
{
  std::cout << "\n[test_H5ImportConfig]\n" << std::endl;

  int nofFailedTests (0);
  
  int rank (3);
  int *dimensions;
  std::string configFile ("h5import.txt");

  dimensions = new int [rank];
  for (int n(0); n<rank; n++) {
    dimensions[n] = (n+1)*10;
  }
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    H5ImportConfig newH5ImportConfig;
    //
    newH5ImportConfig.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simple argumented constructor ..." << std::endl;
  try {
    H5ImportConfig newH5ImportConfig (rank,dimensions);
    //
    newH5ImportConfig.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    H5ImportConfig newH5ImportConfig (configFile,
				      rank,
				      dimensions);
    //
    newH5ImportConfig.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test how to export data stored in a array of values
  
  \code
  h5import h5import.data -c h5import.txt -o testdata.hdf5
  \endcode

  \return nofFailedTests -- The number of failed tests.
*/
int test_exportArray ()
{
  std::cout << "\n[test_exportArray]\n" << std::endl;

  int nofFailedTests (0);

  int rank (3);
  int *dimensions;
  std::string configFile ("h5import.txt");
  std::string dataFile ("h5import.data");

  dimensions = new int [rank];
  dimensions[0] = 20;
  dimensions[1] = 20;
  dimensions[2] = 10;
  
  try {
    std::cout << " --> creating new configuration ..." << std::endl;
    H5ImportConfig h5config (configFile,
			     rank,
			     dimensions);
    h5config.summary();
    
    std::cout << " --> writing configuration file ..." << std::endl;
    h5config.exportSettings();

    std::cout << " --> creating data array ..." << std::endl;
    float *pixels;
    int nofPixels = h5config.nofDatapoints();
    pixels = new float [nofPixels];
 
    std::cout << " --> filling data array ..." << std::endl;
    int nx (0);
    int ny (0);
    int nz (0);
    int n(0);
    for (nz=0; nz<dimensions[2]; nz++) {
      for (ny=0; ny<dimensions[1]; ny++) {
	for (nx=0; nx<dimensions[0]; nx++) {
	  pixels[n] = nx+100*ny+10000*nz;
	  n++;
	}
      }
    }
    
    std::cout << " --> writing data array to disk ..." << std::endl;
    std::ofstream outfile;
    // open the output stream
    outfile.open(dataFile.c_str(),std::ios::out|std::ios::binary);
    // write out the data to the file
    outfile.write(reinterpret_cast<char *>(pixels),nofPixels*sizeof(float));
    // close the output stream
    outfile.close();
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
  nofFailedTests += test_H5ImportConfig ();
  // Test exporting a simple arrayf of values
  nofFailedTests += test_exportArray();

  return nofFailedTests;
}
