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

/* $Id$*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Analysis/SecondStagePipeline.h>
#include <IO/LopesEventIn.h>

using CR::LopesEventIn;
using CR::SecondStagePipeline;

/*!
  \file tSecondStagePipeline.cc

  \ingroup Analysis

  \brief A collection of test routines for SecondStagePipeline
 
  \author Andreas Horneffer
 
  \date 2007/02/22
*/

/*!
  \brief Test constructors for a new SecondStagePipeline object

  \return nofFailedTests -- The number of failed tests.
*/
int test_SecondStagePipeline ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_SecondStagePipeline]\n" << std::endl;

  try {

    // First we need a DataReader object. In our case we take "LopesEventIn" 
    // which is derived from the DataReader
    LopesEventIn lev;

    std::cout << "[1] Testing default constructor ..." << std::endl;
    // Need the pipeline object itself
    SecondStagePipeline newObject;    
    
    std::cout << "[2] Attaching observatories Record ..." << std::endl;
    // Generate a record that points to the CalTable files 
    // Hardcoded path is not nice, but works...
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);

    // Put the record into the pipeline object.
    newObject.SetObsRecord(obsrec);

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
    if (! newObject.InitEvent(&lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[5] Arranging storage, ..." << std::endl;
    Vector<Double> Frequencies;
    Matrix<DComplex> FFT;
    Matrix<Double> absFFT,phaseFFT;
    Int i,j,fftlen,nants;

    FILE *allout;

    allout = fopen("tSecondStagePipe_allants_calfft.tab","w");
    Frequencies = lev.frequencyValues();

    std::cout << "                      ... retrieving the data from the pipeline," << std::endl;
    FFT = newObject.GetData(&lev);
    absFFT = amplitude(FFT);
    phaseFFT = phase(FFT);
    fftlen = absFFT.nrow();
    nants = absFFT.ncolumn();
    std::cout << "                      ... and dumping the filtered data to file." << std::endl;
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
    nofFailedTests += test_SecondStagePipeline ();
  }

  return nofFailedTests;
}
