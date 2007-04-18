/***************************************************************************
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

/* $Id: tFirstStagePipeline.cc,v 1.4 2007/03/06 12:54:13 horneff Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <templates.h>
#include <Analysis/FirstStagePipeline.h>
#include <Data/LopesEventIn.h>


/*!
  \file tFirstStagePipeline.cc

  \ingroup Analysis

  \brief A collection of test routines for FirstStagePipeline
 
  \author Andreas Horneffer
 
  \date 2007/01/11
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new FirstStagePipeline object

  \return nofFailedTests -- The number of failed tests.
*/

// We need that, otherwise we'd have to write "LOPES::LopesEventIn" instead of "LopesEventIn"
using namespace CR;

int test_FirstStagePipeline ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_FirstStagePipeline]\n" << std::endl;

  try {
    // First we need a DataReader object. In our case we take "LopesEventIn" 
    // which is derived from the DataReader
    LopesEventIn lev;

    std::cout << "[1] Testing default constructor ..." << std::endl;
    // Need the pipeline object itself
    FirstStagePipeline newObject;    
    
    std::cout << "[2] Attaching observatories Record ..." << std::endl;
    // Generate a record that points to the CalTable files 
    // Hardcoded path is not nice, but works...
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");

    // Put the record into the pipeline object.
    newObject.SetObsRecord(obsrec);

    std::cout << "[3] Opening Event ..." << std::endl;
    // Open a new event.
    if (! lev.attachFile("example.event") ){
      std::cout << "Failed to attach file: example.event" << endl;
      std::cout << "  (Maybe no file called \"example.event\" in the local directory. Copy one here.)" 
		<< std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[4] Initializing the DataReader ..." << std::endl;
    // Put the calibration data into the DataReader object.
    // This function does (nearly) all the magic.
    if (! newObject.InitEvent(&lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    // Now the DataReader object (in our case "lev") is filled with all the neccesary 
    // calibration information. So we can read out calibrated FFT values.

    std::cout << "[5] Dumping CalFFT of all antennas ..." << std::endl;
    Vector<Double> Frequencies;
    Matrix<DComplex> FFT;
    Matrix<Double> absFFT,phaseFFT;
    Int i,j,fftlen,nants;

    FILE *allout;

    allout = fopen("tFirstStagePipe_allants_calfft.tab","w");
    Frequencies = lev.frequencyValues();
    FFT = lev.calfft();
    absFFT = amplitude(FFT);
    phaseFFT = phase(FFT);
    fftlen = absFFT.nrow();
    nants = absFFT.ncolumn();
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j)-phaseFFT(i,0));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);

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
    nofFailedTests += test_FirstStagePipeline ();
  }

  return nofFailedTests;
}
