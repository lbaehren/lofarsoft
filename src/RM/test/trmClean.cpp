/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
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

/*!
  \file trmClean.cpp
  \ingroup RM
  \brief Test program for the RM Clean algorithm

  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  \date 29-07-2009 (Last change: 11-08-2009)
*/

#include <iostream>
#include "rmclean.h"

int main(int argc, char **argv)
{
  int nofFailedTests (0);
  bool haveRMSF (true);
  std::string filename ("rmsf.dat");

  //________________________________________________________
  // Process parameters from the command line
  
  if (argc < 2) {
    haveRMSF = false;
  } else {
    filename = argv[1];
    haveRMSF = true;
  }

  //________________________________________________________
  // Run the tests

  unsigned int length (100);
  vector<double> gaussian (length);
  vector<double> RMSFfaradayDepths (length);
  vector<double> faradayDepths (length);
  vector<complex<double> > rmsftest (length);
  vector<complex<double> > data (length);
  vector<complex<double> > shiftedRMSF (2*length);
  vector<complex<double> > cleanedMap (length);

  // Create rmclean object to work with __________

  std::cout << "-- create new rmclean object ..." << std::endl;
  rmclean Cleanobject (length);
  
  if (haveRMSF) {
    
    // Test algorithm functions
    for(int i=0; i < static_cast<int>(length); i++) {
      faradayDepths[i]=i-50;
    }

    std::cout << "-- Loading RMSR from file " << filename << " ..."  << std::endl;
    Cleanobject.readRMSFfromFile (RMSFfaradayDepths, rmsftest, filename);

    std::cout << "-- Writing RMSF into the object ..." << std::endl;
    Cleanobject.RMSF=rmsftest;

    /*
      for(unsigned int i=0; i<80; i++)
      {
      std::stringstream out;
      std::string s;
      // create filename
      out << i;
      s = out.str();
      
      string filename=string("gaussian") + s + string(".dat");
      
      //cout << "filename = " << filename << endl;
      // Write shifted RMSF with Faraday depths to a text file
      // Create a Gaussian
      }
      
      // Test creation of Gaussian
      Cleanobject.createGaussian(gaussian, 50, 10, 30);		
      Cleanobject.writeRMtoFile(gaussian, "gaussian.dat");	// debug output		
    */
    
    // Test rmsfClean
    Cleanobject.rmsfClean(data, cleanedMap, 0.4, 0);
    
  }
  else {
    std::cerr << "[trmClean] Skipping tests - missing input file!" << std::endl;
  }

  return nofFailedTests;
  }
  
