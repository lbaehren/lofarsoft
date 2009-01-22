/***************************************************************************
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

/* $Id$*/

#include <Analysis/genCRmap.h>
#include <Analysis/SecondStagePipeline.h>
#include <Data/LopesEventIn.h>

using CR::genCRmap;
using CR::LopesEventIn;
using CR::SecondStagePipeline;

/*!
  \file tgenCRmap.cc

  \ingroup Analysis

  \brief A collection of test routines for genCRmap
 
  \author Andreas Horneffer
 
  \date 2007/04/17
*/

/*!
  \brief Test constructors for a new genCRmap object

  \return nofFailedTests -- The number of failed tests.
*/
int test_genCRmap ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_genCRmap]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {

    // First we need a DataReader object. In our case we take "LopesEventIn" 
    // which is derived from the DataReader
    LopesEventIn lev;

    std::cout << "[1] Testing default constructor ..." << std::endl;
    // Need the pipeline object itself
    SecondStagePipeline SSPipe;    
    
    std::cout << "[2] Attaching observatories Record ..." << std::endl;
    // Generate a record that points to the CalTable files 
    // Hardcoded path is not nice, but works...
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");

    // Put the record into the pipeline object.
    SSPipe.SetObsRecord(obsrec);

    std::cout << "[3] Opening Event ..." << std::endl;
    if (! lev.attachFile("example.event") ){
      std::cout << "Failed to attach file: example.event" << endl;
      std::cout << "  (Maybe no file called \"example.event\" in the local directory. Copy one here.)" 
		<< std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[4] Initializing the DataReader ..." << std::endl;
    // Open a new event/datafile/whatever.
    // This function does (nearly) all the magic.
    if (! SSPipe.InitEvent(&lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    // Get Data out of the pipeline
    Matrix<DComplex> FFT;
    FFT = SSPipe.GetData(&lev);
    
    // Set up the CRmap object
    genCRmap newObject;
    newObject.setMapHeader(lev,SSPipe.GetCalTableReader());

    newObject.GenerateMap("example-map",FFT);


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
  nofFailedTests += test_genCRmap ();
  
  return nofFailedTests;
}
