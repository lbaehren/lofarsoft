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

#include <Calibration/TVCalibrationPlugin.h>
#include <Analysis/FirstStagePipeline.h>
#include <Data/LopesEventIn.h>

/*!
  \file tTVCalibrationPlugin.cc

  \ingroup Calibration

  \brief A collection of test routines for TVCalibrationPlugin
 
  \author Andreas Horneffer
 
  \date 2007/04/05
*/

// -----------------------------------------------------------------------------

using namespace CR;

FirstStagePipeline *Pipeline;
LopesEventIn *lev;
CalTableReader *CTRead;

//Read in an event and initialize a pipeline
int initevent(){
  int nofFailedTests=0;
  try {
    
    lev = new LopesEventIn;
    Pipeline = new FirstStagePipeline;
    
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    
    Pipeline->SetObsRecord(obsrec);
    
    if (! lev->attachFile("example.event") ){
      std::cout << "Failed to attach file: example.event" << endl;
      std::cout << "  (Maybe no file called \"example.event\" in the local directory. Copy one here.)" 
		<< std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    
    if (! Pipeline->InitEvent(lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    CTRead = Pipeline->GetCalTableReader();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }; 
  return nofFailedTests;
}

/*!
  \brief Test constructors for a new TVCalibrationPlugin object
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_TVCalibrationPlugin ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_TVCalibrationPlugin]\n" << std::endl;
  
  try {
    std::cout << "[0] Initializing event ..." << std::endl;
    nofFailedTests += initevent();
    
    std::cout << "[1] Testing default constructor ..." << std::endl;
    TVCalibrationPlugin pcal;
    
    // Initialize the TVCalibrationPlugin-Object:
    // Temporary variables:
    Vector<Int> AntennaIDs;
    Vector<Double> tmpvec;
    Matrix<Double> tmpmat;
    Int nFreqranges,i,numAntennas;
    uInt date;

    std::cout << "                                ... setting parameters" << std::endl;
    //Get data from the DataReader-header
    lev->header().get("Date",date);
    lev->header().get("AntennaIDs",AntennaIDs);

    //Use the header data to get the needed data from the CalTableReader:
    //Set frequencyValues
    tmpvec= lev->frequencyValues();
    pcal.parameters().define("frequencyValues",tmpvec);
    //Set sampleRate
    pcal.parameters().define("sampleRate",lev->sampleFrequency());
    //Set sampleJumps
    CTRead->GetData(date, AntennaIDs(0), "SampleJumps", &tmpvec);
    pcal.parameters().define("sampleJumps",tmpvec);
    //Set frequencyRanges
    CTRead->GetData(date, AntennaIDs(0), "PhaseRefFreqs", &tmpmat);
    if (tmpmat.ncolumn()!=2) {
      cerr << "PhaseRefFreqs: tmpmat.ncolumn()!=2 !!!" << endl;
    };
    pcal.parameters().define("frequencyRanges",tmpmat);
    //Set referencePhases
    nFreqranges = tmpmat.nrow();
    numAntennas = AntennaIDs.nelements();
    tmpmat.resize(nFreqranges,numAntennas);
    for (i=0; i<numAntennas; i++) {
      CTRead->GetData(date, AntennaIDs(i), "PhaseRefPhases", &tmpvec);
      tmpmat.column(i) = tmpvec;
    };
    pcal.parameters().define("referencePhases",tmpmat);
    //Now all parameters are set.

    //"Do all the magic" i.e. calculate the wheights
    // Note that we are using lev-fft() and not lev->calfft()!
    std::cout << "                                ... calculating weights" << std::endl;
    pcal.calcWeights(lev->fft());
    

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
  nofFailedTests += test_TVCalibrationPlugin ();
  
  return nofFailedTests;
}
